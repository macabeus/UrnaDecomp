// One benchmark run: every dataset row is lifted, compiled at its setup's flags and scored by the
// toolkit's lift evaluation (`evaluateLift`: one container compiles every row, each object is
// linked alone and put through the target link's wasm-opt -Oz passes, both are scored against the
// target at depth 0 with canonicalised rows), and the outcome is written per row with totals per setup.
import { spawnSync } from 'node:child_process';
import { createHash } from 'node:crypto';
import { existsSync, mkdirSync, readFileSync, rmSync } from 'node:fs';
import { join } from 'node:path';

import { type EvalRow, evaluateLift, importResolver, glueNamesFor, liftInputs } from '@asmlift/wasm';

import { liftCompileFlags } from './flags';
import { ROWS_JSON, SETUPS_JSON, TARGET, URNA_ROOT, WORK_DIR } from './paths';
import type { BenchResults, DatasetRow, Outcome, RowResult, Totals } from './schema';

function sha256(path: string): string {
  return createHash('sha256').update(readFileSync(path)).digest('hex');
}

/** The urna commit, `+dirty` when the tree has uncommitted changes; null outside git. */
function commit(): string | null {
  const head = spawnSync('git', ['-C', URNA_ROOT, 'rev-parse', '--short', 'HEAD'], { encoding: 'utf8' });
  if (head.status !== 0) {
    return null;
  }
  const dirty = spawnSync('git', ['-C', URNA_ROOT, 'status', '--porcelain', '--untracked-files=no'], {
    encoding: 'utf8',
  });
  return `${head.stdout.trim()}${dirty.stdout.trim() === '' ? '' : '+dirty'}`;
}

function firstLine(path: string): string {
  if (!existsSync(path)) {
    return '';
  }
  const lines = readFileSync(path, 'utf8').split('\n');
  return (lines.find((l) => /error/i.test(l)) ?? lines.find((l) => l.trim() !== '') ?? '').trim();
}

export function rowResult(row: DatasetRow, e: EvalRow, dir: string): RowResult {
  let outcome: Outcome;
  let failure: RowResult['failure'] = null;
  if (e.liftError !== undefined) {
    outcome = 'lift-threw';
    failure = { class: 'lift threw', line: e.liftError };
  } else if (!e.compiled) {
    outcome = e.failure?.cls === 'extract failed' ? 'extract-failed' : 'compile-failed';
    failure = { class: e.failure?.cls ?? 'compile failed', line: e.failure?.line ?? '' };
  } else if (e.optScore === undefined) {
    outcome = 'link-failed';
    failure = { class: 'link or wasm-opt failed', line: firstLine(join(dir, `${row.index}.cpp.opt.err`)) };
  } else {
    outcome = e.optScore === 0 ? 'exact' : 'differs';
  }
  return {
    id: row.id,
    index: row.index,
    setup: row.setup,
    size: row.size,
    band: row.band,
    features: row.features,
    lifted: e.liftError === undefined,
    compiled: e.compiled,
    targetRows: e.targetRows,
    raw: e.score === undefined ? null : { score: e.score, ratio: round(e.ratio!) },
    opt: e.optScore === undefined ? null : { score: e.optScore, ratio: round(e.optRatio!) },
    exact: e.optScore === 0,
    outcome,
    failure,
    standIns: [...new Set(e.notes)].sort(),
  };
}

const round = (x: number): number => Math.round(x * 10000) / 10000;

function median(xs: number[]): number | null {
  if (xs.length === 0) {
    return null;
  }
  const s = [...xs].sort((a, b) => a - b);
  const mid = s.length >> 1;
  return round(s.length % 2 === 1 ? s[mid] : (s[mid - 1] + s[mid]) / 2);
}

export function totals(rows: readonly RowResult[]): Totals {
  const opt = rows.filter((r) => r.opt !== null).map((r) => r.opt!.ratio);
  const outcomes: Partial<Record<Outcome, number>> = {};
  for (const r of rows) {
    outcomes[r.outcome] = (outcomes[r.outcome] ?? 0) + 1;
  }
  return {
    rows: rows.length,
    lifted: rows.filter((r) => r.lifted).length,
    compiled: rows.filter((r) => r.compiled).length,
    optimised: opt.length,
    exactRaw: rows.filter((r) => r.raw?.score === 0).length,
    exact: rows.filter((r) => r.exact).length,
    medianRatio: median(rows.filter((r) => r.raw !== null).map((r) => r.raw!.ratio)),
    medianOptRatio: median(opt),
    meanOptRatio: opt.length === 0 ? null : round(opt.reduce((a, b) => a + b, 0) / opt.length),
    outcomes,
  };
}

export function totalsBySetup(rows: readonly RowResult[]): Record<string, Totals> {
  const keys = [...new Set(rows.map((r) => r.setup))];
  return Object.fromEntries(keys.map((k) => [k, totals(rows.filter((r) => r.setup === k))]));
}

export interface RunOptions {
  label: string;
  rows: readonly DatasetRow[];
  image: string;
  log?: (line: string) => void;
}

export function runBench(opts: RunOptions): BenchResults {
  const log = opts.log ?? ((l: string) => process.stderr.write(`${l}\n`));
  const t0 = Date.now();
  const dir = join(WORK_DIR, opts.label, 'lifted');
  if (existsSync(dir)) {
    rmSync(dir, { recursive: true });
  }
  mkdirSync(dir, { recursive: true });
  const { m, names, mangled, attribution } = liftInputs(TARGET, {}, log);
  const glue = glueNamesFor(TARGET);
  log(`bench ${opts.label}: ${opts.rows.length} rows → ${dir}`);
  const evaluated = evaluateLift(
    m,
    opts.rows.map((r) => ({
      index: r.index,
      component: r.setup,
      band: r.band,
      size: r.size,
      flags: liftCompileFlags(r.flags),
    })),
    {
      lift: { names, ...(mangled ? { mangled } : {}) },
      attribution,
      dir,
      image: opts.image,
      importName: importResolver(glue?.imports ?? null),
    },
  );
  const byIndex = new Map(evaluated.rows.map((e) => [e.index, e]));
  const rows = opts.rows.map((r) => rowResult(r, byIndex.get(r.index)!, dir));
  return {
    schema: 1,
    label: opts.label,
    createdAt: new Date().toISOString(),
    target: { path: 'target/vota_web_wasm.wasm', sha256: sha256(TARGET) },
    image: opts.image,
    dataset: { rows: rows.length, rowsSha256: sha256(ROWS_JSON), setupsSha256: sha256(SETUPS_JSON) },
    commit: commit(),
    durationMs: { lift: evaluated.liftMs, compileAndOptimise: evaluated.compileMs, total: Date.now() - t0 },
    totals: { overall: totals(rows), bySetup: totalsBySetup(rows) },
    rows,
  };
}
