// The committed dataset: its rows still valid against today's target, registry, build and sources, and
// its selection run again on the inputs dataset/selection.json recorded.
import { createHash } from 'node:crypto';
import { existsSync, readFileSync, readdirSync } from 'node:fs';
import { join, relative } from 'node:path';
import { describe, expect, it } from 'vitest';

import { MAX_SIZE, MIN_SIZE, buildRow, loadContext, readRows, selectRows } from '../src/dataset';
import { featuresOf } from '../src/features';
import { liftCompileFlags } from '../src/flags';
import { SELECTION_INPUTS_DIR, TARGET, URNA_ROOT } from '../src/paths';
import { glueNamesFor, parseModule } from '@asmlift/wasm';
import { loadDefinitions } from '../src/provenance';
import { definitionLine } from '../src/reference';
import type { DatasetRow } from '../src/schema';
import { blobId, readSelection, recordedContext, recordedCopy } from '../src/selection';
import { classifyObjects, loadSetups } from '../src/setups';
import {
  imageReferenceProblems,
  referenceProblems,
  referenceTrees,
  setupProblems,
  symbolProblems,
  targetProblems,
} from '../src/validity';
import { IMAGE, hasImage } from './helpers';

const rows = readRows();
const setups = loadSetups();
const selection = readSelection();
const hasTrees = referenceTrees(rows).every((t) => existsSync(join(URNA_ROOT, t)));

describe('rows.json', () => {
  it('has rows from every setup of the registry', () => {
    for (const s of setups) {
      expect(rows.filter((r) => r.setup === s.key).length, s.key).toBeGreaterThanOrEqual(5);
    }
  });

  it('lists every target function once, under an id of its setup and index', () => {
    expect(new Set(rows.map((r) => r.index)).size).toBe(rows.length);
    for (const r of rows) {
      expect(r.id).toBe(`${r.setup}/${r.index}`);
    }
  });

  it('records flags the lifted source compiles with no define of', () => {
    for (const r of rows) {
      expect(liftCompileFlags(r.flags), r.id).not.toMatch(/-D/);
    }
  });

  it('keeps selected rows inside the size limits and names a symbol for every symbol-evidence row', () => {
    for (const r of rows.filter((x) => x.origin === 'selected')) {
      expect(r.size).toBeGreaterThanOrEqual(MIN_SIZE);
      expect(r.size).toBeLessThanOrEqual(MAX_SIZE);
      if (r.evidence === 'symbol') {
        expect(r.symbol, r.id).not.toBeNull();
        expect(r.object, r.id).not.toBeNull();
      }
    }
  });
});

describe('rows.json against today’s repo', () => {
  it('finds every row’s target function at its index, with its size and features', () => {
    const m = parseModule(new Uint8Array(readFileSync(TARGET)));
    expect(targetProblems(rows, m)).toEqual([]);
    const imports = glueNamesFor(TARGET)?.imports ?? null;
    for (const r of rows) {
      expect(featuresOf(m, r.index, imports), r.id).toEqual(r.features);
    }
  });

  it('finds every row’s setup in the registry with its flags, and the build compiling with them', () => {
    expect(setupProblems(rows, setups, classifyObjects(setups, IMAGE).checks)).toEqual([]);
  });

  it('finds every reference the checkout tracks', () => {
    expect(referenceProblems(rows, 'tracked')).toEqual([]);
  });

  it.skipIf(!hasTrees)('finds every reference into a fetched or built library tree', () => {
    expect(referenceProblems(rows, 'tree')).toEqual([]);
  });

  it.skipIf(!hasImage)('finds every symbol row’s symbol defined by its object, under its setup', () => {
    expect(symbolProblems(rows, loadDefinitions(setups, IMAGE, true))).toEqual([]);
  });

  it.skipIf(!hasImage)('finds every system-library reference in the image', () => {
    expect(imageReferenceProblems(rows, IMAGE)).toEqual([]);
  });
});

describe('selection.json', () => {
  const { inputs } = selection;
  const recorded = { [inputs.ledger.path]: inputs.ledger.blob, [inputs.setups.path]: inputs.setups.blob, ...inputs.sources };

  it('records the target, the ledger, the registry, the sources and the definitions the rows were selected from', () => {
    const sha = (path: string): string => createHash('sha256').update(readFileSync(join(URNA_ROOT, path))).digest('hex');
    expect(sha(inputs.target.path)).toBe(inputs.target.sha256);
    expect(sha(inputs.glue.path)).toBe(inputs.glue.sha256);
    for (const [path, blob] of Object.entries(recorded)) {
      expect(blobId(readFileSync(recordedCopy(path))), path).toBe(blob);
    }
    expect(sha(inputs.definitions.path)).toBe(inputs.definitions.sha256);
  });

  it('keeps a copy of the files it records and of no other', () => {
    const copies = readdirSync(SELECTION_INPUTS_DIR, { recursive: true, withFileTypes: true })
      .filter((e) => e.isFile())
      .map((e) => relative(SELECTION_INPUTS_DIR, join(e.parentPath, e.name)));
    expect(copies.sort()).toEqual(Object.keys(recorded).sort());
  });
});

describe('selectRows', () => {
  const selected = rows.filter((r) => r.origin === 'selected');
  const manual = new Set(rows.filter((r) => r.origin === 'manual').map((r) => r.index));
  let again: DatasetRow[] | undefined;
  const selectAgain = (): DatasetRow[] =>
    (again ??= selectRows(recordedContext(selection), selection.per).filter((r) => !manual.has(r.index)));
  const withoutReference = (rs: DatasetRow[]): Omit<DatasetRow, 'reference'>[] =>
    rs.map(({ reference: _, ...r }) => r);

  it('picks the committed rows again from the recorded inputs', () => {
    expect(withoutReference(selectAgain())).toEqual(withoutReference(selected));
  });

  it.skipIf(!hasTrees)('gives the committed rows their references again from the recorded inputs', () => {
    expect(selectAgain()).toEqual(selected);
  });
});

describe.skipIf(!hasImage)('buildRow', () => {
  it('puts a libcrypto function under openssl with its upstream source and line', () => {
    const row = buildRow(loadContext({ image: IMAGE }), 920, 'manual');
    expect(row).toMatchObject({
      setup: 'openssl',
      name: 'ChaCha20_ctr32',
      evidence: 'symbol',
      object: 'build/out/openssl/O3/libcrypto.a:libcrypto-lib-chacha_enc.o',
      reference: { path: 'third_party/openssl/crypto/chacha/chacha_enc.c', line: 71 },
      origin: 'manual',
    });
  });

  it('refuses a function no setup covers', () => {
    const ctx = loadContext({ image: IMAGE });
    const orphan = [...ctx.ledger.values()].find(
      (r) => r.best === null && r.component === '-' && ctx.m.funcs[r.index - ctx.m.importedFuncCount].size > 8,
    )!;
    expect(() => buildRow(ctx, orphan.index)).toThrow(/no setup/);
  });
});

describe('definitionLine', () => {
  it('numbers a definition under a `#line` with the line the directive names, as the compiler does', () => {
    const text = [
      'namespace a {',
      '',
      '// (original lines 3-9: content not recovered)',
      '#line 10',
      'void C::F()',
      '{',
      '}',
      '} // namespace a',
      '',
    ].join('\n');
    expect(definitionLine({ path: 'src/x.cpp' }, 'a::C::F()', () => text)).toBe(10);
  });
});
