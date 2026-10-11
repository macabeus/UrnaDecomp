// The target module as the page reads it: parsed from its bytes, named from its glue and the
// distilled ledger, and every per-function view (WAT, folded readout, lifted C++, facts) computed on
// demand with the toolkit's own modules. Nothing here touches the DOM, so the tests run it in node.
import { type Instr, disassemble, instrText } from '@asmlift/wasm/disasm';
import { foldFunction } from '@asmlift/wasm/fold';
import { exportNamesFromGlue, importNamesFromGlue } from '@asmlift/wasm/imports';
import { type LiftFlags, type Lifted, type TreeEvidence, liftFlags, liftFunction, liftNames } from '@asmlift/wasm/lift';
import { type Func, type Module, funcAt, importedFunc, parseModule, typeText } from '@asmlift/wasm/parse';
import { type LedgerString, referencedStrings } from '@asmlift/wasm/strings';
import type { PairKind } from '@asmlift/wasm-benchmark/schema';

import type { LedgerData } from './ledger-data';

export interface Target {
  m: Module;
  data: LedgerData;
  /** `liftNames` over the glue's import and export names and the ledger's: what the lifter calls each function */
  names: Map<number, string>;
  mangled: Map<string, string>;
  /** function index → the real names it is exported under */
  exports: Map<number, string[]>;
  strings: LedgerString[];
  /** the tree evidence `liftFlags` reads, as `liftInputs` gives it to the CLI */
  trees: TreeEvidence;
}

export function openTarget(wasm: Uint8Array, glue: string, data: LedgerData): Target {
  const m = parseModule(wasm);
  const imports = importNamesFromGlue(glue);
  const exportNames = exportNamesFromGlue(glue);
  const mangled = new Map(Object.entries(data.mangled));
  const ledger = new Map(Object.entries(data.names).map(([k, v]) => [Number(k), v]));
  const names = liftNames(m, { imports, exports: exportNames, ledger, mangled });
  const exports = new Map<number, string[]>();
  for (const e of m.exports) {
    if (e.kind !== 'func') {
      continue;
    }
    const real = exportNames[e.name] ?? e.name;
    exports.set(e.index, [...(exports.get(e.index) ?? []), real]);
  }
  const strings = data.strings.map(([addr, len, enc, text]) => ({ addr, len, enc, text }));
  const trees: TreeEvidence = {
    records: new Map(Object.entries(data.treeRecords).map(([k, v]) => [Number(k), v.map((f) => data.treeFiles[f])])),
    stubs: new Map(Object.entries(data.treeStubs).map(([k, v]) => [Number(k), data.treeFiles[v]])),
  };
  return { m, data, names, mangled, exports, strings, trees };
}

export function isDefined(t: Target, index: number): boolean {
  return funcAt(t.m, index) !== null;
}

export function lastIndex(t: Target): number {
  return t.m.importedFuncCount + t.m.funcs.length - 1;
}

/** The name the page shows: the lifter's name, else `f<index>`. */
export function displayName(t: Target, index: number): string {
  return t.names.get(index) ?? `f${index}`;
}

export interface LedgerFacts {
  component: string;
  evidence: string;
  owner: string;
  best: { kind: PairKind; label: string; index: number; score: number } | null;
  /** the name of a shape pairing, which names a function of the same shape, not necessarily this one */
  shapeName: string | null;
}

export function ledgerFacts(t: Target, index: number): LedgerFacts {
  const k = index - t.data.first;
  const b = t.data.best[index];
  return {
    component: t.data.components[t.data.component[k]] ?? '-',
    evidence: t.data.evidence[t.data.evidenceOf[k]] ?? '-',
    owner: t.data.owners[t.data.owner[k]] ?? '-',
    best: b === undefined ? null : { kind: b[0], label: t.data.labels[b[1]], index: b[2], score: b[3] },
    shapeName: t.data.shapeNames[index] ?? null,
  };
}

/** The flags `pnpm wasm lift` writes into a function's header when no benchmark row gives them: its tree's,
 *  else its component's (`liftFlags`), with why. */
export function defaultLiftFlags(t: Target, index: number): LiftFlags {
  const component = ledgerFacts(t, index).component;
  return liftFlags(index, component === '-' ? undefined : component, t.trees);
}

/** A row's flags as a string, or `defaultLiftFlags`' choice, whose reason the header carries too. */
export function lift(t: Target, index: number, flags: string | LiftFlags): Lifted {
  const f = typeof flags === 'string' ? { flags } : { flags: flags.flags, flagsWhy: flags.why };
  return liftFunction(t.m, index, { names: t.names, mangled: t.mangled, ...f });
}

function defined(t: Target, index: number): Func {
  const fn = funcAt(t.m, index);
  if (fn === null) {
    throw new Error(`function ${index} is an import: it has no body`);
  }
  return fn;
}

export function instructions(t: Target, index: number): Instr[] {
  return disassemble(defined(t, index));
}

export function foldText(t: Target, index: number, instrs = instructions(t, index)): string {
  return foldFunction(t.m, defined(t, index), instrs, { funcName: (i) => displayName(t, i) });
}

export function signatureText(fn: Func): string {
  return typeText({ params: fn.params, results: fn.results });
}

/** `(local i32 i32 i64)`: the declared locals in order. */
export function localsText(fn: Func): string {
  return fn.locals.length === 0 ? '' : `(local ${fn.locals.join(' ')})`;
}

