# @asmlift/wasm-benchmark

The lifter's benchmark over `target/vota_web_wasm.wasm`. Its rows are target functions from every
compile setup that produced code in the module: the TSE application and its libraries, each third-party
library at the flags its build used, and emscripten's prebuilt system libraries. Every row is lifted to
C++, compiled with **its own setup's flags** in the pinned image, and scored against the target. One
run gives a score per row, totals per setup and overall, and a Markdown report; two runs can be diffed.

```sh
cd asmlift-wasm && pnpm install       # once; the workspace links this package to @asmlift/wasm
pnpm bench setups --check             # the registry against the build
pnpm bench run --label my-change      # the committed baseline took 46 s for the whole dataset
pnpm bench diff baseline my-change
```

`pnpm bench <command>` runs from `asmlift-wasm/` (or `pnpm bench` inside `benchmark/`). Every command
that compiles or reads an archive needs Docker and the `urna-emsdk:6.0.0` image (`build/docker-build.sh`
from the repo root builds it). Scratch files go to the checkout's gitignored `build/out/bench/`, which
Docker can mount; nothing goes through `$TMPDIR`.

## What it measures

| step | what happens | in the results |
| --- | --- | --- |
| lift | `liftFunction` turns the target function into a C++ translation unit | `lifted`, outcome `lift-threw` |
| compile | `em++ -c` at the row's setup flags (`liftCompileFlags`: defines and a C `-std` dropped, since the lift is C++ and names no macro) | `compiled`, outcome `compile-failed` |
| extract | the function found in the object by its symbol | outcome `extract-failed` |
| raw score | object vs target at depth 0, rows canonicalised on both sides (callees, data addresses, the stack pointer, the indirect table) | `raw.score`, `raw.ratio` |
| -Oz score | the object linked alone with wasm-ld and put through the target link's two wasm-opt passes, scored again | `opt.score`, `opt.ratio`, outcome `link-failed` |
| exact | `opt.score` is 0 | `exact`, outcome `exact` (else `differs`) |

The ratio is differing rows over target rows. The -Oz score is the one to steer by: the target went
through wasm-opt and an object did not. Even so, an isolated function misses what the whole-program
link does to it (inlining, dead-argument removal, data addresses), so **exact here means structurally
equal after normalisation, never byte-identical**: a byte-identical function is a match of the splat
build (`docs/reference/splat.md`). The steps are the toolkit's own `evaluateLift` (`src/lift-eval.ts`); the
benchmark adds the rows, their flags, and the per-setup accounting.

## The compile setups

`dataset/setups.json` is the registry: a setup is one set of compile flags that produced target
code, with the flags per language, the image, where the flags live in the repo, and how to re-derive
them (`truth`).

| setup | what | flags from | rows |
| --- | --- | --- | --- |
| `uenux2` | the TSE application | `src/Makefile` `UENUX2_CXXFLAGS` | 12 |
| `ecourna` | ecourna's PIC translation units | `src/Makefile` `ECOURNA_CXXFLAGS` + `src/mk/ecourna-flags.mk` + `src/units/*.mk` | 10 |
| `ecourna-nopic` | ecourna's non-PIC units (app/dados, api/io, api/compression, the asn converters) | the same, with the fragments' `-fno-PIC` rules | 12 |
| `asn1` | the TSE fork of the III ASN.1 runtime and its generated modules | `src/mk/asn1.mk` | 12 |
| `asn1-eh` | the ASN.1 units that throw (`-fexceptions -fignore-exceptions`) | `src/mk/asn1.mk` `ASN1_FLAGS_<unit>` | 11 |
| `zlib` | zlib 1.3.1 + minizip | `src/units/06-ecourna-api-compression.mk` `ZLIB_CFLAGS` | 12 |
| `sqlite` | SQLite 3.50.4 amalgamation | `tools/libs/sqlite/build.sh` (recorded in `cflags.txt`) | 12 |
| `openssl` | OpenSSL 3.0.17 libcrypto | `tools/libs/openssl/configure.sh` (the configured Makefile) | 12 |
| `7zip` | 7-Zip / LZMA SDK 21.02, single-threaded | `tools/libs/7zip/build.sh` (recorded in `flags.nul`) | 12 |
| `rhvoice` | RHVoice 1.14.0 core, hts_engine and sonic | `tools/libs/rhvoice/build.sh` (recorded in `flags.nul`) | 12 |
| `boost` | Boost 1.89.0, header-only | the instantiating setup's flags | 12 |
| `libc` | emscripten's musl (`libc.a`) | emscripten `tools/system_libs.py` | 12 |
| `libc-optz` | the `-Oz` link's size-optimised libc members (`libc_optz.a`) | the same | 5 |
| `dlmalloc` | `libdlmalloc.a` | the same | 8 |
| `compiler-rt` | `libcompiler_rt.a` | the same | 12 |
| `libcxx` | `libc++.a` (JS exceptions) | the same | 12 |
| `libcxxabi` | `libc++abi.a` (JS exceptions) | the same | 12 |

