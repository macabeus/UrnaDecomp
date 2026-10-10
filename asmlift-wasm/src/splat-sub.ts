// Substitution: a target function is claimed when the body our whole-program link produces, re-pointed into the
// target's index and address spaces (splat-remap.ts), is byte-identical to the target's piece. The build assembles
// the split with OUR re-pointed bytes for every claimed function and the original piece for the rest, and refuses
// to produce a module while a claim no longer verifies.
import { readFileSync } from 'node:fs';
import { join } from 'node:path';

import { disassemble, instrText } from './disasm';
import type { LedgerRow } from './ledger';
import { GLOBAL_BASE } from './normalize';
import type { Func, Module } from './parse';
import { type AssembleResult, type SplatManifest, assembleSplit, readManifest, sha256 } from './splat';
import { type Piece, type RemapResult, matchesTarget, remapBody } from './splat-remap';
import {
  type DataObject,
  type SymbolMaps,
  compareObject,
  dataObjectAt,
  resolveDataAddress,
  resolveFunction,
  soleFnVoters,
  soleVoters,
} from './splat-symbols';

export type Verdict = 'match' | 'differs' | 'unresolved' | 'no-candidate';

export interface Candidate {
  ours: number;
  /** our function's unique symbol (OurFunction.key) */
  symbol: string;
  /** symbol file, ledger pairing, or the one unpaired target body ours re-points into (contentPairs) */
  source: 'symbol' | 'ledger' | 'content';
}

/** The reading the target body chose for one ambiguous operand, and where the operand sits in the body. */
export interface Chosen {
  reading: string;
  /** the operand's first reading: the one re-pointing alone would write */
  preferred: boolean;
  at: number;
  /** the offset of the operand's instruction in our body */
  oursAt: number;
}

export type Match = { verdict: 'match'; candidate: Candidate; bytes: Uint8Array; readings: string[]; chosen: Chosen[] };

export type Attempt = Match | { verdict: 'differs' | 'unresolved'; candidate: Candidate; detail: string; at: number };

export interface ScanRow {
  index: number;
  /** the matching (else the reported) candidate's symbol; the target's own symbol when nothing was tried */
  symbol: string | null;
  ours: number | null;
  source: Candidate['source'] | null;
  verdict: Verdict;
  detail: string;
  size: number;
  /** the ledger's best match kind for this target function ('none' when it has none) */
  ledgerKind: string;
  group: string;
  tried: number;
  /** a match's chosen readings */
  chosen?: Chosen[];
}

/** target function index → the symbols the symbol file gives it */
export function namesByTarget(maps: SymbolMaps): Map<number, string[]> {
  const out = new Map<number, string[]>();
  for (const [name, index] of maps.target.symbols.fns) {
    out.set(index, [...(out.get(index) ?? []), name]);
  }
  return out;
}

/** Our functions that may produce target function `index`: those the symbol file names it by, then the ledger's
 *  skeleton pairing. */
export function candidatesFor(
  maps: SymbolMaps,
  names: ReadonlyMap<number, readonly string[]>,
  index: number,
  ledger?: LedgerRow,
): Candidate[] {
  const out: Candidate[] = [];
  const firstOurs = maps.ours.module.importedFuncCount;
  const add = (ours: number | undefined, source: Candidate['source']): void => {
    const f = ours === undefined || ours < firstOurs ? undefined : maps.ours.fns.get(ours);
    if (f !== undefined && !out.some((c) => c.ours === f.index)) {
      out.push({ ours: f.index, symbol: f.key, source });
    }
  };
  for (const n of names.get(index) ?? []) {
    add(maps.ours.fnByKey.get(n), 'symbol');
  }
  if (ledger?.best?.label === 'skeleton') {
    add(ledger.best.index, 'ledger');
  }
  return out;
}

function hex(n: number): string {
  return `+0x${n.toString(16)}`;
}

function instrAt(fn: Func, m: Module, at: number): string {
  if (at < fn.codeStart) {
    return 'the locals';
  }
  const i = disassemble(fn).find((x) => x.offset <= at && at < x.end);
  return i === undefined ? '(past the end)' : `${hex(i.offset)} ${instrText(i, m)}`;
}