const OPENS = new Set(['block', 'loop', 'if', 'try']);
const MIDDLES = new Set(['else', 'catch', 'catch_all']);
const CALL = 0x10;
const CALL_INDIRECT = 0x11;
const RETURN_CALL = 0x12;

export interface Wat {
  text: string;
  /** per line, the body offset of its instruction; null on the header lines */
  offsets: (number | null)[];
}

/** The body as linear WAT: one instruction per line, nested blocks indented, a call annotated with
 *  its callee's name and an indirect call with its type. The final `end` closes the `(func`. */
export function watText(t: Target, index: number, instrs = instructions(t, index)): Wat {
  const fn = defined(t, index);
  const lines: string[] = [];
  const offsets: (number | null)[] = [];
  const push = (line: string, offset: number | null): void => {
    lines.push(line);
    offsets.push(offset);
  };
  const params = fn.params.length === 0 ? '' : ` (param ${fn.params.join(' ')})`;
  const results = fn.results.length === 0 ? '' : ` (result ${fn.results.join(' ')})`;
  push(`(func $${index} (type ${fn.typeIndex})${params}${results}`, null);
  if (fn.locals.length > 0) {
    push(`  ${localsText(fn)}`, null);
  }
  let depth = 1;
  instrs.forEach((i, k) => {
    const last = k === instrs.length - 1;
    if (last && i.name === 'end') {
      push(')', i.offset);
      return;
    }
    if (i.name === 'end' || i.name === 'delegate') {
      depth = Math.max(1, depth - 1);
    }
    const indent = '  '.repeat(MIDDLES.has(i.name) ? Math.max(1, depth - 1) : depth);
    let text = instrText(i, t.m);
    if (i.opcode === CALL || i.opcode === RETURN_CALL) {
      const callee = (i.imms[0] as { value: number }).value;
      text += `  ;; ${calleeLabel(t, callee)}`;
    } else if (i.opcode === CALL_INDIRECT) {
      const type = t.m.types[(i.imms[0] as { value: number }).value];
      if (type !== undefined) {
        text += `  ;; ${typeText(type)}`;
      }
    }
    push(`${indent}${text}`, i.offset);
    if (OPENS.has(i.name)) {
      depth++;
    }
  });
  return { text: lines.join('\n'), offsets };
}

function calleeLabel(t: Target, index: number): string {
  const imp = importedFunc(t.m, index);
  const name = displayName(t, index);
  return imp === null ? name : `import ${imp.module}.${imp.name} = ${name}`;
}

export interface Callee {
  index: number;
  sites: number;
  name: string;
  import: boolean;
}

/** Each distinct direct callee (`call`, `return_call`) with its number of call sites, in first-call order. */
export function calleesOf(t: Target, instrs: readonly Instr[]): Callee[] {
  const sites = new Map<number, number>();
  for (const i of instrs) {
    if (i.opcode === CALL || i.opcode === RETURN_CALL) {
      const callee = (i.imms[0] as { value: number }).value;
      sites.set(callee, (sites.get(callee) ?? 0) + 1);
    }
  }
  return [...sites].map(([index, n]) => ({
    index,
    sites: n,
    name: displayName(t, index),
    import: importedFunc(t.m, index) !== null,
  }));
}

/** callee → its distinct direct callers in index order, over every defined function. */
export function directCallers(t: Target): Map<number, number[]> {
  const out = new Map<number, number[]>();
  for (const fn of t.m.funcs) {
    const seen = new Set<number>();
    for (const i of disassemble(fn)) {
      if (i.opcode !== CALL && i.opcode !== RETURN_CALL) {
        continue;
      }
      const callee = (i.imms[0] as { value: number }).value;
      if (seen.has(callee)) {
        continue;
      }
      seen.add(callee);
      const list = out.get(callee);
      if (list === undefined) {
        out.set(callee, [fn.index]);
      } else {
        list.push(fn.index);
      }
    }
  }
  return out;
}

export function stringsOf(t: Target, instrs: readonly Instr[]) {
  return referencedStrings(instrs, t.strings);
}

export interface SearchHit {
  index: number;
  name: string;
}

/** Defined functions by index (the exact one first, then those whose index starts with the digits)
 *  or by a case-insensitive substring of their name, exact and prefix matches first. */
export function searchFunctions(t: Target, query: string, limit = 40): SearchHit[] {
  const q = query.trim();
  if (q === '') {
    return [];
  }
  const first = t.m.importedFuncCount;
  const last = lastIndex(t);
  if (/^\d+$/.test(q)) {
    const out: SearchHit[] = [];
    const exact = Number(q);
    if (exact >= first && exact <= last) {
      out.push({ index: exact, name: displayName(t, exact) });
    }
    for (let i = first; i <= last && out.length < limit; i++) {
      if (i !== exact && String(i).startsWith(q)) {
        out.push({ index: i, name: displayName(t, i) });
      }
    }
    return out;
  }
  const needle = q.toLowerCase();
  const scored: { hit: SearchHit; rank: number }[] = [];
  for (const [index, name] of t.names) {
    if (index < first) {
      continue;
    }
    const hay = name.toLowerCase();
    const at = hay.indexOf(needle);
    if (at < 0) {
      continue;
    }
    const rank = hay === needle ? 0 : at === 0 ? 1 : 2;
    scored.push({ hit: { index, name }, rank });
  }
  scored.sort((a, b) => a.rank - b.rank || a.hit.name.length - b.hit.name.length || a.hit.index - b.hit.index);
  return scored.slice(0, limit).map((s) => s.hit);
}
