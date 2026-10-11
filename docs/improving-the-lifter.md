# Improving the lifter

The lifter (`pnpm wasm lift`, `asmlift-wasm/src/lift.ts`) turns a target function into C++ that `em++` compiles.
This page says what a better lifter means in this project, how to pick something to fix, and how to show that a
change helped. It assumes the toolkit installed and the pinned image built ([getting-started.md](getting-started.md));
the benchmark compiles in the image.

## What the lifter is for

A lifted file is a draft that a person or an agent rewrites into the original source
([how-to-decompile.md](how-to-decompile.md) step 3). It is never committed under `src/`. Lifter work therefore does
not move the official meter: matches come only from C++ that lands in `src/` and verifies in the splat build. A
better lifter shortens the work between a target function and that C++.

That work has two parts, and the lifter serves both:

- **Structure.** The draft should compile to the target's shape: the same calls in the same order, the same frame
  objects, the same branches. The benchmark measures this as the `-Oz` score: the lifted function compiled at its
  setup's flags, linked alone, put through the target link's wasm-opt passes, and scored row by row against the
  target ([asmlift-wasm/benchmark/README.md](../asmlift-wasm/benchmark/README.md)). Lower is closer, 0 is exact.
- **Readability.** The draft should say what the source said. A reconstruction keeps almost none of a draft's text
  verbatim, but uses it as the structural reference for most of its decisions: member offsets, call order, which
  helpers stay out of line, invoke versus call. No number measures readability; the known gaps below are what
  rewriting a draft lacks most.

The two pull apart in one place. To reach the target's shape the lifter adds workarounds that no source had:
`volatile` stores, empty `asm` barriers (`KEEP_IN_ARM()`, the escape at entry), `goto`
([asmlift-wasm/README.md](../asmlift-wasm/README.md) "lift" explains each). The benchmark counts an `exact` outcome
(structurally equal after normalisation) that relies on them, and the how-to tells the reader to strip them. So
judge a change by both:

- a change that lowers `-Oz` ratios without adding a workaround is an improvement;
- a change that removes a workaround, or replaces one with source-like C++, without raising the ratios is an
  improvement;
- a change that lowers a ratio by adding a workaround is a trade: say so in the commit, with the rows it moves.

## Picking a gap

- **The benchmark report.** `asmlift-wasm/benchmark/results/baseline.md` ends with the largest `-Oz` ratios; its
  per-feature table shows where the lifter is weakest: loops, `switch` (`br_table`), shadow-stack frames, callees
  wasm-opt inlined (`inlined-frame`), `i64` and `select`. The per-setup table shows whether a gap is one
  toolchain's (compiler-rt and 7-Zip rows trail the rest).
- **A draft you are rewriting.** When a draft misled you while decompiling a function, that function is a good test
  case: it is a gap someone hit.
- **The known gaps** below.

## The loop

```sh
cd asmlift-wasm
pnpm wasm lift ../target/vota_web_wasm.wasm <index> -o /tmp/lift --check    # the draft and its compile
# edit src/lift.ts; add a test in test/lift.test.ts on the target function that showed the problem
pnpm test && pnpm typecheck
pnpm bench run --label my-change         # the whole dataset; the report prints how long it took
pnpm bench diff baseline my-change       # per setup, then every row whose outcome or -Oz score moved
```

- **Tests** run on the real target, with no mocks or fakes: a lift test lifts a named target function and asserts
  what the draft must contain. Name the subject in `describe` and the behaviour in `it('<present verb> …')`, so it
  reads as a sentence ("lifts every defined function of the target …"). A test that needs the image or a build
  output skips without it; CI's skip budget (`.github/CI.md`) must then be raised in the same change.
- **The benchmark diff** goes in the commit message or pull request. A compile failure in a run is a lifter bug;
  its first error line is in the report's failures table.
- **The baseline.** Refresh `benchmark/results/baseline.{json,md}` (`pnpm bench run --label baseline`) only from a
  committed tree: the report records the commit, with `+dirty` when there were uncommitted changes.
- **The dataset is frozen.** A lifter change never re-selects `benchmark/dataset/rows.json`. A row is added with
  `pnpm bench add`; a references update (`pnpm bench select --references --write`, when a library's tree or object
  moved and the rows did not) and a re-selection (`pnpm bench select --write`) each make a new dataset version. Each
  of the three is its own commit with a fresh baseline, and the benchmark's tests re-run the recorded selection to
  check that `rows.json` still follows from it ([asmlift-wasm/benchmark/README.md](../asmlift-wasm/benchmark/README.md)
  "The dataset").
- **Another sample.** `pnpm wasm lift-eval` scores a stratified sample of about 150 functions; a second seed checks
  that a change did not fit the benchmark's rows alone ([asmlift-wasm/README.md](../asmlift-wasm/README.md)
  "lift-eval").

`src/lift.ts` is one file of about 2,600 lines. [asmlift-wasm/README.md](../asmlift-wasm/README.md) "lift"
describes its parts in order: names, values, stores, control flow, the shadow stack, inlined callees, calls and
branches.

## Known gaps

From the benchmark (`baseline.md`, 190 rows, 101 exact after `-Oz`):

- no large or huge function is exact, and loops (7 of 56 exact), `switch` (1 of 21), frames (7 of 46) and inlined
  frames (0 of 9) account for most of the distance;
- compiler-rt's `i64` arithmetic and 7-Zip's coders have the largest ratios.

From rewriting drafts into landed sources:

- **Data constants are bare numbers.** String literals, vtables, typeinfos, error records and ASN.1 `theInfo`
  objects print as integers, though `pnpm wasm fn` names them, so the readout has to be read beside the draft.
- **The Emscripten exception protocol is spelled out**: `__THREW__` stores, `invoke_*` calls,
  `__cxa_find_matching_catch_2`, `__resumeException`, where the source had `try`, `catch` and `throw`.
- **`std::format` arguments are raw `i64` constants** (the format string's pointer and length packed in one).
- **No class layout.** Every member access is `*(T*)((char*)p + off)`, so a layout is rebuilt by hand from many
  functions. A table of offsets, widths and signedness per pointer parameter would answer it.
- **Switch case order** is not shown, though the order of a `br_table`'s targets encodes the source's case order.
- **Boolean logic reads as bit arithmetic**, and `shared_ptr` reference counting as raw arithmetic.
- **A lone link of a draft needs data stubs** for the `theInfo` and typeinfo symbols it references.
- **The header's compile line** comes from the lifter's own tables (`TREE_FLAGS` for the tree the function's records
  or stub name, else `COMPONENT_FLAGS`), which cannot express ecourna's per-directory `-fno-PIC`;
  `tools/round/try-tu.sh` reads the real flags from `src/Makefile`.

A callee prints by its mangled symbol when the ledger names it: the symbol lists `make -C src link` writes
(`build/out/wasm/vota_web/symbols.{mangled,demangled}.txt`) give the spelling for every name the ledger has. A callee
the ledger has not paired prints as `f<index>`.
