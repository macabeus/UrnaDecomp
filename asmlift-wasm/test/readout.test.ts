// The ledger-backed readout on the project's ledger directory.
import { existsSync } from 'node:fs';
import { describe, expect, it } from 'vitest';

import { disassemble } from '../src/disasm';
import { funcAt } from '../src/parse';
import { calleeNotes, loadLedger, vtableNotes } from '../src/readout';
import { referencedStrings, stringAt } from '../src/strings';
import { target, URNA_ROOT } from './helpers';

const LEDGER = `${URNA_ROOT}/ledger`;
const OWNERS = `${URNA_ROOT}/tools/libs/cls2.json`;

describe.skipIf(!existsSync(`${LEDGER}/strings.tsv`))('readout', () => {
  const m = target();
  const t0 = performance.now();
  const ledger = loadLedger(LEDGER, OWNERS);
  const loadMs = performance.now() - t0;

  it('loads the ledger quickly', () => {
    expect(ledger.strings.length).toBeGreaterThan(10_000);
    expect(ledger.vtables.size).toBeGreaterThan(1000);
    expect(loadMs).toBeLessThan(2_000);
  });

  it('finds the strings a function points at, by offset into the string too', () => {
    const fn = funcAt(m, 146)!;
    const refs = referencedStrings(disassemble(fn), ledger.strings);
    expect(refs.map((r) => r.s.text)).toContain("The format string terminates at a '{'");
    expect(refs.every((r) => r.into === 0)).toBe(true);
    const first = ledger.strings.find((s) => s.addr === 1024)!;
    expect(stringAt(ledger.strings, 1024 + 3)).toEqual({ s: first, offset: 3 });
    expect(stringAt(ledger.strings, 1023)).toBeNull();
  });

  it('names the vtable slot a function fills and the owner of each callee', () => {
    expect(vtableNotes(ledger, 9582)[0]).toMatch(/^ecourna::api::compression::CLzmaCompress slot 0 /);
    const notes = calleeNotes(m, disassemble(funcAt(m, 9582)!), ledger, (_m, f) => f);
    expect(notes.map((n) => n.index)).toEqual([136, 1526]);
    expect(notes[1]).toMatchObject({ owner: 'app', component: 'ecourna' });
    expect(notes[1].vtable).toMatch(/ICompressor slot 0/);
  });
});
