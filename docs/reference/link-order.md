# Link order: inverting the function-order scramble

Tools: `tools/linkorder/wasmcode.py` (code decoder, use counts), `tools/linkorder/linkorder.py` (solver),
`tools/linkorder/tu_slices.py` and `tools/linkorder/tu_clusters.py` (runs of the recovered order),
`tools/linkorder/*.py` (skeleton ground truth). Outputs: `ledger/ordinals.tsv`, `ledger/tu-clusters.tsv`.

The solver has two modes. The *offsets* mode (§3 to §5) anchors chunks by their offsets in the skeleton's live
function list. The *positions* mode (§7) anchors them by their positions in each object's full function list and
gives windows that provably hold the ordinal when the anchors are right. `ledger/ordinals.tsv` and
`ledger/tu-clusters.tsv` hold the positions solve of the target (§7.4, §7.5). Each mode is validated on a frozen
skeleton whose exact order a stage replay gives (§1, §7.1).

## 1. The law, with its mechanism (verified on the skeleton, 0 exceptions)

`em++ -Oz` runs four tools (from `em++ -v` with `EMCC_DEBUG=1`, `build/out/linkorder/run_v.log`; the counts are
those of the frozen skeleton in `build/out/linkorder/`):

1. `wasm-ld … --strip-debug` — its output **does have a name section** (15,966 names in
   `emcc-01-base.wasm`): `--strip-debug` only drops DWARF. 15,824 defined functions, numbered in input order:
   `__wasm_call_ctors` = 0, the `signature_mismatch:*` stubs next, then every input object's live functions in
   the object's own order, then the archive members in the order wasm-ld pulled them (`libc++abi`, `libc`
   `htons.o`/`htonl.o` last). A COMDAT (inline/template) function belongs to the FIRST object on the link line
   that defines it (`roots_boost.o` owns 1,831 functions, `roots_rhvoice.o` 754).
   Before Binaryen runs, emcc removes that name section with `llvm-objcopy` (emscripten's `tools/emscripten.py`), unless
   `-g`, `--profiling-funcs` or `--emit-symbol-map` asks for names.
2. `wasm-opt --strip-target-features --post-emscripten -Oz …` — reads the module without names, because emcc
   removes them; Binaryen would read them if they were there, and keeping them changes the `-Oz` code
   ([include-asm.md](include-asm.md)). It inlines/merges 5,826 functions (15,824 → 9,998 survivors, **link order
   kept**), appends 216 `byn$mgfn-shared$*` thunks, writes no names. Nothing is sorted here.
3. `wasm-metadce --graph-file=…` — re-reads that module, names function k "k" by its defined index, removes
   the dead exports' functions (2 here: `ntohs`, `htonl`, post-Oz indices 9344 and 9997) and runs
   `reorder-functions`: use count (direct `call` sites + funcref table entries + exports) descending, ties by
   `strcmp(name)` DESCENDING on the decimal strings.
4. `wasm-opt --minify-imports-and-exports-and-modules --optimize-stack-ir` — order preserved.

So the "link ordinal" of the law is the **post-Oz defined index** (link order compacted after -Oz, mgfn
thunks at the end), a permutation of 0..N+h-1 where h = metadce's removals (2 in the skeleton). Proof:
replaying stage 3 on a copy of `emcc-05-wasm-opt.wasm` with a synthetic name section ("k" per function) and
`-g` reproduces the code byte for byte and names every final function with its ordinal; sorting the final
module by (uses desc, str(ordinal) desc) puts 0 of 10,212 functions out of place (`tools/linkorder/truth.py`).
Exports count as uses: leaving them out gives one apparent violation, index 3118, which is export `Mb`.

Within a band, same-digit-count ordinals descend numerically; a 3-digit c sits right after the 4-digit 10c
("425" after "4250"), a 4-digit a sits inside the 5-digit zone right after 10a. The band therefore reads,
zone by zone, as REVERSE link order with the lower zone sprinkled in at every decade.

## 2. What is and is not recoverable

Band order alone gives no absolute ordinal: any merge of the bands that respects each band's order is a
solution. Anchors (library objects whose target functions are identified and whose emission order is known)
pin things: an object's live functions are contiguous in object order, so its members fix each other's
relative ordinals. The adjacency test (members consecutive in a band with k functions between need k marker
slots) pins a block's base modulo 10 (and 100/1000 when the hundred/thousand markers show up). Absolute
positions follow from the cut equations (the functions of every band certainly above a rank must fit above
it) — up to the "floaters": functions in bands too small to be ordered against anything can move between gaps
in groups, so every anchored block keeps an interval, not a point.

## 3. Solver (`tools/linkorder/linkorder.py solve`), offsets mode

Per chunk (rigid run of identified members), the set of feasible bases is shrunk to a fixpoint by: the
adjacency test; order-count bounds from the band-adjacency DAG of blocks; chunk chains (loose links where
members exist but are not identified); the cut equations. Output per function: ordinal hull of its rank window
(sound when the anchors are right), a point estimate (window middle), block and offset.

Validation on the frozen skeleton (`build/out/linkorder/plainv/vota_web_wasm.wasm`, 10,212 functions, truth
from §1; `build/out/linkorder/anchors-perfect.tsv` = every non-app object, 8,601 anchors in 860 blocks):

| anchors | truth inside window | point exact | median \|err\| | within 10 | within 50 |
|---|---|---|---|---|---|
| complete identification (8,601) | 8,601/8,601 | 680 (7.9%) | 10 | 4,527 (53%) | 6,408 (75%) |
| 85% random subset, live-list offsets (7,340) | 7,340/7,340 | 65 | 25 | 2,048 | 4,386 |

Blocks with 1–2 members and no band neighbour of their own get no absolute information (their point
estimate is arbitrary; their window says so).

## 4. Offsets solve of the target

Anchors: ledger byte/fingerprint/addr matches (size > 8 B) to the skeleton's named functions, restricted to
skeleton bodies that are UNIQUE after normalisation (1,038 of 5,904 dropped: 7-Zip/libc++ vtable thunks share
bodies across objects and would be attributed to the wrong object), plus the SQLite attribution's names → 4,866
anchors in 694 objects, split into 1,265 chunks because the target's live sets differ from the skeleton's.

Result: 4,860 anchored point estimates; rank windows ≤ 10 for 536 anchored functions, the rest wide (median
window of all functions ≈ the whole range: the unanchored app code is NOT placed). Confident objects (≥ 5 members,
≤ 10 feasible bases): 55, listed below; the grouping by `tools/linkorder/tu_clusters.py … 100` holds only the 580
functions with window ≤ 100. The ledger holds the positions solve instead (§7.4).

Confident placements (ordinal range, members): libcrypto members `dso_lib` 86–90?, `cipher_aes_xts/wrp`
124–151 (these four low-zone placements are suspect — tiny chunks), `ecp_smpl` 924–938, `pmeth_lib`,
`ec_pmeth`, `decoder_meth` 948–978, then the dense libcrypto run `evp_fetch` 9212 … `provider_core` 9970,
`ccm128/gcm128/ocb128/wrap128/o_names/obj_dat/decoder_pkey/property*/rsa_backend` 10003–10198, `sha512` 10349,
`cipher_aria` 10732, `seed_src` 12024; sqlite3.o chunks split between ~1475–1700 and ~12100–12600 (the 1xxx
vs 5-digit ambiguity of the 1-region: both read identically in band order; unresolved).

Reading: libcrypto occupies the top of the 4-digit zone and continues into the 5-digit zone (9200 → 10700+),
i.e. libcrypto.a is pulled at the END of the link line and straddles 9999/10000 — that straddle is the one
absolute anchor the target offers (its members' offsets fix 10000 exactly once the member set is complete).
SQLite most likely sits in the 5-digit zone (12xxx) after the app, consistent with the band walk ([facts §L3](facts/link-order.md)).

## 5. Limits of the offsets mode (§7 answers each)

1. The target anchors are the bottleneck: chunks of 1–4 members carry no absolute information. Complete
   per-library identification (names via strings, the whole live set from a probe with the app's roots) turns
   1,265 chunks back into ~60 rigid blocks, which is where the skeleton numbers (median error 10) come from. §7.2
   identifies more functions, and anchors them by full-list position, which needs no live set.
2. The 1-region ambiguity (4-digit 1000–1435 vs 5-digit ordinals interleave 1:10 in band order) needs a
   zone-consistency rule per object (all chunks in one zone unless the object straddles 9999/10000); §7.3 has the
   zone-aware check.
3. The mgfn thunks (`byn$mgfn-shared$*`, highest ordinals, 216 in the skeleton) are recognisable by shape
   and anchor the top of the 5-digit zone (`--top`, §7.3).
4. Floaters (254 target functions in bands of ≤ 5) bound the attainable precision to intervals of ~10–50.

## 6. Commands

Run from the repository root. The solve and its publication are scripted end to end; the anchors and the skeleton's
ground truth start from intermediate files that one-off commands produce, which are described here but are not in
the repository.
Every path under `build/out/` is gitignored scratch.

The positions solve (§7) and what it publishes:

```
python3 -I tools/linkorder/linkorder.py solve target/vota_web_wasm.wasm --anchors build/out/LINKORDER2/anchors-target-f2.tsv \
        --positions --top build/out/LINKORDER2/mgfn-target.txt --top-max 600 --holes 2 --repair 2 \
        -o build/out/LINKORDER2/ordinals.tsv --blocks build/out/LINKORDER2/blocks.tsv
tools/linkorder/publish.sh build/out/LINKORDER2/ordinals.tsv build/out/LINKORDER2/blocks.tsv
        # ledger/ordinals.tsv, ledger/tu-clusters.tsv and docs/reference/tu-slices.md (tu_slices.py),
        # build/out/LINKORDER2/linkline.md (linkline.py), and the window statistics of §7.4
```

The anchors (§7.2):

```
python3 -I tools/linkorder/ident.py         # build/out/LINKORDER2/{pairs-target-skeleton,anchors-target,mgfn-target}.tsv
python3 -I tools/linkorder/repair.py <anchors.tsv> <badlinks.tsv> <out.tsv>   # drop the smaller chunk of every inconsistent link
python3 -I tools/linkorder/holdout_eval.py  # the hold-out check of §7.4
```

`ident.py` reads the frozen skeleton (`build/out/LINKORDER2/frozen/plain.wasm` and `functions.tsv`, copies of one
plain link and its `ledger/functions.tsv`), its ground truth (`skeleton-truth.tsv`, below), the census outputs
`build/out/census/census-d0.json` and `wasmfn-cmp.txt`, and `tools/libs/sqlite/attribution.tsv`; run against another
census it pairs a slightly different set of functions. Three steps are one-off commands that are not in the
repository: the §7.2 filters (no anchors in the roots and stub objects, none in COMDAT functions, none in libc++
instantiations) that turn `anchors-target.tsv` into `anchors-target-f2.tsv`; `mgfn-target.txt`, the first column of
`mgfn-target.tsv`; and the random 10 % split into `anchors-target-holdin.tsv` / `anchors-target-heldout.tsv` that
`holdout_eval.py` reads (with the solve of the held-in set as `ordinals-target-holdout.tsv`).

The skeleton's ground truth (§1, §7.1) needs a stage replay of one link. The link is run with `em++ -v`,
`EMCC_DEBUG=1` and `EMCC_TEMP_DIR=<dir>` added to its own command (`build/out/wasm/vota_web/link.cmd`), which keeps
emcc's intermediates (`emcc-01-base.wasm`, …) and prints each stage's command line. Then `stripnames.py` drops the
name section of the wasm-ld output, the printed `wasm-opt` command is rerun on it with `--symbolmap`
(`step05n.symbols`), `addnames.py` names every function by its index, and the printed `wasm-metadce` command is rerun
with `-g` (`step10-named-g.wasm`). The scripts that run this hard-code one link's object list and emcc's temporary
file names and are not in the repository; neither are the one-off lists `base-names.tsv`, `link-objects.txt`,
`demangle-map.tsv`, `objfn/<k>.tsv` (`wasm-objdump -x -j Function` of each object) and the offsets mode's `arfn/`
(the same per archive member). With those in place:

```
python3 -I tools/linkorder/truth2.py        # build/out/LINKORDER2/skeleton-truth.tsv, skeleton-objects.tsv, and the law check
python3 -I tools/linkorder/truth.py         # the offsets mode's truth, on build/out/linkorder/ (segment.py assigns objects)
```

The offsets solve (§3, §4) and its grouping by `tu_clusters.py`, whose output format is not the one
`ledger/tu-clusters.tsv` holds (`tu_slices.py` writes that file):

```
python3 -I tools/linkorder/linkorder.py solve MODULE.wasm --anchors A.tsv [--offsets] --holes 2 [--truth T.tsv] -o ordinals.tsv --blocks blocks.tsv
python3 -I tools/linkorder/tu_clusters.py ledger/ordinals.tsv ledger/func-attribution.tsv ledger/vtables.json ledger/func-strings.tsv tu-clusters.tsv 100
```

## 7. Positions mode: full-list positions, sound windows, TU slices

The offsets solve of the target (§4) is imprecise for a structural reason: its anchors are 1,265 chunks of 1–4
members whose relative offsets are guesses (the target's live sets differ from the skeleton's), and two of its
ingredients are unsound across digit zones (§7.3). The positions mode replaces the anchors and those ingredients.
Its scratch files are under `build/out/LINKORDER2/`.

### 7.1 Ground truth on a second frozen skeleton

The stage replay of §1 on the frozen skeleton `build/out/LINKORDER2/frozen/` (`tools/linkorder/truth2.py`, §6):
16,033 wasm-ld functions → 9,942 after -Oz (189 mgfn bodies appended at 9753..9941) → metadce removes 2 (ntohs,
htonl) → 9,940; sorting by (uses desc, str(post-Oz index) desc) puts **0 of 9,940 out of place**. The same truth
gives the *position-gap law* — for two live functions of one object at
positions p < q in the object's FULL function list (`wasm-objdump -x -j Function` on the .o), the ordinal difference is
between 1 and q−p (dead functions between them occupy no ordinal, nothing else can sit between them). Checked on
truth: 8,737 adjacent pairs, 0 violations. This is the anchor model of the positions mode: it needs no live set,
only the .o.

### 7.2 Anchors

`tools/linkorder/ident.py` identifies target functions with skeleton functions (the numbers are in
[facts/link-order.md](facts/link-order.md) §L5): ledger byte/fingerprint/addr pairs, census depth-0 pairs, wasmfn EXACT
by unique name, the SQLite attribution (`tools/libs/sqlite/attribution.tsv`), OPENSSL_FUNC own-name strings, and callee-structure propagation (two
paired functions with equal call counts pair their callees positionally). Gates: unique normalised body on both sides
(or a unique name), ≥ 6 instructions, no double claims. Then the truth table turns a skeleton function into (object,
position). Filters: no anchors in the skeleton's roots/stub objects (they own COMDAT bodies the TSE link owns
elsewhere), no function defined in more than one object (COMDAT), no libc++ template instantiation.

### 7.3 Solver options of the positions mode (`tools/linkorder/linkorder.py --positions --top … --repair R`)

* `--positions`: chunks are runs of ADJACENT positions (rigid); a gap of g positions bounds the next chunk's base
  exactly to span .. span+g−1 — sound, no slack, no widening. An inconsistent link is reported, never widened.
* The block-order DAG (`order_count_bounds`) and the descending-offset `clean_anchors` are skipped: band adjacency
  orders chunks only inside one digit zone ("999" precedes "1000"); with perfect anchors they make the windows
  unsound (6002/8815 inside). A zone-aware check replaces the cleaning (two descending runs per band allowed).
* `--top`: merge-similar-functions bodies (identified by pairing with the skeleton's 189) are confined to the top
  ordinals; every other chunk ends below them. The base range is [0, M−L], ordinal M−1 included.
* `windows_from` bounds an unanchored function by the previous anchor's lowest and the next anchor's highest rank.
* `--repair R`: the anchors of the smaller chunk of every inconsistent link are dropped and the solve is repeated
  (a misidentified function cuts its object's chain and lets the chunks beyond it float).
* `band_structs` is vectorised (one argsort per band per iteration, masked prefix extrema for the exclusion):
  3.4 s per iteration.
* `ordinals.tsv` has `rank_lo`/`rank_hi` (the sound window in rank space; `ord_lo`/`ord_hi` is its ordinal hull,
  which is meaningless when the window spans digit zones).

Validation on the frozen skeleton (truth = §7.1; `--positions --top mgfn-skel.txt --holes 2`):

| anchors | truth inside window | point \|err\| median | within 10 / 50 | all-function windows ≤10 / ≤50 / ≤100 |
|---|---|---|---|---|
| every non-app object, full-list positions (8,628; `anchors-skel-perfect.tsv`) | 8,817/8,817 anchored, 1,123 unanchored (see below) | 42 | 1,279 / 4,740 | 1,075 / 2,445 / 2,928 of 9,940 |
| realistic subset: unique body ≥ 6 instr, 90 % kept (5,935; `anchors-skel-degraded-pos.tsv`) | 6,123/6,123 anchored, 3,817/3,817 unanchored | 157 | 277 / 1,917 | 0 / 0 / 153 |
| the same with 104 (1.7 %) random WRONG anchors injected | 799/5,642 after quarantine, 835/5,346 after one repair pass | 283 | 240 / 619 | 1,745 / 4,017 / 5,280 — tight AND wrong |

Reading: the machinery is sound when the anchors are right (every window contains the truth) and loose when the
anchors are a realistic subset (chunks break at every unidentified function and each chunk floats within its chain
bounds). Wrong anchors are the danger: a misidentified function inside an object's chain shifts the chain silently;
one with a tight wrong window moves the cut-equation counts by its whole band position. The chain check catches
the part of them that produces an impossible gap (quarantine + `--repair`); the rest make windows TIGHTER and WRONG.
Second-order ("robust") envelopes (`--robust`) stay sound but lose all precision (no window ≤ 100 on the realistic
subset), so they are off by default. Identification quality, not the solver, is therefore the lever.

### 7.4 Target result

`python3 -I tools/linkorder/linkorder.py solve target/vota_web_wasm.wasm --anchors build/out/LINKORDER2/anchors-target-f2.tsv
--positions --top build/out/LINKORDER2/mgfn-target.txt --top-max 600 --holes 2 --repair 2` (5,057 anchors in; three passes
of 200–250 s; `build/out/LINKORDER2/target-final.log`). Repair passes: 36 → 33 → 20 quarantined chunks (196 → 131 → 51 anchors
of impossible gaps dropped; 4,730 anchors left, 0 inconsistent links at the end). Holes: 1–5 give the same inconsistent
links (22–24 in the same objects), so the metadce-removal count is not what the data decides; 2 (the skeleton's) is kept.

Rank windows over ALL 14,343 functions (`ledger/ordinals.tsv`, columns `rank_lo`/`rank_hi`; the offsets solve of §4
has 536 / … / 580 at ≤10 / ≤50 / ≤100, for anchored functions only):

| set | n | ≤10 | ≤50 | ≤100 | ≤500 | median |
|---|---|---|---|---|---|---|
| all functions | 14,343 | 1,117 | 2,001 | 2,441 | 4,798 | 2,088 |
| anchored (library) | 4,767 | 953 | 1,582 | 1,790 | 2,371 | 544 |
| unanchored | 9,576 | 164 | 419 | 651 | 2,427 | 2,445 |
| app components (ecourna/vota/comum/api/simulador/vota-web/asn1-modules/ASN1) | 3,270 | 14 | 43 | 77 | 706 | 2,577 |
| component sqlite | 500 | 164 | 263 | 285 | 410 | 30 |
| component openssl | 2,381 | 318 | 593 | 690 | 966 | 1,638 |
| component 7zip / RHVoice / boost | 878 / 666 / 125 | 3 / 0 / 0 | 3 / 2 / 0 | 3 / 4 / 0 | 47 / 130 / 15 | 2,494 / 823 / 2,577 |

Hold-out check on the target itself (no truth needed; `tools/linkorder/holdout_eval.py`): 465 of the 4,730 final anchors held out
(10 %), solve repeated without them, then each held-out function's window tested against the windows of its object's nearest
held-in members below and above its .o position: 423 consistent, 13 inconsistent, 29 without a neighbour; of the 90 held-out
functions that got a window ≤ 50, 6 are inconsistent. So about 7 % of the tight windows are wrong (a lower bound: the test
only sees contradictions with the same object's chain).

Caveats that the validation table imposes: these windows are sound only if the 4,730 anchors are right; the realistic
clean subset on the skeleton gives sound but far looser windows (≤100: 153 of 9,940), and wrong anchors make windows
tighter and wrong, so a tight target window is not a proof. Two solves with slightly different anchor sets
(`blocks-target-h2c.tsv` vs `blocks-target-final.tsv`) agree on 57 of 63 tightly placed objects and flip 6 between digit-zone
twins (rsa_lib 2936–2982 vs 292–344: "293" is the 3-digit twin of "2936"); the chunk's feasible set often holds both twins
and the `base_lo`–`base_hi` hull hides it (`build/out/LINKORDER2/linkline.md` prints the feasible count).

Recovered link line (objects whose first and last chunk have ≤ 20 feasible bases, 74 objects; `linkline.md` has the per-object
table): libcrypto members at 102–155 and 292–344 (or their 4-digit twins), 1010–1033, 1393–1410, 1745–1761, 1824–1882;
libc++abi (cxa_exception/demangle) 1896–1929; libcrypto 1911–2181, 2903–2937, 3238–3259, 4720–4730; the straddle run
10000–10125 (ocb128/gcm128/ccm128/o_names/obj_dat/property/rsa_backend as in §4), 10160–10433, 10557–10599, 11073–11119,
11249–11494 (118 members), 11598–11724, 11862–11895, 13748–13787, 13923–14016; 7zip CPP object at 14058–14068; sqlite3.o chunks
12345–13942 (first chunk 12345–12378, last 13901–13942 in the h2c solve; 218 chunks). The ecourna landed units (8 objects,
anchored through their byte matches) are the only anchors inside the app region; the app itself (ordinals ≈ 300–9,200 by
elimination: everything between the first libcrypto members and the 9xxx libcrypto run) is placed by the band walk (§7.5),
not by the solver.

### 7.5 TU slices (`tools/linkorder/tu_slices.py` → `ledger/tu-clusters.tsv`, `docs/reference/tu-slices.md`)

`tools/linkorder/tu_slices.py` walks every use-count band: anchored (library) functions bound the segments, each segment is
sliced into runs of one *probable file* — the std::source_location records the function references (1,009 functions,
precise), else the file of its vtable class (class → file learned from the records; else the directory convention
`<namespace's most common dir>/<lowercase class>.cpp`, canonicalised to a recorded file of the same basename when
that is unique). Unlabelled functions (libc++ instantiations owned by the TU, string-free helpers) inherit their
neighbours' run; a labelled one-function interruption whose label resumes right after is a lower-zone twin (the
4-digit "1290" sitting inside the 5-digit sqlite run at 12900–12903) and goes to a separate twin sequence.
Result on `ledger/ordinals.tsv`: 1,385 stretches holding 7,339 of 14,343 functions; 460 distinct probable files have a
stretch of ≥ 3 functions; of the 3,270 app-component functions 1,732 carry a label. The uses==1 band's app region
reads as clean reverse link order: e.g. band positions 11,047–11,066 = 20 functions of
`uenux2/src/app/comum/relatorios/crelatoriotesteimpressora.cpp` (18 by class, 1 by record), then `cparterdv.cpp`,
`cparteeleitores.cpp`, `cpartecargos.cpp`, …, then the `dados/asn/estadoaplicacao/cconversor*.cpp` converters one or two
functions each. Each stretch row carries the band, the exact band-position range, the (loose) zone-major ordinal
window, the component mix, the file and the evidence kinds; `docs/reference/tu-slices.md` is the table, `ledger/tu-clusters.tsv`
the machine-readable form (one row per band stretch; a TU appears once per band it has functions in).