/** The bytes of `pieces` with each ambiguous operand at the reading matchesTarget chose. */
export function realize(pieces: readonly Piece[], readings: readonly string[]): Uint8Array {
  const parts: Uint8Array[] = [];
  let r = 0;
  for (const p of pieces) {
    if (p instanceof Uint8Array) {
      parts.push(p);
      continue;
    }
    const k = p.readings.indexOf(readings[r++]);
    if (k < 0) {
      throw new Error(`reading '${readings[r - 1]}' is not one of this operand's (${p.readings.join(', ')})`);
    }
    parts.push(p.alternatives[k]);
  }
  if (r !== readings.length) {
    throw new Error(`${readings.length} readings for ${r} ambiguous operands`);
  }
  const out = new Uint8Array(parts.reduce((n, p) => n + p.length, 0));
  let at = 0;
  for (const p of parts) {
    out.set(p, at);
    at += p.length;
  }
  return out;
}

/** The match the readings matchesTarget chose make of `pieces`. */
function matchOf(candidate: Candidate, pieces: readonly Piece[], readings: readonly string[]): Match {
  const chosen: Chosen[] = [];
  let at = 0;
  let r = 0;
  for (const p of pieces) {
    if (p instanceof Uint8Array) {
      at += p.length;
      continue;
    }
    const reading = readings[r++];
    chosen.push({ reading, preferred: p.readings[0] === reading, at, oursAt: p.at });
    at += p.alternatives[p.readings.indexOf(reading)].length;
  }
  return { verdict: 'match', candidate, bytes: realize(pieces, readings), readings: [...readings], chosen };
}

/** our function index → its body re-pointed through one set of maps */
export type Remapper = (ours: number) => RemapResult;

/**
 * remapBody through `maps`, reusing `previous`'s results for every function whose referenced functions (callees,
 * ref.func, the functions in the table slots its constants may name) resolve the same way in both maps: only the
 * function names differ between the passes of a scan (withClaims).
 */
export function cachedRemapper(
  maps: SymbolMaps,
  previous?: { maps: SymbolMaps; results: Map<number, RemapResult> },
): { remap: Remapper; maps: SymbolMaps; results: Map<number, RemapResult> } {
  const results = new Map<number, RemapResult>();
  const same = (f: number): boolean => {
    const a = resolveFunction(previous!.maps, f);
    const b = resolveFunction(maps, f);
    return a.ok === b.ok && (!a.ok || a.index === (b.ok ? b.index : undefined));
  };
  const remap: Remapper = (ours) => {
    const hit = results.get(ours);
    if (hit !== undefined) {
      return hit;
    }
    const old = previous?.results.get(ours);
    const r = old !== undefined && referencedFunctions(maps, ours).every(same) ? old : remapBody(maps, ours);
    results.set(ours, r);
    return r;
  };
  return { remap, maps, results };
}

const referenced = new WeakMap<object, Map<number, number[]>>();

/** The functions our function `ours` calls, references or names through a constant the target body reads as a
 *  table slot (`chosen`), with the body offset of each operand. A constant the target keeps as an integer names
 *  nothing, even when it equals one of our table slots. */
function referencedFunctionsAt(maps: SymbolMaps, ours: number, chosen: readonly Chosen[]): { f: number; at: number }[] {
  const m = maps.ours.module;
  const slots = new Set(chosen.filter((c) => c.reading.startsWith('table slot')).map((c) => c.oursAt));
  const out: { f: number; at: number }[] = [];
  for (const i of disassemble(m.funcs[ours - m.importedFuncCount])) {
    for (const im of i.imms) {
      if (im.kind === 'u32' && im.role === 'func') {
        out.push({ f: im.value, at: i.offset });
      } else if (im.kind === 's32' && i.opcode === 0x41 && slots.has(i.offset)) {
        const f = maps.ours.elem.get(im.value);
        if (f !== undefined) {
          out.push({ f, at: i.offset });
        }
      }
    }
  }
  return out;
}

function referencedFunctions(maps: SymbolMaps, ours: number): number[] {
  const m = maps.ours.module;
  let byFn = referenced.get(maps.ours);
  if (byFn === undefined) {
    byFn = new Map();
    referenced.set(maps.ours, byFn);
  }
  let refs = byFn.get(ours);
  if (refs === undefined) {
    refs = [];
    for (const i of disassemble(m.funcs[ours - m.importedFuncCount])) {
      for (const im of i.imms) {
        if (im.kind === 'u32' && im.role === 'func') {
          refs.push(im.value);
        } else if (im.kind === 's32' && i.opcode === 0x41) {
          const f = maps.ours.elem.get(im.value);
          if (f !== undefined) {
            refs.push(f);
          }
        }
      }
    }
    byFn.set(ours, refs);
  }
  return refs;
}

