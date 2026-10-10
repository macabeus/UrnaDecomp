# Facts: function order and link order

Why the target's functions sit in the order they do, what that says about the link line, and how much of the
link order can be recovered. `docs/reference/link-order.md` describes the solver and its outputs; this file holds the
measurements behind them.

## L1. The order law: use count descending, ties by the descending decimal string of the link ordinal

**On the target** (`tools/extract/wasmmod.py` + `ledger/callgraph.tsv`): with uses(f) = direct `call f` sites + funcref
table entries + exports, the sequence uses(136), uses(137), … uses(14478) over the 14,343 defined functions is
non-increasing with 0 violations. Uses at 136…146 are 15591, 6511, 2401, 2373, 2368, 2279, 2170, 1876, 1202, 1149,
931; the uses==2 band is 4303…6974 and the uses==1 band 6975…14478 (7,504 functions). The last function, 14478,
is `__wasm_call_ctors`.

**Reproduced in `emscripten/emsdk:6.0.0`** with three TUs `a.c`/`b.c`/`c.c` (functions `zz1..5`, `mm1..5`,
`aa1..5`, two uses each) and a `main.c`:

- `emcc -O1`/`-O2`: functions stay in link order; not sorted by uses. So the target is not -O2.
- `emcc -O3 -g`: no reordering at all (binaryen limits itself because of DWARF).
- `emcc -Oz` (also -O3, -Os) **without** `--emit-symbol-map`: sorted by uses descending, ties by the
  **descending decimal string** of each function's ordinal. With link order a, b, c (zz = 1…5, mm = 6…10,
  aa = 11…15) the output is `mm4 mm3 mm2 mm1 zz5 zz4 zz3 zz2 aa5 aa4 aa3 aa2 aa1 mm5 zz1` = "9", "8", …, "2",
  "15", …, "10", "1". Reversing the link order permutes exactly as predicted. The names are gone before the
  reorder: without a map, `-g` or `--profiling-funcs`, emcc removes the name section with `llvm-objcopy` before
  any Binaryen tool runs (`strip_sections` in the image's `tools/emscripten.py`).
- `emcc -Oz --emit-symbol-map`: ties sort by **name** descending (`zz5 … zz1 mm5 … mm1 aa5 … aa1`), and
  `cmp` of the two wasm files differs. **`--emit-symbol-map` changes an -Oz wasm**: every function index, every
  `call` immediate and the elem segment move. Use a map build only to name bodies, never for byte or index
  comparison with the target. A map for the plain order can be computed afterwards by sorting the mapped build's
  functions by (uses desc, str(ordinal) desc). (At -O2 the map changes nothing, because nothing is reordered.)

The target's `__wasm_call_ctors` (ordinal "0", the smallest string) sitting last in the uses==1 band is the
signature of the plain `em++ -Oz` case; nothing beyond `-Oz` is needed on the link line.

Consequences:

1. Within one use-count band, position order is the reverse link order, scrambled digit-wise by the string
   comparison. One object's functions are not contiguous across a decimal-digit boundary (e.g.
   `crypto/x509/x_pubkey.c` functions at 13800 and 14335): a split is not two link-line occurrences.
2. Function index diffs between a rebuild and the target mean nothing until the link line matches; compare by
   (uses, band position) or by content.

## L2. The mechanism: the ordinal is the post-Oz index, and wasm-metadce sorts

Replaying the stages of the skeleton link (`em++ -v` with `EMCC_DEBUG=1`, intermediate `emcc-NN-*.wasm`; scripts
`tools/linkorder/{truth,segment,stripnames,addnames}.py`):

1. **wasm-ld** (`--strip-debug`) output still has a name section (15,966 names): `--strip-debug` drops DWARF only.
   Defined functions in input order: `__wasm_call_ctors` = 0, the `signature_mismatch:*` stubs, each object's live
   functions in object order, then archive members in pull order (libc `htons.o`/`htonl.o` last). emcc then removes
   the name section with `llvm-objcopy` (emscripten's `tools/emscripten.py`), because nothing asked to keep names.
2. **wasm-opt -Oz** reads the module without names, since emcc removed them; it would read them if present, and
   keeping them (`-g`, `--profiling-funcs`) changes the `-Oz` code (`docs/reference/include-asm.md`): 15,824 → 9,998
   survivors **in link order**, plus 216
   `byn$mgfn-shared$*` thunks appended. It does not sort. (A `--symbolmap` on a name-stripped copy gives names
   "0", "1", … strictly increasing with index.)
3. **wasm-metadce** re-reads the nameless module, names function k "k" (its defined index), removes the dead
   exports' functions (2: `ntohs`, `htonl`) and runs `reorder-functions`: uses descending (call sites + table
   entries + exports), ties by `strcmp` of the decimal strings descending. Replaying metadce on a copy with a
   synthetic name section and `-g` reproduces the code byte for byte and names every final function with its
   ordinal.
4. The last **wasm-opt** (minify, optimize-stack-ir) keeps the order.

Check: sorting the final skeleton by (uses desc, str(ordinal) desc) puts 0 of 10,212 functions out of place.

