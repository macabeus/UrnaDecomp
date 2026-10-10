# UrnaDecomp <img src="./asmlift-wasm/playground/src/assets/urnadecomp.png" align="right" height="125px" />

🗳️ _Matching decompilation of the TSE voting simulator_

> ⚠️ **Notice:** This is an independent matching decompilation of the TSE's unmodified files.
>
> **Matching decompilation.** UrnaDecomp rebuilds C++ source that compiles back, byte for byte, to `vota_web_wasm.wasm`: VOTA, the voting application of the TSE's voting simulator. The target files belong to the TSE and are used and redistributed unmodified.
>
> **Independent.** The files are the ones the TSE publishes in its Voting Simulator. The project is not affiliated with or endorsed by the TSE or the Electoral Justice.
>
> **Why.** To document, openly and verifiably, how VOTA works as the TSE publishes it in the simulator: anyone can rebuild the matched code and check it against the module, byte for byte.
>
> **No evidence of fraud.** Nothing in this project is evidence of electoral fraud, and it cannot be: it studies the simulator's module, not the software sealed into the voting machines, and says nothing about the result of any election. Presenting this code as evidence of fraud misrepresents it; where the reconstruction and the module disagree, the reconstruction is wrong.
>
> **Takedown.** The TSE or another competent authority can ask for this project, or the TSE files in it, to be taken down by opening an [issue on GitHub](https://github.com/macabeus/UrnaDecomp/issues).
>
> In the news: [Estadão / TecMundo](https://www.estadao.com.br/tecmundo/ciberseguranca/simulador-de-votacao-exibe-codigo-do-app-da-urna-eletronica-tse-afirma-intencao/) (in Portuguese). The TSE told the outlet that reusing the voting machine's C++ code in the WebAssembly simulator was a deliberate choice, that code reconstructed from it reveals no secrets, and that it does not intend to ask for its removal.

Brazil's electoral court, the TSE (Tribunal Superior Eleitoral), publishes a voting simulator that runs the urna's
voter-facing software in a browser. Its core is one WebAssembly module, `target/vota_web_wasm.wasm`: 7.7 MB and
14,343 functions, compiled by Emscripten from TSE's C++ (the `uenux2` application and the `ecourna` library)
together with upstream libraries such as OpenSSL, SQLite, 7-Zip and RHVoice.

This project rewrites that C++ so that a pinned Emscripten toolchain rebuilds the module byte for byte. Progress is
measured by the **splat build**: the target is split into pieces, every function whose body our build provably
produces is replaced by our own bytes, and the result must still be the target's sha256
`8579ef8d1a93819152cecdbf52906d2e4dd0f57999cb2e1e8c0d1161cd1007e2`.

The project is not affiliated with the TSE.

## Status

The progress is on the [web app](https://macabeus.github.io/urnadecomp): matched functions and code bytes per
origin, per component and per function, and how they grew over time. [docs/status.md](docs/status.md) has the same
numbers as tables.

A match is a line of `splat/matched.txt` that `TMPDIR=/tmp make -C src splat` verifies on every run; their count is
the only progress number this project reports. `docs/status.md` is written by `make -C src status` from the committed
scan `splat/scan.tsv`, and CI checks it.

A component records which part of the program calls a function, not which library its code came from
([glossary](docs/glossary.md#component-and-owner)); "not attributed" is mostly OpenSSL, SQLite and emscripten's own
system libraries whose callers the attribution could not name. [docs/status.md](docs/status.md) has every component,
what `unknown` holds, and what is left to do. The census tiers in `ledger/functions.tsv` (`byte`,
`fingerprint`, `addr`, …) are diagnostics that show what is close to matching; they are not progress.

## Quick start

[docs/getting-started.md](docs/getting-started.md) takes a fresh clone to a verified splat build: the host tools
(docker, Node 24, pnpm 9, python3, wabt, 7z), the upstream sources at their pinned revisions
(`tools/setup/bootstrap.sh`), the pinned image, the first link and the first splat build. The last two steps are:

```sh
TMPDIR=/tmp make -C src link     # our whole-program link, into build/out/wasm/vota_web/
TMPDIR=/tmp make -C src splat    # must print sha256 8579ef8d…1007e2 and the meter
```

`TMPDIR=/tmp` is needed on macOS: docker cannot mount the default `/var/folders` temp directory.

[docs/how-to-decompile.md](docs/how-to-decompile.md) then walks one function from the work list
(`splat/scan.tsv`) to a match. With Claude Code, the `/decompile-functions` command
([.claude/commands/decompile-functions.md](.claude/commands/decompile-functions.md)) runs a whole round: it picks
functions, matches each in its own worktree, has every match verified by a second agent, lands them on `main`, and
turns each point where the docs misled into a doc fix.

## Repository map

| path | what |
| --- | --- |
| `README.md` | this page |
| `target/` | the TSE files being rebuilt, unchanged, with where they came from and their sha256s (`target/README.md`) |
| `src/uenux2/`, `src/ecourna/` | the reconstructed TSE C++, at the paths the target's `std::source_location` records name, with a generated comment _stub_ where each function not decompiled yet sat and a _gap_ comment plus `#line` where original lines are not recovered (`src/README.md` "Stubs" and "Gaps"); functions no evidence places are stubbed in each tree's `unplaced.cpp` |
| `src/asn1/` | the III ASN.1 runtime as TSE forked it, and the generated `Modulo*` schema types in `src/asn1/modules/`, with the same stubs |
| `src/app/`, `src/web/` | stand-ins: the exported entry points as stubs, the roots that keep the libraries linked, the app's JS library and its EM_JS shim |
| `src/Makefile`, `src/mk/` | the build: compile flags per component, the link order, the library targets, the splat rules |
| `src/units/` | one Makefile fragment per landed unit, named by content (`src/units/README.md`) |
| `splat/` | the match list `matched.txt`, the scan `scan.tsv` (every target function's verdict: the work list) and the target's symbol file `symbol_addrs.txt`, all written by the splat tools |
| `ledger/` | what the project knows about the target, as data: strings, classes, vtables, call graph, attribution, per-function census state (`ledger/README.md`) |
| `tools/` | every script: `setup/`, `extract/` (reading the target), `round/` (the decompile loop), `linkorder/`, `libs/` (pinning each upstream library); `tools/README.md` describes each file |
| `build/` | the toolchain pin: `Dockerfile`, `docker-build.sh` and the link-line probe (`build/README.md`); its `out/` and `dump/` are generated and ignored |
| `asmlift-wasm/` | the decompilation toolkit, with its benchmark and playground (below) |
| `third_party/` | the upstream library sources at the versions the target links, fetched by setup and ignored; `third_party/MANIFEST.md` records each version, how it is fetched and its commit or sha256 |
| `docs/` | the glossary, the how-tos, reference pages and measured facts (index in `docs/README.md`) |
| `.github/` | CI: the toolkit's tests, typecheck and benchmark dataset checks |

## The toolkit: asmlift-wasm

`asmlift-wasm/` is the WebAssembly support built for this project on top of
[asmlift](https://www.npmjs.com/package/@asmlift/core), the matching decompiler. It parses and disassembles the
module, prints a readout of any function, lifts it to compilable C++, compiles and ranks candidates in the pinned
image, runs the census and the ledger, and implements the splat build. It is vendored here so that a clone has the
exact tool the project uses and a fix to the tool can land with the C++ it helped write. It is a fork of
asmlift 0.9.0, maintained here independently of asmlift. `asmlift-wasm/README.md` documents every command and its
tests.

```sh
cd asmlift-wasm
pnpm install
pnpm wasm fn ../target/vota_web_wasm.wasm 1931     # readout of target function 1931
pnpm test && pnpm typecheck
```

- **Benchmark.** `asmlift-wasm/benchmark/` measures the lifter on a frozen dataset of about a dozen target functions
  from each compile setup in the module, compiled at that setup's flags and scored raw and after `-Oz`. Run
  `pnpm bench run` before and after a lifter change and put `pnpm bench diff` in the pull request (`asmlift-wasm/benchmark/README.md`;
  [docs/improving-the-lifter.md](docs/improving-the-lifter.md) says what to optimise and lists the known gaps).
- **Playground.** `asmlift-wasm/playground/` is a web app with four pages: the decompilation progress (waffles from
  origins down to single functions, and the matches over time), any target function's WAT, readout and lifted C++,
  computed in the browser, with the commands to try it filled in, the benchmark's runs, and a link to
  [UrnaEmu](https://eleicoes.rlaneth.com/urnaemu/), the emulator that runs the same module (`pnpm playground`;
  `asmlift-wasm/playground/README.md`). `.github/workflows/pages.yml` deploys it to GitHub Pages on every push to
  `main`.

## Where next

- [docs/README.md](docs/README.md): every document, what it covers and how far to trust it.
- [docs/glossary.md](docs/glossary.md): the project's words, from TSE and uenux2 to match, twin and mgfn.
- [docs/improving-the-lifter.md](docs/improving-the-lifter.md): working on the decompiler itself.
- [docs/reference/splat.md](docs/reference/splat.md): how the splat build re-points and checks a function.
- [docs/reference/include-asm.md](docs/reference/include-asm.md): why there is no `INCLUDE_ASM`, and stubs instead.
- [docs/reference/toolchain.md](docs/reference/toolchain.md): how the target was built, flag by flag.
- [docs/agents.md](docs/agents.md): the house rules, and how several agents share one checkout.
