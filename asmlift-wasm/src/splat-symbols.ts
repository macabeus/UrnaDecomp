// Symbol maps on both sides of a splat build: what OUR link calls a function or a data address, and where the
// TARGET keeps the same thing.
//
// Our side is exact. `tools/round/link-map.sh` (urna) relinks the plain build with wasm-ld's `--Map` and replays
// binaryen on the kept intermediates, then checks the relink is byte-identical to the plain module, so its
// `names.tsv` (final function index → wasm-ld symbol) and `vota_web_wasm.map` (data symbol → address, which
// wasm-opt never moves) describe the plain module itself. A symbol is unique by name, else `name@object`.
//
// The target side is evidence, written to a splat-style symbol file (`splat/symbol_addrs.txt`):
//   glue, export the target glue's import and export names
//   pair         a function pair that is unique on BOTH sides at the census' fingerprint (then addr) tier
//   srcloc-anchor a function pair whose bodies are the same instructions and reference the same std::source_location
//                record, which no other function of either side references; it votes like a pair
//   call         a call operand of an aligned pair (same instructions modulo callees and addresses): our callee ↔
//                the target's callee, every vote agreeing
//   table        an i32.const of an aligned pair inside both tables that its address reading does not explain:
//                our slot's function ↔ the target slot's function
//   vote         an address operand of an aligned pair: our data symbol + offset ↔ the target address
//   rtti         typeinfo, typeinfo-name and vtable symbols from the ledger's classes.json / vtables.json
//   content      a read-only object of ours whose exact bytes occur once in the target's data
//   string       a string from the ledger's strings.tsv, named by its content (`str.<hash>`)
//   srcloc       a std::source_location record from source-locations.tsv, named by what it records
// A name with conflicting evidence is left out: a missing symbol costs a match, a wrong one makes a false one.
import { createHash } from 'node:crypto';
import { existsSync, readFileSync } from 'node:fs';
import { dirname, join, resolve } from 'node:path';

import type { Instr } from './disasm';
import { glueNamesFor } from './glue-file';
import { importResolver } from './imports';
import { GLOBAL_BASE, type NormalizedFn, normalizeModule } from './normalize';
import { type Module, parseModule, typeText } from './parse';
import type { OperandKinds } from './splat-kinds';

// ---------------------------------------------------------------------------------------------------------------
// wasm-ld --Map

export interface MapEntry {
  /** virtual address for data, null for sections without one (code) */
  addr: number | null;
  /** file offset in the wasm-ld output */
  off: number;
  size: number;
}

export interface DataObject {
  addr: number;
  size: number;
  /** the symbol, unique in the link (`name@object` when several objects define `name`) */
  key: string;
  /** the symbol as wasm-ld wrote it; `.L…` names are local and not unique */
  name: string;
  object: string;
  /** the input section (`.rodata.foo`, `.data.rel.ro.bar`, `.bss.baz`), `<merged strings>` for wasm-ld's own */
  section: string;
  /** the output section: `.rodata`, `.data`, `.bss`, `em_js` */
  out: string;
}

export interface LinkMap {
  outputs: (MapEntry & { name: string })[];
  /** data symbols and wasm-ld's merged-string section, sorted by address */
  data: DataObject[];
  /** the CODE section's input functions in wasm-ld order: (symbol, object) */
  functions: { name: string; object: string }[];
}

const MAP_LINE = /^\s*(\S+)\s+([0-9a-f]+)\s+([0-9a-f]+)( +)(.*)$/;

/** Parse wasm-ld's `--Map` output (`Addr Off Size Out In Symbol`, hex, nesting by indentation). */
export function parseLinkMap(text: string): LinkMap {
  const outputs: LinkMap['outputs'] = [];
  const raw: Omit<DataObject, 'key'>[] = [];
  const functions: LinkMap['functions'] = [];
  let out: string | null = null;
  // a data output section (.rodata, .data, .bss, em_js) has a virtual address; TYPE … GLOBAL … DATA do not
  let outIsData = false;
  let input: { object: string; section: string; addr: number; size: number; symbols: number } | null = null;
  const closeInput = (): void => {
    // an input section no symbol names: wasm-ld's merged strings (`<internal>:(.rodata)`)
    if (input !== null && input.symbols === 0 && out !== null && outIsData) {
      raw.push({
        addr: input.addr,
        size: input.size,
        name: `${input.object}:(${input.section})`,
        object: input.object,
        section: input.object === '<internal>' ? '<merged strings>' : input.section,
        out,
      });
    }
  };
  for (const line of text.split('\n')) {
    const m = MAP_LINE.exec(line);
    if (m === null) {
      continue;
    }
    const [, addrText, offText, sizeText, indent, rest] = m;
    const addr = addrText === '-' ? null : Number.parseInt(addrText, 16);
    const off = Number.parseInt(offText, 16);
    const size = Number.parseInt(sizeText, 16);
    if (indent.length === 1) {
      closeInput();
      input = null;
      out = rest.trim();
      outIsData = addr !== null;
      outputs.push({ name: out, addr, off, size });
    } else if (indent.length === 9) {
      closeInput();
      const sep = rest.lastIndexOf(':(');
      if (sep < 0 || !rest.endsWith(')')) {
        // the GLOBAL section lists globals by bare name; code and data entries are always `object:(section)`
        if (out === 'CODE' || outIsData) {
          throw new Error(`--Map: unreadable input section '${rest}' in ${out}`);
        }
        input = null;
        continue;
      }
      const object = rest.slice(0, sep);
      const section = rest.slice(sep + 2, -1);
      if (out === 'CODE') {
        functions.push({ name: section, object });
        input = null;
      } else if (!outIsData) {
        input = null;
      } else {
        input = { object, section, addr: addr ?? 0, size, symbols: 0 };
      }
    } else if (indent.length === 17 && input !== null && out !== null && outIsData && addr !== null) {
      input.symbols++;
      raw.push({ addr, size, name: rest.trim(), object: input.object, section: input.section, out });
    }
  }
  closeInput();
  const count = new Map<string, number>();
  for (const r of raw) {
    count.set(r.name, (count.get(r.name) ?? 0) + 1);
  }
  const data = raw
    .map((r) => ({ ...r, key: (count.get(r.name) ?? 0) > 1 ? `${r.name}@${r.object}` : r.name }))
    .sort((a, b) => a.addr - b.addr || b.size - a.size);
  return { outputs, data, functions };
}

/** The data object containing `addr`: the innermost (smallest) one when symbols nest. */
export function dataObjectAt(data: readonly DataObject[], addr: number): DataObject | null {
  let lo = 0;
  let hi = data.length - 1;
  let last = -1;
  while (lo <= hi) {
    const mid = (lo + hi) >> 1;
    if (data[mid].addr <= addr) {
      last = mid;
      lo = mid + 1;
    } else {
      hi = mid - 1;
    }
  }
  let best: DataObject | null = null;
  // a symbol nested in a bigger one (a merged-string section holds no symbols; vtables hold none) is rare:
  // look back over the few entries that may still cover addr
  for (let k = last; k >= 0 && k > last - 64; k--) {
    const d = data[k];
    if (addr < d.addr + d.size && (best === null || d.size < best.size)) {
      best = d;
    }
  }
  return best;
}

// ---------------------------------------------------------------------------------------------------------------
// our side

export interface OurFunction {
  index: number;
  /** unique symbol: the wasm-ld name, `name@object` when several objects define it */
  key: string;
  name: string;
  demangled: string;
  object: string;
}

export interface OurSide {
  path: string;
  module: Module;
  map: LinkMap;
  fns: Map<number, OurFunction>;
  fnByKey: Map<string, number>;
  /** import field → real name (glue) */
  importNames: Record<string, string>;
  /** [GLOBAL_BASE, end of .bss): where an i32.const may be one of our addresses */
  dataEnd: number;
  /** emscripten's stack sits right above .bss: __stack_pointer's initial value − dataEnd */
  stackSize: number;
  tableSize: number;
  /** table slot → function index */
  elem: Map<number, number>;
}

/** The scratch link beside a plain link: `<root>/build/out/wasm/vota_web/x.wasm` → `<root>/build/out/wasm-map/vota_web`. */
export function defaultMapDir(plainPath: string): string {
  const dir = dirname(resolve(plainPath));
  return join(dirname(dirname(dir)), 'wasm-map', dir.split('/').pop() ?? '');
}

/** Read `names.tsv` (link-map.sh): index, name, object, ld_index, post_oz_index, demangled. */
export function readOurNames(path: string): OurFunction[] {
  const rows = readFileSync(path, 'utf8')
    .split('\n')
    .slice(1)
    .filter((l) => l !== '');
  const parsed = rows.map((l) => {
    const [index, name, object, , , demangled = name] = l.split('\t');
    return { index: Number(index), name, object, demangled };
  });
  const count = new Map<string, number>();
  for (const p of parsed) {
    count.set(p.name, (count.get(p.name) ?? 0) + 1);
  }
  return parsed.map((p) => ({ ...p, key: (count.get(p.name) ?? 0) > 1 ? `${p.name}@${p.object}` : p.name }));
}

export function tableOf(m: Module): Map<number, number> {
  const out = new Map<number, number>();
  for (const e of m.elems) {
    if (e.mode !== 'active' || e.tableIndex !== 0 || typeof e.offset?.value !== 'number') {
      continue;
    }
    e.funcIndices.forEach((f, k) => out.set((e.offset!.value as number) + k, f));
  }
  return out;
}

