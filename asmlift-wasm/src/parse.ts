// A WebAssembly binary module parser with no dependencies. It reads the whole module into plain
// records; function bodies stay as byte views into the input (`bodyBytes`) and are decoded on
// demand by disasm.ts. Relocatable objects (`emcc -c`, `clang --target=wasm32 -c`) are the same
// format plus custom sections (`linking`, `reloc.CODE`, `reloc.DATA`), parsed here too so a
// candidate object's symbol table and relocations are available to object.ts.
import { type Cursor, DecodeError, bytes, f32, f64, name, s32, s64, u8, u32 } from './leb';

export type ValType = 'i32' | 'i64' | 'f32' | 'f64' | 'v128' | 'funcref' | 'externref' | 'exnref';

export interface FuncType {
  params: ValType[];
  results: ValType[];
}

export interface Import {
  module: string;
  name: string;
  kind: 'func' | 'table' | 'memory' | 'global' | 'tag';
  /** for `func`: the type index; for `global`: the valtype; for others: a descriptive string */
  desc: string;
  typeIndex?: number;
}

export interface Export {
  name: string;
  kind: 'func' | 'table' | 'memory' | 'global' | 'tag';
  index: number;
}

export interface Global {
  type: ValType;
  mutable: boolean;
  /** the init expression, as decoded constant-expression text (`i32.const 1024`, `global.get 0`) */
  init: string;
}

export interface ConstExpr {
  /** `i32.const`, `i64.const`, `global.get`, `ref.func`, `ref.null`, or `unknown` */
  op: string;
  value: number | bigint | null;
}

export interface ElemSegment {
  index: number;
  tableIndex: number;
  /** null for passive/declarative segments */
  offset: ConstExpr | null;
  mode: 'active' | 'passive' | 'declarative';
  /** function indices (expressions are reduced to `ref.func n`; a null ref reads as -1) */
  funcIndices: number[];
}

export interface DataSegment {
  index: number;
  memoryIndex: number;
  mode: 'active' | 'passive';
  /** the numeric address when the offset is an `i32.const`; null for passive or `global.get` bases */
  offset: number | null;
  offsetExpr: ConstExpr | null;
  bytes: Uint8Array;
}

export interface Local {
  count: number;
  type: ValType;
}

export interface Func {
  /** index in the FUNCTION INDEX SPACE: imports come first, so `call n` names `funcs[n - importedFuncCount]` */
  index: number;
  typeIndex: number;
  params: ValType[];
  results: ValType[];
  /** the locals declared at the top of the body, expanded one per entry (after the params) */
  locals: ValType[];
  /** the body's absolute file offset: the first byte AFTER its size LEB (the locals vector) */
  bodyOffset: number;
  /** the size LEB's value: locals + instructions, through the final `end` */
  size: number;
  /** `size` bytes starting at `bodyOffset` */
  bodyBytes: Uint8Array;
  /** offset of the first instruction, relative to `bodyOffset` */
  codeStart: number;
}

export interface SectionHeader {
  id: number;
  name: string;
  /** absolute offset of the section payload, and its size */
  start: number;
  end: number;
}

export interface Symbol {
  index: number;
  kind: 'function' | 'data' | 'global' | 'section' | 'tag' | 'table';
  flags: number;
  name: string | null;
  defined: boolean;
  /** function/global/tag/table: the index in its space */
  elementIndex?: number;
  /** data: where the symbol's bytes live */
  segment?: number;
  offset?: number;
  size?: number;
}

export interface Reloc {
  type: number;
  typeName: string;
  /** offset relative to the start of the relocated section's PAYLOAD */
  offset: number;
  symbolIndex: number;
  addend: number;
}

export interface RelocSection {
  /** the index of the section (counting from 0 over the module's sections, customs included) */
  sectionIndex: number;
  name: string;
  relocs: Reloc[];
}

