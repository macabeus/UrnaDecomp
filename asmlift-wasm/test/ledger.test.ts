// The ledger on the real pair: the target against the -Oz probe linked by the project harness
// (build/probe/out-Oz), whose symbol map names what each pair is.
import { existsSync } from 'node:fs';
import { readFileSync } from 'node:fs';
import { describe, expect, it } from 'vitest';

import { census } from '../src/census';
import { glueNamesFor } from '../src/glue-file';
import { importResolver } from '../src/imports';
import { bestMatches, directUses, ledgerRows, readLedgerTsv, renderLedgerTsv, siblingSymbolMap } from '../src/ledger';
import { normalizeModule } from '../src/normalize';
import { parseModule } from '../src/parse';
import { TARGET, target, URNA_ROOT } from './helpers';

const PROBE = `${URNA_ROOT}/build/probe/out-Oz/wasm/vota_web/vota_web_wasm.wasm`;

describe('ledger', () => {
  const m = target();
  const targetNorm = normalizeModule(m, { importName: importResolver(glueNamesFor(TARGET)!.imports) });

  it('counts direct call sites, which is the order the code section is sorted by', () => {
    const uses = directUses(m, targetNorm);
    expect(uses[136]).toBeGreaterThan(uses[137]);
    expect(uses[137]).toBeGreaterThan(uses[138]);
    expect(uses[14478]).toBe(0);
    const defined = uses.slice(m.importedFuncCount);
    expect(defined.reduce((a, b) => a + b, 0)).toBe(
      targetNorm.reduce((n, f) => n + f.instrs.filter((i) => i.opcode === 0x10 || i.opcode === 0x12).length, 0) -
        targetNorm.reduce((n, f) => n + f.rows.filter((r) => r.startsWith('call import:')).length, 0),
    );
  });

  it.skipIf(!existsSync(PROBE))('pairs the probe libc by tier and names the pairs from its symbol map', () => {
    const pm = parseModule(new Uint8Array(readFileSync(PROBE)));
    const pNorm = normalizeModule(pm, { importName: importResolver(glueNamesFor(PROBE)!.imports) });
    const names = siblingSymbolMap(PROBE)!;
    const { best, censuses } = bestMatches(m, targetNorm, [{ label: 'probe', module: pm, norm: pNorm, names }]);
    const c = censuses.get('probe')!;
    expect(c.counts.b).toBe(103);
    // dlmalloc free: identical but for bss addresses, so not a fingerprint but an addr match
    expect(best.get(136)).toMatchObject({ kind: 'addr', name: 'emscripten_builtin_free', score: 35 });
    expect(best.get(262)).toMatchObject({ kind: 'addr', name: 'emscripten_builtin_malloc' });
    // strlen touches no global: same rows, and the same bytes
    expect(best.get(156)).toMatchObject({ kind: 'byte', name: 'strlen', score: 0 });
    expect(best.get(12326)).toMatchObject({ kind: 'addr', name: '__stdio_read' });
    const kinds = [...best.values()].reduce<Record<string, number>>((acc, b) => {
      acc[b.kind] = (acc[b.kind] ?? 0) + 1;
      return acc;
    }, {});
    expect(kinds.byte! + kinds.fingerprint!).toBe(c.counts.byFingerprint);
    expect(kinds.addr).toBe(c.counts.byAddr);
    expect(kinds.shape).toBe(c.counts.byShape);
  });

  it.skipIf(!existsSync(PROBE))('renders rows that read back the same', () => {
    const pm = parseModule(new Uint8Array(readFileSync(PROBE)));
    const pNorm = normalizeModule(pm, { importName: importResolver(glueNamesFor(PROBE)!.imports) });
    const { rows } = ledgerRows(m, targetNorm, [{ label: 'probe', module: pm, norm: pNorm }], {
      owners: new Map([[136, 'libc']]),
    });
    expect(rows).toHaveLength(14343);
    expect(rows[0]).toMatchObject({ index: 136, owner: 'libc', best: { kind: 'addr', label: 'probe', index: 37 } });
    const tsv = renderLedgerTsv(rows);
    const dir = `${process.env.TMPDIR ?? '/tmp'}`;
    const path = `${dir}/functions-${process.pid}.tsv`;
    require('node:fs').writeFileSync(path, tsv);
    const back = readLedgerTsv(path);
    expect(back.size).toBe(14343);
    expect(back.get(136)).toEqual(rows[0]);
    expect(back.get(137)!.best).toBeNull();
    require('node:fs').unlinkSync(path);
  });

  it('matches a module to itself without a single addr or shape pair', () => {
    const c = census(m, targetNorm, m, targetNorm);
    expect(c.counts).toMatchObject({ byFingerprint: 14343, byAddr: 0, byShape: 0 });
  });
});
