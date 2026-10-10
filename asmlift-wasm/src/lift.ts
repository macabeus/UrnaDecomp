// Lift one function of a linked module into a C++ translation unit that em++ compiles: a rough
// first spelling for a human or an agent to refine toward a match. It never declines a decodable
// function: a construct without a closer C++ spelling becomes a compilable stand-in with a
// `/* wasm: … */` comment, and every such stand-in is listed in `notes`.
//
// The operand stack is replayed into expressions. A value that would be evaluated out of order
// is spilled into a temporary first: when a call is pushed every pending value that reads memory
// or has an effect is spilled, when a load or a global read is pushed every pending effect is, so
// no expression combines an effect with an operand C++ may evaluate on its other side. Before a
// memory write, a call statement or a global write every pending memory read is spilled, and
// before a local write every pending value that reads that local is.
//
// Loads and stores go through typedefs with `may_alias` unless the flags turn strict aliasing off:
// the access widths come from the wasm, not from source types, and type-based alias analysis on
// them would reorder accesses the wasm keeps in order. Every store is volatile: the stores of
// optimised code are the ones LLVM kept, and a plain store it is free to merge with a neighbour,
// schedule past one, or drop where it proves more than the target's compiler could. A `select`
// evaluates both arms before choosing, as wasm does. Integer `+`, `-` and `*` are unsigned, which
// wraps as wasm does where signed overflow would be undefined, and a shift count is masked as wasm
// masks it.
//
// Control flow keeps the wasm nesting. A `block` is not a C scope; a branch out of it is
// `break`/`continue`/`return` when that statement lands on the same instruction, a direct-child
// `br_if` out of a void block nests the rest of the block under `if (!c) { … }`, and anything
// else is a `goto` to a label at the block's end. A `loop` is `for (;;)`, rewritten to
// `do { … } while (c)` or `while (c) { … }` when its shape is one. Every declaration is at the top
// of the function, so no `goto` crosses an initialisation.
//
// The shadow stack: `__stack_pointer` (global 0) minus a constant is a frame of local objects
// (`frameObjects`), the writes of the stack pointer that only reserve or release it are dropped,
// and a non-constant subtraction from the stack pointer, or from a frame base written back to it,
// is `__builtin_alloca`. A frame reserved again further in is a callee wasm-opt inlined after LLVM
// ran; where its region qualifies (`inlinedRegions`) it is lifted on its own as a static noinline
// function the caller calls, which wasm-opt inlines back.
//
// An `invoke_*` import stays a call: it is the call the exception lowering emits for a call inside
// a try region, and a plain call would compile to a direct or indirect call instead.
import { type BlockType, type Instr, disassemble } from './disasm';
import { type FuncType, type Module, type ValType, funcAt, importedFunc } from './parse';

export interface LiftOptions {
  /** a function index's real name: an import's or export's symbol, or a ledger name (mangled or demangled) */
  names?: ReadonlyMap<number, string>;
  /** demangled → mangled, so a demangled ledger name becomes the symbol the source must define */
  mangled?: ReadonlyMap<string, string>;
  /** the component's compile flags, written into the header */
  flags?: string;
  /** the decoded body, when the caller already has it */
  instrs?: Instr[];
}

export interface Lifted {
  /** the C++ identifier of the lifted function */
  name: string;
  /** the symbol it defines (differs from `name` behind an `__asm__` label) */
  symbol: string;
  source: string;
  /** one entry per stand-in spelling the source carries */
  notes: string[];
}

const STACK_POINTER = 0;

/** the note of an under-aligned access at a constant address, spelled through `opaque_addr` */
const OPAQUE_NOTE = 'constant address hidden from alignment inference';

/** the note of an arm whose last call or store `KEEP_IN_ARM()` keeps from the join */
const ARM_TAIL_NOTE = 'arm tail kept from the join';

const CTYPE: Record<ValType, string> = {
  i32: 'int32_t',
  i64: 'int64_t',
  f32: 'float',
  f64: 'double',
  v128: 'int32_t',
  funcref: 'int32_t',
  externref: 'int32_t',
  exnref: 'int32_t',
};

const KEYWORDS = new Set(
  (
    'alignas alignof and and_eq asm auto bitand bitor bool break case catch char char8_t char16_t char32_t class ' +
    'compl concept const consteval constexpr constinit const_cast continue co_await co_return co_yield decltype ' +
    'default delete do double dynamic_cast else enum explicit export extern false float for friend goto if inline ' +
    'int long mutable namespace new noexcept not not_eq nullptr operator or or_eq private protected public ' +
    'register reinterpret_cast requires return short signed sizeof static static_assert static_cast struct switch ' +
    'template this thread_local throw true try typedef typeid typename union unsigned using virtual void volatile ' +
    'wchar_t while xor xor_eq main'
  ).split(' '),
);

const IDENT = /^[A-Za-z_$][\w$]*$/;

/** identifiers the lifter generates for locals, temporaries, globals, frames and access types */
const GENERATED = /^(?:[pltrg]\d+|frame\d*(?:_(?:\d+|pad))?|(?:u?int\d+|float|double)_(?:ma|a\d+))$/;

export interface CName {
  ident: string;
  /** the symbol, when it is not `ident` (spelled as an `__asm__` label) */
  label?: string;
  /** the demangled name, for a comment */
  pretty?: string;
}

/** The C++ identifier for a symbol name: a mangled name stands as is; a demangled one is looked up
 *  in `mangled`, else becomes a readable identifier that is not the real symbol. */
export function cName(name: string, mangled?: ReadonlyMap<string, string>): CName {
  const demangled = name.includes('(') || name.includes('::') || name.includes(' ');
  if (demangled) {
    const m = mangled?.get(name);
    if (m !== undefined) {
      return IDENT.test(m) ? { ident: m, pretty: name } : { ident: sanitize(m), label: m, pretty: name };
    }
    return { ident: readable(name), pretty: name };
  }
  if (IDENT.test(name) && !KEYWORDS.has(name) && !name.startsWith('__builtin')) {
    return { ident: name };
  }
  return { ident: `${sanitize(name)}_`, label: name };
}

function sanitize(s: string): string {
  const t = s.replace(/[^\w$]+/g, '_');
  return /^\d/.test(t) ? `_${t}` : t;
}

/** `void ns::Cls<T>::~Cls() const` → `ns__Cls__dtor_Cls`: template arguments, ABI tags, the return
 *  type and the parameters dropped. */