export interface Module {
  bytes: Uint8Array;
  sections: SectionHeader[];
  types: FuncType[];
  imports: Import[];
  importedFuncCount: number;
  funcs: Func[];
  tables: { elemType: ValType; min: number; max: number | null }[];
  memories: { min: number; max: number | null; shared: boolean }[];
  globals: Global[];
  exports: Export[];
  start: number | null;
  elems: ElemSegment[];
  dataCount: number | null;
  datas: DataSegment[];
  /** the code section payload's absolute start, so a `reloc.CODE` offset maps to a file offset */
  codeSectionStart: number | null;
  /** names from the `name` custom section's function subsection, when present */
  funcNames: Map<number, string>;
  symbols: Symbol[];
  relocs: RelocSection[];
  /** every custom section's name, in order */
  customSections: string[];
}

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

export function valType(c: Cursor): ValType {
  const at = c.pos;
  const b = u8(c);
  const t = VALTYPES[b];
  if (t === undefined) {
    throw new DecodeError(`unknown value type 0x${b.toString(16)}`, at);
  }
  return t;
}

const SECTION_NAMES = [
  'Custom',
  'Type',
  'Import',
  'Function',
  'Table',
  'Memory',
  'Global',
  'Export',
  'Start',
  'Elem',
  'Code',
  'Data',
  'DataCount',
  'Tag',
];

export const RELOC_TYPE_NAMES: Record<number, string> = {
  0: 'R_WASM_FUNCTION_INDEX_LEB',
  1: 'R_WASM_TABLE_INDEX_SLEB',
  2: 'R_WASM_TABLE_INDEX_I32',
  3: 'R_WASM_MEMORY_ADDR_LEB',
  4: 'R_WASM_MEMORY_ADDR_SLEB',
  5: 'R_WASM_MEMORY_ADDR_I32',
  6: 'R_WASM_TYPE_INDEX_LEB',
  7: 'R_WASM_GLOBAL_INDEX_LEB',
  8: 'R_WASM_FUNCTION_OFFSET_I32',
  9: 'R_WASM_SECTION_OFFSET_I32',
  10: 'R_WASM_TAG_INDEX_LEB',
  11: 'R_WASM_MEMORY_ADDR_REL_SLEB',
  12: 'R_WASM_TABLE_INDEX_REL_SLEB',
  13: 'R_WASM_GLOBAL_INDEX_I32',
  14: 'R_WASM_MEMORY_ADDR_LEB64',
  15: 'R_WASM_MEMORY_ADDR_SLEB64',
  16: 'R_WASM_MEMORY_ADDR_I64',
  17: 'R_WASM_MEMORY_ADDR_REL_SLEB64',
  18: 'R_WASM_TABLE_INDEX_SLEB64',
  19: 'R_WASM_TABLE_INDEX_I64',
  20: 'R_WASM_TABLE_NUMBER_LEB',
  21: 'R_WASM_MEMORY_ADDR_TLS_SLEB',
  22: 'R_WASM_FUNCTION_OFFSET_I64',
  23: 'R_WASM_MEMORY_ADDR_LOCREL_I32',
  24: 'R_WASM_TABLE_INDEX_REL_SLEB64',
  25: 'R_WASM_MEMORY_ADDR_TLS_SLEB64',
  26: 'R_WASM_FUNCTION_INDEX_I32',
};

/** Relocation types that carry an addend (the LLVM `WasmRelocationEntry::hasAddend` set). */
const RELOC_WITH_ADDEND = new Set([3, 4, 5, 8, 9, 11, 14, 15, 16, 17, 21, 22, 23, 25]);