/** Re-point our candidate and compare it with the target body (the piece's bytes) exactly. */
export function attempt(
  maps: SymbolMaps,
  candidate: Candidate,
  targetIndex: number,
  targetBody: Uint8Array,
  remap: Remapper = (o) => remapBody(maps, o),
): Attempt {
  const r = remap(candidate.ours);
  if (!r.ok) {
    const u = r.unresolved[0];
    return { verdict: 'unresolved', candidate, at: u.at, detail: `ours ${hex(u.at)} ${u.instr}: ${u.reason}` };
  }
  const m = matchesTarget(r.pieces, targetBody);
  if (m.match) {
    const match = matchOf(candidate, r.pieces, m.readings ?? []);
    if (Buffer.compare(Buffer.from(match.bytes), Buffer.from(targetBody)) !== 0) {
      throw new Error(`function ${targetIndex}: the readings matchesTarget chose do not rebuild the target body`);
    }
    return match;
  }
  const at = m.firstDifference ?? 0;
  const tm = maps.target.module;
  const tf = tm.funcs[targetIndex - tm.importedFuncCount];
  const of = maps.ours.module.funcs[candidate.ours - maps.ours.module.importedFuncCount];
  const ourRepointed: Func = { ...of, bodyBytes: r.bytes, size: r.bytes.length };
  const detail =
    at >= targetBody.length
      ? `ours is longer: the target body ends at ${hex(at)} (${r.bytes.length} vs ${targetBody.length} B)`
      : `${hex(at)} target ${instrAt({ ...tf, bodyBytes: targetBody }, tm, at)} | ours ${instrAt(ourRepointed, tm, at)}${ourOperand(maps, of, ourRepointed, at)}`;
  return { verdict: 'differs', candidate, at, detail };
}

/** What our original operand at re-pointed offset `at` is, when it is an address: re-pointing keeps the instruction
 *  sequence, so the k-th re-pointed instruction is the k-th original one. */
function ourOperand(maps: SymbolMaps, original: Func, repointed: Func, at: number): string {
  const k = disassemble(repointed).findIndex((x) => x.offset <= at && at < x.end);
  const i = k < 0 ? undefined : disassemble(original)[k];
  const v = i?.imms.find((im) => im.kind === 'memarg' || (im.kind === 's32' && i.opcode === 0x41));
  const value = v === undefined ? undefined : v.kind === 'memarg' ? v.offset : v.kind === 's32' ? v.value : undefined;
  if (value === undefined || value < GLOBAL_BASE || value >= maps.ours.dataEnd) {
    return '';
  }
  const r = resolveDataAddress(maps.ours, maps.ourImage, maps.target, value);
  const obj = dataObjectAt(maps.ours.map.data, value);
  return ` [ours ${value}${obj ? ` = ${obj.key}+${value - obj.addr}` : ''}: ${r.ok ? `address ${r.how} → 0x${r.addr.toString(16)}` : r.reason}]`;
}

const RANK: Record<Attempt['verdict'], number> = { match: 0, differs: 1, unresolved: 2 };

/** Every candidate of target function `index`, the first match winning; else the attempt that got furthest. */
export function scanFunction(
  maps: SymbolMaps,
  names: ReadonlyMap<number, readonly string[]>,
  index: number,
  targetBody: Uint8Array,
  ledger?: LedgerRow,
  remap?: Remapper,
): { candidates: Candidate[]; best: Attempt | null } {
  const candidates = candidatesFor(maps, names, index, ledger);
  let best: Attempt | null = null;
  for (const c of candidates) {
    const a = attempt(maps, c, index, targetBody, remap);
    if (a.verdict === 'match') {
      return { candidates, best: a };
    }
    if (
      best === null ||
      RANK[a.verdict] < RANK[best.verdict] ||
      (a.verdict === best.verdict && a.at > (best as { at: number }).at)
    ) {
      best = a;
    }
  }
  return { candidates, best };
}

/** A split's function bodies by index, each checked against the manifest's size and sha256. */
export function readPieceBodies(dir: string, manifest: SplatManifest = readManifest(dir)): Map<number, Uint8Array> {
  const out = new Map<number, Uint8Array>();
  for (const f of manifest.functions) {
    const b = new Uint8Array(readFileSync(join(dir, f.path)));
    if (b.length !== f.size || sha256(b) !== f.sha256) {
      throw new Error(`${join(dir, f.path)}: the piece no longer has the bytes the manifest recorded`);
    }
    out.set(f.index, b);
  }
  return out;
}

/** What re-pointing cannot change: the locals and the opcode sequence. */
function shapeKey(fn: Func): string {
  const ops = disassemble(fn).map((i) => i.opcode.toString(36));
  return `${Buffer.from(fn.bodyBytes.subarray(0, fn.codeStart)).toString('hex')}:${ops.join(',')}`;
}