Consequences:

- Ordinals are a permutation of 0…N+h−1, with h = metadce's removals (2); inlining leaves no holes.
- An object's surviving functions are contiguous in object order; -Oz only deletes inside a block.
- A COMDAT (inline/template) function belongs to the **first** object on the link line that defines it. In the
  skeleton, the stand-ins `roots_boost.o` and `roots_rhvoice.o` come before the RHVoice objects and own the template
  bodies they instantiate, hundreds each (`grep -c roots_boost.o build/out/wasm-map/vota_web/names.tsv` counts one;
  the third column is the owning object): those bodies are not in the library objects' runs.
- The mgfn thunks hold the highest ordinals.
- wasm-ld's name section is demangled (lld's default `--demangle`); object files carry mangled names. Compare
  through the image's `llvm-cxxfilt`; 353 demangled names are duplicated across objects.

## L3. What the target's bands say about its link line

Reading the uses==1 band backwards (14478 → 6975) walks the link line roughly from first object to last.
Component runs seen there (`tools/extract/attribute.py`, exact-start string references):

| band positions | component |
| --- | --- |
| 6975–7725 | OpenSSL libcrypto (evp, ec, dsa, dh, conf, engine, provider_core) |
| 7725–8105 | libc++ locale (`time_get`, `num_put`, `money_put`) |
| 8105 | `api::teste::CPowerMock` / `CUrnaMock` |
| 8135–8742 | 7-Zip (NLzma/NLzma2, N7z, CFilterCoder) |
| 8742–9013 | ASN.1 runtime |
| 9013–9290 | `ecourna::app::dados::asn` converters, `Modulo*` types, simulador |
| 9330–9380 | boost exception/regex |
| 9412–9500 | `ecourna::api::sql::sqlite`, security, cepesc |
| 9519–9570 | boost::signals2 |
| 9592–10125 | RHVoice |
| 10125–12375 | the uenux2 app (comum, vota, api; vota-web at 12075); libc++abi `itanium_demangle` 12075–12375 |
| 12375–12975 | mostly unattributed (sqlite/musl by string refs) |
| 12975–14478 | libcrypto providers, then x509, ui, rsa, rand, property, pem, objects, then `__wasm_call_ctors` |

## L4. Ordinal recovery from the bands: what anchors can and cannot give

Band order alone fixes nothing absolute: every interleaving of the bands is a solution. Anchors are library
objects whose target functions are identified and whose emission order is known; an object's members are
contiguous (L2), so:

- the band-adjacency test (k functions between two members need k slots: a 3-digit c sits after the 4-digit 10c)
  pins a block's base modulo 10 (SQLite in the skeleton: 764 feasible bases, all ≡ 8 mod 10);
- the cut equations (functions of every band certainly above a rank must fit above it) give absolute intervals;
- floaters (254 target functions in bands of ≤ 5) move between gaps in groups, so blocks keep intervals, not
  points.

Skeleton validation with complete anchors (8,601 in 860 objects): the truth is inside every window; point
estimate exact for 680, median |error| 10. On the target (4,866 anchors in 694 objects) only 536 anchored
functions got windows ≤ 10, and the app code was not placed. The confident reading: libcrypto members run from
`evp_fetch` ≈ 9212 to `provider_core` ≈ 9970 and on to ≈ 10198, so **libcrypto.a is pulled at the end of the link
line and straddles 9999/10000** (the one absolute anchor the target offers); SQLite chunks sit in the 5-digit
zone (12xxx), which agrees with the band walk (L3). The bottleneck is anchor completeness: 1–4-member chunks carry
no absolute information.

## L5. The law holds on a second skeleton; 5,611 target↔skeleton pairs

Replay of a second, larger skeleton link (16,033 wasm-ld functions, 146 imports): -Oz keeps 9,942 in link order,
appends 189 mgfn thunks, metadce removes 2 (`ntohs`/`htonl` again) → 9,940. Sorting by (uses desc, str(post-Oz index) desc)
puts 0 of 9,940 out of place.

**The position-gap model is exact.** For every pair of an object's live functions adjacent in the object's
**full** function list (`wasm-objdump -x -j Function` on the `.o`), 1 ≤ Δordinal ≤ Δposition: 8,737 pairs, 0
violations. Dead functions between them occupy no ordinal; nothing else can sit between them.

**Identification** (`tools/linkorder/ident.py`): sources are the ledger's byte/fingerprint/addr pairs, census
depth-0 pairs, exact bodies by unique name, the SQLite attribution, OpenSSL own-name strings, then callee-structure
propagation (paired functions with equal call counts pair their callees positionally). Gates: the normalised body
is unique on both sides (or the name is unique), ≥ 6 instructions, no function paired twice. Result: 5,611 pairs (exact
4,448, byte 654, name 234, addr 144, fingerprint 25, propagated 106). Coverage of skeleton-live functions: sqlite
94 %, libc 144/163, OpenSSL members 81 %, libc++ 227/423, 7-Zip 24 %, RHVoice 15 % — the C++ libraries' bodies differ
by whole-program inlining, so their anchors stay sparse. Anchors defined in more than one object (127 COMDAT
templates such as `operator new`) and anchors in the skeleton's roots/stub objects are dropped.

