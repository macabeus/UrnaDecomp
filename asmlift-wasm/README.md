# @asmlift/wasm

A WebAssembly **matching toolkit**: parse a module, decode its functions, give every function a
relocation-proof fingerprint, diff two functions objdiff-style, match a whole module against a
rebuild, and compile-and-rank candidate C++ sources against one target function with core's
`rankBy`. Built for the `vota_web_wasm.wasm` reproduction (14,343 functions, 7.7 MB); no runtime
dependency beyond `@asmlift/core` (`rankBy`) and `@match-kit/compiler` (the compile template).

```sh
cd asmlift-wasm && pnpm install    # once (a pnpm workspace: this package, benchmark/ and playground/)
pnpm wasm <command> …                # from asmlift-wasm/; module paths below are relative to it
pnpm bench <command> …               # the lifter benchmark, benchmark/README.md
pnpm playground                      # the web app over the target and the benchmark, playground/README.md
```

| command                                                                                                     | does                                                                                                                                                                              |
| ----------------------------------------------------------------------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `parse <wasm> [--json] [--functions]`                                                                       | section counts and offsets; `--functions` prints one JSON record per function                                                                                                     |
| `disasm <wasm> <fn>`                                                                                        | the instruction stream, raw immediates, body-relative offsets                                                                                                                     |
| `fingerprint <wasm> [--depth N] [<fn>]`                                                                     | `index size fingerprint addr shape` (TSV) for one or every function                                                                                                               |
| `score <wasmA> <fnA> <wasmB> <fnB> [--depth N] [--full] [--json]`                                           | row diff of B's function against A's; A is the target                                                                                                                             |
| `census <wasmA> <wasmB> [--depth N] [--json] [--unpaired N] [--pairs]`                                     | pair every function of A with B by fingerprint, then addr, then shape; size histogram                                                                                             |
| `fn <wasm> <fn> [--depth N] [--ledger <dir>] [--owners <cls2.json>]`                                        | one function: disassembly beside its normalised rows, exports, table slots, a folded readout, and from the ledger its strings, callee owners, vtable slot, callers and best pair  |
| `bundle <wasm> --name <n> [--file <indices>] [--no-disasm] <fn>…`                                           | one Markdown file (`<ledger>/bundles/<n>.md`): a header table over the listed functions, then each one's `fn` readout                                                             |
| `ledger <target.wasm> [-o functions.tsv] <label>=<cand.wasm>[@<twin.wasm>]…`                                | the census: one row per target function with its owner and best pair over the candidate modules; `@twin` names a plain link through its `--emit-symbol-map` twin                 |
| `progress [--ledger <dir>] [--against <prev.tsv>] [--by owner\|component] [--snapshot [label]]`             | the census tiers per owner or component as a Markdown table with the delta since the previous `<ledger>/history/<stamp>.tsv` snapshot, and the functions that moved              |
| `targets <target.wasm> --tsv <new.tsv> [--against <prev.tsv>] [--file <indices>] <label>=<cand.wasm>… <fn>…` | per listed target function: previous → new kind/score, the best candidate function by name, and the first differing rows                                                          |
| `import-names <glue.js> [-o names.json]`                                                                    | `{ minified import field: real name }` from the glue's `wasmImports` table                                                                                                        |
| `rank <target.wasm> <fn> --extract <sel> [--template '<sh>'] [--image …] [--flags '…'] <src>…`              | compile each source with the pinned toolchain, extract the function, score, rank with core `rankBy`                                                                               |
| `lift <wasm> (<fn>…\|--all) [-o <dir>] [--names <functions.tsv>] [--mangled <m.txt>:<d.txt>] [--check]`     | rough C++ per function (`<dir>/<index>.cpp`) that em++ compiles; `--all` lifts every function and reports throws; `--check` compiles each file in the pinned image                |
| `lift-eval <wasm> [--label <l>] [--size N] [--seed N] [--sample <tsv>] [--against <dir>]`                   | lift, compile and score a stratified sample; `report.md`, `results.tsv`, `sample.tsv` under `<ledger>/lift-eval/<label>/`                                                         |
| `pnpm bench setups [--check]`                                                                               | the compile-setup registry (`benchmark/dataset/setups.json`) checked against `src/Makefile`, the library builds and emscripten                                                    |
| `pnpm bench select [--per N] [--write]` / `pnpm bench add <index>…`                                         | what a re-selection of the frozen dataset (`benchmark/dataset/rows.json`) would change, `--write` makes it a new version; or add a row by hand                                    |
| `pnpm bench run [--label <l>] [--setup k,…]` / `pnpm bench diff <a> <b>`                                    | lift, compile at each row's own setup flags and score; `benchmark/results/<l>.{json,md}`; compare two runs                                                                        |
| `splat-split <wasm> -o <dir> [--names <functions.tsv>] [--attribution <tsv>] [--no-wat]`                    | the module as pieces (function bodies, data segments, manifest), then proves they reassemble to the input's bytes                                                                 |
| `splat-assemble <dir> -o <wasm> [--substitute <dir\|<index>[-<name>].bin>]…`                                | the module from the pieces alone, with replacement bodies for some functions                                                                                                      |
| `splat-symbols <target> <plain> [-o <symbol_addrs.txt>]`                                                    | the target's symbol file (function indices and data addresses → names) from our link's maps and the evidence                                                                      |
| `splat-maps <target> <plain> (--fn <ours> \| --data <addr> \| --target-fn <n> \| --summary)`                | one entry of the symbol maps                                                                                                                                                      |
| `splat-remap <target> <plain> <ours> [--against <target fn>] [--kinds f \| --no-kinds]`                     | one body of ours re-pointed into the target, and whether it is the target body                                                                                                    |
| `splat-sym-coverage <target> <plain> [--smoke N] [--kinds f \| --no-kinds]` / `splat-twin-check <plain> <twin>` | symbol coverage; the `--emit-symbol-map` twin against the plain link                                                                                                       |
| `splat-scan <target> <plain> [-o scan.tsv] [--match [<matches>]] [--kinds f \| --no-kinds]`                 | every target function against our re-pointed candidate; `--match` appends new matches to the match list                                                                           |
| `splat-build <target> <plain> [--matches f] [-o out.wasm] [--expect <sha256>] [--kinds f \| --no-kinds] [--max-kind-unverified n]` | the split with our bytes for every match; fails on a match that does not verify, or on more than n kind-unverified matches; prints the meter, the kind-unverified matches and the review warnings |
| `splat-kinds <plain> [--shifted <dir>] [-o kinds.json] [--matches f]`                                       | the operand-kind table: which constants of our link are addresses, table slots or integers, from a relink with data and table moved                                             |

