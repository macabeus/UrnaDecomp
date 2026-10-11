// Operand kinds: which constants of our plain link are data addresses, which are table slots and which are plain
// integers, learned on OUR side. The final module has no relocations left, and an address, a slot and an integer
// look alike; the target's bytes cannot say which one ours is (an integer of ours may equal the target's address of
// something). So urna links the same objects a second time with the data and the table moved
// (tools/round/link-kinds.sh: -sGLOBAL_BASE=1024+D and -sTABLE_BASE=1+T, wasm-ld's --global-base / --table-base),
// names that link's functions by replaying binaryen as link-map.sh does, and compares the two links function by
// function, aligned by name:
//   the same instruction sequence (opcodes, callees and ref.func by name, types by signature, locals, every other
//   immediate)          → each i32.const, i64.const and memarg offset is an ADDRESS if it moved by exactly D, a
//                         table SLOT if it moved by exactly T (and both slots hold the same function), a LITERAL if
//                         it did not move;
//   anything else       → the function is KIND-UNVERIFIED (with the reason): another shape, or an operand that moved
//                         by another amount.
// A -fPIC object (sqlite's) adds __table_base to a slot relative to it, and binaryen folds the sum unless the relative
// part is not one constant: `select(2035, 2036, 2037) + 1`. The base moves by T, the relative operands do not, so
// they would read as literals. A moved constant equal to the plain link's own base (1 for the table, 1024 for the
// data) is therefore followed through the operand stack to its use:
//   added to constants (through select arms)  → the base is TABLE-BASE and those constants TABLE-REL (a slot minus
//                                               the base);
//   used as a value (stored, passed, compared) → the slot or address it moved as;
//   kept in a local or a global, carried by a branch or a block result, added to anything but constants, or a
//   __memory_base sum                          → KIND-UNVERIFIED: where it ends up cannot be told.
// Binaryen may also fold part of the relative operand into the base (`select(2034, 2035) + 2`): the base then differs
// from the link's, it moves as a slot and the relative constants do not. So every other moved table slot is
// followed too, loosely: added to constants (or to another slot) → KIND-UNVERIFIED, since nothing tells which part is
// the base; anywhere else it is a pointer used as one, and is not followed further.
// splat-remap.ts offers an operand only the readings of its kind; a kind-unverified function keeps every reading.
import { createHash } from 'node:crypto';
import { existsSync, readFileSync } from 'node:fs';
import { dirname, join, resolve } from 'node:path';

import { type Imm, type Instr, disassemble } from './disasm';
import { UNARY } from './fold';
import { type Func, type Module, funcAt, importedFunc } from './parse';
import { readOurNames } from './splat-symbols';

/** `table-base`: the __table_base constant of an unfolded `base + rel` sum; `table-rel`: a constant added to it (a
 *  table slot minus the base) */
export type OperandKind = 'literal' | 'address' | 'slot' | 'table-base' | 'table-rel';
export type MovedKind = Exclude<OperandKind, 'literal'>;

/** A function's operand kinds: `ops` lists the operands that are not literals by the body offset of their instruction;
 *  every other i32.const, i64.const and memarg offset is a literal. */
export type FunctionKinds = { verified: true; ops: Map<number, MovedKind> } | { verified: false; why: string };

export interface OperandKinds {
  /** sha256 of the plain module the table describes */
  plainSha256: string;
  shiftedSha256: string;
  dataShift: number;
  tableShift: number;
  /** our defined function index (the plain link's) → its kinds */
  functions: Map<number, FunctionKinds>;
}

/** One link: its module and the name of every function (names.tsv key for ours, `import:<module>.<field>` for an
 *  import). */
export interface KindsSide {
  module: Module;
  names: Map<number, string>;
}

export interface LinkShifts {
  dataShift: number;
  tableShift: number;
  /** where the plain link's data and table start (GLOBAL_BASE, the first table slot) */
  dataBase: number;
  tableBase: number;
}

// /2: the table-base and table-rel kinds
const FORMAT = 'urna-operand-kinds/2';

export function sha256Of(bytes: Uint8Array): string {
  return createHash('sha256').update(bytes).digest('hex');
}

/** The kinds table beside a plain link: `<root>/build/out/wasm/vota_web/x.wasm` →
 *  `<root>/build/out/wasm-kinds/vota_web/kinds.json`. */
