// The harness end to end on a few real rows (through the pinned image), and the committed baseline.
import { createHash } from 'node:crypto';
import { readFileSync } from 'node:fs';
import { join } from 'node:path';
import { describe, expect, it } from 'vitest';

import { readRows } from '../src/dataset';
import { RESULTS_DIR, ROWS_JSON } from '../src/paths';
import { renderBenchReport, renderDiff } from '../src/report';
import { runBench, totals, totalsBySetup } from '../src/run';
import type { BenchResults } from '../src/schema';
import { loadSetups } from '../src/setups';
import { IMAGE, hasImage } from './helpers';

const baseline = JSON.parse(readFileSync(join(RESULTS_DIR, 'baseline.json'), 'utf8')) as BenchResults;

describe.skipIf(!hasImage)('runBench', () => {
  it('lifts, compiles at each setup flags, links and scores a C++, a C and a system-library row', () => {
    const rows = readRows().filter((r) => ['uenux2/10252', 'zlib/4824', 'libc-optz/504'].includes(r.id));
    expect(rows).toHaveLength(3);
    const r = runBench({ label: 'test-run', rows, image: IMAGE, log: () => {} });
    expect(r.rows.map((x) => x.id)).toEqual(rows.map((x) => x.id));
    for (const x of r.rows) {
      expect(x.compiled, x.id).toBe(true);
      expect(x.opt, x.id).not.toBeNull();
      expect(x.outcome === 'exact' || x.outcome === 'differs').toBe(true);
    }
    expect(r.totals.overall).toEqual(totals(r.rows));
    expect(Object.keys(r.totals.bySetup).sort()).toEqual(['libc-optz', 'uenux2', 'zlib']);
  });
});

describe('baseline.json', () => {
  it('was run on the committed rows.json', () => {
    expect(baseline.dataset.rowsSha256).toBe(createHash('sha256').update(readFileSync(ROWS_JSON)).digest('hex'));
  });

  it('has one result per dataset row', () => {
    expect(baseline.rows.map((r) => r.id).sort()).toEqual(readRows().map((r) => r.id).sort());
  });

  it('holds totals that agree with its rows', () => {
    expect(baseline.totals.overall).toEqual(totals(baseline.rows));
    expect(baseline.totals.bySetup).toEqual(totalsBySetup(baseline.rows));
  });
});

describe('renderBenchReport', () => {
  it('writes a table row for every setup with its exact count', () => {
    const md = renderBenchReport(baseline);
    // the stand-ins' flag set owns no row, so no table row
    for (const s of loadSetups().filter((x) => x.truth.kind !== 'stand-ins')) {
      expect(md).toContain(`| \`${s.key}\` | ${baseline.totals.bySetup[s.key].rows} |`);
    }
  });
});

describe('renderDiff', () => {
  it('counts a row whose -Oz score dropped as better and lists it', () => {
    const after = structuredClone(baseline);
    after.label = 'after';
    const moved = after.rows.find((r) => r.opt !== null && r.opt.score > 1)!;
    moved.opt = { score: 0, ratio: 0 };
    moved.exact = true;
    moved.outcome = 'exact';
    const md = renderDiff(baseline, after);
    expect(md).toContain('1 rows scored better at -Oz, 0 worse');
    expect(md).toContain(`| ${moved.id} |`);
  });

  it('lists nothing when a run is compared with itself', () => {
    expect(renderDiff(baseline, baseline)).toContain('0 rows scored better at -Oz, 0 worse, 0 changed');
  });
});