/** Pairs found by body alone among what the candidates left: one of our functions that no match uses, re-pointed,
 *  equal to exactly one unmatched target body that no other such function of ours equals. */
export function contentPairs(
  maps: SymbolMaps,
  rows: readonly ScanRow[],
  bodies: ReadonlyMap<number, Uint8Array>,
  remap: Remapper = (o) => remapBody(maps, o),
): Map<number, Match> {
  const used = new Set(rows.filter((r) => r.verdict === 'match').map((r) => r.ours!));
  const tm = maps.target.module;
  const byShape = new Map<string, number[]>();
  for (const r of rows) {
    if (r.verdict !== 'match') {
      const k = shapeKey({ ...tm.funcs[r.index - tm.importedFuncCount], bodyBytes: bodies.get(r.index)! });
      byShape.set(k, [...(byShape.get(k) ?? []), r.index]);
    }
  }
  const found = new Map<number, Match[]>();
  const oursHits = new Map<number, number>();
  for (const f of maps.ours.module.funcs) {
    const key = maps.ours.fns.get(f.index)?.key;
    const targets = byShape.get(shapeKey(f));
    if (used.has(f.index) || key === undefined || targets === undefined) {
      continue;
    }
    const r = remap(f.index);
    if (!r.ok) {
      continue;
    }
    for (const t of targets) {
      const m = matchesTarget(r.pieces, bodies.get(t)!);
      if (m.match) {
        const candidate: Candidate = { ours: f.index, symbol: key, source: 'content' };
        found.set(t, [...(found.get(t) ?? []), matchOf(candidate, r.pieces, m.readings ?? [])]);
        oursHits.set(f.index, (oursHits.get(f.index) ?? 0) + 1);
      }
    }
  }
  const out = new Map<number, Match>();
  for (const [t, hits] of found) {
    if (hits.length === 1 && oursHits.get(hits[0].candidate.ours) === 1) {
      out.set(t, hits[0]);
    }
  }
  return out;
}

const SYMBOL_READING = /^address symbol (\S+)$/;

export interface SelfVoted {
  /** body offset of the operand */
  at: number;
  symbol: string;
  /** what our object's bytes said against the target's there; null for a function name, which has no bytes to check */
  bytes: string | null;
}

const ourDataByKey = new WeakMap<object, Map<string, DataObject>>();

/**
 * A data name whose only evidence is a vote cast by target function T came from T's own operand, so it cannot
 * confirm that operand: a match of T that re-points an address through such a name holds only when our object's
 * bytes agree with the target's there (pointers re-pointed, splat-symbols compareObject), or another match that
 * is not refused itself re-points through the same name. Returns every refused match with the operand.
 */
export function selfVoted(
  maps: SymbolMaps,
  matches: ReadonlyMap<number, { chosen: readonly Chosen[]; ours?: number }>,
): Map<number, SelfVoted> {
  const { symbols } = maps.target;
  let byKey = ourDataByKey.get(maps.ours);
  if (byKey === undefined) {
    byKey = new Map(maps.ours.map.data.map((d) => [d.key, d]));
    ourDataByKey.set(maps.ours, byKey);
  }
  const users = new Map<string, Set<number>>();
  const suspects = new Map<number, SelfVoted[]>();
  const verdicts = new Map<string, string>();
  for (const [t, m] of matches) {
    for (const c of m.chosen) {
      const symbol = SYMBOL_READING.exec(c.reading)?.[1];
      if (symbol === undefined) {
        continue;
      }
      users.set(symbol, (users.get(symbol) ?? new Set()).add(t));
      const voters = soleVoters(symbols, symbol);
      if (voters === null || voters.some((v) => v !== t)) {
        continue;
      }
      let bytes = verdicts.get(symbol);
      if (bytes === undefined) {
        const obj = byKey.get(symbol);
        const addr = symbols.data.get(symbol);
        const check = obj === undefined || addr === undefined ? null : compareObject(maps, obj, addr);
        bytes =
          check === null
            ? 'our object is not in the map'
            : check.verdict === 'differs'
              ? `differ at +${check.at} (${check.why})`
              : check.verdict;
        verdicts.set(symbol, bytes);
      }
      if (bytes !== 'agrees') {
        suspects.set(t, [...(suspects.get(t) ?? []), { at: c.at, symbol, bytes }]);
      }
    }
    // a function name voted only by this match's own call or table operands: nothing but another match confirms it
    if (m.ours === undefined) {
      continue;
    }
    for (const { f, at } of referencedFunctionsAt(maps, m.ours, m.chosen)) {
      const symbol = maps.ours.fns.get(f)?.key;
      const index = symbol === undefined ? undefined : symbols.fns.get(symbol);
      if (symbol === undefined || index === undefined || matches.has(index)) {
        continue;
      }
      users.set(symbol, (users.get(symbol) ?? new Set()).add(t));
      const voters = soleFnVoters(symbols, symbol);
      if (voters !== null && voters.every((v) => v === t)) {
        suspects.set(t, [...(suspects.get(t) ?? []), { at, symbol, bytes: null }]);
      }
    }
  }
  const refused = new Map<number, SelfVoted>();
  for (let changed = true; changed;) {
    changed = false;
    for (const [t, list] of suspects) {
      const alone = refused.has(t)
        ? undefined
        : list.find((x) => ![...(users.get(x.symbol) ?? [])].some((u) => u !== t && !refused.has(u)));
      if (alone !== undefined) {
        refused.set(t, alone);
        changed = true;
      }
    }
  }
  return refused;
}

