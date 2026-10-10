// Canonical token rows and the two hashes over them.
//
// A function's bytes are not comparable across two builds: a `call` names a function INDEX, an
// `i32.const` of an address names where the linker put a segment, and both move when any other
// function or datum changes. Normalisation replaces every such relocatable operand with a symbol
// that depends only on what it refers to:
//
//   call / return_call / ref.func   import → `import:<field>` (the module is a function of the name;
//                                   `importName` maps a minified field to its real name, imports.ts)
//                                   self   → `self`
//                                   other  → `fn:<callee fingerprint>` (see the fixed point below)
//   call_indirect                   the TYPE as structure (`(i32 i32)->(i32)`), never its index
//   i32.const inside a data segment `data:<hash of the segment's bytes>+<offset in it>`
//   memory.init / data.drop         `data:<hash of the segment's bytes>`
//   block / loop / if (type n)      the type as structure
//   memarg                          kept as written (`align=2 offset=16`): offsets are field offsets,
//                                   which ARE the program, not where the linker put it
//   locals, labels, globals         kept as written (they are intra-function, or the two emscripten
//                                   globals, whose indices never move)
//
// Table indices are NOT symbolised from a bare `i32.const`: emscripten's table starts at 1 and its
// data at 1024, so every small integer literal is a plausible function-pointer constant and every
// function-pointer constant below the data base is a plausible integer. There is no decidable rule
// without relocations; the object path (object.ts) has them and symbolises exactly.
//
// FINGERPRINT. `fn:<callee fingerprint>` makes a fingerprint depend on its callees', which is
// circular across recursion. It is computed as an iterated hash (Weisfeiler–Lehman style): round
// 0 hashes the rows with every callee as `fn:?`; round r hashes them with round r-1's fingerprint
// of each callee. `depth` rounds (default 2) give a fingerprint that identifies a function by its
// own body AND the bodies within `depth` calls of it. Depth 0 identifies by own body alone, and is
// what `score --depth 0` uses when the two sides' callees are not expected to match.
//
// ADDR. The second hash blinds every ADDRESS and nothing else: an `i32.const` at or above 1024
// (emscripten's GLOBAL_BASE, so a data OR bss address — bss has no segment for the data symbol to
// name) becomes `<addr>`, a memarg offset at or above 1024 (an address binaryen folded into a load)
// becomes `offset=<addr>`, a data symbol becomes `<data>`; callees compare by their ADDR hash. It is
// what two links of the same library at different layouts share (dlmalloc's `free` differs from a
// probe's only in 35 such operands). The price: a genuine integer ≥ 1024 (a buffer size, 65536) is
// blinded too, so two functions differing only in one such constant collide at this tier.
//
// SHAPE. The third hash drops every constant (`i32.const`, `i64.const`, floats, memarg offsets, data
// symbols) and compares callees by their SHAPE instead of their fingerprint — so it survives a
// changed string address, a different struct layout, or a callee that differs only by a constant.
import { createHash } from 'node:crypto';

import { type Imm, type Instr, blockTypeText, disassemble, immText } from './disasm';
import { type Func, type Module, funcAt, importedFunc } from './parse';

export interface NormalizeOptions {
  /** how many call levels the fingerprint reaches through. Default 2. */
  depth?: number;
  /** a name for an imported function, replacing the bare field (target fields are minified; see imports.ts) */
  importName?: (module: string, field: string, funcIndex: number) => string;
}

export interface NormalizedFn {
  index: number;
  /** the rows at the final depth, one per instruction, in order */
  rows: string[];
  /** the rows with every address blinded, one per instruction */
  addrRows: string[];
  /** the rows with every constant dropped, one per instruction */
  shapeRows: string[];
  fingerprint: string;
  addr: string;
  shape: string;
  instrs: Instr[];
}

export interface DataRange {
  start: number;
  end: number;
  hash: string;
  index: number;
}

export function hash(text: string): string {
  return createHash('sha256').update(text).digest('hex').slice(0, 16);
}

