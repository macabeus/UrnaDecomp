// The lifter's measurement harness: a fixed, stratified sample of target functions is lifted,
// compiled with em++ in the pinned image at each function's component flags (one container for
// the whole sample), pulled out of its object by symbol and scored against the target function at
// depth 0. Each object is also linked alone and put through the target link's wasm-opt passes
// (`optimizeLifted`) and scored again: the target went through wasm-opt -Oz, an object did not, so
// the optimised score is the one that tells a lift difference from an optimiser difference. The
// report counts lift throws, compiles, exact rows and the median diff ratio on both scores, per
// component, and clusters what went wrong: compile errors into failure classes, and the differing
// rows of the optimised functions into gap classes.
//
// An object and the target disagree on spellings that say nothing about the lift, so both sides'
// rows pass through `canonRow` first: a callee is a relocation symbol on one side and `fn:?` on the
// other, a data address is a reloc symbol, a raw constant or a segment hash depending on the side,
// and the stack pointer is `global:__stack_pointer` in an object and `0` in the target.
import { existsSync, readFileSync, writeFileSync } from 'node:fs';
import { join } from 'node:path';

import type { Attribution } from './ledger';
import { type LiftOptions, componentFlags, liftFunction } from './lift';
import { type NormalizeOptions, normalizeModule, signature } from './normalize';
import { extractFunction } from './object';
import { type Module, funcAt, parseModule } from './parse';
import { DEFAULT_IMAGE, compileLifted, optimizeLifted } from './rank';
import { type Score, score } from './score';

/** Body-size bands in bytes, upper bound inclusive. */
export const SIZE_BANDS: readonly { name: string; max: number }[] = [
  { name: 'tiny', max: 32 },
  { name: 'small', max: 128 },
  { name: 'medium', max: 512 },
  { name: 'large', max: 2048 },
  { name: 'huge', max: Infinity },
];

export function sizeBand(size: number): string {
  return SIZE_BANDS.find((b) => size <= b.max)!.name;
}

export interface SampleRow {
  index: number;
  component: string;
  band: string;
  size: number;
  /** the em++ flags to compile this function's lift with; the component's (`componentFlags`) without */
  flags?: string;
}

/** The ledger's component, `unknown` where it has none. */
export function componentOf(attribution: ReadonlyMap<number, Attribution> | null, index: number): string {
  const c = attribution?.get(index)?.component;
  return c === undefined || c === '-' ? 'unknown' : c;
}

function mulberry32(seed: number): () => number {
  let a = seed >>> 0;
  return () => {
    a = (a + 0x6d2b79f5) >>> 0;
    let t = a;
    t = Math.imul(t ^ (t >>> 15), t | 1);
    t ^= t + Math.imul(t ^ (t >>> 7), t | 61);
    return ((t ^ (t >>> 14)) >>> 0) / 4294967296;
  };
}

function shuffle<T>(xs: T[], rnd: () => number): T[] {
  for (let i = xs.length - 1; i > 0; i--) {
    const j = Math.floor(rnd() * (i + 1));
    [xs[i], xs[j]] = [xs[j], xs[i]];
  }
  return xs;
}

export interface SampleOptions {
  size?: number;
  seed?: number;
  /** the fewest functions a component gets (all of them when it has fewer) */
  minPerComponent?: number;
}

/** A deterministic sample stratified by component and size band. Each component's share is
 *  proportional to the square root of its function count (so small libraries are not drowned by
 *  the large ones), at least `minPerComponent`; within a component the bands take turns, smallest
 *  first, each band's functions in seeded random order. Sorted by index. */