export function defaultKindsPath(plainPath: string): string {
  const dir = dirname(resolve(plainPath));
  return join(dirname(dirname(dir)), 'wasm-kinds', dir.split('/').pop() ?? '', 'kinds.json');
}

/** The operand-kind table a command reads: `--kinds`'s path, else the one beside the plain link; null with --no-kinds.
 *  Throws when there is none: without a table every match is kind-unverified, which only --no-kinds asks for. */
export function kindsPathOf(plainPath: string, kindsArg: string | undefined, noKinds: boolean): string | null {
  if (noKinds) {
    if (kindsArg !== undefined) {
      throw new Error('--kinds and --no-kinds: pass one');
    }
    return null;
  }
  const path = kindsArg ?? defaultKindsPath(plainPath);
  if (!existsSync(path)) {
    throw new Error(
      `no operand-kind table at ${path}: run make -C src link-kinds, or pass --no-kinds to check without one (every match kind-unverified)`,
    );
  }
  return path;
}

/** Throws on arguments a command did not take (a misspelt flag, or a flag and its value passed as one word). */
export function noMoreArgs(cmd: string, args: readonly string[]): void {
  if (args.length > 0) {
    throw new Error(`${cmd}: unexpected argument${args.length > 1 ? 's' : ''} ${args.map((a) => `'${a}'`).join(' ')}`);
  }
}

/** A link's names: its names.tsv keys (splat-symbols readOurNames) and its imports. */
export function kindsSide(module: Module, namesPath: string): KindsSide {
  const names = new Map<number, string>();
  module.imports
    .filter((i) => i.kind === 'func')
    .forEach((i, k) => {
      names.set(k, `import:${i.module}.${i.name}`);
    });
  for (const f of readOurNames(namesPath)) {
    names.set(f.index, f.key);
  }
  return { module, names };
}

/** The first table slot of a link (its one active element segment's offset): 1 for an emscripten link. */
export function tableBase(m: Module): number {
  return activeElem(m).offset;
}

function activeElem(m: Module): { offset: number; funcs: number[] } {
  const e = m.elems.filter((x) => x.mode === 'active' && x.tableIndex === 0 && typeof x.offset?.value === 'number');
  if (e.length !== 1) {
    throw new Error(`expected one active element segment, found ${e.length}`);
  }
  return { offset: e[0].offset!.value as number, funcs: e[0].funcIndices };
}

/** How far the second link moved the data and the table: the first data segment and __stack_pointer (above .bss)
 *  by one amount, the table's one element segment by another. (Segment boundaries in between may differ by a byte:
 *  binaryen splits the data at runs of zeros, and a moved pointer has other zero bytes.) */
export function linkShifts(plain: Module, shifted: Module): LinkShifts {
  const pd = plain.datas.find((d) => d.mode === 'active');
  const sd = shifted.datas.find((d) => d.mode === 'active');
  if (pd === undefined || sd === undefined) {
    throw new Error('a link has no active data segment');
  }
  const dataShift = sd.offset! - pd.offset!;
  const sp = (m: Module): number | null => {
    const x = m.globals[0]?.init.match(/^i32\.const (\d+)$/);
    return x ? Number(x[1]) : null;
  };
  const ps = sp(plain);
  const ss = sp(shifted);
  if (ps === null || ss === null || ss - ps !== dataShift) {
    throw new Error(`the data moved by ${dataShift} but the stack pointer from ${ps} to ${ss}`);
  }
  const pe = activeElem(plain);
  const se = activeElem(shifted);
  if (pe.funcs.length !== se.funcs.length) {
    throw new Error(`the tables hold ${pe.funcs.length} and ${se.funcs.length} functions`);
  }
  const tableShift = se.offset - pe.offset;
  if (dataShift <= 0 || tableShift <= 0 || dataShift === tableShift) {
    throw new Error(
      `the second link must move data and table by two different positive amounts (${dataShift}, ${tableShift})`,
    );
  }
  return { dataShift, tableShift, dataBase: pd.offset!, tableBase: pe.offset };
}

function signature(m: Module, t: number): string {
  const ty = m.types[t];
  return ty === undefined ? `?${t}` : `${ty.params.join(',')}->${ty.results.join(',')}`;
}

interface Operand {
  at: number;
  op: string;
  plain: bigint;
  shifted: bigint;
}

