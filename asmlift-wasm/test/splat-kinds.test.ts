// Operand kinds on the real links of this checkout: the plain link and its shifted relink
// (tools/round/link-kinds.sh, build/out/wasm-kinds/vota_web), the table splat-kinds wrote from them, and the target's
// split. The perturbation cases (CASES below) guard the kind check: one operand of a matched function of ours
// changed, in both links the way a source edit would change it (an integer stays put in the relink, an address moves
// by the data shift, a table slot by the table shift), then the build. baseSums, which follows a __table_base
// constant through the operand stack, is tested on hand-written bodies.
import { copyFileSync, existsSync, mkdtempSync, readFileSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import { describe, expect, it } from 'vitest';

import { type Instr, disassemble } from '../src/disasm';
import type { Func, Module } from '../src/parse';
import { parseModule } from '../src/parse';
import {
  type FunctionKinds,
  type KindsSide,
  type OperandKinds,
  baseSums,
  classifyFunction,
  classifyOperands,
  defaultKindsPath,
  kindsPathOf,
  kindsSide,
  kindsSummary,
  linkShifts,
  noMoreArgs,
  operandKind,
  parseOperandKinds,
  readOperandKinds,
  renderOperandKinds,
} from '../src/splat-kinds';
import { type Piece, remapBody } from '../src/splat-remap';
import {
  MatchesFailed,
  attempt,
  buildSplat,
  kindUnverifiedOverLimit,
  parseMatchList,
  readPieceBodies,
  referencedFunctionsAt,
  reviewWarnings,
} from '../src/splat-sub';
import { runSplatSubCommand } from '../src/splat-sub-cli';
import { type SymbolMaps, loadOurSide, loadTargetSide, symbolMaps } from '../src/splat-symbols';
import { URNA_ROOT } from './helpers';

const URNA = process.env.URNA_SPLAT_ROOT ?? URNA_ROOT;
const TARGET = `${URNA}/target/vota_web_wasm.wasm`;
const PLAIN = `${URNA}/build/out/wasm/vota_web/vota_web_wasm.wasm`;
const MAP_DIR = `${URNA}/build/out/wasm-map/vota_web`;
const SHIFTED_DIR = `${URNA}/build/out/wasm-kinds/vota_web`;
const KINDS = defaultKindsPath(PLAIN);
const SYMBOLS = `${URNA}/splat/symbol_addrs.txt`;
const PIECES = `${URNA}/build/out/splat/pieces`;
const TARGET_SHA = '8579ef8d1a93819152cecdbf52906d2e4dd0f57999cb2e1e8c0d1161cd1007e2';
const D = 1_048_576;
const T = 20_000;
const real =
  existsSync(`${MAP_DIR}/names.tsv`) &&
  existsSync(`${SHIFTED_DIR}/names.tsv`) &&
  existsSync(KINDS) &&
  existsSync(SYMBOLS) &&
  existsSync(`${PIECES}/manifest.json`);

const NEXT_ENTRY = '_ZN3api10CDirReader9NextEntryEv';
const INVERT_COLORS = '_ZN3api13CBmpConversor12InvertColorsEPhii';
const DO_CONVERTE =
  '_ZNK5comum3asn36CConversorNumViasImpressasRelatorios10DoConverteERKNS_2md15estadoaplicacao27CNumViasImpressasRelatoriosE';
const RAW_READ = '_ZNK7ecourna3api2io5CFile7RawReadEPvj';
const DSA_PEM_THUNK = 'byn$mgfn-shared$dsa_to_SubjectPublicKeyInfo_pem_encode';

interface Loaded {
  maps: SymbolMaps;
  kinds: OperandKinds;
  plain: KindsSide;
  shifted: KindsSide;
}
let cache: Loaded | null = null;
function load(): Loaded {
  if (cache === null) {
    const ours = loadOurSide(PLAIN, MAP_DIR);
    const target = loadTargetSide(TARGET, SYMBOLS, `${URNA}/ledger`, ours.stackSize);
    const kinds = readOperandKinds(KINDS, ours.module.bytes);
    const shiftedModule = parseModule(new Uint8Array(readFileSync(`${SHIFTED_DIR}/vota_web_wasm.wasm`)));
    cache = {
      maps: symbolMaps(ours, target, kinds),
      kinds,
      plain: kindsSide(ours.module, `${MAP_DIR}/names.tsv`),
      shifted: kindsSide(shiftedModule, `${SHIFTED_DIR}/names.tsv`),
    };
  }
  return cache;
}

function indexOf(side: KindsSide, name: string): number {
  const i = [...side.names].find(([, n]) => n === name)?.[0];
  expect(i, name).toBeDefined();
  return i!;
}

function funcOf(m: Module, index: number): Func {
  return m.funcs[index - m.importedFuncCount];
}

/** LEB128 of `v`: signed (an i32.const or i64.const) or not (a callee) */
function encode(v: number | bigint, signed: boolean): number[] {
  const out: number[] = [];
  let x = BigInt(v);
  for (;;) {
    const b = Number(x & 0x7fn);
    x >>= 7n;
    const done = signed ? (x === 0n && (b & 0x40) === 0) || (x === -1n && (b & 0x40) !== 0) : x === 0n;
    if (done) {
      out.push(b);
      return out;
    }
    out.push(b | 0x80);
  }
}

/** The module with the i32.const or i64.const value, memarg offset or callee of our function `index`'s instruction at
 *  body offset `at` replaced (on the parsed module: the body re-encoded around the new operand). */
function patched(m: Module, index: number, at: number, value: number | bigint): Module {
  const fn = funcOf(m, index);
  const i: Instr | undefined = disassemble(fn).find((x) => x.offset === at);
  const im = i?.imms.find(
    (x) => x.kind === 's32' || x.kind === 's64' || x.kind === 'memarg' || (x.kind === 'u32' && x.role === 'func'),
  );
  if (i === undefined || im === undefined) {
    throw new Error(`no constant, memory offset or call at +0x${at.toString(16)}`);
  }
  const from = im.kind === 'memarg' ? im.offsetAt : im.at;
  const next = i.imms[i.imms.indexOf(im) + 1]?.at ?? i.end;
  const bodyBytes = Uint8Array.from([
    ...fn.bodyBytes.subarray(0, from),
    ...encode(value, im.kind === 's32' || im.kind === 's64'),
    ...fn.bodyBytes.subarray(next),
  ]);
  return { ...m, funcs: m.funcs.map((f) => (f === fn ? { ...f, bodyBytes, size: bodyBytes.length } : f)) };
}

/** The offset in the relink's function `s` of the instruction at `at` in the plain link's `p`: the same instruction
 *  sequence, but a moved constant may take another LEB length. */
function shiftedAt(p: number, s: number, at: number): number {
  const { plain, shifted } = load();
  const k = disassemble(funcOf(plain.module, p)).findIndex((x) => x.offset === at);
  return disassemble(funcOf(shifted.module, s))[k].offset;
}

function valueAt(m: Module, index: number, at: number): number | bigint {
  const i = disassemble(funcOf(m, index)).find((x) => x.offset === at)!;
  const im = i.imms.find((x) => x.kind === 'memarg') ?? i.imms[0];
  return im.kind === 'memarg' ? im.offset : im.kind === 's32' || im.kind === 'u32' || im.kind === 's64' ? im.value : Number.NaN;
}

/** A function of one i32 parameter whose body is `code` (no locals, the final `end` added), alone in a module. */
function handWritten(code: number[]): { m: Module; fn: Func; instrs: Instr[] } {
  const bodyBytes = Uint8Array.from([0, ...code, 0x0b]);
  const fn: Func = {
    index: 0,
    typeIndex: 0,
    params: ['i32'],
    results: [],
    locals: [],
    bodyOffset: 0,
    size: bodyBytes.length,
    bodyBytes,
    codeStart: 1,
  };
  const m = { types: [{ params: ['i32'], results: [] }], imports: [], importedFuncCount: 0, funcs: [fn] } as unknown as Module;
  return { m, fn, instrs: disassemble(fn) };
}

describe('baseSums', () => {
  const i32 = (v: number): number[] => [0x41, ...encode(v, true)];
  const LOCAL_GET = [0x20, 0];
  const LOCAL_SET = [0x21, 0];
  const SELECT = [0x1b];
  const ADD = [0x6a];
  const DROP = [0x1a];
  // the body offset of each instruction that is an i32.const of `v`
  const at = (instrs: Instr[], v: number): number[] =>
    instrs.filter((i) => i.opcode === 0x41 && i.imms[0].kind === 's32' && i.imms[0].value === v).map((i) => i.offset);
  // the base is the constant 1; `slots` are the values of the other moved table slots
  const sums = (code: number[], slots: number[] = []) => {
    const { m, fn, instrs } = handWritten(code);
    return { r: baseSums(m, fn, instrs, new Set(at(instrs, 1)), new Set(slots.flatMap((v) => at(instrs, v)))), instrs };
  };

  it('reads the constants added to the base through select arms as relative to it', () => {
    // vdbeSorterSort's `select(2035, 2036, c) + __table_base`
    const { r, instrs } = sums([...i32(2035), ...i32(2036), ...LOCAL_GET, ...SELECT, ...i32(1), ...ADD, ...DROP]);
    expect(r).toEqual({ sums: new Map([[at(instrs, 1)[0], [...at(instrs, 2035), ...at(instrs, 2036)]]]), slotSums: [] });
  });

  it('leaves out a base used as a value: a slot or address of its own', () => {
    const none = { sums: new Map(), slotSums: [] };
    expect(sums([...i32(1), 0x10, 0]).r).toEqual(none); // call 0 (i32)
    expect(sums([...LOCAL_GET, ...i32(1), 0x36, 2, 0]).r).toEqual(none); // i32.store
  });

  it('lists another table slot added to constants: a sum with part of the base in it', () => {
    // binaryen folded 1 of the relative operands into the base: `select(2034, 2035, c) + 2`, and 2 moved as a slot
    const { r, instrs } = sums([...i32(2034), ...i32(2035), ...LOCAL_GET, ...SELECT, ...i32(2), ...ADD, ...DROP], [2]);
    expect(r).toEqual({ sums: new Map(), slotSums: at(instrs, 2) });
    // a slot added to another slot
    const both = sums([...i32(7), ...i32(9), ...ADD, ...DROP], [7, 9]);
    expect(both.r).toEqual({ sums: new Map(), slotSums: [...at(both.instrs, 7), ...at(both.instrs, 9)] });
    // a slot used as a pointer (stored, passed, added to a variable) is not followed further
    const pointer = { sums: new Map(), slotSums: [] };
    expect(sums([...LOCAL_GET, ...i32(7), 0x36, 2, 0], [7]).r).toEqual(pointer);
    expect(sums([...i32(7), ...LOCAL_SET], [7]).r).toEqual(pointer);
    expect(sums([...LOCAL_GET, ...i32(7), ...ADD, ...DROP], [7]).r).toEqual(pointer);
    // the base added to a slot: neither part can be told
    expect(sums([...i32(7), ...i32(1), ...ADD, ...DROP], [7]).r).toEqual({ why: expect.stringMatching(/added to a table slot$/) });
  });

  it('cannot tell where a base kept in a local, carried out of a block or added to a variable ends up', () => {
    expect(sums([...i32(1), ...LOCAL_SET]).r).toEqual({ why: expect.stringMatching(/base itself, kept in a local$/) });
    // block (result i32) i32.const 1 end; i32.const 5; i32.add
    expect(sums([0x02, 0x7f, ...i32(1), 0x0b, ...i32(5), ...ADD, ...DROP]).r).toEqual({
      why: expect.stringMatching(/a block result$/),
    });
    expect(sums([...i32(1), ...i32(7), 0x0d, 0, ...ADD, ...DROP]).r).toEqual({
      why: expect.stringMatching(/carried by a branch$/),
    }); // br_if 0
    expect(sums([...LOCAL_GET, ...i32(1), ...ADD, ...DROP]).r).toEqual({
      why: expect.stringMatching(/added to a value that is not a constant$/),
    });
    // the relative operands chosen first and kept in a local: the sum's other side is a local.get
    const viaLocal = [...i32(2035), ...i32(2036), ...LOCAL_GET, ...SELECT, ...LOCAL_SET];
    expect(sums([...viaLocal, ...LOCAL_GET, ...i32(1), ...ADD, ...DROP]).r).toEqual({
      why: expect.stringMatching(/added to a value that is not a constant$/),
    });
    // the base in a select arm, the select added to a constant
    expect(sums([...i32(1), ...i32(5), ...LOCAL_GET, ...SELECT, ...i32(7), ...ADD, ...DROP]).r).toEqual({
      why: expect.stringMatching(/in a sum this cannot follow$/),
    });
  });
});

// Two hand-written links of one function, the second with the data and the table moved: what classifyFunction makes
// of each constant, with no build output (these run in CI).
describe('classifyFunction on hand-written links', () => {
  const i32 = (v: number): number[] => [0x41, ...encode(v, true)];
  const DROP = [0x1a];
  const LOCAL_GET = [0x20, 0];
  const SELECT = [0x1b];
  const ADD = [0x6a];
  /** `code` as the one function of a link whose data starts at `data` and whose table (every slot holding the
   *  function) at `table` */
  const link = (code: number[], data: number, table: number): KindsSide => {
    const { m } = handWritten(code);
    const module = {
      ...m,
      datas: [{ mode: 'active', offset: data, bytes: new Uint8Array(4) }],
      globals: [{ type: 'i32', mutable: true, init: `i32.const ${data + 65536}` }],
      elems: [{ mode: 'active', tableIndex: 0, offset: { value: table }, funcIndices: Array(2100).fill(0) }],
    } as unknown as Module;
    return { module, names: new Map([[0, 'f']]) };
  };
  // the plain link's constants, and the relink's: each moved by D (an address), T (a slot) or not at all
  const classify = (plain: (number | number[])[], shifted: (number | number[])[]): { k: FunctionKinds; instrs: Instr[] } => {
    const code = (xs: (number | number[])[]): number[] => xs.flatMap((x) => (typeof x === 'number' ? i32(x) : x));
    const p = link(code(plain), 1024, 1);
    const s = link(code(shifted), 1024 + D, 1 + T);
    return {
      k: classifyFunction(p, s, linkShifts(p.module, s.module), 0, 0),
      instrs: disassemble(p.module.funcs[0]),
    };
  };
  const at = (instrs: Instr[], v: number): number =>
    instrs.find((i) => i.opcode === 0x41 && i.imms[0].kind === 's32' && i.imms[0].value === v)!.offset;

  it('reads a constant that did not move as an integer, one moved by D as an address, by T as a table slot', () => {
    const { k, instrs } = classify([7010, DROP, 2048, DROP, 16, DROP], [7010, DROP, 2048 + D, DROP, 16 + T, DROP]);
    expect([7010, 2048, 16].map((v) => operandKind(k, at(instrs, v)))).toEqual(['literal', 'address', 'slot']);
  });

  it('leaves the function kind-unverified when a constant moved by neither shift', () => {
    expect(classify([2048, DROP], [2048 + D + 4, DROP]).k).toEqual({
      verified: false,
      why: `+0x1 i32.const 2048 moved to ${2048 + D + 4}, by neither shift`,
    });
  });

  it("reads an unfolded __table_base sum's base and the constants relative to it", () => {
    const sum = (base: number): (number | number[])[] => [2035, 2036, LOCAL_GET, SELECT, base, ADD, DROP];
    const { k, instrs } = classify(sum(1), sum(1 + T));
    expect([2035, 2036, 1].map((v) => operandKind(k, at(instrs, v)))).toEqual(['table-rel', 'table-rel', 'table-base']);
  });

  it('leaves the function kind-unverified when another table slot is added to constants', () => {
    // binaryen folded 1 of the relative operands into the base: which part of 2 is the base cannot be told
    const sum = (base: number): (number | number[])[] => [2034, 2035, LOCAL_GET, SELECT, base, ADD, DROP];
    expect(classify(sum(2), sum(2 + T)).k).toEqual({
      verified: false,
      why: '+0xa i32.const 2 is a table slot added to constants: a __table_base sum with part of the base in it',
    });
    // a slot stored as a pointer is one
    const { k, instrs } = classify([LOCAL_GET, 2, [0x36, 2, 0]], [LOCAL_GET, 2 + T, [0x36, 2, 0]]);
    expect(operandKind(k, at(instrs, 2))).toBe('slot');
  });
});

describe('the table argument', () => {
  it('is required unless --no-kinds says to check without one', () => {
    const dir = mkdtempSync(join(tmpdir(), 'kinds-'));
    const plain = join(dir, 'wasm', 'vota_web', 'vota_web_wasm.wasm');
    expect(() => kindsPathOf(plain, undefined, false)).toThrow(/no operand-kind table at .*wasm-kinds\/vota_web\/kinds\.json/);
    expect(() => kindsPathOf(plain, join(dir, 'none.json'), false)).toThrow(/no operand-kind table/);
    expect(kindsPathOf(plain, undefined, true)).toBeNull();
    expect(() => kindsPathOf(plain, join(dir, 'none.json'), true)).toThrow(/--kinds and --no-kinds/);
    writeFileSync(join(dir, 'k.json'), '{}');
    expect(kindsPathOf(plain, join(dir, 'k.json'), false)).toBe(join(dir, 'k.json'));
  });

  it('is not skipped by a flag the command does not take', () => {
    expect(() => noMoreArgs('splat-build', [])).not.toThrow();
    expect(() => noMoreArgs('splat-build', ['--kinds /x'])).toThrow("splat-build: unexpected argument '--kinds /x'");
    // splat-build refuses before reading anything
    const dir = mkdtempSync(join(tmpdir(), 'kinds-'));
    const target = join(dir, 'target', 'x.wasm');
    const plain = join(dir, 'plain', 'x.wasm');
    expect(() => runSplatSubCommand('splat-build', [target, plain, '--no-kinds', '--kinds /x'])).toThrow(
      /unexpected argument '--kinds \/x'/,
    );
    expect(() => runSplatSubCommand('splat-build', [target, plain])).toThrow(/no operand-kind table/);
    // the limits take counts (an empty make variable would leave the flag without one)
    for (const flag of ['--max-kind-unverified']) {
      expect(() => runSplatSubCommand('splat-build', [target, plain, flag, '-o', 'x.wasm'])).toThrow(
        `${flag} takes a count, not '-o'`,
      );
    }
  });
});

describe('linkShifts', () => {
  it.skipIf(!real)('reads how far the relink moved the data and the table', () => {
    const { plain, shifted } = load();
    expect(linkShifts(plain.module, shifted.module)).toEqual({ dataShift: D, tableShift: T, dataBase: 1024, tableBase: 1 });
  });

  it.skipIf(!real)('refuses two links that are the same', () => {
    const { plain } = load();
    expect(() => linkShifts(plain.module, plain.module)).toThrow(/two different positive amounts/);
  });
});

describe('classifyFunction', { timeout: 180_000 }, () => {
  const kindsOf = (name: string, plainModule?: Module, shiftedModule?: Module): FunctionKinds => {
    const { plain, shifted } = load();
    const p = { ...plain, module: plainModule ?? plain.module };
    const s = { ...shifted, module: shiftedModule ?? shifted.module };
    return classifyFunction(p, s, linkShifts(plain.module, shifted.module), indexOf(plain, name), indexOf(shifted, name));
  };

  it.skipIf(!real)("tells api::CDirReader::NextEntry's enum, string, typeinfo and destructor pointer apart", () => {
    // the enum 7010 equals the target's slot of the function in our slot 1440 (case c2 below); 16 is __cxa_throw's
    // destructor argument, a table slot
    const k = kindsOf(NEXT_ENTRY);
    expect(k.verified).toBe(true);
    const kind = (at: number) => operandKind(k, at);
    expect([kind(0x1b), kind(0x23), kind(0x2a), kind(0x32), kind(0x37)]).toEqual([
      'literal',
      'address',
      'address',
      'address',
      'slot',
    ]);
    // the frame size and the field offsets are integers
    expect([kind(0x5), kind(0xe), kind(0x20), kind(0x46)]).toEqual(['literal', 'literal', 'literal', 'literal']);
  });

  it.skipIf(!real)('reads a vtable store as an address', () => {
    const k = kindsOf(DO_CONVERTE);
    expect([operandKind(k, 0x3), operandKind(k, 0xc)]).toEqual(['address', 'address']);
  });

  it.skipIf(!real)("reads an unfolded __table_base sum's base and the slots relative to it", () => {
    // sqlite's -fPIC objects add __table_base to select(2035, 2036, 2037): the base moves by T, the relative operands
    // do not, and alone they would read as literals
    const k = kindsOf('vdbeSorterSort');
    expect(k.verified).toBe(true);
    expect([0xa8, 0xab, 0xae, 0xbd].map((at) => operandKind(k, at))).toEqual([
      'table-rel',
      'table-rel',
      'table-rel',
      'table-base',
    ]);
    expect(operandKind(kindsOf('sqlite3VdbeExec'), 0x7df0)).toBe('table-base');
  });

  it.skipIf(!real)('leaves a function kind-unverified when a constant added to __table_base moved', () => {
    const { plain, shifted } = load();
    const p = indexOf(plain, 'vdbeSorterSort');
    const s = indexOf(shifted, 'vdbeSorterSort');
    const k = kindsOf('vdbeSorterSort', undefined, patched(shifted.module, s, shiftedAt(p, s, 0xae), 2037 + T));
    expect(k).toEqual({
      verified: false,
      // 2037 moved as a table slot: the base added to a slot, neither part can be told
      why: "+0xbd i32.const is the link's base itself, added to a table slot",
    });
  });

  it.skipIf(!real)('leaves a function kind-unverified when an operand moved by neither shift', () => {
    const { shifted } = load();
    const s = indexOf(shifted, NEXT_ENTRY);
    const at = shiftedAt(indexOf(load().plain, NEXT_ENTRY), s, 0x23);
    expect(valueAt(shifted.module, s, at)).toBe(64029 + D);
    const k = kindsOf(NEXT_ENTRY, undefined, patched(shifted.module, s, at, 64029 + D + 4));
    expect(k).toEqual({ verified: false, why: `+0x23 i32.const 64029 moved to ${64029 + D + 4}, by neither shift` });
  });

  it.skipIf(!real)('leaves a function kind-unverified when the relink has another shape', () => {
    const { shifted } = load();
    const s = indexOf(shifted, NEXT_ENTRY);
    // another callee of the same signature
    const at = shiftedAt(indexOf(load().plain, NEXT_ENTRY), s, 0x41);
    const k = kindsOf(NEXT_ENTRY, undefined, patched(shifted.module, s, at, indexOf(shifted, '_Znwm')));
    expect(k.verified).toBe(false);
    expect(!k.verified && k.why).toMatch(/^another shape in the shifted link: \+0x41 call: \S+ against _Znwm$/);
  });

  it.skipIf(!real)('leaves a function kind-unverified when the two slots hold different functions', () => {
    // the relink's table with slots 16 and 17 swapped: 16 still moves by T, to a slot naming another function
    const { shifted } = load();
    const elems = shifted.module.elems.map((e) => {
      if (e.mode !== 'active') {
        return e;
      }
      const funcIndices = [...e.funcIndices];
      const at = 16 + T - (e.offset!.value as number);
      [funcIndices[at], funcIndices[at + 1]] = [funcIndices[at + 1], funcIndices[at]];
      return { ...e, funcIndices };
    });
    expect(kindsOf(NEXT_ENTRY, undefined, { ...shifted.module, elems })).toEqual({
      verified: false,
      why: "+0x37 i32.const 16: the two links' slots hold different functions",
    });
  });
});

describe('classifyOperands', { timeout: 180_000 }, () => {
  it.skipIf(!real)('classifies every function of the link, every match kind-verified', () => {
    const { plain, shifted, kinds, maps } = load();
    const k = classifyOperands(plain, shifted);
    expect(renderOperandKinds(k)).toBe(readFileSync(KINDS, 'utf8'));
    expect(k.functions.size).toBe(plain.module.funcs.length);
    const { matches } = parseMatchList(readFileSync(`${URNA}/splat/matched.txt`, 'utf8'));
    const s = kindsSummary(kinds, plain.module, matches.map((m) => maps.ours.fnByKey.get(m.symbol)!));
    expect(s.functions - s.verified).toBe(0);
    expect(s.operands.address).toBeGreaterThan(20_000);
    expect(s.operands.slot).toBeGreaterThan(1_000);
    // vdbeSorterSort's
    expect([s.operands['table-base'], s.operands['table-rel']]).toEqual([1, 3]);
  });

  it.skipIf(!real)('writes a table that reads back the same, and refuses it for another plain link', () => {
    const { kinds, shifted } = load();
    expect(renderOperandKinds(parseOperandKinds(renderOperandKinds(kinds)))).toBe(renderOperandKinds(kinds));
    expect(() => readOperandKinds(KINDS, shifted.module.bytes)).toThrow(/describes another plain link/);
  });
});

describe('remapBody with operand kinds', { timeout: 180_000 }, () => {
  it.skipIf(!real)('offers an operand only the readings of its kind', () => {
    const { maps } = load();
    const ours = maps.ours.fnByKey.get(NEXT_ENTRY)!;
    const r = remapBody(maps, ours);
    expect(r.ok && r.kindUnverified).toBeUndefined();
    const ambiguous = r.ok ? r.pieces.filter((p): p is Exclude<Piece, Uint8Array> => !(p instanceof Uint8Array)) : [];
    // the enum offers nothing (a literal is fixed bytes); the string, the typeinfo and the vtable only addresses, the
    // destructor only its table slot
    expect(ambiguous.map((p) => p.at)).toEqual([0x23, 0x2a, 0x32, 0x37]);
    expect(ambiguous.every((p) => !p.readings.includes('literal'))).toBe(true);
    expect(ambiguous.at(-1)!.readings.every((x) => x.startsWith('table slot'))).toBe(true);
    const all = remapBody({ ...maps, kinds: undefined }, ours);
    expect(all.ok && all.kindUnverified).toBe('no operand-kind table');
    expect(all.ok && all.pieces.some((p) => !(p instanceof Uint8Array) && p.at === 0x1b)).toBe(true);
  });

  it.skipIf(!real)("offers an unfolded sum's base the target's table base, its relative slots minus it", () => {
    const { maps } = load();
    const r = remapBody(maps, maps.ours.fnByKey.get('vdbeSorterSort')!);
    const ambiguous = r.ok ? r.pieces.filter((p): p is Exclude<Piece, Uint8Array> => !(p instanceof Uint8Array)) : [];
    const at = (x: number) => ambiguous.find((p) => p.at === x)!.readings;
    expect(at(0xbd)).toEqual(['table base']);
    for (const x of [0xa8, 0xab, 0xae]) {
      expect(at(x).every((y) => /^table slot (copy \d+ )?of .* − the table base$/.test(y)), x.toString(16)).toBe(true);
    }
  });
});

// The perturbation cases. Without the operand-kind table (every reading offered), c2, c5, c8 and c9 build the target;
// with it, c2, c5 and c8 are rejected and c9 is a review warning. The others: the target's number written as an
// integer (c5L, c8L, I2, TH2), an integer of the target's written as a pointer (c2S), integers over vdbeSorterSort's
// relative slots (V1, V2), and a function the relink does not name. Each case: our function, the instruction, the
// original value, the value in the plain link and the value the shifted relink would have (by the operand's kind in
// the source).
interface Case {
  name: string;
  symbol: string;
  target: number;
  at?: number;
  was?: number | bigint;
  plain?: number | bigint | ((l: Loaded) => number);
  shifted?: number | bigint | ((l: Loaded) => number);
  /** the relink names no function after the symbol */
  unnamed?: boolean;
}
const CASES: Case[] = [
  { name: 'c0 identity', symbol: NEXT_ENTRY, target: 1260 },
  { name: 'c1 enum 7011', symbol: NEXT_ENTRY, target: 1260, at: 0x1b, was: 7010, plain: 7011, shifted: 7011 },
  // our slot 1440 holds the function the target keeps in slot 7010
  { name: 'c2 enum 1440', symbol: NEXT_ENTRY, target: 1260, at: 0x1b, was: 7010, plain: 1440, shifted: 1440 },
  { name: 'c3 enum 1622', symbol: INVERT_COLORS, target: 3663, at: 0xc7, was: 4903, plain: 1622, shifted: 1622 },
  { name: 'c4 wrong string', symbol: NEXT_ENTRY, target: 1260, at: 0x23, was: 64029, plain: 65300, shifted: 65300 + D },
  // 124328 is the target's address of the right string, and an address inside our merged strings
  { name: 'c5 string at the target number', symbol: NEXT_ENTRY, target: 1260, at: 0x23, was: 64029, plain: 124328, shifted: 124328 + D },
  {
    name: 'c6 wrong callee',
    symbol: NEXT_ENTRY,
    target: 1260,
    at: 0x41,
    plain: (l) => l.maps.ours.fnByKey.get('_Znwm')!,
    shifted: (l) => indexOf(l.shifted, '_Znwm'),
  },
  { name: 'c7 wrong slot 17', symbol: NEXT_ENTRY, target: 1260, at: 0x37, was: 16, plain: 17, shifted: 17 + T },
  // 69 is the target's slot of the destructor
  { name: 'c8 slot 69, the target number', symbol: NEXT_ENTRY, target: 1260, at: 0x37, was: 16, plain: 69, shifted: 69 + T },
  // the target's vtable address written as an integer: past our data, so no reading of ours names it
  { name: 'c9 the target vtable address', symbol: DO_CONVERTE, target: 11386, at: 0xc, was: 1212884, plain: 1569340, shifted: 1569340 },
  { name: 'c10 vtable + 4', symbol: DO_CONVERTE, target: 11386, at: 0xc, was: 1212884, plain: 1212888, shifted: 1212888 + D },
  // the target's numbers written as integers: the string's address, the destructor's slot, a packed string view
  // (length 9 << 32 | the target's address 3395) and a string's address in a binaryen thunk
  { name: 'c5L the string address as an integer', symbol: NEXT_ENTRY, target: 1260, at: 0x23, was: 64029, plain: 124328, shifted: 124328 },
  { name: 'c8L the slot as an integer', symbol: NEXT_ENTRY, target: 1260, at: 0x37, was: 16, plain: 69, shifted: 69 },
  {
    name: 'I2 the string view as an integer',
    symbol: RAW_READ,
    target: 598,
    at: 0x25f,
    was: (9n << 32n) | 1096n,
    plain: (9n << 32n) | 3395n,
    shifted: (9n << 32n) | 3395n,
  },
  { name: 'TH2 a thunk string as an integer', symbol: DSA_PEM_THUNK, target: 1724, at: 0x3a, was: 105737, plain: 187586, shifted: 187586 },
  // the enum 7010 written as a pointer to the function in our slot 1440, which the target keeps in slot 7010
  { name: 'c2S the enum as a pointer', symbol: NEXT_ENTRY, target: 1260, at: 0x1b, was: 7010, plain: 1440, shifted: 1440 + T },
  // integers over vdbeSorterSort's relative slots: the table reads them as table-rel, relative to the base, and the
  // slots that makes of them (our 11886 and 11884) are past our table
  { name: 'V1 a relative slot as an integer', symbol: 'vdbeSorterSort', target: 6458, at: 0xae, was: 2037, plain: 11885, shifted: 11885 },
  { name: 'V2 another', symbol: 'vdbeSorterSort', target: 6458, at: 0xa8, was: 2035, plain: 11883, shifted: 11883 },
  { name: 'U c2 in a function the relink does not name', symbol: NEXT_ENTRY, target: 1260, at: 0x1b, was: 7010, plain: 1440, shifted: 1440, unnamed: true },
];

/** The maps of the case: our module patched, and its function's kinds from the patched pair of links. */
function caseMaps(c: Case): { maps: SymbolMaps; kinds: FunctionKinds } {
  const l = load();
  const p = indexOf(l.plain, c.symbol);
  const s = indexOf(l.shifted, c.symbol);
  if (c.at === undefined) {
    return { maps: l.maps, kinds: l.kinds.functions.get(p)! };
  }
  if (c.was !== undefined) {
    expect(valueAt(l.plain.module, p, c.at)).toBe(c.was);
  }
  const v = (x: Case['plain']): number | bigint => (typeof x === 'function' ? x(l) : x!);
  const plainModule = patched(l.plain.module, p, c.at, v(c.plain));
  const shiftedModule = patched(l.shifted.module, s, shiftedAt(p, s, c.at), v(c.shifted));
  const pair = { ...l.plain, module: plainModule };
  const names = new Map(l.shifted.names);
  if (c.unnamed) {
    names.delete(s);
  }
  const kinds = c.unnamed
    ? classifyOperands(pair, { module: shiftedModule, names }).functions.get(p)!
    : classifyFunction(pair, { ...l.shifted, module: shiftedModule }, linkShifts(l.plain.module, l.shifted.module), p, s);
  const functions = new Map(l.kinds.functions);
  functions.set(p, kinds);
  return {
    maps: {
      ...l.maps,
      ours: { ...l.maps.ours, module: plainModule },
      kinds: { ...l.kinds, functions },
    },
    kinds,
  };
}

function build(c: Case, maps: SymbolMaps) {
  return buildSplat(maps, PIECES, parseMatchList(`${c.target} ${c.symbol}\n`).matches, TARGET_SHA);
}

function failure(c: Case, maps: SymbolMaps): string {
  try {
    build(c, maps);
  } catch (e) {
    expect(e).toBeInstanceOf(MatchesFailed);
    return (e as MatchesFailed).message;
  }
  return 'built';
}

describe('the perturbation cases', { timeout: 300_000 }, () => {
  const byName = (prefix: string): Case => CASES.find((c) => c.name.startsWith(`${prefix} `))!;

  it.skipIf(!real)('c0: the unpatched function builds the target, kind-verified, with no review warning', () => {
    const c = byName('c0');
    const r = build(c, caseMaps(c).maps);
    expect(r.assembled.sha256).toBe(TARGET_SHA);
    expect(r.kindUnverified).toEqual([]);
    expect(r.reviewWarnings).toEqual([]);
  });

  it.skipIf(!real)('c2, c5, c8: a value written as another kind than the target number is rejected', () => {
    // every reading offered (no table): each one builds the target
    for (const name of ['c2', 'c5', 'c8']) {
      expect(failure(byName(name), { ...caseMaps(byName(name)).maps, kinds: undefined }), name).toBe('built');
    }
    // c2: an integer that our table slot reading would have turned into the target's 7010
    const c2 = caseMaps(byName('c2'));
    expect(operandKind(c2.kinds, 0x1b)).toBe('literal');
    expect(failure(byName('c2'), c2.maps)).toMatch(
      /function 1260 .* DIFFERS at \+0x1c target \+0x1b i32\.const 7010 \| ours \+0x1b i32\.const 1440 \[ours 1440: an integer/,
    );
    // c5: an address whose literal is the target's number for the right string
    const c5 = caseMaps(byName('c5'));
    expect(operandKind(c5.kinds, 0x23)).toBe('address');
    expect(failure(byName('c5'), c5.maps)).toMatch(
      /function 1260 .* DIFFERS at \+0x24 target \+0x23 i32\.const 124328 \| ours \+0x23 i32\.const 207494 \[ours 124328 = .*: address string/,
    );
    // c8: a table slot whose literal is the target's slot number
    const c8 = caseMaps(byName('c8'));
    expect(operandKind(c8.kinds, 0x37)).toBe('slot');
    expect(failure(byName('c8'), c8.maps)).toMatch(
      /function 1260 .* DIFFERS at \+0x38 target \+0x37 i32\.const 69 \| ours \+0x37 i32\.const 3599$/m,
    );
  });

  it.skipIf(!real)('c1, c3, c4, c6, c7, c10: a wrong value that coincides with nothing stays rejected', () => {
    for (const name of ['c1', 'c3', 'c4', 'c6', 'c7', 'c10']) {
      const c = byName(name);
      const { maps, kinds } = caseMaps(c);
      expect(kinds.verified, name).toBe(true);
      expect(failure(c, maps), name).toMatch(new RegExp(`function ${c.target} .*(DIFFERS|UNRESOLVED)`));
    }
  });

  it.skipIf(!real)('c9, c5L, I2, TH2: the target number written as an integer builds, and is a review warning', () => {
    // an integer is compared as itself, so the bytes agree: the check cannot reject these, it lists them
    const expected: Record<string, [string, number]> = {
      c9: ['past-data', 1569340],
      c5L: ['target-object', 124328],
      I2: ['target-object', 3395],
      TH2: ['target-object', 187586],
    };
    for (const [name, [why, value]] of Object.entries(expected)) {
      const c = byName(name);
      const { maps, kinds } = caseMaps(c);
      expect(operandKind(kinds, c.at!), name).toBe('literal');
      const r = build(c, maps);
      expect(r.assembled.sha256, name).toBe(TARGET_SHA);
      expect(r.kindUnverified, name).toEqual([]);
      expect(
        r.reviewWarnings.map((w) => [w.match.index, w.at, w.why, w.value]),
        name,
      ).toEqual([[c.target, c.at, why, value]]);
    }
    // the same value where the source wrote an address (the relink moved it): no target address of ours, rejected
    const c9 = byName('c9');
    const address = { ...c9, shifted: 1569340 + D };
    const a = caseMaps(address);
    expect(operandKind(a.kinds, 0xc)).toBe('address');
    expect(failure(address, a.maps)).toMatch(/function 11386 .* is UNRESOLVED: ours \+0xc i32\.const 1569340: an address of ours/);
  });

  it.skipIf(!real)('an integer in the target table past ours is a review warning', () => {
    const c = { ...byName('c1'), plain: 12000, shifted: 12000 };
    const { maps } = caseMaps(c);
    const matched = new Map([[1260, { match: parseMatchList(`1260 ${NEXT_ENTRY}\n`).matches[0], ours: indexOf(load().plain, NEXT_ENTRY) }]]);
    expect(reviewWarnings(maps, matched).map((w) => [w.at, w.why, w.value])).toEqual([[0x1b, 'past-table', 12000]]);
  });

  it.skipIf(!real)('c8L, c2S: the limits of the check build the target, with no review warning', () => {
    // c8L: the target's slot 69 written as an integer, inside our table (every small integer equals some target slot);
    // c2S: an integer of the target's (7010) written as a pointer to the function the target keeps in that slot (our
    // kind is checked, the target's cannot be)
    for (const [name, kind] of [
      ['c8L', 'literal'],
      ['c2S', 'slot'],
    ] as const) {
      const c = byName(name);
      const { maps, kinds } = caseMaps(c);
      expect(operandKind(kinds, c.at!), name).toBe(kind);
      const r = build(c, maps);
      expect(r.assembled.sha256, name).toBe(TARGET_SHA);
      expect([r.kindUnverified, r.reviewWarnings], name).toEqual([[], []]);
    }
  });

  it.skipIf(!real)("V1, V2: an integer over vdbeSorterSort's relative slots is read relative to the base and rejected", () => {
    const unchanged = build(byName('V1'), load().maps);
    expect([unchanged.assembled.sha256, unchanged.kindUnverified]).toEqual([TARGET_SHA, []]);
    for (const [name, at, value] of [
      ['V1', 0xae, 11885],
      ['V2', 0xa8, 11883],
    ] as const) {
      const c = byName(name);
      const { maps, kinds } = caseMaps(c);
      expect(kinds, name).toEqual({ verified: true, ops: expect.any(Map) });
      expect(operandKind(kinds, at), name).toBe('table-rel');
      expect(failure(c, maps), name).toMatch(
        new RegExp(`function 6458 .* UNRESOLVED: ours \\+0x${at.toString(16)} i32\\.const ${value}: a table slot relative to the base of ours`),
      );
    }
  });

  it.skipIf(!real)('a function the relink does not name is kind-unverified: it builds with every reading, over the limit', () => {
    const c = byName('U');
    const { maps, kinds } = caseMaps(c);
    expect(kinds).toEqual({ verified: false, why: `no functions of the shifted link are ${NEXT_ENTRY}` });
    const r = build(c, maps);
    expect(r.assembled.sha256).toBe(TARGET_SHA);
    expect(r.kindUnverified.map((u) => [u.match.index, u.why])).toEqual([[1260, kinds.verified ? '' : kinds.why]]);
    expect(kindUnverifiedOverLimit(r, 1)).toBeNull();
    expect(kindUnverifiedOverLimit(r, 0)).toMatch(
      /^1 matches are kind-unverified, more than the 0 allowed \(--max-kind-unverified\)[^]*\n {2}line 1: function 1260 /,
    );
  });
});

describe('table slots read with operand kinds', { timeout: 300_000 }, () => {
  it.skipIf(!real)("names the functions in vdbeSorterSort's relative slots: the table base plus each", () => {
    const { maps } = load();
    const ours = maps.ours.fnByKey.get('vdbeSorterSort')!;
    const a = attempt(maps, { ours, symbol: 'vdbeSorterSort', source: 'symbol' }, 6458, readPieceBodies(PIECES).get(6458)!);
    expect(a.verdict).toBe('match');
    const refs = referencedFunctionsAt(maps, ours, a.verdict === 'match' ? a.chosen : []);
    const named = (at: number) => refs.filter((r) => r.at === at).map((r) => maps.ours.fns.get(r.f)?.key);
    // our slots 2035, 2036 and 2037 hold sqlite3InitCallback, vdbeSorterCompareInt and vdbeSorterCompareText
    expect([0xa8, 0xab, 0xae].map(named)).toEqual([['vdbeSorterCompareInt'], ['vdbeSorterCompareText'], ['vdbeSorterCompare']]);
  });

  it.skipIf(!real)('offers a table slot every target slot of its function, however many', () => {
    // sqlite3_initialize's +0x32 names the empty function binaryen merged the target's into, which fills 126 slots
    const { maps } = load();
    const ours = maps.ours.fnByKey.get('sqlite3_initialize')!;
    const piece = (m: SymbolMaps) => {
      const r = remapBody(m, ours);
      return r.ok ? r.pieces.find((p) => !(p instanceof Uint8Array) && p.at === 0x32) : undefined;
    };
    const p = piece(maps);
    expect(p !== undefined && !(p instanceof Uint8Array) && p.alternatives.length).toBe(126);
    const a = attempt(maps, { ours, symbol: 'sqlite3_initialize', source: 'symbol' }, 250, readPieceBodies(PIECES).get(250)!);
    expect(a.verdict).toBe('match');
    // an operand that may be anything is offered a function's slots only while they are few (here an address too)
    const unverified = piece({ ...maps, kinds: undefined });
    expect(unverified !== undefined && !(unverified instanceof Uint8Array) && unverified.readings).toEqual([
      'address string',
      expect.stringMatching(/^address same-content /),
      'literal',
    ]);
  });

  it.skipIf(!real)("reads 2077 in sqlite3AddDefaultValue as a table slot, as both links' tables say", () => {
    // its object as a whole has address relocations, which does not make this operand an address
    const { kinds, maps, plain } = load();
    const p = indexOf(plain, 'sqlite3AddDefaultValue');
    expect(valueAt(plain.module, p, 0x292)).toBe(2077);
    expect(operandKind(kinds.functions.get(p), 0x292)).toBe('slot');
    expect(maps.ours.fns.get(maps.ours.elem.get(2077)!)?.key).toBe('renameUnmapSelectCb');
  });
});

describe('buildSplat with operand kinds', { timeout: 300_000 }, () => {
  it.skipIf(!real)('reports the kind-unverified matches and the review warnings apart', () => {
    const { maps } = load();
    const { matches } = parseMatchList('162 sqlite3ErrorMsg\n6458 vdbeSorterSort\n3418 LzmaEnc_WriteProperties\n');
    const r = buildSplat(maps, PIECES, matches, TARGET_SHA);
    expect(r.matched).toBe(3);
    expect(r.kindUnverified).toEqual([]);
    // LZMA's dictionary-size bounds: 3 << 12 (+ 1) past our table, 3 << 19 (+ 1) past our data
    expect(r.reviewWarnings.map((w) => [w.match.index, w.why, w.value])).toEqual([
      [3418, 'past-table', 12288],
      [3418, 'past-table', 12289],
      [3418, 'past-data', 1572864],
      [3418, 'past-data', 1572865],
    ]);
    // without a table every match is kind-unverified, and still builds; a limit below their count fails the build
    const none = buildSplat({ ...maps, kinds: undefined }, PIECES, matches, TARGET_SHA);
    expect(none.kindUnverified.map((u) => u.why)).toEqual(Array(3).fill('no operand-kind table'));
    const bodies = readPieceBodies(PIECES);
    expect(none.kindUnverifiedBytes).toBe([162, 6458, 3418].reduce((n, t) => n + bodies.get(t)!.length, 0));
    expect(kindUnverifiedOverLimit(none, 3)).toBeNull();
    expect(kindUnverifiedOverLimit(none, 2)?.split('\n')).toHaveLength(5);
  });
});

describe('splat-kinds', { timeout: 300_000 }, () => {
  it.skipIf(!real)('refuses names that do not describe the links it compares', () => {
    const dir = mkdtempSync(join(tmpdir(), 'kinds-'));
    for (const f of ['vota_web_wasm.wasm', 'names.tsv']) {
      copyFileSync(join(SHIFTED_DIR, f), join(dir, f));
    }
    const out = join(dir, 'kinds.json');
    writeFileSync(join(dir, 'names.sha256'), `${'0'.repeat(64)}\n`);
    expect(() => runSplatSubCommand('splat-kinds', [PLAIN, '--shifted', dir, '-o', out])).toThrow(
      /names\.tsv does not describe its vota_web_wasm\.wasm/,
    );
    copyFileSync(join(SHIFTED_DIR, 'names.sha256'), join(dir, 'names.sha256'));
    // the shifted link's names offered as the plain link's
    expect(() => runSplatSubCommand('splat-kinds', [PLAIN, '--shifted', dir, '--map-dir', SHIFTED_DIR, '-o', out])).toThrow(
      /does not describe .*vota_web_wasm\.wasm: run make -C src link-map/,
    );
    expect(existsSync(out)).toBe(false);
  });
});
