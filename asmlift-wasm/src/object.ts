// Pull one function out of a module a candidate compile produced, and normalise it so it can be
// scored against a target function. Two kinds of module arrive here:
//
//   `module`  a LINKED module (`emcc … -o x.wasm`): the function is found by export name and
//             normalised exactly like the target's — same `fn:<fingerprint>` callee symbols, same
//             data symbols — so a function whose callees also match the target's scores 0, and
//             `byteExact` is meaningful. This is the default, and the only path that can reach 0
//             against an optimised target: emcc runs binaryen's wasm-opt over the LINKED module,
//             so a `-c` object is pre-wasm-opt LLVM output with different instruction streams.
//   `object`  a RELOCATABLE object (`emcc -c`, `clang --target=wasm32 -c`): the function is found by
//             symbol name in the `linking` section, and every immediate a `reloc.CODE` entry points
//             at is replaced by its relocation's symbol (`sym:<name>`, `data:<name>+addend`,
//             `tbl:<name>`), which is exact where the linked path's data-range heuristic is not.
//             Its callee symbols are names, which the target cannot have, so an object is scored
//             with `depth 0` on both sides (callees compare by position only).
import { type Instr, disassemble, immText } from './disasm';
import { type NormalizeOptions, type NormalizedFn, normalizeModule, signature } from './normalize';
import { type Func, type Module, type Reloc, funcAt } from './parse';

export type Extract =
  { kind: 'export'; name: string } | { kind: 'symbol'; name: string } | { kind: 'index'; index: number };

export interface Extracted {
  fn: Func;
  rows: string[];
  signature: string;
  instrs: Instr[];
  normalized: NormalizedFn | null;
}

export interface FindOptions {
  /** the minified export of a real name (imports.ts `exportResolver`), for -O2+ links */
  exportName?: (realName: string) => string | undefined;
  /** the module's normalisation, when the caller already has it (normalising the target takes ~2 s) */
  norm?: NormalizedFn[];
}

export function findFunction(m: Module, how: Extract, opts: FindOptions = {}): Func {
  switch (how.kind) {
    case 'index': {
      const fn = funcAt(m, how.index);
      if (fn === null) {
        throw new Error(
          `no defined function at index ${how.index} (${m.importedFuncCount} imports, ${m.funcs.length} defined)`,
        );
      }
      return fn;
    }
    case 'export': {
      const minified = opts.exportName?.(how.name);
      const e = m.exports.find((x) => x.kind === 'func' && (x.name === how.name || x.name === minified));
      if (e === undefined) {
        const names = m.exports.filter((x) => x.kind === 'func').map((x) => x.name);
        throw new Error(
          `no exported function '${how.name}'; exports: ${names.slice(0, 30).join(' ')}${names.length > 30 ? ' …' : ''}`,
        );
      }
      const fn = funcAt(m, e.index);
      if (fn === null) {
        throw new Error(`export '${how.name}' re-exports import ${e.index}`);
      }
      return fn;
    }
    case 'symbol': {
      const s = m.symbols.find((x) => x.kind === 'function' && x.defined && x.name === how.name);
      if (s === undefined) {
        const names = m.symbols.filter((x) => x.kind === 'function' && x.defined).map((x) => x.name);
        throw new Error(
          `no defined function symbol '${how.name}'; defined: ${names.slice(0, 30).join(' ')}${names.length > 30 ? ' …' : ''}`,
        );
      }
      const fn = funcAt(m, s.elementIndex!);
      if (fn === null) {
        throw new Error(`symbol '${how.name}' names function ${s.elementIndex}, which is not defined here`);
      }
      return fn;
    }
  }
}

/** Normalise one function of a relocatable object with its relocations applied as symbols. */
export function normalizeObjectFunction(m: Module, fn: Func): { rows: string[]; instrs: Instr[] } {
  if (m.codeSectionStart === null) {
    throw new Error('object has no code section');
  }
  const codeIndex = m.sections.findIndex((s) => s.id === 10);
  const relocSection = m.relocs.find((r) => r.sectionIndex === codeIndex);
  const relocAt = new Map<number, Reloc>();
  for (const r of relocSection?.relocs ?? []) {
    relocAt.set(r.offset, r);
  }
  const instrs = disassemble(fn);
  const base = fn.bodyOffset - m.codeSectionStart;
  const symName = (r: Reloc): string => {
    const s = m.symbols[r.symbolIndex];
    return s?.name ?? `sym#${r.symbolIndex}`;
  };
  const rows = instrs.map((i) => {
    const parts: string[] = [];
    for (const imm of i.imms) {
      const r = relocAt.get(base + imm.at) ?? (imm.kind === 'memarg' ? relocAt.get(base + imm.offsetAt) : undefined);
      if (r === undefined) {
        const t = immText(imm, m);
        if (t !== '') {
          parts.push(t);
        }
        continue;
      }
      switch (r.typeName) {
        case 'R_WASM_FUNCTION_INDEX_LEB':
        case 'R_WASM_FUNCTION_INDEX_I32':
          parts.push(`sym:${symName(r)}`);
          break;
        case 'R_WASM_TABLE_INDEX_SLEB':
        case 'R_WASM_TABLE_INDEX_I32':
        case 'R_WASM_TABLE_INDEX_REL_SLEB':
          parts.push(`tbl:${symName(r)}`);
          break;
        case 'R_WASM_MEMORY_ADDR_LEB':
        case 'R_WASM_MEMORY_ADDR_SLEB':
        case 'R_WASM_MEMORY_ADDR_I32':
        case 'R_WASM_MEMORY_ADDR_REL_SLEB':
        case 'R_WASM_MEMORY_ADDR_TLS_SLEB': {
          const sym = `data:${symName(r)}${r.addend === 0 ? '' : `+${r.addend}`}`;
          // the address folded into a load/store offset: keep the align, symbolise the offset
          parts.push(imm.kind === 'memarg' ? `align=${imm.align} offset=${sym}` : sym);
          break;
        }
        case 'R_WASM_TYPE_INDEX_LEB': {
          const t = m.types[(imm as { value: number }).value];
          parts.push(t === undefined ? 'type:?' : `type:(${t.params.join(' ')})->(${t.results.join(' ')})`);
          break;
        }
        case 'R_WASM_GLOBAL_INDEX_LEB':
          parts.push(`global:${symName(r)}`);
          break;
        case 'R_WASM_TAG_INDEX_LEB':
          parts.push(`tag:${symName(r)}`);
          break;
        default:
          parts.push(`${r.typeName}:${symName(r)}`);
      }
    }
    return parts.length === 0 ? i.name : `${i.name} ${parts.join(' ')}`;
  });
  return { rows, instrs };
}

export function isRelocatableObject(m: Module): boolean {
  return m.customSections.includes('linking');
}

/** Extract and normalise `how` from `m`. A relocatable object takes the reloc path; a linked module
 *  is normalised whole (so callee fingerprints exist) and the one function is picked out. */
export function extractFunction(m: Module, how: Extract, opts: NormalizeOptions & FindOptions = {}): Extracted {
  const fn = findFunction(m, how, opts);
  if (isRelocatableObject(m)) {
    const { rows, instrs } = normalizeObjectFunction(m, fn);
    return { fn, rows, signature: signature(fn), instrs, normalized: null };
  }
  const norm = opts.norm ?? normalizeModule(m, opts);
  const n = norm[fn.index - m.importedFuncCount];
  return { fn, rows: n.rows, signature: signature(fn), instrs: n.instrs, normalized: n };
}
