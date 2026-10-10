// A splat-style split of a linked module, and the writer that puts it back together.
//
// splitModule turns a module into pieces that a build can own: one file per function body (the bytes
// after its size LEB: locals vector, instructions, final `end`), one file per data segment, and a
// manifest that holds everything else as decoded records (types, imports, the function→type map,
// tables, memories, globals, exports, start, elem segments, data offsets, section order). The
// assembler re-encodes the module from those pieces alone, optionally with replacement bodies for
// some functions. Every LEB is written minimally; the splitter re-encodes each section it decoded and
// refuses a module whose section differs from that re-encoding, so a split that succeeds is a
// split the writer can reproduce byte for byte.
import { createHash } from 'node:crypto';
import {
  existsSync,
  mkdirSync,
  readFileSync,
  readdirSync,
  rmdirSync,
  statSync,
  unlinkSync,
  writeFileSync,
} from 'node:fs';
import { dirname, join, relative } from 'node:path';

import { disassemble, instrText } from './disasm';
import { type Cursor, DecodeError, name as readName, s32, s64, bytes as takeBytes, u8, u32 } from './leb';
import { type Func, type Module, type ValType, parseModule } from './parse';

export const SPLAT_FORMAT = 'asmlift-wasm-splat/1';
export const MANIFEST = 'manifest.json';

export type ConstInit =
  | { op: 'i32.const'; value: number }
  | { op: 'i64.const'; value: string }
  | { op: 'f32.const'; bits: string }
  | { op: 'f64.const'; bits: string }
  | { op: 'global.get'; index: number }
  | { op: 'ref.func'; index: number }
  | { op: 'ref.null'; type: ValType };

export interface Limits {
  min: number;
  max: number | null;
  shared: boolean;
}

export type SplatImport =
  | { module: string; name: string; kind: 'func'; typeIndex: number }
  | { module: string; name: string; kind: 'table'; refType: ValType; limits: Limits }
  | { module: string; name: string; kind: 'memory'; limits: Limits }
  | { module: string; name: string; kind: 'global'; type: ValType; mutable: boolean };

export interface SplatElem {
  /** the segment's flags field (0–3: function indices; the expression forms 4–7 are not split) */
  flags: number;
  tableIndex: number;
  offset: ConstInit | null;
  elemKind: number;
  funcIndices: number[];
}

export interface SplatFunction {
  index: number;
  typeIndex: number;
  /** body bytes (the size LEB's value) */
  size: number;
  sha256: string;
  /** relative to the manifest */
  path: string;
  name: string | null;
  group: string;
}

export interface SplatData {
  index: number;
  flags: number;
  memoryIndex: number;
  offset: ConstInit | null;
  size: number;
  sha256: string;
  path: string;
}

export type SplatSection =
  | { id: number; name: string }
  | { id: 0; name: 'custom'; customName: string; size: number; sha256: string; path: string };

export interface SplatManifest {
  format: typeof SPLAT_FORMAT;
  source: { sha256: string; size: number };
  sections: SplatSection[];
  types: { params: ValType[]; results: ValType[] }[];
  imports: SplatImport[];
  tables: { refType: ValType; limits: Limits }[];
  memories: Limits[];
  globals: { type: ValType; mutable: boolean; init: ConstInit }[];
  exports: { name: string; kind: ExternKind; index: number }[];
  start: number | null;
  elems: SplatElem[];
  dataCount: number | null;
  functions: SplatFunction[];
  data: SplatData[];
}

type ExternKind = 'func' | 'table' | 'memory' | 'global' | 'tag';

const SECTION_NAMES: Record<number, string> = {
  1: 'type',
  2: 'import',
  3: 'function',
  4: 'table',
  5: 'memory',
  6: 'global',
  7: 'export',
  8: 'start',
  9: 'elem',
  10: 'code',
  11: 'data',
  12: 'datacount',
};

const VALTYPE_BYTE: Record<ValType, number> = {
  i32: 0x7f,
  i64: 0x7e,
  f32: 0x7d,
  f64: 0x7c,
  v128: 0x7b,
  funcref: 0x70,
  externref: 0x6f,
  exnref: 0x69,
};
const BYTE_VALTYPE = new Map(Object.entries(VALTYPE_BYTE).map(([t, b]) => [b, t as ValType]));
const EXTERN_KINDS: ExternKind[] = ['func', 'table', 'memory', 'global', 'tag'];
/** `\0asm` and version 1 */
const MAGIC = Uint8Array.of(0x00, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00);

