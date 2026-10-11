// Substitution: a target function is matched when the body our whole-program link produces, re-pointed into the
// target's index and address spaces (splat-remap.ts), is byte-identical to the target's piece. The build assembles
// the split with OUR re-pointed bytes for every matched function and the original piece for the rest, and refuses
// to produce a module while any match does not verify.
import { readFileSync } from 'node:fs';
import { join } from 'node:path';

import { disassemble, instrText } from './disasm';
import type { LedgerRow } from './ledger';
import { GLOBAL_BASE } from './normalize';
import type { Func, Module } from './parse';
import { type AssembleResult, type SplatManifest, assembleSplit, readManifest, sha256 } from './splat';
import { operandKind, tableBase } from './splat-kinds';
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

export type Match = {
  verdict: 'match';
  candidate: Candidate;
  bytes: Uint8Array;
  readings: string[];
  chosen: Chosen[];
  /** why our function's operand kinds are not known (RemapResult.kindUnverified): it matched with every reading */
  kindUnverified?: string;
};

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
  /** the ledger's best pair kind for this target function ('none' when it has none) */
  ledgerKind: string;
  group: string;
  tried: number;
  /** a match's chosen readings */
  chosen?: Chosen[];
  /** a match of a kind-unverified function of ours: why */
  kindUnverified?: string;
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
function matchOf(
  candidate: Candidate,
  pieces: readonly Piece[],
  readings: readonly string[],
  kindUnverified?: string,
): Match {
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
  return {
    verdict: 'match',
    candidate,
    bytes: realize(pieces, readings),
    readings: [...readings],
    chosen,
    ...(kindUnverified === undefined ? {} : { kindUnverified }),
  };
}

/** our function index → its body re-pointed through one set of maps */
export type Remapper = (ours: number) => RemapResult;

/**
 * remapBody through `maps`, reusing `previous`'s results for every function whose referenced functions (callees,
 * ref.func, the functions in the table slots its constants may name) resolve the same way in both maps: only the
 * function names differ between the passes of a scan (withMatches).
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

/** Our table slot an i32.const of our function `ours` names, read as a slot: its value, or for a constant relative
 *  to an unfolded __table_base (splat-kinds.ts table-rel) the base plus it. */
function slotOf(maps: SymbolMaps, ours: number, at: number, value: number): number {
  const kind = operandKind(maps.kinds?.functions.get(ours), at);
  return kind === 'table-rel' ? tableBase(maps.ours.module) + value : value;
}

/** The functions our function `ours` calls, references or names through a constant the target body reads as a
 *  table slot (`chosen`), with the body offset of each operand. A constant the target keeps as an integer names
 *  nothing, even when it equals one of our table slots. */
