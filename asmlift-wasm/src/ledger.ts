// The progress meter: one row per target function, with its owner and the best pair any candidate
// module offers. A candidate is a whole linked module (a library build, a probe, the project's own
// link) under a label; every target function is paired against each candidate by census.ts and
// the strongest pairing wins, in the order
//
//   byte         the paired function has the same body bytes (call indices and addresses included)
//   fingerprint  same rows after normalisation (callees and data by content)
//   addr         same rows with every address blinded; `score` is the row diff (the address rows)
//   shape        same rows with every constant dropped; `score` is the row diff against it
//   none
//
// `best_name` is the paired function's name when the candidate came with a symbol map (emcc
// --emit-symbol-map writes `x.js.symbols` beside the module; the map changes the link's function
// ORDER, never the bodies, so a mapped build is fine as a candidate and wrong for a byte compare).
//
// `uses` counts the target's direct `call` sites that name the function, the number binaryen's
// reorder-functions sorted the code section by (urna docs/reference/facts/link-order.md §L1).
import { existsSync, readFileSync } from 'node:fs';

import { type Census, census } from './census';
import type { NormalizedFn } from './normalize';
import { signature } from './normalize';
import type { Module } from './parse';
import { score } from './score';

export type PairKind = 'byte' | 'fingerprint' | 'addr' | 'shape' | 'none';

export interface Best {
  label: string;
  index: number;
  kind: PairKind;
  /** rows that differ against the paired function (0 for byte and fingerprint) */
  score: number;
  /** the paired function's name from the candidate's symbol map, when it has one */
  name: string | null;
}

export interface LedgerRow {
  index: number;
  size: number;
  uses: number;
  /** from the per-function class map (cls2.json), '-' when unknown */
  owner: string;
  /** from func-attribution.tsv, '-' when unknown */
  component: string;
  best: Best | null;
}

export interface CandidateModule {
  label: string;
  module: Module;
  norm: NormalizedFn[];
  /** function index → name (`x.js.symbols`) */
  names?: Map<number, string>;
}

const CALL = 0x10;
const RETURN_CALL = 0x12;

/** Incoming direct-call site counts, indexed by function index (imports included, always 0 there). */
export function directUses(m: Module, norm: NormalizedFn[]): number[] {
  const uses = new Array<number>(m.importedFuncCount + m.funcs.length).fill(0);
  for (const n of norm) {
    for (const i of n.instrs) {
      if (i.opcode === CALL || i.opcode === RETURN_CALL) {
        const callee = (i.imms[0] as { value: number }).value;
        if (callee < uses.length) {
          uses[callee]++;
        }
      }
    }
  }
  return uses;
}

/** `{ "<index>": "<class>" }` (tools/libs/cls2.json). */
export function readOwners(path: string): Map<number, string> {
  const parsed = JSON.parse(readFileSync(path, 'utf8')) as Record<string, string>;
  return new Map(Object.entries(parsed).map(([k, v]) => [Number(k), v]));
}

export interface Attribution {
  component: string;
  evidence: string;
  detail: string;
}

/** `func component evidence detail` rows (ledger/func-attribution.tsv); '-' cells stay '-'. */
export function readAttribution(path: string): Map<number, Attribution> {
  const out = new Map<number, Attribution>();
  const lines = readFileSync(path, 'utf8').split('\n');
  for (const line of lines.slice(1)) {
    if (line === '') {
      continue;
    }
    const [func, component = '-', evidence = '-', detail = ''] = line.split('\t');
    out.set(Number(func), { component, evidence, detail });
  }
  return out;
}

function sameBytes(a: Uint8Array, b: Uint8Array): boolean {
  if (a.length !== b.length) {
    return false;
  }
  for (let i = 0; i < a.length; i++) {
    if (a[i] !== b[i]) {
      return false;
    }
  }
  return true;
}

const RANK: Record<PairKind, number> = { byte: 0, fingerprint: 1, addr: 2, shape: 3, none: 4 };

function better(x: Best, y: Best | null): boolean {
  if (y === null) {
    return true;
  }
  if (RANK[x.kind] !== RANK[y.kind]) {
    return RANK[x.kind] < RANK[y.kind];
  }
  return x.score < y.score;
}