export function sha256(b: Uint8Array): string {
  return createHash('sha256').update(b).digest('hex');
}

// ---- the encoder ---------------------------------------------------------------------------------

class Writer {
  #chunks: Uint8Array[] = [];
  #cur: number[] = [];
  length = 0;

  byte(b: number): void {
    this.#cur.push(b);
    this.length++;
  }

  u32(v: number): void {
    if (!Number.isInteger(v) || v < 0 || v > 0xffffffff) {
      throw new Error(`u32 out of range: ${v}`);
    }
    do {
      let b = v % 128;
      v = Math.floor(v / 128);
      if (v !== 0) {
        b |= 0x80;
      }
      this.byte(b);
    } while (v !== 0);
  }

  s64(v: bigint): void {
    for (;;) {
      const b = Number(v & 0x7fn);
      v >>= 7n;
      const done = (v === 0n && (b & 0x40) === 0) || (v === -1n && (b & 0x40) !== 0);
      this.byte(done ? b : b | 0x80);
      if (done) {
        return;
      }
    }
  }

  s32(v: number): void {
    this.s64(BigInt(v | 0));
  }

  raw(b: Uint8Array): void {
    this.#flush();
    this.#chunks.push(b);
    this.length += b.length;
  }

  name(s: string): void {
    const b = new TextEncoder().encode(s);
    this.u32(b.length);
    this.raw(b);
  }

  valType(t: ValType): void {
    this.byte(VALTYPE_BYTE[t]);
  }

  limits(l: Limits): void {
    this.byte((l.max === null ? 0 : 1) | (l.shared ? 2 : 0));
    this.u32(l.min);
    if (l.max !== null) {
      this.u32(l.max);
    }
  }

  constExpr(e: ConstInit): void {
    switch (e.op) {
      case 'i32.const':
        this.byte(0x41);
        this.s32(e.value);
        break;
      case 'i64.const':
        this.byte(0x42);
        this.s64(BigInt(e.value));
        break;
      case 'f32.const':
        this.byte(0x43);
        this.#fixed(BigInt(`0x${e.bits}`), 4);
        break;
      case 'f64.const':
        this.byte(0x44);
        this.#fixed(BigInt(`0x${e.bits}`), 8);
        break;
      case 'global.get':
        this.byte(0x23);
        this.u32(e.index);
        break;
      case 'ref.func':
        this.byte(0xd2);
        this.u32(e.index);
        break;
      case 'ref.null':
        this.byte(0xd0);
        this.valType(e.type);
        break;
    }
    this.byte(0x0b);
  }

  #fixed(v: bigint, n: number): void {
    for (let i = 0; i < n; i++) {
      this.byte(Number((v >> BigInt(8 * i)) & 0xffn));
    }
  }

  #flush(): void {
    if (this.#cur.length > 0) {
      this.#chunks.push(Uint8Array.from(this.#cur));
      this.#cur = [];
    }
  }

  finish(): Uint8Array {
    this.#flush();
    const out = new Uint8Array(this.length);
    let at = 0;
    for (const c of this.#chunks) {
      out.set(c, at);
      at += c.length;
    }
    return out;
  }
}

/** The bodies the writer emits: the manifest's pieces, or a replacement keyed by function index. */
export type BodySource = (f: SplatFunction) => Uint8Array;

