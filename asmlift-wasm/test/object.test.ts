// The relocatable-object path, on an object the local clang compiles (Apple clang has a wasm32
// backend but no wasm sysroot, so the source includes nothing).
import { execFileSync, spawnSync } from 'node:child_process';
import { mkdtempSync, readFileSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import { describe, expect, it } from 'vitest';

import { extractFunction, isRelocatableObject } from '../src/object';
import { parseModule } from '../src/parse';

const SOURCE = `
extern "C" int helper(int);
extern int gval;
static const char table[] = "abcdefghijklmnop";
extern "C" int foo(int x, int n) {
  if (n > 3) return helper(x + table[n & 15] + gval);
  return x * 7 + n;
}
`;

const hasWasm32 = spawnSync('clang', ['--print-targets'], { encoding: 'utf8' }).stdout.includes('wasm32');

describe('extractFunction on a relocatable object', () => {
  it.skipIf(!hasWasm32)('names every relocated immediate by its symbol', () => {
    const dir = mkdtempSync(join(tmpdir(), 'wasm-object-'));
    writeFileSync(join(dir, 'a.cpp'), SOURCE);
    execFileSync('clang', ['--target=wasm32', '-O2', '-c', 'a.cpp', '-o', 'a.o'], { cwd: dir });
    const m = parseModule(new Uint8Array(readFileSync(join(dir, 'a.o'))));
    expect(isRelocatableObject(m)).toBe(true);
    expect(m.symbols.map((s) => s.name)).toEqual(expect.arrayContaining(['foo', 'helper', 'gval', '_ZL5table']));
    const x = extractFunction(m, { kind: 'symbol', name: 'foo' });
    expect(x.rows).toContain('call sym:helper');
    expect(x.rows.some((r) => r.startsWith('i32.load align=2 offset=data:gval'))).toBe(true);
    expect(x.rows.some((r) => /^i32\.(load8_s|const) .*data:_ZL5table/.test(r))).toBe(true);
    // the padded 5-byte LEBs are read through: no raw placeholder index survives in a relocated row
    expect(x.rows.filter((r) => /^call \d+$/.test(r))).toEqual([]);
  });
});
