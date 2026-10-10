// The lift measurement harness on the real target: the stratified sample, the row canonicalisation
// against a real object, and a small evaluation through the pinned image (skipped without it).
import { spawnSync } from 'node:child_process';
import { existsSync, mkdtempSync, readFileSync, writeFileSync } from 'node:fs';
import { join } from 'node:path';
import { describe, expect, it } from 'vitest';

import { glueNamesFor } from '../src/glue-file';
import { importResolver } from '../src/imports';
import { readAttribution } from '../src/ledger';
import { liftNames } from '../src/lift';
import {
  SIZE_BANDS,
  canonRow,
  componentOf,
  errorClass,
  evaluateLift,
  failureClasses,
  readResultsTsv,
  readSampleTsv,
  renderReport,
  renderResultsTsv,
  renderSampleTsv,
  sampleFunctions,
  summarize,
} from '../src/lift-eval';
import { DEFAULT_IMAGE, compileLifted } from '../src/rank';
import { TARGET, target, URNA_ROOT } from './helpers';

const ATTRIBUTION = `${URNA_ROOT}/ledger/func-attribution.tsv`;

const hasImage = spawnSync('docker', ['image', 'inspect', DEFAULT_IMAGE], { encoding: 'utf8' }).status === 0;

let cachedAttribution: ReturnType<typeof readAttribution> | null | undefined;
function attribution(): ReturnType<typeof readAttribution> | null {
  cachedAttribution ??= existsSync(ATTRIBUTION) ? readAttribution(ATTRIBUTION) : null;
  return cachedAttribution;
}

describe('sampleFunctions', () => {
  it('returns the same sample for the same seed and another for another seed', () => {
    const a = sampleFunctions(target(), attribution(), { size: 150, seed: 1 });
    const b = sampleFunctions(target(), attribution(), { size: 150, seed: 1 });
    const c = sampleFunctions(target(), attribution(), { size: 150, seed: 2 });
    expect(b).toEqual(a);
    expect(c.map((r) => r.index)).not.toEqual(a.map((r) => r.index));
  });

  it('gives every component at least three functions and spreads each across its size bands', () => {
    const rows = sampleFunctions(target(), attribution(), { size: 150, seed: 1 });
    const components = new Set(rows.map((r) => r.component));
    expect(components).toEqual(new Set(target().funcs.map((f) => componentOf(attribution(), f.index))));
    for (const c of components) {
      expect(rows.filter((r) => r.component === c).length).toBeGreaterThanOrEqual(3);
    }
    for (const b of SIZE_BANDS) {
      expect(rows.some((r) => r.band === b.name)).toBe(true);
    }
    expect(rows.length).toBeGreaterThanOrEqual(150);
    expect(new Set(rows.map((r) => r.index)).size).toBe(rows.length);
  });

  it('round-trips through sample.tsv', () => {
    const rows = sampleFunctions(target(), attribution(), { size: 40, seed: 7 });
    const dir = mkdtempSync('/tmp/wasm-lift-eval-');
    writeFileSync(join(dir, 'sample.tsv'), renderSampleTsv(rows));
    expect(readSampleTsv(join(dir, 'sample.tsv'))).toEqual(rows);
  });
});

describe('canonRow', () => {
  it('drops callees and memory indices, and blinds addresses on both spellings', () => {
    expect(canonRow('call sym:f364')).toBe('call');
    expect(canonRow('call fn:?')).toBe('call');
    expect(canonRow('memory.copy 0 0')).toBe('memory.copy');
    expect(canonRow('i32.const data:ba4744780c86a45e+186332')).toBe('i32.const <addr>');
    expect(canonRow('i32.const 187356')).toBe('i32.const <addr>');
    expect(canonRow('i32.const 16')).toBe('i32.const 16');
    expect(canonRow('i32.load align=0 offset=177929')).toBe('i32.load align=0 offset=<addr>');
    expect(canonRow('i32.load align=2 offset=8')).toBe('i32.load align=2 offset=8');
  });

  it('spells the stack pointer and the indirect table the way the target does', () => {
    expect(canonRow('global.get global:__stack_pointer')).toBe('global.get 0');
    expect(canonRow('call_indirect type:(i32)->(i32) R_WASM_TABLE_NUMBER_LEB:__indirect_function_table')).toBe(
      'call_indirect type:(i32)->(i32)',
    );
  });
});

describe.skipIf(!hasImage)('evaluateLift', () => {
  it('lifts, compiles, optimises and scores a sample, and its results round-trip through results.tsv', () => {
    const m = target();
    const glue = glueNamesFor(TARGET);
    const attr = attribution();
    const sample = [156, 434, 6283, 10058].map((index) => {
      const f = m.funcs[index - m.importedFuncCount];
      return { index, component: attr?.get(index)?.component ?? '-', band: 'any', size: f.size };
    });
    const dir = mkdtempSync('/tmp/wasm-lift-eval-');
    const { rows } = evaluateLift(m, sample, {
      lift: { names: liftNames(m, { imports: glue?.imports ?? null, exports: glue?.exports ?? null }) },
      attribution: attr,
      dir,
      importName: importResolver(glue?.imports ?? null),
    });
    const s = summarize(rows);
    expect(s).toMatchObject({ n: 4, liftThrows: 0, compiled: 4 });
    expect(rows.every((r) => r.ratio !== undefined && r.score! <= r.rows!)).toBe(true);
    expect(rows.find((r) => r.index === 434)!.score).toBe(0);
    // each object also links alone through the target link's wasm-opt passes and scores again
    expect(rows.every((r) => r.optScore !== undefined && r.optRatio === r.optScore / r.targetRows)).toBe(true);
    expect(rows.find((r) => r.index === 434)!.optScore).toBe(0);
    expect(failureClasses(rows)).toEqual([]);
    writeFileSync(join(dir, 'results.tsv'), renderResultsTsv(rows));
    const back = readResultsTsv(join(dir, 'results.tsv'));
    expect([...back.values()].map((r) => r.score)).toEqual(rows.map((r) => r.score));
    expect([...back.values()].map((r) => r.optScore)).toEqual(rows.map((r) => r.optScore));
    const report = renderReport(rows, {
      label: 't',
      target: TARGET,
      image: DEFAULT_IMAGE,
      seed: null,
      sampleFrom: 'test',
      liftMs: 0,
      compileMs: 0,
    });
    expect(report).toContain('| 4 | 0 | 4 | 100.0% |');
  }, 300_000);
});

describe.skipIf(!hasImage)('errorClass', () => {
  it('clusters em++ errors that differ only in names into one class', () => {
    const dir = mkdtempSync('/tmp/wasm-lift-eval-');
    writeFileSync(join(dir, 'a.cpp'), 'int f() { return undeclared_a; }\n');
    writeFileSync(join(dir, 'b.cpp'), 'int g() { return undeclared_b + 1; }\n');
    expect(compileLifted(dir, ['a.cpp', 'b.cpp']).sort()).toEqual(['a.cpp', 'b.cpp']);
    const a = errorClass(readFileSync(join(dir, 'a.cpp.err'), 'utf8'));
    const b = errorClass(readFileSync(join(dir, 'b.cpp.err'), 'utf8'));
    expect(a.cls).toBe("use of undeclared identifier '…'");
    expect(b.cls).toBe(a.cls);
    expect(a.line).toContain("'undeclared_a'");
  }, 300_000);
});