`<fn>` is a function index (function index space: imports first, so the target's first defined
function is 136), `export:<name>` (linked module; the real name, resolved through the glue when the
module's exports are minified) or `sym:<name>` (relocatable object).

Every command that normalises takes `--import-names <names.json>` for module A (the target) and
`--import-names-b <names.json>` for module B. Without them, the glue emcc wrote beside a module
(`x.wasm` → `x.js`) is read when it exists, for import AND export names; a symbol map beside it
(`x.js.symbols`, from `--emit-symbol-map`) names B's functions in `census --pairs` and `ledger`.

## What is what

**parse** (`parse.ts`) reads the binary into records: types, imports, exports, globals, the elem
segment (table contents as function indices), data segments (offset + bytes) and per function
`{index, typeIndex, params, results, locals, bodyOffset, size, bodyBytes, codeStart}`. Bodies stay
as byte views; nothing is decoded until asked. On a relocatable object it also reads the
`linking` symbol table and every `reloc.*` section. The target parses in ~10 ms
(`cli.ts parse`, 1.1 s wall including tsx start-up).

**disasm** (`disasm.ts`) decodes a body into `{offset, end, opcode, name, imms[]}`; every
immediate carries the byte offset it was read from (relocations point at immediates). MVP,
sign-ext, nontrapping float-to-int, bulk memory, reference types, multivalue block types, tail
calls and legacy EH decode; SIMD (`0xfd`) and atomics (`0xfe`) throw a `DecodeError` naming the file
offset. All 14,343 target functions decode (2,710,054 instructions, 225 ms) and every mnemonic and
file offset agrees with `wasm-objdump -d` on the sampled functions (`test/disasm.test.ts`).

**normalize + fingerprint** (`normalize.ts`). One row per instruction with relocatable operands
replaced by what they refer to:

| operand                                | becomes                                                       |
| -------------------------------------- | ------------------------------------------------------------- |
| `call`/`return_call`/`ref.func` f      | `import:<real name>` · `self` · `fn:<fingerprint of f>`       |
| `call_indirect` type                   | the type's structure `type:(i32 i32)->(i32)`, never its index |
| `i32.const a`, a inside a data segment | `data:<sha256₁₆ of the segment bytes>+<a − segment start>`    |
| `memory.init`/`data.drop` d            | `data:<hash of segment d>`                                    |
| block types                            | structural (`(result i32)`, `(type (i32)->(i32 i32))`)        |
| memarg, locals, labels, globals        | kept as written                                               |

`fn:<fingerprint>` makes a fingerprint depend on its callees', which recursion makes circular, so
it is an **iterated hash** (Weisfeiler–Lehman): round 0 hashes the rows with every callee as
`fn:?`; round r hashes them with round r−1's fingerprints. `--depth N` (default 2) gives a hash
that identifies a function by its body and the bodies within N calls; `--depth 0` is the body
alone, for comparing against an object whose callees are symbol names. The hash covers the
signature and locals too. Two weaker hashes sit under it:

- **addr** blinds every address and nothing else: an `i32.const` ≥ 1024 (emscripten's
  `GLOBAL_BASE`; a data or a bss address, and bss has no segment a data symbol could name), a memarg
  offset ≥ 1024 (an address binaryen folded into a load), a data symbol; callees compare by addr.
  This is what two links of one library at different layouts share: dlmalloc `free` in the target
  and in the harness probe differ only in 35 such operands. A genuine integer ≥ 1024 is blinded too.
- **shape** drops every constant (`i32.const`, `i64.const`, floats, memarg offsets, data symbols)
  and compares callees by shape, so it survives a moved string, a changed struct layout or a callee
  differing by one constant.

Imports are compared by their **real** names (`import:__cxa_throw`, the module dropped: `fd_write`
is always wasi, the rest env). At -O2+ emcc minifies the fields to `a.o`, and the real names survive
only in the glue's `var wasmImports={o:___cxa_begin_catch,…}` table (`imports.ts`): strip exactly one
leading `_` from the JS name (`___cxa_throw` → `__cxa_throw`, `_fd_write` → `fd_write`, EM_JS
`js_init` and `invoke_vii` stay). Exports are minified the same way and recovered from the glue's
`_votaInit=Module["_votaInit"]=wasmExports["Eb"]` assignments; the bare `wasmExports["Db"]()` call is
`__wasm_call_ctors`.

Table indices are deliberately **not** symbolised from a bare `i32.const`: emscripten's table
starts at 1 and its data at 1024, so every small integer is a plausible function pointer and vice
versa. Only the object path (relocations) can tell them apart.

**score** (`score.ts`). Myers' O(ND) diff over the two row arrays (common prefix/suffix stripped
first). `score` = rows that differ after alignment: a deletion 1, an insertion 1, a deletion paired
with an insertion in the same hunk 1 (a _replaced_ row, flagged `operandOnly` when the opcode is the
same). 0 = identical after normalisation; `byteExact` says the raw bodies are the same bytes. Past
an edit ceiling (6,000) the middle is paired positionally and `breakdown.approximate` is set.
`renderSideBySide` prints two columns with `|` `~` `<` `>` marks, `--full` shows context rows too.

**census** (`census.ts`). Groups both modules' functions by fingerprint and pairs them by count
(identical thunks are common), then the leftovers by addr, then by shape; prints the unpaired
indices and a histogram by ⌊log₂ size⌋ with the paired byte share; `--pairs` lists every pair with
B's name from its symbol map. `census target target` pairs 14,343/14,343 by fingerprint. Against the
harness probe (103 functions, same toolchain) at depth 2: 29 fingerprint + 29 addr + 8 shape; at
depth 0: 49 + 22 + 5 (a callee that differs only by an address demotes its caller from fingerprint
to addr at depth ≥ 1). Pairing by count is positional: a hash held by three `end`-only functions on
each side pairs them in index order, which says nothing about identity — 18 of the 66 depth-2 pairs
have a body of ≤ 8 bytes, and shape pairs the probe's own 7-byte `probeExported` with an app function.

**ledger** (`ledger.ts`). For every target function: `index size uses owner component best_label
best_index best_kind best_score best_name`. `uses` counts the target's direct call sites (what
binaryen sorted the code section by); `owner` is the class map (`tools/libs/cls2.json`),
`component` the attribution ledger (`ledger/func-attribution.tsv`); the best pair over every
`<label>=<module.wasm>` candidate is the strongest of **byte** (same body bytes) > **fingerprint** >
**addr** > **shape** (with the row-diff `score`), named from the candidate's `x.js.symbols`. Under
the urna layout (`<root>/target/x.wasm`) the owners, attribution and output (`<root>/ledger/functions.tsv`)
default themselves; the summary on stderr tallies kinds per owner and sets trivial bodies (≤ 8 B) apart.

