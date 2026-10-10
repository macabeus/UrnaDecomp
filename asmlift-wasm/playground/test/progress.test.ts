import { execFileSync } from 'node:child_process';
import { readFileSync } from 'node:fs';
import { join } from 'node:path';

import { describe, expect, it } from 'vitest';

import { URNA_ROOT } from '../scripts/data';
import { PROGRESS_INPUTS, distillProgress, earlierMatches, parseMatches, readScan } from '../scripts/progress';
import {
  NOT_ATTRIBUTED,
  ORIGINS,
  type Tally,
  TSE_COMPONENTS,
  componentsOf,
  matchedAt,
  meter,
  waffleCells,
} from '../src/lib/progress';
import type { ProgressData } from '../src/lib/progress-data';

let data: ProgressData | null = null;
const progress = (): ProgressData => (data ??= distillProgress());

/** The four numbers of a status.py table row: matched / total functions, matched / total code bytes. */
function statusRow(text: string, head: string): number[] {
  const line = text.split('\n').find((l) => l.startsWith(`| ${head} |`));
  if (line === undefined) {
    throw new Error(`docs/status.md: no row ${head}`);
  }
  const m = /\| \**([\d,]+) \/ ([\d,]+)\** \| \**[\d.]+%\** \| \**([\d,]+) \/ ([\d,]+)\** \| \**[\d.]+%\** \|$/.exec(line);
  if (m === null) {
    throw new Error(`docs/status.md: row ${head} has no counts: ${line}`);
  }
  return m.slice(1).map((x) => Number(x.replace(/,/g, '')));
}

const counts = (t: Tally): number[] => [t.fns[0], t.fns.reduce((a, b) => a + b), t.bytes[0], t.bytes.reduce((a, b) => a + b)];

describe('the progress meter', () => {
  const status = readFileSync(join(URNA_ROOT, 'docs/status.md'), 'utf8');

  it('sums every origin as status.py does for docs/status.md', () => {
    const m = meter(progress());
    const heads: Record<string, string> = { tse: 'TSE code', upstream: 'upstream libraries', unattributed: 'not attributed' };
    for (const o of ORIGINS) {
      expect(counts(m.byOrigin.get(o.id)!), o.id).toEqual(statusRow(status, heads[o.id]));
    }
    expect(counts(m.all)).toEqual(statusRow(status, '**total**'));
  });

  it('sums every component as status.py does, and puts it in the origin status.py does', () => {
    const m = meter(progress());
    for (const row of m.byComponent.values()) {
      expect(counts(row.tally), row.id).toEqual(statusRow(status, `\`${row.id}\``));
    }
    expect(componentsOf(m, 'tse').map((r) => r.id)).toEqual(TSE_COMPONENTS.map(([c]) => c));
    expect(componentsOf(m, 'unattributed').map((r) => r.id)).toEqual(NOT_ATTRIBUTED.map(([c]) => c));
    const listed = ORIGINS.flatMap((o) => componentsOf(m, o.id).map((r) => r.id)).sort();
    expect(listed).toEqual([...m.byComponent.keys()].sort());
  });

  it('matches exactly the functions of splat/matched.txt', () => {
    const m = meter(progress());
    const matched = m.fns.filter((f) => f.verdict === 'match').map((f) => f.index);
    expect(matched).toEqual([...new Set(parseMatches(readFileSync(PROGRESS_INPUTS.matches, 'utf8')))].sort((a, b) => a - b));
  });

  it('keeps one row per scan row, and names every matched function', () => {
    const d = progress();
    const rows = readScan();
    expect(d.component.length).toBe(rows.length);
    rows.forEach((r, i) => {
      expect(d.first + i).toBe(r.index);
      expect(d.size[i]).toBe(r.size);
      if (r.verdict === 'match') {
        expect(d.name[i], `f${r.index}`).not.toBe('');
      }
    });
  });
});

describe('the progress history', () => {
  it('ends at the matches of HEAD', () => {
    const d = progress();
    const head = parseMatches(execFileSync('git', ['-C', URNA_ROOT, 'show', 'HEAD:splat/matched.txt'], { encoding: 'utf8' }));
    const last = d.history[d.history.length - 1];
    const all = d.components.map((_, i) => i);
    expect(matchedAt(last, all, 'functions')).toBe(new Set(head).size);
    const m = meter(d);
    const byIndex = new Map(m.fns.map((f) => [f.index, f]));
    expect(matchedAt(last, all, 'bytes')).toBe([...new Set(head)].reduce((s, i) => s + byIndex.get(i)!.size, 0));
  });

  it('is in date order, and its earlier points replay to 7,215 matches', () => {
    const d = progress();
    const dates = d.history.map((p) => p.date);
    expect(dates).toEqual([...dates].sort());
    const seed = earlierMatches();
    expect([...seed[seed.length - 1].matches].length).toBe(7215);
    expect(d.history.slice(0, seed.length).every((p) => p.commit === null)).toBe(true);
  });
});

describe('waffleCells', () => {
  it('splits 100 squares by largest remainder', () => {
    expect(waffleCells([7211, 401, 160, 6571])).toEqual([50, 3, 1, 46]);
    expect(waffleCells([1, 1, 1])).toEqual([34, 33, 33]);
    expect(waffleCells([0, 0, 0, 0])).toEqual([0, 0, 0, 100]);
    for (const vs of [[8, 9, 1, 762], [3, 2, 3, 3], [5513, 300, 50, 2436]]) {
      expect(waffleCells(vs).reduce((a, b) => a + b)).toBe(100);
    }
  });
});