function sectionPayload(
  m: SplatManifest,
  s: SplatSection,
  body: BodySource,
  piece: (path: string) => Uint8Array,
): Uint8Array {
  const w = new Writer();
  switch (s.id) {
    case 0: {
      if (!('customName' in s)) {
        throw new Error('custom section without a name in the manifest');
      }
      w.name(s.customName);
      w.raw(piece(s.path));
      break;
    }
    case 1:
      w.u32(m.types.length);
      for (const t of m.types) {
        w.byte(0x60);
        w.u32(t.params.length);
        t.params.forEach((v) => w.valType(v));
        w.u32(t.results.length);
        t.results.forEach((v) => w.valType(v));
      }
      break;
    case 2:
      w.u32(m.imports.length);
      for (const imp of m.imports) {
        w.name(imp.module);
        w.name(imp.name);
        w.byte(EXTERN_KINDS.indexOf(imp.kind));
        switch (imp.kind) {
          case 'func':
            w.u32(imp.typeIndex);
            break;
          case 'table':
            w.valType(imp.refType);
            w.limits(imp.limits);
            break;
          case 'memory':
            w.limits(imp.limits);
            break;
          case 'global':
            w.valType(imp.type);
            w.byte(imp.mutable ? 1 : 0);
            break;
        }
      }
      break;
    case 3:
      w.u32(m.functions.length);
      m.functions.forEach((f) => w.u32(f.typeIndex));
      break;
    case 4:
      w.u32(m.tables.length);
      for (const t of m.tables) {
        w.valType(t.refType);
        w.limits(t.limits);
      }
      break;
    case 5:
      w.u32(m.memories.length);
      m.memories.forEach((l) => w.limits(l));
      break;
    case 6:
      w.u32(m.globals.length);
      for (const g of m.globals) {
        w.valType(g.type);
        w.byte(g.mutable ? 1 : 0);
        w.constExpr(g.init);
      }
      break;
    case 7:
      w.u32(m.exports.length);
      for (const e of m.exports) {
        w.name(e.name);
        w.byte(EXTERN_KINDS.indexOf(e.kind));
        w.u32(e.index);
      }
      break;
    case 8:
      if (m.start === null) {
        throw new Error('the manifest lists a start section but no start function');
      }
      w.u32(m.start);
      break;
    case 9:
      w.u32(m.elems.length);
      for (const e of m.elems) {
        w.u32(e.flags);
        if ((e.flags & 1) === 0) {
          if (e.flags & 2) {
            w.u32(e.tableIndex);
          }
          if (e.offset === null) {
            throw new Error('an active elem segment without an offset');
          }
          w.constExpr(e.offset);
        }
        if (e.flags & 3) {
          w.byte(e.elemKind);
        }
        w.u32(e.funcIndices.length);
        e.funcIndices.forEach((i) => w.u32(i));
      }
      break;
    case 10:
      w.u32(m.functions.length);
      for (const f of m.functions) {
        const b = body(f);
        w.u32(b.length);
        w.raw(b);
      }
      break;
    case 11:
      w.u32(m.data.length);
      for (const d of m.data) {
        w.u32(d.flags);
        if (d.flags === 2) {
          w.u32(d.memoryIndex);
        }
        if (d.flags !== 1) {
          if (d.offset === null) {
            throw new Error(`data segment ${d.index} is active but has no offset`);
          }
          w.constExpr(d.offset);
        }
        const b = piece(d.path);
        w.u32(b.length);
        w.raw(b);
      }
      break;
    case 12:
      if (m.dataCount === null) {
        throw new Error('the manifest lists a datacount section but no count');
      }
      w.u32(m.dataCount);
      break;
    default:
      throw new Error(`section id ${s.id} is not written by this assembler`);
  }
  return w.finish();
}

function writeModule(m: SplatManifest, body: BodySource, piece: (path: string) => Uint8Array): Uint8Array {
  const w = new Writer();
  w.raw(MAGIC);
  for (const s of m.sections) {
    const payload = sectionPayload(m, s, body, piece);
    w.byte(s.id);
    w.u32(payload.length);
    w.raw(payload);
  }
  return w.finish();
}

// ---- the decoder ---------------------------------------------------------------------------------

function valType(c: Cursor): ValType {
  const at = c.pos;
  const t = BYTE_VALTYPE.get(u8(c));
  if (t === undefined) {
    throw new DecodeError('unknown value type', at);
  }
  return t;
}

function limits(c: Cursor): Limits {
  const at = c.pos;
  const flags = u8(c);
  if (flags & ~3) {
    throw new DecodeError(`limits flags 0x${flags.toString(16)} are not split`, at);
  }
  const min = u32(c);
  return { min, max: flags & 1 ? u32(c) : null, shared: (flags & 2) !== 0 };
}

function hexBits(c: Cursor, n: number): string {
  let v = 0n;
  const b = takeBytes(c, n);
  for (let i = n - 1; i >= 0; i--) {
    v = (v << 8n) | BigInt(b[i]);
  }
  return v.toString(16).padStart(2 * n, '0');
}

function constExpr(c: Cursor): ConstInit {
  const at = c.pos;
  const op = u8(c);
  let e: ConstInit;
  switch (op) {
    case 0x41:
      e = { op: 'i32.const', value: s32(c) };
      break;
    case 0x42:
      e = { op: 'i64.const', value: s64(c).toString() };
      break;
    case 0x43:
      e = { op: 'f32.const', bits: hexBits(c, 4) };
      break;
    case 0x44:
      e = { op: 'f64.const', bits: hexBits(c, 8) };
      break;
    case 0x23:
      e = { op: 'global.get', index: u32(c) };
      break;
    case 0xd2:
      e = { op: 'ref.func', index: u32(c) };
      break;
    case 0xd0:
      e = { op: 'ref.null', type: valType(c) };
      break;
    default:
      throw new DecodeError(`constant expression opcode 0x${op.toString(16)} is not split`, at);
  }
  if (u8(c) !== 0x0b) {
    throw new DecodeError('a constant expression longer than one instruction is not split', at);
  }
  return e;
}