function constExpr(c: Cursor): ConstExpr {
  const at = c.pos;
  const op = u8(c);
  let out: ConstExpr;
  switch (op) {
    case 0x41:
      out = { op: 'i32.const', value: s32(c) };
      break;
    case 0x42:
      out = { op: 'i64.const', value: s64(c) };
      break;
    case 0x43:
      out = { op: 'f32.const', value: f32(c) };
      break;
    case 0x44:
      out = { op: 'f64.const', value: f64(c) };
      break;
    case 0x23:
      out = { op: 'global.get', value: u32(c) };
      break;
    case 0xd2:
      out = { op: 'ref.func', value: u32(c) };
      break;
    case 0xd0:
      u8(c);
      out = { op: 'ref.null', value: null };
      break;
    default:
      throw new DecodeError(`unsupported constant expression opcode 0x${op.toString(16)}`, at);
  }
  const end = u8(c);
  if (end !== 0x0b) {
    throw new DecodeError('constant expression does not end with `end`', c.pos - 1);
  }
  return out;
}

function limits(c: Cursor): { min: number; max: number | null; shared: boolean } {
  const at = c.pos;
  const flags = u8(c);
  const min = u32(c);
  const max = flags & 1 ? u32(c) : null;
  if (flags & ~0x3) {
    throw new DecodeError(`unsupported limits flags 0x${flags.toString(16)}`, at);
  }
  return { min, max, shared: (flags & 2) !== 0 };
}

function externKind(b: number, at: number): Export['kind'] {
  switch (b) {
    case 0:
      return 'func';
    case 1:
      return 'table';
    case 2:
      return 'memory';
    case 3:
      return 'global';
    case 4:
      return 'tag';
    default:
      throw new DecodeError(`unknown extern kind ${b}`, at);
  }
}

