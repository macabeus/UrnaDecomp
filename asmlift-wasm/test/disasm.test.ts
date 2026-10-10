// The decoder against the real target and against wabt's disassembly of it.
import { execFileSync } from 'node:child_process';
import { describe, expect, it } from 'vitest';

import { disassemble } from '../src/disasm';
import { DecodeError } from '../src/leb';
import { parseModule } from '../src/parse';
import { TARGET, target } from './helpers';

describe('disassemble', () => {
  it('decodes every function of the target and lands every `end` on the body size', () => {
    const m = target();
    let total = 0;
    const broken: string[] = [];
    for (const f of m.funcs) {
      const ins = disassemble(f);
      total += ins.length;
      const last = ins[ins.length - 1];
      if (last.name !== 'end' || last.end !== f.size) {
        broken.push(`fn ${f.index}: last ${last.name} ends at ${last.end} of ${f.size}`);
      }
      for (let i = 1; i < ins.length; i++) {
        if (ins[i].offset !== ins[i - 1].end) {
          broken.push(
            `fn ${f.index}: gap between 0x${ins[i - 1].end.toString(16)} and 0x${ins[i].offset.toString(16)}`,
          );
          break;
        }
      }
    }
    expect(broken).toEqual([]);
    expect(total).toBe(2_710_054);
  }, 60_000);

  it('agrees with wasm-objdump -d on mnemonics and file offsets', () => {
    // wasm-objdump prints absolute file offsets, and one extra `local[a..b] type=t` row per
    // locals group before the instructions — which is why its line count exceeds the instruction
    // count by the number of functions with locals, not by anything about the decoding.
    const text = execFileSync('wasm-objdump', ['-d', TARGET], { encoding: 'utf8', maxBuffer: 1 << 30 });
    const m = target();
    const sample = [7000, 200, 7787, 136, 136 + 14342];
    for (const index of sample) {
      const defined = index - m.importedFuncCount;
      // an exported function's header carries its export name: `func[14478] <Db>:`
      const header = (i: number): RegExp => new RegExp(`^[0-9a-f]+ func\\[${i}\\]( <[^>]*>)?:$`, 'm');
      const start = header(index).exec(text)?.index ?? -1;
      expect(start).toBeGreaterThan(0);
      const nextHit = header(index + 1).exec(text.slice(start));
      const block = text.slice(start, nextHit === null ? undefined : start + nextHit.index);
      const rows = [...block.matchAll(/^ ([0-9a-f]+): [0-9a-f ]+\| +(\S+)/gm)]
        .map((r) => [parseInt(r[1], 16), r[2]])
        .filter((r) => !String(r[1]).startsWith('local['));
      const fn = m.funcs[defined];
      const ours = disassemble(fn).map((i) => [fn.bodyOffset + i.offset, i.name]);
      expect(ours.length).toBe(rows.length);
      expect(ours).toEqual(rows);
    }
  }, 60_000);

  it('refuses an unknown opcode loudly, with the file offset', () => {
    const m = target();
    const fn = m.funcs[7000 - m.importedFuncCount];
    const body = Uint8Array.from(fn.bodyBytes);
    body[fn.codeStart] = 0xfd; // SIMD prefix
    expect(() => disassemble({ ...fn, bodyBytes: body })).toThrow(DecodeError);
    expect(() => disassemble({ ...fn, bodyBytes: body })).toThrow(
      new RegExp(`SIMD.*0x${(fn.bodyOffset + fn.codeStart).toString(16)}`),
    );
  });

  it('reads a multivalue block type as a signed LEB type index, not a byte', () => {
    // (module (type (func (result i32 i32))) ×65 then a block with type index 64 → 0xc0 0x00)
    const types = Array.from({ length: 65 }, () => [0x60, 0x00, 0x02, 0x7f, 0x7f]).flat();
    const typeSec = [0x01, ...leb(types.length + 1), ...leb(65), ...types];
    const funcSec = [0x03, 0x02, 0x01, 0x00];
    const body = [0x00, 0x02, 0xc0, 0x00, 0x41, 0x01, 0x41, 0x02, 0x0b, 0x0b];
    const codeSec = [0x0a, ...leb(body.length + 2), 0x01, ...leb(body.length), ...body];
    const bytes = Uint8Array.from([0, 0x61, 0x73, 0x6d, 1, 0, 0, 0, ...typeSec, ...funcSec, ...codeSec]);
    const m = parseModule(bytes);
    const ins = disassemble(m.funcs[0]);
    expect(ins[0].name).toBe('block');
    expect(ins[0].imms[0]).toMatchObject({ kind: 'blocktype', value: { kind: 'type', index: 64 } });
    expect(ins.map((i) => i.name)).toEqual(['block', 'i32.const', 'i32.const', 'end', 'end']);
  });
});

function leb(n: number): number[] {
  const out: number[] = [];
  do {
    let b = n & 0x7f;
    n >>>= 7;
    if (n !== 0) {
      b |= 0x80;
    }
    out.push(b);
  } while (n !== 0);
  return out;
}
