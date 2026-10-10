# Why there is no INCLUDE_ASM

Decision record. A function nobody has decompiled yet is a generated comment, a *stub*, in the TSE
sources ([../../src/README.md](../../src/README.md) "Stubs"). The target's bytes never go back into our build.
Byte identity comes from the splat build instead: it re-assembles the target after the toolchain has run
([splat.md](splat.md)). This page says why the project does not use a real `INCLUDE_ASM`.

The numbers below come from an external research note, "Real INCLUDE_ASM survives the linker but not wasm-opt",
which is not part of this repository. Its experiments run in the pinned image `urna-emsdk:6.0.0` (emscripten, clang
23 and binaryen at the revisions of [toolchain.md](toolchain.md) §1) on one skeleton link, so its counts are those
of that link.

## What INCLUDE_ASM relies on

In a GCC, IDO or agbcc decomp, `INCLUDE_ASM` puts the original assembly of an undecompiled function into the C
file, so the build links and runs and the function keeps its place. This works because nothing between the
assembler and the final image rewrites code. Tools such as asm-processor refuse the cases where something does,
such as IDO's function reordering at `-O3`.

## The wasm toolchain facts

**clang can include a whole wasm function.** clang 23 accepts a complete function in three forms: a `.s` file, a
file-scope `__asm__` that `.include`s a disassembled function (a literal `INCLUDE_ASM` macro), and a
`__attribute__((naked))` function whose body is the asm. Only the naked form keeps a translation unit's function
order, and it cannot declare locals. File-scope asm moves the function ahead of every compiled function, and
function order inside an object decides the link ordinal ([link-order.md](link-order.md)).

**wasm-ld keeps those bodies byte-exact.** It changes only the relocated operands, which stay 5-byte padded LEBs
because emcc does not pass `--compress-relocations`. One hazard: when the asm declares a callee with another
signature than the C++ definition, wasm-ld only warns, and points the call at a stub that traps.

**Binaryen's `-Oz` then rewrites them.** emcc runs `wasm-opt -Oz` on the linked module, then `wasm-metadce`, which
sorts the functions, then a last `wasm-opt` that minifies names ([toolchain.md](toolchain.md) §4).

- The reader alone changes bodies. Reading the target and writing it back with the final stage's flags, running
  no pass, changes 1,483 of the 14,343 bodies (10.3%). Every change is in the locals: Binaryen's tree IR adds a
  scratch local, and the writer drops the set and get again but keeps the declaration.
- `-Oz` is not a fixed point. A second `-Oz` leaves only 74 to 82% of the functions identical: dead-argument
  removal, more inlining, merges, peepholes. Binaryen's own pass list says it "may allow more
  inlining/dae/etc., need --converge for that".
- Binaryen v130 cannot exempt one function. Its only per-function control is `--no-inline`, and it acts only
  before the inlining pass. emcc appends extra passes after `-Oz`, where the flag has no effect.

**Where the original bodies go in decides the result.** The note splices the skeleton's own final bodies back in
and replays the rest of the pipeline:

| splice point | functions it can express | bodies that come out identical | function order |
| --- | --- | --- | --- |
| before `-Oz` (what a real INCLUDE_ASM gives) | 8,205 of 10,836 (75.7%) | 78.2% of those | scrambled: one survivor more or less reshuffles the module |
| after `-Oz`, before `wasm-metadce` | all | 86.9%, and 97.9% after a heuristic step that moves Binaryen's scratch locals into dead slots | kept exactly |

A quarter of the final functions cannot be written before `-Oz` at all. They call a callee whose signature
dead-argument removal changes, call a `byn$mgfn-shared$` thunk Binaryen makes, have their own signature changed,
or are such a thunk. An included final body also lacks the calls that were inlined into it, so it changes the
inlining and dead-argument decisions for its callees.

## The decision

A real `INCLUDE_ASM` would give a build that links and runs, but not the target's bytes. The note's design pairs a
naked `INCLUDE_ASM` in the sources with a wrapper `wasm-opt`, selected through `EM_BINARYEN_ROOT`, that writes the
original bodies back after `-Oz`. None of that is built: it needs a wasm-to-`.s` converter, a locals patcher and a
liveness-based local allocator. One question decides whether the design holds, and it is not measured: can a
partly decompiled link reproduce the original's set of functions that survive `-Oz`? `wasm-metadce`'s order
depends on that set.

The project therefore keeps two separate things:

- **The splat build** is the byte check. It splits the target into pieces and substitutes our bytes for a
  function only when our whole-program link provably produces that body, after the toolchain
  ([splat.md](splat.md)).
- **Stubs** mark the work left in the sources. They are comments, generated where the evidence puts each
  function. They take only blank lines, so every object compiles byte-identical with or without them, and CI
  checks each one against the evidence (`tools/round/stubs.py --check`). A stub is not a half-real
  `INCLUDE_ASM`: it puts nothing into the link.

## Names change the `-Oz` output

emcc removes the module's `name` section with `llvm-objcopy` before Binaryen runs. That happens in
emscripten's `tools/emscripten.py`, unless something needs the names: `-g`, `--profiling-funcs` or `--emit-symbol-map`.
Binaryen would read names if they were there, and keeping them changes the `-Oz` output. That is why the named
twin's function order differs from the plain link's. [link-order.md](link-order.md) §1 places this step in the
pipeline.