function readable(demangled: string): string {
  const s = demangled
    .replace(/\[abi:[^\]]*\]/g, '')
    .replace(/\(anonymous namespace\)/g, 'anon')
    .replace(/\{lambda\([^)]*\)#(\d+)\}/g, 'lambda$1')
    .replace(/operator\s*\(\)/g, 'operator_call')
    .replace(/operator\s+(\w+)/g, 'operator_$1')
    .replace(
      /operator\s*([^\w\s(]+)/g,
      (_, o: string) => `operator_${[...o].map((ch) => ch.charCodeAt(0).toString(16)).join('')}`,
    );
  let out = '';
  let depth = 0;
  for (const c of s) {
    if (c === '<') {
      depth++;
    } else if (c === '>') {
      depth = Math.max(0, depth - 1);
    } else if (depth === 0) {
      out += c;
    }
  }
  const head = out.split('(')[0].trim();
  const last = head
    .slice(head.lastIndexOf(' ') + 1)
    .replace(/::~/g, '::dtor_')
    .replace(/::/g, '__');
  const id = sanitize(last)
    .replace(/^_+(?=[A-Za-z])/, '')
    .replace(/_+$/, '');
  return id === '' ? 'fn' : KEYWORDS.has(id) ? `${id}_` : id;
}

// ---------------------------------------------------------------------------------------------
// expressions

interface Expr {
  s: string;
  t: ValType;
  /** a binary or ternary spelling: parenthesised as an operand */
  bin: boolean;
  /** calls or other effects */
  fx: boolean;
  /** reads memory or a global */
  mem: boolean;
  /** an integer division, which LLVM does not speculate past its guard */
  trap?: boolean;
  /** the value as unsigned arithmetic, for a wrapping operation that consumes it */
  u?: string;
  locals: readonly number[];
  /** a call expression, which stands as a statement on its own */
  call?: boolean;
  /** the logical negation, when shorter than `!(s)` */
  not?: string;
  /** the value of a constant */
  k?: number | bigint;
  /** `frame + off`; `interior` when LLVM computed it inside the object at the frame's base */
  frame?: { name: string; off: number; interior?: boolean };
  /** `base + index (+ k)` with `index` scaled by a constant: pointer arithmetic, base first */
  sum?: [Expr, Expr];
  /** the constant `k` a `sum` carries */
  sumOff?: number;
  /** a shift left or a product by a constant */
  scaled?: boolean;
  /** `__stack_pointer` itself */
  sp?: boolean;
  /** derived from a local once written with a frame address: a stack-pointer write of it releases the frame */
  fromFrame?: boolean;
  /** a C++ `bool` (a comparison), which `__builtin_bit_cast` must widen first */
  bool?: boolean;
  /** a load, spelled as a volatile read: what a dropped load stands as */
  vol?: string;
  /** operations in the expression tree */
  ops?: number;
  /** the C type of the 8- or 16-bit value this is the sign or zero extension of */
  narrow?: string;
}

function leaf(s: string, t: ValType, extra: Partial<Expr> = {}): Expr {
  return { s, t, bin: false, fx: false, mem: false, locals: [], ...extra };
}

function from(s: string, t: ValType, ops: readonly Expr[], extra: Partial<Expr> = {}): Expr {
  const locals = new Set<number>();
  for (const o of ops) {
    for (const l of o.locals) {
      locals.add(l);
    }
  }
  return {
    s,
    t,
    bin: false,
    fx: ops.some((o) => o.fx),
    mem: ops.some((o) => o.mem),
    ...(ops.some((o) => o.trap) ? { trap: true } : {}),
    locals: [...locals],
    ops: ops.reduce((n, o) => n + (o.ops ?? 0), 1),
    ...extra,
  };
}

/** whether LLVM may evaluate `e` where the C source does not */
function speculatable(e: Expr): boolean {
  return !e.mem && !e.fx && !e.trap;
}

function op(e: Expr): string {
  return e.bin ? `(${e.s})` : e.s;
}

function cond(e: Expr): string {
  return e.s;
}

function negate(e: Expr): string {
  return e.not ?? `!${op(e)}`;
}

function i32Literal(v: number): string {
  return v === -2147483648 ? '(-2147483647 - 1)' : String(v);
}

function i64Literal(v: bigint): string {
  return v === -9223372036854775808n ? '(-9223372036854775807LL - 1)' : `${v}LL`;
}

function floatLiteral(bits: number): string {
  const dv = new DataView(new ArrayBuffer(4));
  dv.setUint32(0, bits >>> 0);
  const v = dv.getFloat32(0);
  if (!Number.isFinite(v)) {
    return `__builtin_bit_cast(float, (int32_t)0x${(bits >>> 0).toString(16)})`;
  }
  if (Object.is(v, -0)) {
    return '-0.0f';
  }
  for (let p = 1; p <= 9; p++) {
    const s = v.toPrecision(p);
    if (Math.fround(Number(s)) === v) {
      return `${withPoint(s)}f`;
    }
  }
  return `${withPoint(v.toPrecision(9))}f`;
}

function doubleLiteral(bits: bigint): string {
  const dv = new DataView(new ArrayBuffer(8));
  dv.setBigUint64(0, BigInt.asUintN(64, bits));
  const v = dv.getFloat64(0);
  if (!Number.isFinite(v)) {
    return `__builtin_bit_cast(double, (int64_t)0x${BigInt.asUintN(64, bits).toString(16)}ULL)`;
  }
  if (Object.is(v, -0)) {
    return '-0.0';
  }
  return withPoint(String(v));
}

function withPoint(s: string): string {
  if (/[.e]/.test(s)) {
    return s.replace(/e\+?/, 'e');
  }
  return `${s}.0`;
}

// ---------------------------------------------------------------------------------------------
// the operator tables

const BINARY: Record<string, string> = {
  add: '+',
  sub: '-',
  mul: '*',
  div_s: '/',
  rem_s: '%',
  and: '&',
  or: '|',
  xor: '^',
  shl: '<<',
  shr_s: '>>',
  div: '/',
};
const SHIFTS = new Set(['shl', 'shr_s', 'shr_u']);
const UNSIGNED_BINARY: Record<string, string> = { div_u: '/', rem_u: '%', shr_u: '>>' };
const COMPARE: Record<string, [string, string]> = {
  eq: ['==', '!='],
  ne: ['!=', '=='],
  lt_s: ['<', '>='],
  lt_u: ['<', '>='],
  gt_s: ['>', '<='],
  gt_u: ['>', '<='],
  le_s: ['<=', '>'],
  le_u: ['<=', '>'],
  ge_s: ['>=', '<'],
  ge_u: ['>=', '<'],
  lt: ['<', ''],
  gt: ['>', ''],
  le: ['<=', ''],
  ge: ['>=', ''],
};

interface MemOp {
  c: string;
  t: ValType;
  natural: number;
  store: boolean;
}

function memOp(name: string): MemOp | null {
  const m = /^(i32|i64|f32|f64)\.(load|store)(8|16|32)?(?:_([su]))?$/.exec(name);
  if (m === null) {
    return null;
  }
  const t = m[1] as ValType;
  const store = m[2] === 'store';
  const bits = m[3] === undefined ? (t === 'i64' || t === 'f64' ? 64 : 32) : Number(m[3]);
  const natural = Math.log2(bits / 8);
  let c: string;
  if (t === 'f32' || t === 'f64') {
    c = CTYPE[t];
  } else if (m[3] === undefined) {
    c = CTYPE[t];
  } else {
    c = `${m[4] === 'u' ? 'u' : ''}int${bits}_t`;
  }
  return { c, t, natural, store };
}

// ---------------------------------------------------------------------------------------------
// structure

type CtrlKind = 'func' | 'block' | 'loop' | 'if' | 'try';

interface Ctrl {
  kind: CtrlKind;
  start: number;
  end: number;
  results: ValType[];
  params: ValType[];
  resultVars: string[];
  /** a loop's parameters, which a `continue` reassigns */
  paramVars?: string[];
  label?: string;
  /** the placeholder line a loop's top label replaces */
  labelLine?: Line;
  /** stack height at entry, below the block's params */
  base: number;
  /** `if (!c) {` braces open in the current arm */
  guards: number;
  /** `continue` statements spelled for this loop */
  continues: number;
  open?: Line;
  /** a loop's trailing `br_if 0` condition, spelled as `while (c)` */
  doWhile?: Expr;
  /** an if: its condition, and the line `} else {` */
  cond?: Expr;
  elseLine?: Line;
  bodyStart?: number;
  indent: number;
}

interface Line {
  ind: number;
  text: string;
  dead?: boolean;
  /** `if (c) break;`, the first statement of a loop that `while (!c)` can absorb */
  ifBreak?: Expr;
  /** a statement that calls or writes memory (true), or an assignment that does neither (false) */
  fx?: boolean;
}

type CScope = { kind: 'loop'; ctrl: Ctrl } | { kind: 'switch' };

function blockSig(m: Module, bt: BlockType): FuncType {
  switch (bt.kind) {
    case 'empty':
      return { params: [], results: [] };
    case 'val':
      return { params: [], results: [bt.type] };
    case 'type':
      return m.types[bt.index] ?? { params: [], results: [] };
  }
}

function calleeType(m: Module, index: number): FuncType {
  const imp = importedFunc(m, index);
  if (imp !== null) {
    return (imp.typeIndex === undefined ? undefined : m.types[imp.typeIndex]) ?? { params: [], results: [] };
  }
  const fn = funcAt(m, index);
  return fn === null ? { params: [], results: [] } : { params: fn.params, results: fn.results };
}

function retType(results: readonly ValType[]): string {
  return results.length === 0 ? 'void' : CTYPE[results[0]];
}

function fnPtrType(t: FuncType): string {
  return `${retType(t.results)}(*)(${t.params.length === 0 ? 'void' : t.params.map((p) => CTYPE[p]).join(', ')})`;
}

/** Lift function `index` of `m` into a C++ translation unit. */
export function liftFunction(m: Module, index: number, opts: LiftOptions = {}): Lifted {
  const target = funcAt(m, index);
  if (target === null) {
    throw new Error(`function ${index} is an import, not a defined function`);
  }
  const notes: string[] = [];
  const note = (s: string): void => {
    if (!notes.includes(s)) {
      notes.push(s);
    }
  };

  // --- names
  const used = new Map<string, number>();
  const names = new Map<number, CName>();
  const nameOf = (idx: number): CName => {
    const known = names.get(idx);
    if (known !== undefined) {
      return known;
    }
    const real = opts.names?.get(idx);
    let n: CName = real === undefined ? { ident: `f${idx}` } : cName(real, opts.mangled);
    if (GENERATED.test(n.ident)) {
      n = { ...n, ident: `${n.ident}_`, label: n.label ?? n.ident };
    }
    const owner = used.get(n.ident);
    if (owner !== undefined && owner !== idx) {
      n = { ...n, ident: `${n.ident}_f${idx}` };
    }
    used.set(n.ident, idx);
    names.set(idx, n);
    return n;
  };
  const self = nameOf(index);

  // --- declarations every unit shares
  const typedefs = new Map<string, string>();
  /** the callees to declare: identifier → function index */
  const decls = new Map<string, number>();
  /** per callee, the narrow type a call site extends each argument from ('' where two sites differ) */
  const narrowArgs = new Map<number, (string | undefined)[]>();
  const globalsUsed = new Set<number>();
  /** the callees outlined from the function, under negative indices */
  const helpers = new Map<number, { ident: string; type: FuncType }>();
  let helperNames = 0;
  const typeOf = (idx: number): FuncType => helpers.get(idx)?.type ?? calleeType(m, idx);
  const declareCallee = (idx: number, args: readonly Expr[] = []): CName => {
    const h = helpers.get(idx);
    if (h !== undefined) {
      return { ident: h.ident };
    }
    const c = nameOf(idx);
    if (idx !== index) {
      decls.set(c.ident, idx);
      const seen = narrowArgs.get(idx) ?? [];
      narrowArgs.set(
        idx,
        args.map((a, k) =>
          seen[k] === undefined || a.narrow === undefined || seen[k] === a.narrow ? (seen[k] ?? a.narrow) : '',
        ),
      );
    }
    return c;
  };
  /** a callee's declaration; a parameter a call site sign- or zero-extends from 8 or 16 bits is that
   *  narrow type: the wasm ABI has the caller extend it, which clang does even where it can prove
   *  the extension of an `int` redundant, and leaves out at a site whose value fits already */
  const declaration = (idx: number): string => {
    const c = nameOf(idx);
    const t = calleeType(m, idx);
    const narrow = narrowArgs.get(idx) ?? [];
    const params = t.params.length === 0 ? 'void' : t.params.map((p, k) => narrow[k] || CTYPE[p]).join(', ');
    const label = c.label === undefined ? '' : ` __asm__("${c.label}")`;
    const pretty = c.pretty === undefined ? '' : ` // ${c.pretty}`;
    return `extern "C" ${retType(t.results)} ${c.ident}(${params})${label};${pretty}`;
  };
  const globalName = (g: number): string => {
    globalsUsed.add(g);
    return `g${g}`;
  };

  /** Replace every region a callee wasm-opt inlined with a call to that callee, lifted on its own:
   *  LLVM compiled it as a function of its own, so it lays out, allocates and schedules it apart. */
  const outline = (u: Unit): { instrs: Instr[]; definitions: string[] } => {
    const regions = inlinedRegions(u.instrs, u.params.length + u.locals.length, u.own ?? 0);
    if (regions.length === 0) {
      return { instrs: u.instrs, definitions: [] };
    }
    const localType = (l: number): ValType => (l < u.params.length ? u.params[l] : u.locals[l - u.params.length]);
    const instrs: Instr[] = [];
    const definitions: string[] = [];
    let at = 0;
    for (const r of regions) {
      const body = u.instrs.slice(r.body, r.block ? r.end : r.end + 1);
      const idx = -1 - helperNames;
      const ident = `${self.ident}_inlined${helperNames++}`;
      /** the helper taking `stacked` values the caller leaves on the stack, then the region's
       *  parameters, and returning `results` */
      const lift = (stacked: ValType[], results: ValType[]): UnitOut => {
        const map = new Map(r.params.map((l, k) => [l, stacked.length + k]));
        const locals: ValType[] = [];
        for (const i of body) {
          if (isLocalOp(i.name) && !map.has(immValue(i))) {
            map.set(immValue(i), stacked.length + r.params.length + locals.length);
            locals.push(localType(immValue(i)));
          }
        }
        const instrs = [
          ...stacked.map((_, k) => synthetic('local.get', k)),
          ...body.map((i) => (isLocalOp(i.name) ? synthetic(i.name, map.get(immValue(i))) : i)),
          ...(r.out === null ? [] : [synthetic('local.get', map.get(r.out))]),
          synthetic('end'),
        ];
        const params = [...stacked, ...r.params.map(localType)];
        helpers.set(idx, { ident, type: { params, results } });
        return liftUnit({ params, locals, results, instrs, ident, helper: true, own: stacked.length });
      };
      let stacked: ValType[] = [];
      let results: ValType[] = r.block
        ? blockSig(m, (u.instrs[r.start].imms[0] as { value: BlockType }).value).results
        : r.out === null
          ? []
          : [localType(r.out)];
      let h = lift(stacked, results);
      if (!r.block && h.underflow.length > 0) {
        // the callee takes a value the caller computed before its frame, straight off the stack
        stacked = h.underflow;
        h = lift(stacked, results);
      }
      if (!r.block && r.out === null && h.leftover.length === 1) {
        results = h.leftover;
        h = lift(stacked, results);
      }
      if (h.underflow.length > 0 || h.leftover.length > 0) {
        helpers.delete(idx);
        continue;
      }
      note('inlined callee outlined');
      instrs.push(...u.instrs.slice(at, r.start));
      instrs.push(...u.instrs.slice(r.start + 1, r.body));
      instrs.push(...r.params.map((l) => synthetic('local.get', l)));
      instrs.push(synthetic('call', idx, 'func'));
      if (r.out !== null) {
        instrs.push(synthetic('local.set', r.out));
      }
      definitions.push(...h.definitions);
      at = r.end + 1;
    }
    instrs.push(...u.instrs.slice(at));
    return { instrs, definitions };
  };

  const liftUnit = (u: Unit): UnitOut => {
    const fn = u;
    const outlined = outline(u);
    const instrs = outlined.instrs;
    /** the types popped below the start of the body, deepest last */
    const underflow: ValType[] = [];
    let leftover: ValType[] = [];

    // --- pre-pass: construct extents, local write counts
    const n = instrs.length;
    const endOf = new Int32Array(n).fill(-1);
    const ownerOf = new Int32Array(n).fill(-1);
    const sigOf = new Map<number, FuncType>();
    {
      const open: number[] = [];
      for (let i = 0; i < n; i++) {
        const name = instrs[i].name;
        if (name === 'block' || name === 'loop' || name === 'if' || name === 'try') {
          open.push(i);
          const bt = instrs[i].imms[0];
          sigOf.set(i, bt?.kind === 'blocktype' ? blockSig(m, bt.value) : { params: [], results: [] });
        } else if (name === 'else' || name === 'catch' || name === 'catch_all') {
          ownerOf[i] = open[open.length - 1] ?? -1;
        } else if (name === 'end' || name === 'delegate') {
          const s = open.pop();
          if (s !== undefined) {
            endOf[s] = i;
            ownerOf[i] = s;
          }
        }
      }
    }
    const funcEnd = n - 1;
    const resultsAt = (start: number): ValType[] => sigOf.get(start)?.results ?? [];
    /** where execution continues after the construct ending at `e` falls through, past ends that carry nothing */
    const exitAfter = (e: number): number => {
      let j = e + 1;
      for (;;) {
        if (j >= funcEnd) {
          return funcEnd;
        }
        const name = instrs[j].name;
        const s = ownerOf[j];
        if (name === 'end' && s >= 0 && resultsAt(s).length === 0) {
          j++;
          continue;
        }
        if (name === 'else' && s >= 0 && resultsAt(s).length === 0 && endOf[s] >= 0) {
          j = endOf[s] + 1;
          continue;
        }
        return j;
      }
    };
    const writes = new Map<number, number>();
    for (const i of instrs) {
      if (i.name === 'local.set' || i.name === 'local.tee') {
        const l = (i.imms[0] as { value: number }).value;
        writes.set(l, (writes.get(l) ?? 0) + 1);
      }
    }

    // --- declarations
    const localType = (l: number): ValType => (l < fn.params.length ? fn.params[l] : fn.locals[l - fn.params.length]);
    const localName = (l: number): string => (l < fn.params.length ? `p${l}` : `l${l}`);
    const usedLocals = new Set<number>();
    const temps: { name: string; t: ValType }[] = [];
    /** a recovered frame; `atLeast`/`atMost` bound the alignment of its object at offset 0 from how
     *  the wasm offsets its base (LLVM spells base + k as `or` exactly when the alignment exceeds k,
     *  `adds` are the k it spells as `add`), `access` is the widest load or store through it */
    const frames: FrameInfo[] = [];
    const frameNamed = (name: string) => frames.find((f) => f.name === name);
    const frameAlias = new Map<number, Expr>();
    const holdsFrame = new Set<number>();
    /** locals that hold the stack pointer, written at the top level of the function before any loop */
    const spLocals = new Set<number>();
    let labels = 0;
    let tempCount = 0;
    let resultCount = 0;

    const newTemp = (t: ValType, prefix = 't'): string => {
      const name = prefix === 't' ? `t${tempCount++}` : `r${resultCount++}`;
      temps.push({ name, t });
      return name;
    };
    // --- emission state
    const lines: Line[] = [];
    let indent = 1;
    const emit = (text: string, extra: Partial<Line> = {}): Line => {
      const line: Line = { ind: indent, text, ...extra };
      lines.push(line);
      return line;
    };
    let stack: Expr[] = [];
    const ctrls: Ctrl[] = [];
    const cscopes: CScope[] = [];
    let dead = false;
    let deadDepth = 0;
    /** the instruction being lifted */
    let cursor = 0;
    /** the value computed at the cursor is written to the stack pointer next, directly or through a tee */
    const writesStackPointer = (): boolean => {
      const next = instrs[cursor + 1];
      const after = instrs[cursor + 2];
      const isSpSet = (i: Instr | undefined): boolean =>
        i?.name === 'global.set' && (i.imms[0] as { value: number }).value === STACK_POINTER;
      return isSpSet(next) || (next?.name === 'local.tee' && isSpSet(after));
    };

    const top = (): Ctrl => ctrls[ctrls.length - 1];
    const pop = (t: ValType = 'i32'): Expr => {
      if (stack.length <= (ctrls.length === 0 ? 0 : top().base)) {
        underflow.push(t);
        if (!u.helper) {
          note('stack underflow');
        }
        return leaf(`0 /* wasm: stack underflow */`, t);
      }
      return stack.pop() as Expr;
    };
    const popN = (types: readonly ValType[]): Expr[] => {
      const vals: Expr[] = [];
      for (let i = types.length - 1; i >= 0; i--) {
        vals.unshift(pop(types[i]));
      }
      return vals;
    };
    const spillAt = (k: number): void => {
      const e = stack[k];
      if (
        e.k !== undefined ||
        (e.frame !== undefined && !e.fx) ||
        (e.sp && e.locals.length === 0) ||
        (!e.fx && !e.mem && e.locals.length === 0)
      ) {
        return;
      }
      const t = newTemp(e.t);
      emit(`${t} = ${e.s};`, { fx: e.fx });
      stack[k] = leaf(t, e.t, {
        ...(e.frame === undefined ? {} : { frame: e.frame }),
        ...(e.sp ? { sp: true } : {}),
        ...(e.fromFrame ? { fromFrame: true } : {}),
      });
    };
    /** spill the entries `pred` picks, and every memory-reading or effectful entry below one */
    const spillWhere = (pred: (e: Expr) => boolean): void => {
      let hi = -1;
      for (let k = stack.length - 1; k >= 0; k--) {
        if (pred(stack[k])) {
          hi = k;
          break;
        }
      }
      for (let k = 0; k <= hi; k++) {
        if (k === hi || stack[k].fx || stack[k].mem || pred(stack[k])) {
          spillAt(k);
        }
      }
    };
    const barrierMemory = (): void => spillWhere((e) => e.fx || e.mem);
    const barrierEffects = (): void => spillWhere((e) => e.fx);
    const barrierLocal = (l: number): void => spillWhere((e) => e.locals.includes(l));
    const spillAll = (): void => {
      for (let k = 0; k < stack.length; k++) {
        spillAt(k);
      }
    };
    /** push a value that itself reads memory (`mem`) or has an effect (`fx`), after spilling the
     *  pending values C++ could otherwise evaluate after it */
    const pushOrdered = (e: Expr): void => {
      if (e.fx) {
        barrierMemory();
      } else if (e.mem) {
        barrierEffects();
      }
      stack.push(e);
    };
    /** the effects of the pending values a branch discards, below its `keep` branch values */
    const flushDiscarded = (keep: number): void => {
      for (let k = top().base; k < stack.length - keep; k++) {
        const e = stack[k];
        if (e.fx) {
          emit(e.call ? `${e.s};` : `(void)${op(e)};`, { fx: true });
          stack[k] = leaf('0', e.t);
        }
      }
    };
    /** `return` of the function's results; a second result has no C++ spelling here */
    const returnOf = (vals: readonly Expr[]): string => {
      if (vals.length === 0) {
        return 'return;';
      }
      if (vals.length > 1) {
        note('multi-value result');
        const rest = vals
          .slice(1)
          .map((v) => v.s.replace(/\*\//g, '* /'))
          .join(', ');
        return `return ${vals[0].s}; /* wasm: also returns ${rest} */`;
      }
      return `return ${vals[0].s};`;
    };

    const labelOf = (c: Ctrl): string => {
      c.label ??= `L${labels++}`;
      return c.label;
    };
    /** how a branch to `c` is spelled from here; `goto` when no statement lands on the same instruction */
    const planJump = (c: Ctrl): 'return' | 'continue' | 'break' | 'goto' => {
      if (c.kind === 'func') {
        return 'return';
      }
      if (c.kind === 'loop') {
        const loop = cscopes.findLast((s) => s.kind === 'loop');
        return loop?.kind === 'loop' && loop.ctrl === c ? 'continue' : 'goto';
      }
      if (c.results.length === 0) {
        const exit = exitAfter(c.end);
        if (exit === funcEnd && fn.results.length === 0) {
          return 'return';
        }
        const inner = cscopes[cscopes.length - 1];
        if (inner?.kind === 'loop' && exitAfter(inner.ctrl.end) === exit) {
          return 'break';
        }
      }
      return 'goto';
    };
    /** the statement(s) that branch to `c` from here, assigning the branch's values first */
    const jumpTo = (c: Ctrl, values: Expr[]): string[] => {
      const kind = planJump(c);
      if (c.kind === 'func') {
        return [returnOf(values)];
      }
      const vars = c.kind === 'loop' ? (c.paramVars ?? []) : c.resultVars;
      const out: string[] = [];
      if (vars.length > 1 && c.kind === 'loop') {
        // the new values may read the old ones: through temporaries, as a parallel assignment
        const tmp = vars.map((_, k) => newTemp(c.params[k]));
        tmp.forEach((t, k) => out.push(`${t} = ${values[k]?.s ?? '0'};`));
        vars.forEach((r, k) => out.push(`${r} = ${tmp[k]};`));
      } else {
        vars.forEach((r, k) => out.push(`${r} = ${values[k]?.s ?? '0'};`));
      }
      switch (kind) {
        case 'continue':
          c.continues++;
          return [...out, 'continue;'];
        case 'return':
          return [...out, 'return;'];
        case 'break':
          return [...out, 'break;'];
        default:
          note('goto');
          return [...out, `goto ${labelOf(c)};`];
      }
    };
    const branchValues = (c: Ctrl): Expr[] => {
      const arity = c.kind === 'loop' ? c.params : c.results;
      const vals = stack.slice(stack.length - arity.length);
      return vals.length === arity.length ? vals : arity.map((t) => leaf('0', t));
    };
    /** Before an arm's unconditional exit to a join other arms also reach: an empty `asm` after its
     *  last call or memory write, which SimplifyCFG cannot sink, so the target's separate copies of
     *  a common tail do not merge into one at the join */
    const keepInArm = (): void => {
      const from = top().bodyStart ?? 0;
      for (let j = lines.length - 1; j >= from; j--) {
        const l = lines[j];
        if (l.dead || l.fx === false) {
          continue;
        }
        if (l.fx === true) {
          lines.splice(j + 1, 0, { ind: l.ind, text: 'KEEP_IN_ARM();' });
          note(ARM_TAIL_NOTE);
        }
        return;
      }
    };
    const closeGuards = (c: Ctrl): void => {
      while (c.guards > 0) {
        c.guards--;
        indent--;
        emit('}');
      }
    };
    const assignResults = (c: Ctrl): void => {
      if (c.results.length === 0) {
        return;
      }
      const vals = popN(c.results);
      c.resultVars.forEach((r, k) => {
        emit(`${r} = ${vals[k].s};`, { fx: vals[k].fx });
      });
    };
    const leaveArm = (): void => {
      stack = stack.slice(0, top().base);
    };
    const pushResults = (c: Ctrl): void => {
      for (let k = 0; k < c.results.length; k++) {
        stack.push(leaf(c.resultVars[k], c.results[k]));
      }
    };
    const openCtrl = (kind: CtrlKind, i: number): Ctrl => {
      const sig = sigOf.get(i) ?? { params: [], results: [] };
      const c: Ctrl = {
        kind,
        start: i,
        end: endOf[i],
        results: sig.results,
        params: sig.params,
        resultVars: sig.results.map((t) => newTemp(t, 'r')),
        base: stack.length - sig.params.length,
        guards: 0,
        continues: 0,
        indent,
      };
      ctrls.push(c);
      return c;
    };

    /** `a o b`; integer `+`, `-` and `*` wrap as wasm's do, where signed overflow would be undefined */
    const binary = (o: string, t: ValType, a: Expr, b: Expr, extra: Partial<Expr> = {}): Expr =>
      (t === 'i32' || t === 'i64') && (o === '+' || o === '-' || o === '*')
        ? wrapped(`(${ucast(t, a)} ${o} ${ucast(t, b)})`, t, [a, b], extra)
        : from(`${op(a)} ${o} ${op(b)}`, t, [a, b], { bin: true, ...extra });
    /** wasm takes a shift count modulo the width, where C makes a count past it undefined */
    const shiftCount = (t: ValType, b: Expr): Expr => {
      const mask = t === 'i64' ? 63 : 31;
      if (b.k !== undefined) {
        const k = Number(BigInt.asUintN(64, BigInt(b.k)) & BigInt(mask));
        return leaf(String(k), t, { k });
      }
      return binary('&', t, b, leaf(String(mask), t, { k: mask }));
    };
    const wrapped = (u: string, t: ValType, ops: Expr[], extra: Partial<Expr>): Expr =>
      from(`(${CTYPE[t]})${u}`, t, ops, { ...extra, u });
    const ucast = (t: ValType, e: Expr): string => {
      if (e.u !== undefined) {
        return e.u;
      }
      if (e.k !== undefined) {
        return t === 'i64' ? `${BigInt.asUintN(64, BigInt(e.k))}ULL` : `${Number(e.k) >>> 0}u`;
      }
      return `(${t === 'i64' ? 'uint64_t' : 'uint32_t'})${op(e)}`;
    };

    /** `base + index + k` as a getelementptr: LLVM keeps its base first, where it orders an integer
     *  add's operands by complexity (an argument after an instruction) */
    const gep = (base: Expr, index: Expr, k: number): string =>
      `((char*)${op(base)} + ${op(index)}${k === 0 ? '' : ` ${k > 0 ? '+' : '-'} ${Math.abs(k)}`})`;
    /** a pointer sum as a value: a cast of the getelementptr, which LLVM folds back into an integer
     *  add only where nothing dereferences it */
    const sumValue = (base: Expr, index: Expr, k: number, extra: Partial<Expr> = {}): Expr =>
      from(`(int32_t)${gep(base, index, k)}`, 'i32', [base, index], {
        sum: [base, index],
        ...(k === 0 ? {} : { sumOff: k }),
        ...extra,
      });
    /** the address of a load or store of `1 << natural` bytes whose memarg alignment is `align` (log2) */
    const address = (base: Expr, off: number, ctype: string, natural: number, align: number): string => {
      const width = 1 << natural;
      if (base.frame !== undefined) {
        const f = frameNamed(base.frame.name);
        if (f !== undefined) {
          f.access = Math.max(f.access, width);
        }
        return `(${ctype}*)${frameToken('A', base.frame.name, base.frame.off + off, { natural, align })}`;
      }
      if (base.k !== undefined) {
        const at = (Number(base.k) + off) >>> 0;
        if (align < natural && (at & ((1 << natural) - 1)) % (2 << align) === 0) {
          // LLVM raises an access's alignment to what a constant address shows; the target's address
          // is a symbol's, whose alignment says no more than the memarg
          note(OPAQUE_NOTE);
          return `(${ctype}*)opaque_addr(${at})`;
        }
        return `(${ctype}*)${at}`;
      }
      if (base.sum !== undefined) {
        return `(${ctype}*)${gep(base.sum[0], base.sum[1], (base.sumOff ?? 0) + off)}`;
      }
      return off === 0 ? `(${ctype}*)${op(base)}` : `(${ctype}*)((char*)${op(base)} + ${off})`;
    };
    const strictAliasing = !/(^|\s)-fno-strict-aliasing(\s|$)/.test(opts.flags ?? '');
    const memType = (mo: MemOp, align: number): string => {
      const attrs = strictAliasing ? ['may_alias'] : [];
      const under = align < mo.natural;
      if (under) {
        attrs.unshift(`aligned(${1 << align})`);
      }
      if (attrs.length === 0) {
        return mo.c;
      }
      const name = `${mo.c.replace(/_t$/, '')}_${under ? `a${1 << align}` : 'ma'}`;
      typedefs.set(name, `typedef ${mo.c} ${name} __attribute__((${attrs.join(', ')}));`);
      return name;
    };

    const numeric = (name: string): boolean => {
      const mm = /^(i32|i64|f32|f64)\.(\w+)$/.exec(name);
      if (mm === null) {
        return false;
      }
      const t = mm[1] as ValType;
      const o = mm[2];
      const fs = t === 'f32' ? 'f' : '';
      const bits = t === 'i64' ? 64 : 32;
      const ct = CTYPE[t];
      if (
        BINARY[o] !== undefined &&
        (t === 'i32' || t === 'i64' || o === 'add' || o === 'sub' || o === 'mul' || o === 'div')
      ) {
        const b0 = pop(t);
        const a = pop(t);
        const b = SHIFTS.has(o) ? shiftCount(t, b0) : b0;
        const frameOr = o === 'or' && a.frame !== undefined && a.frame.off === 0;
        if (
          t === 'i32' &&
          (o === 'add' || o === 'sub' || frameOr) &&
          b.k !== undefined &&
          (a.frame !== undefined || a.sp)
        ) {
          const k = Number(b.k);
          const size = o === 'sub' ? k : -k;
          if (a.sp && size > 0) {
            const name = frames.length === 0 ? 'frame' : `frame${frames.length}`;
            frames.push({ name, size, atLeast: 1, atMost: 16, adds: [], access: 0, spans: [] });
            stack.push(leaf(`(int32_t)${frameToken('E', name, 0)}`, 'i32', { frame: { name, off: 0 } }));
            return true;
          }
          if (a.frame !== undefined) {
            const f = frameNamed(a.frame.name);
            if (f !== undefined && a.frame.off === 0 && k > 0) {
              if (o === 'or') {
                f.atLeast = Math.max(f.atLeast, 2 ** (Math.floor(Math.log2(k)) + 1));
              } else if (o === 'add') {
                f.adds.push(k);
              }
            }
            const off = a.frame.off + (o === 'sub' ? -k : k);
            // an address LLVM spells as `or` lies inside the object at the frame's base; one it spells
            // as `add` starts an object of its own when it is taken as a value
            const kind = o === 'or' || a.frame.off !== 0 || a.frame.interior === true ? 'I' : 'E';
            stack.push(
              leaf(`(int32_t)${frameToken(kind, a.frame.name, off)}`, 'i32', {
                frame: { name: a.frame.name, off, ...(kind === 'I' ? { interior: true } : {}) },
              }),
            );
            return true;
          }
        }
        const frameBase = a.frame !== undefined && a.frame.off === 0 && !a.fx;
        if (t === 'i32' && o === 'sub' && (a.sp || (frameBase && writesStackPointer()))) {
          pushOrdered(from(`(int32_t)__builtin_alloca(${b.s})`, 'i32', [b], { fx: true, call: true, fromFrame: true }));
          return true;
        }
        if (t === 'i32' && (o === 'add' || o === 'sub') && a.sum !== undefined && b.k !== undefined) {
          const k = (a.sumOff ?? 0) + (o === 'add' ? Number(b.k) : -Number(b.k));
          stack.push(sumValue(a.sum[0], a.sum[1], k, a.fromFrame ? { fromFrame: true } : {}));
          return true;
        }
        if (
          (o === 'add' || o === 'sub') &&
          b.k !== undefined &&
          b.k < 0 &&
          b.k !== -2147483648 &&
          b.k !== -9223372036854775808n
        ) {
          const pos =
            t === 'i64'
              ? leaf(i64Literal(-(b.k as bigint)), t, { k: -(b.k as bigint) })
              : leaf(String(-Number(b.k)), t, { k: -Number(b.k) });
          stack.push(binary(o === 'add' ? '-' : '+', t, a, pos, a.fromFrame ? { fromFrame: true } : {}));
          return true;
        }
        const sum = t === 'i32' && o === 'add' && a.k === undefined && !a.scaled && b.scaled === true;
        if (sum) {
          stack.push(sumValue(a, b, 0, a.fromFrame ? { fromFrame: true } : {}));
          return true;
        }
        const scaled = (o === 'shl' || o === 'mul') && b.k !== undefined;
        const mask = t === 'i32' && o === 'and' ? { 255: 'uint8_t', 65535: 'uint16_t' }[Number(b.k)] : undefined;
        stack.push(
          binary(BINARY[o], t, a, b, {
            ...((o === 'div_s' || o === 'rem_s') && b.k === undefined ? { trap: true } : {}),
            ...(a.fromFrame && b.k !== undefined ? { fromFrame: true } : {}),
            ...(mask === undefined ? {} : { narrow: mask }),
            ...(scaled ? { scaled: true } : {}),
          }),
        );
        return true;
      }
      if (UNSIGNED_BINARY[o] !== undefined) {
        const b = pop(t);
        const a = pop(t);
        const rhs = o === 'shr_u' ? op(shiftCount(t, b)) : ucast(t, b);
        stack.push(
          from(
            `(${ct})(${ucast(t, a)} ${UNSIGNED_BINARY[o]} ${rhs})`,
            t,
            [a, b],
            o === 'shr_u' || b.k !== undefined ? {} : { trap: true },
          ),
        );
        return true;
      }
      if (o === 'rotl' || o === 'rotr') {
        const b = pop(t);
        const a = pop(t);
        const fnName = `__builtin_rotate${o === 'rotl' ? 'left' : 'right'}${bits}`;
        stack.push(from(`(${ct})${fnName}(${ucast(t, a)}, ${ucast(t, b)})`, t, [a, b]));
        return true;
      }
      if (COMPARE[o] !== undefined) {
        const b = pop(t);
        const a = pop(t);
        const [c, inv] = COMPARE[o];
        const unsigned = o.endsWith('_u');
        const as = unsigned ? ucast(t, a) : op(a);
        const bs = unsigned ? ucast(t, b) : op(b);
        stack.push(
          from(`${as} ${c} ${bs}`, 'i32', [a, b], {
            bin: true,
            bool: true,
            ...(inv === '' ? {} : { not: `${as} ${inv} ${bs}` }),
          }),
        );
        return true;
      }
      if (o === 'eqz') {
        const a = pop(t);
        stack.push(from(`!${op(a)}`, 'i32', [a], { bool: true, not: a.s }));
        return true;
      }
      const unary = (s: string, rt: ValType, a: Expr, extra: Partial<Expr> = {}): boolean => {
        stack.push(from(s, rt, [a], extra));
        return true;
      };
      switch (o) {
        case 'clz':
        case 'ctz': {
          const a = pop(t);
          return unary(`(${ct})__builtin_${o}g(${ucast(t, a)}, ${bits})`, t, a);
        }
        case 'popcnt': {
          const a = pop(t);
          return unary(`(${ct})__builtin_popcount${t === 'i64' ? 'll' : ''}(${ucast(t, a)})`, t, a);
        }
        case 'min':
        case 'max':
        case 'copysign': {
          const b = pop(t);
          const a = pop(t);
          const f = o === 'copysign' ? 'copysign' : o === 'min' ? 'fmin' : 'fmax';
          stack.push(from(`__builtin_${f}${fs}(${a.s}, ${b.s})`, t, [a, b]));
          return true;
        }
        case 'abs':
        case 'ceil':
        case 'floor':
        case 'trunc':
        case 'nearest':
        case 'sqrt': {
          const a = pop(t);
          const f = { abs: 'fabs', ceil: 'ceil', floor: 'floor', trunc: 'trunc', nearest: 'nearbyint', sqrt: 'sqrt' }[
            o
          ];
          return unary(`__builtin_${f}${fs}(${a.s})`, t, a);
        }
        case 'neg': {
          const a = pop(t);
          return unary(op(a).startsWith('-') ? `-(${a.s})` : `-${op(a)}`, t, a);
        }
        case 'wrap_i64': {
          const a = pop('i64');
          return unary(`(int32_t)${op(a)}`, 'i32', a);
        }
        case 'extend_i32_s':
        case 'extend_i32_u': {
          const a = pop('i32');
          return unary(o.endsWith('_u') ? `(int64_t)(uint32_t)${op(a)}` : `(int64_t)${op(a)}`, 'i64', a);
        }
        case 'extend8_s':
        case 'extend16_s':
        case 'extend32_s': {
          const a = pop(t);
          const narrow = `int${/\d+/.exec(o)?.[0]}_t`;
          return unary(`(${ct})(${narrow})${op(a)}`, t, a, t === 'i32' ? { narrow } : {});
        }
        case 'demote_f64': {
          const a = pop('f64');
          return unary(`(float)${op(a)}`, 'f32', a);
        }
        case 'promote_f32': {
          const a = pop('f32');
          return unary(`(double)${op(a)}`, 'f64', a);
        }
        default:
          break;
      }
      const cv = /^(trunc|trunc_sat|convert|reinterpret)_([if])(32|64)(?:_([su]))?$/.exec(o);
      if (cv !== null) {
        const from_ = `${cv[2]}${cv[3]}` as ValType;
        const a = pop(from_);
        if (cv[1] === 'reinterpret') {
          return unary(`__builtin_bit_cast(${ct}, (${CTYPE[from_]})${op(a)})`, t, a);
        }
        if (cv[1] === 'convert') {
          const src = cv[4] === 'u' ? `(${from_ === 'i64' ? 'uint64_t' : 'uint32_t'})${op(a)}` : op(a);
          return unary(`(${ct})${src}`, t, a);
        }
        return unary(
          cv[4] === 'u' ? `(${ct})(${t === 'i64' ? 'uint64_t' : 'uint32_t'})${op(a)}` : `(${ct})${op(a)}`,
          t,
          a,
        );
      }
      return false;
    };

    /** a call through table slot `target`: a direct call when the slot is a constant the table fills with
     *  a function of the same type, else a cast of the slot to a function pointer */
    const indirect = (target: Expr, t: FuncType, args: Expr[]): Expr => {
      const slot = target.k === undefined ? undefined : tableSlots(m).get(Number(target.k));
      if (slot !== undefined && sameType(calleeType(m, slot), t)) {
        const c = declareCallee(slot, args);
        return from(`${c.ident}(${args.map((a) => a.s).join(', ')})`, t.results[0] ?? 'i32', args, {
          fx: true,
          call: true,
        });
      }
      return from(
        `((${fnPtrType(t)})${op(target)})(${args.map((a) => a.s).join(', ')})`,
        t.results[0] ?? 'i32',
        [...args, target],
        { fx: true, call: true },
      );
    };

    /** pop an op's operands and push a typed zero: the stand-in for an op with no C++ spelling here */
    const fallback = (i: Instr, pops: ValType[], push: ValType | null): void => {
      note(`no spelling for ${i.name}`);
      const args = popN(pops);
      for (const a of args) {
        if (a.fx) {
          emit(`(void)${op(a)};`, { fx: true });
        }
      }
      const what = `/* wasm: ${i.name}${i.imms.length === 0 ? '' : ' …'} */`;
      if (push === null) {
        emit(what);
      } else {
        pushOrdered(leaf(`0 ${what}`, push, { fx: true }));
      }
    };

    const emitCall = (call: Expr, results: readonly ValType[], tail: boolean): void => {
      if (tail) {
        emit(fn.results.length === 0 ? `${call.s};` : `return ${call.s};`);
        if (fn.results.length === 0) {
          emit('return;');
        }
        dead = true;
        return;
      }
      if (results.length === 0) {
        barrierMemory();
        emit(`${call.s};`, { fx: true });
        return;
      }
      pushOrdered(call);
      if (results.length > 1) {
        note('multi-value call');
        for (const t of results.slice(1)) {
          stack.push(leaf(`0 /* wasm: a further result of ${call.s.replace(/\*\//g, '* /')} */`, t));
        }
      }
    };

    // --- the walk
    ctrls.push({
      kind: 'func',
      start: -1,
      end: funcEnd,
      results: fn.results,
      params: [],
      resultVars: [],
      base: 0,
      guards: 0,
      continues: 0,
      indent: 1,
    });

    for (let idx = 0; idx < n; idx++) {
      cursor = idx;
      const ins = instrs[idx];
      const name = ins.name;
      const imm = (k = 0): number => (ins.imms[k] as { value: number }).value;
      if (dead) {
        if (name === 'block' || name === 'loop' || name === 'if' || name === 'try') {
          deadDepth++;
          continue;
        }
        if (deadDepth > 0) {
          if (name === 'end' || name === 'delegate') {
            deadDepth--;
          }
          continue;
        }
        if (name !== 'end' && name !== 'else' && name !== 'delegate' && name !== 'catch' && name !== 'catch_all') {
          continue;
        }
      }
      switch (name) {
        case 'nop':
          break;
        case 'block':
        case 'try': {
          spillAll();
          const c = openCtrl(name === 'try' ? 'try' : 'block', idx);
          if (name === 'try') {
            note('try');
            emit('/* wasm: try */');
          }
          c.bodyStart = lines.length;
          break;
        }
        case 'loop': {
          spillAll();
          spLocals.clear();
          const params = sigOf.get(idx)?.params ?? [];
          const paramVars = params.map((t) => newTemp(t, 'r'));
          if (params.length > 0) {
            popN(params).forEach((v, k) => emit(`${paramVars[k]} = ${v.s};`, { fx: v.fx }));
            paramVars.forEach((r, k) => stack.push(leaf(r, params[k])));
          }
          const labelLine = emit('', { dead: true });
          const c = openCtrl('loop', idx);
          c.labelLine = labelLine;
          c.paramVars = paramVars;
          c.open = emit('for (;;) {');
          indent++;
          c.bodyStart = lines.length;
          cscopes.push({ kind: 'loop', ctrl: c });
          break;
        }
        case 'if': {
          const c0 = pop('i32');
          spillAll();
          const c = openCtrl('if', idx);
          c.cond = c0;
          c.open = emit(`if (${cond(c0)}) {`);
          indent++;
          c.bodyStart = lines.length;
          break;
        }
        case 'else':
        case 'catch':
        case 'catch_all': {
          const c = top();
          if (!dead) {
            if (name === 'else') {
              keepInArm();
            }
            assignResults(c);
          }
          closeGuards(c);
          leaveArm();
          dead = false;
          if (name !== 'else') {
            note(name);
            emit(`/* wasm: ${name} */`);
            dead = true;
            break;
          }
          indent--;
          if (lines.length === c.bodyStart && c.cond !== undefined && c.open !== undefined) {
            c.open.text = `if (${negate(c.cond)}) {`;
            c.elseLine = undefined;
          } else {
            c.elseLine = emit('} else {');
          }
          indent++;
          c.bodyStart = lines.length;
          break;
        }
        case 'end':
        case 'delegate': {
          const c = top();
          if (c.kind === 'func') {
            if (!dead && fn.results.length > 0) {
              emit(returnOf(popN(fn.results)));
            }
            leftover = dead ? [] : stack.map((e) => e.t);
            break;
          }
          const reachable = !dead;
          if (c.kind === 'loop') {
            if (reachable && c.doWhile === undefined) {
              assignResults(c);
            }
            closeGuards(c);
            leaveArm();
            cscopes.pop();
            const last = lines.findLast((l) => !l.dead);
            if (c.doWhile !== undefined) {
              indent--;
              emit(`} while (${cond(c.doWhile)});`);
              if (c.open !== undefined) {
                c.open.text = 'do {';
              }
            } else {
              if (last !== undefined && last.text === 'continue;' && last.ind === c.indent + 1) {
                last.dead = true;
              } else if (reachable) {
                emit('break;');
              }
              indent--;
              emit('}');
              const first = lines.slice(c.bodyStart).find((l) => !l.dead);
              if (first?.ifBreak !== undefined && first.ind === c.indent + 1 && c.open !== undefined) {
                c.open.text = `while (${negate(first.ifBreak)}) {`;
                first.dead = true;
              }
            }
            if (c.label !== undefined && c.labelLine !== undefined) {
              c.labelLine.text = `${c.label}:;`;
              c.labelLine.dead = false;
            }
          } else {
            if (reachable) {
              if ((c.kind === 'if' && c.elseLine !== undefined) || c.label !== undefined) {
                keepInArm();
              }
              assignResults(c);
            }
            closeGuards(c);
            leaveArm();
            if (c.kind === 'if') {
              indent--;
              if (c.elseLine !== undefined && lines[lines.length - 1] === c.elseLine) {
                c.elseLine.text = '}';
              } else {
                emit('}');
              }
            }
            if (c.label !== undefined) {
              emit(`${c.label}:;`, { ind: c.indent });
            }
          }
          ctrls.pop();
          pushResults(c);
          dead = false;
          break;
        }
        case 'br': {
          const c = ctrls[ctrls.length - 1 - imm()];
          flushDiscarded((c.kind === 'loop' ? c.params : c.results).length);
          const vals = branchValues(c);
          const direct = imm() === 0 && c.kind !== 'loop' && c.kind !== 'func' && idx + 1 === c.end;
          if (c.kind !== 'loop') {
            keepInArm();
          }
          if (!direct || c.results.length > 0) {
            for (const s of jumpTo(c, vals)) {
              emit(s);
            }
          }
          dead = true;
          break;
        }
        case 'br_if': {
          const c0 = pop('i32');
          const c = ctrls[ctrls.length - 1 - imm()];
          spillAll();
          if (
            c.kind === 'loop' &&
            imm() === 0 &&
            c.end === idx + 1 &&
            c.continues === 0 &&
            c.results.length === 0 &&
            c.guards === 0 &&
            stack.length === c.base
          ) {
            c.doWhile = c0;
            break;
          }
          const vals = branchValues(c);
          if (
            imm() === 0 &&
            (c.kind === 'block' || c.kind === 'if') &&
            c.results.length === 0 &&
            planJump(c) === 'goto'
          ) {
            emit(`if (${negate(c0)}) {`);
            indent++;
            c.guards++;
            break;
          }
          const jump = jumpTo(c, vals);
          if (jump.length === 1) {
            emit(`if (${cond(c0)}) ${jump[0]}`, jump[0] === 'break;' ? { ifBreak: c0 } : {});
          } else {
            emit(`if (${cond(c0)}) {`);
            indent++;
            for (const s of jump) {
              emit(s);
            }
            indent--;
            emit('}');
          }
          break;
        }
        case 'br_table': {
          const ix = pop('i32');
          spillAll();
          const lab = ins.imms[0];
          if (lab.kind !== 'labels') {
            break;
          }
          const def = ctrls[ctrls.length - 1 - lab.default];
          flushDiscarded((def.kind === 'loop' ? def.params : def.results).length);
          const vals = branchValues(def);
          if (lab.labels.every((d) => d === lab.default)) {
            if (ix.fx) {
              emit(`(void)${op(ix)};`);
            }
            for (const s of jumpTo(def, vals)) {
              emit(s);
            }
            dead = true;
            break;
          }
          emit(`switch (${ix.s}) {`);
          cscopes.push({ kind: 'switch' });
          const groups = new Map<number, number[]>();
          lab.labels.forEach((d, k) => {
            if (d !== lab.default) {
              groups.set(d, [...(groups.get(d) ?? []), k]);
            }
          });
          for (const [d, cases] of groups) {
            const target = ctrls[ctrls.length - 1 - d];
            const jump = jumpTo(target, vals);
            emit(`${cases.map((k) => `case ${k}:`).join(' ')} ${jump.join(' ')}`);
          }
          emit(`default: ${jumpTo(def, vals).join(' ')}`);
          cscopes.pop();
          emit('}');
          dead = true;
          break;
        }
        case 'return': {
          flushDiscarded(fn.results.length);
          keepInArm();
          emit(returnOf(popN(fn.results)));
          dead = true;
          break;
        }
        case 'unreachable':
          spillAll();
          emit('__builtin_trap();');
          dead = true;
          break;
        case 'throw':
        case 'rethrow':
        case 'throw_ref':
          note(name);
          emit(`__builtin_trap(); /* wasm: ${name} */`);
          dead = true;
          break;
        case 'drop': {
          const v = pop();
          if (v.call) {
            barrierMemory();
            emit(`${v.s};`, { fx: true });
          } else if (v.fx) {
            barrierMemory();
            emit(`(void)${op(v)};`, { fx: true });
          } else if (v.vol !== undefined) {
            // a load kept only because it may trap: a volatile read is the spelling clang keeps
            emit(`(void)${v.vol};`, { fx: true });
          } else if (v.mem) {
            emit(`(void)(${v.s});`);
          }
          break;
        }
        case 'select': {
          const c0 = pop('i32');
          const b = pop();
          const a = pop(b.t);
          const pick = from(`${op(c0)} ? ${op(a)} : ${op(b)}`, a.t, [c0, a, b], { bin: true });
          // wasm evaluates both arms; a C conditional evaluates one, and LLVM cannot speculate a load
          // or a division it does not know is safe back into a select
          if (!speculatable(a) || !speculatable(b)) {
            stack.push(a, b);
            spillAt(stack.length - 2);
            spillAt(stack.length - 1);
            const bb = stack.pop() as Expr;
            const aa = stack.pop() as Expr;
            stack.push(from(`${op(c0)} ? ${aa.s} : ${bb.s}`, a.t, [c0, aa, bb], { bin: true }));
          } else {
            stack.push(pick);
          }
          break;
        }
        case 'local.get': {
          const l = imm();
          const alias = frameAlias.get(l);
          if (alias !== undefined) {
            stack.push(alias);
          } else {
            usedLocals.add(l);
            stack.push(
              leaf(localName(l), localType(l), {
                locals: [l],
                ...(holdsFrame.has(l) ? { fromFrame: true } : {}),
                ...(spLocals.has(l) ? { sp: true } : {}),
              }),
            );
          }
          break;
        }
        case 'local.set':
        case 'local.tee': {
          const l = imm();
          const v = pop(localType(l));
          if (v.frame !== undefined && writes.get(l) === 1 && v.frame.off === 0) {
            frameAlias.set(l, v);
            if (name === 'local.tee') {
              stack.push(v);
            }
            break;
          }
          usedLocals.add(l);
          barrierLocal(l);
          if (v.frame !== undefined || v.fromFrame) {
            holdsFrame.add(l);
          }
          const sp = v.sp === true && ctrls.length === 1;
          if (sp) {
            spLocals.add(l);
          } else {
            spLocals.delete(l);
          }
          emit(`${localName(l)} = ${v.s};`, { fx: v.fx });
          if (name === 'local.tee') {
            stack.push(
              v.frame === undefined
                ? leaf(localName(l), localType(l), {
                    locals: [l],
                    ...(sp ? { sp: true } : {}),
                    ...(v.fromFrame ? { fromFrame: true } : {}),
                  })
                : v,
            );
          }
          break;
        }
        case 'global.get': {
          const g = imm();
          if (g === STACK_POINTER) {
            stack.push(leaf('(int32_t)__builtin_frame_address(0)', 'i32', { sp: true, mem: true }));
          } else {
            pushOrdered(leaf(globalName(g), m.globals[g - importedGlobals(m)]?.type ?? 'i32', { mem: true }));
          }
          break;
        }
        case 'global.set': {
          const g = imm();
          const v = pop();
          if (g === STACK_POINTER) {
            if (v.frame === undefined && !v.sp && !v.fromFrame) {
              note('stack pointer write');
              if (v.fx) {
                emit(`(void)${op(v)};`);
              }
              emit(`/* wasm: __stack_pointer = ${v.s.replace(/\*\//g, '* /')} */`);
            }
            break;
          }
          barrierMemory();
          emit(`${globalName(g)} = ${v.s};`, { fx: true });
          break;
        }
        case 'i32.const':
          stack.push(leaf(i32Literal(imm()), 'i32', { k: imm(), bin: imm() < 0 }));
          break;
        case 'i64.const': {
          const v = (ins.imms[0] as { value: bigint }).value;
          stack.push(leaf(i64Literal(v), 'i64', { k: v, bin: v < 0n }));
          break;
        }
        case 'f32.const': {
          const s = floatLiteral((ins.imms[0] as { bits: number }).bits);
          stack.push(leaf(s, 'f32', { bin: s.startsWith('-') }));
          break;
        }
        case 'f64.const': {
          const s = doubleLiteral((ins.imms[0] as { bits: bigint }).bits);
          stack.push(leaf(s, 'f64', { bin: s.startsWith('-') }));
          break;
        }
        case 'call':
        case 'return_call': {
          const callee = imm();
          const t = typeOf(callee);
          const args = popN(t.params);
          const imp = callee >= 0 ? importedFunc(m, callee) : null;
          const real = opts.names?.get(callee);
          if (imp !== null && real?.startsWith('invoke_') && args[0]?.k !== undefined) {
            // the JS thunk the exception lowering calls in place of a call inside a try region; a constant
            // first argument is the wrapped callee's table slot
            const slot = tableSlots(m).get(Number(args[0].k));
            if (slot !== undefined) {
              const c = nameOf(slot);
              args[0] = {
                ...args[0],
                s: `${args[0].s} /* &${(c.pretty ?? c.label ?? c.ident).replace(/\*\//g, '* /')} */`,
              };
            }
          }
          const c = declareCallee(callee, args);
          const call = from(`${c.ident}(${args.map((a) => a.s).join(', ')})`, t.results[0] ?? 'i32', args, {
            fx: true,
            call: true,
          });
          emitCall(call, t.results, name === 'return_call');
          break;
        }
        case 'call_indirect':
        case 'return_call_indirect': {
          const t = m.types[imm()] ?? { params: [], results: [] };
          const target = pop('i32');
          const args = popN(t.params);
          emitCall(indirect(target, t, args), t.results, name === 'return_call_indirect');
          break;
        }
        case 'memory.size':
          pushOrdered(leaf('(int32_t)__builtin_wasm_memory_size(0)', 'i32', { mem: true }));
          break;
        case 'memory.grow': {
          const a = pop('i32');
          pushOrdered(from(`(int32_t)__builtin_wasm_memory_grow(0, ${a.s})`, 'i32', [a], { fx: true, call: true }));
          break;
        }
        case 'memory.copy':
        case 'memory.fill': {
          const [d, s, len] = popN(['i32', 'i32', 'i32']);
          if (len.k !== undefined) {
            // the bytes one call fills or copies lie in one object
            for (const e of name === 'memory.copy' ? [d, s] : [d]) {
              if (e.frame !== undefined) {
                frameNamed(e.frame.name)?.spans.push([e.frame.off, Number(len.k)]);
              }
            }
          }
          barrierMemory();
          emit(
            name === 'memory.copy'
              ? `__builtin_memmove((void*)${op(d)}, (void*)${op(s)}, ${len.s});`
              : `__builtin_memset((void*)${op(d)}, ${s.s}, ${len.s});`,
            { fx: true },
          );
          break;
        }
        case 'memory.init':
          fallback(ins, ['i32', 'i32', 'i32'], null);
          break;
        case 'data.drop':
        case 'elem.drop':
          fallback(ins, [], null);
          break;
        case 'table.get':
          fallback(ins, ['i32'], 'i32');
          break;
        case 'table.set':
          fallback(ins, ['i32', 'i32'], null);
          break;
        case 'table.size':
          fallback(ins, [], 'i32');
          break;
        case 'table.grow':
          fallback(ins, ['i32', 'i32'], 'i32');
          break;
        case 'table.fill':
        case 'table.copy':
        case 'table.init':
          fallback(ins, ['i32', 'i32', 'i32'], null);
          break;
        case 'ref.null':
          fallback(ins, [], 'funcref');
          break;
        case 'ref.func':
          stack.push(leaf(String(imm()), 'funcref'));
          break;
        case 'ref.is_null':
          fallback(ins, ['funcref'], 'i32');
          break;
        default: {
          const mo = memOp(name);
          const ma = ins.imms[0];
          if (mo !== null && ma?.kind === 'memarg') {
            const ctype = memType(mo, ma.align);
            if (mo.store) {
              const v = pop(mo.t);
              const a = pop('i32');
              barrierMemory();
              emit(`*${address(a, ma.offset, `volatile ${ctype}`, mo.natural, ma.align)} = ${v.s};`, { fx: true });
            } else {
              const a = pop('i32');
              const widen = mo.t === 'i64' && mo.c !== 'int64_t' ? '(int64_t)' : '';
              pushOrdered(
                from(`${widen}*${address(a, ma.offset, ctype, mo.natural, ma.align)}`, mo.t, [a], {
                  mem: true,
                  vol: `*${address(a, ma.offset, `volatile ${ctype}`, mo.natural, ma.align)}`,
                }),
              );
            }
            break;
          }
          if (numeric(name)) {
            break;
          }
          fallback(ins, [], null);
        }
      }
    }

    // --- assemble
    const body = unguardMemOps(lines.filter((l) => !l.dead));
    while (body.length > 0 && body[body.length - 1].text === 'return;' && body[body.length - 1].ind === 1) {
      body.pop();
    }
    const out: string[] = [];
    const params = fn.params.map((p, k) => `${CTYPE[p]} p${k}`).join(', ') || 'void';
    if (u.helper) {
      out.push(`static __attribute__((noinline)) ${retType(fn.results)} ${u.ident}(${params}) {`);
    } else {
      const proto = `extern "C" ${retType(fn.results)} ${u.ident}(${params})`;
      if (self.label !== undefined) {
        out.push(`${proto} __asm__("${self.label}");`);
      }
      out.push(`${proto} {`);
    }
    const objects = frameObjects(frames, body);
    for (const o of objects.decls) {
      out.push(`  ${o}`);
    }
    for (const nt of objects.notes) {
      note(nt);
    }
    const byType = new Map<string, string[]>();
    for (let l = fn.params.length; l < fn.params.length + fn.locals.length; l++) {
      if (usedLocals.has(l)) {
        const ct = CTYPE[localType(l)];
        byType.set(ct, [...(byType.get(ct) ?? []), `${localName(l)} = 0`]);
      }
    }
    for (const t of temps) {
      byType.set(CTYPE[t.t], [...(byType.get(CTYPE[t.t]) ?? []), t.name]);
    }
    for (const [ct, vars] of byType) {
      for (let k = 0; k < vars.length; k += 12) {
        out.push(`  ${ct} ${vars.slice(k, k + 12).join(', ')};`);
      }
    }
    for (const l of body) {
      out.push(`${'  '.repeat(Math.max(1, l.ind))}${objects.spell(l.text)}`);
    }
    out.push('}', '');
    return {
      definitions: [...outlined.definitions, out.join('\n')],
      leftover,
      underflow: [...underflow].reverse(),
    };
  };

  const main = liftUnit({
    params: target.params,
    locals: target.locals,
    results: target.results,
    instrs: opts.instrs ?? disassemble(target),
    ident: self.ident,
    helper: false,
  });
  const out: string[] = [];
  const pretty = self.pretty === undefined ? '' : ` ${self.pretty}`;
  out.push(`// wasm function ${index}${pretty}`);
  if (opts.flags !== undefined) {
    out.push(`// em++ ${opts.flags}`);
  }
  out.push('#include <stdint.h>', '');
  if (notes.includes(ARM_TAIL_NOTE)) {
    out.push(
      "// an arm's last call or store stays in the arm: SimplifyCFG sinks no asm into the join",
      '#define KEEP_IN_ARM() __asm__ volatile("")',
      '',
    );
  }
  if (notes.includes(OPAQUE_NOTE)) {
    out.push(
      '// the address of an under-aligned access, which clang would otherwise align by its value',
      'static inline int32_t opaque_addr(int32_t a) {',
      '  __asm__("" : "+r"(a));',
      '  return a;',
      '}',
      '',
    );
  }
  if (typedefs.size > 0) {
    out.push(...typedefs.values(), '');
  }
  if (decls.size > 0 || globalsUsed.size > 0) {
    out.push(...[...decls.values()].map(declaration));
    for (const g of [...globalsUsed].sort((a, b) => a - b)) {
      const t = m.globals[g - importedGlobals(m)]?.type ?? 'i32';
      out.push(`extern "C" ${CTYPE[t]} g${g};`);
    }
    out.push('');
  }
  if (main.definitions.length > 1) {
    // an outlined callee may call the function back
    const params = target.params.map((p) => CTYPE[p]).join(', ') || 'void';
    const label = self.label === undefined ? '' : ` __asm__("${self.label}")`;
    out.push(`extern "C" ${retType(target.results)} ${self.ident}(${params})${label};`, '');
  }
  out.push(...main.definitions);
  return { name: self.ident, symbol: self.label ?? self.ident, source: out.join('\n'), notes };
}

interface Unit {
  params: ValType[];
  locals: ValType[];
  results: ValType[];
  instrs: Instr[];
  ident: string;
  /** an outlined callee, defined `static` ahead of the function */
  helper: boolean;
  /** where the unit's own frame is reserved */
  own?: number;
}

interface UnitOut {
  /** the definitions of the unit's outlined callees, then its own */
  definitions: string[];
  /** the types left on the stack where the body falls off its end */
  leftover: ValType[];
  /** the types the body pops below its start, bottom of the stack first */
  underflow: ValType[];
}

/** `if (n) { __builtin_memmove(d, s, n); }` → the call alone: LLVM lowers memory.copy and memory.fill
 *  behind a test of the length of its own, which the wasm's `if` is (wasm-opt folded the source's
 *  test, if any, into it); spelled out, LLVM rewrites the test (a length `end - begin` becomes
 *  `end != begin`) and keeps both. */
function unguardMemOps(body: Line[]): Line[] {
  const out: Line[] = [];
  for (let i = 0; i < body.length; i++) {
    const [open, call, close] = [body[i], body[i + 1], body[i + 2]];
    const n = /^if \(([lpt]\d+)\) \{$/.exec(open.text)?.[1];
    if (
      n !== undefined &&
      call !== undefined &&
      close !== undefined &&
      call.ind === open.ind + 1 &&
      close.ind === open.ind &&
      close.text === '}' &&
      /^__builtin_mem(move|set)\(.*\);$/.test(call.text) &&
      call.text.endsWith(`, ${n});`)
    ) {
      out.push({ ...call, ind: open.ind });
      i += 2;
      continue;
    }
    out.push(open);
  }
  return out;
}

const APP = '-O2 -fPIC -std=c++20 -DNDEBUG';
const ECOURNA =
  '-O3 -fexceptions -std=c++20 -DNDEBUG -fno-strict-aliasing -fno-delete-null-pointer-checks -fno-strict-overflow -fPIC';

/** The vota build's compile flags per ledger component (urna docs/reference/toolchain.md §3); libc++,
 *  libc++abi and unattributed functions get the application's. A copy for the lifted file's header and for
 *  lift-eval: urna's tools/round/try-tu.sh reads the flags from src/Makefile, including the per-directory
 *  -fno-PIC of ecourna's non-PIC units that a component cannot express. */
export const COMPONENT_FLAGS: Readonly<Record<string, string>> = {
  vota: APP,
  'vota-web': APP,
  comum: APP,
  simulador: APP,
  'asn1-modules': APP,
  utf8cpp: APP,
  rapidxml: APP,
  boost: APP,
  api: APP,
  ecourna: ECOURNA,
  ASN1: '-Os -fno-exceptions -std=c++17 -DNDEBUG',
  sqlite: '-O3 -fPIC',
  openssl: '-O3 -fPIC',
  '7zip': '-O3 -D_7ZIP_ST -DNDEBUG',
  RHVoice: '-O3 -std=c++14 -DNDEBUG',
};

export function componentFlags(component: string | undefined): string {
  return (component === undefined ? undefined : COMPONENT_FLAGS[component]) ?? APP;
}

// ---------------------------------------------------------------------------------------------
// inlined callees

const immValue = (i: Instr): number => (i.imms[0] as { value: number } | undefined)?.value ?? -1;

function synthetic(name: string, value?: number, role: 'local' | 'func' = 'local'): Instr {
  return {
    offset: -1,
    end: -1,
    opcode: -1,
    name,
    imms: value === undefined ? [] : [{ kind: 'u32', value, at: -1, role }],
  };
}

/** Each instruction's successors in the control flow of a body. */
function successors(instrs: readonly Instr[]): number[][] {
  const n = instrs.length;
  const endOf = new Int32Array(n).fill(-1);
  const armOf = new Int32Array(n).fill(-1);
  {
    const open: number[] = [];
    for (let i = 0; i < n; i++) {
      const name = instrs[i].name;
      if (name === 'block' || name === 'loop' || name === 'if' || name === 'try') {
        open.push(i);
      } else if ((name === 'else' || name === 'catch' || name === 'catch_all') && open.length > 0) {
        const o = open[open.length - 1];
        if (armOf[o] < 0) {
          armOf[o] = i;
        }
      } else if (name === 'end' || name === 'delegate') {
        const o = open.pop();
        if (o !== undefined) {
          endOf[o] = i;
        }
      }
    }
  }
  const succ: number[][] = [];
  const open: number[] = [];
  const target = (d: number): number[] => {
    const c = open[open.length - 1 - d];
    if (c === undefined) {
      return [];
    }
    return [instrs[c].name === 'loop' ? c : endOf[c]];
  };
  for (let i = 0; i < n; i++) {
    const ins = instrs[i];
    const next = i + 1 < n ? [i + 1] : [];
    switch (ins.name) {
      case 'block':
      case 'loop':
        open.push(i);
        succ.push(next);
        break;
      case 'try':
        open.push(i);
        succ.push(armOf[i] >= 0 ? [...next, armOf[i] + 1] : next);
        break;
      case 'if':
        open.push(i);
        succ.push([...next, armOf[i] >= 0 ? armOf[i] + 1 : endOf[i]]);
        break;
      case 'else':
      case 'catch':
      case 'catch_all': {
        const o = open[open.length - 1];
        succ.push(o === undefined ? [] : [endOf[o]]);
        break;
      }
      case 'end':
      case 'delegate':
        open.pop();
        succ.push(next);
        break;
      case 'br':
        succ.push(target(immValue(ins)));
        break;
      case 'br_if':
        succ.push([...next, ...target(immValue(ins))]);
        break;
      case 'br_table': {
        const l = ins.imms[0];
        succ.push(l?.kind === 'labels' ? [...l.labels, l.default].flatMap(target) : []);
        break;
      }
      case 'return':
      case 'unreachable':
      case 'throw':
      case 'rethrow':
      case 'throw_ref':
      case 'return_call':
      case 'return_call_indirect':
        succ.push([]);
        break;
      default:
        succ.push(next);
    }
  }
  return succ.map((xs) => xs.filter((x) => x >= 0));
}

/** The locals live before each instruction, as bitsets. */
function liveness(instrs: readonly Instr[], succ: readonly number[][], nLocals: number): Uint32Array[] {
  const words = Math.max(1, Math.ceil(nLocals / 32));
  const n = instrs.length;
  const live = Array.from({ length: n + 1 }, () => new Uint32Array(words));
  for (let changed = true; changed;) {
    changed = false;
    for (let i = n - 1; i >= 0; i--) {
      const ins = instrs[i];
      const acc = new Uint32Array(words);
      for (const s of succ[i]) {
        for (let w = 0; w < words; w++) {
          acc[w] |= live[s][w];
        }
      }
      const l = immValue(ins);
      if (ins.name === 'local.set' || ins.name === 'local.tee') {
        acc[l >> 5] &= ~(1 << (l & 31));
      } else if (ins.name === 'local.get') {
        acc[l >> 5] |= 1 << (l & 31);
      }
      for (let w = 0; w < words; w++) {
        if (acc[w] !== live[i][w]) {
          live[i] = acc;
          changed = true;
          break;
        }
      }
    }
  }
  return live;
}

/** A stretch of a body that a callee wasm-opt inlined after LLVM ran: from the reservation of its
 *  stack frame to its release, or the block that starts with the reservation. */
export interface InlinedRegion {
  start: number;
  end: number;
  /** the region is the block at `start`, whose branches to itself return */
  block: boolean;
  /** where the callee's body starts: a block region's straight-line head before it (the
   *  parameters wasm-opt sets) stays with the caller */
  body: number;
  /** the locals it reads that are live on entry, in order */
  params: number[];
  /** the local it writes that is live after it */
  out: number | null;
}

/** instructions that neither branch nor open or close a construct */
const STRAIGHT = /^(local\.|global\.get|[if](32|64)\.(const|load|add|sub|mul|and|or|xor|shl|shr_[su]|eqz?|ne))/;

const isLocalOp = (name: string): boolean => name === 'local.get' || name === 'local.set' || name === 'local.tee';

/** The regions of `instrs` that reserve a stack frame after the function's own (at `own`) and release it
 *  before they end, entered and left only at their ends, with at most one local live out of them. */
export function inlinedRegions(instrs: readonly Instr[], nLocals: number, own = 0): InlinedRegion[] {
  const n = instrs.length;
  const reserves: { s: number; x: number; k: number }[] = [];
  for (let i = own + 1; i + 4 < n; i++) {
    const [a, b, c, d, e] = instrs.slice(i, i + 5);
    if (
      a.name === 'global.get' &&
      immValue(a) === STACK_POINTER &&
      b.name === 'i32.const' &&
      c.name === 'i32.sub' &&
      d.name === 'local.tee' &&
      e.name === 'global.set' &&
      immValue(e) === STACK_POINTER
    ) {
      reserves.push({ s: i, x: immValue(d), k: immValue(b) });
    }
  }
  if (reserves.length === 0) {
    return [];
  }
  const endOf = new Int32Array(n).fill(-1);
  {
    const open: number[] = [];
    for (let i = 0; i < n; i++) {
      const name = instrs[i].name;
      if (name === 'block' || name === 'loop' || name === 'if' || name === 'try') {
        open.push(i);
      } else if (name === 'end' || name === 'delegate') {
        const o = open.pop();
        if (o !== undefined) {
          endOf[o] = i;
        }
      }
    }
  }
  let succ: number[][] | null = null;
  let live: Uint32Array[] | null = null;
  const isLive = (at: number, l: number): boolean => {
    succ ??= successors(instrs);
    live ??= liveness(instrs, succ, nLocals);
    return (live[at][l >> 5] & (1 << (l & 31))) !== 0;
  };
  /** the locals some path from `start` writes before reaching `exit` */
  const writtenOnTheWay = (start: number, exit: number): Set<number> => {
    succ ??= successors(instrs);
    const seen = new Map<number, Set<number>>([[start, new Set()]]);
    const work = [start];
    while (work.length > 0) {
      const i = work.pop()!;
      if (i === exit) {
        continue;
      }
      const w = new Set(seen.get(i));
      const ins = instrs[i];
      if (ins.name === 'local.set' || ins.name === 'local.tee') {
        w.add(immValue(ins));
      }
      for (const t of succ[i]) {
        if (t < start || t > exit) {
          continue;
        }
        const have = seen.get(t);
        if (have === undefined) {
          seen.set(t, w);
          work.push(t);
        } else if ([...w].some((l) => !have.has(l))) {
          for (const l of w) {
            have.add(l);
          }
          work.push(t);
        }
      }
    }
    return seen.get(exit) ?? new Set();
  };
  const out: InlinedRegion[] = [];
  let taken = -1;
  for (const r of reserves) {
    if (r.s <= taken) {
      continue;
    }
    let release = -1;
    for (let i = r.s + 5; i + 3 < n; i++) {
      const ins = instrs[i];
      if ((ins.name === 'local.set' || ins.name === 'local.tee') && immValue(ins) === r.x) {
        break;
      }
      const [b, c, d] = instrs.slice(i + 1, i + 4);
      if (
        ins.name === 'local.get' &&
        immValue(ins) === r.x &&
        b.name === 'i32.const' &&
        immValue(b) === r.k &&
        c.name === 'i32.add' &&
        d.name === 'global.set' &&
        immValue(d) === STACK_POINTER
      ) {
        release = i + 3;
      }
    }
    if (release < 0) {
      continue;
    }
    // wasm-opt sets an inlined callee's parameters at the top of the block it wraps the body in
    let b = r.s - 1;
    while (b > 0 && STRAIGHT.test(instrs[b].name)) {
      b--;
    }
    const opener = instrs[b];
    const block =
      opener.name === 'block' &&
      endOf[b] > release &&
      opener.imms[0]?.kind === 'blocktype' &&
      opener.imms[0].value.kind !== 'type';
    const start = block ? b : r.s;
    const end = block ? endOf[b] : release;
    // entered and left only at its ends; inside a block region a branch to the block returns
    let depth = 0;
    let closed = true;
    const spWrites: number[] = [];
    for (let i = start; i <= end && closed; i++) {
      const ins = instrs[i];
      switch (ins.name) {
        case 'block':
        case 'loop':
        case 'if':
        case 'try':
          depth++;
          break;
        case 'end':
        case 'delegate':
          depth--;
          closed = depth >= 0;
          break;
        case 'else':
        case 'catch':
        case 'catch_all':
          closed = depth > (block ? 1 : 0);
          break;
        case 'br':
        case 'br_if':
          closed = immValue(ins) < depth;
          break;
        case 'br_table': {
          const l = ins.imms[0];
          closed = l?.kind === 'labels' && [...l.labels, l.default].every((d) => d < depth);
          break;
        }
        case 'return':
        case 'return_call':
        case 'return_call_indirect':
          closed = false;
          break;
        case 'global.set':
          if (immValue(ins) === STACK_POINTER) {
            spWrites.push(i);
          }
          break;
        default:
          break;
      }
    }
    if (!closed || depth !== 0) {
      continue;
    }
    // the stack pointer moves only by this frame and by frames reserved and released inside it
    const nested = spWrites.filter((i) => i !== r.s + 4 && i !== release);
    if (nested.length % 2 !== 0) {
      continue;
    }
    const used = new Set<number>();
    for (let i = r.s; i <= end; i++) {
      if (isLocalOp(instrs[i].name)) {
        used.add(immValue(instrs[i]));
      }
    }
    const after = end + 1;
    const outs = [...writtenOnTheWay(r.s, after)].filter((l) => isLive(after, l));
    if (outs.length > 1 || (outs.length === 1 && block && blockResults(instrs[start]) > 0)) {
      continue;
    }
    const params = [...used].filter((l) => isLive(r.s, l)).sort((a, b) => a - b);
    out.push({ start, end, block, body: r.s, params, out: outs[0] ?? null });
    taken = end;
  }
  return out;
}

function blockResults(i: Instr): number {
  const bt = i.imms[0];
  if (bt?.kind !== 'blocktype') {
    return 0;
  }
  return bt.value.kind === 'empty' ? 0 : 1;
}

// ---------------------------------------------------------------------------------------------
// frame objects

const TOKEN = /\u27e6([EIA]):(\w+):(-?\d+)(?::(\d+):(\d+))?\u27e7/g;

/** A placeholder for an address in a frame, resolved once every use of the frame is known: `E` an
 *  address taken as a value at the start of an object, `I` one taken inside an object, `A` the
 *  address of a load or store of `1 << natural` bytes at memarg alignment `align` (log2). */
function frameToken(
  kind: 'E' | 'I' | 'A',
  frame: string,
  off: number,
  mem?: { natural: number; align: number },
): string {
  return `\u27e6${kind}:${frame}:${off}${mem === undefined ? '' : `:${mem.natural}:${mem.align}`}\u27e7`;
}

interface FrameInfo {
  name: string;
  size: number;
  atLeast: number;
  atMost: number;
  adds: number[];
  access: number;
  /** `[offset, length]` of each constant-length fill or copy through the frame */
  spans: [number, number][];
}

/** The local objects a frame splits into, and the spelling of every frame address. An address
 *  taken as a value starts an object: LLVM keeps one alloca's address per use, where offsets into
 *  one array are shared and carried in locals. The objects are declared top of the frame first,
 *  which is the order LLVM lays allocas out downward, each sized to reach the next, so every one
 *  lands at its wasm offset. A frame nothing addresses, and the unused bottom of one past 16 bytes,
 *  are kept by an empty `asm` reading them, as the target reserves them. */
function frameObjects(
  frames: readonly FrameInfo[],
  body: readonly Line[],
): { decls: string[]; notes: string[]; spell: (text: string) => string } {
  const uses = new Map<string, { kind: string; off: number; line: number; natural?: number; align?: number }[]>();
  for (const [line, l] of body.entries()) {
    for (const m of l.text.matchAll(TOKEN)) {
      const list = uses.get(m[2]) ?? [];
      list.push({
        kind: m[1],
        off: Number(m[3]),
        line,
        ...(m[4] === undefined ? {} : { natural: Number(m[4]), align: Number(m[5]) }),
      });
      uses.set(m[2], list);
    }
  }
  /** the function's own frame, and the frames of callees inlined into it after LLVM ran */
  const own: string[] = [];
  const inlined: string[] = [];
  const keep: { name: string; why: string }[] = [];
  const notes: string[] = [];
  const objectsOf = new Map<string, { name: string; start: number }[]>();
  const lowbit = (x: number): number => (x === 0 ? Infinity : x & -x);
  for (const f of frames) {
    const decls = f === frames[0] ? own : inlined;
    const fu = uses.get(f.name) ?? [];
    if (fu.length === 0) {
      const align = Math.min(16, Math.max(f.atLeast, Math.min(f.atMost, 4)));
      decls.push(`alignas(${align}) uint8_t ${f.name}[${f.size}];`);
      keep.push({ name: f.name, why: 'stack the target reserves and nothing here reads' });
      notes.push('unused frame');
      continue;
    }
    const inSpan = (off: number): boolean => f.spans.some(([s, len]) => off > s && off < s + len);
    const starts = [
      ...new Set(fu.filter((u) => u.kind === 'E' && u.off >= 0 && u.off < f.size && !inSpan(u.off)).map((u) => u.off)),
    ].sort((a, b) => a - b);
    if (starts[0] !== 0 && (starts.length === 0 || fu.some((u) => u.off < starts[0]))) {
      starts.unshift(0);
    }
    const objs = starts.map((start, k) => ({
      name: starts.length === 1 && start === 0 ? f.name : `${f.name}_${start}`,
      start,
      end: starts[k + 1] ?? f.size,
    }));
    const containing = (off: number) => objs.findLast((o) => o.start <= off) ?? objs[0];
    const decl: string[] = [];
    for (const o of objs) {
      let atLeast = 1;
      let atMost = 16;
      let widest = 0;
      if (o.start === 0) {
        atLeast = f.atLeast;
        for (const k of f.adds) {
          if (!starts.includes(k)) {
            atMost = Math.min(atMost, 2 ** Math.floor(Math.log2(k)));
          }
        }
      }
      for (const u of fu) {
        if (u.natural === undefined || u.align === undefined || containing(u.off) !== o) {
          continue;
        }
        widest = Math.max(widest, 1 << u.natural);
        // an under-aligned access at a well-aligned offset says how aligned the object is
        if (u.align < u.natural && lowbit(u.off - o.start) > 1 << u.align) {
          atMost = Math.min(atMost, 1 << u.align);
        }
      }
      const placed = Math.min(16, lowbit(o.start));
      const align = Math.min(placed, Math.max(atLeast, Math.min(atMost, Math.max(4, widest))));
      decl.unshift(`alignas(${align}) uint8_t ${o.name}[${o.end - o.start}];`);
    }
    decls.push(...decl);
    // the split is a guess at where one object ends: an object might be a member of one below it,
    // which a call given that object's address may read or write. Once its own address is taken
    // LLVM assumes as much, so only an object read or written before that, while an object below
    // it has already escaped, escapes at entry
    for (const o of objs.slice(1)) {
      const mine = fu.filter((u) => containing(u.off) === o);
      const taken = Math.min(...mine.filter((u) => u.kind !== 'A').map((u) => u.line));
      const below = fu.some((u) => u.kind !== 'A' && u.off < o.start && u.line < taken);
      if (below && mine.some((u) => u.kind === 'A' && u.line < taken)) {
        keep.push({ name: o.name, why: `may be part of ${objs[objs.indexOf(o) - 1].name}` });
        notes.push('frame object escapes at entry');
      }
    }
    if (starts[0] >= 16) {
      decls.push(`alignas(16) uint8_t ${f.name}_pad[${starts[0]}];`);
      keep.push({ name: `${f.name}_pad`, why: 'stack the target reserves and nothing here reads' });
      notes.push('unused frame');
    }
    objectsOf.set(f.name, objs);
  }
  // a jump past a declaration leaves it without lifetime markers, so LLVM's stack colouring cannot
  // fold two of the function's objects into one slot: their offsets are the target's already. The
  // inlined frames are separate reservations below the function's own in the target; declared
  // first, they sit above it here, which leaves the function's objects at their wasm offsets
  const decls = own.length > 1 ? [...inlined, 'goto frame_objects;', ...own, 'frame_objects:;'] : [...inlined, ...own];
  for (const k of keep) {
    decls.push(`__asm__ volatile("" :: "r"(${k.name})); /* wasm: ${k.why} */`);
  }
  const spell = (text: string): string =>
    text.replace(TOKEN, (_, _kind: string, frame: string, offS: string) => {
      const objs = objectsOf.get(frame);
      const off = Number(offS);
      if (objs === undefined) {
        return off === 0 ? frame : `(${frame} ${off > 0 ? '+' : '-'} ${Math.abs(off)})`;
      }
      const o = objs.findLast((x) => x.start <= off) ?? objs[0];
      const d = off - o.start;
      return d === 0 ? o.name : `(${o.name} ${d > 0 ? '+' : '-'} ${Math.abs(d)})`;
    });
  return { decls, notes, spell };
}

function sameType(a: FuncType, b: FuncType): boolean {
  return a.params.join() === b.params.join() && a.results.join() === b.results.join();
}

const SLOTS = new WeakMap<Module, Map<number, number>>();

/** Table 0's slot → function index, from its active element segments. */
function tableSlots(m: Module): Map<number, number> {
  const cached = SLOTS.get(m);
  if (cached !== undefined) {
    return cached;
  }
  const slots = new Map<number, number>();
  for (const e of m.elems) {
    if (e.mode !== 'active' || e.tableIndex !== 0 || e.offset?.op !== 'i32.const') {
      continue;
    }
    const base = Number(e.offset.value);
    e.funcIndices.forEach((f, j) => {
      if (f >= 0) {
        slots.set(base + j, f);
      }
    });
  }
  SLOTS.set(m, slots);
  return slots;
}

function importedGlobals(m: Module): number {
  return m.imports.filter((i) => i.kind === 'global').length;
}

/** Index → real name for every function of `m` that has one: import names, export names, then the
 *  ledger's (a later source never overrides an earlier one). A ledger name whose symbol (through
 *  `mangled`, as `cName` spells it) another index already has is left out, so two lifted
 *  translation units never define one symbol. */
export function liftNames(
  m: Module,
  sources: {
    imports?: Readonly<Record<string, string>> | null;
    exports?: Readonly<Record<string, string>> | null;
    ledger?: ReadonlyMap<number, string>;
    mangled?: ReadonlyMap<string, string>;
  },
): Map<number, string> {
  const out = new Map<number, string>();
  let k = 0;
  for (const imp of m.imports) {
    if (imp.kind !== 'func') {
      continue;
    }
    const real = sources.imports?.[imp.name];
    if (real !== undefined) {
      out.set(k, real);
    }
    k++;
  }
  for (const e of m.exports) {
    if (e.kind !== 'func' || out.has(e.index)) {
      continue;
    }
    const real = sources.exports?.[e.name];
    if (real !== undefined && real !== '__indirect_function_table' && real !== 'memory') {
      out.set(e.index, real);
    }
  }
  const symbolOf = (name: string): string => {
    const c = cName(name, sources.mangled);
    return c.label ?? c.ident;
  };
  const taken = new Set([...out.values()].map(symbolOf));
  for (const [idx, name] of [...(sources.ledger ?? [])].sort((a, b) => a[0] - b[0])) {
    const sym = symbolOf(name);
    if (!out.has(idx) && !taken.has(sym)) {
      out.set(idx, name);
      taken.add(sym);
    }
  }
  return out;
}