/** Our plain link with its names and data map; throws when the map's scratch link is not the plain module. */
export function loadOurSide(plainPath: string, mapDir = defaultMapDir(plainPath)): OurSide {
  const bytes = new Uint8Array(readFileSync(plainPath));
  const scratch = join(mapDir, 'vota_web_wasm.wasm');
  if (!existsSync(scratch) || !Buffer.from(readFileSync(scratch)).equals(Buffer.from(bytes))) {
    throw new Error(
      `${mapDir} does not describe ${plainPath} (its scratch link is missing or differs): run make -C src link-map`,
    );
  }
  const module = parseModule(bytes);
  const map = parseLinkMap(readFileSync(join(mapDir, 'vota_web_wasm.map'), 'utf8'));
  const list = readOurNames(join(mapDir, 'names.tsv'));
  const fns = new Map(list.map((f) => [f.index, f]));
  const fnByKey = new Map(list.map((f) => [f.key, f.index]));
  const glue = glueNamesFor(plainPath);
  const bss = map.outputs.find((o) => o.name === '.bss');
  const lastData = map.data[map.data.length - 1];
  const dataEnd = bss !== undefined && bss.addr !== null ? bss.addr + bss.size : lastData.addr + lastData.size;
  const sp = module.globals[0]?.init.match(/^i32\.const (\d+)$/);
  return {
    path: plainPath,
    module,
    map,
    fns,
    fnByKey,
    importNames: glue?.imports ?? {},
    dataEnd,
    stackSize: sp ? Number(sp[1]) - dataEnd : 0,
    tableSize: module.tables[0]?.min ?? 0,
    elem: tableOf(module),
  };
}

// ---------------------------------------------------------------------------------------------------------------
// the symbol file

export type SymbolTarget =
  { kind: 'func'; index: number } | { kind: 'import'; index: number } | { kind: 'data'; addr: number };

export interface SymbolEntry {
  name: string;
  at: SymbolTarget;
  size?: number;
  evidence: string;
  /** a vote's target functions (`by:`), when at most VOTERS_LISTED of them voted */
  voters?: number[];
  comment?: string;
}

export const VOTERS_LISTED = 4;

export const HAND_EDIT_MARKER =
  '# ---- hand edits below this line: kept by regeneration, they win over generated lines ----';

const ENTRY = /^(\S+) = (func:(\d+)|import:(\d+)|0x([0-9a-f]+));(?:\s*\/\/\s*(.*))?$/;

export function renderSymbolEntry(e: SymbolEntry): string {
  const at = e.at.kind === 'data' ? `0x${e.at.addr.toString(16)}` : `${e.at.kind}:${e.at.index}`;
  const notes = [
    e.at.kind === 'data' ? 'type:data' : null,
    e.size !== undefined ? `size:0x${e.size.toString(16)}` : null,
    `evidence:${e.evidence}`,
    e.voters !== undefined ? `by:${e.voters.join(',')}` : null,
    e.comment ?? null,
  ].filter((s) => s !== null);
  return `${e.name} = ${at}; // ${notes.join(' ')}`;
}

export function parseSymbolLine(line: string): SymbolEntry | null {
  const m = ENTRY.exec(line.trim());
  if (m === null) {
    return null;
  }
  const [, name, , fn, imp, addr, notes = ''] = m;
  const at: SymbolTarget =
    fn !== undefined
      ? { kind: 'func', index: Number(fn) }
      : imp !== undefined
        ? { kind: 'import', index: Number(imp) }
        : { kind: 'data', addr: Number.parseInt(addr, 16) };
  const size = /size:0x([0-9a-f]+)/.exec(notes);
  const evidence = /evidence:(\S+)/.exec(notes);
  const by = /\bby:(\d+(?:,\d+)*)/.exec(notes);
  return {
    name,
    at,
    ...(size ? { size: Number.parseInt(size[1], 16) } : {}),
    evidence: evidence?.[1] ?? 'hand',
    ...(by ? { voters: by[1].split(',').map(Number) } : {}),
  };
}

export interface SymbolFile {
  generated: SymbolEntry[];
  /** the hand-edit section's lines, verbatim */
  handLines: string[];
  hand: SymbolEntry[];
}

export function parseSymbolFile(text: string): SymbolFile {
  const lines = text.split('\n');
  const cut = lines.indexOf(HAND_EDIT_MARKER);
  const gen = cut < 0 ? lines : lines.slice(0, cut);
  const handLines = cut < 0 ? [] : lines.slice(cut + 1);
  while (handLines.length > 0 && handLines[handLines.length - 1] === '') {
    handLines.pop();
  }
  const parse = (ls: string[]): SymbolEntry[] => ls.map(parseSymbolLine).filter((e): e is SymbolEntry => e !== null);
  return { generated: parse(gen), handLines, hand: parse(handLines) };
}

export function renderSymbolFile(header: string[], generated: SymbolEntry[], handLines: string[]): string {
  return `${[...header.map((h) => `# ${h}`), ...generated.map(renderSymbolEntry), HAND_EDIT_MARKER, ...handLines].join('\n')}\n`;
}

/** name → where, hand edits winning; a name with two generated places is dropped. */
export interface TargetSymbols {
  fns: Map<string, number>;
  imports: Map<string, number>;
  data: Map<string, number>;
  /** a data name's evidence and, for a vote, the target functions that cast it */
  dataEvidence: Map<string, { evidence: string; voters?: number[] }>;
  /** a function name's evidence and, when anchored pairs voted for it, the target functions that cast the votes */
  fnEvidence: Map<string, { evidence: string; voters?: number[] }>;
}

export function emptySymbols(): TargetSymbols {
  return { fns: new Map(), imports: new Map(), data: new Map(), dataEvidence: new Map(), fnEvidence: new Map() };
}

export function targetSymbolsOf(file: SymbolFile): TargetSymbols {
  const out = emptySymbols();
  for (const e of [...file.generated, ...file.hand]) {
    const into = e.at.kind === 'func' ? out.fns : e.at.kind === 'import' ? out.imports : out.data;
    into.set(e.name, e.at.kind === 'data' ? e.at.addr : e.at.index);
    const evidence = e.at.kind === 'data' ? out.dataEvidence : e.at.kind === 'func' ? out.fnEvidence : null;
    evidence?.set(e.name, { evidence: e.evidence, ...(e.voters ? { voters: e.voters } : {}) });
  }
  return out;
}

/** The target functions whose own operands are the only evidence for data name `name` (a vote cast by them
 *  alone); null when the name has evidence of another kind or more voters than the symbol file lists. */
export function soleVoters(symbols: TargetSymbols, name: string): number[] | null {
  const e = symbols.dataEvidence.get(name);
  return e !== undefined && /^vote:\d+$/.test(e.evidence) && e.voters !== undefined ? e.voters : null;
}

/** The target functions whose call or table operands are the only evidence for function name `name`; null when
 *  the name has evidence of another kind or its voters are not listed. */
export function soleFnVoters(symbols: TargetSymbols, name: string): number[] | null {
  const e = symbols.fnEvidence.get(name);
  return e !== undefined && (e.evidence === 'call' || e.evidence === 'table') && e.voters !== undefined
    ? e.voters
    : null;
}

// ---------------------------------------------------------------------------------------------------------------
// the target side

export interface TargetSide {
  path: string;
  module: Module;
  importNames: Record<string, string>;
  /** real import name → import index */
  importByName: Map<string, number>;
  symbols: TargetSymbols;
  /** function index → table slots holding it */
  slots: Map<number, number[]>;
  /** table slot → function index */
  elem: Map<number, number>;
  /** the data segments laid out as memory from GLOBAL_BASE */
  image: Buffer;
  imageBase: number;
  /** the end of the target's .bss, when known (OurSide.stackSize below __stack_pointer's initial value) */
  dataEnd: number | null;
  /** srcloc key → record address (see srclocKey) */
  srcloc: Map<string, number | 'ambiguous'>;
}

export function memoryImage(m: Module): { image: Buffer; base: number } {
  let hi = 0;
  for (const d of m.datas) {
    if (d.offset !== null) {
      hi = Math.max(hi, d.offset + d.bytes.length);
    }
  }
  const image = Buffer.alloc(Math.max(0, hi - GLOBAL_BASE));
  for (const d of m.datas) {
    if (d.offset !== null) {
      image.set(d.bytes, d.offset - GLOBAL_BASE);
    }
  }
  return { image, base: GLOBAL_BASE };
}

function u32At(image: Buffer, base: number, addr: number): number | null {
  const k = addr - base;
  return k >= 0 && k + 4 <= image.length ? image.readUInt32LE(k) : null;
}

/** The NUL-terminated bytes at addr (without the NUL), or null outside the image. */
export function cStringAt(image: Buffer, base: number, addr: number, max = 1 << 16): Buffer | null {
  const k = addr - base;
  if (k < 0 || k >= image.length) {
    return null;
  }
  const end = image.indexOf(0, k);
  if (end < 0 || end - k > max) {
    return null;
  }
  return image.subarray(k, end);
}

/** What a std::source_location record says: file, function, line, column (the record's four words). */
export function srclocKey(image: Buffer, base: number, addr: number): string | null {
  const [file, fn, line, col] = [0, 4, 8, 12].map((o) => u32At(image, base, addr + o));
  if (file === null || fn === null || line === null || col === null) {
    return null;
  }
  const f = cStringAt(image, base, file);
  const g = cStringAt(image, base, fn);
  if (f === null || g === null) {
    return null;
  }
  return `${f.toString('latin1')}\u0000${g.toString('latin1')}\u0000${line}\u0000${col}`;
}

