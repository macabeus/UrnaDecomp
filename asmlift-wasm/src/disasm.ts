// Decode one function body into an instruction stream. Every immediate keeps the byte offset it was
// read from, because a relocatable object's relocations point AT the immediate's LEB, and the only
// way to pair a reloc with its instruction is by that offset.
//
// Coverage: MVP, sign-extension, non-trapping float-to-int, bulk memory, reference types, multivalue
// block types, tail calls and the legacy exception-handling opcodes. SIMD (0xFD) and atomics (0xFE)
// are refused loudly with the offset — a silent skip would desynchronise every row after it.
import { type Cursor, DecodeError, f32bits, f64bits, s32, s33, s64, u8, u32 } from './leb';
import type { Func, Module, ValType } from './parse';

export type BlockType = { kind: 'empty' } | { kind: 'val'; type: ValType } | { kind: 'type'; index: number };

export type Imm =
  | { kind: 'u32'; value: number; at: number; role: ImmRole }
  | { kind: 's32'; value: number; at: number; role: 's32' }
  | { kind: 's64'; value: bigint; at: number; role: 's64' }
  | { kind: 'f32'; bits: number; at: number; role: 'f32' }
  | { kind: 'f64'; bits: bigint; at: number; role: 'f64' }
  | { kind: 'blocktype'; value: BlockType; at: number; role: 'blocktype' }
  | { kind: 'memarg'; align: number; offset: number; memory: number; at: number; offsetAt: number; role: 'memarg' }
  | { kind: 'labels'; labels: number[]; default: number; at: number; role: 'labels' }
  | { kind: 'valtypes'; types: ValType[]; at: number; role: 'valtypes' }
  | { kind: 'reftype'; type: ValType; at: number; role: 'reftype' };

/** What a u32 immediate NAMES, so normalisation knows which index space to symbolise it against. */
export type ImmRole =
  | 'label'
  | 'func'
  | 'type'
  | 'table'
  | 'local'
  | 'global'
  | 'memory'
  | 'data'
  | 'elem'
  | 'tag'
  | 'u32'
  | 's32'
  | 's64'
  | 'f32'
  | 'f64'
  | 'blocktype'
  | 'memarg'
  | 'labels'
  | 'valtypes'
  | 'reftype';

export interface Instr {
  /** byte offset of the opcode, relative to the body start (`Func.bodyOffset`) */
  offset: number;
  /** byte offset one past the last immediate */
  end: number;
  /** the opcode; prefixed opcodes are `(prefix << 8) | sub` (0xfc08 = memory.init) */
  opcode: number;
  name: string;
  imms: Imm[];
}

type ImmSpec =
  | 'bt'
  | 'label'
  | 'labels'
  | 'func'
  | 'type'
  | 'table'
  | 'local'
  | 'global'
  | 'memarg'
  | 'zero'
  | 's32'
  | 's64'
  | 'f32'
  | 'f64'
  | 'valtypes'
  | 'reftype'
  | 'data'
  | 'elem'
  | 'tag'
  | 'memory';

const OPS: Record<number, [string, ...ImmSpec[]]> = {
  0x00: ['unreachable'],
  0x01: ['nop'],
  0x02: ['block', 'bt'],
  0x03: ['loop', 'bt'],
  0x04: ['if', 'bt'],
  0x05: ['else'],
  0x06: ['try', 'bt'],
  0x07: ['catch', 'tag'],
  0x08: ['throw', 'tag'],
  0x09: ['rethrow', 'label'],
  0x0a: ['throw_ref'],
  0x0b: ['end'],
  0x0c: ['br', 'label'],
  0x0d: ['br_if', 'label'],
  0x0e: ['br_table', 'labels'],
  0x0f: ['return'],
  0x10: ['call', 'func'],
  0x11: ['call_indirect', 'type', 'table'],
  0x12: ['return_call', 'func'],
  0x13: ['return_call_indirect', 'type', 'table'],
  0x18: ['delegate', 'label'],
  0x19: ['catch_all'],
  0x1a: ['drop'],
  0x1b: ['select'],
  0x1c: ['select', 'valtypes'],
  0x20: ['local.get', 'local'],
  0x21: ['local.set', 'local'],
  0x22: ['local.tee', 'local'],
  0x23: ['global.get', 'global'],
  0x24: ['global.set', 'global'],
  0x25: ['table.get', 'table'],
  0x26: ['table.set', 'table'],
  0x28: ['i32.load', 'memarg'],
  0x29: ['i64.load', 'memarg'],
  0x2a: ['f32.load', 'memarg'],
  0x2b: ['f64.load', 'memarg'],
  0x2c: ['i32.load8_s', 'memarg'],
  0x2d: ['i32.load8_u', 'memarg'],
  0x2e: ['i32.load16_s', 'memarg'],
  0x2f: ['i32.load16_u', 'memarg'],
  0x30: ['i64.load8_s', 'memarg'],
  0x31: ['i64.load8_u', 'memarg'],
  0x32: ['i64.load16_s', 'memarg'],
  0x33: ['i64.load16_u', 'memarg'],
  0x34: ['i64.load32_s', 'memarg'],
  0x35: ['i64.load32_u', 'memarg'],
  0x36: ['i32.store', 'memarg'],
  0x37: ['i64.store', 'memarg'],
  0x38: ['f32.store', 'memarg'],
  0x39: ['f64.store', 'memarg'],
  0x3a: ['i32.store8', 'memarg'],
  0x3b: ['i32.store16', 'memarg'],
  0x3c: ['i64.store8', 'memarg'],
  0x3d: ['i64.store16', 'memarg'],
  0x3e: ['i64.store32', 'memarg'],
  0x3f: ['memory.size', 'memory'],
  0x40: ['memory.grow', 'memory'],
  0x41: ['i32.const', 's32'],
  0x42: ['i64.const', 's64'],
  0x43: ['f32.const', 'f32'],
  0x44: ['f64.const', 'f64'],
  0xd0: ['ref.null', 'reftype'],
  0xd1: ['ref.is_null'],
  0xd2: ['ref.func', 'func'],
};