/** Parse a `.wasm` byte array (a linked module or a relocatable object). */
export function parseModule(input: Uint8Array): Module {
  const c: Cursor = { bytes: input, pos: 0 };
  if (input.length < 8 || input[0] !== 0 || input[1] !== 0x61 || input[2] !== 0x73 || input[3] !== 0x6d) {
    throw new DecodeError('not a wasm module (bad magic)', 0);
  }
  const version = input[4] | (input[5] << 8) | (input[6] << 16) | (input[7] << 24);
  if (version !== 1) {
    throw new DecodeError(`unsupported wasm version ${version}`, 4);
  }
  c.pos = 8;

  const m: Module = {
    bytes: input,
    sections: [],
    types: [],
    imports: [],
    importedFuncCount: 0,
    funcs: [],
    tables: [],
    memories: [],
    globals: [],
    exports: [],
    start: null,
    elems: [],
    dataCount: null,
    datas: [],
    codeSectionStart: null,
    funcNames: new Map(),
    symbols: [],
    relocs: [],
    customSections: [],
  };
  const funcTypeIndices: number[] = [];

  while (c.pos < input.length) {
    const idAt = c.pos;
    const id = u8(c);
    const size = u32(c);
    const start = c.pos;
    const end = start + size;
    if (end > input.length) {
      throw new DecodeError(`section ${id} runs past the end of the file`, idAt);
    }
    const sc: Cursor = { bytes: input, pos: start };
    let sectionName = SECTION_NAMES[id] ?? `unknown(${id})`;
    if (id === 0) {
      sectionName = name(sc);
      m.customSections.push(sectionName);
    }
    m.sections.push({ id, name: sectionName, start, end });
    switch (id) {
      case 0:
        parseCustom(m, sectionName, sc, end);
        break;
      case 1: {
        const n = u32(sc);
        for (let i = 0; i < n; i++) {
          const at = sc.pos;
          const form = u8(sc);
          if (form !== 0x60) {
            throw new DecodeError(`type ${i} is not a function type (0x${form.toString(16)})`, at);
          }
          const params = Array.from({ length: u32(sc) }, () => valType(sc));
          const results = Array.from({ length: u32(sc) }, () => valType(sc));
          m.types.push({ params, results });
        }
        break;
      }
      case 2: {
        const n = u32(sc);
        for (let i = 0; i < n; i++) {
          const module = name(sc);
          const field = name(sc);
          const kindAt = sc.pos;
          const kind = externKind(u8(sc), kindAt);
          let desc = '';
          let typeIndex: number | undefined;
          switch (kind) {
            case 'func':
              typeIndex = u32(sc);
              desc = `type ${typeIndex}`;
              m.importedFuncCount++;
              break;
            case 'table': {
              const t = valType(sc);
              const l = limits(sc);
              desc = `${t} ${l.min}${l.max === null ? '' : ` ${l.max}`}`;
              m.tables.push({ elemType: t, min: l.min, max: l.max });
              break;
            }
            case 'memory': {
              const l = limits(sc);
              desc = `${l.min}${l.max === null ? '' : ` ${l.max}`}${l.shared ? ' shared' : ''}`;
              m.memories.push(l);
              break;
            }
            case 'global': {
              const t = valType(sc);
              const mut = u8(sc);
              desc = `${mut ? 'mut ' : ''}${t}`;
              m.globals.push({ type: t, mutable: mut === 1, init: `import ${module}.${field}` });
              break;
            }
            case 'tag': {
              u8(sc);
              desc = `type ${u32(sc)}`;
              break;
            }
          }
          m.imports.push({ module, name: field, kind, desc, ...(typeIndex === undefined ? {} : { typeIndex }) });
        }
        break;
      }
      case 3: {
        const n = u32(sc);
        for (let i = 0; i < n; i++) {
          funcTypeIndices.push(u32(sc));
        }
        break;
      }
      case 4: {
        const n = u32(sc);
        for (let i = 0; i < n; i++) {
          const t = valType(sc);
          const l = limits(sc);
          m.tables.push({ elemType: t, min: l.min, max: l.max });
        }
        break;
      }
      case 5: {
        const n = u32(sc);
        for (let i = 0; i < n; i++) {
          m.memories.push(limits(sc));
        }
        break;
      }
      case 6: {
        const n = u32(sc);
        for (let i = 0; i < n; i++) {
          const t = valType(sc);
          const mut = u8(sc);
          const e = constExpr(sc);
          m.globals.push({ type: t, mutable: mut === 1, init: `${e.op} ${e.value ?? ''}`.trim() });
        }
        break;
      }
      case 7: {
        const n = u32(sc);
        for (let i = 0; i < n; i++) {
          const field = name(sc);
          const kindAt = sc.pos;
          const kind = externKind(u8(sc), kindAt);
          m.exports.push({ name: field, kind, index: u32(sc) });
        }
        break;
      }
      case 8:
        m.start = u32(sc);
        break;
      case 9: {
        const n = u32(sc);
        for (let i = 0; i < n; i++) {
          m.elems.push(elemSegment(sc, i));
        }
        break;
      }
      case 10: {
        m.codeSectionStart = start;
        const n = u32(sc);
        if (n !== funcTypeIndices.length) {
          throw new DecodeError(
            `code section has ${n} bodies but the function section declared ${funcTypeIndices.length}`,
            start,
          );
        }
        for (let i = 0; i < n; i++) {
          const bodySize = u32(sc);
          const bodyOffset = sc.pos;
          const body = bytes(sc, bodySize);
          const bc: Cursor = { bytes: body, pos: 0 };
          const localGroups = u32(bc);
          const locals: ValType[] = [];
          for (let g = 0; g < localGroups; g++) {
            const count = u32(bc);
            const t = valType(bc);
            if (locals.length + count > 50_000) {
              throw new DecodeError('too many locals', bodyOffset + bc.pos);
            }
            for (let k = 0; k < count; k++) {
              locals.push(t);
            }
          }
          const typeIndex = funcTypeIndices[i];
          const ft = m.types[typeIndex];
          if (ft === undefined) {
            throw new DecodeError(`function ${i} names type ${typeIndex}, of ${m.types.length}`, bodyOffset);
          }
          m.funcs.push({
            index: m.importedFuncCount + i,
            typeIndex,
            params: ft.params,
            results: ft.results,
            locals,
            bodyOffset,
            size: bodySize,
            bodyBytes: body,
            codeStart: bc.pos,
          });
        }
        break;
      }
      case 11: {
        const n = u32(sc);
        for (let i = 0; i < n; i++) {
          const at = sc.pos;
          const flags = u32(sc);
          let memoryIndex = 0;
          let offsetExpr: ConstExpr | null = null;
          let mode: DataSegment['mode'] = 'active';
          if (flags === 0) {
            offsetExpr = constExpr(sc);
          } else if (flags === 1) {
            mode = 'passive';
          } else if (flags === 2) {
            memoryIndex = u32(sc);
            offsetExpr = constExpr(sc);
          } else {
            throw new DecodeError(`unknown data segment flags ${flags}`, at);
          }
          const len = u32(sc);
          const payload = bytes(sc, len);
          const offset =
            offsetExpr !== null && offsetExpr.op === 'i32.const' ? (offsetExpr.value as number) >>> 0 : null;
          m.datas.push({ index: i, memoryIndex, mode, offset, offsetExpr, bytes: payload });
        }
        break;
      }
      case 12:
        m.dataCount = u32(sc);
        break;
      case 13: {
        const n = u32(sc);
        for (let i = 0; i < n; i++) {
          u8(sc);
          u32(sc);
        }
        break;
      }
      default:
        throw new DecodeError(`unknown section id ${id}`, idAt);
    }
    if (id !== 0 && sc.pos !== end) {
      throw new DecodeError(`section ${sectionName} decoded ${sc.pos - start} of ${size} bytes`, sc.pos);
    }
    c.pos = end;
  }
  if (m.dataCount !== null && m.dataCount !== m.datas.length) {
    throw new DecodeError(`DataCount says ${m.dataCount} segments, Data holds ${m.datas.length}`, 0);
  }
  return m;
}