/** `loc_addr` column of the ledger's source-locations.tsv → key. */
export function readSourceLocations(path: string, image: Buffer, base: number): Map<string, number | 'ambiguous'> {
  const out = new Map<string, number | 'ambiguous'>();
  for (const line of readFileSync(path, 'utf8').split('\n').slice(1)) {
    const addr = Number(line.split('\t')[0]);
    if (line === '' || !Number.isFinite(addr)) {
      continue;
    }
    const key = srclocKey(image, base, addr);
    if (key !== null) {
      out.set(key, out.has(key) ? 'ambiguous' : addr);
    }
  }
  return out;
}

export function loadTargetSide(
  targetPath: string,
  symbolFilePath: string | null,
  ledgerDir?: string,
  stackSize?: number,
): TargetSide {
  const module = parseModule(new Uint8Array(readFileSync(targetPath)));
  const glue = glueNamesFor(targetPath);
  const importNames = glue?.imports ?? {};
  const importByName = new Map<string, number>();
  module.imports.filter((i) => i.kind === 'func').forEach((i, k) => importByName.set(importNames[i.name] ?? i.name, k));
  const slots = new Map<number, number[]>();
  const elem = tableOf(module);
  for (const [slot, f] of elem) {
    const list = slots.get(f);
    if (list === undefined) {
      slots.set(f, [slot]);
    } else {
      list.push(slot);
    }
  }
  const { image, base } = memoryImage(module);
  const symbols =
    symbolFilePath !== null && existsSync(symbolFilePath)
      ? targetSymbolsOf(parseSymbolFile(readFileSync(symbolFilePath, 'utf8')))
      : emptySymbols();
  const locs = ledgerDir === undefined ? null : join(ledgerDir, 'source-locations.tsv');
  const srcloc = locs !== null && existsSync(locs) ? readSourceLocations(locs, image, base) : new Map();
  const sp = module.globals[0]?.init.match(/^i32\.const (\d+)$/);
  const dataEnd = sp && stackSize !== undefined ? Number(sp[1]) - stackSize : null;
  return {
    path: targetPath,
    module,
    importNames,
    importByName,
    symbols,
    slots,
    elem,
    image,
    imageBase: base,
    dataEnd,
    srcloc,
  };
}

// ---------------------------------------------------------------------------------------------------------------
// content identity

/** `same`: every other target address holding the same content (a string's bytes through its NUL, a read-only
 *  object's bytes), when there are at most SAME_CONTENT_LIMIT of them (STRING_COPY_LIMIT for a C string) */
export type Resolution =
  { ok: true; addr: number; how: string; same?: number[] } | { ok: false; reason: string; same?: number[] };

export const SAME_CONTENT_LIMIT = 64;
/** a C string's copies: the target's tail merging leaves short strings in hundreds of places, each the same string */
export const STRING_COPY_LIMIT = 4096;

function occurrences(hay: Buffer, needle: Buffer, limit = 3): number[] {
  const out: number[] = [];
  for (let k = hay.indexOf(needle); k >= 0 && out.length < limit; k = hay.indexOf(needle, k + 1)) {
    out.push(k);
  }
  return out;
}

const NUL = Buffer.from([0]);

/** A string of ours (wasm-ld merged strings or a `.L.str`) by its bytes: the whole NUL-terminated string that
 *  holds `addr`, found once in the target preceded by a NUL, at the same offset; failing that, the bytes from
 *  `addr` to the NUL found once anywhere (a tail another string shares). */
export function resolveString(ourImage: Buffer, target: TargetSide, obj: DataObject, addr: number): Resolution {
  const k = addr - GLOBAL_BASE;
  let s = k;
  const lo = obj.addr - GLOBAL_BASE;
  while (s > lo && ourImage[s - 1] !== 0) {
    s--;
  }
  const end = ourImage.indexOf(0, k);
  if (end < 0) {
    return { ok: false, reason: `no NUL after 0x${addr.toString(16)}` };
  }
  const whole = Buffer.concat([NUL, ourImage.subarray(s, end + 1)]);
  const hits = occurrences(target.image, whole, STRING_COPY_LIMIT + 1);
  // the same C string: the bytes from `addr` through the NUL, wherever the target's tail merging put them (a short
  // tail such as "" sits in too many places to offer them all; the whole string's places are offered anyway)
  const tailHits = occurrences(target.image, ourImage.subarray(k, end + 1), STRING_COPY_LIMIT + 2);
  const tails = tailHits.map((h) => target.imageBase + h);
  const found = [
    ...(hits.length > STRING_COPY_LIMIT ? [] : hits.map((h) => target.imageBase + h + 1 + (k - s))),
    ...(tails.length > STRING_COPY_LIMIT + 1 ? [] : tails),
  ];
  const places = [...new Set(found)];
  if (hits.length === 1) {
    const a = target.imageBase + hits[0] + 1 + (k - s);
    return { ok: true, addr: a, how: 'string', same: places.filter((p) => p !== a) };
  }
  if ((hits.length === 0 || k !== s) && tails.length === 1) {
    return { ok: true, addr: tails[0], how: 'string-tail' };
  }
  if (tails.length === 0) {
    return { ok: false, reason: 'string not in the target' };
  }
  return {
    ok: false,
    reason:
      hits.length === 0
        ? `string tail occurs ${tails.length} times`
        : `string occurs ${hits.length} times in the target`,
    same: places.length === 0 ? undefined : places,
  };
}

/** A read-only object of ours by its exact bytes: found once in the target at the same 4-byte alignment. */
export function resolveBytes(ourImage: Buffer, target: TargetSide, obj: DataObject): Resolution {
  const bytes = ourImage.subarray(obj.addr - GLOBAL_BASE, obj.addr - GLOBAL_BASE + obj.size);
  if (bytes.length !== obj.size || obj.size < 4) {
    return { ok: false, reason: 'object too small to identify by content' };
  }
  const align = obj.addr % 4 === 0 ? 4 : 1;
  const hits = occurrences(target.image, bytes, SAME_CONTENT_LIMIT + 1).filter(
    (h) => (target.imageBase + h) % align === 0,
  );
  if (hits.length === 1) {
    return { ok: true, addr: target.imageBase + hits[0], how: 'content' };
  }
  return hits.length === 0
    ? { ok: false, reason: 'bytes not in the target' }
    : {
        ok: false,
        reason: `bytes occur ${hits.length}+ times`,
        same: hits.length > SAME_CONTENT_LIMIT ? undefined : hits.map((h) => target.imageBase + h),
      };
}

export function isReadOnly(obj: DataObject): boolean {
  return obj.out === '.rodata';
}

/** clang's std::source_location record: `.L.constant`, or `.L.constant.<n>` after the first in an object. */
export function isSourceLocationRecord(obj: DataObject): boolean {
  return /^\.L\.constant(\.\d+)?$/.test(obj.name) && obj.size === 16;
}

export function isAnonymous(obj: DataObject): boolean {
  return obj.name.startsWith('.L') || obj.section === '<merged strings>';
}

/** A resolution moved by `off` bytes: the place of a byte inside an object resolved as a whole. */
function shifted(r: Resolution, off: number): Resolution {
  const same = r.same?.map((a) => a + off);
  return r.ok ? { ...r, addr: r.addr + off, ...(same ? { same } : {}) } : { ...r, ...(same ? { same } : {}) };
}

/** Results that depend on the two images alone (strings, read-only bytes), per our image, target image and key. */
const contentMemo = new WeakMap<Buffer, WeakMap<Buffer, Map<string, Resolution>>>();

function byContent(ourImage: Buffer, target: TargetSide, key: string, f: () => Resolution): Resolution {
  let byTarget = contentMemo.get(ourImage);
  if (byTarget === undefined) {
    byTarget = new WeakMap();
    contentMemo.set(ourImage, byTarget);
  }
  let memo = byTarget.get(target.image);
  if (memo === undefined) {
    memo = new Map();
    byTarget.set(target.image, memo);
  }
  let r = memo.get(key);
  if (r === undefined) {
    r = f();
    memo.set(key, r);
  }
  return r;
}

/** Where the target keeps the byte our `addr` names: the object's symbol, else what the object holds. */
export function resolveDataAddress(ours: OurSide, ourImage: Buffer, target: TargetSide, addr: number): Resolution {
  const obj = dataObjectAt(ours.map.data, addr);
  if (obj === null) {
    return { ok: false, reason: `0x${addr.toString(16)} is in no data object of ours` };
  }
  const off = addr - obj.addr;
  if (!isAnonymous(obj)) {
    const t = target.symbols.data.get(obj.key);
    if (t !== undefined) {
      return { ok: true, addr: t + off, how: `symbol ${obj.key}` };
    }
  }
  if (obj.section === '<merged strings>' || obj.name.startsWith('.L.str')) {
    return byContent(ourImage, target, `s${addr}`, () => resolveString(ourImage, target, obj, addr));
  }
  if (isSourceLocationRecord(obj)) {
    const key = srclocKey(ourImage, GLOBAL_BASE, obj.addr);
    const t = key === null ? undefined : target.srcloc.get(key);
    if (typeof t === 'number') {
      return { ok: true, addr: t + off, how: 'srcloc' };
    }
    return {
      ok: false,
      reason: t === 'ambiguous' ? 'source_location record ambiguous' : 'source_location not in the target',
    };
  }
  if (isReadOnly(obj)) {
    const r = readOnlyBytes(ourImage, target, obj);
    if (r.ok) {
      return shifted(r, off);
    }
    const p = r.reason === 'bytes not in the target' ? repointedTable(ours, ourImage, target).get(obj) : undefined;
    return p?.ok
      ? shifted(p, off)
      : {
          ok: false,
          reason: `${obj.key}: no target symbol; ${r.reason}${p === undefined ? '' : `; ${p.reason}`}`,
          same: (p?.same ?? r.same)?.map((a) => a + off),
        };
  }
  const p = obj.out === '.data' ? repointedTable(ours, ourImage, target).get(obj) : undefined;
  // a copy of read-only content is as good as another; a writable object is one object, whatever its first bytes
  const same = obj.section.startsWith('.data.rel.ro') ? p?.same : undefined;
  return p?.ok
    ? shifted(p, off)
    : {
        ok: false,
        reason: `${obj.key} (${obj.out}) has no target symbol${p === undefined ? '' : `; ${p.reason}`}`,
        ...(same ? { same: same.map((a) => a + off) } : {}),
      };
}

