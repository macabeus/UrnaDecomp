# ledger/ — what the project knows about the target, as data

Every file here is generated: by an extraction script that reads the target once, or by the census, which
rewrites its files on every run. Run the commands from the repository root (the `pnpm wasm` ones from
`asmlift-wasm/`, as shown). `build/dump/vota.objdump.txt` (gitignored) is the output of
`wasm-objdump -d target/vota_web_wasm.wasm`.

## Read once from the target

These describe the target only, so they change only when an extractor changes. Produce them in the order below
(later scripts read earlier outputs). `docs/reference/appmap.md` summarises them.

| file | holds | producer |
| --- | --- | --- |
| `strings.tsv` | every NUL-terminated string of the data segments: address, length, encoding, text | `python3 -I tools/extract/extract_strings.py target/vota_web_wasm.wasm ledger/strings.tsv` |
| `source-files.txt` | the 759 source paths the target spells (`__FILE__` strings and `std::source_location` file names) | `awk -F'\t' 'NR>1{print $4}' ledger/strings.tsv \| grep -E '/.*\.(c\|cc\|cpp\|cxx\|h\|hpp)$' \| LC_ALL=C sort -u > ledger/source-files.txt` |
| `functions-pretty.txt` | 1,394 function-signature texts (`__PRETTY_FUNCTION__` and `source_location` function names, the uenux2 `Assert (…)` texts) | picked by hand from the 1,698 strings of `strings.tsv` that hold a `(…)`, dropping SQL, format strings and messages; no script |
| `classes.json`, `vtables.json` | every typeinfo object (name, kind, bases) and every vtable (address, class, slots as function indices) | `python3 -I tools/extract/rtti.py target/vota_web_wasm.wasm ledger/classes.json ledger/vtables.json` |
| `func-strings.tsv`, `callgraph.tsv` | per function, the strings, vptrs and typeinfos it loads; every direct `call` edge | `python3 -I tools/extract/func_refs.py build/dump/vota.objdump.txt ledger/strings.tsv ledger/vtables.json ledger/classes.json ledger/func-strings.tsv ledger/callgraph.tsv` |
| `source-locations.tsv` | the 1,939 `std::source_location` records (file, function, line, column) and the functions that load each | `python3 -I tools/extract/source_locations.py build/dump/vota.objdump.txt > ledger/source-locations.tsv` |
| `func-attribution.tsv`, `functions-by-file.tsv` | the component of every defined function with its evidence; the second file pairs a `__FILE__` string with a pretty-function string loaded by the same function, and is empty (header only): the target's file and function names reach the code through `std::source_location` records, not as separate strings | `python3 -I tools/extract/attribute.py ledger/func-strings.tsv ledger/vtables.json ledger/classes.json target/vota_web_wasm.wasm ledger/func-attribution.tsv ledger/functions-by-file.tsv` |
| `ui-strings.tsv` | the user-facing pt-BR strings and the components that load them | `python3 -I tools/extract/ui_strings.py ledger/strings.tsv ledger/func-strings.tsv ledger/func-attribution.tsv ledger/ui-strings.tsv` |
| `import-names.json` | the real name of each minified import field, from the glue's `wasmImports` table | `cd asmlift-wasm && pnpm wasm import-names ../target/vota_web_wasm.js -o ../ledger/import-names.json` |

## Written by the link-order solver

Rewritten only when the solver runs again; `docs/reference/link-order.md` has the
commands and the results.

| file | holds | producer |
| --- | --- | --- |
| `ordinals.tsv` | each target function's recovered link ordinal: point estimate, ordinal and rank windows, anchoring object | `python3 -I tools/linkorder/linkorder.py solve …` (link-order.md §6) |
| `tu-clusters.tsv` | the order cut into probable translation units, one row per use-count band stretch | `python3 -I tools/linkorder/tu_slices.py …`, which also writes `docs/reference/tu-slices.md` |

## Rewritten by every census

`make -C src census` (`tools/round/census.sh`) compares the current link with the target and rewrites these.

| file | holds |
| --- | --- |
| `functions.tsv` | one row per target function: size, use count, owner, component, and its best match so far (`best_kind` = byte, fingerprint, addr, shape or none, against the skeleton link or the libc probe) |
| `history/<stamp>-census.tsv` | a copy of `functions.tsv` taken at the end of a census |

The census prints its tiers with `pnpm wasm progress`, which compares `functions.tsv` against the newest file
in `history/` (or the file `--against` names) and then saves a new snapshot. Only the newest snapshot is kept
in the working tree; it is the state of the last committed census, so `progress` prints what moved since that
census. Earlier snapshots are in git (`git log -- ledger/history`). After a census, commit the new
snapshot and `git rm` the previous one by name.

`functions.tsv` and its tiers are diagnostics: they say how close each function is. The project's official
count of matched functions is the splat build's claim list, `splat/matched.txt` (`docs/reference/splat.md`).

## Not tracked

`bundles/` (Markdown readouts written by `pnpm wasm bundle`) and `lift-eval/` (the output of the lifter
evaluation) are generated on demand and gitignored.