/** The strongest pairing per target function over every candidate, plus each candidate's census. */
export function bestPairs(
  target: Module,
  targetNorm: NormalizedFn[],
  candidates: readonly CandidateModule[],
): { best: Map<number, Best>; censuses: Map<string, Census> } {
  const best = new Map<number, Best>();
  const censuses = new Map<string, Census>();
  const tFirst = target.importedFuncCount;
  for (const c of candidates) {
    const cen = census(target, targetNorm, c.module, c.norm);
    censuses.set(c.label, cen);
    const cFirst = c.module.importedFuncCount;
    for (const p of cen.pairs) {
      const ta = target.funcs[p.a - tFirst];
      const tb = c.module.funcs[p.b - cFirst];
      const name = c.names?.get(p.b) ?? null;
      let b: Best;
      if (p.by === 'fingerprint') {
        const kind = sameBytes(ta.bodyBytes, tb.bodyBytes) ? 'byte' : 'fingerprint';
        b = { label: c.label, index: p.b, kind, score: 0, name };
      } else {
        const na = targetNorm[p.a - tFirst];
        const nb = c.norm[p.b - cFirst];
        const s = score(
          { rows: na.rows, fn: ta, signature: signature(ta) },
          { rows: nb.rows, fn: tb, signature: signature(tb) },
        );
        b = { label: c.label, index: p.b, kind: p.by, score: s.score, name };
      }
      if (better(b, best.get(p.a) ?? null)) {
        best.set(p.a, b);
      }
    }
  }
  return { best, censuses };
}

export interface LedgerInputs {
  owners?: Map<number, string>;
  attribution?: Map<number, Attribution>;
}

export function ledgerRows(
  target: Module,
  targetNorm: NormalizedFn[],
  candidates: readonly CandidateModule[],
  inputs: LedgerInputs = {},
): { rows: LedgerRow[]; censuses: Map<string, Census> } {
  const uses = directUses(target, targetNorm);
  const { best, censuses } = bestPairs(target, targetNorm, candidates);
  const rows = target.funcs.map((fn) => ({
    index: fn.index,
    size: fn.size,
    uses: uses[fn.index],
    owner: inputs.owners?.get(fn.index) ?? '-',
    component: inputs.attribution?.get(fn.index)?.component ?? '-',
    best: best.get(fn.index) ?? null,
  }));
  return { rows, censuses };
}

/** body size at or below which a match carries no identity: see `ledgerSummary` */
export const TRIVIAL = 8;

export const LEDGER_HEADER = [
  'index',
  'size',
  'uses',
  'owner',
  'component',
  'best_label',
  'best_index',
  'best_kind',
  'best_score',
  'best_name',
];

export function renderLedgerTsv(rows: readonly LedgerRow[]): string {
  const lines = [LEDGER_HEADER.join('\t')];
  for (const r of rows) {
    const b = r.best;
    lines.push(
      [
        r.index,
        r.size,
        r.uses,
        r.owner,
        r.component,
        b?.label ?? '-',
        b?.index ?? '-',
        b?.kind ?? 'none',
        b?.score ?? '-',
        b?.name ?? '',
      ].join('\t'),
    );
  }
  return `${lines.join('\n')}\n`;
}

/** The meter: counts and bytes per kind, overall and per owner. */
export function ledgerSummary(rows: readonly LedgerRow[]): string {
  const kinds: PairKind[] = ['byte', 'fingerprint', 'addr', 'shape', 'none'];
  const tally = (rs: readonly LedgerRow[]): string => {
    const n: Record<PairKind, number> = { byte: 0, fingerprint: 0, addr: 0, shape: 0, none: 0 };
    const bytes: Record<PairKind, number> = { byte: 0, fingerprint: 0, addr: 0, shape: 0, none: 0 };
    for (const r of rs) {
      const k = r.best?.kind ?? 'none';
      n[k]++;
      bytes[k] += r.size;
    }
    return kinds.map((k) => `${k} ${n[k]} (${bytes[k]} B)`).join('  ');
  };
  const lines = [`all ${rows.length}: ${tally(rows)}`];
  // a body of ≤ TRIVIAL bytes (`end`, `local.get 0 end`, one call) pairs with any same-signature twin:
  // its match says nothing about which library it came from, so the meter shows it apart
  const trivial = rows.filter((r) => r.size <= TRIVIAL && r.best !== null);
  lines.push(`of which trivial (≤ ${TRIVIAL} B bodies, no identity evidence): ${tally(trivial)}`);
  const byOwner = new Map<string, LedgerRow[]>();
  for (const r of rows) {
    const list = byOwner.get(r.owner);
    if (list === undefined) {
      byOwner.set(r.owner, [r]);
    } else {
      list.push(r);
    }
  }
  for (const [owner, rs] of [...byOwner.entries()].sort((x, y) => y[1].length - x[1].length)) {
    lines.push(`${owner.padEnd(10)} ${String(rs.length).padStart(6)}: ${tally(rs)}`);
  }
  return lines.join('\n');
}