function readOnlyBytes(ourImage: Buffer, target: TargetSide, obj: DataObject): Resolution {
  return byContent(ourImage, target, `b${obj.addr}`, () => resolveBytes(ourImage, target, obj));
}

/** Our objects the re-pointed bytes may place: .data objects, and read-only ones whose exact bytes are not in the
 *  target, that have no target symbol. */
function repointable(ourImage: Buffer, target: TargetSide, obj: DataObject): boolean {
  if (
    (!isAnonymous(obj) && target.symbols.data.has(obj.key)) ||
    obj.section === '<merged strings>' ||
    obj.name.startsWith('.L.str') ||
    isSourceLocationRecord(obj)
  ) {
    return false;
  }
  if (obj.out === '.data') {
    return true;
  }
  const r = isReadOnly(obj) ? readOnlyBytes(ourImage, target, obj) : null;
  return r !== null && !r.ok && r.reason === 'bytes not in the target';
}

const repointedMemo = new WeakMap<OurSide, WeakMap<TargetSymbols, Map<DataObject, Resolution>>>();

/**
 * Every repointable object of ours placed by its re-pointed bytes (resolveRepointed) under one set of target names,
 * until no more resolve: a pointer word into an object placed in an earlier round re-points too. The rounds visit
 * the objects in address order, so the result depends on the names alone.
 */
function repointedTable(ours: OurSide, ourImage: Buffer, target: TargetSide): Map<DataObject, Resolution> {
  let bySymbols = repointedMemo.get(ours);
  if (bySymbols === undefined) {
    bySymbols = new WeakMap();
    repointedMemo.set(ours, bySymbols);
  }
  const known = bySymbols.get(target.symbols);
  if (known !== undefined) {
    return known;
  }
  // visible while it fills: a word pointing at an object not placed yet does not resolve
  const table = new Map<DataObject, Resolution>();
  bySymbols.set(target.symbols, table);
  const maps: SymbolMaps = { ours, ourImage, target, types: new Map(), globals: new Map() };
  const objects = ours.map.data.filter((o) => repointable(ourImage, target, o));
  // the words' values of each object's pattern when it was last searched: the same pattern finds the same places
  const searched = new Map<DataObject, string>();
  for (let changed = true; changed;) {
    changed = false;
    for (const o of objects) {
      const prev = table.get(o);
      if (prev?.ok) {
        continue;
      }
      const p = objectPattern(maps, o);
      const words = p.words.map((w) => `${w.at}:${w.values.join(',')}`).join(' ');
      if (searched.get(o) === words) {
        continue;
      }
      searched.set(o, words);
      const r = resolveRepointed(maps, o, p);
      const narrower = r.same !== undefined && (prev?.same === undefined || r.same.length < prev.same.length);
      if (r.ok || narrower) {
        table.set(o, r);
        changed = true;
      } else if (prev === undefined) {
        table.set(o, r);
      }
    }
  }
  return table;
}

/** The class an RTTI symbol at `addr` describes, read from the image: a `_ZTS` is the name itself, a `_ZTI` points
 *  at it from +4, a `_ZTV` points at its `_ZTI` from `vtableTypeinfo` (4, or more after virtual-base offsets).
 *  null when the words lead nowhere. */
export function rttiClassAt(
  image: Buffer,
  base: number,
  symbol: string,
  addr: number,
  vtableTypeinfo = 4,
): string | null {
  const kind = symbol.slice(0, 4);
  let nameAddr: number | null = addr;
  if (kind === '_ZTV' || kind === '_ZTI') {
    const ti = kind === '_ZTV' ? u32At(image, base, addr + vtableTypeinfo) : addr;
    nameAddr = ti === null ? null : u32At(image, base, ti + 4);
  }
  const s = nameAddr === null ? null : cStringAt(image, base, nameAddr);
  return s === null ? null : s.toString('latin1');
}

/** The mangled class an RTTI symbol (`_ZTV`, `_ZTI`, `_ZTS`, possibly `name@object`) is about, else null. */
export function rttiClassOf(symbol: string): string | null {
  const bare = symbol.split('@')[0];
  return /^_ZT[VIS]./.test(bare) ? bare.slice(4) : null;
}

/** Where our vtable `_ZTV<class>` keeps its typeinfo pointer: after the offset-to-top word, and after one more word
 *  per virtual-base or virtual-call offset ahead of it. 4 when our link has no such vtable or typeinfo. */
export function vtableTypeinfoOffset(
  ourByKey: ReadonlyMap<string, DataObject>,
  ourImage: Buffer,
  vtable: string,
): number {
  const cls = rttiClassOf(vtable);
  const vt = ourByKey.get(vtable);
  const ti = cls === null ? undefined : ourByKey.get(`_ZTI${cls}`);
  if (vt === undefined || ti === undefined) {
    return 4;
  }
  for (let q = 4; q + 4 <= vt.size; q += 4) {
    if (u32At(ourImage, GLOBAL_BASE, vt.addr + q) === ti.addr) {
      return q;
    }
  }
  return 4;
}

export type ObjectCheck = { verdict: 'agrees' | 'unknown' } | { verdict: 'differs'; at: number; why: string };

/**
 * Whether our data object `obj` and the target's bytes at `addr` are the same object: equal byte for byte, except
 * that a word of ours that is one of our addresses or table slots may be its re-pointed value. 'unknown' when our
 * object is all zeros (a .bss object says nothing) or a word differs only where our pointer does not resolve.
 */
export function compareObject(maps: SymbolMaps, obj: DataObject, addr: number): ObjectCheck {
  const { ours, ourImage, target } = maps;
  const ob = ourImage.subarray(obj.addr - GLOBAL_BASE, obj.addr - GLOBAL_BASE + obj.size);
  const tb = Buffer.alloc(obj.size);
  const k0 = addr - target.imageBase;
  if (k0 >= 0 && k0 < target.image.length) {
    target.image.copy(tb, 0, k0, Math.min(target.image.length, k0 + obj.size));
  }
  if (ob.length < obj.size || ob.every((b) => b === 0)) {
    return ob.length === obj.size && !tb.every((b) => b === 0)
      ? { verdict: 'differs', at: tb.findIndex((b) => b !== 0), why: 'our object is zeros, the target bytes are not' }
      : { verdict: 'unknown' };
  }
  let unknown = false;
  for (let k = 0; k < obj.size;) {
    const word = (obj.addr + k) % 4 === 0 && k + 4 <= obj.size;
    if (!word) {
      if (ob[k] !== tb[k]) {
        return { verdict: 'differs', at: k, why: `byte ${ob[k]} vs ${tb[k]}` };
      }
      k++;
      continue;
    }
    const ow = ob.readUInt32LE(k);
    const tw = tb.readUInt32LE(k);
    if (ow !== tw) {
      // our table overlaps our data: a word in both ranges may be either
      const resolved: number[] = [];
      let open = false;
      if (ow >= GLOBAL_BASE && ow < ours.dataEnd) {
        const r = resolveDataAddress(ours, ourImage, target, ow);
        resolved.push(...(r.ok ? [r.addr] : []), ...(r.same ?? []));
        open ||= !r.ok;
      }
      if (ow >= 1 && ow < ours.tableSize && ours.elem.has(ow)) {
        const r = resolveFunction(maps, ours.elem.get(ow)!);
        resolved.push(...(r.ok ? (target.slots.get(r.index!) ?? []) : []));
        open ||= !r.ok;
      }
      const str = ow >= GLOBAL_BASE && ow < ours.dataEnd ? dataObjectAt(ours.map.data, ow) : null;
      if (str !== null && (str.section === '<merged strings>' || str.name.startsWith('.L.str'))) {
        // the same C string, whichever copy the target's tail merging points at
        const a = cStringAt(ourImage, GLOBAL_BASE, ow);
        const b = cStringAt(target.image, target.imageBase, tw);
        if (a !== null && b !== null && a.equals(b)) {
          k += 4;
          continue;
        }
      }
      // past our data and inside our memory: the stack or the heap (sbrk's __heap_base), which nothing maps
      open ||= ow >= ours.dataEnd && ow < (ours.module.memories[0]?.min ?? 0) * 65536;
      if (!resolved.includes(tw)) {
        if (!open) {
          return {
            verdict: 'differs',
            at: k,
            why: `word ${ow} (re-pointed ${resolved.join('/') || 'nowhere'}) vs ${tw}`,
          };
        }
        unknown = true;
      }
    }
    k += 4;
  }
  return { verdict: unknown ? 'unknown' : 'agrees' };
}

/** What the target's bytes must be for one of our data objects, its pointers re-pointed: `fixed[k]` is the byte at
 *  +k, or null where a word holds one of `words`' values; a word whose pointer does not resolve is in neither. */
export interface ObjectPattern {
  fixed: (number | null)[];
  words: { at: number; values: number[] }[];
  /** non-zero fixed bytes, plus 4 per pointer word at or above GLOBAL_BASE and the non-zero bytes of one below it,
   *  which is as likely a small integer: how much of the pattern tells places apart */
  informative: number;
}