function vec<T>(c: Cursor, item: (c: Cursor) => T): T[] {
  return Array.from({ length: u32(c) }, () => item(c));
}

interface RawSection {
  id: number;
  start: number;
  end: number;
}

function rawSections(input: Uint8Array): RawSection[] {
  if (firstDifference(input.subarray(0, 8), MAGIC) !== null) {
    throw new DecodeError('not a version-1 wasm module', 0);
  }
  const c: Cursor = { bytes: input, pos: 8 };
  const out: RawSection[] = [];
  while (c.pos < input.length) {
    const id = u8(c);
    const size = u32(c);
    out.push({ id, start: c.pos, end: c.pos + size });
    c.pos += size;
  }
  if (c.pos !== input.length) {
    throw new DecodeError('the last section runs past the end of the file', c.pos);
  }
  return out;
}

/** The manifest of `input` with pieces still in memory (`pieces`: path → bytes). */
export function decomposeModule(
  input: Uint8Array,
  layout: {
    functionPath: (index: number, name: string | null, group: string) => string;
    dataPath: (index: number, offset: number | null) => string;
    names?: ReadonlyMap<number, string>;
    groups?: ReadonlyMap<number, string>;
  },
): { manifest: SplatManifest; pieces: Map<string, Uint8Array> } {
  const m: SplatManifest = {
    format: SPLAT_FORMAT,
    source: { sha256: sha256(input), size: input.length },
    sections: [],
    types: [],
    imports: [],
    tables: [],
    memories: [],
    globals: [],
    exports: [],
    start: null,
    elems: [],
    dataCount: null,
    functions: [],
    data: [],
  };
  const pieces = new Map<string, Uint8Array>();
  let funcTypes: number[] = [];
  let importedFuncs = 0;
  let customs = 0;
  for (const s of rawSections(input)) {
    const c: Cursor = { bytes: input.subarray(0, s.end), pos: s.start };
    switch (s.id) {
      case 0: {
        const customName = readName(c);
        const payload = input.subarray(c.pos, s.end);
        const path = `custom/${customs++}-${fileSafe(customName, 40)}.bin`;
        pieces.set(path, payload);
        m.sections.push({ id: 0, name: 'custom', customName, size: payload.length, sha256: sha256(payload), path });
        c.pos = s.end;
        break;
      }
      case 1:
        m.types = vec(c, (c) => {
          const at = c.pos;
          if (u8(c) !== 0x60) {
            throw new DecodeError('a non-function type is not split', at);
          }
          return { params: vec(c, valType), results: vec(c, valType) };
        });
        break;
      case 2:
        m.imports = vec(c, (c): SplatImport => {
          const module = readName(c);
          const name = readName(c);
          const at = c.pos;
          const kind = EXTERN_KINDS[u8(c)];
          switch (kind) {
            case 'func':
              importedFuncs++;
              return { module, name, kind, typeIndex: u32(c) };
            case 'table':
              return { module, name, kind, refType: valType(c), limits: limits(c) };
            case 'memory':
              return { module, name, kind, limits: limits(c) };
            case 'global':
              return { module, name, kind, type: valType(c), mutable: u8(c) === 1 };
            default:
              throw new DecodeError(`import kind ${kind ?? '?'} is not split`, at);
          }
        });
        break;
      case 3:
        funcTypes = vec(c, u32);
        break;
      case 4:
        m.tables = vec(c, (c) => ({ refType: valType(c), limits: limits(c) }));
        break;
      case 5:
        m.memories = vec(c, limits);
        break;
      case 6:
        m.globals = vec(c, (c) => ({ type: valType(c), mutable: u8(c) === 1, init: constExpr(c) }));
        break;
      case 7:
        m.exports = vec(c, (c) => {
          const name = readName(c);
          const at = c.pos;
          const kind = EXTERN_KINDS[u8(c)];
          if (kind === undefined) {
            throw new DecodeError('unknown export kind', at);
          }
          return { name, kind, index: u32(c) };
        });
        break;
      case 8:
        m.start = u32(c);
        break;
      case 9:
        m.elems = vec(c, (c) => {
          const at = c.pos;
          const flags = u32(c);
          if (flags > 3) {
            throw new DecodeError(`elem segment flags ${flags} (expression elements) are not split`, at);
          }
          const tableIndex = (flags & 1) === 0 && flags & 2 ? u32(c) : 0;
          const offset = (flags & 1) === 0 ? constExpr(c) : null;
          const elemKind = flags & 3 ? u8(c) : 0;
          return { flags, tableIndex, offset, elemKind, funcIndices: vec(c, u32) };
        });
        break;
      case 10: {
        const n = u32(c);
        if (n !== funcTypes.length) {
          throw new DecodeError(`code section has ${n} bodies, the function section ${funcTypes.length}`, s.start);
        }
        for (let i = 0; i < n; i++) {
          const size = u32(c);
          const body = takeBytes(c, size);
          const index = importedFuncs + i;
          const name = layout.names?.get(index) ?? null;
          const group = layout.groups?.get(index) ?? 'unknown';
          const path = layout.functionPath(index, name, group);
          pieces.set(path, body);
          m.functions.push({ index, typeIndex: funcTypes[i], size, sha256: sha256(body), path, name, group });
        }
        break;
      }
      case 11:
        for (let index = 0, n = u32(c); index < n; index++) {
          const at = c.pos;
          const flags = u32(c);
          if (flags > 2) {
            throw new DecodeError(`data segment flags ${flags}`, at);
          }
          const memoryIndex = flags === 2 ? u32(c) : 0;
          const offset = flags === 1 ? null : constExpr(c);
          const b = takeBytes(c, u32(c));
          const path = layout.dataPath(index, offset?.op === 'i32.const' ? offset.value >>> 0 : null);
          pieces.set(path, b);
          m.data.push({ index, flags, memoryIndex, offset, size: b.length, sha256: sha256(b), path });
        }
        break;
      case 12:
        m.dataCount = u32(c);
        break;
      default:
        throw new DecodeError(`section id ${s.id} is not split`, s.start);
    }
    if (c.pos !== s.end) {
      throw new DecodeError(
        `section ${SECTION_NAMES[s.id] ?? s.id} decoded ${c.pos - s.start} of ${s.end - s.start} bytes`,
        c.pos,
      );
    }
    if (s.id !== 0) {
      m.sections.push({ id: s.id, name: SECTION_NAMES[s.id] });
    }
  }
  if (funcTypes.length !== m.functions.length) {
    throw new DecodeError('a function section without a code section is not split', 0);
  }
  // every section must come back as the bytes it was read from, or the writer cannot reproduce it
  const sections = rawSections(input);
  const original = (f: SplatFunction): Uint8Array => pieces.get(f.path)!;
  const piece = (p: string): Uint8Array => pieces.get(p)!;
  m.sections.forEach((s, k) => {
    const raw = sections[k];
    const was = input.subarray(raw.start, raw.end);
    const now = sectionPayload(m, s, original, piece);
    const at = firstDifference(was, now);
    if (at !== null) {
      throw new DecodeError(
        `section ${s.name} re-encodes differently from payload byte ${at} (${was.length} → ${now.length} bytes): a non-minimal LEB or a form this writer does not keep`,
        raw.start + at,
      );
    }
    const headerLeb = raw.start - (k === 0 ? 8 : sections[k - 1].end) - 1;
    if (headerLeb !== lebLength(was.length)) {
      throw new DecodeError(
        `section ${s.name}'s size LEB takes ${headerLeb} bytes, minimal is ${lebLength(was.length)}`,
        raw.start,
      );
    }
  });
  return { manifest: m, pieces };
}