const NUMERIC: string[] = [
  // 0x45
  'i32.eqz',
  'i32.eq',
  'i32.ne',
  'i32.lt_s',
  'i32.lt_u',
  'i32.gt_s',
  'i32.gt_u',
  'i32.le_s',
  'i32.le_u',
  'i32.ge_s',
  'i32.ge_u',
  // 0x50
  'i64.eqz',
  'i64.eq',
  'i64.ne',
  'i64.lt_s',
  'i64.lt_u',
  'i64.gt_s',
  'i64.gt_u',
  'i64.le_s',
  'i64.le_u',
  'i64.ge_s',
  'i64.ge_u',
  // 0x5b
  'f32.eq',
  'f32.ne',
  'f32.lt',
  'f32.gt',
  'f32.le',
  'f32.ge',
  // 0x61
  'f64.eq',
  'f64.ne',
  'f64.lt',
  'f64.gt',
  'f64.le',
  'f64.ge',
  // 0x67
  'i32.clz',
  'i32.ctz',
  'i32.popcnt',
  'i32.add',
  'i32.sub',
  'i32.mul',
  'i32.div_s',
  'i32.div_u',
  'i32.rem_s',
  'i32.rem_u',
  'i32.and',
  'i32.or',
  'i32.xor',
  'i32.shl',
  'i32.shr_s',
  'i32.shr_u',
  'i32.rotl',
  'i32.rotr',
  // 0x79
  'i64.clz',
  'i64.ctz',
  'i64.popcnt',
  'i64.add',
  'i64.sub',
  'i64.mul',
  'i64.div_s',
  'i64.div_u',
  'i64.rem_s',
  'i64.rem_u',
  'i64.and',
  'i64.or',
  'i64.xor',
  'i64.shl',
  'i64.shr_s',
  'i64.shr_u',
  'i64.rotl',
  'i64.rotr',
  // 0x8b
  'f32.abs',
  'f32.neg',
  'f32.ceil',
  'f32.floor',
  'f32.trunc',
  'f32.nearest',
  'f32.sqrt',
  'f32.add',
  'f32.sub',
  'f32.mul',
  'f32.div',
  'f32.min',
  'f32.max',
  'f32.copysign',
  // 0x99
  'f64.abs',
  'f64.neg',
  'f64.ceil',
  'f64.floor',
  'f64.trunc',
  'f64.nearest',
  'f64.sqrt',
  'f64.add',
  'f64.sub',
  'f64.mul',
  'f64.div',
  'f64.min',
  'f64.max',
  'f64.copysign',
  // 0xa7
  'i32.wrap_i64',
  'i32.trunc_f32_s',
  'i32.trunc_f32_u',
  'i32.trunc_f64_s',
  'i32.trunc_f64_u',
  'i64.extend_i32_s',
  'i64.extend_i32_u',
  'i64.trunc_f32_s',
  'i64.trunc_f32_u',
  'i64.trunc_f64_s',
  'i64.trunc_f64_u',
  'f32.convert_i32_s',
  'f32.convert_i32_u',
  'f32.convert_i64_s',
  'f32.convert_i64_u',
  'f32.demote_f64',
  'f64.convert_i32_s',
  'f64.convert_i32_u',
  'f64.convert_i64_s',
  'f64.convert_i64_u',
  'f64.promote_f32',
  'i32.reinterpret_f32',
  'i64.reinterpret_f64',
  'f32.reinterpret_i32',
  'f64.reinterpret_i64',
  // 0xc0
  'i32.extend8_s',
  'i32.extend16_s',
  'i64.extend8_s',
  'i64.extend16_s',
  'i64.extend32_s',
];
for (let i = 0; i < NUMERIC.length; i++) {
  OPS[0x45 + i] = [NUMERIC[i]];
}