function selfVotedText(maps: SymbolMaps, index: number, body: Uint8Array, x: SelfVoted): string {
  const tm = maps.target.module;
  const tf = tm.funcs[index - tm.importedFuncCount];
  if (x.bytes === null) {
    const e = maps.target.symbols.fnEvidence.get(x.symbol);
    return (
      `self-voted ours +${x.at.toString(16)}: ${x.symbol} = func:${maps.target.symbols.fns.get(x.symbol)} ` +
      `(evidence:${e?.evidence} by:${e?.voters?.join(',')}) comes from this function's own operand ` +
      'and no other match re-points through it'
    );
  }
  const e = maps.target.symbols.dataEvidence.get(x.symbol);
  return (
    `self-voted ${instrAt({ ...tf, bodyBytes: body }, tm, x.at)}: ${x.symbol} = 0x${maps.target.symbols.data.get(x.symbol)!.toString(16)} ` +
    `(evidence:${e?.evidence} by:${e?.voters?.join(',')}) comes from this function's own operand; ` +
    `our object's bytes ${x.bytes === 'unknown' ? 'say nothing (zeros, or pointers that do not resolve)' : x.bytes} ` +
    'and no other match re-points through it'
  );
}

function readingsText(readings: readonly string[]): string {
  return readings.length > 0
    ? `readings: ${[...new Set(readings.map((x) => x.split(' ').slice(0, 2).join(' ')))].join(',')}`
    : '';
}

/**
 * A claim is the strongest evidence of which target function our symbol is: the maps with every claimed symbol
 * naming its target function, and no other symbol naming a claimed function.
 */
export function withClaims(maps: SymbolMaps, claims: Iterable<{ index: number; symbol: string }>): SymbolMaps {
  const claimed = new Map<number, string>();
  for (const c of claims) {
    claimed.set(c.index, c.symbol);
  }
  const fns = new Map(maps.target.symbols.fns);
  for (const [name, index] of fns) {
    if (claimed.has(index) && claimed.get(index) !== name) {
      fns.delete(name);
    }
  }
  for (const [index, symbol] of claimed) {
    fns.set(symbol, index);
  }
  return { ...maps, target: { ...maps.target, symbols: { ...maps.target.symbols, fns } } };
}

const SCAN_PASSES = 6;

/**
 * Scan every target function of the split: its candidates first, then the pairs found by body alone. Each pass
 * re-points through the maps with the previous pass's matches claimed (a match found by body corrects the name
 * a caller's call resolves through), until the matches stop changing; the last pass's rows therefore hold for a
 * build that claims exactly its matches.
 */