function lebLength(v: number): number {
  let n = 1;
  while (v >= 128) {
    v = Math.floor(v / 128);
    n++;
  }
  return n;
}

function firstDifference(a: Uint8Array, b: Uint8Array): number | null {
  const n = Math.min(a.length, b.length);
  for (let i = 0; i < n; i++) {
    if (a[i] !== b[i]) {
      return i;
    }
  }
  return a.length === b.length ? null : n;
}

/** A path component from a name: unsafe characters become `_`, runs collapse, at most `max` characters. */
export function fileSafe(s: string, max: number): string {
  const t = s
    .replace(/[^A-Za-z0-9._+-]+/g, '_')
    .replace(/_+/g, '_')
    .replace(/^[_.]+|_+$/g, '');
  return t.slice(0, max).replace(/_+$/, '') || '_';
}

// ---- the split on disk ---------------------------------------------------------------------------

export interface SplitOptions {
  /** function index → best known name */
  names?: ReadonlyMap<number, string>;
  /** function index → owner/component; `unknown` otherwise */
  groups?: ReadonlyMap<number, string>;
  /** also write `<piece>.wat` beside each function body (default true) */
  wat?: boolean;
}

/** Each function's piece group: its component (func-attribution) when known, else its owner (the class
 *  map) spelled as the component that more than half of that owner's attributed functions carry
 *  (`libcxx` → `libc++`), else the owner itself; functions known to neither are left out. Groups that
 *  differ only by case share the spelling more functions carry, since a case-insensitive file system
 *  would merge their directories anyway. */