function elemSegment(sc: Cursor, index: number): ElemSegment {
  const at = sc.pos;
  const flags = u32(sc);
  let tableIndex = 0;
  let offset: ConstExpr | null = null;
  let mode: ElemSegment['mode'] = 'active';
  const usesExprs = (flags & 4) !== 0;
  if ((flags & 1) === 0) {
    if (flags & 2) {
      tableIndex = u32(sc);
    }
    offset = constExpr(sc);
  } else {
    mode = flags & 2 ? 'declarative' : 'passive';
  }
  // bit 0 set or bit 1 set: an explicit elemkind (0x00 = funcref) or reftype byte follows
  if (flags & 3) {
    if (usesExprs) {
      valType(sc);
    } else {
      const kind = u8(sc);
      if (kind !== 0) {
        throw new DecodeError(`unsupported elemkind ${kind}`, sc.pos - 1);
      }
    }
  }
  const n = u32(sc);
  if (n > 10_000_000) {
    throw new DecodeError(`implausible elem count ${n}`, at);
  }
  const funcIndices: number[] = new Array(n);
  for (let i = 0; i < n; i++) {
    if (usesExprs) {
      const e = constExpr(sc);
      funcIndices[i] = e.op === 'ref.func' ? (e.value as number) : -1;
    } else {
      funcIndices[i] = u32(sc);
    }
  }
  return { index, tableIndex, offset, mode, funcIndices };
}