**fn** prints the disassembly beside the normalised rows, the export names (real names in
parentheses) and table slots that reach the function, and a **folded readout** (`fold.ts`): the
stack replayed into expressions, `if (i32.lt_s(l8, 0)) {`, `i32.store[l7+12] = 0;`,
`return helper(…)`. It is a reading aid (block results show as `<block result>`), not a decompiler.
With a ledger directory (`--ledger`, defaulting to `<root>/ledger` beside `<root>/target/`;
`readout.ts`) it adds the function's owner and component, its best pair from `functions.tsv`, its
callers (`callgraph.tsv`), the vtable class and slot it fills (`vtables.json`), every string an
`i32.const` points at (`strings.tsv`, with the offset into the string) and each callee's owner,
component, vtable slot and best pair. The whole ledger loads in well under a second.
`wasm-decompile` is run and its block for the function printed when it is on PATH; Homebrew's wabt
does not ship it.

## The decompile loop: bundle, ledger names, progress, targets

**bundle** (`report.ts`) writes what `fn` prints for every listed function (arguments, or `--file` with one
index per line — a ledger TSV works, the first column is taken) into one Markdown file under
`<ledger>/bundles/<name>.md` (or `--out-dir`), behind a header table: index, size, uses, owner,
component, vtable class/slot, best pair so far. The module is normalised once for the whole file, so
a 50-function bundle takes about as long as one `fn` (measured 4.2–4.8 s wall for 50 ecourna
functions, 54 KB of body; `fn` alone is 4.3 s). The full readout is large (2.1 MB for those 50);
`--no-disasm` leaves out the two-column disassembly and keeps the folded readout and the tables
(349 KB). `fn` prints through the same renderer. `fn` lists the table slot an `invoke_*`
thunk takes as its first argument among the strings referenced (any `i32.const` in the data range is read as a
pointer) and gives only the import as the callee; `lift` names the function behind the slot in a comment,
`invoke_iii(6708 /* &f339 */, …)`.

**ledger `label=plain.wasm@twin.wasm`**. A plain `-Oz` link has no names and its `--emit-symbol-map`
twin has a different function order, so the byte tier needs the plain module and the names live in
the twin. With `@twin` the ledger runs a fingerprint census between the two (same bodies, different
indices) and carries the twin's names over to the plain functions. Measured on a 10,212-function skeleton link:
9,996 of its functions get a name (the rest pair by addr/shape or not at all — bodies that differ between the two
links because the symbol map changed inlining order), and `best_name` is filled for 6,438 of the
6,539 rows the skeleton matches; a separate `skeleton-named` candidate (the twin alone) names 2. Two
identical bodies pair positionally, so a thunk's name is one of its identical twins'. 21 s wall for
probe + skeleton@twin.

**progress** (`progress.ts`) reads `functions.tsv` (default `<ledger>/functions.tsv`, `--tsv` otherwise)
and the newest `<ledger>/history/*.tsv` (or `--against <prev.tsv>`), prints one Markdown table per
`--by owner` (cls2 class, default) or `--by component` (attribution): functions, count per kind with
the delta in parentheses, the byte-exact share by count and by bytes; then the functions that moved
kind or score, improvements first (`--moves N`, 0 hides the list). `--snapshot [label]` copies the
current ledger into `history/<stamp>[-label].tsv` after printing, so the next run's delta is against
it. `tools/round/census.sh` in urna does exactly that at the end of every census.

**targets** prints, for each listed target function (arguments or `--file`): `<index> <size> <owner>/
<component>: <prev kind/score> → <new kind/score> <label>:<index> <name>`, and when the new pair is an
addr or shape pair against a candidate module given as `label=path.wasm`, the `score` diff limited to
`--rows N` differing rows. It is what `try-tu.sh` prints for the target functions a candidate TU is expected to
produce (`--targets`).

## lift: rough C++ to start a function from

In short: `lift` writes one `<index>.cpp` per function that `em++ -c` compiles at the function's
component flags, with that compile line in its header comment. It is a starting point to refine,
not a match. The workarounds that keep it compiling and close to clang's own lowering:

- `goto` where a branch is not a plain `if`/`while`/`do … while`/`switch`;
- every callee `extern "C"` under its mangled symbol (a ledger name without a known mangled
  spelling is a readable identifier that will not link; no name at all is `f<index>`), an
  indirect call a cast of the table slot to a function pointer, an `invoke_*` thunk a call to it;
- every load and store a typed `*(T*)((char*)base + off)`, every store `volatile`, integer
  arithmetic unsigned so it wraps;
- the shadow-stack frame as local objects (`frame_48`), `__builtin_alloca` for a dynamic one, a
  callee wasm-opt inlined outlined again as a `static noinline` helper;
- empty `asm volatile` statements (`KEEP_IN_ARM()`, an escape at entry, `opaque_addr`) where LLVM
  would otherwise merge, sink or re-align what the target kept apart.

Refining toward a match mostly means taking these back out: `volatile` where the source had
none, the escape `asm`s, the casts, the `goto`s; then giving the integers and pointers their
source types.

`liftFunction(module, index, opts)` (`lift.ts`) turns any defined function into a self-contained
C++ translation unit: `<stdint.h>`, `extern "C"` declarations of every callee and global it
touches (wasm types mapped to `int32_t`/`int64_t`/`float`/`double`), then the function under its
symbol. It never declines: a construct without a closer spelling is a compilable stand-in with a
`/* wasm: … */` comment, and `notes` lists them. Every function of the target lifts and compiles
with `em++ -c` at its component's flags (`lift --all -o <dir> --check`).

```sh
pnpm wasm lift ../target/vota_web_wasm.wasm 156 10058 -o /tmp/lifted --check
pnpm wasm lift ../target/vota_web_wasm.wasm --all
```

- **Names.** Imports and exports by the glue's real names; other functions by the ledger's
  `best_name` when the pairing is byte, fingerprint or addr (a shape pairing is often another
  function). The ledger spells C++ names demangled, so `--mangled` takes two line-aligned lists
  (`llvm-nm` and `llvm-cxxfilt` of one build's inputs; default
  `<root>/build/out/wasm/vota_web/symbols.{mangled,demangled}.txt`, which `make -C src link` writes)
  to recover the mangled symbol, which is a valid identifier: `extern "C" … _ZN7RHVoice4item8iteratormmEv(…)`
  defines the C++ symbol. A demangled name without a pair becomes a readable identifier that is not
  the symbol; a symbol that is not an identifier (`main` included) gets an `__asm__` label, and so
  does one a generated local (`t0`, `p1`, `g3`, `frame`, …) would shadow. `liftNames` leaves out a
  ledger name whose symbol another function already has, so two lifted units never define one symbol.