export function sampleFunctions(
  m: Module,
  attribution: ReadonlyMap<number, Attribution> | null,
  opts: SampleOptions = {},
): SampleRow[] {
  const size = opts.size ?? 150;
  const rnd = mulberry32(opts.seed ?? 1);
  const min = opts.minPerComponent ?? 3;
  const byComponent = new Map<string, SampleRow[]>();
  for (const f of m.funcs) {
    const component = componentOf(attribution, f.index);
    const row = { index: f.index, component, band: sizeBand(f.size), size: f.size };
    const list = byComponent.get(component);
    if (list === undefined) {
      byComponent.set(component, [row]);
    } else {
      list.push(row);
    }
  }
  const components = [...byComponent.keys()].sort();
  // largest-remainder apportionment of `size` by sqrt(count), then raised to the floor
  const weight = new Map(components.map((c) => [c, Math.sqrt(byComponent.get(c)!.length)]));
  const total = [...weight.values()].reduce((a, b) => a + b, 0);
  const quota = new Map<string, number>();
  const remainders: [string, number][] = [];
  let given = 0;
  for (const c of components) {
    const exact = (size * weight.get(c)!) / total;
    quota.set(c, Math.floor(exact));
    given += Math.floor(exact);
    remainders.push([c, exact - Math.floor(exact)]);
  }
  remainders.sort((a, b) => b[1] - a[1] || a[0].localeCompare(b[0]));
  for (let k = 0; given < size && k < remainders.length; k++, given++) {
    quota.set(remainders[k][0], quota.get(remainders[k][0])! + 1);
  }
  const out: SampleRow[] = [];
  for (const c of components) {
    const rows = byComponent.get(c)!;
    const want = Math.min(rows.length, Math.max(min, quota.get(c)!));
    const bands = SIZE_BANDS.map((b) =>
      shuffle(
        rows.filter((r) => r.band === b.name),
        rnd,
      ),
    );
    let taken = 0;
    for (let round = 0; taken < want; round++) {
      for (const band of bands) {
        if (taken < want && round < band.length) {
          out.push(band[round]);
          taken++;
        }
      }
    }
  }
  return out.sort((a, b) => a.index - b.index);
}

const SAMPLE_HEADER = 'index\tcomponent\tband\tsize';

export function renderSampleTsv(rows: readonly SampleRow[]): string {
  return `${[SAMPLE_HEADER, ...rows.map((r) => `${r.index}\t${r.component}\t${r.band}\t${r.size}`)].join('\n')}\n`;
}

export function readSampleTsv(path: string): SampleRow[] {
  const lines = readFileSync(path, 'utf8').split('\n');
  return lines
    .slice(1)
    .filter((l) => l !== '')
    .map((l) => {
      const [index, component, band, size] = l.split('\t');
      return { index: Number(index), component, band, size: Number(size) };
    });
}

/** One row with every spelling that differs between an object and the target for reasons other
 *  than the code: callees, data addresses (a constant or memarg offset ≥ 1024, emscripten's
 *  GLOBAL_BASE, or a data symbol), the stack-pointer global, the indirect table, and
 *  memory.copy/fill's memory indices. */
export function canonRow(row: string): string {
  if (/^(call|return_call|ref\.func) /.test(row)) {
    return row.slice(0, row.indexOf(' '));
  }
  if (/^memory\.(copy|fill)\b/.test(row)) {
    return row.split(' ')[0];
  }
  return row
    .replace(/ R_WASM_TABLE_NUMBER_LEB:\S+/, '')
    .replace(/global:__stack_pointer\b/, '0')
    .replace(/\bdata:\S+/g, '<addr>')
    .replace(/^(i32\.const|i64\.const) (\d+)$/, (s, op: string, n: string) => (Number(n) >= 1024 ? `${op} <addr>` : s))
    .replace(/offset=(\d+)/, (s, n: string) => (Number(n) >= 1024 ? 'offset=<addr>' : s));
}

/** A first-error class for em++'s output: the diagnostic text with quoted names and numbers
 *  blanked, so `use of undeclared identifier 'x'` and `… 'y'` are one class. */
export function errorClass(stderr: string): { cls: string; line: string } {
  const lines = stderr.split('\n');
  const line = lines.find((l) => /\berror: /.test(l)) ?? lines.find((l) => l.trim() !== '') ?? '';
  if (line === '') {
    return { cls: 'no diagnostic (compiler produced no object and no message)', line };
  }
  const m = /\berror: (.*)$/.exec(line);
  const text = m === null ? line.trim() : m[1];
  const cls = text
    .replace(/'[^']*'/g, "'…'")
    .replace(/"[^"]*"/g, '"…"')
    .replace(/\b\d+\b/g, 'N');
  return { cls, line: line.trim() };
}

