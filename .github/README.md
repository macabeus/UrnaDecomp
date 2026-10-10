# CI

`workflows/ci.yml` runs on every push to `main` and every pull request. Its `repo` job runs three checks
from the repository root:

| step | what it checks |
| --- | --- |
| `python3 -I -B tools/round/status.py --check` | the committed scan `splat/scan.tsv` matches exactly the claims in `splat/matched.txt`, and every generated block in `README.md` and `docs/status.md` is what that scan gives (`docs/status.md` "Regenerating this page") |
| `python3 -I -B tools/round/check_encoding.py` | every tracked file `.editorconfig` declares Latin-1 (TSE's sources) has not been re-saved as UTF-8 |
| `python3 -I -B tools/round/stubs.py --check` | every unclaimed TSE function with no code of ours has exactly one stub, in the file and at the place its evidence gives, with the text the generator derives, and no other function has one (`src/README.md` "Stubs"); every run of 3 or more blank lines that holds code at its original lines is a gap comment and `#line`, each the generator's ("Gaps") |

Its `asmlift-wasm` job runs, in `asmlift-wasm/`:

| step | what it checks |
| --- | --- |
| `pnpm install --frozen-lockfile` | the lockfile matches the workspace |
| `pnpm typecheck` | the toolkit, `tools/`, the benchmark and the playground typecheck |
| `pnpm test` | the toolkit's suite, against the committed `target/` and `ledger/` |
| `pnpm test:bench` | the benchmark's suite |
| `pnpm test:playground` | the playground's suite |
| `pnpm bench setups --check` | every compile setup still has the flags `src/Makefile` compiles its objects with |
| `pnpm playground:build` | the playground builds from `target/` and `ledger/` alone |

Every check runs even when an earlier one failed. The setup check and the benchmark's `setups` tests run
`make -n` over `src/Makefile`'s objects, and the zlib rules need zlib's sources, so the job first clones
zlib 1.3.1 into `third_party/zlib`. No other library is fetched. The checkout takes the whole history: the
benchmark re-runs its frozen dataset's selection on the ledger and registry at the commit
`asmlift-wasm/benchmark/dataset/selection.json` records.

## Skipped tests

Tests that need the toolchain image or the outputs of a build skip when those are absent. After each
suite, `scripts/test-counts.mjs` prints its passed, failed and skipped counts as an annotation and in the
job summary, with every skipped test listed. The suite fails when more tests skip than its budget in
`ci.yml`. The budgets are the counts in a fresh clone:

| suite | budget | what skips, and what it needs |
| --- | ---: | --- |
| `pnpm test` | 41 | 32 in `splat-sub` and `splat-symbols` (`build/out/wasm-map/` from `make -C src link-map`, and for `splat-sub` the pieces from `make -C src splat-split`); 4 in `lift` (`build/out/wasm/vota_web/symbols.mangled.txt`, which `make -C src link` writes); 4 in `lift`, `lift-eval` and `rank` (the image); 1 in `report` (the plain link, `build/out/wasm/vota_web/vota_web_wasm.wasm`) |
| `pnpm test:bench` | 9 | 6 in `dataset`, `run` and `setups` (the image); 2 in `dataset` (the library trees the rows' references point into: `third_party/` fetched, `build/out/` built); 1 in `setups` (the SQLite build's recorded flags in `build/out/sqlite/`) |
| `pnpm test:playground` | 0 | nothing |

A run that skips more than the budget has lost an input it used to find, such as the target or the
ledger, and its tests are passing by skipping. A run that skips fewer prints a notice to lower the budget.
When a change adds a gated test, raise the budget in the same change.

`pnpm bench setups --check` behaves the same way: a setup whose flags only the image or a library build
can confirm prints as `unverified`, and only a `drift` fails the step. In a fresh clone 10 of the 17
setups print as `unverified`.

## What CI does not run

- **The C++ build and the official meter.** `make -C src link` and `make -C src splat`, which must print
  the target's sha256 (`docs/reference/splat.md`), are not run. They need the `urna-emsdk:6.0.0` image
  (`build/Dockerfile`: `emscripten/emsdk:6.0.0` plus wabt), every library of `third_party/MANIFEST.md`
  fetched at its revision (including the 7-Zip 21.02 source archive and the Boost 1.89.0 headers), and a
  full link of the module. None of this is set up for a hosted runner, so a change to `src/`, a unit
  fragment or a flag is checked only by running the two make targets locally.
- **The tests gated on the image or on `build/out/`.** They skip, as the table above lists. So CI does
  not check that lifted code compiles with em++, the ranking of candidates, the benchmark's compile and
  score run, or the splat claim verifier.
- **The source_location check**, `make -C src check-srcloc`: it reads the records out of the app objects, which
  only the C++ build writes. The stub check above guarantees that stubs take no line of code, not that a
  decompiled function's throw sites sit at their recorded lines; run the make target after a link.
- **The census and ledger producers** under `tools/`, which read build outputs or the image.

A failure in CI is a real failure. A pass says nothing about the bytes of the build.