export function referencedFunctionsAt(maps: SymbolMaps, ours: number, chosen: readonly Chosen[]): { f: number; at: number }[] {
  const m = maps.ours.module;
  const slots = new Set(chosen.filter((c) => c.reading.startsWith('table slot')).map((c) => c.oursAt));
  const out: { f: number; at: number }[] = [];
  for (const i of disassemble(m.funcs[ours - m.importedFuncCount])) {
    for (const im of i.imms) {
      if (im.kind === 'u32' && im.role === 'func') {
        out.push({ f: im.value, at: i.offset });
      } else if (im.kind === 's32' && i.opcode === 0x41 && slots.has(i.offset)) {
        const f = maps.ours.elem.get(slotOf(maps, ours, i.offset, im.value));
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
          // every function the constant may name: its own slot, and for a table-rel constant the base plus it
          for (const slot of new Set([im.value, slotOf(maps, ours, i.offset, im.value)])) {
            const f = maps.ours.elem.get(slot);
            if (f !== undefined) {
              refs.push(f);
            }
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
    const match = matchOf(candidate, r.pieces, m.readings ?? [], r.kindUnverified);
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
  const kind = i === undefined ? undefined : operandKind(maps.kinds?.functions.get(original.index), i.offset);
  if (value !== undefined && kind === 'literal' && value >= GLOBAL_BASE) {
    return ` [ours ${value}: an integer (operand kinds)]`;
  }
  if (
    value === undefined ||
    (kind !== undefined && kind !== 'address' && kind !== 'literal') ||
    value < GLOBAL_BASE ||
    value >= maps.ours.dataEnd
  ) {
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
        found.set(t, [...(found.get(t) ?? []), matchOf(candidate, r.pieces, m.readings ?? [], r.kindUnverified)]);
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
 * A match is the strongest evidence of which target function our symbol is: the maps with every matched symbol
 * naming its target function, and no other symbol naming a matched function.
 */
export function withMatches(maps: SymbolMaps, matches: Iterable<{ index: number; symbol: string }>): SymbolMaps {
  const matched = new Map<number, string>();
  for (const m of matches) {
    matched.set(m.index, m.symbol);
  }
  const fns = new Map(maps.target.symbols.fns);
  for (const [name, index] of fns) {
    if (matched.has(index) && matched.get(index) !== name) {
      fns.delete(name);
    }
  }
  for (const [index, symbol] of matched) {
    fns.set(symbol, index);
  }
  return { ...maps, target: { ...maps.target, symbols: { ...maps.target.symbols, fns } } };
}

const SCAN_PASSES = 6;

/**
 * Scan every target function of the split: its candidates first, then the pairs found by body alone. Each pass
 * re-points through the maps with the previous pass's matches listed (withMatches; a match found by body corrects
 * the name a caller's call resolves through), until the matches stop changing; the last pass's rows therefore hold
 * for a build whose match list is exactly its matches.
 */
export function scanSplit(
  maps: SymbolMaps,
  manifest: SplatManifest,
  bodies: ReadonlyMap<number, Uint8Array>,
  ledger: ReadonlyMap<number, LedgerRow>,
  matchList: readonly { index: number; symbol: string }[] = [],
  onPass?: (pass: number, matches: number) => void,
): ScanRow[] {
  const firstSource = new Map<number, ScanRow>();
  let overlay = matchList;
  let previous = '';
  let rows: ScanRow[] = [];
  let cache: ReturnType<typeof cachedRemapper> | undefined;
  for (let pass = 1; pass <= SCAN_PASSES; pass++) {
    const passMaps = withMatches(maps, overlay);
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
        ...(a.kindUnverified === undefined ? {} : { kindUnverified: a.kindUnverified }),
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
            kindUnverified: undefined,
          };
    });
    // a later pass reaches through the matched name what an earlier one found: keep how it was first found
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
      ...(best.verdict === 'match' && best.kindUnverified !== undefined ? { kindUnverified: best.kindUnverified } : {}),
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
        clean(r.kindUnverified === undefined ? r.detail : `${r.detail}; kinds unverified: ${r.kindUnverified}`.replace(/^; /, '')) ||
          '-',
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
// the match list: `<target index> <our symbol>` per line, `#` starts a comment

/** a line of the match list (a scan Match the list records) */
export interface ListedMatch {
  index: number;
  symbol: string;
  line: number;
}

export function parseMatchList(text: string): { matches: ListedMatch[]; lines: string[] } {
  const lines = text.split('\n');
  if (lines.at(-1) === '') {
    lines.pop();
  }
  const matches: ListedMatch[] = [];
  const seen = new Map<number, number>();
  lines.forEach((raw, k) => {
    const l = raw.replace(/#.*/, '').trim();
    if (l === '') {
      return;
    }
    const m = l.match(/^(\d+)\s+(\S+)$/);
    if (m === null) {
      throw new Error(`match list line ${k + 1}: expected '<target index> <symbol>', got '${raw}'`);
    }
    const index = Number(m[1]);
    const prev = seen.get(index);
    if (prev !== undefined) {
      throw new Error(`match list line ${k + 1}: function ${index} is already matched on line ${prev}`);
    }
    seen.set(index, k + 1);
    matches.push({ index, symbol: m[2], line: k + 1 });
  });
  return { matches, lines };
}

/** A match line whose symbol left our link, renamed to the twin the scan matches its function with. */
export interface Renamed {
  index: number;
  line: number;
  from: string;
  to: string;
}

export interface AddedMatches {
  text: string;
  added: number;
  /** the lines rewritten in place: a twin whose name moved */
  renamed: Renamed[];
  /** the listed matches whose symbol is not in our link and that the scan matches under no other symbol: left as
   *  written, for the build to refuse */
  missing: ListedMatch[];
}

/**
 * The match list with a line added for every scan match it does not list yet; existing lines stay as written, but
 * one. When our link merges byte-identical bodies, binaryen keeps the first in link order, so a new unit whose body
 * is a matched function's twin, linked before it, takes the merged body's name and the matched symbol leaves our
 * link (docs/reference/facts/splat.md §S6). A listed line whose symbol `inOurLink` refuses is rewritten in place to
 * the symbol the scan matches the same target function with (its body, re-pointed, is the target's bytes); any other
 * line whose symbol is missing stays, and is reported in `missing`. Without `inOurLink` no line is touched.
 */
export function addMatches(
  text: string,
  rows: readonly ScanRow[],
  inOurLink?: (symbol: string) => boolean,
): AddedMatches {
  const { matches, lines } = parseMatchList(text);
  const listed = new Set(matches.map((m) => m.index));
  const scanned = new Map(rows.map((r) => [r.index, r]));
  const renamed: Renamed[] = [];
  const missing: ListedMatch[] = [];
  const out = [...lines];
  const present = inOurLink ?? (() => true);
  for (const m of matches) {
    if (present(m.symbol)) {
      continue;
    }
    const r = scanned.get(m.index);
    if (r?.verdict === 'match' && r.symbol !== null && r.symbol !== m.symbol && present(r.symbol)) {
      out[m.line - 1] = out[m.line - 1].replace(/^(\s*\d+\s+)\S+/, `$1${r.symbol}`);
      renamed.push({ index: m.index, line: m.line, from: m.symbol, to: r.symbol });
    } else {
      missing.push(m);
    }
  }
  const fresh = rows.filter((r) => r.verdict === 'match' && !listed.has(r.index));
  out.push(...fresh.map((r) => `${r.index} ${r.symbol}`));
  return { text: `${out.join('\n')}\n`, added: fresh.length, renamed, missing };
}

// ---------------------------------------------------------------------------------------------------------------
// the build

export interface MatchFailure {
  match: ListedMatch;
  message: string;
}

export interface ComponentMeter {
  group: string;
  functions: number;
  matched: number;
  bytes: number;
  matchedBytes: number;
}

export interface BuildResult {
  assembled: AssembleResult;
  meter: ComponentMeter[];
  readings: ReadingMeter;
  matched: number;
  matchedBytes: number;
  functions: number;
  bytes: number;
  /** the matches of kind-unverified functions of ours (counted in `matched` too), with why */
  kindUnverified: KindUnverifiedMatch[];
  kindUnverifiedBytes: number;
  reviewWarnings: ReviewWarning[];
}

export interface KindUnverifiedMatch {
  match: ListedMatch;
  ours: number;
  why: string;
}

/**
 * A literal operand of a match that the check cannot tell from a target number: the source may have written the
 * target's address or slot by hand (read off the target), and the bytes would agree, since an integer of ours is
 * compared as itself. Three kinds of integer are listed; the rest are left out, as they are as often legitimate as
 * any constant (OpenSSL's error codes in our data range, small integers that equal some target slot):
 *   past-data     in the target's data [GLOBAL_BASE, target data end) and past ours, where nothing of ours lives
 *                 (an i64.const by its low word);
 *   target-object the address of a target data object the symbol file names, at REVIEW_OBJECT_FROM or above (below
 *                 it sizes, masks such as 65520, line numbers and field offsets coincide with the addresses of the
 *                 target's many strings: 36 more on 7,217 matches); an i64.const by its value, or by its low word,
 *                 at any value, when its high word looks like a length (a packed string view: length << 32 | address);
 *   past-table    an i32.const in the target's table [our table size, target table size), past ours.
 * A literal equal to a target slot inside our table, or an operand of ours that is a pointer where the target holds
 * an integer equal to its re-pointed value, is not listed: nothing tells it from a legitimate one.
 */
export interface ReviewWarning {
  match: ListedMatch;
  ours: number;
  /** body offset of our instruction */
  at: number;
  instr: string;
  value: number;
  why: 'past-data' | 'target-object' | 'past-table';
  /** the target object a target-object warning names */
  object?: string;
}

/** Below it, a literal equal to a target object's address is not a review warning (see ReviewWarning). */
export const REVIEW_OBJECT_FROM = 65536;

/** The review warnings of the matches `matched` (target index → our function). */
export function reviewWarnings(
  maps: SymbolMaps,
  matched: ReadonlyMap<number, { match: ListedMatch; ours: number }>,
): ReviewWarning[] {
  const end = maps.target.dataEnd ?? GLOBAL_BASE;
  const tableEnd = maps.target.module.tables[0]?.min ?? 0;
  const objects = new Map<number, string>();
  for (const [name, addr] of maps.target.symbols.data) {
    if (!objects.has(addr)) {
      objects.set(addr, name);
    }
  }
  const m = maps.ours.module;
  const out: ReviewWarning[] = [];
  for (const { match, ours } of matched.values()) {
    const fk = maps.kinds?.functions.get(ours);
    for (const i of disassemble(m.funcs[ours - m.importedFuncCount])) {
      const kind = operandKind(fk, i.offset);
      if (kind !== undefined && kind !== 'literal') {
        continue;
      }
      for (const im of i.imms) {
        // the 32-bit number the operand may be: an i32.const or memarg offset, an i64.const's low word
        let value: number | undefined;
        let object: number | undefined;
        if (im.kind === 'memarg' || (im.kind === 's32' && i.opcode === 0x41)) {
          value = im.kind === 'memarg' ? im.offset : im.value;
          object = value >= REVIEW_OBJECT_FROM ? value : undefined;
        } else if (im.kind === 's64' && i.opcode === 0x42) {
          // any other i64 is no 32-bit number of the target's
          const high = Number(BigInt.asUintN(64, im.value) >> 32n);
          value = high < 65536 ? Number(BigInt.asUintN(32, im.value)) : undefined;
          object = value === undefined || (high === 0 && value < REVIEW_OBJECT_FROM) ? undefined : value;
        }
        if (value === undefined) {
          continue;
        }
        const warn = (why: ReviewWarning['why'], name?: string): void => {
          const named = name === undefined ? {} : { object: name };
          out.push({ match, ours, at: i.offset, instr: instrText(i, m), value, why, ...named });
        };
        if (value >= GLOBAL_BASE && value >= maps.ours.dataEnd && value < end) {
          warn('past-data');
        } else if (object !== undefined && objects.has(object)) {
          warn('target-object', objects.get(object));
        } else if (im.kind === 's32' && value >= maps.ours.tableSize && value < tableEnd) {
          warn('past-table');
        }
      }
    }
  }
  return out.sort((a, b) => a.match.line - b.match.line || a.at - b.at);
}

/** splat-build --max-kind-unverified: what is wrong when more than `max` matches are kind-unverified, else null. A
 *  kind-unverified match is checked with every reading, so each new one is a decision to raise the limit. */
export function kindUnverifiedOverLimit(r: BuildResult, max: number): string | null {
  if (r.kindUnverified.length <= max) {
    return null;
  }
  return [
    `${r.kindUnverified.length} matches are kind-unverified, more than the ${max} allowed (--max-kind-unverified); the`,
    'operand-kind table cannot vouch for their constants, so each is checked with every reading:',
    ...r.kindUnverified.map((u) => `  line ${u.match.line}: function ${u.match.index} ${u.match.symbol} (ours ${u.ours}): ${u.why}`),
  ].join('\n');
}

/** Re-point every matched function and compare it with its piece; failures name the function and where it differs. */
export function verifyMatches(
  maps: SymbolMaps,
  manifest: SplatManifest,
  bodies: ReadonlyMap<number, Uint8Array>,
  matches: readonly ListedMatch[],
): {
  substitutes: Map<number, Uint8Array>;
  chosen: Map<number, Chosen[]>;
  failures: MatchFailure[];
  /** target index → why our function's operand kinds are not known, for each verified match that has none */
  kindUnverified: Map<number, string>;
} {
  const byIndex = new Map(manifest.functions.map((f) => [f.index, f]));
  const substitutes = new Map<number, Uint8Array>();
  const chosen = new Map<number, Chosen[]>();
  const kindUnverified = new Map<number, string>();
  const failures: MatchFailure[] = [];
  const who = (match: ListedMatch, ours?: number): string => {
    const f = byIndex.get(match.index);
    return `function ${match.index}${f?.name ? ` (${f.name})` : ''} matched to ${match.symbol}${ours === undefined ? '' : ` (ours ${ours})`}`;
  };
  for (const match of matches) {
    const ours = maps.ours.fnByKey.get(match.symbol);
    if (!byIndex.has(match.index)) {
      failures.push({ match, message: `${who(match)}: ${match.index} is not a defined function of the target` });
      continue;
    }
    if (ours === undefined || ours < maps.ours.module.importedFuncCount) {
      failures.push({ match, message: `${who(match)}: our link has no function ${match.symbol}` });
      continue;
    }
    const a = attempt(maps, { ours, symbol: match.symbol, source: 'symbol' }, match.index, bodies.get(match.index)!);
    if (a.verdict === 'match') {
      substitutes.set(match.index, a.bytes);
      chosen.set(match.index, a.chosen);
      if (a.kindUnverified !== undefined) {
        kindUnverified.set(match.index, a.kindUnverified);
      }
    } else {
      failures.push({
        match,
        message: `${who(match, ours)} ${a.verdict === 'differs' ? 'DIFFERS at' : 'is UNRESOLVED:'} ${a.detail}`,
      });
    }
  }
  const oursOf = new Map(matches.map((m) => [m.index, maps.ours.fnByKey.get(m.symbol)]));
  const refused = selfVoted(maps, new Map([...chosen].map(([t, c]) => [t, { chosen: c, ours: oursOf.get(t) }])));
  for (const match of matches) {
    const x = refused.get(match.index);
    if (x !== undefined) {
      substitutes.delete(match.index);
      chosen.delete(match.index);
      kindUnverified.delete(match.index);
      failures.push({
        match,
        message: `${who(match, maps.ours.fnByKey.get(match.symbol))} is UNCONFIRMED: ${selfVotedText(maps, match.index, bodies.get(match.index)!, x)}`,
      });
    }
  }
  failures.sort((a, b) => a.match.line - b.match.line);
  return { substitutes, chosen, failures, kindUnverified };
}

/** How the target's bytes chose among our readings: per match, the strongest choice it needed. */
export interface ReadingMeter {
  /** matches whose every ambiguous operand took its first reading */
  preferred: { matches: number; bytes: number };
  /** matches where the target picked another reading (literal, table slot or address) of some operand */
  other: { matches: number; bytes: number; operands: number; byKind: Record<string, number> };
  /** matches where the target picked which copy of identical string or read-only bytes an operand names, or which of
   *  the table slots holding one function */
  copy: { matches: number; bytes: number; operands: number };
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
    preferred: { matches: 0, bytes: 0 },
    other: { matches: 0, bytes: 0, operands: 0, byKind: {} },
    copy: { matches: 0, bytes: 0, operands: 0 },
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
    into.matches++;
    into.bytes += size.get(t) ?? 0;
  }
  return out;
}

export function meterOf(manifest: SplatManifest, matched: ReadonlySet<number>): ComponentMeter[] {
  const by = new Map<string, ComponentMeter>();
  for (const f of manifest.functions) {
    const c = by.get(f.group) ?? { group: f.group, functions: 0, matched: 0, bytes: 0, matchedBytes: 0 };
    c.functions++;
    c.bytes += f.size;
    if (matched.has(f.index)) {
      c.matched++;
      c.matchedBytes += f.size;
    }
    by.set(f.group, c);
  }
  return [...by.values()].sort(
    (a, b) => b.matchedBytes - a.matchedBytes || b.bytes - a.bytes || a.group.localeCompare(b.group),
  );
}

export class MatchesFailed extends Error {
  constructor(readonly failures: MatchFailure[]) {
    super(
      `${failures.length} matched function${failures.length === 1 ? '' : 's'} no longer verif${failures.length === 1 ? 'ies' : 'y'}:\n${failures
        .map((f) => `  line ${f.match.line}: ${f.message}`)
        .join('\n')}`,
    );
  }
}

/** Assemble the split with our re-pointed bytes for every match; throws (MatchesFailed) when any match does not
 *  verify, and when the module is not `expectSha256`. */
export function buildSplat(maps: SymbolMaps, dir: string, matches: readonly ListedMatch[], expectSha256: string): BuildResult {
  const manifest = readManifest(dir);
  const bodies = readPieceBodies(dir, manifest);
  const { substitutes, chosen, failures, kindUnverified } = verifyMatches(
    withMatches(maps, matches),
    manifest,
    bodies,
    matches,
  );
  if (failures.length > 0) {
    throw new MatchesFailed(failures);
  }
  const assembled = assembleSplit(dir, substitutes);
  if (assembled.substituted.length > 0) {
    throw new Error(`substitutes that differ from their piece: ${assembled.substituted.join(', ')}`);
  }
  if (assembled.sha256 !== expectSha256) {
    throw new Error(`the assembled module is sha256 ${assembled.sha256}, not ${expectSha256}`);
  }
  const matchedSet = new Set(substitutes.keys());
  const meter = meterOf(manifest, matchedSet);
  const sum = (k: keyof ComponentMeter): number => meter.reduce((n, c) => n + (c[k] as number), 0);
  const size = new Map(manifest.functions.map((f) => [f.index, f.size]));
  const oursOf = (m: ListedMatch): number => maps.ours.fnByKey.get(m.symbol)!;
  const listed = matches.filter((m) => substitutes.has(m.index));
  return {
    assembled,
    meter,
    readings: readingMeterOf(manifest, chosen),
    matched: sum('matched'),
    matchedBytes: sum('matchedBytes'),
    functions: sum('functions'),
    bytes: sum('bytes'),
    kindUnverified: listed
      .filter((m) => kindUnverified.has(m.index))
      .map((m) => ({ match: m, ours: oursOf(m), why: kindUnverified.get(m.index)! })),
    kindUnverifiedBytes: [...kindUnverified.keys()].reduce((n, t) => n + (size.get(t) ?? 0), 0),
    reviewWarnings: reviewWarnings(maps, new Map(listed.map((m) => [m.index, { match: m, ours: oursOf(m) }]))),
  };
}
