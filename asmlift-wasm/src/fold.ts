// A wasm-decompile-like readout: the instruction stream folded back into expressions by replaying
// the operand stack. It is a READING AID, not a decompiler: a block that leaves a value on the
// stack is shown as `<block result>`, and a branch that carries values is shown with the values
// flushed as statements before it. Arities come from the opcode name (unary/binary/compare), the
// memarg (load/store) and the type table (call, call_indirect).
import { type Instr, immText } from './disasm';
import { type Func, type Module, funcAt, importedFunc } from './parse';

export const UNARY =
  /^(i32|i64|f32|f64)\.(eqz|clz|ctz|popcnt|abs|neg|ceil|floor|trunc|nearest|sqrt|wrap_i64|extend\d+_s|extend_i32_[su]|trunc_(sat_)?f(32|64)_[su]|convert_i(32|64)_[su]|demote_f64|promote_f32|reinterpret_[if](32|64))$/;
const LOAD = /\.load/;
const STORE = /\.store/;

export interface FoldOptions {
  funcName?: (index: number) => string;
}

export function foldFunction(m: Module, fn: Func, instrs: Instr[], opts: FoldOptions = {}): string {
  const out: string[] = [];
  let indent = 1;
  let stack: string[] = [];
  const nameOf = (idx: number): string => {
    if (opts.funcName) {
      return opts.funcName(idx);
    }
    const imp = importedFunc(m, idx);
    if (imp !== null) {
      return `${imp.module}.${imp.name}`;
    }
    return m.funcNames.get(idx) ?? `f${idx}`;
  };
  const emit = (s: string): void => {
    out.push(`${'  '.repeat(indent)}${s}`);
  };
  const flush = (): void => {
    for (const v of stack) {
      emit(`push ${v};`);
    }
    stack = [];
  };
  const pop = (): string => stack.pop() ?? '<?>';
  const popN = (n: number): string[] => {
    const vals: string[] = [];
    for (let i = 0; i < n; i++) {
      vals.unshift(pop());
    }
    return vals;
  };
  const sig = `(${fn.params.map((p, i) => `${p} p${i}`).join(', ')}) -> (${fn.results.join(', ')})`;
  out.push(`function ${nameOf(fn.index)}${sig} {`);
  if (fn.locals.length > 0) {
    emit(`local ${fn.locals.map((t, i) => `${t} l${fn.params.length + i}`).join(', ')};`);
  }
  const local = (i: number): string => (i < fn.params.length ? `p${i}` : `l${i}`);
  const imm0 = (i: Instr): number => (i.imms[0] as { value: number }).value;
  const memarg = (i: Instr): string => {
    const ma = i.imms[0];
    return ma.kind === 'memarg' && ma.offset !== 0 ? `+${ma.offset}` : '';
  };
  for (const i of instrs) {
    const n = i.name;
    switch (n) {
      case 'block':
      case 'loop':
      case 'try': {
        flush();
        emit(`${n} ${i.imms.map((x) => immText(x, m)).join(' ')}{`.replace(' {', ' {').replace(/^(\w+) \{/, '$1 {'));
        indent++;
        break;
      }
      case 'if': {
        const c = pop();
        flush();
        emit(`if (${c}) ${i.imms.map((x) => immText(x, m)).join(' ')}{`.replace(') {', ') {'));
        indent++;
        break;
      }
      case 'else':
        flush();
        indent--;
        emit('} else {');
        indent++;
        break;
      case 'end':
        flush();
        indent = Math.max(1, indent - 1);
        if (indent === 1 && i === instrs[instrs.length - 1]) {
          break;
        }
        emit('}');
        stack.push('<block result>');
        if (stack.length === 1) {
          stack = [];
        }
        break;
      case 'br':
        flush();
        emit(`br ${imm0(i)};`);
        break;
      case 'br_if': {
        const c = pop();
        flush();
        emit(`if (${c}) br ${imm0(i)};`);
        break;
      }
      case 'br_table': {
        const c = pop();
        flush();
        emit(`br_table ${immText(i.imms[0])} on ${c};`);
        break;
      }
      case 'return': {
        const vals = popN(Math.min(stack.length, fn.results.length));
        flush();
        emit(`return ${vals.join(', ')};`);
        break;
      }
      case 'unreachable':
        flush();
        emit('unreachable;');
        break;
      case 'nop':
        break;
      case 'drop':
        emit(`${pop()};`);
        break;
      case 'select': {
        const c = pop();
        const b = pop();
        const a = pop();
        stack.push(`(${c} ? ${a} : ${b})`);
        break;
      }
      case 'local.get':
        stack.push(local(imm0(i)));
        break;
      case 'local.set':
        emit(`${local(imm0(i))} = ${pop()};`);
        break;
      case 'local.tee': {
        const v = pop();
        emit(`${local(imm0(i))} = ${v};`);
        stack.push(local(imm0(i)));
        break;
      }
      case 'global.get':
        stack.push(`g${imm0(i)}`);
        break;
      case 'global.set':
        emit(`g${imm0(i)} = ${pop()};`);
        break;
      case 'i32.const':
      case 'i64.const':
      case 'f32.const':
      case 'f64.const':
        stack.push(immText(i.imms[0]));
        break;
      case 'call':
      case 'return_call': {
        const callee = imm0(i);
        const t = calleeType(m, callee);
        const args = popN(t?.params.length ?? 0);
        const call = `${nameOf(callee)}(${args.join(', ')})`;
        if (n === 'return_call') {
          emit(`return ${call};`);
        } else if ((t?.results.length ?? 0) === 0) {
          emit(`${call};`);
        } else {
          stack.push(call);
        }
        break;
      }
      case 'call_indirect':
      case 'return_call_indirect': {
        const t = m.types[imm0(i)];
        const target = pop();
        const args = popN(t?.params.length ?? 0);
        const call = `table[${target}](${args.join(', ')})`;
        if (n === 'return_call_indirect') {
          emit(`return ${call};`);
        } else if ((t?.results.length ?? 0) === 0) {
          emit(`${call};`);
        } else {
          stack.push(call);
        }
        break;
      }
      case 'memory.size':
        stack.push('memory.size');
        break;
      case 'memory.grow':
        stack.push(`memory.grow(${pop()})`);
        break;
      case 'memory.copy':
      case 'memory.fill':
      case 'memory.init': {
        const [a, b, c] = popN(3);
        emit(`${n}(${a}, ${b}, ${c});`);
        break;
      }
      default:
        if (LOAD.test(n)) {
          const addr = pop();
          stack.push(`${n}[${addr}${memarg(i)}]`);
        } else if (STORE.test(n)) {
          const v = pop();
          const addr = pop();
          emit(`${n}[${addr}${memarg(i)}] = ${v};`);
        } else if (UNARY.test(n)) {
          stack.push(`${n}(${pop()})`);
        } else if (/^(i32|i64|f32|f64)\./.test(n)) {
          const b = pop();
          const a = pop();
          stack.push(`${n}(${a}, ${b})`);
        } else {
          flush();
          emit(`${n} ${i.imms.map((x) => immText(x, m)).join(' ')};`);
        }
    }
  }
  flush();
  out.push('}');
  return out.join('\n');
}

function calleeType(m: Module, index: number): { params: string[]; results: string[] } | null {
  const imp = importedFunc(m, index);
  if (imp !== null) {
    return imp.typeIndex === undefined ? null : m.types[imp.typeIndex];
  }
  const fn = funcAt(m, index);
  return fn === null ? null : { params: fn.params, results: fn.results };
}