export function dataRanges(m: Module): DataRange[] {
  const out: DataRange[] = [];
  for (const d of m.datas) {
    if (d.offset === null) {
      continue;
    }
    out.push({
      start: d.offset,
      end: d.offset + d.bytes.length,
      hash: hash(Buffer.from(d.bytes).toString('latin1')),
      index: d.index,
    });
  }
  out.sort((a, b) => a.start - b.start);
  return out;
}

export function dataSymbol(ranges: DataRange[], value: number): string | null {
  // ranges sorted by start; binary search the last range starting at or before `value`
  let lo = 0;
  let hi = ranges.length - 1;
  let found = -1;
  while (lo <= hi) {
    const mid = (lo + hi) >> 1;
    if (ranges[mid].start <= value) {
      found = mid;
      lo = mid + 1;
    } else {
      hi = mid - 1;
    }
  }
  if (found < 0) {
    return null;
  }
  const r = ranges[found];
  if (value >= r.end) {
    return null;
  }
  return `data:${r.hash}+${value - r.start}`;
}

/** A row with the CALL TARGET left as a hole, to be filled per round. */
interface RowTemplate {
  text: string;
  /** function index the hole names, or null when there is no hole */
  callee: number | null;
  addrText: string;
  shapeText: string;
}

export const GLOBAL_BASE = 1024;

const CALL_OPS = new Set([0x10, 0x12, 0xd2]);

function typeShape(m: Module, index: number): string {
  const t = m.types[index];
  return t === undefined ? `type?${index}` : `(${t.params.join(' ')})->(${t.results.join(' ')})`;
}

function rowTemplate(
  m: Module,
  fn: Func,
  i: Instr,
  ranges: DataRange[],
  dataHashByIndex: string[],
  opts: NormalizeOptions,
): RowTemplate {
  const parts: string[] = [];
  const addrParts: string[] = [];
  const shapeParts: string[] = [];
  let callee: number | null = null;
  for (const imm of i.imms) {
    const [full, addr, shape] = immSymbol(m, fn, i, imm, ranges, dataHashByIndex, opts);
    if (full === null) {
      continue;
    }
    if (full === CALLEE_HOLE) {
      callee = (imm as { value: number }).value;
    }
    parts.push(full);
    addrParts.push(addr);
    shapeParts.push(shape);
  }
  const join = (ps: string[]): string => (ps.length === 0 ? i.name : `${i.name} ${ps.join(' ')}`);
  return { text: join(parts), callee, addrText: join(addrParts), shapeText: join(shapeParts) };
}

const CALLEE_HOLE = '\u0000callee';

/** [full symbol, address-blind symbol, shape symbol]; `null` drops the immediate. */
function immSymbol(
  m: Module,
  fn: Func,
  i: Instr,
  imm: Imm,
  ranges: DataRange[],
  dataHashByIndex: string[],
  opts: NormalizeOptions,
): [string | null, string, string] {
  const same = (s: string): [string, string, string] => [s, s, s];
  switch (imm.kind) {
    case 'u32':
      switch (imm.role) {
        case 'func': {
          if (!CALL_OPS.has(i.opcode)) {
            return same(String(imm.value));
          }
          if (imm.value === fn.index) {
            return same('self');
          }
          const imp = importedFunc(m, imm.value);
          if (imp !== null) {
            const n = opts.importName ? opts.importName(imp.module, imp.name, imm.value) : imp.name;
            return same(`import:${n}`);
          }
          return same(CALLEE_HOLE);
        }
        case 'type':
          return same(`type:${typeShape(m, imm.value)}`);
        case 'table':
        case 'memory':
          return imm.value === 0 ? [null, '', ''] : same(`${imm.role}:${imm.value}`);
        case 'data':
          return [`data:${dataHashByIndex[imm.value] ?? `?${imm.value}`}`, '<data>', '<data>'];
        default:
          return same(String(imm.value));
      }
    case 's32': {
      if (i.opcode !== 0x41) {
        return [String(imm.value), String(imm.value), '<c>'];
      }
      const sym = dataSymbol(ranges, imm.value >>> 0);
      // a negative constant is never an address: memory is at most 2 GB (32768 pages) and -2 is -2
      return [sym ?? String(imm.value), imm.value >= GLOBAL_BASE ? '<addr>' : String(imm.value), '<c>'];
    }
    case 's64':
      return [String(imm.value), String(imm.value), '<c>'];
    case 'f32':
    case 'f64':
      return [immText(imm), immText(imm), '<c>'];
    case 'blocktype': {
      const bt = imm.value;
      const s = bt.kind === 'type' ? `(type ${typeShape(m, bt.index)})` : blockTypeText(bt);
      return [s === '' ? null : s, s, s];
    }
    case 'memarg': {
      const mem = imm.memory ? `mem=${imm.memory} ` : '';
      const full = `${mem}align=${imm.align} offset=${imm.offset}`;
      return [full, imm.offset >= GLOBAL_BASE ? `${mem}align=${imm.align} offset=<addr>` : full, `align=${imm.align}`];
    }
    case 'labels':
    case 'valtypes':
    case 'reftype':
      return same(immText(imm));
  }
}