const FAMILIES: [RegExp, string][] = [
  [/^;;/, 'signature'],
  [/^local\./, 'local'],
  [/^global\./, 'global'],
  [/^(block|loop|if|else|end|br|br_if|br_table|return|unreachable|nop|try|catch|delegate|throw|rethrow)$/, 'control'],
  [/^(call|call_indirect|return_call|return_call_indirect)$/, 'call'],
  [/^\w+\.const$/, 'const'],
  [/^\w+\.load/, 'load'],
  [/^\w+\.store/, 'store'],
  [/^(select|drop)$/, 'select/drop'],
  [/^memory\./, 'memory'],
  [/^\w+\.(eqz|eq|ne|lt_[su]|gt_[su]|le_[su]|ge_[su]|lt|gt|le|ge)$/, 'compare'],
  [/^\w+\.(extend|wrap|trunc|convert|demote|promote|reinterpret|extend\d+)/, 'convert'],
];

export function opFamily(row: string): string {
  const op = row.split(' ')[0];
  for (const [re, fam] of FAMILIES) {
    if (re.test(op)) {
      return fam;
    }
  }
  return 'arith';
}

/** The gap class of one differing row: `operand <family>` for a replaced row with the same opcode,
 *  `<family> → <family>` for another replacement, `target-only <family>` and `lift-only <family>`
 *  for the unpaired ones. */
export function gapClass(r: Score['diff'][number]): string {
  switch (r.kind) {
    case 'replaced':
      return r.sameOpcode ? `operand ${opFamily(r.left!)}` : `${opFamily(r.left!)} → ${opFamily(r.right!)}`;
    case 'deleted':
      return `target-only ${opFamily(r.left!)}`;
    case 'inserted':
      return `lift-only ${opFamily(r.right!)}`;
    case 'same':
      return 'same';
  }
}

export interface EvalRow extends SampleRow {
  symbol: string;
  targetRows: number;
  liftError?: string;
  notes: string[];
  compiled: boolean;
  /** the failure class: a compile error's, or an extraction's */
  failure?: { cls: string; line: string };
  score?: number;
  rows?: number;
  /** score / target rows */
  ratio?: number;
  /** the diff hit the edit-distance ceiling and paired the rest by position */
  approximate?: boolean;
  /** the score after linking the object alone and running the target link's wasm-opt passes */
  optScore?: number;
  optRatio?: number;
  /** differing rows per gap class */
  gaps?: Map<string, number>;
  /** one concrete differing row pair per gap class */
  gapExamples?: Map<string, string>;
  /** differing rows per gap class and opcode pair (`if ⇢ br_if`) */
  gapOps?: Map<string, Map<string, number>>;
}

export interface EvalOptions {
  lift: Omit<LiftOptions, 'flags' | 'instrs'>;
  attribution: ReadonlyMap<number, Attribution> | null;
  /** where the lifted sources, objects and em++ errors are written (docker must be able to mount it) */
  dir: string;
  image?: string;
  importName?: NormalizeOptions['importName'];
}

export interface EvalResult {
  rows: EvalRow[];
  liftMs: number;
  compileMs: number;
}