- **Values.** The operand stack folds into expressions. Pushing a call spills every pending value
  that reads memory or calls, and pushing a load or a global read spills every pending call, so no
  expression leaves C++ an evaluation order the wasm does not have. Loads and stores are
  `*(T*)((char*)base + off)` with the memarg's width and signedness; under strict aliasing (every
  component but ecourna) `T` is a `may_alias` typedef (`int32_ma`), since a width in the wasm says
  nothing about a source type, and an under-aligned access adds `aligned(N)` (`int64_a4`). A base
  plus a scaled index (`l4 + (l2 << 4)`) is pointer arithmetic, base first, as clang lowers
  `&base[i]`, also as a value (`l3 = (int32_t)((char*)p0 + (l6 << 2));`): an integer add puts an
  argument after the shift, and LLVM folds the cast back into one only where nothing dereferences
  it. A variable shift count is masked (`x >> (n & 31)`) as wasm masks it. Integer `+`,
  `-` and `*` are unsigned (`(int32_t)((uint32_t)a + 36u)`): wasm's wrap, and a signed overflow
  LLVM may assume away (an overflow check before a trap folds to nothing). Data addresses stay
  numeric; an under-aligned access at one passes it through `opaque_addr` (an `asm` LLVM cannot see
  through), since LLVM would raise the access's alignment to what the number shows where the
  target's symbol showed nothing. A `select` whose arm reads memory or divides evaluates both arms into temporaries
  first, as wasm does: LLVM does not speculate a load back into a select.
- **Stores are volatile** (`*(volatile int32_ma*)p = v;`). The stores of optimised code are the
  ones LLVM kept; a plain store it would merge with a neighbour (two vtable-pointer constants into
  one `i64.store`), schedule past another or drop. Refining toward a match starts by taking
  `volatile` off where the source had none.
- **Control flow.** Blocks are flat; a branch is `break`/`continue`/`return` when that statement
  lands on the same instruction, a direct-child `br_if` out of a void block nests the rest as
  `if (!c) { … }`, and the rest are `goto`s to labels at block ends. Loops are `for (;;)`,
  rewritten to `do … while (c)` or `while (c)` when the shape is one; `br_table` is a `switch` of
  branches. Locals are declared (zeroed) at the top, so no `goto` crosses an initialisation. An
  arm that leaves for a join other arms reach (`goto`, `break`, `return`, the end of an `if` arm)
  gets `KEEP_IN_ARM();`, an empty `asm volatile`, after its last call or store: SimplifyCFG sinks
  the common tail of the arms into the join, where the target keeps one copy per arm, and no
  `asm` sinks.
- **Shadow stack.** `__stack_pointer - N` is a frame of local objects, the writes that reserve or
  release it are dropped, and a non-constant subtraction from the stack pointer (also through a
  local saved at the top of the function) or from the frame base, written back to the stack
  pointer, is `__builtin_alloca`. Every offset whose address is taken as a value starts an object
  (`frame_48`) reaching to the next: LLVM rematerialises an alloca's address at each use, where it
  keeps `frame + 48` of one array in a local. The objects are declared top of the frame first (the
  order LLVM lays allocas out downward), so each lands at its wasm offset, behind a
  `goto frame_objects;` that jumps past the declarations: without lifetime markers, stack colouring
  cannot fold two of them into one slot. The bytes a constant-length `memory.fill` or
  `memory.copy` covers lie in one object, however many addresses inside it are taken. An object's alignment is the largest its offset allows,
  capped by an under-aligned access at its start (`int64_a4` there means the object is 4-aligned)
  and, for the one at 0, by how the wasm offsets the frame base (LLVM spells `frame + k` as `i32.or`
  exactly when the alignment exceeds `k`). An object read or written before its address is taken,
  while one below it has escaped, might be a member of that one, so an empty
  `__asm__ volatile("" :: "r"(frame_464))` lets it escape at entry, so LLVM does not assume a
  call given the lower address leaves it alone. The same `asm` keeps a frame nothing addresses, and
  the unused bottom of one, since the target reserves them. wasm-opt drops the address the `asm`
  takes, so the `-Oz` score does not see it.
- **Inlined callees.** A frame reserved again inside the function is a callee wasm-opt inlined
  after LLVM ran. Its region, from the reservation to the release, or the block wasm-opt wrapped it
  in (whose straight-line head, the parameter sets, stays with the caller), is lifted on its own
  as `static __attribute__((noinline)) … f9127_inlined0(…)` ahead of the function, which calls it: LLVM compiled it apart, and wasm-opt inlines a single-call static
  function back. A region qualifies when no branch leaves it but to its own end, the stack pointer
  moves inside it only by frames it also releases, and at most one local it writes is live after
  it (by liveness and the writes that reach its end): the parameters are the locals it uses that
  are live on entry, a value the caller left on the stack before the frame is a leading parameter,
  and the live local or a value left on the stack is the result. A frame that does not qualify
  stays in the function as its own objects, declared before the function's own: the target
  reserved it below them, and declared first it sits above them here, so the function's objects
  keep their wasm offsets. The `-c` object holds the
  callee apart, so only the `-Oz` score compares the whole.
- **Calls.** `call_indirect` casts the slot to a function pointer. An `invoke_*` thunk (legacy JS
  exceptions) stays a call to the import, `invoke_vi(241 /* &callee */, …)`, with the callee a
  constant slot holds named in a comment: that is the call the target makes, and clang only emits
  it from a call inside a try region. The `__THREW__` stores and loads around it stay as memory
  accesses. A callee parameter some call site sign- or zero-extends from 8 or 16 bits
  (`(int32_t)(int8_t)x`, `x & 255`) is declared that narrow type, so the caller extends it as the
  wasm ABI has it, also where clang could prove an `int`'s extension redundant. `if (n)` around a
  `memory.copy` or `memory.fill` of length `n` is dropped: LLVM lowers both behind a test of the
  length of its own, which is the one the wasm shows.
- **Branches** that discard pending calls still make them; a loop's parameters are temporaries a
  `continue` reassigns; a second function result is returned only in a `/* wasm: also returns */`
  comment (LLVM never emits one for this target).
- A dropped load (kept by wasm-opt because it may trap) is `(void)*(volatile T*)addr;`, which clang
  emits as the same load and drop.

Scoring a lifted object against the target at depth 0 (`score … sym:<symbol> --depth 0`) shows
`call fn:?` against `call sym:<name>` on every call row: depth 0 names no callee on the target
side. A linked candidate (the `rank` template) compares like with like, except that an isolated
link resolves a libc callee such as `emscripten_builtin_free` and wasm-opt then inlines it.

## lift-eval: measuring the lifter

