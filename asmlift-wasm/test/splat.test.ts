// The splat split and its assembler, on the real target: the pieces must give back the target's
// exact bytes, and a replaced body must give a valid module whose other functions are untouched.
import { execFileSync } from 'node:child_process';
import {
  copyFileSync,
  existsSync,
  mkdirSync,
  mkdtempSync,
  readFileSync,
  rmSync,
  unlinkSync,
  writeFileSync,
} from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import { afterAll, beforeAll, describe, expect, it } from 'vitest';

import { disassemble } from '../src/disasm';
import { readAttribution, readLedgerTsv } from '../src/ledger';
import { type Module, parseModule } from '../src/parse';
import {
  type SplatManifest,
  assembleSplit,
  functionWat,
  pieceGroups,
  readManifest,
  readSubstitutes,
  sha256,
  splitModule,
} from '../src/splat';
import { URNA_ROOT } from './helpers';

const URNA = process.env.URNA_SPLAT_ROOT ?? URNA_ROOT;
const TARGET = join(URNA, 'target/vota_web_wasm.wasm');
const TARGET_SHA256 = '8579ef8d1a93819152cecdbf52906d2e4dd0f57999cb2e1e8c0d1161cd1007e2';

let scratch: string;
let pieces: string;
let manifest: SplatManifest;
let target: Module;

beforeAll(() => {
  scratch = mkdtempSync(join(tmpdir(), 'splat-test-'));
  pieces = join(scratch, 'pieces');
  // split a copy, then delete the copy: nothing after this point can read the module it came from
  const copy = join(scratch, 'source.wasm');
  copyFileSync(TARGET, copy);
  const input = new Uint8Array(readFileSync(copy));
  target = parseModule(input);
  manifest = splitModule(input, pieces, { wat: false });
  unlinkSync(copy);
}, 120_000);

afterAll(() => {
  rmSync(scratch, { recursive: true, force: true });
}, 120_000);

function bodyOf(index: number): Uint8Array {
  return target.funcs[index - target.importedFuncCount].bodyBytes;
}

/** A target function of `size` body bytes whose type returns nothing. */
function voidFunctionOfSize(size: number): number {
  const f = target.funcs.find((f) => f.size === size && f.results.length === 0);
  if (f === undefined) {
    throw new Error(`the target has no void function of ${size} bytes`);
  }
  return f.index;
}

describe('splitModule', { timeout: 120_000 }, () => {
  it('writes one piece per function and per data segment, and a manifest with the source hash', () => {
    expect(manifest.source.sha256).toBe(TARGET_SHA256);
    expect(manifest.functions.length).toBe(14_343);
    expect(manifest.functions[0].index).toBe(136);
    expect(manifest.functions.at(-1)?.index).toBe(14_478);
    expect(manifest.data.length).toBe(4_224);
    expect(manifest.sections.map((s) => s.name)).toEqual([
      'type',
      'import',
      'function',
      'table',
      'memory',
      'global',
      'export',
      'elem',
      'datacount',
      'code',
      'data',
    ]);
    const f = manifest.functions[0];
    expect(new Uint8Array(readFileSync(join(pieces, f.path)))).toEqual(bodyOf(f.index));
    const d = manifest.data[0];
    expect(d.offset).toEqual({ op: 'i32.const', value: 1024 });
    expect(new Uint8Array(readFileSync(join(pieces, d.path)))).toEqual(target.datas[0].bytes);
  });

  it('splits into its own previous split, and refuses a non-empty directory that holds no split', () => {
    const again = join(scratch, 'again');
    const input = new Uint8Array(readFileSync(TARGET));
    splitModule(input, again, { wat: false, groups: new Map([[136, 'first']]) });
    splitModule(input, again, { wat: false, groups: new Map([[136, 'second']]) });
    expect(existsSync(join(again, 'asm/first'))).toBe(false);
    expect(existsSync(join(again, 'asm/second/136.bin'))).toBe(true);
    expect(assembleSplit(again).identical).toBe(true);

    const foreign = join(scratch, 'foreign');
    mkdirSync(foreign);
    writeFileSync(join(foreign, 'notes.txt'), 'x');
    expect(() => splitModule(input, foreign, { wat: false })).toThrow(/not empty and holds no manifest.json/);
  });
});

describe('pieceGroups', () => {
  it("groups a function by its component, else by the component most of its owner's functions carry", () => {
    const rows = readLedgerTsv(join(URNA, 'ledger/functions.tsv'));
    const attribution = readAttribution(join(URNA, 'ledger/func-attribution.tsv'));
    const component = new Map([...attribution].map(([i, a]) => [i, a.component]));
    const owner = new Map([...rows].map(([i, r]) => [i, r.owner]));
    const groups = pieceGroups(
      target.funcs.map((f) => f.index),
      component,
      owner,
    );
    const ownerOnly = (o: string): number | undefined =>
      [...rows.values()].find((r) => r.owner === o && component.get(r.index) === '-')?.index;
    const libcxx = ownerOnly('libcxx');
    expect(libcxx).toBeDefined();
    expect(groups.get(libcxx!)).toBe('libc++');
    const asn1 = ownerOnly('asn1');
    expect(groups.get(asn1!)).toBe('ASN1');
    const unknown = [...rows.values()].find((r) => r.owner === '-' && component.get(r.index) === '-')!.index;
    expect(groups.has(unknown)).toBe(false);
    const spellings = new Set(groups.values());
    expect(new Set([...spellings].map((g) => g.toLowerCase())).size).toBe(spellings.size);
  });
});