/** null when the two bodies are the same instructions up to their constants (the operands collected), else why not. */
function sameShape(p: KindsSide, s: KindsSide, pi: Instr[], si: Instr[], out: Operand[]): string | null {
  if (pi.length !== si.length) {
    return `${pi.length} instructions against ${si.length}`;
  }
  for (let k = 0; k < pi.length; k++) {
    const a = pi[k];
    const b = si[k];
    const where = `+0x${a.offset.toString(16)} ${a.name}`;
    if (a.opcode !== b.opcode || a.imms.length !== b.imms.length) {
      return `${where} against ${b.name}`;
    }
    for (let j = 0; j < a.imms.length; j++) {
      const x: Imm = a.imms[j];
      const y: Imm = b.imms[j];
      if (x.kind !== y.kind) {
        return `${where}: immediate kinds differ`;
      }
      if (x.kind === 'u32' && y.kind === 'u32') {
        if (x.role === 'func') {
          const fp = p.names.get(x.value) ?? `#${x.value}`;
          const fs = s.names.get(y.value) ?? `#${y.value}`;
          if (fp !== fs) {
            return `${where}: ${fp} against ${fs}`;
          }
        } else if (x.role === 'type') {
          if (signature(p.module, x.value) !== signature(s.module, y.value)) {
            return `${where}: type signatures differ`;
          }
        } else if (x.value !== y.value) {
          return `${where}: ${x.role} ${x.value} against ${y.value}`;
        }
      } else if (x.kind === 's32' && y.kind === 's32' && a.opcode === 0x41) {
        out.push({ at: a.offset, op: a.name, plain: BigInt(x.value), shifted: BigInt(y.value) });
      } else if (x.kind === 's64' && y.kind === 's64' && a.opcode === 0x42) {
        out.push({ at: a.offset, op: a.name, plain: x.value, shifted: y.value });
      } else if (x.kind === 'memarg' && y.kind === 'memarg') {
        if (x.align !== y.align || x.memory !== y.memory) {
          return `${where}: memarg alignment or memory differs`;
        }
        out.push({ at: a.offset, op: `${a.name} offset`, plain: BigInt(x.offset), shifted: BigInt(y.offset) });
      } else if (x.kind === 'blocktype' && y.kind === 'blocktype') {
        const tx = x.value.kind === 'type' ? signature(p.module, x.value.index) : JSON.stringify(x.value);
        const ty = y.value.kind === 'type' ? signature(s.module, y.value.index) : JSON.stringify(y.value);
        if (tx !== ty) {
          return `${where}: block types differ`;
        }
      } else {
        const text = (v: Imm): string =>
          JSON.stringify({ ...v, at: 0, offsetAt: 0 }, (_k, z) => (typeof z === 'bigint' ? z.toString() : z));
        if (text(x) !== text(y)) {
          return `${where}: ${x.kind} immediates differ`;
        }
      }
    }
  }
  return null;
}

/** A value on the operand stack, as baseSums follows it: the i32.const leaves it is made of (through select arms), the
 *  base constants and the other table slots among them, and whether anything else went into it. */
interface StackValue {
  leaves: number[];
  bases: number[];
  slots: number[];
  opaque: boolean;
}

const OPAQUE: StackValue = { leaves: [], bases: [], slots: [], opaque: true };

/** Where the base constants `bases` (body offsets of i32.const) of `fn` end up: `sums` maps each one added to constants
 *  to the offsets of those constants (one used as a plain value is not in it); or why it cannot be told. `slotSums`
 *  lists the table slot constants among `slots` (the other moved ones) that are added to constants: a sum binaryen
 *  left with part of the base in it (`select(2034, 2035) + 2`), which nothing here can split. Where a slot of `slots`
 *  goes otherwise is not followed: it is a pointer used as one. */
