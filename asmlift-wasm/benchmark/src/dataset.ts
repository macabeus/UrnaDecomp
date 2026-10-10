// The dataset: target functions, each with the compile setup that built it, its features, the
// ledger's current match and a pointer to source that reproduces it. Rows are selected per setup
// deterministically (size bands in turn, each pick the one adding the most features not yet
// covered, the strongest ledger match and a fixed hash breaking ties); `add` appends a hand-picked
// row built the same way. The committed rows are frozen: selection.ts records what they were selected
// from, and a re-selection is a deliberate new dataset version.
import { readFileSync, writeFileSync } from 'node:fs';

import {
  type ImportNames,
  type LedgerRow,
  type Module,
  SIZE_BANDS,
  funcAt,
  glueNamesFor,
  parseModule,
  readLedgerTsv,
  sizeBand,
} from '@asmlift/wasm';

import { featuresOf } from './features';
import { LEDGER_TSV, ROWS_JSON, TARGET } from './paths';
import { type Definitions, type Resolution, loadDefinitions, resolveName } from './provenance';
import { type ReadRepoFile, definitionLine, readCheckout, referenceFor } from './reference';
import type { DatasetRow, Feature, Lang, MatchKind, Setup } from './schema';
import { loadSetups, setupFlags } from './setups';

/** Larger bodies make the row diff approximate and the run slow; the dataset stops here. */
export const MAX_SIZE = 8192;
/** Bodies this small pair positionally among their identical twins, so their ledger name (and with
 *  it the setup) is not evidence; selection skips them. */
export const MIN_SIZE = 9;
export const DEFAULT_PER_SETUP = 12;

/** Setups whose sources are reconstructed in this repo (a reference only when they reproduce it). */
const RECONSTRUCTED = /^src\//;

/** What selection reads besides the pinned target: the ledger, the registry, the build's definitions
 *  and the repo's sources. */
export interface SelectionInputs {
  ledger: Map<number, LedgerRow>;
  setups: Setup[];
  defs: Definitions;
  /** the repo's files a reference is looked up in */
  read: ReadRepoFile;
}

export interface Context extends SelectionInputs {
  m: Module;
  imports: ImportNames | null;
  /** ledger names held by more than one target function */
  sharedNames: Set<string>;
}

export function contextOf(inputs: SelectionInputs): Context {
  const seen = new Map<string, number>();
  for (const r of inputs.ledger.values()) {
    if (r.best?.name) {
      seen.set(r.best.name, (seen.get(r.best.name) ?? 0) + 1);
    }
  }
  return {
    ...inputs,
    m: parseModule(new Uint8Array(readFileSync(TARGET))),
    imports: glueNamesFor(TARGET)?.imports ?? null,
    sharedNames: new Set([...seen].filter(([, n]) => n > 1).map(([name]) => name)),
  };
}

/** Today's inputs: the checkout's ledger, registry and sources, the build's definitions. */
export function loadContext(opts: { image?: string; rebuildDefinitions?: boolean } = {}): Context {
  const setups = loadSetups();
  return contextOf({
    ledger: readLedgerTsv(LEDGER_TSV),
    setups,
    defs: loadDefinitions(setups, opts.image ?? setups[0].image, opts.rebuildDefinitions ?? false),
    read: readCheckout,
  });
}

const STRONG: readonly MatchKind[] = ['byte', 'fingerprint', 'addr'];
const KIND_RANK: Record<MatchKind, number> = { byte: 0, fingerprint: 1, addr: 2, shape: 3, none: 4 };

/** The ledger's name for target function `index` when a byte, fingerprint or addr pairing gives one. */
function strongName(ctx: Context, index: number): string | null {
  const r = ctx.ledger.get(index);
  return r?.best && STRONG.includes(r.best.kind) && r.best.name !== null ? r.best.name : null;
}