/**
 * Our object `obj` as the target must hold it: equal byte for byte, except that each aligned word that is one of our
 * addresses or table slots must be its re-pointed value (any copy, for content found at several places) or the plain
 * integer it also is, and a word whose pointer does not resolve, or that points past our data into the stack or the
 * heap, may be anything.
 */
export function objectPattern(maps: SymbolMaps, obj: DataObject): ObjectPattern {
  const { ours, ourImage, target } = maps;
  const bytes = ourImage.subarray(obj.addr - GLOBAL_BASE, obj.addr - GLOBAL_BASE + obj.size);
  const fixed: (number | null)[] = Array.from(bytes);
  const words: ObjectPattern['words'] = [];
  let informative = 0;
  const memoryEnd = (ours.module.memories[0]?.min ?? 0) * 65536;
  for (let k = 0; k + 4 <= bytes.length; k++) {
    if ((obj.addr + k) % 4 !== 0) {
      continue;
    }
    const w = bytes.readUInt32LE(k);
    const isAddress = w >= GLOBAL_BASE && w < ours.dataEnd;
    const isSlot = w >= 1 && w < ours.tableSize && ours.elem.has(w);
    if (!isAddress && !isSlot) {
      if (w >= ours.dataEnd && w < memoryEnd) {
        fixed.fill(null, k, k + 4);
      }
      continue;
    }
    fixed.fill(null, k, k + 4);
    const values: number[] = [];
    let open = false;
    if (isAddress) {
      const r = resolveDataAddress(ours, ourImage, target, w);
      values.push(...(r.ok ? [r.addr] : []), ...(r.same ?? []));
      open ||= !r.ok && r.same === undefined;
    }
    if (isSlot) {
      const r = resolveFunction(maps, ours.elem.get(w)!);
      values.push(...(r.ok ? (target.slots.get(r.index!) ?? []) : []));
      open ||= !r.ok;
    }
    if (open || values.length === 0) {
      continue;
    }
    // the word may also be the plain integer it is, as compareObject allows
    words.push({ at: k, values: [...new Set([...values, w])] });
    informative += w < GLOBAL_BASE ? bytes.subarray(k, k + 4).filter((b) => b !== 0).length : 4;
  }
  informative += fixed.filter((b) => b !== null && b !== 0).length;
  return { fixed, words, informative };
}

/** a pattern this informative places an object when it occurs once, as 4 exact bytes do for resolveBytes */
export const PATTERN_INFORMATIVE = 4;
/** occurrences of a search key past which it is too common to search by */
const PATTERN_SCAN_LIMIT = 20000;

/** Every place at `align` (mod 4) where the target image holds bytes the pattern accepts, up to `limit`; null when
 *  no key to search by is rare enough. */
function patternPlaces(image: Buffer, base: number, align: number, p: ObjectPattern, limit: number): number[] | null {
  // search keys, longest first: each fixed run of 4 bytes or more holding a non-zero byte, then each word with no
  // value below 256 (a byte-sized value is everywhere), by how few values it has, then pairs of adjacent words
  const keys: { at: number; needles: Buffer[] }[] = [];
  const runs: { from: number; to: number }[] = [];
  for (let k = 0, from = 0; k <= p.fixed.length; k++) {
    if (k === p.fixed.length || p.fixed[k] === null) {
      if (k - from >= 4 && p.fixed.slice(from, k).some((b) => b !== 0)) {
        runs.push({ from, to: k });
      }
      from = k + 1;
    }
  }
  runs.sort((x, y) => y.to - y.from - (x.to - x.from) || x.from - y.from);
  for (const r of runs) {
    keys.push({ at: r.from, needles: [Buffer.from(p.fixed.slice(r.from, r.to) as number[])] });
  }
  const word = (v: number): Buffer => {
    const b = Buffer.alloc(4);
    b.writeUInt32LE(v >>> 0);
    return b;
  };
  const words = p.words.filter((w) => w.values.every((v) => v >= 256));
  words.sort((x, y) => x.values.length - y.values.length);
  for (const w of words) {
    keys.push({ at: w.at, needles: w.values.map(word) });
  }
  // two adjacent words, every pair of their values: a table of small integers and function pointers has no other
  const pairs = p.words.flatMap((w, k) => {
    const next = p.words[k + 1];
    return next?.at === w.at + 4 && w.values.length * next.values.length <= 16 ? [{ w, next }] : [];
  });
  pairs.sort((x, y) => x.w.values.length * x.next.values.length - y.w.values.length * y.next.values.length);
  for (const { w, next } of pairs) {
    keys.push({
      at: w.at,
      needles: w.values.flatMap((a) => next.values.map((b) => Buffer.concat([word(a), word(b)]))),
    });
  }
  const accepts = (at: number): boolean =>
    at >= 0 &&
    at + p.fixed.length <= image.length &&
    (base + at) % 4 === align &&
    p.fixed.every((b, k) => b === null || image[at + k] === b) &&
    p.words.every((w) => w.values.includes(image.readUInt32LE(at + w.at)));
  for (const key of keys) {
    const starts: number[] = [];
    for (const needle of key.needles) {
      for (let h = image.indexOf(needle); h >= 0; h = image.indexOf(needle, h + 1)) {
        starts.push(h - key.at);
        if (starts.length > PATTERN_SCAN_LIMIT) {
          break;
        }
      }
      if (starts.length > PATTERN_SCAN_LIMIT) {
        break;
      }
    }
    if (starts.length > PATTERN_SCAN_LIMIT) {
      continue;
    }
    const places = [...new Set(starts)].sort((x, y) => x - y).filter(accepts);
    return places.slice(0, limit).map((at) => base + at);
  }
  return null;
}

/**
 * Where the target holds our data object `obj`, found by its re-pointed bytes (objectPattern): the one place at the
 * object's alignment whose bytes the pattern accepts; with several places, up to SAME_CONTENT_LIMIT of them in
 * `same`, for the target body to pick from.
 */
export function resolveRepointed(maps: SymbolMaps, obj: DataObject, p = objectPattern(maps, obj)): Resolution {
  if (p.informative < PATTERN_INFORMATIVE) {
    return { ok: false, reason: 'too little content to place by its re-pointed bytes' };
  }
  const places = patternPlaces(maps.target.image, maps.target.imageBase, obj.addr % 4, p, SAME_CONTENT_LIMIT + 1);
  if (places === null) {
    return { ok: false, reason: 'its re-pointed bytes are too common to search' };
  }
  if (places.length === 1) {
    return { ok: true, addr: places[0], how: 'content-repointed' };
  }
  return places.length === 0
    ? { ok: false, reason: 're-pointed bytes not in the target' }
    : {
        ok: false,
        reason: `re-pointed bytes occur ${places.length}${places.length > SAME_CONTENT_LIMIT ? '+' : ''} times`,
        ...(places.length > SAME_CONTENT_LIMIT ? {} : { same: places }),
      };
}

// ---------------------------------------------------------------------------------------------------------------
// structural maps

/** our type index → the target's type with the same signature (types are unique in a binaryen module). */
export function typeMap(ours: Module, target: Module): Map<number, number> {
  const byText = new Map(target.types.map((t, k) => [typeText(t), k]));
  const out = new Map<number, number>();
  ours.types.forEach((t, k) => {
    const j = byText.get(typeText(t));
    if (j !== undefined) {
      out.set(k, j);
    }
  });
  return out;
}

/** our global → the target's at the same index with the same type: emscripten's __stack_pointer and tempRet0. */
export function globalMap(ours: Module, target: Module): Map<number, number> {
  const out = new Map<number, number>();
  ours.globals.forEach((g, k) => {
    const t = target.globals[k];
    if (t !== undefined && t.type === g.type && t.mutable === g.mutable) {
      out.set(k, k);
    }
  });
  return out;
}

export interface SymbolMaps {
  ours: OurSide;
  ourImage: Buffer;
  target: TargetSide;
  types: Map<number, number>;
  globals: Map<number, number>;
  /** which of our constants are addresses, table slots or integers (splat-kinds.ts); without it every function of
   *  ours is kind-unverified and each ambiguous operand keeps every reading */
  kinds?: OperandKinds;
}

export function symbolMaps(ours: OurSide, target: TargetSide, kinds?: OperandKinds): SymbolMaps {
  return {
    ours,
    ourImage: memoryImage(ours.module).image,
    target,
    types: typeMap(ours.module, target.module),
    globals: globalMap(ours.module, target.module),
    ...(kinds === undefined ? {} : { kinds }),
  };
}

/** our function index → target function index, with how. */
export function resolveFunction(maps: SymbolMaps, index: number): Resolution & { index?: number } {
  const { ours, target } = maps;
  if (index < ours.module.importedFuncCount) {
    const imp = ours.module.imports.filter((i) => i.kind === 'func')[index];
    const real = ours.importNames[imp.name] ?? imp.name;
    const t = target.importByName.get(real);
    return t === undefined
      ? { ok: false, reason: `import ${real} is not imported by the target` }
      : { ok: true, addr: t, index: t, how: `import ${real}` };
  }
  const f = ours.fns.get(index);
  if (f === undefined) {
    return { ok: false, reason: `our function ${index} has no name` };
  }
  const t = target.symbols.fns.get(f.key);
  return t === undefined
    ? { ok: false, reason: `${f.key} has no target function` }
    : { ok: true, addr: t, index: t, how: `symbol ${f.key}` };
}

// ---------------------------------------------------------------------------------------------------------------
// target symbol generation

export interface Evidence {
  /** target function index → names (several when the target merged bodies we keep apart); `voters` lists the
   *  anchored pairs whose call or table votes named it */
  fnNames: Map<string, { index: number; evidence: string; voters?: number[] }>;
  dataNames: Map<string, { addr: number; size?: number; evidence: string; voters?: number[]; comment?: string }>;
  stats: Record<string, number>;
}

