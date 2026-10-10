// The benchmark's runs as the page shows them: newest first, one summary row per compile setup, and
// what moved between two runs, by the rule `pnpm bench diff` uses (a row moved when its outcome or
// its -Oz score changed).
import type { BenchResults, DatasetRow, Outcome, RowResult, Setup, Totals } from '@asmlift/wasm-benchmark/schema';

/** Runs newest first; equal stamps by label. */
export function sortRuns(runs: readonly BenchResults[]): BenchResults[] {
  return [...runs].sort((a, b) => b.createdAt.localeCompare(a.createdAt) || a.label.localeCompare(b.label));
}

/** `benchmark/results/<label>.json` paths (as `import.meta.glob` keys them) → the label. */
export function labelOf(path: string): string {
  const base = path.slice(path.lastIndexOf('/') + 1);
  return base.endsWith('.json') ? base.slice(0, -5) : base;
}

export const FAILURES: Outcome[] = ['lift-threw', 'compile-failed', 'extract-failed', 'link-failed'];

export function failures(t: Totals): number {
  return FAILURES.reduce((n, o) => n + (t.outcomes[o] ?? 0), 0);
}

export interface SetupSummary {
  key: string;
  title: string;
  totals: Totals;
}

/** One entry per setup the run has rows for, in the registry's order, then any setup the registry lacks. */
export function setupSummaries(run: BenchResults, setups: readonly Setup[]): SetupSummary[] {
  const known = setups.filter((s) => run.totals.bySetup[s.key] !== undefined);
  const extra = Object.keys(run.totals.bySetup).filter((k) => !setups.some((s) => s.key === k));
  return [
    ...known.map((s) => ({ key: s.key, title: s.title, totals: run.totals.bySetup[s.key] })),
    ...extra.map((k) => ({ key: k, title: k, totals: run.totals.bySetup[k] })),
  ];
}

const optKey = (x: RowResult): number => x.opt?.score ?? Infinity;

export interface MovedRow {
  id: string;
  before: RowResult | undefined;
  after: RowResult | undefined;
  /** `better`/`worse` by the -Oz score; `changed` when only the outcome moved or the row is in one run */
  direction: 'better' | 'worse' | 'changed';
}

export interface RunDiff {
  sameDataset: boolean;
  moved: MovedRow[];
  better: number;
  worse: number;
}

export function diffRuns(before: BenchResults, after: BenchResults): RunDiff {
  const a = new Map(before.rows.map((r) => [r.id, r]));
  const b = new Map(after.rows.map((r) => [r.id, r]));
  const moved: MovedRow[] = [];
  for (const id of new Set([...a.keys(), ...b.keys()])) {
    const x = a.get(id);
    const y = b.get(id);
    if (x !== undefined && y !== undefined && x.outcome === y.outcome && optKey(x) === optKey(y)) {
      continue;
    }
    const direction =
      x === undefined || y === undefined || optKey(x) === optKey(y)
        ? 'changed'
        : optKey(y) < optKey(x)
          ? 'better'
          : 'worse';
    moved.push({ id, before: x, after: y, direction });
  }
  moved.sort((p, q) => p.id.localeCompare(q.id));
  return {
    sameDataset: before.dataset.rowsSha256 === after.dataset.rowsSha256,
    moved,
    better: moved.filter((m) => m.direction === 'better').length,
    worse: moved.filter((m) => m.direction === 'worse').length,
  };
}

/** The dataset rows of a function index (a function can be a row of more than one setup). */
export function rowsOfIndex(rows: readonly DatasetRow[], index: number): DatasetRow[] {
  return rows.filter((r) => r.index === index);
}

/** Dataset rows grouped by setup, in the registry's order, each group by index. */
export function rowsBySetup(rows: readonly DatasetRow[], setups: readonly Setup[]): { setup: Setup; rows: DatasetRow[] }[] {
  return setups
    .map((setup) => ({ setup, rows: rows.filter((r) => r.setup === setup.key).sort((p, q) => p.index - q.index) }))
    .filter((g) => g.rows.length > 0);
}

/** `path:line` of a row's reference. */
export function referenceText(row: DatasetRow): string | null {
  const r = row.reference;
  if (r === null) {
    return null;
  }
  return `${r.path}${r.line === undefined ? '' : `:${r.line}`}`;
}

export function ratioText(x: number | null | undefined): string {
  return x === null || x === undefined ? '–' : x.toFixed(3);
}