`lift-eval` (`lift-eval.ts`) measures the lifter, one run against another. It samples about 150 target
functions with a fixed seed, stratified by the ledger's component (`func-attribution.tsv`, `unknown`
where it has none; shares proportional to the square root of each component's size, at least 3
each) and by body-size band (tiny ≤ 32 bytes, small ≤ 128, medium ≤ 512, large ≤ 2048, huge). It
lifts each one, compiles them all with `em++ -c` in one pinned container at each function's component
flags, pulls the function out of its object by symbol and scores it against the target at depth 0.
Then `optimizeLifted` (`rank.ts`) links each object alone with `wasm-ld` (exporting its symbol) and
runs the target link's two wasm-opt passes over it (`-Oz --low-memory-unused --zero-filled-memory
…`, then `--optimize-stack-ir`), and the function is scored again: the `-Oz` columns. The target
went through wasm-opt and an object did not, so the object score counts wasm-opt's rewrites
(`add` of a negative constant against `sub`, `if (result)` against `br_if`, coalesced locals) as
lift gaps; the `-Oz` score takes them out and is the one to steer by.

```sh
pnpm wasm lift-eval ../target/vota_web_wasm.wasm --label baseline
pnpm wasm lift-eval ../target/vota_web_wasm.wasm --label r1 \
  --sample ../ledger/lift-eval/baseline/sample.tsv --against ../ledger/lift-eval/baseline
```

Both sides' rows go through `canonRow` first: callees drop to the bare opcode, a data address (a
data symbol, or a constant or memarg offset ≥ 1024) is `<addr>`, the object's
`global:__stack_pointer` is `0` and its indirect-table relocation is dropped. The report has lift
throws, compile rate, exact (score 0) count and median ratio (differing rows over target rows,
compiled functions only) on both scores, per component and per size band; failures clustered by
their first em++ error with names and numbers blanked; and the score-gap classes of the optimised
functions (kind × opcode family, ranked by their mean share of the ratio, with the top opcode
pairs). `--against` compares per function on the `-Oz` score when both runs have one. The lifted
sources, objects, optimised modules and `.err` files stay in `<out>/lifted/`. An isolated function
misses what the whole-program link does to it (inlining, dead-argument removal, data addresses), so
even the `-Oz` score is a structure signal, never byte identity.

Reading a report: the compile rate should be 100% (a failure is a lifter bug; its class and first
error line are in the failures table and `<out>/lifted/<index>.cpp.err`). Then read `exact -Oz`
and `median ratio -Oz`, by size band before by component: tiny and small functions are mostly
exact, so movement shows in the medium-to-huge medians. The movement table lists the functions
whose `-Oz` score changed most against `--against`; the score-gap classes say which rows account
for the rest (`target-only`, `lift-only` or replaced, per opcode family, `operand local` being
local numbering), and the stand-ins table counts the functions carrying each workaround's note.
`results.tsv` has one row per function (`score`/`ratio` on the object, `opt_score`/`opt_ratio`
after `-Oz`), the input for a per-function comparison. A sample drawn with another `--seed` (as `--sample`, without the
indices of the steering sample) checks that a lifter change does not fit the steering sample alone.

## The benchmark

`benchmark/` (package `@asmlift/wasm-benchmark`, a workspace member depending on this one) is the
lifter's benchmark: target functions from every compile setup of the module, the TSE application and
its libraries and emscripten's system libraries, each compiled with its own setup's flags by
`evaluateLift` (a sample row's `flags` override `componentFlags`). Where lift-eval samples by the
ledger's component, the benchmark decides each row's setup from the build object that defines the
ledger's name and checks its flags against `src/Makefile`. Its README has the setups, the dataset, the
results schema (`BenchResults`) and how to add a row.

## The playground

`playground/` (package `@asmlift/wasm-playground`, MIT) is a web app with two pages. The playground
shows any target function's WAT, folded readout and lifted C++, its facts from the module, the glue
and the ledger, its callers, callees and strings, its benchmark result when it is a row, and the
local commands that compile and score it; all of it is computed in the browser from the module with
`parse`, `disasm`, `fold`, `lift`, `imports` and `strings`. The benchmark page shows every run under
`benchmark/results/` per compile setup, as a table and a chart, with a per-row drill-down and a diff
between two runs. Those six modules import no `node:*` module so that a browser can bundle them: the
glue file and import-name file readers are in `glue-file.ts`, the ledger readers in `readout.ts` and
`ledger.ts`.

## rank: compiling candidates

`rank.ts` compiles every candidate through `@match-kit/compiler`'s `createRunner` with your shell
template (`{{inputPath}}`, `{{outputPath}}`, optional `{{flags}}` and `{{symbol}}` = the extract
name), parses what the template wrote, pulls the function out, scores it against the target and
hands core `rankBy` a lookup. Failed compiles land in `ranked.dropped` with the compiler's output.

Two kinds of output are accepted, told apart by the `linking` section (`object.ts`):

- **linked module** (default, `--extract export:<name>`): emcc links the candidate alone
  (`--no-entry -sERROR_ON_UNDEFINED_SYMBOLS=0 -sEXPORTED_FUNCTIONS=_<name>`). This is the only path
  that can reach 0 against an optimised target: at `-O2`+ emcc runs binaryen's `wasm-opt` over the
  **linked** module, so a `-c` object is pre-wasm-opt LLVM output with a different instruction
  stream (measured on the test function: the object calls `strlen`, the linked module has musl's
  `strlen` inlined, score 55/73). Callees in the candidate module get fingerprints by the same
  rule as the target's, so a candidate whose callees also reproduce the target's scores 0.
  Imports compare by name, and the target's are minified (`a.x`); pass `importName` to
  `normalizeModule` with a glue-derived map to equalise them.
- **relocatable object** (`--extract sym:<name>`, `emcc -c` or `clang --target=wasm32 -c`): every
  immediate a `reloc.CODE` entry points at is replaced by its symbol — `call sym:helper`,
  `i32.const data:_ZL5table+3`, `i32.load align=2 offset=data:gval` (an address LLVM folded into a
  memarg offset is relocated _there_, not on an `i32.const`). Exact where the linked path's
  data-range heuristic is not, but callees are names the target cannot have, so the target is
  compared at depth 0 (callees by position).

The default template is the project's pinned link (`defaultTemplate()` in `rank.ts`; the tests use
it): `emscripten/emsdk:6.0.0` at `-Oz -fexceptions -sALLOW_MEMORY_GROWTH=1 -sSTACK_SIZE=8MB`, the
target's link line without its exports and preload (urna `build/README.md`):

```sh
docker run --rm -v /tmp:/tmp -w /tmp emscripten/emsdk:6.0.0 em++ -Oz -fexceptions -sALLOW_MEMORY_GROWTH=1 -sSTACK_SIZE=8MB {{flags}} {{inputPath}} -o {{outputPath}}.js --no-entry -sERROR_ON_UNDEFINED_SYMBOLS=0 -sEXPORTED_FUNCTIONS=_{{symbol}} && mv {{outputPath}}.wasm {{outputPath}}