/** Lift, compile and score every sampled function. */
export function evaluateLift(m: Module, sample: readonly SampleRow[], opts: EvalOptions): EvalResult {
  const norm = normalizeModule(m, { depth: 0, ...(opts.importName ? { importName: opts.importName } : {}) });
  const t0 = Date.now();
  const rows: EvalRow[] = [];
  const files: string[] = [];
  for (const s of sample) {
    const fn = funcAt(m, s.index);
    if (fn === null) {
      throw new Error(`sample index ${s.index} is not a defined function`);
    }
    const row: EvalRow = {
      ...s,
      symbol: '',
      targetRows: norm[s.index - m.importedFuncCount].rows.length,
      notes: [],
      compiled: false,
    };
    try {
      const lifted = liftFunction(m, s.index, {
        ...opts.lift,
        flags: s.flags ?? componentFlags(opts.attribution?.get(s.index)?.component),
      });
      row.symbol = lifted.symbol;
      row.notes = lifted.notes;
      writeFileSync(join(opts.dir, `${s.index}.cpp`), lifted.source);
      files.push(`${s.index}.cpp`);
    } catch (e) {
      row.liftError = e instanceof Error ? e.message : String(e);
    }
    rows.push(row);
  }
  const t1 = Date.now();
  const failed = new Set(files.length === 0 ? [] : compileLifted(opts.dir, files, opts.image ?? DEFAULT_IMAGE));
  const objects = rows
    .filter(
      (r) =>
        r.liftError === undefined && !failed.has(`${r.index}.cpp`) && existsSync(join(opts.dir, `${r.index}.cpp.o`)),
    )
    .map((r) => ({ file: `${r.index}.cpp`, symbol: r.symbol }));
  if (objects.length > 0) {
    optimizeLifted(opts.dir, objects, opts.image ?? DEFAULT_IMAGE);
  }
  const t2 = Date.now();
  for (const row of rows) {
    if (row.liftError !== undefined) {
      continue;
    }
    const file = join(opts.dir, `${row.index}.cpp`);
    if (failed.has(`${row.index}.cpp`) || !existsSync(`${file}.o`)) {
      row.failure = errorClass(existsSync(`${file}.err`) ? readFileSync(`${file}.err`, 'utf8') : '');
      continue;
    }
    row.compiled = true;
    try {
      const obj = parseModule(new Uint8Array(readFileSync(`${file}.o`)));
      const got = extractFunction(obj, { kind: 'symbol', name: row.symbol });
      const t = norm[row.index - m.importedFuncCount];
      const targetFn = funcAt(m, row.index)!;
      const s = score(
        { rows: t.rows.map(canonRow), signature: signature(targetFn) },
        { rows: got.rows.map(canonRow), signature: got.signature },
      );
      row.score = s.score;
      row.rows = s.rows;
      row.ratio = s.score / Math.max(1, row.targetRows);
      const opt = optimizedScore(join(opts.dir, `${row.index}.cpp.opt.wasm`), row.symbol, t.rows, signature(targetFn));
      if (opt !== null) {
        row.optScore = opt.score;
        row.optRatio = opt.score / Math.max(1, row.targetRows);
      }
      const gapScore = opt ?? s;
      row.approximate = gapScore.breakdown.approximate;
      row.gaps = new Map();
      row.gapExamples = new Map();
      row.gapOps = new Map();
      for (const d of gapScore.diff) {
        if (d.kind === 'same') {
          continue;
        }
        const cls = gapClass(d);
        row.gaps.set(cls, (row.gaps.get(cls) ?? 0) + 1);
        if (!row.gapExamples.has(cls)) {
          row.gapExamples.set(cls, `${d.left ?? '∅'} ⇢ ${d.right ?? '∅'}`);
        }
        const pair = `${d.left?.split(' ')[0] ?? '∅'} ⇢ ${d.right?.split(' ')[0] ?? '∅'}`;
        const ops = row.gapOps.get(cls) ?? new Map<string, number>();
        ops.set(pair, (ops.get(pair) ?? 0) + 1);
        row.gapOps.set(cls, ops);
      }
    } catch (e) {
      row.compiled = false;
      row.failure = { cls: 'extract failed', line: e instanceof Error ? e.message : String(e) };
    }
  }
  return { rows, liftMs: t1 - t0, compileMs: t2 - t1 };
}

/** The score of the function `symbol` exports from the optimised module at `path`, or null when
 *  the link or the optimiser failed. */
function optimizedScore(path: string, symbol: string, targetRows: readonly string[], targetSig: string): Score | null {
  if (!existsSync(path)) {
    return null;
  }
  try {
    const om = parseModule(new Uint8Array(readFileSync(path)));
    const e = om.exports.find((x) => x.kind === 'func' && x.name === symbol);
    const fn = e === undefined ? null : funcAt(om, e.index);
    if (fn === null) {
      return null;
    }
    const rows = normalizeModule(om, { depth: 0 })[fn.index - om.importedFuncCount].rows;
    return score(
      { rows: targetRows.map(canonRow), signature: targetSig },
      { rows: rows.map(canonRow), signature: signature(fn) },
    );
  } catch {
    return null;
  }
}