export function pieceGroups(
  indices: readonly number[],
  component: ReadonlyMap<number, string>,
  owner: ReadonlyMap<number, string>,
): Map<number, string> {
  const known = (s: string | undefined): s is string => s !== undefined && s !== '-' && s !== '';
  const tally = new Map<string, Map<string, number>>();
  for (const i of indices) {
    const o = owner.get(i);
    const c = component.get(i);
    if (known(o) && known(c)) {
      const t = tally.get(o) ?? new Map<string, number>();
      t.set(c, (t.get(c) ?? 0) + 1);
      tally.set(o, t);
    }
  }
  const ownerGroup = new Map<string, string>();
  for (const [o, t] of tally) {
    const total = [...t.values()].reduce((a, b) => a + b, 0);
    const [best, n] = [...t].sort((a, b) => b[1] - a[1])[0];
    if (2 * n > total) {
      ownerGroup.set(o, best);
    }
  }
  const raw = new Map<number, string>();
  for (const i of indices) {
    const c = component.get(i);
    const o = owner.get(i);
    const g = known(c) ? c : known(o) ? (ownerGroup.get(o) ?? o) : undefined;
    if (g !== undefined) {
      raw.set(i, g);
    }
  }
  const counts = new Map<string, number>();
  for (const g of raw.values()) {
    counts.set(g, (counts.get(g) ?? 0) + 1);
  }
  const spelling = new Map<string, string>();
  for (const [g] of [...counts].sort((a, b) => b[1] - a[1])) {
    if (!spelling.has(g.toLowerCase())) {
      spelling.set(g.toLowerCase(), g);
    }
  }
  return new Map([...raw].map(([i, g]) => [i, spelling.get(g.toLowerCase())!]));
}

export function functionPiecePath(index: number, name: string | null, group: string): string {
  return `asm/${fileSafe(group, 40)}/${index}${name === null ? '' : `-${fileSafe(name, 60)}`}.bin`;
}

export function dataPiecePath(index: number, offset: number | null): string {
  return `data/${String(index).padStart(4, '0')}${offset === null ? '' : `-${offset.toString(16).padStart(8, '0')}`}.bin`;
}

/** Write the pieces of `input` and its manifest under `outDir`. A previous split there (its manifest
 *  names every file it wrote) is removed first; any other non-empty directory is refused. */
export function splitModule(input: Uint8Array, outDir: string, opts: SplitOptions = {}): SplatManifest {
  const { manifest, pieces } = decomposeModule(input, {
    functionPath: functionPiecePath,
    dataPath: dataPiecePath,
    ...(opts.names ? { names: opts.names } : {}),
    ...(opts.groups ? { groups: opts.groups } : {}),
  });
  clearPreviousSplit(outDir);
  for (const [path, b] of pieces) {
    const p = join(outDir, path);
    mkdirSync(dirname(p), { recursive: true });
    writeFileSync(p, b);
  }
  if (opts.wat !== false) {
    const mod = parseModule(input);
    const names = new Map<number, string>(opts.names ?? []);
    for (const f of manifest.functions) {
      writeFileSync(
        join(outDir, watPath(f.path)),
        functionWat(mod, mod.funcs[f.index - mod.importedFuncCount], f, names),
      );
    }
  }
  writeFileSync(join(outDir, MANIFEST), renderManifest(manifest));
  return manifest;
}

export function watPath(binPath: string): string {
  return binPath.replace(/\.bin$/, '.wat');
}