pnpm wasm rank target.wasm 7000 --extract export:foo cand1.cpp cand2.cpp
pnpm wasm rank target.wasm 7000 --extract export:foo --image urna-emsdk:6.0.0 --flags '-std=c++17 -DFOO' cand.cpp
```

`--image` swaps the image, `--link-flags` the pinned flags, `--template` the whole command; `--flags`
are the per-candidate compile flags. The template writes `-o x.js` so emcc links (an extension-less
`-o` makes an object) and leaves the glue beside the module: at -Oz imports and exports are minified,
and the glue is where the rank adapter reads their real names from, for the candidate as for the
target. The CLI sets `TMPDIR=/tmp` for a Docker template, because the runner creates its compile
directory under `os.tmpdir()`, a `/var/folders` path on macOS that the daemon cannot see. Apple's
`/usr/bin/clang` has a wasm32 backend but no wasm sysroot: it compiles header-free sources to objects
and nothing more.

## splat: the module as pieces, and the writer that puts it back

The splat build rebuilds a module byte for byte from the target's own pieces. Each function our link provably
produces is then swapped in. There are four stages, covered in the next three sections:

1. **split** (`splat-split`): pieces, a manifest, and a proof that they reassemble.
2. **symbols** (`splat-symbols`): our link's exact names and the target's evidence-backed symbol file.
3. **substitute** (`splat-scan`, `splat-build`): a body of ours, re-pointed through both maps, against its piece.
4. **assemble** (`splat-assemble`, or `splat-build` with a match list): the module, checked against the
   source sha256.

urna drives them as `make -C src splat-split | link-map | splat-symbols | splat-scan | splat-match | splat`.
Its docs/reference/splat.md has the contributor loop, the drift rule and the meter.

A post-link counterpart of splat for a wasm build whose optimiser re-optimises anything fed through the link
(`splat.ts`). **splat-split** writes, under `<dir>`:

| path                               | holds                                                                                         |
| ---------------------------------- | --------------------------------------------------------------------------------------------- |
| `asm/<group>/<index>[-<name>].bin` | one function body: the bytes after its size LEB (locals vector, instructions, final `end`)    |
| `asm/<group>/<index>[-<name>].wat` | the same body for reading: signature, locals, one instruction per line, `(;offset;)` in front |
| `data/<nnnn>[-<addr8>].bin`        | one data segment's bytes; its address is in the name and in the manifest                      |
| `custom/<k>-<name>.bin`            | a custom section's payload (after its name)                                                   |
| `manifest.json`                    | everything else, decoded: section order, types, imports, function → type index, tables,       |
|                                    | memories, globals, exports, start, elem segments, data offsets and flags; per piece its path, |
|                                    | size and sha256; the source module's sha256 and size                                          |

`<index>` is the function index space (imports first); `<name>` the best known name (the glue's import and
export names, then the ledger's byte/fingerprint/addr names), made file-safe and cut at 60 characters, the full
name staying in the manifest. `<group>` is the function's component (`func-attribution.tsv`), else its owner
(`functions.tsv`) spelled as the component most of that owner's attributed functions carry (`libcxx` →
`libc++`), else the owner, else `unknown`; spellings that differ only by case are merged. `--names` and
`--attribution` default to the urna layout's `<root>/ledger/` beside `<root>/target/`.

Every LEB is written minimally. The splitter re-encodes each section it decoded and refuses the module when
the re-encoding differs (naming the section and the first differing byte), then assembles the pieces it wrote
and fails unless that gives the input's sha256: a split that succeeds is one the writer reproduces. Splitting
into a previous split removes the files its manifest lists first; a non-empty directory without a manifest is
refused.

**splat-assemble** reads only `manifest.json` and the pieces it names, and checks each piece against its recorded
sha256 (an edited piece is an error: a changed body goes in through `--substitute`). `--substitute` (repeatable)
takes a file named `<index>.bin` or `<index>-<anything>.bin` holding one replacement body, in the same form as
an `asm/` piece, or a directory whose files of that name, at any depth, are each one replacement; the same
index twice is an error, as is a body that does not decode or an index that is not a defined function. The
function keeps its manifest type index; body size LEBs, the code section size and the module are re-encoded
around the new sizes. It reports the output sha256, whether that is the split's source, and how many
substitutes differ from their piece (and their body bytes) or equal it. `assembleSplit(dir, Map<index, bytes>)`
is the same from code.

## splat symbols: our link's names, the target's symbol file, a body re-pointed

`splat-symbols.ts`, `splat-remap.ts`, commands in `splat-symbols-cli.ts`. A target function counts as decompiled
when the body OUR link produces, re-pointed into the target's index and address spaces, is the target body.

**Our side is exact.** urna's `make -C src link-map` (`tools/round/link-map.sh`) reruns the plain link's own
command with `-Wl,--Map` and `EMCC_DEBUG=1`, fails unless the result is byte-identical to the plain module, and
replays binaryen on the kept intermediates (`-Oz` with `--symbolmap`, then metadce and the final `wasm-opt` with
index names and `-g`, each checked against emcc's own output) to write `build/out/wasm-map/vota_web/names.tsv`:
every final function index with its wasm-ld symbol and object. The map gives every data symbol's address;
wasm-opt keeps data where wasm-ld put it. `loadOurSide(plain.wasm)` refuses a map directory whose relink is not
the plain module. The `--emit-symbol-map` twin is not needed: its bodies are the plain ones with function, type
and import indices permuted (`splat-twin-check`).

**The target side is evidence**, in `splat/symbol_addrs.txt` (`<name> = func:N; | import:N; | 0xADDR;` with
`// size:… evidence:… by:…`; lines after the hand-edit marker survive regeneration and win). The kinds are listed
at the top of `splat-symbols.ts`; the strong ones are a function pair unique on both sides at the census'
fingerprint or addr tier, and the votes of aligning such pairs instruction by instruction (callees, function
pointers in both tables, data symbols; `by:` lists the target functions that cast a data vote, up to 4).
Anything with conflicting evidence is left out, and so is a data name the target's bytes contradict
(`refuteDataNames`): an RTTI name whose vtable/typeinfo decodes to another class, a voted object whose bytes,
our pointers re-pointed, differ from the target's there (`compareObject`), a voted name at an address another
object of ours with different bytes, or the target's RTTI, holds. A vtable of a class with virtual bases starts
at its first virtual-base offset, as many words ahead of the typeinfo pointer as our own vtable has.

What the pairs leave unnamed, **anchors** may fill (`srclocAnchors`): a target function and one of ours that both
reference a `std::source_location` record (resolved by what it records) that no other function of either side
references, and whose bodies are the same instructions, are a pair (`evidence:srcloc-anchor`). Their call and table
votes name a function only when neither side is named yet, the votes agree and the signatures match; anchors cast
no data votes. A name only anchors voted for lists them in `by:`. This reaches std::format throwers, whose format
helpers our link inlines differently from the target's, so no unique pair ever reaches them.