const FC_OPS: Record<number, [string, ...ImmSpec[]]> = {
  0: ['i32.trunc_sat_f32_s'],
  1: ['i32.trunc_sat_f32_u'],
  2: ['i32.trunc_sat_f64_s'],
  3: ['i32.trunc_sat_f64_u'],
  4: ['i64.trunc_sat_f32_s'],
  5: ['i64.trunc_sat_f32_u'],
  6: ['i64.trunc_sat_f64_s'],
  7: ['i64.trunc_sat_f64_u'],
  8: ['memory.init', 'data', 'memory'],
  9: ['data.drop', 'data'],
  10: ['memory.copy', 'memory', 'memory'],
  11: ['memory.fill', 'memory'],
  12: ['table.init', 'elem', 'table'],
  13: ['elem.drop', 'elem'],
  14: ['table.copy', 'table', 'table'],
  15: ['table.grow', 'table'],
  16: ['table.size', 'table'],
  17: ['table.fill', 'table'],
};

const VALTYPES: Record<number, ValType> = {
  0x7f: 'i32',
  0x7e: 'i64',
  0x7d: 'f32',
  0x7c: 'f64',
  0x7b: 'v128',
  0x70: 'funcref',
  0x6f: 'externref',
  0x69: 'exnref',
};

/** A block type is a SIGNED 33-bit LEB: 0x40 (= s33 -64) is empty, the valtype bytes (0x7f.. = s33 -1..)
 *  are single results, and anything ≥ 0 is a type index — so `block (type 64)` is NOT the byte 0x40
 *  but the two bytes 0xc0 0x00. Reading one byte would misread every multivalue block. */
function blockType(c: Cursor): BlockType {
  const at = c.pos;
  const v = s33(c);
  if (v >= 0) {
    return { kind: 'type', index: v };
  }
  if (v === -64) {
    return { kind: 'empty' };
  }
  const t = VALTYPES[(v & 0x7f) | 0];
  if (t === undefined) {
    throw new DecodeError(`unknown block type ${v}`, at);
  }
  return { kind: 'val', type: t };
}

function readImm(spec: ImmSpec, c: Cursor): Imm {
  const at = c.pos;
  switch (spec) {
    case 'bt':
      return { kind: 'blocktype', value: blockType(c), at, role: 'blocktype' };
    case 'label':
    case 'func':
    case 'type':
    case 'table':
    case 'local':
    case 'global':
    case 'data':
    case 'elem':
    case 'tag':
    case 'memory':
      return { kind: 'u32', value: u32(c), at, role: spec };
    case 'zero': {
      const b = u8(c);
      if (b !== 0) {
        throw new DecodeError(`reserved byte is 0x${b.toString(16)}, not 0`, at);
      }
      return { kind: 'u32', value: 0, at, role: 'u32' };
    }
    case 'labels': {
      const n = u32(c);
      if (n > 1_000_000) {
        throw new DecodeError(`implausible br_table arity ${n}`, at);
      }
      const labels = new Array<number>(n);
      for (let i = 0; i < n; i++) {
        labels[i] = u32(c);
      }
      return { kind: 'labels', labels, default: u32(c), at, role: 'labels' };
    }
    case 'memarg': {
      const align = u32(c);
      // multi-memory: bit 6 of align flags a memory index before the offset
      const memory = align & 0x40 ? u32(c) : 0;
      // a relocation on a load/store points at the OFFSET's LEB, not at the instruction's first immediate
      const offsetAt = c.pos;
      const offset = u32(c);
      return { kind: 'memarg', align: align & 0x3f, offset, memory, at, offsetAt, role: 'memarg' };
    }
    case 's32':
      return { kind: 's32', value: s32(c), at, role: 's32' };
    case 's64':
      return { kind: 's64', value: s64(c), at, role: 's64' };
    case 'f32': {
      if (c.pos + 4 > c.bytes.length) {
        throw new DecodeError('truncated f32.const', at);
      }
      const bits = f32bits(c.bytes, c.pos);
      c.pos += 4;
      return { kind: 'f32', bits, at, role: 'f32' };
    }
    case 'f64': {
      if (c.pos + 8 > c.bytes.length) {
        throw new DecodeError('truncated f64.const', at);
      }
      const bits = f64bits(c.bytes, c.pos);
      c.pos += 8;
      return { kind: 'f64', bits, at, role: 'f64' };
    }
    case 'valtypes': {
      const n = u32(c);
      const types: ValType[] = [];
      for (let i = 0; i < n; i++) {
        types.push(refOrVal(c));
      }
      return { kind: 'valtypes', types, at, role: 'valtypes' };
    }
    case 'reftype':
      return { kind: 'reftype', type: refOrVal(c), at, role: 'reftype' };
  }
}