/** Normalise every defined function of a module and compute both hashes. */
export function normalizeModule(m: Module, opts: NormalizeOptions = {}): NormalizedFn[] {
  const depth = opts.depth ?? 2;
  const ranges = dataRanges(m);
  const dataHashByIndex = m.datas.map((d) => hash(Buffer.from(d.bytes).toString('latin1')));
  const templates: RowTemplate[][] = [];
  const instrsAll: Instr[][] = [];
  for (const fn of m.funcs) {
    const instrs = disassemble(fn);
    instrsAll.push(instrs);
    templates.push(instrs.map((i) => rowTemplate(m, fn, i, ranges, dataHashByIndex, opts)));
  }
  const byIndex = (index: number): number => index - m.importedFuncCount;

  const fill = (tpls: RowTemplate[], which: 'text' | 'addrText' | 'shapeText', prev: string[] | null): string[] =>
    tpls.map((t) => {
      const src = t[which];
      if (t.callee === null) {
        return src;
      }
      const k = byIndex(t.callee);
      const sym = prev === null ? '?' : (prev[k] ?? '?');
      return src.replace(CALLEE_HOLE, `fn:${sym}`);
    });

  let fps: string[] | null = null;
  let addrs: string[] | null = null;
  let shapes: string[] | null = null;
  let rows: string[][] = [];
  let addrRows: string[][] = [];
  let shapeRows: string[][] = [];
  const hashAll = (rs: string[][]): string[] => rs.map((r, k) => hash(`${signature(m.funcs[k])}\n${r.join('\n')}`));
  for (let round = 0; round <= depth; round++) {
    rows = templates.map((t) => fill(t, 'text', fps));
    addrRows = templates.map((t) => fill(t, 'addrText', addrs));
    shapeRows = templates.map((t) => fill(t, 'shapeText', shapes));
    fps = hashAll(rows);
    addrs = hashAll(addrRows);
    shapes = hashAll(shapeRows);
  }
  return m.funcs.map((fn, k) => ({
    index: fn.index,
    rows: rows[k],
    addrRows: addrRows[k],
    shapeRows: shapeRows[k],
    fingerprint: fps![k],
    addr: addrs![k],
    shape: shapes![k],
    instrs: instrsAll[k],
  }));
}

/** The type signature and locals, which the body's rows do not state but a match must agree on. */
export function signature(fn: Func): string {
  return `(${fn.params.join(' ')})->(${fn.results.join(' ')}) locals(${fn.locals.join(' ')})`;
}

export function normalizedByIndex(norm: NormalizedFn[], m: Module, index: number): NormalizedFn | null {
  const fn = funcAt(m, index);
  if (fn === null) {
    return null;
  }
  return norm[index - m.importedFuncCount] ?? null;
}