export interface Summary {
  n: number;
  liftThrows: number;
  compiled: number;
  compileRate: number;
  exact: number;
  medianRatio: number | null;
  /** functions whose optimised score is 0 */
  exactOpt: number;
  /** the median optimised ratio, over the functions that have one */
  medianOptRatio: number | null;
}

function median(xs: number[]): number | null {
  if (xs.length === 0) {
    return null;
  }
  const s = [...xs].sort((a, b) => a - b);
  const mid = s.length >> 1;
  return s.length % 2 === 1 ? s[mid] : (s[mid - 1] + s[mid]) / 2;
}

export function summarize(rows: readonly EvalRow[]): Summary {
  const compiled = rows.filter((r) => r.compiled);
  return {
    n: rows.length,
    liftThrows: rows.filter((r) => r.liftError !== undefined).length,
    compiled: compiled.length,
    compileRate: rows.length === 0 ? 0 : compiled.length / rows.length,
    exact: compiled.filter((r) => r.score === 0).length,
    medianRatio: median(compiled.map((r) => r.ratio!)),
    exactOpt: compiled.filter((r) => r.optScore === 0).length,
    medianOptRatio: median(compiled.filter((r) => r.optRatio !== undefined).map((r) => r.optRatio!)),
  };
}

export interface FailureClass {
  cls: string;
  count: number;
  example: string;
  indices: number[];
}

/** Lift throws and compile/extract failures, grouped by class, most frequent first. */
export function failureClasses(rows: readonly EvalRow[]): FailureClass[] {
  const out = new Map<string, FailureClass>();
  for (const r of rows) {
    const f =
      r.liftError !== undefined ? { cls: `lift threw: ${errorClass(r.liftError).cls}`, line: r.liftError } : r.failure;
    if (f === undefined) {
      continue;
    }
    const c = out.get(f.cls) ?? { cls: f.cls, count: 0, example: `${r.index}: ${f.line}`, indices: [] };
    c.count++;
    c.indices.push(r.index);
    out.set(f.cls, c);
  }
  return [...out.values()].sort((a, b) => b.count - a.count || a.cls.localeCompare(b.cls));
}

export interface GapClass {
  cls: string;
  /** differing rows of this class over the sample */
  rows: number;
  /** compiled functions with at least one */
  functions: number;
  /** the class's mean contribution to a compiled function's ratio (its rows over target rows) */
  share: number;
  /** the function with the most rows of this class, and one of its row pairs */
  example: string;
  /** the most frequent opcode pairs of this class, most first */
  ops: [string, number][];
}

export function gapClasses(rows: readonly EvalRow[]): GapClass[] {
  const out = new Map<string, Omit<GapClass, 'ops'> & { top: number; opCounts: Map<string, number> }>();
  const compiled = rows.filter((r) => r.compiled).length;
  for (const r of rows) {
    for (const [cls, n] of r.gaps ?? []) {
      const c = out.get(cls) ?? { cls, rows: 0, functions: 0, share: 0, example: '', top: 0, opCounts: new Map() };
      c.share += n / Math.max(1, r.targetRows) / compiled;
      for (const [pair, k] of r.gapOps?.get(cls) ?? []) {
        c.opCounts.set(pair, (c.opCounts.get(pair) ?? 0) + k);
      }
      c.rows += n;
      c.functions++;
      if (n > c.top) {
        c.top = n;
        c.example = `${r.index} (${n} rows): ${r.gapExamples!.get(cls)}`;
      }
      out.set(cls, c);
    }
  }
  return [...out.values()]
    .sort((a, b) => b.share - a.share || a.cls.localeCompare(b.cls))
    .map(({ top: _top, opCounts, ...c }) => ({
      ...c,
      ops: [...opCounts].sort((a, b) => b[1] - a[1] || a[0].localeCompare(b[0])),
    }));
}

const RESULTS_HEADER =
  'index\tcomponent\tband\tsize\ttarget_rows\tstatus\tscore\trows\tratio\tnotes\tsymbol\tfailure\topt_score\topt_ratio';

