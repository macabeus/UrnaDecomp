import { readFileSync, readdirSync } from 'node:fs';
import { join } from 'node:path';
import { describe, expect, it } from 'vitest';

import { renderDiff } from '@asmlift/wasm-benchmark';
import type { BenchResults, DatasetRow, Setup } from '@asmlift/wasm-benchmark/schema';

import { URNA_ROOT } from '../scripts/data';
import { diffRuns, labelOf, referenceText, rowsBySetup, setupSummaries, sortRuns } from '../src/lib/bench';

const BENCH = join(URNA_ROOT, 'asmlift-wasm/benchmark');
const read = <T>(p: string): T => JSON.parse(readFileSync(join(BENCH, p), 'utf8')) as T;
const setups = read<Setup[]>('dataset/setups.json');
const rows = read<DatasetRow[]>('dataset/rows.json');
const baseline = read<BenchResults>('results/baseline.json');

describe('setupSummaries', () => {
  it('lists every setup of the baseline in the registry order, their rows adding up to the run', () => {
    const s = setupSummaries(baseline, setups);
    expect(s.map((x) => x.key)).toEqual(setups.map((x) => x.key));
    expect(s.reduce((n, x) => n + x.totals.rows, 0)).toBe(baseline.totals.overall.rows);
  });
});

describe('rowsBySetup', () => {
  it('puts each of the 190 dataset rows under its setup', () => {
    const groups = rowsBySetup(rows, setups);
    expect(groups.reduce((n, g) => n + g.rows.length, 0)).toBe(rows.length);
    expect(groups.length).toBe(17);
  });
});

describe('diffRuns', () => {
  it('reports nothing between a run and itself', () => {
    const d = diffRuns(baseline, baseline);
    expect(d).toEqual({ sameDataset: true, moved: [], better: 0, worse: 0 });
  });

  it('counts better and worse rows as pnpm bench diff does', () => {
    const differs = baseline.rows.filter((r) => r.outcome === 'differs');
    const [down, up] = [differs[0], differs[1]];
    const after: BenchResults = {
      ...baseline,
      label: 'after',
      rows: baseline.rows.map((r) =>
        r.id === down.id
          ? { ...r, opt: { score: 0, ratio: 0 }, exact: true, outcome: 'exact' as const }
          : r.id === up.id
            ? { ...r, opt: { score: r.opt!.score + 3, ratio: r.opt!.ratio } }
            : r,
      ),
    };
    const d = diffRuns(baseline, after);
    expect(d.moved.map((m) => [m.id, m.direction])).toEqual(
      [
        [down.id, 'better'],
        [up.id, 'worse'],
      ].sort((a, b) => a[0].localeCompare(b[0])),
    );
    expect(renderDiff(baseline, after)).toContain(`${d.better} rows scored better at -Oz, ${d.worse} worse, 0 changed otherwise`);
  });
});

describe('sortRuns', () => {
  it('puts the newest run first', () => {
    const older = { ...baseline, label: 'older', createdAt: '2000-01-01T00:00:00.000Z' };
    expect(sortRuns([older, baseline]).map((r) => r.label)).toEqual([baseline.label, 'older']);
  });
});

describe('labelOf', () => {
  it('reads the label off every results file name', () => {
    for (const f of readdirSync(join(BENCH, 'results')).filter((f) => f.endsWith('.json'))) {
      expect(labelOf(`../../benchmark/results/${f}`)).toBe(read<BenchResults>(`results/${f}`).label);
    }
  });
});

describe('referenceText', () => {
  it('writes a reference as path:line', () => {
    const withLine = rows.find((r) => r.reference?.line !== undefined)!;
    expect(referenceText(withLine)).toBe(`${withLine.reference!.path}:${withLine.reference!.line}`);
    expect(referenceText(rows.find((r) => r.reference === null)!)).toBeNull();
  });
});
