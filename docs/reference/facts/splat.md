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
   happens to land in RHVoice's unicode table in our layout. Never pick one reading of an `i32.const ≥ 1024` blind:
   keep address, slot and literal, and let the target body choose. A vote from an aligned pair whose two values are
   equal is an integer, not an address.
6. **Our table overlaps our data.** With 9,841 table slots and data from 1024, every `i32.const` in 1024…9840 is both
   a possible address and a possible function pointer (`__cxa_allocate_exception`'s 9806 is a table slot that
   target 143 spells 12818).
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