/** The setup behind target function `index`, from the ledger's name, else from its component. */
export function resolveIndex(ctx: Context, index: number): Resolution & { name: string | null } {
  const r = ctx.ledger.get(index);
  const name = strongName(ctx, index);
  if (name !== null && !ctx.sharedNames.has(name)) {
    const res = resolveName(ctx.defs, name, ctx.setups);
    if (res.kind === 'resolved') {
      return { ...res, name };
    }
  }
  const byComponent = ctx.setups.find((s) => s.components?.includes(r?.component ?? '-'));
  if (byComponent !== undefined) {
    return { kind: 'unresolved', reason: `component:${byComponent.key}`, name };
  }
  return { kind: 'unresolved', reason: name === null ? 'no strong ledger name' : 'name unresolved', name };
}

export function buildRow(ctx: Context, index: number, origin: DatasetRow['origin'] = 'selected'): DatasetRow {
  const fn = funcAt(ctx.m, index);
  if (fn === null) {
    throw new Error(`${index} is not a defined function of the target`);
  }
  const led = ctx.ledger.get(index);
  const res = resolveIndex(ctx, index);
  const kind: MatchKind = led?.best?.kind ?? 'none';
  const base = {
    id: '',
    index,
    size: fn.size,
    band: sizeBand(fn.size),
    component: led?.component === undefined || led.component === '-' ? 'unknown' : led.component,
    owner: led?.owner ?? '-',
    features: featuresOf(ctx.m, index, ctx.imports),
    ledger: { kind, score: led?.best ? led.best.score : null },
    origin,
  };
  if (res.kind === 'resolved') {
    const setup = ctx.setups.find((s) => s.key === res.setup)!;
    const flagsFrom = res.instantiatedBy === undefined ? setup : ctx.setups.find((s) => s.key === res.instantiatedBy)!;
    const obj = ctx.defs.objects.find((o) => o.object === res.def.object);
    let reference =
      res.unique && !res.viaStandIn ? referenceFor(ctx.defs, res.def, res.setup, obj?.source ?? null, ctx.read) : null;
    if (reference !== null && RECONSTRUCTED.test(reference.path) && kind !== 'byte' && kind !== 'fingerprint') {
      reference = null;
    }
    if (reference !== null) {
      const line = definitionLine(reference, res.name!, ctx.read);
      if (line !== undefined) {
        reference = { ...reference, line };
      }
    }
    return {
      ...base,
      id: `${res.setup}/${index}`,
      setup: res.setup,
      lang: res.lang,
      flags: setupFlags(flagsFrom, res.lang),
      ...(res.instantiatedBy !== undefined ? { instantiatedBy: res.instantiatedBy } : {}),
      name: res.name,
      symbol: res.def.mangled,
      evidence: 'symbol',
      object: res.def.member === undefined ? res.def.object : `${res.def.object}:${res.def.member}`,
      reference,
    } satisfies DatasetRow;
  }
  if (res.reason.startsWith('component:')) {
    const setup = ctx.setups.find((s) => s.key === res.reason.slice('component:'.length))!;
    const lang: Lang = setup.flags?.['c++'] !== undefined ? 'c++' : 'c';
    return {
      ...base,
      id: `${setup.key}/${index}`,
      setup: setup.key,
      lang,
      flags: setupFlags(setup, lang),
      name: res.name,
      symbol: null,
      evidence: 'component',
      object: null,
      reference: null,
    } satisfies DatasetRow;
  }
  throw new Error(`function ${index}: no setup (${res.reason}); name it with --setup … once its object is in the build`);
}

/** FNV-1a of a string: a fixed tie-break that does not depend on iteration order. */
function hash(s: string): number {
  let h = 0x811c9dc5;
  for (let i = 0; i < s.length; i++) {
    h = Math.imul(h ^ s.charCodeAt(i), 0x01000193) >>> 0;
  }
  return h;
}

interface Candidate {
  index: number;
  band: string;
  features: Feature[];
  /** symbol evidence first, then the ledger's kind, then the hash */
  rank: [number, number, number];
}

