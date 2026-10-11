# Facts: splat symbol maps and matches

What the splat build's symbol maps rest on, and the ways a match can verify and still be wrong. `docs/reference/splat.md`
describes the build (`make -C src splat`), the match list `splat/matched.txt` and the symbol file
`splat/symbol_addrs.txt`; this file holds the measurements behind them. Toolkit commands run from
`asmlift-wasm/` as `pnpm wasm <command>`.

## S1. Our side of the maps is exact

These hold on the plain skeleton link (`build/out/wasm/vota_web`), whose functions `make -C src link-map` names:

1. **The map flag changes no byte.** Re-running the plain link's own command (`build/out/wasm/vota_web/link.cmd`)
   with `-Wl,--Map=… -Wl,--no-demangle` and `EMCC_DEBUG=1` into another directory gives a wasm byte-identical to
   the plain one. `make -C src link-map` (`tools/round/link-map.sh`) does this and fails if they ever differ, so
   the real link stays untouched and the scratch relink's map describes it.
2. **wasm-opt does not move data.** The final module's memory image equals wasm-ld's output at every address
   below the em_js block; the only difference is the em_js segment (the EM_JS source strings, moved into the
   glue), which is zero in the final module. `.bss` ends at `__stack_pointer`'s initial value minus 8 MiB.
3. **Every final function is named exactly by replay.** `wasm-opt -Oz` rerun on wasm-ld's output stripped of
   names, with `--symbolmap`, reproduces the post-Oz module byte for byte (post-Oz index → wasm-ld index);
   wasm-metadce and the final wasm-opt rerun with index names and `-g` reproduce the plain code section (final
   index → post-Oz index). So every function of the link is named (`build/out/wasm-map/vota_web/names.tsv`, one row
   per function), the bodies binaryen merged as `byn$mgfn-shared$<first merged function>`
   (`grep -c 'byn\$mgfn-shared' build/out/wasm-map/vota_web/names.tsv` counts them).
4. **The `--emit-symbol-map` twin is the plain link, permuted.** `pnpm wasm splat-twin-check <plain> <twin>`
   pairs most functions by a name unique on both sides and the rest by body; every twin body equals its plain body
   once function, type and import indices are re-pointed (`different: 0`); the table and the data agree.
   So substitution reads bodies from the plain build and names them through `names.tsv`; it needs no remap
   layer.

## S2. Coverage of the target symbol file

`pnpm wasm splat-symbols target/vota_web_wasm.wasm build/out/wasm/vota_web/vota_web_wasm.wasm`
(`make -C src splat-symbols`) writes `splat/symbol_addrs.txt`: names for part of the 14,343 target functions, the
136 imports (from the glue) and data objects. Each line's `evidence:` says where its name comes from. Functions:
a pair by unique fingerprint (`pair-fingerprint`) or address-blind body (`pair-addr`), call votes (`call`),
table-constant votes (`table`), exports (`export`), the slots of RTTI-named vtables (`vtable`, §S7). Data: RTTI
(`rtti`), address votes (`vote:N`), content (`content`), strings (`string`; a text that occurs twice in the target
is left out) and `source_location` records (`srcloc`). Count the lines per evidence with

```sh
grep -o 'evidence:[a-z0-9-]*' splat/symbol_addrs.txt | sort | uniq -c | sort -rn
```

`pnpm wasm splat-sym-coverage <target> <plain>` prints `target functions with a symbol: <named>/<all> (<bytes>/<all>
body bytes)` and re-points the ledger's skeleton pairs through the symbol file, per tier: nearly every byte and
fingerprint pair re-points with nothing unresolved; of the addr pairs, the rest keep only an ambiguous constant
≥ 1024 or stay unresolved (mostly a callee the symbol file does not name, or a string whose text occurs twice).
It prints, for example, byte 893 of 895, fingerprint 525 of 529 and addr 4,798 of 5,499. `--smoke 12` re-points 12
non-byte pairs and checks each against its target body: each gives exactly the target body.