**remapBody(maps, ourIndex)** rewrites calls, `ref.func`, type indices and globals; a reference that does not
resolve makes the body `unresolved`, with the instruction and the reason. An `i32.const`, an `i64.const` and a memarg
offset may each be an address, a table slot or an integer, which the final module does not say; the operand-kind
table (`splat-kinds.ts`, below) says it, and each operand is offered only the readings of its kind: a literal its own
bytes, an address the target place(s) of what it points at, a table slot the target slot(s) of its function (an
`i64.const` address by its low word). An address or slot with no target image makes the body `unresolved`. In a
kind-unverified function (no table, or one the table cannot vouch for) an `i32.const` and a memarg offset of 1024 or
more (a folded address, or a field offset such as `BF_KEY`'s S-boxes at 1024) keep every reading that resolves plus
their literal, and an `i64.const` whose low word is one
of our C strings and whose high word is that string's length is a string_view clang packed into one constant
(std::format's format string), read as that string's target address(es) with the same length, or literally;
**matchesTarget(pieces, body)** says whether some choice of readings gives the target body, and the first byte none
explains. A data address resolves through our symbol to the target's; anonymous objects by what they hold: a string
by its whole NUL-terminated text found once in the target, a `std::source_location` record by its file, function,
line and column, other read-only objects by bytes found once. Content that the target holds at several places adds each
place as a reading, and the target body picks the copy it uses: every place of the same C string through its NUL,
wherever tail merging put it (`STRING_COPY_LIMIT`, 4,096), and up to `SAME_CONTENT_LIMIT` (64) places of the same
read-only bytes. An object with no name whose exact bytes the target does not hold (a `.data` object, or read-only
data holding pointers) is placed by its bytes with each pointer word re-pointed (`content-repointed`): where that
pattern occurs once, or at each place for read-only content; a writable object found at several places stays
unresolved. Placing one object can re-point a word of another, so the placement repeats until nothing more
resolves. A function the target's table holds in several slots (binaryen merged identical functions) offers each
slot as a reading: every one of them for a table slot of known kind (binaryen's merged empty function fills 126), up
to `SAME_CONTENT_LIMIT` (64) for an operand of a kind-unverified function, where a function in more slots gives no
slot reading.

| command                                                                                        | does                                                                       |
| ---------------------------------------------------------------------------------------------- | -------------------------------------------------------------------------- |
| `splat-symbols <target> <plain> [-o f]`                                                        | gather the evidence and write the symbol file, keeping its hand edits      |
| `splat-maps <target> <plain> --fn <i\|sym:n> \| --data <addr> \| --target-fn <i> \| --summary` | one entry of the maps                                                      |
| `splat-remap <target> <plain> <i\|sym:n> [--against <t>] [--kinds f \| --no-kinds]`             | one body re-pointed: unresolved operands, or whether it is the target body |
| `splat-sym-coverage <target> <plain> [--smoke N] [--kinds f \| --no-kinds]`                     | named target functions, and the ledger's pairs re-pointed                  |
| `splat-twin-check <plain> <twin>`                                                              | the twin's bodies against the plain link's through a name/body pairing     |

`splat-remap` prints `ours <i>: re-pointed, <n> bytes; rewritten {<kind>: <count>}; soft <n>; operand kinds verified`
(or `KIND-UNVERIFIED (<why>)`; each soft operand, one of a kind-unverified function that looks like an address or
table slot at or above 1024 and resolves to neither, on a line under it), then `target <t>: IDENTICAL (<n> ambiguous
operands read as <readings>)` or `differs at body byte <k>` with the target instruction there. The readings are the
ones `matchesTarget` chose for the operands re-pointed as an address or a table slot (and, in a kind-unverified
function, the literals); the first reading of each is the preferred one (`RemapResult.bytes`), so it needed no
choice. It loads the symbol file (`--symbols`, default
`<root>/splat/symbol_addrs.txt`) and the map directory beside the plain module (`--map-dir`), and applies no
matches, unlike `splat-scan` and `splat-build` (`withMatches`). `<ours>` is our index or `sym:<names.tsv name>`; run
it as `pnpm exec tsx src/cli.ts splat-remap …` when the name holds a `$`, which `pnpm wasm` hands to a shell.

## splat substitution: the match list and the build

`splat-sub.ts`, commands in `splat-sub-cli.ts`. A target function is **matched** when our function, re-pointed,
is byte-identical to its piece (`matchesTarget`, no blinding). **splat-scan** tries, per target function, the
functions of ours the symbol file names it by, then the ledger's skeleton pair; then, among what is left, pairs by
body alone (`contentPairs`): one unused function of ours with the same locals and opcode sequence whose re-pointed
body equals exactly one unmatched target body, and no other such function of ours equals it. A match is the
strongest evidence of a name (`withMatches`: the matched symbol names that target function, no other symbol does),
so the scan runs again with its matches listed, until they stop changing: a pair found by body corrects the name
its callers' calls resolve through, and the last pass holds for a build whose match list is exactly its matches. Verdicts: `match`,
`differs` (the first differing instruction on both sides, plus what our operand there resolves to), `unresolved`
(a callee or operand with no target image), `no-candidate`. It writes a TSV (`index symbol ours source verdict
detail size ledger_kind group tried object`) and prints the verdicts per ledger kind and the reasons for the rest.

A data name whose only evidence is a vote cast by the target function being matched came from that function's
own operand, so it checks nothing there (`selfVoted`): such a match holds only when our object's bytes agree
with the target's at that address (`compareObject`; a .bss object's zeros say nothing), or another match that
holds re-points through the same name. Otherwise it is `unresolved` with a `self-voted` detail in the scan, and
`UNCONFIRMED` in the build. The same holds for a function name only the match's own anchored call or table operand
voted for (`by:` its index alone): it holds only when another match calls or references the same name. A constant
references a function only where the target body reads it as a table slot; a constant the target keeps as an integer
references nothing, even when it equals that function's slot in our table.

The meter has a second axis (`readingMeterOf`): how much the target's bytes chose among our readings. Per match,
the strongest choice it needed: none (re-pointing alone writes the target body), another reading of some operand
(a literal or table slot where the address reading leads, or the reverse: only in a kind-unverified match), or which
copy of identical string or read-only bytes, or which of a function's table slots, an operand names.

The match list (urna `splat/matched.txt`) is `<target index> <our symbol>` per line with `#` comments;
`splat-scan --match` appends the scan's new matches and keeps every existing line. **splat-build** re-points each
matched function through the maps with every match applied, compares it with its piece, and assembles the split with those bytes (every other function
from its piece); it refuses to write a module while any match fails (each failure names the match list line, the
function and its first differing instruction, or the reason it does not resolve) and when the module is not the
split's source sha256. It prints the meter: matched functions and code bytes, in total and per component, and
the readings the target chose.

| command                                                                                     | does                                                            |
| ------------------------------------------------------------------------------------------- | --------------------------------------------------------------- |
| `splat-scan <target> <plain> [-o scan.tsv] [--match [<matches>]] [--pieces d]`              | every target function against our re-pointed candidate          |
| `splat-build <target> <plain> [--matches f] [-o out.wasm] [--expect <sha256>] [--pieces d] [--max-kind-unverified n]` | the split with our bytes for every match, verified, and a meter |
| `splat-kinds <plain> [--shifted <dir>] [--map-dir d] [-o kinds.json] [--matches f]`         | the operand-kind table from the plain link and its shifted relink |

`splat-scan`, `splat-build`, `splat-remap` and `splat-sym-coverage` take `--kinds <kinds.json>`, default
`build/out/wasm-kinds/<name>/kinds.json` beside the plain link; they refuse to run without a table unless given
`--no-kinds`, and refuse a table that describes another plain module (sha256) or another format. With it, each
`i32.const`, `i64.const` and memarg offset is offered only the readings of its kind: a literal its own bytes, an
address the target address(es) of what it points at, a table slot the target slot(s) of its function; an address or
slot with no target image is unresolved. `splat-kinds` (`splat-kinds.ts`) writes the table: urna's
`tools/round/link-kinds.sh` relinks the plain link's objects with `-sGLOBAL_BASE=1024+D` and `-sTABLE_BASE=1+T` and
names that link's functions like `link-map.sh` (recording the sha256 of the wasm it named, which `splat-kinds`
checks, as it checks that the map dir describes the plain link); the two links are aligned by name, and in a function
with the same instructions an operand that moved by D is an address, by T a table slot, not at all a literal. A moved
constant equal to the link's own base is followed through the operand stack: added to constants (an unfolded `-fPIC`
sum such as sqlite's `select(2035, 2036, 2037) + 1`), it is the table base, read as the target's, and those constants
are slots relative to it, read as the target slot minus the target's base. Any other moved slot is followed too:
added to constants or to another slot, part of the base may be folded into it, so nothing tells which part is the
base. A function it cannot vouch for (another shape, a move by neither shift, a base kept in a local or carried out
of a block or added to a variable, such a slot sum, a `__memory_base` sum) is kind-unverified and keeps every
reading; so is every function with `--no-kinds`. `link-kinds.sh` refuses to start while an object or archive on the
plain link's line is newer than the plain wasm, and fails unless its relink's map lists the same input sections, in
the same order and of the same sizes, as `link-map.sh`'s map; urna's make re-derives the table whenever a toolkit
source changes. `splat-build` lists the kind-unverified matches apart, and fails on more of them than
`--max-kind-unverified`. A count flag needs a count: an empty one (`--max-kind-unverified` with no number) throws,
and urna's make leaves the flag out when its variable is empty. It also lists as review warnings the literal operands
of matches that may be a target number the source copied, which no byte check can reject: `past-data` (in the
target's data, past ours), `target-object` (the address of a named target object, from 64 KiB, or the low word of a
packed string view) and `past-table` (in the target's table, past ours). A target slot number inside our table
written as an integer, and an integer of the target's written as a pointer whose re-pointed value equals it, are not
listed: nothing tells them from legitimate ones. urna's `docs/reference/splat.md` "Reviewing a match" lists what no
check rejects.

## Tests

```sh
pnpm test            # from asmlift-wasm/; URNA_ROOT=<checkout> points the suites at another urna checkout
```

Every test runs on the real target, `target/vota_web_wasm.wasm` of the urna checkout around this directory:
section counts and offsets equal `wasm-objdump -h`; every function decodes and its `end` lands on
the body size; sampled functions agree with `wasm-objdump -d` mnemonic by mnemonic and offset by
offset; a function scores 0 and byte-exact against itself, 1 against a one-constant mutation;
`census(target, target)` pairs 14,343/14,343; the glue names every import and export of the target;
the harness probe's `free`/`malloc` pair by addr and `strlen` by byte, named from the symbol map;
the readout finds libc++'s format-error strings and `CLzmaCompress` slot 0; the object path names
relocated immediates (local clang); the rank adapter ranks the reproducing candidate first at
0/byte-exact through the pinned image (skips without it); every function lifts to a definition with
a body, the representative shapes (do/while, while, frame, call_indirect, invoke, switch, `main`)
read as expected, the lifter's evaluation-order, aliasing, shift, frame, alloca, store-order,
naming, drop, pointer-sum, arm-tail, memory-op, narrow-parameter and constant-address spellings
each hold on a target function that needs them, and a sample of
lifted sources compiles with em++ (skips without the image); the lift-eval sample is deterministic
and covers every component and band, `canonRow` equalises the object's and the target's spellings,
a four-function evaluation compiles, optimises and scores through the pinned image, and two em++
errors differing only in a name fall in one class (skips without it).

The splat tests (`splat.test.ts`, `splat-symbols.test.ts`, `splat-sub.test.ts`) read a checkout that has the
splat build (`URNA_SPLAT_ROOT`, default the same checkout): its target, split, plain link, link map and `splat/`
files, and skip what needs them when they are absent. They cover:

- the round trip to the target's sha256, and a refused edited piece;
- the symbol file's pairs, refutations and virtual-base vtables;
- a match that re-points through both maps;
- AudioCollector's destructors not matching;
- a self-voted match failing the build.

## Traps

- A block type is a **signed** 33-bit LEB: `0x40` is empty, `0x7f` is `(result i32)`, and type
  index 64 is `0xc0 0x00`, not the byte `0x40`. Reading one byte misreads every multivalue block.
- `i32.const` is a signed LEB; `>>> 0` before range-checking an address or `-2139062144` never
  falls in a segment.
- `wasm-objdump -d` prints one `local[a..b] type=t` row per locals group with the same `|`
  separator as instructions: its line count exceeds the instruction count by the number of
  functions with locals (2,723,980 vs 2,710,054 here), not because of decoding.
- A relocation on a load/store points at the **offset** LEB of the memarg, after the align LEB.
- An undefined function symbol in `linking` has no name field unless `EXPLICIT_NAME` is set; its
  name is the import's field.
- Relocatable objects pad relocated LEBs to 5 bytes, so `byteExact` is never true against an object.
- A bss address has no data segment: `free`'s `i32.const 1936372` stays a raw integer under the data
  symbol rule, so fingerprints under-match every function touching a global unless the addr tier
  runs too; and a negative `i32.const` (`-2`) is never an address (memory tops out at 2 GB).
- `-sMINIFY_WASM_IMPORTS_AND_EXPORTS=0` is an internal setting emcc refuses on the command line; the
  unminified names come from `-sASSERTIONS=1` (which changes the wasm) or from the glue of the
  minified build (which does not).
- `extractFunction` on a linked module normalises the WHOLE module (callee fingerprints need it): a
  loop calling it per function pays ~2.5 s each. Pass `norm` (`FindOptions.norm`) once; `bundle` does.
- A `--emit-symbol-map` file spells names DEMANGLED (`std::__2::vector<int, std::__2::allocator<int>>
const&`, LLVM style); `llvm-nm` without `-C` gives the mangled form, and the two never intersect.