export function scanSplit(
  maps: SymbolMaps,
  manifest: SplatManifest,
  bodies: ReadonlyMap<number, Uint8Array>,
  ledger: ReadonlyMap<number, LedgerRow>,
  claims: readonly { index: number; symbol: string }[] = [],
  onPass?: (pass: number, matches: number) => void,
): ScanRow[] {
  const firstSource = new Map<number, ScanRow>();
  let overlay = claims;
  let previous = '';
  let rows: ScanRow[] = [];
  let cache: ReturnType<typeof cachedRemapper> | undefined;
  for (let pass = 1; pass <= SCAN_PASSES; pass++) {
    const passMaps = withClaims(maps, overlay);
    cache = cachedRemapper(passMaps, cache);
    const found = scanCandidates(passMaps, manifest, bodies, ledger, cache.remap);
    const extra = contentPairs(passMaps, found, bodies, cache.remap);
    rows = found.map((r) => {
      const a = extra.get(r.index);
      if (a === undefined) {
        return r;
      }
      const was = r.verdict === 'no-candidate' ? '' : `; ${r.symbol} ${r.verdict}`;
      return {
        ...r,
        symbol: a.candidate.symbol,
        ours: a.candidate.ours,
        source: 'content',
        verdict: 'match',
        detail: `${readingsText(a.readings)}${was}`.replace(/^; /, ''),
        chosen: a.chosen,
      };
    });
    const refused = selfVoted(
      passMaps,
      new Map(
        rows.filter((r) => r.verdict === 'match').map((r) => [r.index, { chosen: r.chosen ?? [], ours: r.ours! }]),
      ),
    );
    rows = rows.map((r) => {
      const x = refused.get(r.index);
      return x === undefined
        ? r
        : {
            ...r,
            verdict: 'unresolved',
            detail: selfVotedText(passMaps, r.index, bodies.get(r.index)!, x),
            chosen: undefined,
          };
    });
    // a later pass reaches through the claimed name what an earlier one found: keep how it was first found
    rows = rows.map((r) => {
      const first = firstSource.get(r.index);
      if (r.verdict !== 'match') {
        return r;
      }
      if (first === undefined || first.symbol !== r.symbol) {
        firstSource.set(r.index, r);
        return r;
      }
      return { ...r, source: first.source, detail: first.detail };
    });
    overlay = rows.filter((r) => r.verdict === 'match').map((r) => ({ index: r.index, symbol: r.symbol! }));
    onPass?.(pass, overlay.length);
    const key = overlay.map((c) => `${c.index} ${c.symbol}`).join('\n');
    if (key === previous) {
      break;
    }
    previous = key;
  }
  return rows;
}

function scanCandidates(
  maps: SymbolMaps,
  manifest: SplatManifest,
  bodies: ReadonlyMap<number, Uint8Array>,
  ledger: ReadonlyMap<number, LedgerRow>,
  remap: Remapper,
): ScanRow[] {
  const names = namesByTarget(maps);
  return manifest.functions.map((f): ScanRow => {
    const row = ledger.get(f.index);
    const { candidates, best } = scanFunction(maps, names, f.index, bodies.get(f.index)!, row, remap);
    const base = {
      index: f.index,
      size: f.size,
      ledgerKind: row?.best?.kind ?? 'none',
      group: f.group,
      tried: candidates.length,
    };
    if (best === null) {
      return {
        ...base,
        symbol: names.get(f.index)?.[0] ?? null,
        ours: null,
        source: null,
        verdict: 'no-candidate',
        detail: names.has(f.index) ? 'the symbol is not one of our functions' : 'no symbol and no ledger pair',
      };
    }
    return {
      ...base,
      symbol: best.candidate.symbol,
      ours: best.candidate.ours,
      source: best.candidate.source,
      verdict: best.verdict,
      detail: best.verdict === 'match' ? readingsText(best.readings) : best.detail,
      ...(best.verdict === 'match' ? { chosen: best.chosen } : {}),
    };
  });
}

export const SCAN_HEADER = [
  'index',
  'symbol',
  'ours',
  'source',
  'verdict',
  'detail',
  'size',
  'ledger_kind',
  'group',
  'tried',
  'object',
];

/** The emscripten sysroot's library directory, as the link map spells an archive member from it. */
const SYSROOT_LIB = '/emsdk/upstream/emscripten/cache/sysroot/lib/wasm32-emscripten/';

/**
 * One scan row per target function. `object` is the input that defines our candidate (`ours`), as the link map
 * names it relative to build/out (`openssl/O3/…/x.o`, `app/ecourna/….o`), with emscripten's own libraries as
 * `sysroot:libc.a(…)`, and `-` when there is no candidate.
 */
export function renderScan(
  rows: readonly ScanRow[],
  objectOf: (ours: number) => string | undefined = () => undefined,
): string {
  const clean = (s: string): string => s.replace(/[\t\n]/g, ' ');
  const object = (ours: number | null): string => {
    const o = ours === null ? undefined : objectOf(ours);
    if (o === undefined || o === '') {
      return '-';
    }
    return clean(o.startsWith(SYSROOT_LIB) ? `sysroot:${o.slice(SYSROOT_LIB.length)}` : o);
  };
  return `${[
    SCAN_HEADER.join('\t'),
    ...rows.map((r) =>
      [
        r.index,
        r.symbol ?? '-',
        r.ours ?? '-',
        r.source ?? '-',
        r.verdict,
        clean(r.detail) || '-',
        r.size,
        r.ledgerKind,
        r.group,
        r.tried,
        object(r.ours),
      ].join('\t'),
    ),
  ].join('\n')}\n`;
}

// ---------------------------------------------------------------------------------------------------------------
// the claim list: `<target index> <our symbol>` per line, `#` starts a comment