/** Every target function each setup can claim, as selection candidates. */
export function candidates(ctx: Context): Map<string, Candidate[]> {
  const out = new Map<string, Candidate[]>(ctx.setups.map((s) => [s.key, []]));
  for (const fn of ctx.m.funcs) {
    if (fn.size > MAX_SIZE || fn.size < MIN_SIZE) {
      continue;
    }
    const res = resolveIndex(ctx, fn.index);
    const kind = ctx.ledger.get(fn.index)?.best?.kind ?? 'none';
    let setup: string | null = null;
    let evidence = 0;
    if (res.kind === 'resolved') {
      if (res.viaStandIn && kind !== 'byte' && kind !== 'fingerprint') {
        continue;
      }
      setup = res.setup;
    } else if (res.reason.startsWith('component:')) {
      setup = res.reason.slice('component:'.length);
      evidence = 1;
    }
    if (setup === null) {
      continue;
    }
    out.get(setup)!.push({
      index: fn.index,
      band: sizeBand(fn.size),
      features: featuresOf(ctx.m, fn.index, ctx.imports),
      rank: [evidence, KIND_RANK[kind], hash(`${setup}/${fn.index}`)],
    });
  }
  return out;
}

const byRank = (a: Candidate, b: Candidate): number =>
  a.rank[0] - b.rank[0] || a.rank[1] - b.rank[1] || a.rank[2] - b.rank[2];

/** `per` rows of one setup's candidates: the size bands take turns, smallest first; each turn
 *  takes the band's candidate adding the most features the setup's picks do not cover yet, the
 *  better-ranked one on a tie. */
export function pick(cands: readonly Candidate[], per: number): number[] {
  const pools = SIZE_BANDS.map((b) => cands.filter((c) => c.band === b.name).sort(byRank));
  const covered = new Set<Feature>();
  const out: number[] = [];
  while (out.length < per && pools.some((p) => p.length > 0)) {
    for (const pool of pools) {
      if (out.length >= per || pool.length === 0) {
        continue;
      }
      let best = 0;
      let bestGain = -1;
      pool.forEach((c, k) => {
        const gain = c.features.filter((f) => !covered.has(f)).length;
        if (gain > bestGain) {
          best = k;
          bestGain = gain;
        }
      });
      const [c] = pool.splice(best, 1);
      c.features.forEach((f) => covered.add(f));
      out.push(c.index);
    }
  }
  return out;
}

/** The names the definitions are looked up by in selection: a strong ledger name, held by one target
 *  function, of a function inside the size limits. */
export function selectionNames(ctx: Context): Set<string> {
  const out = new Set<string>();
  for (const fn of ctx.m.funcs) {
    const name = fn.size > MAX_SIZE || fn.size < MIN_SIZE ? null : strongName(ctx, fn.index);
    if (name !== null && !ctx.sharedNames.has(name)) {
      out.add(name);
    }
  }
  return out;
}

export function selectRows(ctx: Context, per = DEFAULT_PER_SETUP): DatasetRow[] {
  const cands = candidates(ctx);
  const rows: DatasetRow[] = [];
  for (const s of ctx.setups) {
    for (const index of pick(cands.get(s.key)!, per)) {
      rows.push(buildRow(ctx, index));
    }
  }
  return sortRows(rows, ctx.setups);
}

/** The dataset a selection gives: the hand-added rows of `rows`, then the selected rows whose function
 *  is not one of them. */
export function selectDataset(ctx: Context, rows: readonly DatasetRow[], per = DEFAULT_PER_SETUP): DatasetRow[] {
  const manual = rows.filter((r) => r.origin === 'manual');
  const taken = new Set(manual.map((r) => r.index));
  return sortRows([...manual, ...selectRows(ctx, per).filter((r) => !taken.has(r.index))], ctx.setups);
}

export function sortRows(rows: DatasetRow[], setups: readonly Setup[] = loadSetups()): DatasetRow[] {
  const order = new Map(setups.map((s, k) => [s.key, k]));
  return rows.sort((a, b) => order.get(a.setup)! - order.get(b.setup)! || a.index - b.index);
}

export function readRows(path = ROWS_JSON): DatasetRow[] {
  return JSON.parse(readFileSync(path, 'utf8')) as DatasetRow[];
}

export function writeRows(rows: readonly DatasetRow[], path = ROWS_JSON): void {
  const ids = new Set<string>();
  const indices = new Set<number>();
  for (const r of rows) {
    if (ids.has(r.id) || indices.has(r.index)) {
      throw new Error(`row ${r.id}: function ${r.index} is in the dataset twice`);
    }
    ids.add(r.id);
    indices.add(r.index);
  }
  writeFileSync(path, `${JSON.stringify(rows, null, 2)}\n`);
}
