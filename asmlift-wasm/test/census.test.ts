// Fingerprints on the real target: stable, self-matching, and sensitive to what they should be.
import { describe, expect, it } from 'vitest';

import { type CensusFn, census } from '../src/census';
import { dataRanges, dataSymbol, normalizeModule } from '../src/normalize';
import { target } from './helpers';

describe('census', () => {
  const m = target();

  it('matches every function of the target to itself by fingerprint', () => {
    // Each side keeps only its hashes: two whole normalisations of the target do not fit one default heap.
    const hashes = (): CensusFn[] =>
      normalizeModule(m).map(({ index, fingerprint, addr, shape }) => ({ index, fingerprint, addr, shape }));
    // CPU time, not wall time: other builds on the machine stretch the wall clock several times over. The budget is
    // about three times the cost on a laptop, so it catches a change of complexity, not a slower runner.
    const t0 = process.cpuUsage();
    const c = census(m, hashes(), m, hashes());
    expect(c.pairs.length).toBe(14343);
    expect(c.counts.byFingerprint).toBe(14343);
    expect(c.counts.byAddr).toBe(0);
    expect(c.counts.byShape).toBe(0);
    expect(c.unpairedA).toEqual([]);
    expect(c.unpairedB).toEqual([]);
    expect(c.histogram.reduce((s, r) => s + r.total, 0)).toBe(14343);
    const cpu = process.cpuUsage(t0);
    expect((cpu.user + cpu.system) / 1000).toBeLessThan(60_000);
  }, 300_000);

  it('symbolises call targets and data addresses', () => {
    const n = normalizeModule(m, { depth: 1 });
    const f7000 = n[7000 - m.importedFuncCount];
    expect(f7000.rows.filter((r) => r.startsWith('call fn:'))).toHaveLength(3);
    const ranges = dataRanges(m);
    expect(dataSymbol(ranges, 1024)).toMatch(/^data:[0-9a-f]{16}\+0$/);
    expect(dataSymbol(ranges, 1023)).toBeNull();
    const withData = n.filter((x) => x.rows.some((r) => r.startsWith('i32.const data:')));
    expect(withData.length).toBeGreaterThan(1000);
  }, 30_000);

  it('blinds addresses and nothing else in the addr rows', () => {
    const n = normalizeModule(m, { depth: 0 });
    const f = n[136 - m.importedFuncCount];
    expect(f.addrRows.filter((r) => r === 'i32.const <addr>').length).toBeGreaterThan(10);
    expect(f.addrRows).toContain('i32.const -2');
    expect(f.addrRows.some((r) => /offset=<addr>/.test(r))).toBe(true);
    expect(
      f.addrRows.filter((r) => /offset=\d+$/.test(r)).every((r) => Number(/offset=(\d+)$/.exec(r)![1]) < 1024),
    ).toBe(true);
    expect(f.addr).not.toBe(f.fingerprint);
    expect(f.addr).not.toBe(f.shape);
  }, 30_000);

  it('keeps the shape when only a constant differs', () => {
    const n = normalizeModule(m, { depth: 0 });
    const f = n[7000 - m.importedFuncCount];
    expect(f.shapeRows.filter((r) => r.startsWith('i32.const'))).toEqual(
      f.rows.filter((r) => r.startsWith('i32.const')).map(() => 'i32.const <c>'),
    );
  }, 30_000);

  it('gives a function a different fingerprint at depth 0 than at depth 2 when it calls', () => {
    const d0 = normalizeModule(m, { depth: 0 })[7000 - m.importedFuncCount];
    const d2 = normalizeModule(m, { depth: 2 })[7000 - m.importedFuncCount];
    expect(d0.fingerprint).not.toBe(d2.fingerprint);
    expect(d0.rows.filter((r) => r === 'call fn:?')).toHaveLength(3);
  }, 30_000);
});
