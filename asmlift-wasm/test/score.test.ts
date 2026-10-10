// Scoring on the real target: a function against itself, against a mutated copy, against another.
import { describe, expect, it } from 'vitest';

import { disassemble } from '../src/disasm';
import { normalizeModule, signature } from '../src/normalize';
import { diffRows, renderSideBySide, score } from '../src/score';
import { target } from './helpers';

describe('score', () => {
  const m = target();
  const norm = normalizeModule(m, { depth: 2 });
  const at = (index: number) => {
    const n = norm[index - m.importedFuncCount];
    const fn = m.funcs[index - m.importedFuncCount];
    return { rows: n.rows, fn, signature: signature(fn) };
  };

  it('scores a function against itself as 0 and byte-exact', () => {
    for (const index of [200, 7000, 7787, 12000]) {
      const s = score(at(index), at(index));
      expect(s.score).toBe(0);
      expect(s.byteExact).toBe(true);
      expect(s.rows).toBe(at(index).rows.length + 1);
    }
  });

  it('scores a mutated copy above 0 and not byte-exact', () => {
    const index = 7000;
    const fn = m.funcs[index - m.importedFuncCount];
    const body = Uint8Array.from(fn.bodyBytes);
    // `i32.const 2` at body offset 0x8 (the objdump shows `41 02`): make it `i32.const 3`
    const ins = disassemble(fn);
    const c = ins.find((i) => i.name === 'i32.const' && (i.imms[0] as { value: number }).value === 2)!;
    expect(body[c.imms[0].at]).toBe(2);
    body[c.imms[0].at] = 3;
    const mutated = { ...fn, bodyBytes: body };
    const rows = disassemble(mutated).map((i) =>
      i.name === 'i32.const'
        ? `i32.const ${(i.imms[0] as { value: number }).value}`
        : norm[index - m.importedFuncCount].rows[ins.indexOf(ins.find((x) => x.offset === i.offset)!)],
    );
    const s = score(at(index), { rows, fn: mutated, signature: signature(mutated) });
    expect(s.score).toBe(1);
    expect(s.byteExact).toBe(false);
    expect(s.breakdown.operandOnly).toBe(1);
    expect(renderSideBySide(s, { onlyDiff: true })).toContain('i32.const 2');
  });

  it('counts a deleted instruction as one row, aligning the rest', () => {
    const a = at(7000).rows;
    const b = [...a.slice(0, 5), ...a.slice(6)];
    const d = diffRows(a, b);
    expect(d.diff.filter((r) => r.kind !== 'same')).toEqual([{ kind: 'deleted', left: a[5], right: null }]);
    expect(d.approximate).toBe(false);
  });

  it('scores two different functions above 0', () => {
    const s = score(at(7000), at(7001));
    expect(s.score).toBeGreaterThan(0);
    expect(s.byteExact).toBe(false);
  });

  it('falls back to positional pairing past the edit ceiling and says so', () => {
    const a = Array.from({ length: 50 }, (_, i) => `a${i}`);
    const b = Array.from({ length: 50 }, (_, i) => `b${i}`);
    const d = diffRows(a, b, 10);
    expect(d.approximate).toBe(true);
    expect(d.diff.length).toBe(50);
  });
});