/** `<function index>:<name>` lines of an emcc `--emit-symbol-map` file (`x.js.symbols`). */
export function readSymbolMap(path: string): Map<number, string> {
  const out = new Map<number, string>();
  for (const line of readFileSync(path, 'utf8').split('\n')) {
    const colon = line.indexOf(':');
    if (colon > 0) {
      out.set(Number(line.slice(0, colon)), line.slice(colon + 1));
    }
  }
  return out;
}

/** Names for a PLAIN link's functions carried over from its `--emit-symbol-map` twin: the map
 *  changes the function order and never the bodies, so a fingerprint census between the two pairs
 *  the same function on both sides and the twin's name applies to the plain index. Identical bodies
 *  (thunks, trivial destructors) pair positionally, so such a name is one of the identical twins'. */
export function namesViaTwin(
  plain: Module,
  plainNorm: NormalizedFn[],
  twin: Module,
  twinNorm: NormalizedFn[],
  twinNames: Map<number, string>,
): Map<number, string> {
  const out = new Map<number, string>();
  for (const p of census(plain, plainNorm, twin, twinNorm).pairs) {
    if (p.by !== 'fingerprint') {
      continue;
    }
    const name = twinNames.get(p.b);
    if (name !== undefined) {
      out.set(p.a, name);
    }
  }
  return out;
}

/** The symbol map emcc wrote beside a linked module: `x.wasm` → `x.js.symbols`. */
export function siblingSymbolMap(wasmPath: string): Map<number, string> | null {
  const p = wasmPath.endsWith('.wasm') ? `${wasmPath.slice(0, -5)}.js.symbols` : `${wasmPath}.js.symbols`;
  return existsSync(p) ? readSymbolMap(p) : null;
}

/** Read a functions.tsv back (the `fn` readout shows the owner and best pair from it). */
export function readLedgerTsv(path: string): Map<number, LedgerRow> {
  return parseLedgerTsv(readFileSync(path, 'utf8'));
}

/** A functions.tsv's text as rows, e.g. one read from git at another commit. */
export function parseLedgerTsv(text: string): Map<number, LedgerRow> {
  const out = new Map<number, LedgerRow>();
  const lines = text.split('\n');
  for (const line of lines.slice(1)) {
    if (line === '') {
      continue;
    }
    const [index, size, uses, owner, component, label, bIndex, kind, bScore, bName = ''] = line.split('\t');
    out.set(Number(index), {
      index: Number(index),
      size: Number(size),
      uses: Number(uses),
      owner,
      component,
      best:
        kind === 'none'
          ? null
          : {
              label,
              index: Number(bIndex),
              kind: kind as PairKind,
              score: Number(bScore),
              name: bName === '' ? null : bName,
            },
    });
  }
  return out;
}

/** Index → name for every row whose best pair is strong enough to name the function: a byte,
 *  fingerprint or addr pairing (a shape pairing is often another function of the same shape). */
export function ledgerNames(rows: ReadonlyMap<number, LedgerRow>): Map<number, string> {
  const out = new Map<number, string>();
  for (const [index, row] of rows) {
    const name = row.best?.name;
    if (name !== undefined && name !== null && row.best?.kind !== 'shape') {
      out.set(index, name);
    }
  }
  return out;
}

/** demangled → mangled from two line-aligned name lists of one build (`llvm-nm` and `llvm-nm -C`). */
export function readMangledPairs(mangledPath: string, demangledPath: string): Map<string, string> {
  const mangled = readFileSync(mangledPath, 'utf8').split('\n');
  const demangled = readFileSync(demangledPath, 'utf8').split('\n');
  if (mangled.length !== demangled.length) {
    throw new Error(`${mangledPath} has ${mangled.length} lines but ${demangledPath} has ${demangled.length}`);
  }
  const out = new Map<string, string>();
  demangled.forEach((d, k) => {
    if (d !== '' && d !== mangled[k] && !out.has(d)) {
      out.set(d, mangled[k]);
    }
  });
  return out;
}