## L6. Second recovery: sound windows, the recovered link line, TU slices

Anchors are (object, position in the object's full function list) for 5,057 target functions; chunks of
adjacent positions are rigid and chains between chunks are exact bounds. Three solve rounds quarantine chunks
with impossible gaps (36 → 33 → 20 chunks; 4,730 anchors left, 0 inconsistent links):

```
python3 -I tools/linkorder/linkorder.py solve target/vota_web_wasm.wasm --anchors <anchors.tsv> --positions \
    --top <mgfn-target.txt> --top-max 600 --holes 2 --repair 2 -o ledger/ordinals.tsv --blocks <blocks.tsv>
```

(200–250 s per round.) Rank windows over all 14,343 functions: ≤ 10 for 1,117, ≤ 50 for 2,001, ≤ 100 for 2,441.
The app (3,270 functions) is barely placed by the solver (14 / 43 / 77); the band walk places it.

**Validation.** With perfect skeleton anchors every window contains the truth. With 1.7 % random wrong anchors
injected the windows become unsound and look tighter. On the target, holding out 465 anchors: 423 consistent, 13
inconsistent; ≈ 7 % of tight target windows are wrong. **Read a tight window as likely, not proven**;
identification quality is the lever, not the solver. Hole counts 1–5 give the same inconsistencies, so the metadce
removal count is not decidable from this data; 2 is kept.

**Recovered link line** (74 objects with ≤ 20 feasible bases at both ends): libcrypto members in several runs from
~100 to ~4,720 and the 9999/10000 straddle run 10000–10125; libc++abi 1896–1929; `sqlite3.o` 12345 → 13942 (5-digit
zone confirmed); a 7-Zip object at 14058–14068; mgfn bodies above everything. By elimination, the app code sits at
ordinals ≈ 300–9200.

**TU slices** (`tools/linkorder/tu_slices.py` → `ledger/tu-clusters.tsv`, `docs/reference/tu-slices.md`): in each use band the
unanchored functions between library anchors are sliced into runs of one probable file, labelled by
`source_location` records, else by the vtable class's file. 1,385 stretches over 7,339 functions; 1,732 of 3,270
app functions labelled. The uses==1 band's app region is clean reverse link order (e.g. band 11047–11066 = 20
functions of `app/comum/relatorios/crelatoriotesteimpressora.cpp`). A stretch is a TU candidate for
`tools/round/try-tu.sh`.

## L7. Traps when inverting the order

1. `wasm-opt … --symbolmap=f` makes wasm-opt read the name section, so the output differs from the plain build.
   To see the names binaryen uses in the plain pipeline, strip the name section first
   (`tools/linkorder/stripnames.py`), then add `--symbolmap`: the map reads "0".."N" plus `byn$` names.
2. The order is decided in wasm-metadce, not in `wasm-opt -Oz`: a module taken after the first wasm-opt is still
   in link order. Compare orders only on the final module.
3. The shared skeleton output (`build/out/wasm/vota_web/vota_web_wasm.wasm`) is relinked by every build: tie each
   truth table or anchor file to a frozen copy of one module.
4. Body-identity anchors to non-unique bodies attribute a function to the wrong object: 2,198 of the skeleton's
   10,212 normalised bodies are shared (7-Zip/libc++ vtable thunks, ASN.1 accessors), and such anchors produce
   order cycles. Use only unique bodies (`tools/linkorder/wasmcode.py`'s `norm_body`) or names.
5. Object function lists are mangled and the linked name section is demangled: match through `llvm-cxxfilt` from
   the image.
6. A full skeleton link is seconds, not minutes, on a warm cache (7.4 s wall for the 564-object link including
   wasm-opt); the minutes go to recompiling objects.
7. Skeleton-live offsets inside one object do not transfer to the target (different roots, different live sets):
   chunk at every gap instead of anchoring a rigid block.
8. Band adjacency orders two chunks only inside one digit zone: "999" precedes "1000" in a band yet is the smaller
   ordinal; every 3-digit c ≥ 200 precedes every 1xxx; every 4-digit ≥ 2000 precedes every 5-digit. A block-order
   graph that ignores zones is unsound (perfect anchors fell outside their windows). `--positions` mode skips those
   checks and allows two descending runs per band.
9. Fragmentation, not wrong anchors, costs precision: perfect anchors as rigid objects give median |error| 10, as
   position chunks 42.
10. The skeleton links 345 libcrypto members explicitly **and** `libcrypto.a`; a "defined in two files" test must
    compare object basenames, or it flags 2,695 ordinary C functions.
11. A function with a Portuguese app string sitting between two SQLite functions in the uses==1 band is not a
    misattribution: it is a 4-digit twin (ordinal ≈ 1290) sprinkled into the 5-digit SQLite run.
12. Python's `print` buffers when stdout is a file: pass `flush=True` in long solver runs, or they look dead.