export function baseSums(
  m: Module,
  fn: Func,
  instrs: readonly Instr[],
  bases: ReadonlySet<number>,
  slots: ReadonlySet<number> = new Set(),
): { sums: Map<number, number[]>; slotSums: number[] } | { why: string } {
  const stack: StackValue[] = [];
  const frames: { height: number; results: number }[] = [{ height: 0, results: fn.results.length }];
  const sums = new Map<number, number[]>();
  const slotSums: number[] = [];
  let why: string | null = null;
  const height = (): number => frames[frames.length - 1].height;
  // below the frame's height the stack is the enclosing block's (or, after a branch, anything): an unknown value
  const pop = (): StackValue => (stack.length > height() ? stack.pop()! : OPAQUE);
  const popN = (n: number): StackValue[] => Array.from({ length: n }, pop);
  const push = (n: number): void => {
    for (let k = 0; k < n; k++) {
      stack.push(OPAQUE);
    }
  };
  const escapes = (vs: StackValue[], how: string): void => {
    const b = vs.find((v) => v.bases.length > 0);
    if (b !== undefined && why === null) {
      why = `+0x${b.bases[0].toString(16)} i32.const is the link's base itself, ${how}`;
    }
  };
  // the frame's values leave it (a branch, a block's end): a base among them goes where this cannot follow
  const leave = (how: string): void => {
    escapes(stack.splice(height()), how);
  };
  const typeOf = (index: number): { params: unknown[]; results: unknown[] } | undefined => {
    const imp = importedFunc(m, index);
    if (imp !== null) {
      return imp.typeIndex === undefined ? undefined : m.types[imp.typeIndex];
    }
    return funcAt(m, index) ?? undefined;
  };
  const u32 = (i: Instr): number => (i.imms[0] as { value: number }).value;
  for (const i of instrs) {
    const n = i.name;
    switch (n) {
      case 'try':
      case 'catch':
      case 'catch_all':
      case 'delegate':
      case 'throw':
      case 'rethrow':
        // the legacy exception-handling blocks: not followed (no function with a base constant has them)
        return { why: `+0x${i.offset.toString(16)} ${n}: exception handling, which this does not follow` };
      case 'block':
      case 'loop':
      case 'if': {
        if (n === 'if') {
          pop();
        }
        const bt = i.imms[0];
        const t = bt?.kind === 'blocktype' && bt.value.kind === 'type' ? m.types[bt.value.index] : undefined;
        if (t !== undefined && t.params.length > 0) {
          return { why: `+0x${i.offset.toString(16)} ${n} takes parameters` };
        }
        const results =
          bt?.kind !== 'blocktype' || bt.value.kind === 'empty' ? 0 : bt.value.kind === 'val' ? 1 : t!.results.length;
        frames.push({ height: stack.length, results });
        break;
      }
      case 'else':
        leave('a block result');
        break;
      case 'end': {
        if (frames.length === 1) {
          leave('returned');
          break;
        }
        leave('a block result');
        push(frames.pop()!.results);
        break;
      }
      case 'br':
      case 'br_table':
      case 'return':
        if (n === 'br_table') {
          pop();
        }
        leave('carried by a branch');
        break;
      case 'unreachable':
        stack.splice(height());
        break;
      case 'br_if':
        pop();
        escapes(stack.slice(height()), 'carried by a branch');
        break;
      case 'nop':
      case 'data.drop':
      case 'elem.drop':
        break;
      case 'drop':
        pop();
        break;
      case 'select': {
        pop();
        const [b, a] = popN(2);
        stack.push({
          leaves: [...a.leaves, ...b.leaves],
          bases: [...a.bases, ...b.bases],
          slots: [...a.slots, ...b.slots],
          opaque: a.opaque || b.opaque,
        });
        break;
      }
      case 'local.get':
      case 'global.get':
      case 'memory.size':
      case 'table.size':
      case 'ref.null':
      case 'ref.func':
      case 'i64.const':
      case 'f32.const':
      case 'f64.const':
        push(1);
        break;
      case 'local.set':
      case 'global.set':
        escapes([pop()], `kept in a ${n.split('.')[0]}`);
        break;
      case 'local.tee':
        escapes([pop()], 'kept in a local');
        push(1);
        break;
      case 'i32.const':
        stack.push(
          bases.has(i.offset)
            ? { leaves: [], bases: [i.offset], slots: [], opaque: false }
            : slots.has(i.offset)
              ? { leaves: [], bases: [], slots: [i.offset], opaque: false }
              : { leaves: [i.offset], bases: [], slots: [], opaque: false },
        );
        break;
      case 'i32.add': {
        const [y, x] = popN(2);
        // a slot added to constants (or to another slot): part of a slot sum
        for (const [s, o] of [
          [x, y],
          [y, x],
        ]) {
          if (s.slots.length > 0 && !o.opaque && o.leaves.length + o.slots.length > 0) {
            slotSums.push(...s.slots);
          }
        }
        if (x.bases.length + y.bases.length > 0) {
          const pure = (v: StackValue): boolean => v.bases.length === 1 && v.leaves.length === 0 && !v.opaque;
          const [base, rel] =
            pure(x) && y.bases.length === 0 ? [x, y] : pure(y) && x.bases.length === 0 ? [y, x] : [null, null];
          if (base === null || rel === null) {
            escapes([x, y], 'in a sum this cannot follow');
          } else if (rel.opaque || rel.slots.length > 0) {
            escapes([base], rel.opaque ? 'added to a value that is not a constant' : 'added to a table slot');
          } else {
            sums.set(base.bases[0], rel.leaves);
          }
        }
        push(1);
        break;
      }
      case 'call':
      case 'return_call':
      case 'call_indirect':
      case 'return_call_indirect': {
        const indirect = n.endsWith('indirect');
        const t = indirect ? m.types[u32(i)] : typeOf(u32(i));
        if (t === undefined) {
          return { why: `+0x${i.offset.toString(16)} ${n}: no type` };
        }
        popN(t.params.length + (indirect ? 1 : 0));
        if (n.startsWith('return_')) {
          leave('carried by a branch');
        } else {
          push(t.results.length);
        }
        break;
      }
      case 'memory.grow':
      case 'ref.is_null':
      case 'table.get':
        popN(1);
        push(1);
        break;
      case 'table.set':
        popN(2);
        break;
      case 'table.grow':
        popN(2);
        push(1);
        break;
      case 'memory.copy':
      case 'memory.fill':
      case 'memory.init':
      case 'table.copy':
      case 'table.init':
      case 'table.fill':
        popN(3);
        break;
      default:
        if (/\.load/.test(n)) {
          popN(1);
          push(1);
        } else if (/\.store/.test(n)) {
          popN(2);
        } else if (UNARY.test(n)) {
          popN(1);
          push(1);
        } else if (/^(i32|i64|f32|f64)\.\w+$/.test(n)) {
          popN(2);
          push(1);
        } else {
          return { why: `+0x${i.offset.toString(16)} ${n}: the operand stack cannot be followed through it` };
        }
    }
  }
  return why === null ? { sums, slotSums: [...new Set(slotSums)].sort((a, b) => a - b) } : { why };
}

