// The parser against the real target, checked against wabt's own section header dump.
import { execFileSync } from 'node:child_process';
import { describe, expect, it } from 'vitest';

import { TARGET, target } from './helpers';

/** `wasm-objdump -h` rows: name → count. */
function objdumpCounts(): Map<string, number> {
  const text = execFileSync('wasm-objdump', ['-h', TARGET], { encoding: 'utf8' });
  const out = new Map<string, number>();
  for (const line of text.split('\n')) {
    const m = /^\s*(\w+) start=0x([0-9a-f]+) end=0x([0-9a-f]+) .* count: (\d+)/.exec(line);
    if (m) {
      out.set(m[1], Number(m[4]));
    }
  }
  return out;
}

describe('parseModule', () => {
  it('counts every section the way wasm-objdump -h does', () => {
    const m = target();
    const h = objdumpCounts();
    expect(m.types.length).toBe(h.get('Type'));
    expect(m.imports.length).toBe(h.get('Import'));
    expect(m.funcs.length).toBe(h.get('Function'));
    expect(m.funcs.length).toBe(h.get('Code'));
    expect(m.tables.length).toBe(h.get('Table'));
    expect(m.memories.length).toBe(h.get('Memory'));
    expect(m.globals.length).toBe(h.get('Global'));
    expect(m.exports.length).toBe(h.get('Export'));
    expect(m.elems.length).toBe(h.get('Elem'));
    expect(m.datas.length).toBe(h.get('Data'));
    expect(m.dataCount).toBe(h.get('DataCount'));
  });

  it('places each section where wasm-objdump -h says it starts', () => {
    const text = execFileSync('wasm-objdump', ['-h', TARGET], { encoding: 'utf8' });
    const starts = [...text.matchAll(/^\s*(\w+) start=0x([0-9a-f]+) end=0x([0-9a-f]+)/gm)].map((m) => [
      m[1],
      parseInt(m[2], 16),
      parseInt(m[3], 16),
    ]);
    expect(target().sections.map((s) => [s.name, s.start, s.end])).toEqual(starts);
  });

  it('reads the table contents and the data layout of the target', () => {
    const m = target();
    expect(m.elems[0].funcIndices.length).toBe(12854);
    expect(m.elems[0].offset).toEqual({ op: 'i32.const', value: 1 });
    expect(m.datas[0].offset).toBe(1024);
    expect(m.datas.reduce((n, d) => n + d.bytes.length, 0)).toBe(1732475);
    expect(m.globals[0].init).toBe('i32.const 10326000');
    expect(m.memories[0]).toEqual({ min: 414, max: 32768, shared: false });
  });

  it('indexes defined functions after the imports', () => {
    const m = target();
    expect(m.importedFuncCount).toBe(136);
    expect(m.funcs[0].index).toBe(136);
    expect(m.funcs[m.funcs.length - 1].index).toBe(136 + 14343 - 1);
    for (const f of m.funcs) {
      expect(f.bodyBytes.length).toBe(f.size);
      expect(m.types[f.typeIndex].params).toEqual(f.params);
    }
  });

  it('parses the target in well under a few seconds', () => {
    const t0 = performance.now();
    target();
    expect(performance.now() - t0).toBeLessThan(3000);
  }, 20_000);
});
