// The decompilation meter as the Progress page draws it: matched functions and code bytes per origin, per
// component and per function, summed the way tools/round/status.py sums them for docs/status.md (a function counts
// when the scan's verdict is `match`, which is exactly a line of splat/matched.txt). The origins and the TSE
// components' descriptions are status.py's; the tests check every number against docs/status.md.
import { type ProgressData, type ProgressPoint, VERDICTS, type Verdict } from './progress-data';

export type OriginId = 'tse' | 'upstream' | 'unattributed';
export type Metric = 'functions' | 'bytes';

export interface Origin {
  id: OriginId;
  label: string;
  blurb: string;
}

export const ORIGINS: readonly Origin[] = [
  { id: 'tse', label: 'TSE code', blurb: "TSE's own code, the code this project exists to recover" },
  {
    id: 'upstream',
    label: 'Upstream libraries',
    blurb: 'rebuilt from their own sources at the versions third_party/MANIFEST.md pins',
  },
  { id: 'unattributed', label: 'Not attributed', blurb: 'no evidence yet of the component that calls them' },
];

/** status.py's UENUX2 and TSE_OTHER, in its order. */
export const TSE_COMPONENTS: readonly [string, string][] = [
  ['vota', 'uenux2 src/app/vota: the voting application'],
  ['comum', 'uenux2 src/app/comum: shared urna code (domain model, converters, reports)'],
  ['api', 'uenux2 src/api: GUI toolkit, io, ipc, util, logging, audio'],
  ['simulador', "the web simulator's CWasm* devices"],
  ['vota-web', 'the web engine (CVotaWebEngine, …)'],
  ['ecourna', 'the ecourna library: security, SQLite wrapper, compression, app/dados'],
  ['asn1-modules', 'the generated Modulo* ASN.1 types'],
  ['ASN1', "TSE's fork of the III ASN.1 runtime"],
];
export const NOT_ATTRIBUTED: readonly [string, string][] = [
  ['unknown', 'ledger/func-attribution.tsv gives no component'],
  ['app', 'classed as application code by the library census, with no component evidence'],
];
const LIBRARIES: Record<string, string> = {
  openssl: 'OpenSSL 3.0.17 (libcrypto)',
  sqlite: 'SQLite 3.50.4',
  'libc++': 'libc++ of emsdk 6.0.0',
  '7zip': '7-Zip 21.02',
  RHVoice: 'RHVoice 1.14.0, the speech synthesizer',
  boost: 'Boost 1.89.0 (header-only parts)',
  'libc++abi': 'libc++abi of emsdk 6.0.0',
  libc: 'musl libc of emsdk 6.0.0',
  utf8cpp: 'utf8cpp, bundled in RHVoice',
  rapidxml: 'rapidxml, bundled in RHVoice',
};

const TSE_SET = new Set(TSE_COMPONENTS.map(([c]) => c));
const NA_SET = new Set(NOT_ATTRIBUTED.map(([c]) => c));

export function originOf(component: string): OriginId {
  return TSE_SET.has(component) ? 'tse' : NA_SET.has(component) ? 'unattributed' : 'upstream';
}

export function describe(component: string): string {
  return (
    TSE_COMPONENTS.find(([c]) => c === component)?.[1] ??
    NOT_ATTRIBUTED.find(([c]) => c === component)?.[1] ??
    LIBRARIES[component] ??
    'an upstream library'
  );
}

/** Functions and code bytes per verdict (indexed like VERDICTS). */
export interface Tally {
  fns: number[];
  bytes: number[];
}

export const emptyTally = (): Tally => ({ fns: VERDICTS.map(() => 0), bytes: VERDICTS.map(() => 0) });

export function addTally(into: Tally, t: Tally): Tally {
  for (let v = 0; v < VERDICTS.length; v++) {
    into.fns[v] += t.fns[v];
    into.bytes[v] += t.bytes[v];
  }
  return into;
}