function refOrVal(c: Cursor): ValType {
  const at = c.pos;
  const b = u8(c);
  const t = VALTYPES[b];
  if (t === undefined) {
    throw new DecodeError(`unknown value type 0x${b.toString(16)}`, at);
  }
  return t;
}

/** Decode the instructions of `fn` (its locals are already in the record). Offsets are relative to
 *  `fn.bodyOffset`; add it for a file offset. The stream ends with the function's closing `end`. */
export function disassemble(fn: Func): Instr[] {
  const c: Cursor = { bytes: fn.bodyBytes, pos: fn.codeStart };
  const out: Instr[] = [];
  let depth = 0;
  while (c.pos < c.bytes.length) {
    const offset = c.pos;
    let opcode = u8(c);
    let spec: [string, ...ImmSpec[]] | undefined;
    if (opcode === 0xfc) {
      const sub = u32(c);
      spec = FC_OPS[sub];
      opcode = 0xfc00 | sub;
      if (spec === undefined) {
        throw new DecodeError(`unknown 0xfc opcode ${sub}`, fn.bodyOffset + offset);
      }
    } else if (opcode === 0xfd || opcode === 0xfe) {
      throw new DecodeError(
        `${opcode === 0xfd ? 'SIMD' : 'atomic'} opcode prefix 0x${opcode.toString(16)} is not decoded by this disassembler`,
        fn.bodyOffset + offset,
      );
    } else {
      spec = OPS[opcode];
      if (spec === undefined) {
        throw new DecodeError(`unknown opcode 0x${opcode.toString(16)}`, fn.bodyOffset + offset);
      }
    }
    const [name, ...immSpecs] = spec;
    const imms: Imm[] = [];
    for (const s of immSpecs) {
      try {
        imms.push(readImm(s, c));
      } catch (e) {
        if (e instanceof DecodeError) {
          throw new DecodeError(`${name}: ${e.message.replace(/ at byte offset .*$/, '')}`, fn.bodyOffset + e.offset);
        }
        throw e;
      }
    }
    out.push({ offset, end: c.pos, opcode, name, imms });
    if (opcode === 0x02 || opcode === 0x03 || opcode === 0x04 || opcode === 0x06) {
      depth++;
    } else if (opcode === 0x0b) {
      if (depth === 0) {
        if (c.pos !== c.bytes.length) {
          throw new DecodeError(
            `function ends at 0x${c.pos.toString(16)} but body has ${c.bytes.length - c.pos} trailing bytes`,
            fn.bodyOffset + c.pos,
          );
        }
        return out;
      }
      depth--;
    }
  }
  throw new DecodeError('function body has no closing `end`', fn.bodyOffset + c.pos);
}

export function blockTypeText(bt: BlockType, m?: Module): string {
  switch (bt.kind) {
    case 'empty':
      return '';
    case 'val':
      return `(result ${bt.type})`;
    case 'type': {
      const t = m?.types[bt.index];
      return t === undefined
        ? `(type ${bt.index})`
        : `(type ${bt.index}: (${t.params.join(' ')}) -> (${t.results.join(' ')}))`;
    }
  }
}

/** The literal immediate text, before any symbolisation. */
export function immText(imm: Imm, m?: Module): string {
  switch (imm.kind) {
    case 'u32':
      return String(imm.value);
    case 's32':
      return String(imm.value);
    case 's64':
      return String(imm.value);
    case 'f32':
      return `0x${imm.bits.toString(16).padStart(8, '0')}`;
    case 'f64':
      return `0x${imm.bits.toString(16).padStart(16, '0')}`;
    case 'blocktype':
      return blockTypeText(imm.value, m);
    case 'memarg':
      return `${imm.memory ? `mem=${imm.memory} ` : ''}align=${imm.align} offset=${imm.offset}`;
    case 'labels':
      return `[${imm.labels.join(' ')}] ${imm.default}`;
    case 'valtypes':
      return imm.types.join(' ');
    case 'reftype':
      return imm.type;
  }
}

/** One instruction as text, with raw (unsymbolised) immediates. */
export function instrText(i: Instr, m?: Module): string {
  const parts = i.imms.map((imm) => immText(imm, m)).filter((s) => s !== '');
  return parts.length === 0 ? i.name : `${i.name} ${parts.join(' ')}`;
}