## S3. Traps in building the symbol maps

1. **The ledger's `best_index`/`best_name` is positional inside a non-unique group.** Some ledger address pairs
   name a different function than their callers' call votes. The `i2d_`/`d2i_` wrappers differ only by their `ASN1_ITEM`
   address and the item's `sname` settles it: target 3282 holds the "DSA" item although the ledger calls it
   `i2d_RSAPrivateKey`, and 4263 holds "RSAPrivateKey" (the same swap for 4561/6890, 4562/4583/4595/6947/6892,
   1644/1646; `asmlift-wasm/test/splat-symbols.test.ts` pins it). Name a target function from a pair only when its
   census group has one member on each side, or from its callers.
2. **`EMCC_DEBUG`'s `emcc-02-strip.wasm` still has the name section** (emcc saves it before `llvm-objcopy`
   strips it). A replay of -Oz on it as saved reads the names and does not reproduce the next stage: strip the name
   and producers sections first.
3. **wasm-ld's `--Map` names a `signature_mismatch` stub by the function's plain name**
   (`<internal>:(HTS_Audio_initialize)`), colliding with the real one. Take function names from wasm-ld's name
   section and only the object from the map.
4. **The twin permutes imports and types too.** Re-pointing only call indices reports false differences (calls to
   imports such as `close`, `fopen`, `sbrk`, and a `call_indirect` type). Re-point imports by glue name and types by
   signature.
5. **An integer can sit inside one of our data objects.** `BN_CTX_get`'s `i32.const 786688` is a constant that
   happens to land in RHVoice's unicode table in our layout. Never pick one reading of an `i32.const ≥ 1024` by its
   value. The splat check reads the kind from the shifted link (§S5); only with `--no-kinds` does it keep address,
   slot and literal and let the target body choose. A vote from an aligned pair whose two values are equal is an
   integer, not an address.
6. **Our table overlaps our data.** Our table has thousands of slots and our data starts at 1024, so every
   `i32.const` from 1024 up to the table's size is both a possible address and a possible function pointer (`__cxa_allocate_exception`'s 9806 is a table slot that
   target 143 spells 12818). Only the shifted link tells them apart (§S5).
7. `python3 -I` still writes `__pycache__` beside an imported module: run the Python tools with `-B`.

## S4. A vote can map one class onto another, and the match still verifies

A data symbol whose only evidence is a `vote` can put one of our objects onto a different object in the target; a
match that re-points through that symbol then matches byte for byte and is still wrong.

The case: aligning our ASN.1 destructors with target 10822 and 10840 votes twice for
`_ZTVN4ASN117OBJECT_IDENTIFIERE` at target 0x18336c, where RTTI names `_ZTVN3api14AudioCollectorE`. The target's
vtable there has typeinfo 0x1833a8, whose name pointer gives `N3api14AudioCollectorE`;
`grep -c N4ASN117OBJECT_IDENTIFIERE target/vota_web_wasm.wasm` → 0. A symbol file with both names lets the matches
10822 `_ZN4ASN117OBJECT_IDENTIFIERD0Ev` and 10840 `…D2Ev` verify exactly — their only re-pointed address is
`i32.const 1586036` = 0x18336c + 8 — but they are `api::AudioCollector`'s destructors, and our ASN.1 destructors
happen to compile to the same shape. Leaving out a name that has conflicting addresses does not catch it: here two
names share one address. So `splat-symbols` drops the data names the target's own bytes contradict
(`refuteDataNames` in `asmlift-wasm/src/splat-symbols.ts`): an RTTI symbol whose bytes describe another class, a
voted object whose bytes differ from ours, and one of two names at one target address that cannot both be right (a
vote yields to independent evidence). `asmlift-wasm/test/splat-symbols.test.ts` (`refuteDataNames`) and
`splat-sub.test.ts` pin this case, and `grep -c _ZTVN4ASN117OBJECT_IDENTIFIERE splat/symbol_addrs.txt` prints 0.

