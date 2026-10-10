// One of our function bodies re-pointed into the target's index and address spaces, so it can be compared with
// (and substituted for) a target body byte for byte.
//
// Every operand that names something is rewritten through the symbol maps (splat-symbols.ts):
//   call / return_call / ref.func   our function → its symbol → the target function (imports by real name)
//   call_indirect / block types     our type → the target type with the same signature
//   global.get / global.set         by index when the type agrees (__stack_pointer, tempRet0)
//   memarg offset ≥ GLOBAL_BASE     AMBIGUOUS: an address binaryen folded into the load, or a field offset;
//                                   it carries its address readings and its literal
//   i32.const                       AMBIGUOUS: an address, a table slot and a plain integer look alike, so it
//                                   carries every reading that resolves (address, table slot, literal); a function
//                                   the target's table holds in several slots offers each slot
//   i64.const                       a string_view clang packed into one constant, `(length << 32) | address`, when
//                                   the low word is one of our C strings of exactly that length: its address
//                                   readings, plus the literal
// An address reading is the target address of our object's symbol or content (splat-symbols resolveDataAddress:
// a C string through its NUL, read-only bytes, an object's bytes with its pointers re-pointed), plus every other
// target address holding the same content: the target body picks the copy it uses.
// A named operand that does not resolve makes the body unresolved, with the reason; an ambiguous operand whose
// address or slot reading does not resolve keeps only its literal reading and is listed in `soft`.
import { type Imm, type Instr, disassemble, instrText } from './disasm';
import { GLOBAL_BASE } from './normalize';
import type { Func, Module } from './parse';
import { SAME_CONTENT_LIMIT, type SymbolMaps, cStringAt, resolveDataAddress, resolveFunction } from './splat-symbols';

/** A stretch of the re-pointed body: fixed bytes, or the encodings an ambiguous operand may take. */
export type Piece = Uint8Array | { alternatives: Uint8Array[]; readings: string[] };

export interface Soft {
  /** body-relative offset of the instruction */
  at: number;
  instr: string;
  reason: string;
}

export interface Unresolved {
  at: number;
  instr: string;
  reason: string;
}

export type RemapResult =
  | {
      ok: true;
      /** every operand at its preferred reading: below GLOBAL_BASE the literal, above it the address, else the
       *  table slot, else the literal */
      bytes: Uint8Array;
      pieces: Piece[];
      /** constants and memarg offsets ≥ GLOBAL_BASE that look like an address or a table slot and resolved to
       *  neither */
      soft: Soft[];
      /** how many operands were rewritten, per kind */
      rewritten: Record<string, number>;
    }
  | { ok: false; unresolved: Unresolved[] };

function uleb(v: number): number[] {
  const out: number[] = [];
  let x = v >>> 0;
  for (;;) {
    const b = x & 0x7f;
    x >>>= 7;
    if (x === 0) {
      out.push(b);
      return out;
    }
    out.push(b | 0x80);
  }
}

function sleb(v: number): number[] {
  const out: number[] = [];
  let x = v | 0;
  for (;;) {
    const b = x & 0x7f;
    x >>= 7;
    if ((x === 0 && (b & 0x40) === 0) || (x === -1 && (b & 0x40) !== 0)) {
      out.push(b);
      return out;
    }
    out.push(b | 0x80);
  }
}

/** The target addresses our address `v` may be: its resolution first, then the other places holding the same
 *  content. */
function addressReadings(maps: SymbolMaps, v: number): { addrs: number[]; readings: string[]; reason: string } {
  const r = resolveDataAddress(maps.ours, maps.ourImage, maps.target, v);
  const addrs = r.ok ? [r.addr] : [];
  const readings = r.ok ? [`address ${r.how}`] : [];
  for (const a of r.same ?? []) {
    if (!addrs.includes(a)) {
      addrs.push(a);
      readings.push(`address same-content 0x${a.toString(16)}`);
    }
  }
  return { addrs, readings, reason: r.ok ? '' : r.reason };
}