function sha12(b: Uint8Array | string): string {
  return createHash('sha256').update(b).digest('hex').slice(0, 12);
}

function preview(b: Buffer): string {
  const s = b.toString('latin1').slice(0, 40);
  return JSON.stringify(s) + (b.length > 40 ? '…' : '');
}

const ALIGNABLE_IMM = new Set(['func', 's32', 'memarg']);

/** Votes from two bodies that are the same instructions: our callee/data symbol ↔ the target's operand. */
interface VoteBox {
  fn: Map<number, Map<number, number>>;
  /** our function ↔ target function through a constant both sides keep in their tables */
  table: Map<number, Map<number, number>>;
  data: Map<string, Map<number, number>>;
  /** data symbol → the target functions whose operands voted for it */
  voters: Map<string, Set<number>>;
  /** our function → the target functions whose call or table operands voted for it */
  fnVoters: Map<number, Set<number>>;
}

function alignVotes(
  ours: OurSide,
  target: TargetSide,
  /** our address → the target address content identity gives it, when it does */
  addressOf: (vo: number) => number | null,
  oi: Instr[],
  ti: Instr[],
  votes: VoteBox,
  /** the target function `ti` is */
  voter: number,
): boolean {
  const targetEnd = target.dataEnd;
  const targetTable = target.module.tables[0]?.min ?? 0;
  const targetElem = target.elem;
  if (oi.length !== ti.length) {
    return false;
  }
  for (let k = 0; k < oi.length; k++) {
    if (oi[k].opcode !== ti[k].opcode || oi[k].imms.length !== ti[k].imms.length) {
      return false;
    }
  }
  const vote = <K>(into: Map<K, Map<number, number>>, key: K, v: number): void => {
    let m = into.get(key);
    if (m === undefined) {
      m = new Map();
      into.set(key, m);
    }
    m.set(v, (m.get(v) ?? 0) + 1);
  };
  // the same value on both sides is an integer (or an address both layouts share): it says nothing
  const dataVote = (vo: number, vt: number): void => {
    if (
      vo === vt ||
      vo < GLOBAL_BASE ||
      vo >= ours.dataEnd ||
      vt < GLOBAL_BASE ||
      (targetEnd !== null && vt >= targetEnd)
    ) {
      return;
    }
    const obj = dataObjectAt(ours.map.data, vo);
    if (obj !== null && !isAnonymous(obj)) {
      vote(votes.data, obj.key, vt - (vo - obj.addr));
      const who = votes.voters.get(obj.key) ?? new Set();
      votes.voters.set(obj.key, who.add(voter));
    }
  };
  // a constant inside both tables is a function pointer unless its address reading already explains it
  const constVote = (vo: number, vt: number): void => {
    if (vo !== vt && vo >= 1 && vo < ours.tableSize && vt >= 1 && vt < targetTable) {
      if (vo >= GLOBAL_BASE && addressOf(vo) === vt) {
        return;
      }
      const fo = ours.elem.get(vo);
      const ft = targetElem.get(vt);
      if (fo !== undefined && ft !== undefined) {
        vote(votes.table, fo, ft);
        votes.fnVoters.set(fo, (votes.fnVoters.get(fo) ?? new Set()).add(voter));
      }
      return;
    }
    dataVote(vo, vt);
  };
  for (let k = 0; k < oi.length; k++) {
    const o = oi[k];
    const t = ti[k];
    o.imms.forEach((im, j) => {
      const tm = t.imms[j];
      if (!ALIGNABLE_IMM.has(im.role)) {
        return;
      }
      if (im.kind === 'u32' && im.role === 'func' && tm.kind === 'u32') {
        if (im.value >= ours.module.importedFuncCount) {
          vote(votes.fn, im.value, tm.value);
          votes.fnVoters.set(im.value, (votes.fnVoters.get(im.value) ?? new Set()).add(voter));
        }
      } else if (im.kind === 's32' && tm.kind === 's32' && o.opcode === 0x41) {
        constVote(im.value, tm.value);
      } else if (im.kind === 'memarg' && tm.kind === 'memarg') {
        dataVote(im.offset, tm.offset);
      }
    });
  }
  return true;
}

interface GroupKeyed {
  fingerprint: string;
  addr: string;
}

function uniquePairs(
  tn: NormalizedFn[],
  on: NormalizedFn[],
  key: (n: GroupKeyed) => string,
  skipT: Set<number>,
  skipO: Set<number>,
): [number, number][] {
  const group = (ns: NormalizedFn[], skip: Set<number>): Map<string, number[]> => {
    const g = new Map<string, number[]>();
    for (const n of ns) {
      if (skip.has(n.index)) {
        continue;
      }
      const k = key(n);
      const l = g.get(k);
      if (l === undefined) {
        g.set(k, [n.index]);
      } else {
        l.push(n.index);
      }
    }
    return g;
  };
  const gt = group(tn, skipT);
  const go = group(on, skipO);
  const out: [number, number][] = [];
  for (const [k, ts] of gt) {
    const os = go.get(k);
    if (ts.length === 1 && os?.length === 1) {
      out.push([ts[0], os[0]]);
    }
  }
  return out;
}

/**
 * Pairs anchored by a std::source_location record: the record (resolved by what it records) is referenced by an
 * i32.const in exactly one target function and exactly one of ours. Whether the two bodies are the same
 * instructions is left to the caller (alignVotes).
 */
export function srclocAnchors(
  ours: OurSide,
  ourImage: Buffer,
  target: TargetSide,
  tn: readonly NormalizedFn[],
  on: readonly NormalizedFn[],
): [number, number][] {
  const records = new Set([...target.srcloc.values()].filter((a): a is number => typeof a === 'number'));
  const users = new Map<number, { t: Set<number>; o: Set<number> }>();
  const use = (record: number, side: 't' | 'o', fn: number): void => {
    let u = users.get(record);
    if (u === undefined) {
      u = { t: new Set(), o: new Set() };
      users.set(record, u);
    }
    u[side].add(fn);
  };
  const consts = (n: NormalizedFn): number[] =>
    n.instrs.flatMap((i) => (i.opcode === 0x41 && i.imms[0]?.kind === 's32' ? [i.imms[0].value] : []));
  for (const n of tn) {
    for (const v of consts(n)) {
      if (records.has(v)) {
        use(v, 't', n.index);
      }
    }
  }
  for (const n of on) {
    for (const v of consts(n)) {
      if (v < GLOBAL_BASE || v >= ours.dataEnd) {
        continue;
      }
      const obj = dataObjectAt(ours.map.data, v);
      if (obj === null || obj.addr !== v || !isSourceLocationRecord(obj)) {
        continue;
      }
      const key = srclocKey(ourImage, GLOBAL_BASE, obj.addr);
      const t = key === null ? undefined : target.srcloc.get(key);
      if (typeof t === 'number') {
        use(t, 'o', n.index);
      }
    }
  }
  const byOurs = new Map<number, Set<number>>();
  const byTarget = new Map<number, Set<number>>();
  for (const { t, o } of users.values()) {
    if (t.size === 1 && o.size === 1) {
      const [tf] = t;
      const [of] = o;
      byOurs.set(of, (byOurs.get(of) ?? new Set()).add(tf));
      byTarget.set(tf, (byTarget.get(tf) ?? new Set()).add(of));
    }
  }
  // a function anchored to two different functions of the other side anchors nothing
  return [...byOurs]
    .filter(([o, ts]) => ts.size === 1 && byTarget.get([...ts][0])?.size === 1 && byTarget.get([...ts][0])?.has(o))
    .map(([o, ts]) => [[...ts][0], o]);
}

export interface GenerateInputs {
  ours: OurSide;
  target: TargetSide;
  ledgerDir: string;
  /** body size at or below which a unique pair is not trusted to name anything */
  trivial?: number;
}