Related facts:

- **Self-certification is common.** In nearly every (match, a `vote:1` symbol used only by that match) row, the
  match's own ledger pair is the target's skeleton pair: the one vote comes from the matched function itself, and
  that operand checks nothing.
- **An independent check exists wherever the object names itself.** For a vtable: vtable + 4 → typeinfo, typeinfo
  + 4 → the name. The vote-evidenced vtables and typeinfos, but for this one, decode to their own class. Every
  vote-evidenced `ASN1_ITEM` `*_it.local_it` entry carries the same `sname` (item + 24) on both sides. An `sname`
  is not the symbol name (`DSAPrivateKey_it`'s is "DSA"): compare ours with the target's, never the `sname` with
  the symbol.

So a hand edit to `splat/symbol_addrs.txt` needs evidence that does not come from the match it enables.

## S5. Operand kinds: our side says what each constant is

Without a kind table (`--no-kinds`), the check offers every `i32.const` (and every memarg offset of 1024 or more)
each reading its value allows: the literal, the target address of what it points at in our data, and the target slot
of the function in our table slot. The target's bytes then pick the reading, so an operand's kind comes from the
target. Patch one operand of a copy of our link and run `splat-build` that way, and these wrong values pass: an
integer of ours equal to one of our table slots whose function the target keeps in the target's slot of that number
(c2); an address written as the target's number for the right string (c5); a function-pointer slot written as the
target's slot number (c8); and a vtable address written as the target's, past the end of our data (c9). The case
names are those of `asmlift-wasm/test/splat-kinds.test.ts` (item 3).

`make -C src link-kinds` learns the kinds from a second link (`docs/reference/splat.md` §3 "Operand kinds"). On the
plain link and the matches of `splat/matched.txt`, after `make -C src splat`, from `asmlift-wasm/`
(`build/out/splat-check/` is a scratch directory):

```sh
mkdir -p ../build/out/splat-check
pnpm --silent wasm splat-kinds ../build/out/wasm/vota_web/vota_web_wasm.wasm -o ../build/out/splat-check/kinds.json \
  --matches ../splat/matched.txt
# prints, for example:
# ../build/out/splat-check/kinds.json: data moved by 1048576, table by 20000; 10854 functions in 0.6 s
#   every function: 10854/10854 kind-verified; operands: 527074 literal, 68835 address, 9890 table slot, 2 table base + 6 relative to it
#   the 7217 matches: 7217/7217 kind-verified; operands: 261150 literal, 26256 address, 1518 table slot, 1 table base + 3 relative to it
```

The counts grow with the link and the match list; what holds is `N/N kind-verified` on both lines.

1. **The shifted link is the plain link with two bases moved.** `tools/round/link-kinds.sh` checks that its wasm-ld
   map lists the same input sections, in the same order and of the same sizes, as link-map's map of the plain link;
   only addresses and the sizes of the TABLE, GLOBAL, ELEM and DATA output sections differ. Every function of the
   plain link has a namesake with the same instructions there, and every constant moved by exactly D = 1,048,576,
   by exactly T = 20,000, or not at all. So no function is kind-unverified. The same holds on a try link
   (`tools/round/link-kinds.sh build/out/try/<name>/wasm/vota_web`): the relink and its binaryen replay take about
   40 s at a load average of 68, and `splat-kinds` on it kind-verifies every function.
2. **The target body does not pick a kind.** The same match list built without and with the table, from
   `asmlift-wasm/` (each prints, for example, the lines below):

   ```sh
   pnpm --silent wasm splat-build ../target/vota_web_wasm.wasm ../build/out/wasm/vota_web/vota_web_wasm.wasm \
     --matches ../splat/matched.txt --no-kinds -o ../build/out/splat-check/nokinds.wasm
   #   re-pointing alone     5321 matches (845384 B)
   #   another reading       1638 matches (911037 B), 7068 operands: literal 5634, table slot 1434
   #   which copy             258 matches (360326 B), 497 operands
   pnpm --silent wasm splat-build ../target/vota_web_wasm.wasm ../build/out/wasm/vota_web/vota_web_wasm.wasm \
     --matches ../splat/matched.txt -o ../build/out/splat-check/kinds.wasm
   #   re-pointing alone     6964 matches (1758167 B)
   #   another reading          0 matches (0 B), 0 operands: -
   #   which copy             253 matches (358580 B), 491 operands
   ```

   Both outputs have the same sha256 (`shasum -a 256 ../build/out/splat-check/{nokinds,kinds}.wasm`), so the same
   matches verify when each operand is offered only its own kind. Without the table, the operands of "another
   reading" are those whose value suggests another kind than the one the target body takes: integers that look like addresses (OpenSSL's error code 786688 lies in RHVoice's
   data in our layout, §S3.5), and function pointers that look like integers or addresses. No rule on the value alone
   tells them apart, and with the table none is needed.