`pnpm bench setups` prints every setup's flags, the flags its lifted sources compile with, its
objects and its check status. The link also takes `libstubs.a`, `libsockets.a` and `libnoexit.a`,
but no target function pairs with one of their symbols, so they are not setups; `libc-optz` and
`dlmalloc` have fewer rows because the ledger pairs only that many of their functions.

**The check** (`pnpm bench setups --check`, also run before every `run`) re-derives each setup from
the build and fails on drift, so a flag changed in the Makefile cannot leave the registry silently stale:

- `make`: the objects of the named `src/Makefile` variables (`make print-<VAR>`), each one's compile
  command from `make -n -B`. Every object must compile with the flags of exactly one setup naming its
  variable; this is also how `ecourna` and `ecourna-nopic` split the same `ECOURNA_OBJS`.
- `artefact`: a library build records the flags it compiled with beside its objects (`cflags.txt`,
  `flags.nul`, OpenSSL's configured Makefile); they must be the setup's. Absent in a checkout that has
  not built the library, the setup is `unverified`, not failed.
- `emscripten`: `tools/system-lib-flags.py` asks emscripten in the image which flags it builds each
  system library with for a `-fexceptions`, no-assertions link.
- `header-only`: nothing to compile; see Boost below.

Flags are compared by effect (`effectiveFlags`): the last `-O` and `-std` win, PIC and exceptions are
on or off whatever the spelling, include paths, path maps, warnings and debug info are not flags of
the code. A changed define is drift.

## How a row's setup is decided

The ledger (`ledger/functions.tsv`) names a target function after the skeleton function it pairs
with at the byte, fingerprint or addr tier. The skeleton links exactly the objects the check
classifies, so the object that defines that name is the function's setup: `llvm-nm` over every link
input in the image, cached in `build/out/bench/definitions.json` (rebuilt when the registry changes or
on `select`). Link semantics decide between several definitions: an object's weak definition shadows
an archive's, and of two archives the earlier in link order wins (`libc_optz.a` before `libc.a`). A
name is left out when a **stand-in** (a hand-written root or stub object of `src/app` and
`src/units/*-roots.cpp`) also defines it, when two setups define it, or when two target functions
share it.

- **Boost** is header-only. A `boost::` function belongs to the `boost` setup with the flags of the
  setup whose objects instantiate it (`instantiatedBy`). Where only the Boost probe stand-in
  (`roots_boost.o`) instantiates it, its flags (the application's) are recorded, and the row is
  selected only when the ledger pairs it at byte or fingerprint.
- **uenux2** has few reconstructed units, so its rows also come from the ledger's attribution
  (components `vota`, `vota-web`, `comum`, `simulador`): `evidence: "component"`, no object, no
  reference.

## The dataset

`dataset/rows.json`, one object per row (type `DatasetRow` in `src/schema.ts`):

| field | |
| --- | --- |
| `id` | `<setup>/<index>` |
| `index`, `size`, `band` | function index in the target (imports first), body bytes, size band (tiny ≤ 32, small ≤ 128, medium ≤ 512, large ≤ 2048, huge) |
| `setup`, `lang`, `flags` | the setup, the source language, that setup's exact flags for it |
| `instantiatedBy` | Boost rows: the setup whose flags `flags` are |
| `component`, `owner` | the ledger's attribution and owner class |
| `name`, `symbol` | the ledger's name and the linker symbol it resolved to |
| `features` | read off the target body: `leaf` (no calls), `loop`, `switch` (br_table), `call_indirect`, `invoke` (an `invoke_*` import), `throw` (`__cxa_throw`, `__resumeException`), `frame` (a shadow-stack frame), `alloca`, `inlined-frame` (a second frame: a callee wasm-opt inlined), `i64`, `float`, `bulk-memory`, `select` |
| `ledger` | the ledger's current pair: `kind` (byte, fingerprint, addr, shape, none) and `score` |
| `evidence`, `object` | `symbol` with the defining object (`<archive>:<member>` for an archive member), or `component` |
| `reference` | source that reproduces it: a reconstructed unit under `src/` when the ledger pairs it at byte or fingerprint; a library's upstream file in the tree its build compiles (`third_party/…`); `emsdk:<path>` for a system library, from the member's debug info; `line` when one line defines it, numbered as the compiler numbers it (through `#line`, so in a TSE source it is the line of TSE's file: `src/README.md` "Gaps") |
| `origin` | `selected` or `manual` |

**Selection** is deterministic. A setup's candidates are the target functions it covers with a body
of 9 to 8192 bytes (smaller bodies pair positionally among identical twins, so their name is no
evidence). The size bands take turns, smallest first; each turn takes the band's candidate that adds
the most features the setup's picks do not cover yet, ties going to symbol evidence, then the stronger
ledger pair, then an FNV-1a hash of the id. The same target, ledger, registry, definitions and sources
give the same rows.

**The dataset is frozen.** Selection is not re-run as the project moves: a new unit or a census changes
the ledger and the build, which would swap rows and leave nothing to compare with the committed
baseline. `rows.json` changes only by adding a row, by a references update (the same rows) or by a deliberate
re-selection, each with a fresh baseline (a test holds `results/baseline.json` to the sha256 of `rows.json`).

`dataset/selection.json` (type `Selection` in `src/schema.ts`) is the dataset's `version` and what its
selected rows were selected from, enough to run that selection again:

| field | |
| --- | --- |
| `version` | one more on every re-selection or references update |
| `per` | rows per setup |
| `commit` | the urna commit selection ran at, whose files the ledger, the registry and the sources are |
| `inputs.target`, `inputs.glue` | the pinned target and its glue, with their sha256 |
| `inputs.ledger`, `inputs.setups` | `ledger/functions.tsv` and `dataset/setups.json`, with their git blob ids at `commit` |
| `inputs.sources` | the other files `commit` tracked that selection read, the sources references were looked up in: repo path → git blob id |
| `inputs.definitions` | `dataset/selection-definitions.json` and its sha256: the build's definitions (llvm-nm in the image) of the `names` selection looked up, with the objects, archive members and stand-ins they name; `build` is the commit checked out when they were read; `moved`, the link inputs the build has moved since (old path → new path), which the snapshot names at their new path |
| `references` | after a references update: `rowsOf`, the version whose rows these are, and `note`, why the references moved |

The build is not in git, so its definitions are kept beside the rows. The ledger, the registry and the
sources are kept too, as they were at `commit`: `dataset/selection-inputs/` holds a copy of each at its
repo path, which must be the recorded blob, so any clone, shallow or not, re-runs the selection. References
into the fetched and built library trees and the image are read from those, each pinned by its own
revision. A `note` says where a version's record departs from that.

**Today's repo** is checked against the rows instead (`test/dataset.test.ts`): every row's target
function at its index with its size and features; its setup in the registry with the row's flags, and
no drift between that setup and the build; for a `symbol` row, its object still defining its symbol
under its setup (llvm-nm in the image); its reference still there (in the checkout, in the fetched or
built library tree, or in the image). A row that no longer holds fails the test with its id and the
reason. Fix the row's cause, or retire it deliberately: a re-selection.

**Adding a row**: `pnpm bench add <index>…` builds the row from today's inputs, marks it `manual` and
keeps it across re-selections; then `pnpm bench run --label baseline` and commit both. A function no
setup covers is refused with the reason; reconstruct or link its object first. A new setup is a new
entry in `setups.json` with a `truth` the check can re-derive, then a re-selection.

**Updating the references** (a library's source tree or build output moved, the rows did not): `pnpm bench select
--references` runs selection again on the recorded inputs, as the test does, and prints the rows whose reference it
now gives differently; it refuses when those inputs no longer select the committed rows field for field, and when the
dataset has hand-added rows (no recorded input gives their reference). `--move <old>=<new>` (repeatable) takes a link
input the build has moved, an object or archive, at its new path: in the recorded definitions and in the rows'
`object`; it refuses an old path the snapshot does not name and a new path the checkout does not have.
`pnpm bench select --references --write [--move <old>=<new>]… --note "<why>"` writes those references and objects into
`rows.json` and `selection.json` at the next version, recording the version whose rows they are and the note, and
with `--move` the moved snapshot and its `moved` paths; the other inputs and their commit stay. Commit them, run
`pnpm bench run --label baseline` from that commit, so the run records a clean tree, and commit
`results/baseline.{json,md}`.

**Re-selecting**: `pnpm bench select [--per N]` prints what selection from today's ledger and build
would drop, add and change, and writes nothing. `pnpm bench select --write [--per N]` re-selects:
`rows.json` (hand-added rows kept), `selection.json` at the next version recording HEAD, the
definitions snapshot and the copies in `dataset/selection-inputs/`. It refuses while the ledger, the registry, `src/` or the 7-Zip source list have
uncommitted changes, since it records HEAD as their commit. Then `pnpm bench run --label baseline`, and
commit `rows.json`, `selection.json`, `selection-definitions.json`, `selection-inputs/` and
`results/baseline.{json,md}` together.

## Results

`pnpm bench run --label <l> [--setup k,…]` writes `results/<l>.json` (type `BenchResults` in
`src/schema.ts`; the playground imports it from `@asmlift/wasm-benchmark/schema`) and `results/<l>.md`.
The lifted sources, objects, linked modules and compiler output stay in `build/out/bench/<l>/lifted/`.

| field | |
| --- | --- |
| `schema` | 1 |
| `label`, `createdAt` | |
| `target` | path and sha256 of the target module |
| `image` | the docker image every compile ran in |
| `dataset` | row count and the sha256 of `rows.json` and `setups.json` as run |
| `commit` | the urna commit the run started from, `+dirty` with uncommitted changes |
| `durationMs` | lift, compile and optimise, total |
| `totals.overall`, `totals.bySetup[key]` | `Totals`: `rows`, `lifted`, `compiled`, `optimised` (linked and through wasm-opt), `exactRaw` (raw score 0), `exact` (-Oz score 0), `medianRatio`, `medianOptRatio`, `meanOptRatio`, `outcomes` (count per outcome) |
| `rows[]` | `RowResult`: `id`, `index`, `setup`, `size`, `band`, `features`, `lifted`, `compiled`, `targetRows`, `raw {score, ratio}`, `opt {score, ratio}`, `exact`, `outcome` (`exact`, `differs`, `lift-threw`, `compile-failed`, `extract-failed`, `link-failed`), `failure {class, line}`, `standIns` (the lifter's workaround notes in the source) |

**Reading a report.** The per-setup table is the point: a lifter change that helps one toolchain and
hurts another shows there first. Compile failures are lifter bugs (their first error line is in the
failures table, the full output in `<index>.cpp.err`). Then compare `exact -Oz` and `median -Oz` by
setup, by size band (tiny and small functions are mostly exact, so movement shows in the medium to huge
medians) and by feature. The largest -Oz ratios list the rows to look at next.

`pnpm bench diff <a> <b> [-o out.md]` takes labels or paths: per-setup totals side by side, then every
row whose outcome or -Oz score moved. `pnpm bench report <label>` renders a results file's report again.

`results/baseline.json` and `results/baseline.md` are the committed reference run.

## Tests

```sh
pnpm test            # from benchmark/, or `pnpm test:bench` from asmlift-wasm/
```

They run on the real target, ledger and build, with no mocks. The flag helpers on spellings the
build uses; the registry check on `src/Makefile`'s dry run (every object classified, a changed flag
reported as drift), on the library builds' recorded flags and on emscripten's own system-library
flags; the committed rows against today's repo (above); `selection.json` against the files it
records, and selection on those recorded inputs reproducing `rows.json`; the harness end to end on a
C++, a C and a system-library row; the baseline against `rows.json` and its totals against its rows;
the report and the diff. The tests that need the image skip without it; the artefact test skips in a
checkout that has not built SQLite, and the two that read the library sources skip without the fetched
and built trees the rows' references point into.
