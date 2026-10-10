# @asmlift/wasm-playground

A web app over `target/vota_web_wasm.wasm`, the decompilation meter and the lifter benchmark, with four pages:
Progress (the default), Playground, Benchmark and UrnaEmu (a link to the emulator that runs the same module).
Above them, on every page, a folded notice (`src/shared/components/Notice.tsx`, in English and Portuguese): the
project is an independent matching decompilation of the TSE's unmodified files, why it exists, how the TSE or another
competent authority asks for its takedown (a GitHub issue), and the press coverage of the simulator's code.
`.github/workflows/pages.yml` deploys it to GitHub Pages on every push to `main` (`.github/CI.md` "Deployment").
Everything the Playground shows about a function is computed in the browser from the real module with the toolkit's
own modules (`parse`, `disasm`, `fold`, `lift`, `imports`, `strings`); nothing is precomputed per function and there
is no server. em++ does not run in a browser, so there is no in-browser compile: the page prints the local commands
that compile and score a function instead.

```sh
cd asmlift-wasm
pnpm install
pnpm playground          # dev server (prepares data/, then vite)
pnpm playground:build    # data/, tsc over the app and the tests, vite build → playground/dist/
pnpm test:playground     # vitest
```

`dist/` is a static site: serve it with any static server (`python3 -m http.server -d playground/dist`)
and open it. Every view is a link: the page, the function, the code pane, the benchmark run, the run
it is compared with, the setup filter and the open row live in the URL fragment (`#fn=955&pane=fold`,
`#view=benchmark&setup=zlib&row=zlib%2F990`), as do the Progress page's drill-down, metric and order
(`#view=progress&component=api&by=bytes&sort=size`).

## Progress

The official meter, the matches of `splat/matched.txt` that the splat build checks, as the README and
`docs/status.md` give it: the same numbers, summed from the same `splat/scan.tsv` by the same rule as
`tools/round/status.py` (the tests compare every origin and component row).

| part | what it is |
| --- | --- |
| Headline | the share of TSE's own code matched, the project's goal, with its code bytes and a bar of its functions by verdict (matched, differs, unresolved, no candidate); beside it the whole module's functions and code bytes, the upstream libraries' functions, and the commit the data was made at |
| Waffles | one card per origin (TSE code, upstream libraries, not attributed) with a 10 × 10 waffle split by verdict; click one for a card per component, click a component for that component alone. Functions or code bytes |
| Every function | one square per target function, coloured by verdict, grouped by origin, then by component; hover names it, click selects it. Ordered by verdict, index or size |
| Function | the selected function's name (our symbol demangled, else the ledger's), verdict, component, size, the scan's first difference for one that differs, and a button that opens it in the playground |
| Matches over time | the share matched at every commit of `main` that changed `splat/matched.txt`, for the module and each origin, an origin and its components, or one component, following the drill-down; and the list of those commits with the matches each added or removed |

The history sums every point with today's scan (its components and sizes), so the shares are comparable over
time. The points that the history of `main` does not hold are in `scripts/matches-earlier.json` (the matches as
deltas, without commits); the commits are read from git, so a shallow clone draws fewer.

## Playground

The page opens with what it is for (the intro at the top of the Playground and Benchmark pages is in `src/App.tsx`).

Pick a function by index (136 to 14478; 0 to 135 are imports) or by part of its name, from the 190
benchmark rows grouped by compile setup, or step to the previous or next index. For that function:

| pane | what it is |
| --- | --- |
| WAT | the body as linear WAT, one instruction per line, blocks indented; the gutter is the offset from the body start; a direct call names its callee, an indirect call its type |
| Folded readout | `foldFunction`: the operand stack replayed into expressions, callees by name (a reading aid, not a decompiler) |
| Lifted C++ | `liftFunction` with the names `pnpm wasm lift` uses (the glue's import and export names, the ledger's names and their mangled spellings); editable for reading, Copy, Download `<index>.cpp`, Reset |
| Facts | index, name, symbol, exports, size, instruction count, body offset, signature, locals, the ledger's component (and its evidence), owner and best pair, the flags the lift was made for |
| Benchmark row | for one of the 190 rows: the result in the chosen run (outcome, raw and -Oz score, failure, the lifter's stand-ins), its setup, build flags, features, the object that defines it and its reference source |
| Compile and score it locally | the commands below, filled in for the function |
| Callers, Callees, Strings | direct callers (computed over every body after the first paint), direct callees with their call sites, and the ledger strings an `i32.const` points into |

A benchmark row is lifted with its setup's flags as the benchmark compiles them (`liftCompileFlags`);
any other function with its ledger component's flags (`componentFlags`), as `pnpm wasm lift` does.
Both produce the same source as the CLI: the tests compare the page's lift with `liftInputs` +
`liftFunction` on every row.

The commands, with `/tmp/lift/<index>.cpp` as the file (Download saves the editor's text under that name):

```sh
# in asmlift-wasm/
pnpm wasm fn ../target/vota_web_wasm.wasm <index>
pnpm wasm lift ../target/vota_web_wasm.wasm <index> -o /tmp/lift --check
pnpm wasm rank ../target/vota_web_wasm.wasm <index> --extract export:<symbol> --flags '<flags>' /tmp/lift/<index>.cpp
pnpm bench run --label my-change --setup <setup> && pnpm bench diff baseline my-change   # a benchmark row
pnpm bench add <index>                                                                   # any other function
# in the urna checkout, for an application function: the build's flags for the directory it lands in
tools/round/try-tu.sh --single <index> --extract export:<symbol> --component <dir under src/> /tmp/lift/<index>.cpp
```

## Benchmark

Every results file under `benchmark/results/` (collected at build time; `baseline.json` is
committed), newest first, with a run picker and, when there is more than one, a run to compare with.
For the chosen run: rows, compiled, exact at -Oz, exact raw, median and mean -Oz ratio (with the
change against the compared run); a chart of the rows per setup by outcome (exact at -Oz, differs,
failed) and a table per setup (rows, compiled, exact raw, exact -Oz, failed, median raw, median -Oz,
mean -Oz); what moved between the two runs, by `pnpm bench diff`'s rule (outcome or -Oz score
changed); every row (filtered by a click on a setup), and a row's details with a button that opens
the function in the playground.

## UrnaEmu

A short page about UrnaEmu (<https://eleicoes.rlaneth.com/urnaemu/>, by Rodrigo Laneth), the emulator that runs
VOTA, the same `vota_web_wasm.wasm`, in the browser, with links to it, to its author's Twitter profile and to its
GitHub repository. The links open in a new tab; the page fetches nothing.

## Data

`scripts/prepare-data.ts` (run by `dev` and `build`, or alone as `pnpm --filter @asmlift/wasm-playground data`)
writes `public/data/`, which is gitignored:

| file | from |
| --- | --- |
| `vota_web_wasm.wasm`, `vota_web_wasm.js` | copied from `target/` when they changed |
| `progress.json` (660 KB, 120 KB gzipped) | distilled by `scripts/progress.ts` from `splat/scan.tsv` (verdict, component, size and first difference of every function; its symbols demangled by `c++filt` when it is on the path), `ledger/functions.tsv` (the names of functions with no symbol of ours), `scripts/matches-earlier.json` and `git log --first-parent` over `splat/matched.txt` |
| `ledger.json` (1.3 MB, 268 KB gzipped) | distilled by `scripts/data.ts` from `ledger/functions.tsv` (owner, best pair and the names `ledgerNames` keeps), `ledger/func-attribution.tsv` (component and evidence), `ledger/strings.tsv` (only the strings some function points into, which leaves every function's references unchanged) and `build/out/wasm/vota_web/symbols.{mangled,demangled}.txt` (the mangled spelling of each ledger name, written by `make -C src link`; absent before the first link, as for the CLI) |

The benchmark's dataset and setups come from `@asmlift/wasm-benchmark/dataset/*.json`, its types from
`@asmlift/wasm-benchmark/schema`, the lift flags from `@asmlift/wasm-benchmark/flags`.

The bundle imports no `node:*` module: `imports.ts` (glue parsing) and `strings.ts` (string
references) read no file; the file-reading halves are `glue-file.ts` and `readout.ts`.

## Tests

`pnpm test:playground` runs on the real target, glue, ledger and results, with no mocks: the distilled
names equal `ledgerNames`, every function's component, owner and best pair equal the ledger files',
and every function references the same strings with the distilled table as with the full one; the
page's names equal `liftInputs`' and its lift equals the CLI's for all 190 rows; the WAT has one line
per instruction and names every callee; the folded readout calls callees by name; the direct callers
equal `ledger/callgraph.tsv` for every function; search by index and by name; the per-setup
summaries and the dataset groups cover every row; the run diff counts better and worse rows as
`renderDiff` does; the commands; the Progress page's sums equal every origin and component row of
`docs/status.md`, its matches are exactly `splat/matched.txt`'s, every matched function has a name, and its
history ends at the matches of `HEAD`.