function clearPreviousSplit(outDir: string): void {
  if (!existsSync(outDir)) {
    mkdirSync(outDir, { recursive: true });
    return;
  }
  const manifestPath = join(outDir, MANIFEST);
  if (!existsSync(manifestPath)) {
    if (readdirSync(outDir).length > 0) {
      throw new Error(`${outDir} is not empty and holds no ${MANIFEST}: refusing to split into it`);
    }
    return;
  }
  const old = readManifest(outDir);
  const files = [
    ...old.functions.flatMap((f) => [f.path, watPath(f.path)]),
    ...old.data.map((d) => d.path),
    ...old.sections.flatMap((s) => ('path' in s ? [s.path] : [])),
  ];
  const dirs = new Set<string>();
  for (const f of files) {
    const p = join(outDir, f);
    if (existsSync(p)) {
      unlinkSync(p);
    }
    dirs.add(dirname(p));
  }
  unlinkSync(manifestPath);
  // deepest first, and only the ones the split emptied
  for (const d of [...dirs].sort((a, b) => b.length - a.length)) {
    for (
      let cur = d;
      relative(outDir, cur) !== '' && existsSync(cur) && readdirSync(cur).length === 0;
      cur = dirname(cur)
    ) {
      rmdirSync(cur);
    }
  }
}

/** The manifest as JSON with one record per line in every list. */
export function renderManifest(m: SplatManifest): string {
  const lines = ['{'];
  const entries = Object.entries(m);
  entries.forEach(([k, v], i) => {
    const comma = i === entries.length - 1 ? '' : ',';
    if (Array.isArray(v) && v.length > 0) {
      lines.push(` ${JSON.stringify(k)}: [`);
      v.forEach((item, j) => lines.push(`  ${JSON.stringify(item)}${j === v.length - 1 ? '' : ','}`));
      lines.push(` ]${comma}`);
    } else {
      lines.push(` ${JSON.stringify(k)}: ${JSON.stringify(v)}${comma}`);
    }
  });
  lines.push('}');
  return `${lines.join('\n')}\n`;
}

export function readManifest(dir: string): SplatManifest {
  const path = join(dir, MANIFEST);
  const m = JSON.parse(readFileSync(path, 'utf8')) as SplatManifest;
  if (m.format !== SPLAT_FORMAT) {
    throw new Error(`${path}: format ${String(m.format)}, expected ${SPLAT_FORMAT}`);
  }
  return m;
}

/** A readable listing of one function: WAT-shaped, raw immediates, body offsets in `(;…;)`. */
export function functionWat(m: Module, fn: Func, f: SplatFunction, names: ReadonlyMap<number, string>): string {
  const sig = (xs: ValType[], kw: string): string => (xs.length === 0 ? '' : ` (${kw} ${xs.join(' ')})`);
  const out = [
    `;; function ${f.index}${f.name === null ? '' : ` ${f.name}`}  group ${f.group}  size ${f.size}`,
    `(func $f${f.index} (type ${fn.typeIndex})${sig(fn.params, 'param')}${sig(fn.results, 'result')}`,
  ];
  if (fn.locals.length > 0) {
    out.push(`  (local ${fn.locals.join(' ')})`);
  }
  let depth = 1;
  for (const i of disassemble(fn)) {
    if (i.opcode === 0x0b || i.opcode === 0x05 || i.opcode === 0x07) {
      depth--;
    }
    const callee =
      i.opcode === 0x10 || i.opcode === 0x12 ? names.get(i.imms[0]?.kind === 'u32' ? i.imms[0].value : -1) : undefined;
    out.push(
      `(;${i.offset.toString(16).padStart(5, '0')};) ${'  '.repeat(Math.max(depth, 0))}${instrText(i, m)}${callee === undefined ? '' : `  ;; ${callee}`}`,
    );
    if (
      i.opcode === 0x02 ||
      i.opcode === 0x03 ||
      i.opcode === 0x04 ||
      i.opcode === 0x06 ||
      i.opcode === 0x05 ||
      i.opcode === 0x07
    ) {
      depth++;
    }
  }
  out.push(')');
  return `${out.join('\n')}\n`;
}

// ---- the assembler -------------------------------------------------------------------------------

export interface AssembleResult {
  bytes: Uint8Array;
  sha256: string;
  /** whether the output is the module the split was taken from */
  identical: boolean;
  /** replaced functions whose bytes differ from their piece */
  substituted: number[];
  substitutedBytes: number;
  /** replaced functions whose bytes are their piece's */
  unchanged: number[];
}

/** Rebuild the module from the pieces under `dir`, with `substitutes` (function index → body bytes)
 *  in place of those functions' pieces. A piece whose bytes no longer hash to the manifest is an
 *  error: an edited piece is a substitution and goes through `substitutes`. */