function sleb64(v: bigint): number[] {
  const out: number[] = [];
  let x = BigInt.asIntN(64, v);
  for (;;) {
    const b = Number(x & 0x7fn);
    x >>= 7n;
    if ((x === 0n && (b & 0x40) === 0) || (x === -1n && (b & 0x40) !== 0)) {
      out.push(b);
      return out;
    }
    out.push(b | 0x80);
  }
}

/** The address readings of an i64.const that packs a string_view `(length << 32) | address` of ours, else null. */
function packedStringViewReadings(maps: SymbolMaps, v: bigint): { values: bigint[]; readings: string[] } | null {
  if (v <= 0n || v >> 32n >= 1n << 31n) {
    return null;
  }
  const addr = Number(v & 0xffffffffn);
  const length = Number(v >> 32n);
  if (length === 0 || addr < GLOBAL_BASE || addr >= maps.ours.dataEnd) {
    return null;
  }
  if (cStringAt(maps.ourImage, GLOBAL_BASE, addr)?.length !== length) {
    return null;
  }
  const a = addressReadings(maps, addr);
  if (a.addrs.length === 0) {
    return null;
  }
  return {
    values: a.addrs.map((x) => (BigInt(length) << 32n) | BigInt(x)),
    readings: a.readings.map((r) => `${r} (string_view)`),
  };
}

/** s33 block type index (non-negative) */
function s33(v: number): number[] {
  return sleb(v);
}

const CALLISH = new Set([0x10, 0x12, 0xd2]);