/** The kinds of our function `plainIndex` against its namesake `shiftedIndex` in the second link. */
export function classifyFunction(
  plain: KindsSide,
  shifted: KindsSide,
  shifts: LinkShifts,
  plainIndex: number,
  shiftedIndex: number,
): FunctionKinds {
  const pf = plain.module.funcs[plainIndex - plain.module.importedFuncCount];
  const sf = shifted.module.funcs[shiftedIndex - shifted.module.importedFuncCount];
  if (pf === undefined || sf === undefined) {
    return { verified: false, why: 'not a defined function of both links' };
  }
  const locals = (f: typeof pf): Buffer => Buffer.from(f.bodyBytes.subarray(0, f.codeStart));
  if (!locals(pf).equals(locals(sf))) {
    return { verified: false, why: 'the locals differ in the shifted link' };
  }
  if (signature(plain.module, pf.typeIndex) !== signature(shifted.module, sf.typeIndex)) {
    return { verified: false, why: 'the signature differs in the shifted link' };
  }
  const operands: Operand[] = [];
  const instrs = disassemble(pf);
  const why = sameShape(plain, shifted, instrs, disassemble(sf), operands);
  if (why !== null) {
    return { verified: false, why: `another shape in the shifted link: ${why}` };
  }
  const pElem = activeElemOf(plain.module);
  const sElem = activeElemOf(shifted.module);
  // both links' slots hold the same function, or are both empty (then the slot has no target image: unresolved)
  const sameSlot = (p: number, s: number): boolean => {
    const fp = pElem.get(p);
    const fs = sElem.get(s);
    if (fp === undefined || fs === undefined) {
      return fp === fs;
    }
    const np = plain.names.get(fp);
    return np !== undefined && np === shifted.names.get(fs);
  };
  const byAt = new Map(operands.map((o) => [o.at, o]));
  const where = (o: Operand): string => `+0x${o.at.toString(16)} ${o.op} ${o.plain}`;
  const ops = new Map<number, MovedKind>();
  const tableBases = new Set<number>();
  const dataBases = new Set<number>();
  const slotConsts = new Set<number>();
  for (const o of operands) {
    const moved = o.shifted - o.plain;
    if (moved === 0n) {
      continue;
    }
    const kind = moved === BigInt(shifts.dataShift) ? 'address' : moved === BigInt(shifts.tableShift) ? 'slot' : null;
    if (kind === null) {
      return { verified: false, why: `${where(o)} moved to ${o.shifted}, by neither shift` };
    }
    if (kind === 'slot' && !sameSlot(Number(o.plain), Number(o.shifted))) {
      return { verified: false, why: `${where(o)}: the two links' slots hold different functions` };
    }
    if (o.op === 'i32.const' && o.plain === BigInt(kind === 'slot' ? shifts.tableBase : shifts.dataBase)) {
      (kind === 'slot' ? tableBases : dataBases).add(o.at);
    } else if (o.op === 'i32.const' && kind === 'slot') {
      slotConsts.add(o.at);
    }
    ops.set(o.at, kind);
  }
  if (tableBases.size + dataBases.size + slotConsts.size === 0) {
    return { verified: true, ops };
  }
  // a constant equal to the base: a slot or an address used as such, or the base of a sum binaryen did not fold; any
  // other slot: a pointer, unless it is added to constants
  const r = baseSums(plain.module, pf, instrs, new Set([...tableBases, ...dataBases]), slotConsts);
  if ('why' in r) {
    return { verified: false, why: r.why };
  }
  if (r.slotSums.length > 0) {
    const o = byAt.get(r.slotSums[0])!;
    return {
      verified: false,
      why: `${where(o)} is a table slot added to constants: a __table_base sum with part of the base in it`,
    };
  }
  for (const [at, rels] of r.sums) {
    const base = byAt.get(at)!;
    if (dataBases.has(at)) {
      return { verified: false, why: `${where(base)} is __memory_base itself, added to operands relative to it` };
    }
    ops.set(at, 'table-base');
    for (const k of rels) {
      const rel = byAt.get(k)!;
      if (rel.shifted !== rel.plain) {
        return { verified: false, why: `${where(rel)} is added to __table_base (${where(base)}) and moved` };
      }
      const v = Number(rel.plain);
      if (!sameSlot(shifts.tableBase + v, shifts.tableBase + shifts.tableShift + v)) {
        return {
          verified: false,
          why: `${where(rel)} is added to __table_base (${where(base)}): the two links' slots hold different functions`,
        };
      }
      ops.set(k, 'table-rel');
    }
  }
  return { verified: true, ops };
}