describe('functionWat', () => {
  it('lists the signature, the locals and one instruction per line with its body offset', () => {
    const f = manifest.functions.find((f) => f.index === 1005)!;
    const fn = target.funcs[f.index - target.importedFuncCount];
    const text = functionWat(target, fn, f, new Map([[148, 'callee']]));
    const lines = text.split('\n');
    expect(lines[1]).toBe('(func $f1005 (type 6) (param i32 i32 i32 i32) (result i32)');
    expect(text).toContain('(;00008;)   call 148  ;; callee');
    expect(lines.filter((l) => l.startsWith('(;')).length).toBe(disassemble(fn).length);
  });
});

describe('assembleSplit', { timeout: 120_000 }, () => {
  it("rebuilds the target's exact bytes from the pieces alone", () => {
    const r = assembleSplit(pieces);
    expect(r.sha256).toBe(TARGET_SHA256);
    expect(r.identical).toBe(true);
  });

  it('still gives the target when a substitute equals its piece', () => {
    const index = 1005;
    const r = assembleSplit(pieces, new Map([[index, bodyOf(index).slice()]]));
    expect(r.sha256).toBe(TARGET_SHA256);
    expect(r.unchanged).toEqual([index]);
    expect(r.substituted).toEqual([]);
  });

  it('writes a valid module around bodies of other sizes and leaves every other function and segment as it was', () => {
    // 127 → 128 bytes widens the body's size LEB to two bytes; the shrunk one becomes `end` alone
    const grown = voidFunctionOfSize(127);
    const shrunk = voidFunctionOfSize(200);
    const g = bodyOf(grown);
    const grownBody = new Uint8Array([...g.subarray(0, g.length - 1), 0x01, 0x0b]);
    const shrunkBody = Uint8Array.of(0x00, 0x0b);
    const r = assembleSplit(
      pieces,
      new Map([
        [grown, grownBody],
        [shrunk, shrunkBody],
      ]),
    );
    expect(r.identical).toBe(false);
    expect(r.substituted.sort((a, b) => a - b)).toEqual([grown, shrunk].sort((a, b) => a - b));

    const out = join(scratch, 'substituted.wasm');
    writeFileSync(out, r.bytes);
    execFileSync('wasm-validate', [out]);
    const m = parseModule(r.bytes);
    expect(m.funcs.length).toBe(target.funcs.length);
    for (const f of m.funcs) {
      disassemble(f);
      const want = f.index === grown ? grownBody : f.index === shrunk ? shrunkBody : bodyOf(f.index);
      expect(sha256(f.bodyBytes), `function ${f.index}`).toBe(sha256(want));
      expect(f.typeIndex).toBe(target.funcs[f.index - target.importedFuncCount].typeIndex);
    }
    expect(m.datas.map((d) => [d.offset, sha256(d.bytes)])).toEqual(
      target.datas.map((d) => [d.offset, sha256(d.bytes)]),
    );
    expect(m.exports).toEqual(target.exports);
    expect(m.elems).toEqual(target.elems);
  });

  it('refuses a piece edited in place', () => {
    const f = manifest.functions[0];
    const path = join(pieces, f.path);
    const original = new Uint8Array(readFileSync(path));
    const edited = original.slice();
    edited[edited.length - 2] ^= 1;
    writeFileSync(path, edited);
    try {
      expect(() => assembleSplit(pieces)).toThrow(/no longer has the bytes the manifest recorded/);
    } finally {
      writeFileSync(path, original);
    }
  });

  it('refuses a substitute that is not a function body, or names no defined function', () => {
    expect(() => assembleSplit(pieces, new Map([[1005, Uint8Array.of(0x00, 0x01)]]))).toThrow(
      /substitute for function 1005 is not a function body/,
    );
    expect(() => assembleSplit(pieces, new Map([[3, Uint8Array.of(0x00, 0x0b)]]))).toThrow(
      /substitute for function 3: not a defined function/,
    );
  });
});

describe('readSubstitutes', { timeout: 120_000 }, () => {
  it('reads every <index>[-<name>].bin beneath a directory and refuses the same index twice', () => {
    const dir = join(scratch, 'subs');
    mkdirSync(join(dir, 'a'), { recursive: true });
    writeFileSync(join(dir, 'a/1005-some_name.bin'), bodyOf(1005));
    writeFileSync(join(dir, '1006.bin'), bodyOf(1006));
    writeFileSync(join(dir, 'README.txt'), 'not a body');
    const subs = readSubstitutes([dir]);
    expect([...subs.keys()].sort()).toEqual([1005, 1006]);
    expect(assembleSplit(pieces, subs).identical).toBe(true);

    writeFileSync(join(scratch, '1005.bin'), bodyOf(1005));
    expect(() => readSubstitutes([dir, join(scratch, '1005.bin')])).toThrow(/function 1005 is substituted twice/);
    expect(readManifest(pieces).functions.length).toBe(14_343);
  });
});