/** Re-point our function `ourIndex` into the target's spaces. */
export function remapBody(maps: SymbolMaps, ourIndex: number): RemapResult {
  const m = maps.ours.module;
  const fn: Func | undefined = m.funcs[ourIndex - m.importedFuncCount];
  if (fn === undefined) {
    return { ok: false, unresolved: [{ at: 0, instr: '', reason: `${ourIndex} is not a defined function of ours` }] };
  }
  const body = fn.bodyBytes;
  const instrs = disassemble(fn);
  const pieces: Piece[] = [body.subarray(0, fn.codeStart)];
  const unresolved: Unresolved[] = [];
  const soft: Soft[] = [];
  const rewritten: Record<string, number> = {};
  const count = (k: string): void => {
    rewritten[k] = (rewritten[k] ?? 0) + 1;
  };
  let fixed: number[] = [];
  const flush = (): void => {
    if (fixed.length > 0) {
      pieces.push(Uint8Array.from(fixed));
      fixed = [];
    }
  };
  const copy = (from: number, to: number): void => {
    for (let k = from; k < to; k++) {
      fixed.push(body[k]);
    }
  };
  const fail = (i: Instr, reason: string): void => {
    unresolved.push({ at: i.offset, instr: instrText(i, m), reason });
  };

  for (const i of instrs) {
    const starts = i.imms.map((im) => im.at);
    copy(i.offset, starts[0] ?? i.end);
    i.imms.forEach((im: Imm, j: number) => {
      const end = starts[j + 1] ?? i.end;
      switch (im.kind) {
        case 'u32': {
          if (im.role === 'func' && CALLISH.has(i.opcode)) {
            const r = resolveFunction(maps, im.value);
            if (!r.ok) {
              fail(i, r.reason);
              copy(im.at, end);
            } else {
              fixed.push(...uleb(r.index!));
              count('func');
            }
          } else if (im.role === 'type') {
            const t = maps.types.get(im.value);
            if (t === undefined) {
              fail(i, `type ${im.value} has no target type with its signature`);
              copy(im.at, end);
            } else {
              fixed.push(...uleb(t));
              count('type');
            }
          } else if (im.role === 'global') {
            const g = maps.globals.get(im.value);
            if (g === undefined) {
              fail(i, `global ${im.value} has no target global`);
              copy(im.at, end);
            } else {
              fixed.push(...uleb(g));
              count('global');
            }
          } else if ((im.role === 'table' || im.role === 'memory') && im.value !== 0) {
            fail(i, `${im.role} ${im.value}: only table/memory 0 exist`);
            copy(im.at, end);
          } else if (im.role === 'data' || im.role === 'elem' || im.role === 'tag') {
            fail(i, `${im.role} segment/tag index ${im.value} is not mapped`);
            copy(im.at, end);
          } else {
            copy(im.at, end);
          }
          return;
        }
        case 'blocktype': {
          if (im.value.kind === 'type') {
            const t = maps.types.get(im.value.index);
            if (t === undefined) {
              fail(i, `block type ${im.value.index} has no target type with its signature`);
              copy(im.at, end);
            } else {
              fixed.push(...s33(t));
              count('type');
            }
          } else {
            copy(im.at, end);
          }
          return;
        }
        case 'memarg': {
          copy(im.at, im.offsetAt);
          if (im.offset >= GLOBAL_BASE && im.offset < maps.ours.dataEnd) {
            // binaryen folds a constant address into the offset, but a field offset of 1024 or more looks the same
            const a = addressReadings(maps, im.offset);
            if (a.addrs.length === 0) {
              soft.push({ at: i.offset, instr: instrText(i, m), reason: `offset: ${a.reason}` });
              copy(im.offsetAt, end);
            } else {
              flush();
              pieces.push({
                alternatives: [...a.addrs.map((x) => Uint8Array.from(uleb(x))), body.slice(im.offsetAt, end)],
                readings: [...a.readings, 'literal'],
              });
              count('memarg');
            }
          } else {
            copy(im.offsetAt, end);
          }
          return;
        }
        case 's32': {
          if (i.opcode !== 0x41) {
            copy(im.at, end);
            return;
          }
          const v = im.value;
          const alternatives: Uint8Array[] = [];
          const readings: string[] = [];
          const reasons: string[] = [];
          if (v >= GLOBAL_BASE && v < maps.ours.dataEnd) {
            const a = addressReadings(maps, v);
            if (a.addrs.length === 0) {
              reasons.push(`address: ${a.reason}`);
            }
            a.addrs.forEach((x, k) => {
              alternatives.push(Uint8Array.from(sleb(x)));
              readings.push(a.readings[k]);
            });
          }
          if (v >= 1 && v < maps.ours.tableSize) {
            const f = maps.ours.elem.get(v);
            const r = f === undefined ? null : resolveFunction(maps, f);
            const slots = r?.ok ? maps.target.slots.get(r.index!) : undefined;
            if (slots?.length === 1) {
              alternatives.push(Uint8Array.from(sleb(slots[0])));
              readings.push(`table slot of ${r!.ok ? r!.how : ''}`);
            } else if (slots !== undefined && slots.length <= SAME_CONTENT_LIMIT) {
              // the target's table holds the function in several slots: each is a pointer to it
              for (const slot of slots) {
                alternatives.push(Uint8Array.from(sleb(slot)));
                readings.push(`table slot copy ${slot} of ${r!.ok ? r!.how : ''}`);
              }
            } else {
              reasons.push(
                `table: ${
                  f === undefined
                    ? 'empty slot'
                    : r !== null && !r.ok
                      ? r.reason
                      : slots === undefined
                        ? 'the target function is in no table slot'
                        : `the target function fills ${slots.length} slots`
                }`,
              );
            }
          }
          // below GLOBAL_BASE a constant is an integer far more often than a table slot: no reading is no news
          if (v >= GLOBAL_BASE && reasons.length > 0 && alternatives.length === 0) {
            soft.push({ at: i.offset, instr: instrText(i, m), reason: reasons.join('; ') });
          }
          if (alternatives.length === 0) {
            copy(im.at, end);
            return;
          }
          count(readings[0].split(' ')[0] === 'address' ? 'address' : 'table');
          const literal = body.subarray(im.at, end);
          // the preferred reading leads: below GLOBAL_BASE the integer, above it the address or slot
          if (v < GLOBAL_BASE) {
            alternatives.unshift(literal);
            readings.unshift('literal');
          } else {
            alternatives.push(literal);
            readings.push('literal');
          }
          flush();
          pieces.push({ alternatives, readings });
          return;
        }
        case 's64': {
          const sv = i.opcode === 0x42 ? packedStringViewReadings(maps, im.value) : null;
          if (sv === null) {
            copy(im.at, end);
            return;
          }
          flush();
          pieces.push({
            alternatives: [...sv.values.map((x) => Uint8Array.from(sleb64(x))), body.slice(im.at, end)],
            readings: [...sv.readings, 'literal'],
          });
          count('address');
          return;
        }
        default:
          copy(im.at, end);
      }
    });
  }
  flush();
  if (unresolved.length > 0) {
    return { ok: false, unresolved };
  }
  return { ok: true, bytes: preferred(pieces), pieces, soft, rewritten };
}