const elemCache = new WeakMap<Module, Map<number, number>>();
function activeElemOf(m: Module): Map<number, number> {
  let e = elemCache.get(m);
  if (e === undefined) {
    const { offset, funcs } = activeElem(m);
    e = new Map(funcs.map((f, k) => [offset + k, f]));
    elemCache.set(m, e);
  }
  return e;
}

/** Every defined function of the plain link, aligned by name with the shifted link and classified. */
export function classifyOperands(plain: KindsSide, shifted: KindsSide): OperandKinds {
  const shifts = linkShifts(plain.module, shifted.module);
  const byName = new Map<string, number[]>();
  for (const f of shifted.module.funcs) {
    const n = shifted.names.get(f.index);
    if (n !== undefined) {
      byName.set(n, [...(byName.get(n) ?? []), f.index]);
    }
  }
  const functions = new Map<number, FunctionKinds>();
  for (const f of plain.module.funcs) {
    const name = plain.names.get(f.index);
    const there = name === undefined ? undefined : byName.get(name);
    functions.set(
      f.index,
      name === undefined
        ? { verified: false, why: 'our function has no name' }
        : there === undefined || there.length !== 1
          ? { verified: false, why: `${there?.length ?? 'no'} functions of the shifted link are ${name}` }
          : classifyFunction(plain, shifted, shifts, f.index, there[0]),
    );
  }
  return {
    plainSha256: sha256Of(plain.module.bytes),
    shiftedSha256: sha256Of(shifted.module.bytes),
    dataShift: shifts.dataShift,
    tableShift: shifts.tableShift,
    functions,
  };
}