3. **The perturbation cases are tests.** `pnpm test test/splat-kinds.test.ts` pins them (it needs the build outputs
   of `make -C src splat`; without them the cases are skipped). Each case patches one operand, at a body offset of one
   function, in a copy of our plain link and its shifted link (the `plain` and `shifted` values of the case), and
   builds that function's match:
   - c2, c5 and c8 build the target with every reading offered, and fail with the table (`DIFFERS`): the operand is a
     literal, an address and a slot, and only that reading is offered.
   - c1, c3, c4, c6, c7 and c10 (wrong values that coincide with nothing) stay rejected.
   - c9, c5L, I2 and TH2 (the target's number written as an integer) build the target and are each one review
     warning. An integer of ours is compared as itself, so no byte check can reject them.
   - c8L (the target's slot 69 written as an integer, inside our table) and c2S (an integer of the target's written
     as a pointer to the function the target keeps in that slot) build the target with no warning. These are the
     limit of any check that compares bytes.
   - V1 and V2 (an integer written over one of `vdbeSorterSort`'s slots relative to `__table_base`) are read relative
     to the base and rejected.
   - Where a reading of the objects' relocations disagrees with the table, the table is right. `i32.const 2077` in
     `sqlite3AddDefaultValue` +0x292 is a table slot (both links' slot holds `renameUnmapSelectCb`), although a
     relocation reading takes it for an address. `vdbeSorterSort`'s 2035, 2036 and 2037 are relative to
     `__table_base` and name `vdbeSorterCompareInt`, `vdbeSorterCompareText` and `vdbeSorterCompare`, not the
     functions in slots 2035 to 2037.
4. **A table slot of known kind is offered every target slot of its function.** Binaryen merged identical
   functions, and the target's table holds its empty `(i32) -> ()` function (target 218, a 2-byte body) in 126 slots.
   Each slot holds the same function, so a pointer of ours to it is offered all 126; an operand of a kind-unverified
   function is still offered a function's slots only up to `SAME_CONTENT_LIMIT` (64, `asmlift-wasm/src/splat-symbols.ts`).
   From `asmlift-wasm/`:

   ```sh
   pnpm --silent wasm splat-remap ../target/vota_web_wasm.wasm ../build/out/wasm/vota_web/vota_web_wasm.wasm \
     sym:sqlite3_bind_pointer --against 12829
   # target 12829: IDENTICAL (3 ambiguous operands read as table slot copy 11892 of symbol
   #   _ZN12_GLOBAL__N_112probe_client4doneEv, address same-content 0x6de8b, address symbol sqlite3Config)
   ```

   With `--no-kinds` the body `differs at body byte 43 (target instruction +2a i32.const 11892)`. Functions that
   match through this rule include 7201 `do_load_builtin_modules_ossl_`, 7880 and 7882 (`money_get<wchar_t>`), 12829
   `sqlite3_bind_pointer`, 12863 `sqlite3_result_pointer` and 12932 `sqlite3_mutex_alloc`;
   `grep -c 'fills 126 slots' splat/scan.tsv` prints 0. The target body picks which of the 126 slots each pointer
   becomes, so check each pick against the two links' tables (read both element segments): every slot the target
   body uses has the same neighbours as our slot. For example our 2044 and the target's 11892 both
   sit between `getPageNormal` and `sqlite3RCStrUnref`, and `sqlite3_mutex_alloc`'s nine slots 2018 to 2029 line up
   with 11866 to 11877, `pthreadMutexFree` and `pthreadMutexEnd` included. The one copied address,
   `sqlite3_bind_pointer`'s and `sqlite3_result_pointer`'s `""` (SQLite's default `zPType`), is the NUL after a
   `" \t"` string in both links (ours 0x32590, the target's 0x6de8b).
5. **The build's lists.** `TMPDIR=/tmp make -C src splat` prints `kind-unverified: 0/<matches> matches` and
   `review warnings: <n> literal operands … <n> past-data, <n> target-object, <n> past-table`. The warnings are in
   upstream library functions built from their own sources (sqlite, OpenSSL, libc++, 7-Zip, RHVoice, zlib's minizip),
   whose numbers come from those sources.

The limits of the check that remain are listed in `docs/reference/splat.md` "Reviewing a match".

## S6. Our link merges byte-identical functions too, and a new twin can take a match's name

binaryen merges byte-identical functions in our link as it did in the target, and the link map names the merged
body once: `build/out/wasm-map/vota_web/names.tsv` lists one name per function. Unit 20 (commit 1f6d86a) brought in
the deleting destructor of `ModuloEstadoGeralDefs::NumViasImpressasRelatorios`, a generated ASN.1 class, which is
byte-identical to `ModuloTiposEleitorais::Foto`'s. The converter's object instantiates it, and the uenux2 objects
come before the ASN.1 modules on the link line (`LINK_OBJS` in `src/Makefile`; the modules ride in `EXTRA_OBJS`).
The merged body's name became the new one (`names.tsv`'s object column names the converter's object), and
`_ZN21ModuloTiposEleitorais4FotoD0Ev` left our link:

- before: `git show 58b2352:splat/matched.txt | awk -v n=246 '$(1) == n'` prints
  `246 _ZN21ModuloTiposEleitorais4FotoD0Ev`;
- after the unit's link, census, symbols and match, `TMPDIR=/tmp make -C src splat` failed with `function 246
  matched to _ZN21ModuloTiposEleitorais4FotoD0Ev: our link has no function _ZN21ModuloTiposEleitorais4FotoD0Ev`,
  while `splat/scan.tsv` already paired 246 with `_ZN21ModuloEstadoGeralDefs26NumViasImpressasRelatoriosD0Ev` by
  content;
- `splat-match` keeps every existing line and only appends (`addMatches` in `asmlift-wasm/src/splat-sub.ts`), so
  it does not rename the stale line. Deleting it and running `make -C src splat-match` again added
  `246 _ZN21ModuloEstadoGeralDefs26NumViasImpressasRelatoriosD0Ev`, and the build verified it;
- today `/usr/bin/grep -c FotoD0Ev build/out/wasm-map/vota_web/names.tsv` prints 0 and
  `awk -v n=246 '$(1) == n' splat/matched.txt` prints the new name.

`git diff 58b2352 -- splat/matched.txt` shows `-246 …FotoD0Ev` although no match was lost; count lost matches by
index (`docs/how-to-decompile.md` step 10). The same merging makes a callee's matched name possibly another
instantiation's: 760 is matched as `vector<pair<unsigned long, unsigned long>>::push_back(&&)`
(`awk -v n=760 '$(1) == n' splat/matched.txt`).