export interface Claim {
  index: number;
  symbol: string;
  line: number;
}

export function parseClaims(text: string): { claims: Claim[]; lines: string[] } {
  const lines = text.split('\n');
  if (lines.at(-1) === '') {
    lines.pop();
  }
  const claims: Claim[] = [];
  const seen = new Map<number, number>();
  lines.forEach((raw, k) => {
    const l = raw.replace(/#.*/, '').trim();
    if (l === '') {
      return;
    }
    const m = l.match(/^(\d+)\s+(\S+)$/);
    if (m === null) {
      throw new Error(`claims line ${k + 1}: expected '<target index> <symbol>', got '${raw}'`);
    }
    const index = Number(m[1]);
    const prev = seen.get(index);
    if (prev !== undefined) {
      throw new Error(`claims line ${k + 1}: function ${index} is already claimed on line ${prev}`);
    }
    seen.set(index, k + 1);
    claims.push({ index, symbol: m[2], line: k + 1 });
  });
  return { claims, lines };
}

/** The claim list with a line added for every scan match it does not claim yet; existing lines stay as written. */
export function addClaims(text: string, rows: readonly ScanRow[]): { text: string; added: number } {
  const { claims, lines } = parseClaims(text);
  const claimed = new Set(claims.map((c) => c.index));
  const fresh = rows.filter((r) => r.verdict === 'match' && !claimed.has(r.index));
  const out = [...lines, ...fresh.map((r) => `${r.index} ${r.symbol}`)];
  return { text: `${out.join('\n')}\n`, added: fresh.length };
}

// ---------------------------------------------------------------------------------------------------------------
// the build

export interface ClaimFailure {
  claim: Claim;
  message: string;
}

export interface ComponentMeter {
  group: string;
  functions: number;
  claimed: number;
  bytes: number;
  claimedBytes: number;
}

export interface BuildResult {
  assembled: AssembleResult;
  meter: ComponentMeter[];
  readings: ReadingMeter;
  claimed: number;
  claimedBytes: number;
  functions: number;
  bytes: number;
}

/** Re-point every claimed function and compare it with its piece; failures name the function and where it differs. */
export function verifyClaims(
  maps: SymbolMaps,
  manifest: SplatManifest,
  bodies: ReadonlyMap<number, Uint8Array>,
  claims: readonly Claim[],
): { substitutes: Map<number, Uint8Array>; chosen: Map<number, Chosen[]>; failures: ClaimFailure[] } {
  const byIndex = new Map(manifest.functions.map((f) => [f.index, f]));
  const substitutes = new Map<number, Uint8Array>();
  const chosen = new Map<number, Chosen[]>();
  const failures: ClaimFailure[] = [];
  const who = (claim: Claim, ours?: number): string => {
    const f = byIndex.get(claim.index);
    return `function ${claim.index}${f?.name ? ` (${f.name})` : ''} claimed by ${claim.symbol}${ours === undefined ? '' : ` (ours ${ours})`}`;
  };
  for (const claim of claims) {
    const ours = maps.ours.fnByKey.get(claim.symbol);
    if (!byIndex.has(claim.index)) {
      failures.push({ claim, message: `${who(claim)}: ${claim.index} is not a defined function of the target` });
      continue;
    }
    if (ours === undefined || ours < maps.ours.module.importedFuncCount) {
      failures.push({ claim, message: `${who(claim)}: our link has no function ${claim.symbol}` });
      continue;
    }
    const a = attempt(maps, { ours, symbol: claim.symbol, source: 'symbol' }, claim.index, bodies.get(claim.index)!);
    if (a.verdict === 'match') {
      substitutes.set(claim.index, a.bytes);
      chosen.set(claim.index, a.chosen);
    } else {
      failures.push({
        claim,
        message: `${who(claim, ours)} ${a.verdict === 'differs' ? 'DIFFERS at' : 'is UNRESOLVED:'} ${a.detail}`,
      });
    }
  }
  const oursOf = new Map(claims.map((c) => [c.index, maps.ours.fnByKey.get(c.symbol)]));
  const refused = selfVoted(maps, new Map([...chosen].map(([t, c]) => [t, { chosen: c, ours: oursOf.get(t) }])));
  for (const claim of claims) {
    const x = refused.get(claim.index);
    if (x !== undefined) {
      substitutes.delete(claim.index);
      chosen.delete(claim.index);
      failures.push({
        claim,
        message: `${who(claim, maps.ours.fnByKey.get(claim.symbol))} is UNCONFIRMED: ${selfVotedText(maps, claim.index, bodies.get(claim.index)!, x)}`,
      });
    }
  }
  failures.sort((a, b) => a.claim.line - b.claim.line);
  return { substitutes, chosen, failures };
}

/** How the target's bytes chose among our readings: per claim, the strongest choice it needed. */
export interface ReadingMeter {
  /** claims whose every ambiguous operand took its first reading */
  preferred: { claims: number; bytes: number };
  /** claims where the target picked another reading (literal, table slot or address) of some operand */
  other: { claims: number; bytes: number; operands: number; byKind: Record<string, number> };
  /** claims where the target picked which copy of identical string or read-only bytes an operand names, or which of
   *  the table slots holding one function */
  copy: { claims: number; bytes: number; operands: number };
}

export function readingKind(reading: string): string {
  return reading.startsWith('address same-content') || reading.startsWith('table slot copy')
    ? 'copy'
    : reading.startsWith('address')
      ? 'address'
      : reading.startsWith('table')
        ? 'table slot'
        : 'literal';
}

export function readingMeterOf(manifest: SplatManifest, chosen: ReadonlyMap<number, readonly Chosen[]>): ReadingMeter {
  const size = new Map(manifest.functions.map((f) => [f.index, f.size]));
  const out: ReadingMeter = {
    preferred: { claims: 0, bytes: 0 },
    other: { claims: 0, bytes: 0, operands: 0, byKind: {} },
    copy: { claims: 0, bytes: 0, operands: 0 },
  };
  for (const [t, list] of chosen) {
    const copies = list.filter((c) => readingKind(c.reading) === 'copy');
    const others = list.filter((c) => !c.preferred && readingKind(c.reading) !== 'copy');
    out.copy.operands += copies.length;
    out.other.operands += others.length;
    for (const c of others) {
      const k = readingKind(c.reading);
      out.other.byKind[k] = (out.other.byKind[k] ?? 0) + 1;
    }
    const into = copies.length > 0 ? out.copy : others.length > 0 ? out.other : out.preferred;
    into.claims++;
    into.bytes += size.get(t) ?? 0;
  }
  return out;
}

export function meterOf(manifest: SplatManifest, claimed: ReadonlySet<number>): ComponentMeter[] {
  const by = new Map<string, ComponentMeter>();
  for (const f of manifest.functions) {
    const c = by.get(f.group) ?? { group: f.group, functions: 0, claimed: 0, bytes: 0, claimedBytes: 0 };
    c.functions++;
    c.bytes += f.size;
    if (claimed.has(f.index)) {
      c.claimed++;
      c.claimedBytes += f.size;
    }
    by.set(f.group, c);
  }
  return [...by.values()].sort(
    (a, b) => b.claimedBytes - a.claimedBytes || b.bytes - a.bytes || a.group.localeCompare(b.group),
  );
}

export class ClaimsFailed extends Error {
  constructor(readonly failures: ClaimFailure[]) {
    super(
      `${failures.length} claimed function${failures.length === 1 ? '' : 's'} no longer verif${failures.length === 1 ? 'ies' : 'y'}:\n${failures
        .map((f) => `  line ${f.claim.line}: ${f.message}`)
        .join('\n')}`,
    );
  }
}

/** Assemble the split with our re-pointed bytes for every claim; throws (ClaimsFailed) when any claim does not
 *  verify, and when the module is not `expectSha256`. */
export function buildSplat(maps: SymbolMaps, dir: string, claims: readonly Claim[], expectSha256: string): BuildResult {
  const manifest = readManifest(dir);
  const bodies = readPieceBodies(dir, manifest);
  const { substitutes, chosen, failures } = verifyClaims(withClaims(maps, claims), manifest, bodies, claims);
  if (failures.length > 0) {
    throw new ClaimsFailed(failures);
  }
  const assembled = assembleSplit(dir, substitutes);
  if (assembled.substituted.length > 0) {
    throw new Error(`substitutes that differ from their piece: ${assembled.substituted.join(', ')}`);
  }
  if (assembled.sha256 !== expectSha256) {
    throw new Error(`the assembled module is sha256 ${assembled.sha256}, not ${expectSha256}`);
  }
  const claimedSet = new Set(substitutes.keys());
  const meter = meterOf(manifest, claimedSet);
  const sum = (k: keyof ComponentMeter): number => meter.reduce((n, c) => n + (c[k] as number), 0);
  return {
    assembled,
    meter,
    readings: readingMeterOf(manifest, chosen),
    claimed: sum('claimed'),
    claimedBytes: sum('claimedBytes'),
    functions: sum('functions'),
    bytes: sum('bytes'),
  };
}