/** The kind of the operand of our function's instruction at body offset `at`; undefined when the function's kinds are
 *  not known. */
export function operandKind(f: FunctionKinds | undefined, at: number): OperandKind | undefined {
  return f?.verified ? (f.ops.get(at) ?? 'literal') : undefined;
}

/** kinds.json's key per kind: a function's row lists the body offsets of each kind but literal */
const KEYS = { address: 'a', slot: 's', 'table-base': 'b', 'table-rel': 'r' } as const satisfies Record<
  MovedKind,
  string
>;

export function renderOperandKinds(k: OperandKinds): string {
  const functions: Record<string, Partial<Record<(typeof KEYS)[MovedKind], number[]>> | { unverified: string }> = {};
  for (const [index, f] of [...k.functions].sort((x, y) => x[0] - y[0])) {
    if (!f.verified) {
      functions[index] = { unverified: f.why };
      continue;
    }
    const row: Partial<Record<string, number[]>> = {};
    for (const [kind, key] of Object.entries(KEYS)) {
      const at = [...f.ops].filter(([, x]) => x === kind).map(([o]) => o);
      if (at.length > 0) {
        row[key] = at;
      }
    }
    functions[index] = row;
  }
  const { plainSha256: plain, shiftedSha256: shifted, dataShift, tableShift } = k;
  return `${JSON.stringify({ format: FORMAT, plain, shifted, dataShift, tableShift, functions })}\n`;
}

export function parseOperandKinds(text: string): OperandKinds {
  const j = JSON.parse(text);
  if (j.format !== FORMAT) {
    throw new Error(`not an operand-kind table (${FORMAT})`);
  }
  const functions = new Map<number, FunctionKinds>();
  const rows = j.functions as Record<string, Partial<Record<string, number[]>> & { unverified?: string }>;
  for (const [index, f] of Object.entries(rows)) {
    functions.set(
      Number(index),
      f.unverified !== undefined
        ? { verified: false, why: f.unverified }
        : {
            verified: true,
            ops: new Map(
              (Object.entries(KEYS) as [MovedKind, string][]).flatMap(([kind, key]) =>
                (f[key] ?? []).map((at): [number, MovedKind] => [at, kind]),
              ),
            ),
          },
    );
  }
  return {
    plainSha256: j.plain,
    shiftedSha256: j.shifted,
    dataShift: j.dataShift,
    tableShift: j.tableShift,
    functions,
  };
}

/** The table at `path`; throws unless it describes `plain` (the module's bytes). */
export function readOperandKinds(path: string, plain: Uint8Array): OperandKinds {
  const k = parseOperandKinds(readFileSync(path, 'utf8'));
  if (k.plainSha256 !== sha256Of(plain)) {
    throw new Error(`${path} describes another plain link (sha256 ${k.plainSha256}): run make -C src link-kinds`);
  }
  return k;
}

export interface KindsSummary {
  functions: number;
  verified: number;
  operands: Record<OperandKind, number>;
  /** why → how many functions */
  unverified: Map<string, number>;
}

/** Counts over the functions `among` (default: all), the literals counted from the bodies. */
export function kindsSummary(k: OperandKinds, m: Module, among?: Iterable<number>): KindsSummary {
  const out: KindsSummary = {
    functions: 0,
    verified: 0,
    operands: { literal: 0, address: 0, slot: 0, 'table-base': 0, 'table-rel': 0 },
    unverified: new Map(),
  };
  for (const index of among ?? k.functions.keys()) {
    const f = k.functions.get(index);
    out.functions++;
    if (f === undefined || !f.verified) {
      const why = f === undefined ? 'not in the table' : f.why.replace(/0x[0-9a-f]+/g, 'N').replace(/\b\d+\b/g, 'N');
      out.unverified.set(why, (out.unverified.get(why) ?? 0) + 1);
      continue;
    }
    out.verified++;
    let operands = 0;
    for (const i of disassemble(m.funcs[index - m.importedFuncCount])) {
      const constant = i.opcode === 0x41 || i.opcode === 0x42;
      operands += i.imms.filter((im) => im.kind === 'memarg' || constant).length;
    }
    for (const kind of f.ops.values()) {
      out.operands[kind]++;
    }
    out.operands.literal += operands - f.ops.size;
  }
  return out;
}