function preferred(pieces: Piece[]): Uint8Array {
  const parts = pieces.map((p) => (p instanceof Uint8Array ? p : p.alternatives[0]));
  const out = new Uint8Array(parts.reduce((n, p) => n + p.length, 0));
  let k = 0;
  for (const p of parts) {
    out.set(p, k);
    k += p.length;
  }
  return out;
}

export interface TargetMatch {
  match: boolean;
  /** the readings chosen for each ambiguous operand, when it matches */
  readings?: string[];
  /** body offset of the first byte no reading accounts for, when it does not */
  firstDifference?: number;
}

/** Whether some choice of readings makes the pieces exactly `targetBody`. */
export function matchesTarget(pieces: readonly Piece[], targetBody: Uint8Array): TargetMatch {
  let furthest = 0;
  const failed = new Set<string>();
  const chosen: string[] = [];
  const walk = (p: number, at: number): boolean => {
    if (p === pieces.length) {
      return at === targetBody.length;
    }
    const key = `${p}:${at}`;
    if (failed.has(key)) {
      return false;
    }
    const piece = pieces[p];
    const options =
      piece instanceof Uint8Array
        ? [{ bytes: piece, reading: null }]
        : piece.alternatives.map((bytes, k) => ({ bytes, reading: piece.readings[k] }));
    for (const o of options) {
      let k = 0;
      while (k < o.bytes.length && at + k < targetBody.length && o.bytes[k] === targetBody[at + k]) {
        k++;
      }
      furthest = Math.max(furthest, at + k);
      if (k === o.bytes.length) {
        if (o.reading !== null) {
          chosen.push(o.reading);
        }
        if (walk(p + 1, at + k)) {
          return true;
        }
        if (o.reading !== null) {
          chosen.pop();
        }
      }
    }
    failed.add(key);
    return false;
  };
  return walk(0, 0) ? { match: true, readings: chosen } : { match: false, firstDifference: furthest };
}

/** A body with only its function and type references (call, return_call, ref.func, call_indirect, block types)
 *  re-pointed; null when one has no image. For two links of one program whose functions and types differ only in
 *  order. */
export function repointIndices(
  m: Module,
  index: number,
  fnMap: (fn: number) => number | undefined,
  typeMap: (type: number) => number | undefined,
): Uint8Array | null {
  const fn = m.funcs[index - m.importedFuncCount];
  const body = fn.bodyBytes;
  const out: number[] = Array.from(body.subarray(0, fn.codeStart));
  for (const i of disassemble(fn)) {
    let from = i.offset;
    for (const [j, im] of i.imms.entries()) {
      let to: number[] | null = null;
      if (im.kind === 'u32' && im.role === 'func' && CALLISH.has(i.opcode)) {
        const f = fnMap(im.value);
        if (f === undefined) {
          return null;
        }
        to = uleb(f);
      } else if (im.kind === 'u32' && im.role === 'type') {
        const t = typeMap(im.value);
        if (t === undefined) {
          return null;
        }
        to = uleb(t);
      } else if (im.kind === 'blocktype' && im.value.kind === 'type') {
        const t = typeMap(im.value.index);
        if (t === undefined) {
          return null;
        }
        to = s33(t);
      }
      if (to !== null) {
        for (let k = from; k < im.at; k++) {
          out.push(body[k]);
        }
        out.push(...to);
        from = i.imms[j + 1]?.at ?? i.end;
      }
    }
    for (let k = from; k < i.end; k++) {
      out.push(body[k]);
    }
  }
  return Uint8Array.from(out);
}