/** Every piece of evidence for the target's symbols (see the header for the kinds). */
export function gatherEvidence({ ours, target, ledgerDir, trivial = 8 }: GenerateInputs): Evidence {
  const stats: Record<string, number> = {};
  const tm = target.module;
  const om = ours.module;
  const tOpts = { importName: importResolver(target.importNames) };
  const oOpts = { importName: importResolver(ours.importNames) };
  const tn2 = normalizeModule(tm, { ...tOpts, depth: 2 });
  const on2 = normalizeModule(om, { ...oOpts, depth: 2 });
  const tn0 = normalizeModule(tm, { ...tOpts, depth: 0 });
  const on0 = normalizeModule(om, { ...oOpts, depth: 0 });
  const tFirst = tm.importedFuncCount;
  const oFirst = om.importedFuncCount;
  const sizeT = (i: number): number => tm.funcs[i - tFirst].size;

  // seeds: unique on both sides at depth 2, fingerprint first, then addr on what is left
  const seeds = uniquePairs(tn2, on2, (n) => n.fingerprint, new Set(), new Set()).filter(([t]) => sizeT(t) > trivial);
  const seedT = new Set(seeds.map(([t]) => t));
  const seedO = new Set(seeds.map(([, o]) => o));
  stats.pairFingerprint = seeds.length;
  const addrSeeds = uniquePairs(tn2, on2, (n) => n.addr, seedT, seedO).filter(([t]) => sizeT(t) > trivial);
  stats.pairAddr = addrSeeds.length;
  const pairs = [...seeds, ...addrSeeds];

  // propagate: align every pair and vote callees and table constants; a voted pair whose bodies agree at depth 0
  // (modulo callees and addresses) is aligned in turn
  const ourImage = memoryImage(om).image;
  const addressOf = (vo: number): number | null => {
    const r = resolveDataAddress(ours, ourImage, target, vo);
    return r.ok ? r.addr : null;
  };
  const tTypes = typeMap(om, tm);
  const sameSignature = (o: number, t: number): boolean =>
    t >= tFirst && tTypes.get(om.funcs[o - oFirst].typeIndex) === tm.funcs[t - tFirst].typeIndex;
  const votes: VoteBox = { fn: new Map(), table: new Map(), data: new Map(), voters: new Map(), fnVoters: new Map() };
  const aligned = new Set<string>();
  const pairOf = new Map<number, number>(pairs.map(([t, o]) => [o, t]));
  const viaTable = new Set<number>();
  /** the one target function every call and table vote for `o` names, or null */
  const agreed = (o: number): number | null => {
    const all = new Set([...(votes.fn.get(o)?.keys() ?? []), ...(votes.table.get(o)?.keys() ?? [])]);
    return all.size === 1 ? [...all][0] : null;
  };
  let queue = pairs.slice();
  while (queue.length > 0) {
    const next: [number, number][] = [];
    for (const [t, o] of queue) {
      const key = `${t}:${o}`;
      if (aligned.has(key)) {
        continue;
      }
      aligned.add(key);
      alignVotes(ours, target, addressOf, on0[o - oFirst].instrs, tn0[t - tFirst].instrs, votes, t);
    }
    for (const o of new Set([...votes.fn.keys(), ...votes.table.keys()])) {
      if (pairOf.has(o)) {
        continue;
      }
      // a call or a table slot reaches only a function of the caller's signature: votes for another one are a
      // coincidence of the aligned operands
      const t = agreed(o);
      if (t === null || t < tFirst || !sameSignature(o, t)) {
        continue;
      }
      pairOf.set(o, t);
      if (!votes.fn.has(o)) {
        viaTable.add(o);
      }
      if (tn0[t - tFirst].addr === on0[o - oFirst].addr) {
        next.push([t, o]);
      }
    }
    queue = next;
  }

  // anchors fill what the pairs left: a pair a source_location record names, kept when the bodies are the same
  // instructions, and the names its call and table votes agree on. Neither side may be named already, the
  // signatures must agree, and anchors cast no data votes.
  const anchorVotes: VoteBox = {
    fn: new Map(),
    table: new Map(),
    data: new Map(),
    voters: new Map(),
    fnVoters: new Map(),
  };
  const namedT = new Set(pairOf.values());
  const anchoredT = new Set<number>();
  const anchorEvidence = new Map<number, string>();
  stats.pairSrclocAnchor = 0;
  for (const [t, o] of srclocAnchors(ours, ourImage, target, tn0, on0)) {
    if (namedT.has(t) || pairOf.has(o) || sizeT(t) <= trivial || !sameSignature(o, t)) {
      continue;
    }
    if (alignVotes(ours, target, addressOf, on0[o - oFirst].instrs, tn0[t - tFirst].instrs, anchorVotes, t)) {
      pairOf.set(o, t);
      namedT.add(t);
      anchoredT.add(t);
      anchorEvidence.set(o, 'srcloc-anchor');
      stats.pairSrclocAnchor++;
    }
  }
  const anchorAgreed = (o: number): number | null => {
    const all = new Set([...(anchorVotes.fn.get(o)?.keys() ?? []), ...(anchorVotes.table.get(o)?.keys() ?? [])]);
    return all.size === 1 ? [...all][0] : null;
  };
  stats.fnAnchorVote = 0;
  for (const o of new Set([...anchorVotes.fn.keys(), ...anchorVotes.table.keys()])) {
    const t = anchorAgreed(o);
    if (pairOf.has(o) || votes.fn.has(o) || votes.table.has(o) || t === null || namedT.has(t) || !sameSignature(o, t)) {
      continue;
    }
    pairOf.set(o, t);
    namedT.add(t);
    anchorEvidence.set(o, anchorVotes.fn.has(o) ? 'call' : 'table');
    votes.fnVoters.set(o, anchorVotes.fnVoters.get(o) ?? new Set());
    stats.fnAnchorVote++;
  }

  const fnNames: Evidence['fnNames'] = new Map();
  const pairEvidence = new Map<number, string>();
  for (const [, o] of seeds) {
    pairEvidence.set(o, 'pair-fingerprint');
  }
  for (const [, o] of addrSeeds) {
    pairEvidence.set(o, 'pair-addr');
  }
  for (const [o, e] of anchorEvidence) {
    pairEvidence.set(o, e);
  }
  stats.callConflicts = 0;
  for (const [o, t] of pairOf) {
    if ((votes.fn.has(o) || votes.table.has(o)) && agreed(o) !== t) {
      stats.callConflicts++;
      continue;
    }
    const f = ours.fns.get(o);
    if (f === undefined || t < tFirst) {
      continue;
    }
    const evidence = pairEvidence.get(o) ?? (viaTable.has(o) ? 'table' : 'call');
    const voters = [...(votes.fnVoters.get(o) ?? [])].sort((a, b) => a - b);
    // a name only anchored pairs voted for lists them, so a match can tell when it is its own sole voter
    const anchored = voters.length > 0 && voters.every((v) => anchoredT.has(v));
    fnNames.set(f.key, { index: t, evidence, ...(anchored && voters.length <= VOTERS_LISTED ? { voters } : {}) });
  }
  stats.fnTable = [...fnNames.values()].filter((v) => v.evidence === 'table').length;
  stats.fnCall = [...fnNames.values()].filter((v) => v.evidence === 'call').length;

  // exports: the glue's real names
  const glue = glueNamesFor(target.path);
  stats.fnExport = 0;
  for (const e of tm.exports) {
    if (e.kind !== 'func' || e.index < tFirst) {
      continue;
    }
    const real = glue?.exports[e.name];
    if (real === undefined) {
      continue;
    }
    const prev = fnNames.get(real);
    if (prev !== undefined && prev.index !== e.index) {
      stats.exportConflicts = (stats.exportConflicts ?? 0) + 1;
      fnNames.delete(real);
      continue;
    }
    fnNames.set(real, { index: e.index, evidence: prev ? `${prev.evidence}+export` : 'export' });
    stats.fnExport++;
  }

  // data
  const dataNames: Evidence['dataNames'] = new Map();
  const ourByKey = new Map(ours.map.data.map((d) => [d.key, d]));
  stats.dataVoteConflicts = 0;
  for (const [key, vs] of votes.data) {
    if (vs.size !== 1) {
      stats.dataVoteConflicts++;
      continue;
    }
    const [[addr, n]] = [...vs];
    const obj = ourByKey.get(key);
    const voters = [...(votes.voters.get(key) ?? [])].sort((a, b) => a - b);
    dataNames.set(key, {
      addr,
      ...(obj ? { size: obj.size } : {}),
      evidence: `vote:${n}`,
      ...(voters.length <= VOTERS_LISTED ? { voters } : {}),
    });
  }
  stats.dataVote = dataNames.size;

  // rtti
  const { image, imageBase } = target;
  const classesPath = join(ledgerDir, 'classes.json');
  const vtablesPath = join(ledgerDir, 'vtables.json');
  const rtti = new Map<string, { addr: number; evidence: string } | 'conflict'>();
  const put = (name: string, addr: number): void => {
    const prev = rtti.get(name);
    rtti.set(
      name,
      prev === undefined ? { addr, evidence: 'rtti' } : prev !== 'conflict' && prev.addr === addr ? prev : 'conflict',
    );
  };
  if (existsSync(classesPath)) {
    const classes = (
      JSON.parse(readFileSync(classesPath, 'utf8')) as { classes: { typeinfo: number; mangled: string }[] }
    ).classes;
    const mangledByTi = new Map<number, string>();
    for (const c of classes) {
      put(`_ZTI${c.mangled}`, c.typeinfo);
      mangledByTi.set(c.typeinfo, c.mangled);
      const nameAddr = u32At(image, imageBase, c.typeinfo + 4);
      const s = nameAddr === null ? null : cStringAt(image, imageBase, nameAddr);
      if (nameAddr !== null && s !== null && s.toString('latin1') === c.mangled) {
        put(`_ZTS${c.mangled}`, nameAddr);
      }
    }
    if (existsSync(vtablesPath)) {
      const vtables = JSON.parse(readFileSync(vtablesPath, 'utf8')) as {
        address: number;
        offset_to_top: number;
        typeinfo: number;
      }[];
      // `address` is the primary table's offset-to-top word; a class with virtual bases has their offsets ahead
      // of it, as many as our own vtable of the class has
      for (const v of vtables) {
        const m = mangledByTi.get(v.typeinfo);
        if (v.offset_to_top === 0 && m !== undefined) {
          put(`_ZTV${m}`, v.address + 4 - vtableTypeinfoOffset(ourByKey, ourImage, `_ZTV${m}`));
        }
      }
    }
  }
  stats.dataRtti = 0;
  stats.rttiConflicts = 0;
  for (const [name, r] of rtti) {
    if (r === 'conflict') {
      stats.rttiConflicts++;
      continue;
    }
    const prev = dataNames.get(name);
    if (prev !== undefined && prev.addr !== r.addr) {
      stats.rttiConflicts++;
      dataNames.delete(name);
      continue;
    }
    const obj = ourByKey.get(name);
    dataNames.set(name, {
      addr: r.addr,
      ...(obj ? { size: obj.size } : {}),
      evidence: prev ? `${prev.evidence}+rtti` : 'rtti',
      ...(prev?.voters ? { voters: prev.voters } : {}),
    });
    stats.dataRtti++;
  }

  // the functions in the slots of a vtable the RTTI names on both sides: the target's slot holds the function ours
  // does. Named only when the slot's run has the target's length (below), every slot that holds our function agrees
  // on one target function, no other function of ours votes for it, neither side is named yet, and the signatures
  // agree (target 11611 is wrapexcept<bad_month>::rethrow by its vtable's slot 3)
  stats.fnVtable = 0;
  {
    const namedIndex = new Set([...fnNames.values()].map((v) => v.index));
    const slotVotes = new Map<number, Set<number>>();
    for (const [name, d] of dataNames) {
      const obj = ourByKey.get(name);
      if (!name.startsWith('_ZTV') || !d.evidence.endsWith('rtti') || obj === undefined) {
        continue;
      }
      // a vtable is one table per base with virtual functions, each a run of slots after its offset-to-top and
      // typeinfo words; a run pairs with the target's only when the target has a run of the same length at the same
      // offset (a base can declare other members in the target's version, which shifts every later slot)
      const ourSlot = (q: number): boolean => {
        const w = u32At(ourImage, GLOBAL_BASE, obj.addr + q);
        return w !== null && ours.elem.has(w);
      };
      const targetSlot = (q: number): boolean => {
        const w = u32At(image, imageBase, d.addr + q);
        return w !== null && target.elem.has(w);
      };
      for (let q = 0; q + 4 <= obj.size; ) {
        if (!ourSlot(q)) {
          q += 4;
          continue;
        }
        let end = q;
        while (end + 4 <= obj.size && ourSlot(end)) {
          end += 4;
        }
        let same = (q === 0 || !targetSlot(q - 4)) && !targetSlot(end);
        for (let k = q; same && k < end; k += 4) {
          same = targetSlot(k);
        }
        for (let k = q; same && k < end; k += 4) {
          const o = ours.elem.get(u32At(ourImage, GLOBAL_BASE, obj.addr + k)!)!;
          (slotVotes.get(o) ?? slotVotes.set(o, new Set()).get(o)!).add(target.elem.get(u32At(image, imageBase, d.addr + k)!)!);
        }
        q = end;
      }
    }
    // a target function several of ours vote for is a body binaryen merged: which name it keeps is not evidence
    const voters = new Map<number, number>();
    for (const ts of slotVotes.values()) {
      for (const t of ts) {
        voters.set(t, (voters.get(t) ?? 0) + 1);
      }
    }
    for (const [o, ts] of slotVotes) {
      const f = ours.fns.get(o);
      const [t] = ts;
      if (ts.size !== 1 || voters.get(t) !== 1 || f === undefined || fnNames.has(f.key) || namedIndex.has(t) ||
          !sameSignature(o, t)) {
        continue;
      }
      fnNames.set(f.key, { index: t, evidence: 'vtable' });
      namedIndex.add(t);
      stats.fnVtable++;
    }
  }

  // our named read-only objects found by content
  stats.dataContent = 0;
  for (const obj of ours.map.data) {
    if (isAnonymous(obj) || !isReadOnly(obj) || dataNames.has(obj.key)) {
      continue;
    }
    const r = resolveBytes(ourImage, target, obj);
    if (r.ok) {
      dataNames.set(obj.key, { addr: r.addr, size: obj.size, evidence: 'content' });
      stats.dataContent++;
    }
  }

  // strings and source locations of the ledger, named by content
  const stringsPath = join(ledgerDir, 'strings.tsv');
  stats.dataString = 0;
  stats.stringDuplicates = 0;
  if (existsSync(stringsPath)) {
    const byName = new Map<string, { addr: number; size: number; text: Buffer } | 'dup'>();
    for (const line of readFileSync(stringsPath, 'utf8').split('\n').slice(1)) {
      const addr = Number(line.split('\t')[0]);
      if (line === '' || !Number.isFinite(addr)) {
        continue;
      }
      const s = cStringAt(image, imageBase, addr);
      if (s === null) {
        continue;
      }
      const name = `str.${sha12(s)}`;
      byName.set(name, byName.has(name) ? 'dup' : { addr, size: s.length + 1, text: Buffer.from(s) });
    }
    for (const [name, v] of byName) {
      if (v === 'dup') {
        stats.stringDuplicates++;
        continue;
      }
      dataNames.set(name, { addr: v.addr, size: v.size, evidence: 'string', comment: preview(v.text) });
      stats.dataString++;
    }
  }
  stats.dataSrcloc = 0;
  for (const [key, addr] of target.srcloc) {
    if (addr === 'ambiguous') {
      continue;
    }
    const [file, , line, col] = key.split('\u0000');
    dataNames.set(`srcloc.${sha12(key)}`, {
      addr,
      size: 16,
      evidence: 'srcloc',
      comment: `${file.split('/').pop()}:${line}:${col}`,
    });
    stats.dataSrcloc++;
  }
  Object.assign(stats, refuteDataNames(ours, ourImage, target, fnNames, dataNames));
  stats.fnNamed = new Set([...fnNames.values()].map((v) => v.index)).size;
  return { fnNames, dataNames, stats };
}