function status(r: EvalRow): string {
  if (r.liftError !== undefined) {
    return 'lift-threw';
  }
  if (!r.compiled) {
    return r.failure?.cls === 'extract failed' ? 'extract-failed' : 'compile-failed';
  }
  return r.score === 0 ? 'exact' : 'compiled';
}

export function renderResultsTsv(rows: readonly EvalRow[]): string {
  const cell = (s: string): string => s.replace(/[\t\n]/g, ' ');
  return `${[
    RESULTS_HEADER,
    ...rows.map((r) =>
      [
        r.index,
        r.component,
        r.band,
        r.size,
        r.targetRows,
        status(r),
        r.score ?? '',
        r.rows ?? '',
        r.ratio === undefined ? '' : r.ratio.toFixed(4),
        r.notes.length,
        r.symbol,
        cell(r.liftError ?? r.failure?.line ?? ''),
        r.optScore ?? '',
        r.optRatio === undefined ? '' : r.optRatio.toFixed(4),
      ].join('\t'),
    ),
  ].join('\n')}\n`;
}

export interface PriorRow {
  index: number;
  status: string;
  score: number | null;
  ratio: number | null;
  optScore: number | null;
  optRatio: number | null;
}

export function readResultsTsv(path: string): Map<number, PriorRow> {
  const out = new Map<number, PriorRow>();
  for (const l of readFileSync(path, 'utf8').split('\n').slice(1)) {
    if (l === '') {
      continue;
    }
    const c = l.split('\t');
    out.set(Number(c[0]), {
      index: Number(c[0]),
      status: c[5],
      score: c[6] === '' ? null : Number(c[6]),
      ratio: c[8] === '' ? null : Number(c[8]),
      optScore: (c[12] ?? '') === '' ? null : Number(c[12]),
      optRatio: (c[13] ?? '') === '' ? null : Number(c[13]),
    });
  }
  return out;
}

export interface ReportMeta {
  label: string;
  target: string;
  image: string;
  seed: number | null;
  sampleFrom: string;
  liftMs: number;
  compileMs: number;
  /** a previous run's results, for the movement section */
  against?: { label: string; rows: Map<number, PriorRow> };
}

const pct = (x: number): string => `${(x * 100).toFixed(1)}%`;
const fixed = (x: number | null): string => (x === null ? '–' : x.toFixed(3));

function table(head: string[], body: (string | number)[][]): string[] {
  return [
    `| ${head.join(' | ')} |`,
    `| ${head.map(() => '---').join(' | ')} |`,
    ...body.map((r) => `| ${r.map((c) => String(c).replace(/\|/g, '\\|')).join(' | ')} |`),
  ];
}

