import { describe, expect, it } from 'vitest';

import { disassemble } from '@asmlift/wasm/disasm';
import { ledgerNames, readAttribution, readLedgerTsv } from '@asmlift/wasm/ledger';
import { readStrings } from '@asmlift/wasm/readout';
import { referencedStrings } from '@asmlift/wasm/strings';

import { INPUTS } from '../scripts/data';
import { ledgerFacts } from '../src/lib/target';
import { ledgerData, pageTarget } from './helpers';

describe('distillLedger', () => {
  it('keeps exactly the names ledgerNames gives the lifter', () => {
    const expected = ledgerNames(readLedgerTsv(INPUTS.functions));
    const names = ledgerData().names;
    expect(Object.keys(names).length).toBe(expected.size);
    for (const [index, name] of expected) {
      expect(names[index]).toBe(name);
    }
  });

  it('gives every function the component, owner and best match the ledger files have', () => {
    const t = pageTarget();
    const rows = readLedgerTsv(INPUTS.functions);
    const attribution = readAttribution(INPUTS.attribution);
    for (const fn of t.m.funcs) {
      const f = ledgerFacts(t, fn.index);
      const row = rows.get(fn.index);
      expect(f.component).toBe(attribution.get(fn.index)?.component ?? '-');
      expect(f.owner).toBe(row?.owner ?? '-');
      expect(f.best?.kind ?? 'none').toBe(row?.best?.kind ?? 'none');
      expect(f.best?.index).toBe(row?.best?.index);
      expect(f.best?.score).toBe(row?.best?.score);
    }
  });

  it('keeps every string a function points into, so each function references the same strings as with the full table', () => {
    const t = pageTarget();
    const full = readStrings(INPUTS.strings);
    expect(t.strings.length).toBeLessThan(full.length);
    let references = 0;
    for (const fn of t.m.funcs) {
      const instrs = disassemble(fn);
      const a = referencedStrings(instrs, full);
      const b = referencedStrings(instrs, t.strings);
      expect(b).toEqual(a);
      references += a.length;
    }
    expect(references).toBeGreaterThan(10_000);
  });
});