const isVote = (evidence: string): boolean => /^vote:\d+$/.test(evidence);

/**
 * Drop the data names the target's own bytes contradict: an RTTI symbol whose bytes describe another class, a
 * voted object whose bytes (pointers re-pointed through the other names) differ from ours, and two names at one
 * target address that cannot both be right: two objects of ours with different bytes, or an object of ours where
 * the target's RTTI names another (a vote yields to independent evidence; two votes both go). Overlaps that do not
 * share a start are left alone: our object can be bigger than the target's (a class with more virtual functions
 * in our build).
 */
export function refuteDataNames(
  ours: OurSide,
  ourImage: Buffer,
  target: TargetSide,
  fnNames: Evidence['fnNames'],
  dataNames: Evidence['dataNames'],
): Record<string, number> {
  const ourByKey = new Map(ours.map.data.map((d) => [d.key, d]));
  const stats = { rttiRefuted: 0, voteBytesRefuted: 0, placeConflicts: 0, placeRefuted: 0 };
  for (const [name, v] of dataNames) {
    const cls = rttiClassOf(name);
    const q = vtableTypeinfoOffset(ourByKey, ourImage, name);
    if (cls !== null && rttiClassAt(target.image, target.imageBase, name, v.addr, q) !== cls) {
      dataNames.delete(name);
      stats.rttiRefuted++;
    }
  }
  const symbols: TargetSymbols = {
    ...emptySymbols(),
    fns: new Map([...fnNames].map(([n, v]) => [n, v.index])),
    imports: target.importByName,
    data: new Map([...dataNames].map(([n, v]) => [n, v.addr])),
  };
  const maps: SymbolMaps = { ours, ourImage, target: { ...target, symbols }, types: new Map(), globals: new Map() };
  for (const [name, v] of dataNames) {
    const obj = ourByKey.get(name);
    if (obj !== undefined && isVote(v.evidence) && compareObject(maps, obj, v.addr).verdict === 'differs') {
      dataNames.delete(name);
      stats.voteBytesRefuted++;
    }
  }
  const placed = [...dataNames]
    .map(([name, v]) => ({ name, v, obj: ourByKey.get(name) }))
    .filter((p) => p.obj !== undefined || rttiClassOf(p.name) !== null)
    .sort((a, b) => a.v.addr - b.v.addr);
  const bytesOf = (o: DataObject): Buffer => ourImage.subarray(o.addr - GLOBAL_BASE, o.addr - GLOBAL_BASE + o.size);
  const refuted = new Set<string>();
  for (let i = 0; i < placed.length; i++) {
    const a = placed[i];
    for (let j = i + 1; j < placed.length && placed[j].v.addr === a.v.addr; j++) {
      const b = placed[j];
      // one object of ours under two names or two with the same bytes; two RTTI names of the target alone say
      // nothing about our objects
      const compatible =
        a.obj !== undefined && b.obj !== undefined
          ? a.obj.addr === b.obj.addr || bytesOf(a.obj).equals(bytesOf(b.obj))
          : a.obj === undefined && b.obj === undefined;
      if (compatible) {
        continue;
      }
      stats.placeConflicts++;
      for (const p of [a, b]) {
        if (isVote(p.v.evidence)) {
          refuted.add(p.name);
        }
      }
    }
  }
  for (const name of refuted) {
    dataNames.delete(name);
  }
  stats.placeRefuted = refuted.size;
  return stats;
}

/** The symbol file's generated lines: imports (glue names), functions, data; sorted by place. */
export function generatedEntries(target: TargetSide, ev: Evidence): SymbolEntry[] {
  const out: SymbolEntry[] = [];
  for (const [name, index] of target.importByName) {
    out.push({ name, at: { kind: 'import', index }, evidence: 'glue' });
  }
  const fns = [...ev.fnNames].sort((a, b) => a[1].index - b[1].index || (a[0] < b[0] ? -1 : 1));
  for (const [name, v] of fns) {
    const size = target.module.funcs[v.index - target.module.importedFuncCount]?.size;
    out.push({
      name,
      at: { kind: 'func', index: v.index },
      ...(size !== undefined ? { size } : {}),
      evidence: v.evidence,
      ...(v.voters !== undefined ? { voters: v.voters } : {}),
    });
  }
  const data = [...ev.dataNames].sort((a, b) => a[1].addr - b[1].addr || (a[0] < b[0] ? -1 : 1));
  for (const [name, v] of data) {
    out.push({
      name,
      at: { kind: 'data', addr: v.addr },
      ...(v.size !== undefined ? { size: v.size } : {}),
      evidence: v.evidence,
      ...(v.voters !== undefined ? { voters: v.voters } : {}),
      ...(v.comment !== undefined ? { comment: v.comment } : {}),
    });
  }
  return out;
}
