// The bundle (one Markdown file per TU cluster) and the progress meter over functions.tsv.
import { existsSync, readFileSync } from 'node:fs';
import { describe, expect, it } from 'vitest';

import { census } from '../src/census';
import { glueNamesFor } from '../src/glue-file';
import { importResolver } from '../src/imports';
import { type LedgerRow, type MatchKind, readLedgerTsv } from '../src/ledger';
import { normalizeModule } from '../src/normalize';
import { funcAt, parseModule } from '../src/parse';
import { movements, renderProgress, tally } from '../src/progress';
import { loadLedger } from '../src/readout';
import { renderBundle, renderFunctionReport } from '../src/report';
import { TARGET, target, URNA_ROOT } from './helpers';

const LEDGER = `${URNA_ROOT}/ledger`;
const SKELETON = `${URNA_ROOT}/build/out/wasm/vota_web/vota_web_wasm.wasm`;

describe('report', () => {
  const m = target();
  const importName = importResolver(glueNamesFor(TARGET)!.imports);
  const norm = normalizeModule(m, { importName });
  const ledger = existsSync(`${LEDGER}/strings.tsv`) ? loadLedger(LEDGER) : null;
  const opts = { importName, exports: null, ledger, norm };

  it('renders one function with its header, rows and folded readout', () => {
    const text = renderFunctionReport(m, funcAt(m, 9582)!, opts);
    expect(text).toMatch(/^;; function 9582 \(i32\)->\(i32\)/);
    expect(text).toContain(';; table slots 6083');
    expect(text).toContain(';; --- disassembly');
    expect(text).toContain('function f9582(i32 p0) -> (i32) {');
    const brief = renderFunctionReport(m, funcAt(m, 9582)!, { ...opts, noDisasm: true });
    expect(brief).not.toContain(';; --- disassembly');
    expect(brief).toContain('function f9582(i32 p0) -> (i32) {');
  });

  it('bundles several functions behind one header table, normalising the module once', () => {
    const fns = [9582, 282, 136].map((i) => funcAt(m, i)!);
    const t0 = performance.now();
    const md = renderBundle('sample', m, fns, opts);
    expect(performance.now() - t0).toBeLessThan(2_000);
    expect(md).toMatch(/^# bundle sample\n/);
    expect(md.match(/^\| \[\d+\]\(#fn-\d+\) /gm)).toHaveLength(3);
    expect(md.match(/^## fn \d+$/gm)).toEqual(['## fn 9582', '## fn 282', '## fn 136']);
    if (ledger !== null) {
      expect(md).toContain('ecourna::api::compression::CLzmaCompress slot 0');
    }
  });

  it.skipIf(!existsSync(SKELETON))(
    'pairs functions whose call_indirect type INDICES differ (types compare by structure)',
    () => {
      const sm = parseModule(new Uint8Array(readFileSync(SKELETON)));
      const sNorm = normalizeModule(sm, { importName: importResolver(glueNamesFor(SKELETON)!.imports) });
      const c = census(m, norm, sm, sNorm);
      const typeImm = (n: (typeof norm)[number]): number[] =>
        n.instrs.filter((i) => i.opcode === 0x11).map((i) => (i.imms[0] as { value: number }).value);
      let differing = 0;
      for (const p of c.pairs) {
        if (p.by !== 'fingerprint') {
          continue;
        }
        const ta = typeImm(norm[p.a - m.importedFuncCount]);
        const tb = typeImm(sNorm[p.b - sm.importedFuncCount]);
        if (ta.length > 0 && ta.some((t, k) => t !== tb[k])) {
          differing++;
          expect(norm[p.a - m.importedFuncCount].rows).toEqual(sNorm[p.b - sm.importedFuncCount].rows);
        }
      }
      expect(differing).toBeGreaterThan(50);
    },
    // a census of both whole modules
    60_000,
  );
});

describe('progress', () => {
  const row = (index: number, size: number, owner: string, kind: MatchKind, score = 0): LedgerRow => ({
    index,
    size,
    uses: 0,
    owner,
    component: '-',
    best: kind === 'none' ? null : { label: 'x', index, kind, score, name: null },
  });
  const prev = new Map(
    [row(1, 10, 'app', 'none'), row(2, 20, 'app', 'addr', 3), row(3, 30, 'sqlite', 'byte')].map((r) => [r.index, r]),
  );
  const cur = new Map(
    [row(1, 10, 'app', 'byte'), row(2, 20, 'app', 'addr', 1), row(3, 30, 'sqlite', 'byte')].map((r) => [r.index, r]),
  );

  it('tallies kinds with their bytes', () => {
    const t = tally(cur.values());
    expect(t).toMatchObject({ total: 3, totalBytes: 60 });
    expect(t.n).toEqual({ byte: 2, fingerprint: 0, addr: 1, shape: 0, none: 0 });
    expect(t.bytes.byte).toBe(40);
  });

  it('renders the per-owner table with deltas against the previous ledger', () => {
    const md = renderProgress(cur, prev, 'owner');
    expect(md).toContain('| **all** | 3 | 2 (+1) | 0 | 1 | 0 | 0 (-1) | 66.7 | 66.7 |');
    expect(md).toContain('| app | 2 | 1 (+1) | 0 | 1 | 0 | 0 (-1) | 50.0 | 33.3 |');
    expect(md).toContain('| sqlite | 1 | 1 | 0 | 0 | 0 | 0 | 100.0 | 100.0 |');
  });

  it('lists every function whose kind or score moved, improvements first', () => {
    const ms = movements(cur, prev);
    expect(ms.map((x) => [x.index, x.from, x.to])).toEqual([
      [1, 'none', 'byte'],
      [2, 'addr', 'addr'],
    ]);
    expect(ms[1]).toMatchObject({ fromScore: 3, toScore: 1 });
  });

  it.skipIf(!existsSync(`${LEDGER}/functions.tsv`))('reads the project ledger', () => {
    const cur2 = readLedgerTsv(`${LEDGER}/functions.tsv`);
    expect(cur2.size).toBe(14343);
    expect(renderProgress(cur2, null, 'component')).toMatch(/\| ecourna \| \d+ \|/);
  });
});
