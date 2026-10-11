# Glossary

Words a newcomer meets in this repository, each with where it is used. Paths are relative to the repo root.
Several words carry more than one meaning here (twin, probe, band, anchor, rank, stand-in, stub); each entry lists
every meaning in use.

The words to know first, enough for the README and the how-to: **target** (the TSE module being rebuilt),
**component** (which part of the program calls a function), **match** and the **meter** (the one progress number),
the **splat build** (the check that verifies every match), **scan verdicts** (the work list), the **census tiers**
(how the census pairs functions: diagnostics, not progress), **unit** and **roots** (how reconstructed C++ lands),
and **try-tu** (trying a candidate).

Contents: [the subject](#the-subject) · [Portuguese words in the code](#portuguese-words-in-the-code) ·
[component and owner](#component-and-owner) · [the build](#the-build) · [pairing and progress](#pairing-and-progress) ·
[the splat build](#the-splat-build) · [link order](#link-order) · [the toolkit](#the-toolkit) ·
[process](#process)

## The subject

**TSE** — Tribunal Superior Eleitoral, Brazil's superior electoral court. It publishes the voting simulator whose
WebAssembly module this project rebuilds (https://www.justicaeleitoral.jus.br/simulador-votacao/).
*Where:* `README.md`, `docs/reference/appmap.md`.

**urna** — the urna eletrônica, Brazil's electronic voting machine, and the name of this project. The simulator runs
the urna's voter-facing software in a browser. *Where:* the repo name; `comum` ("shared urna app code") in
`docs/reference/appmap.md` §1.

**target** — the files being reproduced: `target/vota_web_wasm.wasm` (sha256 `8579ef8d…1007e2`, 14,343 defined
functions), its emscripten JS glue `vota_web_wasm.js`, the preloaded-file payload `vota_web_wasm.data`, the
simulator's scenario list `manifest.json`, and `fs/` + `fs-manifest.tsv` (the payload unpacked by
`tools/setup/unpack_fs.mjs`). "Target function N" always means function index N of the target. *Where:* `target/`,
`target/SHA256SUMS`, every doc.

**function index** — a function's position in the wasm function index space, imports first: the target's imports are
0–135, so its first defined function is 136. *Where:* `ledger/functions.tsv` column `index`, every toolkit command
(`asmlift-wasm/README.md`), `splat/matched.txt`.

**vota_web** — the emscripten output name of the simulator build; the glue embeds
`PACKAGE_NAME = "wasm/vota_web/vota_web_wasm.data"`, so our link writes under `build/out/wasm/vota_web/`.
*Where:* `docs/reference/toolchain.md` §2, `src/Makefile`.

**uenux2** — the TSE application source tree; its paths in the binary start `/home/rubio/tse/uenux2/src/…` and
`…/uenux2/wasm/vota_web/`. Our reconstruction mirrors it under `src/uenux2/`, compiled with `UENUX2_CXXFLAGS`
(`-O2 -fPIC`, no `-fexceptions`). It splits into the components `vota`, `comum`, `api`, `simulador` and `vota-web`.
*Where:* `src/uenux2/`, `src/Makefile`, `docs/reference/appmap.md` §2, `docs/reference/app-flags.md`.

**ecourna** — a TSE library shipped as the conan package `libecea1da310e5107` (security, SQLite wrapper, compression,
io, util, and the `app/dados` data model with its ASN.1 converters). Mirrored under `src/ecourna/`, compiled with
`ECOURNA_CXXFLAGS` (`-O3 -fexceptions`, `-fPIC` per file). Note that ecourna has its own `api/` directory, which is
component `ecourna`, not component `api`. *Where:* `src/ecourna/`, `src/mk/ecourna-flags.mk`,
`docs/reference/toolchain.md` §3.

**rubio paths** — `/home/rubio/tse/uenux2` and `/home/rubio/.conan2/p/b/libecea1da310e5107/b/src/ecourna` are the TSE
build machine's directories, embedded in the binary through `std::source_location` records. We reproduce them with
`-ffile-prefix-map`. *Where:* `docs/reference/app-flags.md` "Path mechanism", `src/units/02-uenux2-cthreadvota.mk`, `src/units/13-uenux2-crdvvota.mk`.

**components of the TSE code** — the attribution names of `docs/reference/appmap.md` §1:

| component | what it is |
| --- | --- |
| `vota` | uenux2 `src/app/vota`: the voting application (states, screens, operator flows) |
| `comum` | uenux2 `src/app/comum`: shared urna app code (domain model, ASN.1 converters, reports, DAO) |
| `api` | uenux2 `src/api`: the GUI toolkit, io, ipc, hwil (the hardware interfaces: printer, input, power), util, hash, logging, persistence, audio |
| `simulador` | the web-simulator shim: `CWasm*` devices implementing `api::I*`, `wasm/vota_web/vota_web_wasm.cpp` |
| `vota-web` | the anonymous-namespace web engine (`CVotaWebEngine`, `CWasmSavd`, …) |
| `ecourna` | the ecourna conan package |
| `asn1-modules` | the asnparser-generated ASN.1 types `Modulo*::` (sources in `src/asn1/modules/`) |
| `ASN1` | the TSE fork of the III ASN.1 runtime (namespace `ASN1::`, sources in `src/asn1/`) |

The other components are the upstream libraries (`openssl`, `sqlite`, `7zip`, `RHVoice`, `boost`, `libc++`,
`libc++abi`, `libc`, `utf8cpp`, `rapidxml`) and `-` (no attribution, printed `unknown` by the splat meter).
*Where:* `ledger/func-attribution.tsv`, `docs/reference/appmap.md` §1, `third_party/MANIFEST.md` for the library versions.

**source_location record** — the 16-byte `{file_name, function_name, line, column}` record clang emits for each
`std::source_location::current()` call site; TSE's exception constructors take one, which is why the binary holds
source paths and pretty function names. *Where:* `ledger/source-locations.tsv` (`tools/extract/source_locations.py`),
`docs/reference/app-flags.md` "The error records are std::source_location, not a macro".

## Portuguese words in the code

The TSE sources are in Portuguese. The words that name whole subsystems:

| word | meaning | seen in |
| --- | --- | --- |
| eleitor | voter | uenux2 `src/app/vota/eleitor/` (`docs/reference/appmap.md` §2), `vota::CThreadEleitor` |
| mesário | poll worker operating the urna | `CRegistrarMesarios`, `comum/comparecimentomesario/` |
| dados | data (the domain model) | `src/ecourna/app/dados/`, `comum/dados/` |
| BU (boletim de urna) | the per-machine tally report | `CGeradorBUBase`, `IQRCodeBUDS` |
| RDV (registro digital do voto) | the digital record of each vote | `comum::CRdvVota`, `md/rdv/` |
| zerésima | the report printed before voting starts, showing zero votes | `cverificahorariozeresima` |
| cargo | an office being voted for | `CConfereVotoEmCargo`, `cpartecargos.cpp` |
| turno | election round (1st or 2nd) | `CConversorTurno`, `target/manifest.json` |
| resultado urna cadastro (RUC) | the voter-register result data | `src/ecourna/app/dados/resultadournacadastro/`, rounds `R-RUC-*` |
| conversor | an ASN.1 ⇄ model converter class (`CConversor*`) | `app/dados/asn/`, `comum/asn/` |
| mídia | the storage media the urna's software and data are loaded from | `src/ecourna/app/dados/midias/`, round `R-MIDIAS` |

*Where:* `docs/reference/appmap.md` §2–§3 has the subsystem tour.

## Component and owner

Two schemes label each target function with where it belongs; they disagree on thousands of functions.

**component** — the attribution of `tools/extract/attribute.py`: a function's own evidence (a `__FILE__` or
`source_location` string, a vtable slot, a vptr store, a typeinfo reference, an owner-specific string), else the one
component all its attributed callers share, iterated to a fixpoint. Because of the caller step, a component says who
uses a function, not which library its code comes from: libc's `pad` (target 788) is `RHVoice`. Own evidence can
point away from the code's tree too: a uenux2 function that throws ecourna's `CBaseError<E, …>` template with a
uenux2 enum `E` has an ecourna typeinfo reference, so it is component `ecourna` (238 such functions, among them
`1708 comum::CCargos::Next`), while its source and its stub are in `src/uenux2/`.
*Where:* `ledger/func-attribution.tsv`, `ledger/functions.tsv` column `component`, `docs/reference/appmap.md` §1, the splat
pieces and meter (`docs/reference/splat.md`), `docs/reference/link-order.md` §7.4, `docs/reference/tu-slices.md`, lift-eval sampling and the
benchmark rows' `component` field.

**owner** — the coarser library class of `tools/libs/classify.py`, stored in `tools/libs/cls2.json`: `app`,
`openssl`, `sqlite`, `7zip`, `rhvoice`, `libcxx`, `asn1`, `boost`, `libc`, `zlib`, or `-`. It was seeded from
library strings and vtable namespaces during the library census. *Where:* `ledger/functions.tsv` column `owner`,
`pnpm wasm progress --by owner` (the command's default) and so `build/out/census/progress.md`, `pnpm wasm fn --owners`, the benchmark rows' `owner` field.

**The scheme to use is component.** The official meter (the splat build) groups by it, it separates the TSE code
into its parts, and it is what `docs/reference/appmap.md` describes. The splat build only falls back to the owner, spelled as
a component (`libcxx` → `libc++`), when a function has no component; its `app` group is those owner-`app` functions.
Say "library" when you mean which library's source a function comes from; for a function our build links, the
benchmark's setup (decided from the object that defines it) answers that exactly.

**origin, TSE code** — the meter's three groups of components (`docs/status.md` "By origin", written by
`tools/round/status.py`): **TSE code** (`vota`, `comum`, `api`, `simulador`, `vota-web`, `ecourna`, `asn1-modules`,
`ASN1`: the code this project exists to recover; "TSE's own code" means the same), the **upstream libraries**
(rebuilt from their own sources) and **not attributed** (`unknown`, `app`). An origin is a sum of components, so it
inherits their caller rule: a library function only TSE code calls counts as TSE code.

## The build

**pinned image** — the docker image `urna-emsdk:6.0.0`: `emscripten/emsdk:6.0.0` plus wabt, built and run by
`build/docker-build.sh`, which mounts the repo root at `/work`. Every compile goes through it. Run it with
`TMPDIR=/tmp` on macOS, because docker cannot see the default `/var/folders` temp directory.
*Where:* `build/Dockerfile`, `build/README.md`, `docs/reference/toolchain.md` §1.

**skeleton** — our whole-module link of `src/Makefile`: the libraries at their pinned flags, the app stubs and roots,
and every landed unit. A unit lands by being added to it. *Where:* `src/Makefile`, `src/README.md`,
`tools/round/try-tu.sh`.

**plain link** — the skeleton's real output, `build/out/wasm/vota_web/vota_web_wasm.{js,wasm,data}`, the one that is
byte-comparable with `target/`; its exact command is in `link.cmd` beside it. *Where:* `src/README.md`,
`docs/reference/splat.md` "our link".

**named twin** (also "the twin", "symbol-map twin") — a second link of the same objects with `--emit-symbol-map`, in
`build/out/wasm-named/vota_web/`. The flag changes the `-Oz` function order, so the twin is used only for names and
is never byte-compared. The ledger label `skeleton=plain@twin` names the plain link's functions through it.
*Where:* `src/Makefile` (`OUT_NAMED`), `tools/round/census.sh`, `asmlift-wasm/README.md` "ledger label=plain.wasm@twin.wasm".

**link map** — `make -C src link-map` reruns the plain link's command with `-Wl,--Map`, checks the relink is
byte-identical, and names every final function (`build/out/wasm-map/vota_web/names.tsv`). *Where:* `tools/round/link-map.sh`,
`src/mk/splat-map.mk`, `docs/reference/splat.md` §2.

**stub** — two meanings. (1) A generated comment for a TSE function not decompiled yet, where the evidence says it
sat in TSE's file: `// f7010 · 683 B · unmatched · name unknown` over its signature, between `// <stubs>` and
`// </stubs>` markers. `tools/round/stubs.py` writes them (`make -C src stubs`, run by `splat-match`, so a matched
function's stub goes) and CI checks them (`stubs.py --check`); a function no evidence places is in its tree's
`unplaced.cpp`. A function the generator finds code of ours for has none, and a pending member counts as code of
ours, so it loses its stub when its unit lands. The signature line names the function only when the symbol file
does; otherwise it is `f<index>` with the wasm types. A stub is a comment, not code: it puts nothing into the link
(see **INCLUDE_ASM**). *Where:*
`src/README.md` "Stubs", `docs/how-to-decompile.md` steps 1, 3 and 9. (2) A hand-written
stand-in for an exported function whose body is not reconstructed: `src/app/vota_init.cpp`, `vota_api.cpp`,
`main.cpp`, `wasm_sound.cpp` (signatures from the target's export types; their order on the link line fixes the
export order). `src/app/stubs_7zip.cpp` supplies four symbols 7-Zip needs and emscripten lacks. *Where:*
`src/README.md`, `docs/reference/toolchain.md` §2 "Export order".

**gap** — a run of blank lines in a TSE source that holds the code below it at its original line numbers (the lines of
TSE's file we have not recovered), written as one blank line, `// (original lines 9-16: content not recovered)` and
`#line 17`. A run is a gap when it has 3 or more blank lines and a line a `std::source_location` record of the file
names below it; shorter runs are formatting. `tools/round/stubs.py` writes them with the stubs and checks them
(`stubs.py --check`); `stubs.py --strip` turns them back into blank lines for editing. *Where:* `src/README.md`
"Gaps", `docs/how-to-decompile.md` step 3.

**`#line`** — the preprocessor directive `#line N`, which numbers the next line N without changing the file name, so
it changes no byte of an object. The generator writes one after each gap comment and after a stub run longer than
the blank lines it took; the compiler, its diagnostics and the benchmark's reference lines then number the lines as
TSE's file did. No other `#line` belongs in the TSE sources (`stubs.py --check`). *Where:* `src/README.md` "Gaps".

**INCLUDE_ASM** — in other decomps, the macro that puts an undecompiled function's original assembly into the C
source so the build links it in place. This project has none: clang and wasm-ld would carry an included wasm body
through, but Binaryen's `-Oz` rewrites it and reorders the module, so the target's bytes are restored after the
toolchain by the splat build, and an undecompiled function is a comment stub. *Where:* `docs/reference/include-asm.md`
(the measurements and the decision), `docs/reference/splat.md`.

**roots, roots TU** — a translation unit whose only job is to keep functions alive in the link. Nothing in the
skeleton calls a reconstructed unit yet, so without a reference wasm-ld drops it and wasm-opt removes what survives.
A roots TU either calls the functions with the target's call-site counts (binaryen inlines a function called once)
or takes their address from a static constructor. Kinds in use: `src/app/roots.cpp` (reaches every library entry
point), `src/app/roots_ecourna.cpp` and `roots_uenux2.cpp`, the per-unit `src/units/*-roots.cpp`, the generated TU of
`tools/round/gen_try_roots.py` (used by `try-tu.sh` and `src/mk/asn1.mk`), and the library roots `sqlite_roots.c` /
`openssl_roots.c` generated by `tools/libs/openssl/gen-probe-main.py`. *Where:* `src/README.md`,
`tools/round/gen_try_roots.py` docstring.

**unit, unit fragment, landed** — a unit is one reconstructed source file (or a small group of them). It is landed
when its sources are under `src/` and a fragment `src/units/<name>.mk` adds them to `ECOURNA_SRCS` or `UENUX2_SRCS`
(or adds its own rule and appends to `EXTRA_OBJS`), so the skeleton links it. *Where:* `src/units/README.md`,
`src/Makefile` (the comment above `SKIP_UNITS`).

**placeholder, pending TU** — a landed TU that holds definitions belonging elsewhere. `src/units/14-uenux2-ctextfieldmultiline-pending.cpp`
defines the `CTextFieldMultiLine` members not reconstructed yet with `__builtin_trap()` bodies, because the class's
vtable needs them defined; each moves into the real source when reconstructed. Such a member has no stub, so its
pending file's header is the note that it is left to do. `src/units/12-ecourna-api-sql-sqlite-externs.cpp` holds
reconstructed `CStringUtils` functions that belong to a unit not landed yet. *Where:* those files' headers,
`src/units/14-uenux2-ctextfieldmultiline.mk`, `src/units/12-ecourna-api-sql-sqlite.mk`.

**stand-in** — two uses. In the benchmark, a hand-written root or stub object (`src/app/*`,
`src/units/*-roots.cpp`) that also defines a name, which disqualifies that name from deciding a row's setup
(`asmlift-wasm/benchmark/README.md` "How a row's setup is decided"). In the lifter, a compilable spelling of a
construct that has no closer C++ form, marked `/* wasm: … */` (`asmlift-wasm/README.md` "lift").

**COMDAT** — a section an object may define that the linker keeps once however many objects define it: inline
functions and template instantiations. Whichever object wasm-ld reads first supplies the copy, which is why a roots TU
declares what it calls instead of instantiating it. *Where:* `src/units/README.md` "Companion files".

**D0, D1, D2** (in mangled names `…D0Ev`, `…D1Ev`, `…D2Ev`) — the Itanium C++ ABI's destructor variants: D0 the
deleting destructor (destroys, then frees), D1 the complete-object destructor, D2 the base-object destructor. clang
often makes D1 an alias of D2, so the link carries only the D2 name. The constructors' C1 and C2 are the same pair.
*Where:* `splat/symbol_addrs.txt` hand edits.

**compile setup, component flags** — one set of compile flags that produced target code (uenux2, ecourna PIC and
non-PIC, asn1, the libraries, emscripten's system libraries). The benchmark registry `asmlift-wasm/benchmark/dataset/setups.json`
lists them and `pnpm bench setups --check` re-derives them from the build. *Where:* `asmlift-wasm/benchmark/README.md`,
`docs/reference/toolchain.md` §3.

**PIC / non-PIC** — whether a TU was compiled with `-fPIC`. In ecourna it varies per file: `app/dados`, `api/io` and
`api/compression` are non-PIC (a table address folds into the load offset), `api/security/base64.cpp` is PIC.
*Where:* `src/mk/ecourna-flags.mk`, `src/units/03-ecourna-api-io-util.mk`, `src/units/06-ecourna-api-compression.mk`, `docs/reference/toolchain.md` §3.

**glue** — the JS file emscripten writes beside the module (`vota_web_wasm.js`); at `-Oz` the module's import and
export names are minified (`a.o`, `Cb…Yb`) and the glue is where their real names are read from.
*Where:* `docs/reference/toolchain.md` §2, `asmlift-wasm/README.md` "What is what" (imports).

**EM_JS** — JavaScript function bodies compiled into the C++ objects; the app's `js_*` / `wasm_input_*` functions
are EM_JS. `src/web/wasm_shim.cpp` holds all 34, generated from `src/web/library_vota.pretty.js` by `tools/extract/gen_wasm_shim.py`.
*Where:* `docs/reference/toolchain.md` §2 "App JavaScript", `build/README.md`.

**invoke thunk** — an `invoke_*` import through which legacy JS exception handling (`-fexceptions`) makes a call
inside a `try`. Whether a function calls one tells whether its TU had `-fexceptions` (ecourna does, uenux2 does not).
*Where:* `docs/reference/toolchain.md` §2–§3, `docs/reference/app-flags.md` "Exceptions".

**harness probe** — the hello-world program in `build/probe/` used to pin the toolchain and link flags; its link
`build/probe/out-Oz/` is the ledger candidate labelled `probe`. *Where:* `build/README.md` "Probe",
`tools/round/census.sh`.

**probe (other uses)** — an isolated build of one library scored against the target (`tools/libs/<library>/`); a
flag-sweep build in `tools/round/appflags/probe.sh`; the Boost probe stand-in `roots_boost.o`.

**sweep** — compiling the same reconstructed units under every plausible flag set and scoring each against the
target, to decide a flag by measurement. *Where:* `docs/reference/app-flags.md` "Measuring a flag",
`docs/reference/facts/app-flags.md`, `tools/round/appflags/`.

**third_party** — the upstream library sources at the exact versions the target links (gitignored): git checkouts,
the SQLite 3.50.4 amalgamation, the 7-Zip 21.02 source release and the Boost 1.89.0 headers; the library builds
compile them there and write their objects under `build/out/` (OpenSSL is configured out of tree, in
`build/out/openssl/O3`). `third_party/MANIFEST.md` records each version, how it is fetched, its commit or sha256 and
the evidence that pinned it.

## Pairing and progress

**census** — three related things. The toolkit command `census` pairs every function of one module with one of
another by fingerprint, then addr, then shape (`asmlift-wasm/README.md` "census"). `make -C src census`
(`tools/round/census.sh`) runs it on the skeleton, rewrites `ledger/functions.tsv` and writes a snapshot and
`build/out/census/{report,progress}.md`.

**census tiers, pairing tiers** (the ledger's `best_kind`) — how a target function pairs with a candidate function,
strongest first. The census pairs; only the splat build matches, so a tier is never called a match:

| tier | holds when |
| --- | --- |
| `byte` | the two bodies are the same bytes |
| `fingerprint` | the bodies are equal after relocatable operands are replaced by what they refer to (callees by their own fingerprint, data by segment content) |
| `addr` | equal once every address is blinded (`i32.const` and memarg offsets ≥ 1024) |
| `shape` | equal once every constant is dropped |
| `none` | no pairing |

These are diagnostics: a `byte` or `fingerprint` pair is not a match, and only a match counts.
*Where:* `asmlift-wasm/README.md` "normalize + fingerprint" and "ledger", `ledger/functions.tsv`.

**depth** — how many call levels a fingerprint covers (`--depth N`, default 2): the hash is iterated over callees, so
depth 0 is the body alone and depth 2 also identifies the bodies within two calls. A callee that differs only by an
address demotes its caller from fingerprint to addr at depth ≥ 1. *Where:* `asmlift-wasm/README.md`,
`tools/round/census.sh`.

**trivial body** — a body of 8 bytes or less. Many functions share such bodies, so pairing them says nothing about
identity, and binaryen merges identical ones; meters set them apart. *Where:* `docs/reference/splat.md`
"How to read the meter", `docs/status.md` "Reading the meter".

**score, rows, structural vs operand-only** — `score` diffs two functions' normalised rows (one per instruction) and
counts the rows that differ; 0 means identical after normalisation. A replaced row with the same opcode is
operand-only; an inserted, deleted or opcode-changing row is structural, which is what flag sweeps count.
*Where:* `asmlift-wasm/README.md` "score", `tools/round/appflags/compare.sh`.

**ledger** — the folder `ledger/` of tables derived from the target (strings, classes, vtables, call graph,
attribution, ordinals, per-function pairing state). Its central file `ledger/functions.tsv` (also "the ledger") has one
row per target function: `index size uses owner component best_label best_index best_kind best_score best_name`,
written by the toolkit's `ledger` command. The `best_*` columns describe the function's **best pair**: the
candidate function the census paired it with (its module's label, index and name), the tier and the score. A best
pair is not a match. *Where:* `ledger/`, `docs/reference/appmap.md` (artefact table), `asmlift-wasm/README.md`
"ledger".

**meter** — the tally `TMPDIR=/tmp make -C src splat` prints: matched functions and code bytes, in total and per
component. Its count of matches in `splat/matched.txt` is the project's one progress number; `docs/status.md` has it,
written by `make -C src status`, and the playground's Progress page draws it. *Where:* `docs/status.md`,
`docs/reference/splat.md` "The meter".

**progress** — two things with the one name. (1) The toolkit command `pnpm wasm progress` (also the end of
`make -C src census`, and its `build/out/census/progress.md`): the census tiers per owner or component with the delta
since the last snapshot. Despite its name it is a diagnostic, not the meter. *Where:* `asmlift-wasm/README.md`
"progress". (2) The playground's Progress page: the meter drawn, the matches per origin, component and function and
their count over time, built from `splat/scan.tsv` and the history of `splat/matched.txt`. *Where:*
`asmlift-wasm/playground/README.md`. "Progress" alone, and "the progress number", mean the meter.

**snapshot** — a copy of `ledger/functions.tsv` saved as `ledger/history/<stamp>[-label].tsv` by
`progress --snapshot`, so the next run can print what moved. *Where:* `tools/round/census.sh`, `ledger/history/`.

**match, matched, match list** — a match is a line `<target index> <our symbol>` in `splat/matched.txt` (the match
list): a promise that our build produces exactly that target body, which the splat build verifies on every run (see
the next section). A matched function is a target function with such a line, and the meter counts them. The
build also lists the matches it could check only in part (*kind-unverified*) and fails when there are more of them
than its limit allows.
`make -C src splat-match` adds the scan's new `match` verdicts to the list (`docs/how-to-decompile.md` step 9). The
census never matches, it pairs (see **census tiers, pairing tiers**). The target functions a candidate TU is
expected to produce are its *targets* (`try-tu.sh --targets`, the toolkit's `targets` command), not matches. A
board's `rounds/` *claim* is unrelated: it reserves paths (see **board**). *Where:* `splat/matched.txt`, `docs/reference/splat.md`,
`docs/how-to-decompile.md` step 9.

## The splat build

The splat build is the official progress check: `TMPDIR=/tmp make -C src splat` rebuilds the target from its own
pieces with our bytes for every matched function and fails unless the result has the target's sha256.
`docs/reference/splat.md` is the full description. The name comes from splat, the tool console decomps use to
split a ROM into pieces; here the re-assembly runs after the toolchain, because nothing fed through `em++` comes out
unchanged (`docs/reference/include-asm.md`).

**split, piece, manifest** — `make -C src splat-split` cuts the target into pieces under `build/out/splat/pieces/`:
one `asm/<group>/<index>-<name>.bin` per function body (with a `.wat` beside it for reading), one
`data/<nnnn>-<addr8>.bin` per data segment, and `manifest.json` for everything else. Key a function by its index,
never by its piece path: the name is the ledger's guess and can change. *Where:* `docs/reference/splat.md` §1,
`src/mk/splat-build.mk`.

**group** — a piece's directory: the function's component, else its owner spelled as a component, else `unknown`.
The splat meter's "component" column is this group. *Where:* `asmlift-wasm/src/splat.ts` (`pieceGroups`),
`asmlift-wasm/README.md` "splat".

**symbol file** — `splat/symbol_addrs.txt`, the target-side names with their evidence:
`<name> = func:N; | import:N; | 0xADDR; // … evidence:… by:…`. Lines after the `# ---- hand edits below this line`
marker survive regeneration; a hand edit needs evidence that does not come from the match it enables.
*Where:* `docs/reference/splat.md` §2, `make -C src splat-symbols`.

**evidence kinds** — why the symbol file names something: `glue` and `export` (names from the target's glue),
`pair-fingerprint` and `pair-addr` (a census pair unique on both sides), `call` and `table` (aligned callees and
function pointers), `vote:N` (aligned address operands; `by:` lists up to four voters), `rtti`, `content`, `string`,
`srcloc`, `srcloc-anchor`; the hand edits name their own (`vtable-slot`, `rtti-vptr`, `rtti-operand`,
`pair-depth0`, `callers+content`). *Where:* `docs/reference/splat.md` §2, `asmlift-wasm/src/splat-symbols.ts`.

**re-pointing** (`splat-remap`) — rewriting one of our function bodies into the target's index and address spaces:
callees, `call_indirect` types, globals, imports, table slots and data addresses, through our link map, the
operand-kind table and the symbol file. A target function is **matched** when the re-pointed body is byte-identical
to its piece and nothing in the re-pointing read a target body. *Where:* `docs/reference/splat.md` §3,
`asmlift-wasm/README.md` "splat symbols".

**operand kind, shifted link, kind-unverified** — whether a constant of ours (an `i32.const`, an `i64.const` or a
memarg offset) is an address, a table slot or a plain integer (a *literal*). The final module does not say, so `make
-C src link-kinds` links the same objects again with the data moved by 1 MiB and the table by 20,000 slots (the
*shifted link*, `build/out/wasm-kinds/vota_web/`). In a function with the same instructions in both links, a
constant that moved by the data shift is an address, by the table shift a table slot, and not at all a literal;
`kinds.json` records it. A function the table cannot vouch for (another shape in the shifted link, a constant moved
by neither shift, a sum of the table base or of a table slot it cannot follow) is *kind-unverified*: its operands
keep every reading, and the build lists it and fails on more of them than `SPLAT_MAX_KIND_UNVERIFIED` (0). *Where:*
`docs/reference/splat.md` §3 "Operand kinds", `tools/round/link-kinds.sh`, `asmlift-wasm/src/splat-kinds.ts`,
`docs/reference/facts/splat.md` §S5.

**reading** — one target value an operand of ours may re-point to. Each operand is offered only the readings of its
kind: a literal its own bytes; an address the target place of what it points at, plus every other place holding the
same content (a *copy*); a table slot the target slot that holds its function, or each of them when binaryen merged
identical functions into one that fills several slots (up to 64; a function in more slots leaves the operand
unresolved). The target body picks only among copies. The meter's "readings the target chose" lines (re-pointing
alone / another reading / which copy) count how often it had to pick; "another reading" (the target body picking a
kind) happens only in a kind-unverified match. A data object the symbol file does not
name is placed by its content: a C string, a `source_location` record, exact read-only bytes, or its bytes with each
pointer word re-pointed (`content-repointed`). *Where:* `docs/reference/splat.md` §3 and "The meter".

**review warning** — a literal operand of a match that may be a target number written into our source (`past-data`,
`target-object`, `past-table`). An integer is compared as itself, so the bytes agree and no check can reject it; the
build lists them for a reviewer and never fails on them (all in upstream libraries). *Where:*
`docs/reference/splat.md` "Reviewing a match".

**scan verdicts** — `make -C src splat-scan` writes `splat/scan.tsv` (committed: the work list) with one verdict per
target function: `match`, `differs` (with the first differing instruction), `unresolved` (an operand with no target
image), or `no-candidate` (no body of ours was found for it: no symbol-file name, no census pair, no content pair; the
function may still be written, see `docs/how-to-decompile.md` step 1). `make -C src splat-match` appends the new
`match` rows to `splat/matched.txt`, so after it the `match` rows are exactly the match list. The scan's columns
(`ours`, `source`, `tried`, `object`, …) are explained in `docs/how-to-decompile.md` step 1. In prose, "no candidate"
is the `no-candidate` verdict. *Where:* `docs/status.md` "What is left", `docs/reference/splat.md` "Matching a
function".

**self-voted** — a data name whose only vote came from the function being matched, so it proves nothing about that
match. Such a match holds only if our object's bytes agree with the target's at that address, or another holding match
uses the name; otherwise the scan reports `unresolved (self-voted)` and the build `UNCONFIRMED`.
*Where:* `docs/reference/splat.md` §3.

**drift** — a match that stops verifying because a source, flag, link-order or toolchain change altered our body. The
build exits 1 and names each failing match; the fix is the source, or deleting the match line with a reason in the
commit message, never a piece or symbol-file edit. *Where:* `docs/reference/splat.md` "The drift rule".

**twin in the target** — a matched body that is byte-identical to another target function (binaryen merges identical
bodies), so which of the twins a match names is arbitrary. *Where:* `docs/status.md` "Reading the meter" (the count),
`docs/reference/splat.md` "How to read the meter".

**grouped** — the split's log line `14343 functions (6255 named, 12820 grouped; …)` counts the pieces that got a name
from the glue or the ledger and the pieces filed under a group (the component, else the owner); the rest go to
`asm/unknown/`. *Where:* `make -C src splat-split`, `docs/reference/splat.md` §1.

## Link order

The order of functions in the target's code section encodes the order of objects on TSE's link line.
`docs/reference/link-order.md` derives and uses this.

**use count, uses** — a function's direct `call` sites plus function-table entries plus exports. binaryen's
`reorder-functions` (run by `wasm-metadce` at `-Oz`) sorts the code section by it, descending. *Where:*
`ledger/functions.tsv` column `uses`, `docs/reference/link-order.md` §1.

**band** — all functions with the same use count; the `uses=1` band is the largest and the most informative.
(The benchmark and lift-eval also use "size band": tiny ≤ 32, small ≤ 128, medium ≤ 512, large ≤ 2048 bytes, huge.)
*Where:* `docs/reference/link-order.md`, `docs/reference/tu-slices.md`, `asmlift-wasm/benchmark/README.md` (size band).

**ordinal** (link ordinal) — a function's defined index after `wasm-opt -Oz`: wasm-ld's input order (each object's
live functions in the object's own order, then archive members) compacted by inlining and merging, with the mgfn
thunks appended last. *Where:* `ledger/ordinals.tsv`, `docs/reference/link-order.md` §1, `tools/linkorder/linkorder.py` docstring.

**order law** — inside a band, functions are ordered by the decimal string of their ordinal, descending; across bands
nothing orders them. So a band reads as reverse link order with lower digit counts interleaved. Verified with 0
exceptions on the skeleton. *Where:* `docs/reference/link-order.md` §1, `src/Makefile` header comment, `docs/reference/appmap.md` §6.

**digit zone, zone twin** — the ordinals with the same number of digits; a zone twin is an ordinal that sorts next to
one of another zone ("293" sits right after "2936"), so a placement can flip between the two. *Where:*
`docs/reference/link-order.md` §1 and §7.4, `docs/reference/tu-slices.md`.

**rank** — here, the position of an ordinal's decimal string in the descending string order of all ordinals; the
solver's sound windows are rank ranges (`rank_lo`, `rank_hi`). Not to be confused with the toolkit's compile-and-rank
(`pnpm wasm rank`). *Where:* `ledger/ordinals.tsv`, `tools/linkorder/linkorder.py`.

**mgfn, binaryen thunks** — the `byn$mgfn-shared$*` thunks that binaryen's merge-similar-functions pass adds after
`-Oz` inlining: one shared body that several functions call with their differing constants. They take the highest
ordinals (189 in the frozen skeleton of `docs/reference/link-order.md` §7.1). `link-map.sh`'s log counts them as "binaryen thunks", and the
scan's `object` column says `<binaryen>`. *Where:* `docs/reference/link-order.md` §1, §5 and §7.

**frozen skeleton** — a copy of one plain link and its `ledger/functions.tsv` kept under
`build/out/LINKORDER2/frozen/`, so a link-order measurement keeps running on the same link while the tree moves on.
*Where:* `docs/reference/link-order.md` §6–§7.

**sound window, `--holes`, `--repair`** — the link-order solver's output per target function is a window of ranks
(`rank_lo`…`rank_hi`) that provably contains its ordinal when the anchors are right: "sound". `--holes H` allows H
ordinals no surviving function takes; `--repair R` drops the anchors of the smaller chunk of each inconsistent link
and solves again, up to R times. *Where:* `docs/reference/link-order.md` §7.3, `tools/linkorder/linkorder.py`.

**anchor, chunk, floater** — in link order: an anchor is a target function identified with a skeleton function whose
object and position in that object are known; a chunk is a rigid run of anchors at adjacent positions; a floater is
a function in a band too small to be ordered against anything. (The splat symbol file's `srcloc` anchors are a
different thing: target/ours pairs that share a unique `source_location` record.) *Where:* `docs/reference/link-order.md`
§2–§3 and §7, `asmlift-wasm/README.md` "splat symbols".

**position-gap law** — for two live functions of one object at positions p < q in its full function list, their
ordinals differ by 1 to q − p. It lets an anchor be placed from the `.o` alone. *Where:* `docs/reference/link-order.md` §7.1.

**stretch, TU slice** — a run of functions inside one band that probably come from one source file, found by
`tools/linkorder/tu_slices.py` from `source_location` records and vtable classes. *Where:* `docs/reference/tu-slices.md`,
`ledger/tu-clusters.tsv`, `docs/reference/link-order.md` §7.5.

**link line** — the ordered list of objects passed to `em++` at link time. Ours is `LINK_OBJS` in `src/Makefile`,
changed only on evidence; the recovered TSE order is in `docs/reference/link-order.md` §7.4.

## The toolkit

**asmlift-wasm** (`@asmlift/wasm`) — the vendored WebAssembly matching toolkit this project is decompiled with, run as
`pnpm wasm <command>` from `asmlift-wasm/`; scripts reach it through `WASM_KIT` (default `asmlift-wasm/`) and call
it `kit`. *Where:* `asmlift-wasm/README.md`, `README.md`, `tools/round/census.sh`.

**readout, folded readout** — `pnpm wasm fn` prints a function's disassembly beside its normalised rows and a folded
readout: the operand stack replayed into expressions. It is a reading aid, not a decompiler. *Where:*
`asmlift-wasm/README.md` "What is what".

**bundle** — the same readout for a group of functions in one Markdown file, `ledger/bundles/<name>.md`.
*Where:* `asmlift-wasm/README.md` "The decompile loop", `ledger/bundles/`.

**lift** — `pnpm wasm lift` writes compilable C++ for a function: a structural starting point to rewrite, never code to
commit (its `volatile` stores and empty `asm` barriers exist to steer clang). *Where:* `asmlift-wasm/README.md`
"lift", `docs/improving-the-lifter.md`.

**rank, rank adapter** — `pnpm wasm rank` compiles candidate sources in the pinned image and orders them by score
against one target function (asmlift core's `rankBy`); `try-tu.sh --single` uses it to score one function compiled
alone. *Where:* `asmlift-wasm/README.md`, `src/README.md`.

**try-tu** — `tools/round/try-tu.sh`, the inner loop of reconstruction: compile a candidate TU, link the whole skeleton
with it, and report what moved (or, with `--single`, score one function compiled alone). A lone link inlines
differently from the program, so `--single` is for iterating and the whole link for concluding.
*Where:* `src/README.md` "Trying a candidate", `docs/how-to-decompile.md` steps 4 and 5.

**lift-eval** — lifts, compiles and scores a stratified sample of target functions to measure the lifter; output goes
to the untracked `ledger/lift-eval/<label>/`. *Where:* `asmlift-wasm/README.md`.

**benchmark row, features, outcome, raw / -Oz score** — the lifter benchmark (`pnpm bench`) scores about a dozen
target functions per compile setup. A row is one target function (`<setup>/<index>` in
`asmlift-wasm/benchmark/dataset/rows.json`), its features are read off the target body (`leaf`, `loop`, `invoke`, …),
its outcome is how far it got (`exact`, `differs`, `compile-failed`, …), the raw score compares the object and the
-Oz score compares it after the target link's wasm-opt passes. The outcome `exact` means structurally equal after
normalisation, never byte-identical, and it is not a match.
*Where:* `asmlift-wasm/benchmark/README.md`.

**lift notes** — the lines `pnpm wasm lift` prints under its summary, one per workaround a lifted file contains,
with how many functions carry it: `goto` (a branch no structured statement expresses), `arm tail kept from the join`
(a `KEEP_IN_ARM()` barrier keeps an arm's last call or store from being sunk into the join), `frame object escapes
at entry` (an empty `asm` takes a frame object's address so LLVM cannot assume calls leave it alone), `inlined
callee outlined` (a callee wasm-opt inlined is lifted as a `static noinline` helper), `constant address hidden from
alignment inference` (`opaque_addr`), `try`, `unused frame`. lift-eval's **stand-ins table** counts the functions
carrying each note. *Where:* `asmlift-wasm/README.md` "lift".

**mangled pairs** — the lifter's count of demangled names it can spell as a mangled symbol, from the two line-aligned
lists `make -C src link` writes (`build/out/wasm/vota_web/symbols.{mangled,demangled}.txt`). *Where:*
`asmlift-wasm/README.md` "lift", `tools/round/link-symbols.sh`.

**frozen dataset, dataset version, selection, references update (benchmark)** — the benchmark's rows
(`asmlift-wasm/benchmark/dataset/rows.json`) are selected once and then frozen: a new unit or census does not
re-select them, so every run compares with the committed baseline, which a test ties to the rows' sha256.
`dataset/selection.json` records the dataset's `version` and the commit and inputs its selection read, so the
selection can be re-run and checked. The rows change only by `pnpm bench add`, by a references update
(`pnpm bench select --references --write`: same rows, moved sources or objects) or by a deliberate re-selection
(`pnpm bench select --write`), each with a fresh baseline; the last two make a new version. *Where:* `asmlift-wasm/benchmark/README.md`
"The dataset", `docs/improving-the-lifter.md`.

**`evidence: component`, `unverified`, `drift` (benchmark)** — a benchmark row's `evidence` says how its setup was
decided: `symbol` (the object that defines its ledger name) or `component` (uenux2 rows, from the ledger's
attribution, with no object). `pnpm bench setups --check` prints a setup as `unverified` when only the image or a
library build could confirm its flags, and as `drift` when the build's flags are not the registry's; only drift
fails. *Where:* `asmlift-wasm/benchmark/README.md`, `.github/CI.md`.

**playground** — the web app in `asmlift-wasm/playground/` that shows the meter and its history (the Progress
page), any target function's WAT, readout, lift and ledger data, the benchmark runs, and a link to UrnaEmu, the
emulator that runs the same module. *Where:*
`asmlift-wasm/playground/README.md`, deployed by `.github/workflows/pages.yml`.

## Process

These words describe how a decompilation run is organised: `.claude/commands/decompile-functions.md` runs one, and
`docs/agents.md` has its rules.

**workflow, stage** — one orchestrated run of several agents; a stage is one agent's assignment in it (PICK, MATCH,
VERIFY, LAND and DOCS in a `/decompile-functions` round).

**round** — one reconstruction or measurement task under one handle, such as reconstructing one ecourna directory and
landing it. A `/decompile-functions` run is a round with one lane per target function. The directory `tools/round/`
keeps the name: its scripts are the decompile loop any contributor runs.

**lane** — in a `/decompile-functions` round, one target function with its own worktree, its branch
`decomp/<lane>`, its matcher and its verifier.

**handle** — the short name of a round or lane, used in commit messages, file names and board posts (`<ROUND>-A`).
Handles of the tasks that built the repository appear in comments, in generated file names under `ledger/bundles/`
and `build/out/`, and in commit messages:

| handle | the task |
| --- | --- |
| `TOOLCHAIN`, `APPMAP`, `BUILD` | toolchain pinning (`build/README.md`), the application map (`docs/reference/appmap.md`), the skeleton link (`src/README.md`) |
| `APPFLAGS`, `APPFLAGS2` | pinning the ecourna and uenux2 compile flags (`docs/reference/app-flags.md`, `src/units/01-uenux2-api-util-io-ipc.mk`) |
| `ASN1`, `ASN1-2`, `ASN1PORT` | the ASN.1 runtime fork, its generated modules, and their port to C++20 (`src/asn1/README.md`, `src/mk/asn1.mk`, `src/mk/ecourna-flags.mk`) |
| `LINKORDER`, `LINKORDER2` | the offsets and positions solves of `docs/reference/link-order.md` (`build/out/LINKORDER2/` is the positions solve's scratch) |
| `R-<area>` | ecourna reconstruction, named for the area rebuilt: `R-SEC` security, `R-CEPESC` the CEPESC cipher envelopes, `R-COMP` compression, `R-SQL` the SQLite DAO, `R-APIMISC` api io/util, `R-DADOS` `app/dados` value types, `R-MIDIAS` media descriptors, `R-RUC-DATA` / `R-RUC-CONV` the voter-register result data and converters, `R-ASNCORE` the core ASN.1 converters |
| `W-<n>`, `FIVE-<n>` | single-function tasks named for the target function index they matched (`W-1931`, `W-5498`, `FIVE-700`) |
| `SPLAT-SYM` | the splat symbol maps (`src/mk/splat-map.mk`) |

The `src/units/` fragments are named by their content, not by a handle (`src/units/README.md` "Names").

**board** — the folder outside the repository through which a run's agents coordinate (append-only facts, traps,
round posts and frictions, plus the user's inbox). A `rounds/` *claim* there is a post reserving the paths a stage
owns (`docs/agents.md`), unrelated to a match. Facts that tracked files cite are kept in `docs/reference/facts/`.

**friction** — a board post about a point where a doc was missing, wrong, ambiguous or misleading: what the doc led
the agent to expect and what happened instead. A round's DOCS stage turns each one into a doc change
(`.claude/commands/decompile-functions.md` "The friction log").