export function renderReport(rows: readonly EvalRow[], meta: ReportMeta): string {
  const s = summarize(rows);
  const out: string[] = [];
  out.push(`# Lift evaluation: ${meta.label}`, '');
  out.push(
    "> An isolated `em++ -c` object is LLVM output before wasm-opt, which ran whole-program over the target: no score here can be byte-exact, so the score is a structure signal. The `-Oz` columns score the object linked alone and put through the target link's wasm-opt passes, which takes the optimiser's rewrites out of the gap.",
    '',
  );
  out.push(
    `Target \`${meta.target}\`, image \`${meta.image}\`, ${s.n} functions ` +
      `(${meta.seed === null ? `from ${meta.sampleFrom}` : `seed ${meta.seed}, list in ${meta.sampleFrom}`}). ` +
      `Lift ${meta.liftMs} ms, compile and optimise ${(meta.compileMs / 1000).toFixed(1)} s. Each function is scored at depth 0 ` +
      'against the target with callees, data addresses, the stack-pointer global and the indirect table canonicalised ' +
      'on both sides (`canonRow`); the ratio is differing rows over target rows.',
    '',
  );
  out.push('## Summary', '');
  out.push(
    ...table(
      [
        'sampled',
        'lift throws',
        'compiled',
        'compile rate',
        'exact (score 0)',
        'median ratio',
        'exact -Oz',
        'median ratio -Oz',
      ],
      [
        [
          s.n,
          s.liftThrows,
          s.compiled,
          pct(s.compileRate),
          s.exact,
          fixed(s.medianRatio),
          s.exactOpt,
          fixed(s.medianOptRatio),
        ],
      ],
    ),
    '',
  );
  if (meta.against !== undefined) {
    out.push(...movement(rows, meta.against), '');
  }
  const groups = (key: (r: EvalRow) => string, order: string[]): (string | number)[][] =>
    order.map((k) => {
      const g = rows.filter((r) => key(r) === k);
      const gs = summarize(g);
      return [
        k,
        gs.n,
        gs.liftThrows,
        gs.compiled,
        gs.exact,
        fixed(gs.medianRatio),
        gs.exactOpt,
        fixed(gs.medianOptRatio),
      ];
    });
  const components = [...new Set(rows.map((r) => r.component))].sort();
  out.push('## By component', '');
  out.push(
    ...table(
      ['component', 'n', 'throws', 'compiled', 'exact', 'median ratio', 'exact -Oz', 'median -Oz'],
      groups((r) => r.component, components),
    ),
    '',
  );
  const bandText = SIZE_BANDS.map((b, k) =>
    b.max === Infinity ? `${b.name} > ${SIZE_BANDS[k - 1].max}` : `${b.name} ≤ ${b.max}`,
  ).join(', ');
  out.push(`## By size band (body bytes: ${bandText})`, '');
  out.push(
    ...table(
      ['band', 'n', 'throws', 'compiled', 'exact', 'median ratio', 'exact -Oz', 'median -Oz'],
      groups(
        (r) => r.band,
        SIZE_BANDS.map((b) => b.name).filter((b) => rows.some((r) => r.band === b)),
      ),
    ),
    '',
  );
  const ratios = rows.filter((r) => r.compiled).map((r) => r.optRatio ?? r.ratio!);
  const buckets: [number, number][] = [
    [0, 0],
    [0, 0.1],
    [0.1, 0.25],
    [0.25, 0.5],
    [0.5, 0.75],
    [0.75, 1],
    [1, Infinity],
  ];
  out.push('## Ratio distribution (compiled, -Oz where it linked)', '');
  out.push(
    ...table(
      ['ratio', 'functions'],
      buckets.map(([lo, hi]) => [
        lo === hi ? '0 (exact)' : hi === Infinity ? `> ${lo}` : `(${lo}, ${hi}]`,
        ratios.filter((x) => (lo === hi ? x === 0 : x > lo && x <= hi)).length,
      ]),
    ),
    '',
  );
  const failures = failureClasses(rows);
  out.push('## Failure classes', '');
  if (failures.length === 0) {
    out.push('None: every sampled function lifted, compiled and was found in its object.', '');
  } else {
    out.push(
      ...table(
        ['class', 'count', 'example'],
        failures.map((f) => [f.cls, f.count, `\`${f.example.slice(0, 200)}\``]),
      ),
      '',
    );
  }
  const gaps = gapClasses(rows);
  out.push('## Largest score-gap classes (compiled, -Oz where it linked)', '');
  const approximate = rows.filter((r) => r.approximate).map((r) => r.index);
  out.push(
    'Differing rows of the canonicalised diff, by kind and opcode family: `operand X` is a replaced row whose ' +
      'opcode matches, `A → B` a replaced row, `target-only` a row only the target has, `lift-only` a row only the ' +
      "lifted object has. Ranked by share: the class's rows over target rows, averaged over the compiled functions " +
      '(the shares sum to the mean ratio), so one huge function does not outweigh the sample.' +
      (approximate.length === 0
        ? ''
        : ` ${approximate.length} diff${approximate.length === 1 ? '' : 's'} hit the edit-distance ceiling and ` +
          `paired the rest by position (${approximate.join(', ')}), which inflates the cross-family classes.`),
    '',
  );
  out.push(
    ...table(
      ['class', 'share', 'rows', 'functions', 'top opcode pairs', 'example (function with the most)'],
      gaps.slice(0, 20).map((g) => [
        g.cls,
        g.share.toFixed(3),
        g.rows,
        g.functions,
        g.ops
          .slice(0, 3)
          .map(([p, k]) => `\`${p}\` ${k}`)
          .join(', '),
        `\`${g.example.slice(0, 160)}\``,
      ]),
    ),
    '',
  );
  const worst = rows
    .filter((r) => r.compiled && (r.optScore ?? r.score!) > 0)
    .sort((a, b) => (b.optRatio ?? b.ratio!) - (a.optRatio ?? a.ratio!) || a.index - b.index)
    .slice(0, 15);
  out.push('## Largest ratios (compiled, -Oz where it linked)', '');
  out.push(
    ...table(
      ['index', 'component', 'size', 'target rows', 'score', 'ratio', 'score -Oz', 'ratio -Oz', 'stand-ins'],
      worst.map((r) => [
        r.index,
        r.component,
        r.size,
        r.targetRows,
        r.score!,
        r.ratio!.toFixed(3),
        r.optScore ?? '–',
        r.optRatio === undefined ? '–' : r.optRatio.toFixed(3),
        r.notes.length,
      ]),
    ),
    '',
  );
  const notes = new Map<string, number>();
  for (const r of rows) {
    for (const n of new Set(r.notes)) {
      notes.set(n, (notes.get(n) ?? 0) + 1);
    }
  }
  out.push('## Stand-ins in the lifted sources', '');
  out.push(
    notes.size === 0
      ? 'None.'
      : table(
          ['stand-in', 'functions'],
          [...notes].sort((a, b) => b[1] - a[1]).map(([n, c]) => [n, c]),
        ).join('\n'),
    '',
  );
  return out.join('\n');
}