export function assembleSplit(dir: string, substitutes: ReadonlyMap<number, Uint8Array> = new Map()): AssembleResult {
  const m = readManifest(dir);
  const byIndex = new Map(m.functions.map((f) => [f.index, f]));
  for (const [index, b] of substitutes) {
    const f = byIndex.get(index);
    if (f === undefined) {
      throw new Error(
        `substitute for function ${index}: not a defined function of the split (${m.functions[0]?.index}..${m.functions.at(-1)?.index})`,
      );
    }
    checkBody(index, f.typeIndex, b);
  }
  const piece = (path: string, expect?: { sha256: string; size: number }): Uint8Array => {
    const b = new Uint8Array(readFileSync(join(dir, path)));
    if (expect !== undefined && (b.length !== expect.size || sha256(b) !== expect.sha256)) {
      throw new Error(
        `${join(dir, path)}: the piece no longer has the bytes the manifest recorded (pass a changed body with --substitute)`,
      );
    }
    return b;
  };
  const substituted: number[] = [];
  const unchanged: number[] = [];
  let substitutedBytes = 0;
  const body: BodySource = (f) => {
    const original = piece(f.path, f);
    const sub = substitutes.get(f.index);
    if (sub === undefined) {
      return original;
    }
    if (firstDifference(original, sub) === null) {
      unchanged.push(f.index);
    } else {
      substituted.push(f.index);
      substitutedBytes += sub.length;
    }
    return sub;
  };
  const dataBySection = new Map(m.data.map((d) => [d.path, d]));
  const customByPath = new Map(m.sections.flatMap((s) => ('path' in s ? [[s.path, s] as const] : [])));
  const bytes = writeModule(m, body, (p) => piece(p, dataBySection.get(p) ?? customByPath.get(p)));
  const digest = sha256(bytes);
  return {
    bytes,
    sha256: digest,
    identical: digest === m.source.sha256 && bytes.length === m.source.size,
    substituted,
    unchanged,
    substitutedBytes,
  };
}

/** Check that `body` is one well-formed function body (locals vector, instructions, final `end`). */
export function checkBody(index: number, typeIndex: number, body: Uint8Array): void {
  const c: Cursor = { bytes: body, pos: 0 };
  const locals: ValType[] = [];
  try {
    for (let g = u32(c); g > 0; g--) {
      const n = u32(c);
      const t = valType(c);
      if (locals.length + n > 50_000) {
        throw new DecodeError('too many locals', c.pos);
      }
      for (let k = 0; k < n; k++) {
        locals.push(t);
      }
    }
    disassemble({
      index,
      typeIndex,
      params: [],
      results: [],
      locals,
      bodyOffset: 0,
      size: body.length,
      bodyBytes: body,
      codeStart: c.pos,
    });
  } catch (e) {
    throw new Error(`substitute for function ${index} is not a function body: ${(e as Error).message}`);
  }
}

const SUBSTITUTE_FILE = /^(\d+)(?:-[^/]*)?\.bin$/;

/** Replacement bodies from files and directories: a file named `<index>.bin` or `<index>-<anything>.bin`
 *  holds that function's body bytes (the bytes after the size LEB); a directory contributes every such
 *  file beneath it. The same index twice is an error. */
export function readSubstitutes(paths: readonly string[]): Map<number, Uint8Array> {
  const out = new Map<number, Uint8Array>();
  const from = new Map<number, string>();
  const add = (file: string): void => {
    const m = SUBSTITUTE_FILE.exec(file.split('/').at(-1) ?? '');
    if (m === null) {
      return;
    }
    const index = Number(m[1]);
    if (from.has(index)) {
      throw new Error(`function ${index} is substituted twice: ${from.get(index)} and ${file}`);
    }
    from.set(index, file);
    out.set(index, new Uint8Array(readFileSync(file)));
  };
  const walk = (p: string): void => {
    if (statSync(p).isDirectory()) {
      for (const e of readdirSync(p).sort()) {
        walk(join(p, e));
      }
    } else {
      add(p);
    }
  };
  for (const p of paths) {
    if (!statSync(p).isDirectory() && SUBSTITUTE_FILE.exec(p.split('/').at(-1) ?? '') === null) {
      throw new Error(`${p}: a substitute file is named <index>.bin or <index>-<name>.bin`);
    }
    walk(p);
  }
  return out;
}
