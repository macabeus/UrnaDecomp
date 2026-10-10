// A row's features, read off the target function's own instructions.
import { type ImportNames, type Instr, type Module, disassemble, funcAt, importedFunc } from '@asmlift/wasm';

import { FEATURES, type Feature } from './schema';

/** emscripten's shadow-stack pointer is global 0 of the target. */
const STACK_POINTER = 0;

/** The real name of an imported callee: the glue's name for a minified field, else the field. */
function importName(m: Module, index: number, names: ImportNames | null): string | null {
  const imp = importedFunc(m, index);
  if (imp === null) {
    return null;
  }
  return names?.[imp.name] ?? imp.name;
}

const isSp = (i: Instr): boolean => (i.imms[0] as { value: number } | undefined)?.value === STACK_POINTER;

/** Frames the body reserves: `global.get sp; i32.const N; i32.sub` (a leaf keeps the result in a
 *  local and never writes it back), and stack-pointer writes of a non-constant subtraction (alloca). */
function frameReservations(ins: readonly Instr[]): { constant: number; dynamic: number } {
  let constant = 0;
  let dynamic = 0;
  for (let i = 0; i + 2 < ins.length; i++) {
    if (ins[i].name === 'global.get' && isSp(ins[i]) && ins[i + 1].name === 'i32.const' && ins[i + 2].name === 'i32.sub') {
      constant++;
    }
  }
  for (let i = 0; i < ins.length; i++) {
    if (ins[i].name !== 'global.set' || !isSp(ins[i])) {
      continue;
    }
    const window = ins.slice(Math.max(0, i - 4), i).map((x) => x.name);
    const sub = window.lastIndexOf('i32.sub');
    if (sub > 0 && window[sub - 1] !== 'i32.const') {
      dynamic++;
    }
  }
  return { constant, dynamic };
}

export function featuresOf(m: Module, index: number, imports: ImportNames | null): Feature[] {
  const fn = funcAt(m, index);
  if (fn === null) {
    throw new Error(`${index} is not a defined function`);
  }
  const ins = disassemble(fn);
  const out = new Set<Feature>();
  let calls = 0;
  for (const i of ins) {
    const n = i.name;
    if (n === 'loop') {
      out.add('loop');
    } else if (n === 'br_table') {
      out.add('switch');
    } else if (n === 'call_indirect' || n === 'return_call_indirect') {
      out.add('call_indirect');
      calls++;
    } else if (n === 'call' || n === 'return_call') {
      calls++;
      const callee = importName(m, (i.imms[0] as { value: number }).value, imports);
      if (callee?.startsWith('invoke_')) {
        out.add('invoke');
      } else if (callee === '__cxa_throw' || callee === '__resumeException') {
        out.add('throw');
      }
    } else if (n === 'select' || n === 'select_t') {
      out.add('select');
    } else if (n.startsWith('memory.copy') || n.startsWith('memory.fill')) {
      out.add('bulk-memory');
    }
    if (n.startsWith('i64.') || n === 'i32.wrap_i64' || n.startsWith('f64.convert_i64') || n.startsWith('f32.convert_i64')) {
      out.add('i64');
    }
    if (/^f(32|64)\./.test(n) || n.includes('_f32') || n.includes('_f64')) {
      out.add('float');
    }
  }
  if (fn.params.includes('i64') || fn.results.includes('i64') || fn.locals.includes('i64')) {
    out.add('i64');
  }
  if ([...fn.params, ...fn.results, ...fn.locals].some((t) => t === 'f32' || t === 'f64')) {
    out.add('float');
  }
  const frames = frameReservations(ins);
  if (frames.constant > 0) {
    out.add('frame');
  }
  if (frames.constant > 1) {
    out.add('inlined-frame');
  }
  if (frames.dynamic > 0) {
    out.add('alloca');
  }
  if (calls === 0) {
    out.add('leaf');
  }
  return FEATURES.filter((f) => out.has(f));
}