function parseCustom(m: Module, sectionName: string, sc: Cursor, end: number): void {
  if (sectionName === 'name') {
    while (sc.pos < end) {
      const sub = u8(sc);
      const size = u32(sc);
      const subEnd = sc.pos + size;
      if (sub === 1) {
        const n = u32(sc);
        for (let i = 0; i < n; i++) {
          const idx = u32(sc);
          m.funcNames.set(idx, name(sc));
        }
      }
      sc.pos = subEnd;
    }
  } else if (sectionName === 'linking') {
    const version = u32(sc);
    if (version !== 2) {
      throw new DecodeError(`linking section version ${version} (only 2 is known)`, sc.pos);
    }
    while (sc.pos < end) {
      const sub = u8(sc);
      const size = u32(sc);
      const subEnd = sc.pos + size;
      if (sub === 8) {
        const n = u32(sc);
        for (let i = 0; i < n; i++) {
          m.symbols.push(symbolEntry(m, sc, i));
        }
      }
      sc.pos = subEnd;
    }
  } else if (sectionName.startsWith('reloc.')) {
    const target = u32(sc);
    const n = u32(sc);
    const relocs: Reloc[] = [];
    for (let i = 0; i < n; i++) {
      const at = sc.pos;
      const type = u8(sc);
      const offset = u32(sc);
      const symbolIndex = u32(sc);
      const addend = RELOC_WITH_ADDEND.has(type) ? s32(sc) : 0;
      const typeName = RELOC_TYPE_NAMES[type];
      if (typeName === undefined) {
        throw new DecodeError(`unknown relocation type ${type}`, at);
      }
      relocs.push({ type, typeName, offset, symbolIndex, addend });
    }
    m.relocs.push({ sectionIndex: target, name: sectionName, relocs });
  }
}

const SYM_KINDS: Symbol['kind'][] = ['function', 'data', 'global', 'section', 'tag', 'table'];
const WASM_SYM_UNDEFINED = 0x10;
const WASM_SYM_EXPLICIT_NAME = 0x40;

function symbolEntry(m: Module, sc: Cursor, index: number): Symbol {
  const at = sc.pos;
  const kindByte = u8(sc);
  const kind = SYM_KINDS[kindByte];
  if (kind === undefined) {
    throw new DecodeError(`unknown symbol kind ${kindByte}`, at);
  }
  const flags = u32(sc);
  const defined = (flags & WASM_SYM_UNDEFINED) === 0;
  const sym: Symbol = { index, kind, flags, name: null, defined };
  switch (kind) {
    case 'function':
    case 'global':
    case 'tag':
    case 'table':
      sym.elementIndex = u32(sc);
      if (defined || flags & WASM_SYM_EXPLICIT_NAME) {
        sym.name = name(sc);
      }
      // an undefined symbol without an explicit name is named by the import it stands for
      else if (kind === 'function') {
        sym.name = importedFunc(m, sym.elementIndex)?.name ?? null;
      } else {
        sym.name = importByIndex(m, kind, sym.elementIndex)?.name ?? null;
      }
      break;
    case 'data':
      sym.name = name(sc);
      if (defined) {
        sym.segment = u32(sc);
        sym.offset = u32(sc);
        sym.size = u32(sc);
      }
      break;
    case 'section':
      sym.elementIndex = u32(sc);
      break;
  }
  return sym;
}

function importByIndex(m: Module, kind: 'global' | 'tag' | 'table', index: number): Import | null {
  let k = 0;
  for (const imp of m.imports) {
    if (imp.kind !== kind) {
      continue;
    }
    if (k === index) {
      return imp;
    }
    k++;
  }
  return null;
}

/** The function record for an index in the function index space, or null for an import. */
export function funcAt(m: Module, index: number): Func | null {
  const i = index - m.importedFuncCount;
  return i >= 0 && i < m.funcs.length ? m.funcs[i] : null;
}

/** The import record behind a function index, or null when it is a defined function. */
export function importedFunc(m: Module, index: number): Import | null {
  if (index >= m.importedFuncCount) {
    return null;
  }
  let k = 0;
  for (const imp of m.imports) {
    if (imp.kind !== 'func') {
      continue;
    }
    if (k === index) {
      return imp;
    }
    k++;
  }
  return null;
}

export function typeText(t: FuncType): string {
  return `(${t.params.join(', ')}) -> (${t.results.join(', ')})`;
}