function movement(rows: readonly EvalRow[], against: { label: string; rows: Map<number, PriorRow> }): string[] {
  let better = 0;
  let worse = 0;
  let same = 0;
  let missing = 0;
  const moves: [number, string, string, number][] = [];
  for (const r of rows) {
    const p = against.rows.get(r.index);
    if (p === undefined) {
      missing++;
      continue;
    }
    // the -Oz scores when both runs have one, else the object scores
    const opt = r.optScore !== undefined && p.optScore !== null;
    const now = r.compiled ? (opt ? r.optScore! : r.score!) : Infinity;
    const was = (opt ? p.optScore : p.score) ?? Infinity;
    if (now < was) {
      better++;
    } else if (now > was) {
      worse++;
    } else {
      same++;
    }
    if (now !== was) {
      moves.push([
        r.index,
        p.score === null ? p.status : `${was}${opt ? ' (-Oz)' : ''}`,
        r.compiled ? `${now}${opt ? ' (-Oz)' : ''}` : status(r),
        was - now,
      ]);
    }
  }
  const prior = [...against.rows.values()];
  const priorCompiled = prior.filter((p) => p.score !== null);
  const priorOpt = prior.filter((p) => p.optRatio !== null);
  const out = ['## Movement against ' + against.label, ''];
  out.push(
    ...table(
      ['', against.label, 'this round'],
      [
        ['compiled', priorCompiled.length, rows.filter((r) => r.compiled).length],
        ['exact', priorCompiled.filter((p) => p.score === 0).length, rows.filter((r) => r.score === 0).length],
        ['median ratio', fixed(median(priorCompiled.map((p) => p.ratio!))), fixed(summarize(rows).medianRatio)],
        [
          'exact -Oz',
          priorOpt.length === 0 ? '–' : priorOpt.filter((p) => p.optScore === 0).length,
          summarize(rows).exactOpt,
        ],
        ['median ratio -Oz', fixed(median(priorOpt.map((p) => p.optRatio!))), fixed(summarize(rows).medianOptRatio)],
      ],
    ),
    '',
  );
  out.push(
    `${better} functions scored better, ${worse} worse, ${same} the same` +
      (missing > 0 ? `; ${missing} were not in ${against.label}'s sample` : '') +
      '.',
  );
  if (moves.length > 0) {
    moves.sort((a, b) => Math.abs(b[3]) - Math.abs(a[3]) || a[0] - b[0]);
    out.push(
      '',
      ...table(
        ['index', against.label, 'this round'],
        moves.slice(0, 15).map(([i, was, now]) => [i, was, now]),
      ),
    );
  }
  return out;
}