export const matched = (t: Tally, m: Metric): number => (m === 'functions' ? t.fns : t.bytes)[0];
export const total = (t: Tally, m: Metric): number => (m === 'functions' ? t.fns : t.bytes).reduce((a, b) => a + b, 0);
export const share = (t: Tally, m: Metric): number => (total(t, m) === 0 ? 0 : matched(t, m) / total(t, m));

export interface Fn {
  index: number;
  component: string;
  verdict: Verdict;
  size: number;
  name: string;
}

export interface ComponentRow {
  id: string;
  origin: OriginId;
  description: string;
  tally: Tally;
}

export interface Meter {
  fns: Fn[];
  byComponent: Map<string, ComponentRow>;
  byOrigin: Map<OriginId, Tally>;
  all: Tally;
}

export function meter(d: ProgressData): Meter {
  const fns: Fn[] = d.component.map((c, i) => ({
    index: d.first + i,
    component: d.components[c],
    verdict: VERDICTS[d.verdict[i]],
    size: d.size[i],
    name: d.name[i],
  }));
  const byComponent = new Map<string, ComponentRow>();
  for (const f of fns) {
    let row = byComponent.get(f.component);
    if (row === undefined) {
      row = { id: f.component, origin: originOf(f.component), description: describe(f.component), tally: emptyTally() };
      byComponent.set(f.component, row);
    }
    const v = VERDICTS.indexOf(f.verdict);
    row.tally.fns[v]++;
    row.tally.bytes[v] += f.size;
  }
  const byOrigin = new Map<OriginId, Tally>(ORIGINS.map((o) => [o.id, emptyTally()]));
  const all = emptyTally();
  for (const row of byComponent.values()) {
    addTally(byOrigin.get(row.origin)!, row.tally);
    addTally(all, row.tally);
  }
  return { fns, byComponent, byOrigin, all };
}

/** An origin's components: TSE's and the unattributed ones in status.py's order, libraries by matches
 *  (status.py's order for them too). */
export function componentsOf(m: Meter, origin: OriginId): ComponentRow[] {
  const fixed = origin === 'tse' ? TSE_COMPONENTS : origin === 'unattributed' ? NOT_ATTRIBUTED : null;
  if (fixed !== null) {
    return fixed.map(([c]) => m.byComponent.get(c)).filter((r): r is ComponentRow => r !== undefined);
  }
  return [...m.byComponent.values()]
    .filter((r) => r.origin === origin)
    .sort((a, b) => b.tally.fns[0] - a.tally.fns[0] || (a.id < b.id ? -1 : 1));
}

/** `cells` squares split by verdict in proportion to `values`, by largest remainder: the counts add up to
 *  `cells`, and a verdict with any share at all gets a square only when its share rounds to one. */
export function waffleCells(values: readonly number[], cells = 100): number[] {
  const sum = values.reduce((a, b) => a + b, 0);
  if (sum === 0) {
    return values.map((_, i) => (i === values.length - 1 ? cells : 0));
  }
  const exact = values.map((v) => (v * cells) / sum);
  const out = exact.map(Math.floor);
  let left = cells - out.reduce((a, b) => a + b, 0);
  const order = exact.map((e, i) => [e - Math.floor(e), i] as const).sort((a, b) => b[0] - a[0] || a[1] - b[1]);
  for (const [, i] of order) {
    if (left === 0) {
      break;
    }
    out[i]++;
    left--;
  }
  return out;
}

/** A history point's matched functions or bytes over a set of components. */
export function matchedAt(p: ProgressPoint, componentIds: readonly number[], m: Metric): number {
  const k = m === 'functions' ? 0 : 1;
  let s = 0;
  for (const c of componentIds) {
    s += p.matched[c][k];
  }
  return s;
}

export const pct = (x: number, digits = 2): string => `${(100 * x).toFixed(digits)}%`;
export const num = (x: number): string => x.toLocaleString('en-US');
