# The splat build

The splat build gives `vota_web_wasm.wasm` byte for byte however few functions are matched, and counts each
function our C++ produces exactly. It works like splat does for a ROM. The target is split into pieces, and the
module is re-assembled from them. A piece is replaced by our own bytes once our build provably produces
the same body.

There is no `INCLUDE_ASM`. clang can include a whole wasm function, and wasm-ld keeps it byte for byte, but
anything that goes through `em++` then goes through `wasm-opt -Oz`, the `wasm-metadce` reorder and minify
([toolchain.md](toolchain.md) §4, [link-order.md](link-order.md)). Those passes inline, merge and renumber the
whole program, and Binaryen rewrites even a body it only reads and writes back
([include-asm.md](include-asm.md) has the measurements and the decision). So the re-assembly happens after the
toolchain, in our own writer (the vendored toolkit `asmlift-wasm/`, `WASM_KIT` in `src/mk/splat-build.mk`), and a
function not decompiled yet is a comment stub in the sources ([../../src/README.md](../../src/README.md) "Stubs").

```sh
TMPDIR=/tmp make -C src splat
```

This runs link (if stale), link-map and link-kinds (if stale), split (if stale), then the build. It writes
`build/out/splat/vota_web_wasm.wasm` and prints the meter. It fails unless the result has the
target's sha256, `8579ef8d1a93819152cecdbf52906d2e4dd0f57999cb2e1e8c0d1161cd1007e2`, and unless at most
`SPLAT_MAX_KIND_UNVERIFIED` (0) matches are kind-unverified (§3).

## The pipeline

| step | command | reads | writes |
| --- | --- | --- | --- |
| split | `make -C src splat-split` | `target/vota_web_wasm.wasm`, `ledger/functions.tsv`, `ledger/func-attribution.tsv` | `build/out/splat/pieces/` |
| our link | `TMPDIR=/tmp make -C src link` | `src/`, `third_party/`, `src/mk/*.mk`, `src/units/*.mk` | `build/out/wasm/vota_web/vota_web_wasm.wasm` (the plain link) |
| our symbols | `make -C src link-map` | the plain link's own `link.cmd` | `build/out/wasm-map/vota_web/{names.tsv,vota_web_wasm.map}` |
| our operand kinds | `make -C src link-kinds` | the plain link's `link.cmd` and objects, link-map's map | the shifted link `build/out/wasm-kinds/vota_web/{vota_web_wasm.wasm,names.tsv}` and `kinds.json` |
| target symbols | `make -C src splat-symbols` | the target, the plain link, its map, `ledger/` | `splat/symbol_addrs.txt` (committed) |
| scan | `make -C src splat-scan` | all of the above | `splat/scan.tsv` (committed) and the status blocks (`make -C src status`) |
| match | `make -C src splat-match` | the scan | appends its new matches to `splat/matched.txt` (committed), renames a twin whose name moved |
| build | `make -C src splat` | pieces, plain link, maps, operand kinds, symbol file, match list | `build/out/splat/vota_web_wasm.wasm` + meter |

Everything under `build/out/` is generated from the target and our link, and is gitignored. The committed outputs
are `splat/symbol_addrs.txt`, `splat/matched.txt` and `splat/scan.tsv`, with the `src/mk/splat-*.mk` rules.

### 1. Split

`splat-split` cuts the target into pieces under `build/out/splat/pieces/`:

- `asm/<group>/<index>[-<name>].bin`: one function body (the bytes after its size LEB). Each comes with a
  `.wat` beside it, for reading only. A function the split has no name for is `<index>.bin`.
- `data/<nnnn>-<addr8>.bin`: one data segment.
- `manifest.json`: everything else. That covers the section order, types, imports, each function's type
  index, table and elem, memory, globals, exports, data offsets, and each piece's size and sha256.

`<group>` is the function's component. `<name>`, when there is one, is the ledger's best guess, so it can be wrong:
target 10822's piece says `OBJECT_IDENTIFIER`, but the function is `api::AudioCollector`'s destructor. The split
re-runs after every census, so a piece's path can change. Always key a function by its target index, never by its
path.

The splitter ends by assembling its own pieces and fails unless that gives the target's sha256. Editing a
piece in place is refused at assemble time, because each piece's sha256 is in the manifest.

### 2. Symbols

There are two maps, one for each side.

**Our side is exact.** `tools/round/link-map.sh` re-runs the plain link's own command with `-Wl,--Map` and
`EMCC_DEBUG=1`. It refuses to write anything unless that relink is byte-identical to the plain wasm. It
then replays binaryen on the kept intermediates to name every final function index. `names.tsv` maps our
function index to its wasm-ld symbol and object. The map gives every data symbol's address, and wasm-opt
keeps data where wasm-ld put it. What each constant of ours is (an address, a table slot or a plain
integer) comes from a second link of the same objects, with the data and the table moved (§3, "Operand kinds").

**The target side is evidence**, in `splat/symbol_addrs.txt`. Each line is
`<name> = func:N; | import:N; | 0xADDR; // [type:data] [size:0x…] evidence:… [by:…]`. The evidence kinds:

- `glue`, `export`: import and export names from `target/vota_web_wasm.js`.
- `pair-fingerprint`, `pair-addr`: a census pair that is unique on both sides.
- `call`, `table`: aligned call operands and function-pointer constants. They name a function only when
  every vote agrees and our function has the target function's signature.
- `vote:N`: aligned address operands. `by:` lists up to 4 target functions that cast a vote. `vote:N+rtti` is a
  vote that the RTTI decoding agrees with.
- `rtti`: vtables and typeinfos decoded from the target's bytes.
- `content`, `string`, `srcloc`: unique bytes, a C string, or a `std::source_location` record.
- `srcloc-anchor`: a function pair whose bodies are the same instructions and load the same
  `std::source_location` record.

The hand edits name their own evidence in the comment after it. The kinds in use: `vtable` and `vtable-slot` (the
function fills a slot of a vtable decoded from RTTI), `rtti-vptr` (the only target function that stores a class's
vtable address point), `rtti-operand` (the only target function holding a class's vtable and typeinfo addresses),
`pair-depth0` (the only pair with that fingerprint at depth 0, where deeper fingerprints differ only by an inlined
callee) and `callers+content` (the target callers and the loaded content single the function out). Each hand edit's
comment gives the addresses or indices that make the evidence checkable.

A name with conflicting evidence is left out. So is a data name that the target's own bytes contradict.

Lines after `# ---- hand edits below this line …` survive regeneration and win over generated lines. A
hand edit needs evidence that does not come from the match it enables. A name whose only use is to make a
function match proves nothing; see the AudioCollector trap in [facts/splat.md](facts/splat.md) §S4.

### 3. Substitute

A target function is **matched** when four things hold:

1. Our whole-program plain link contains the function.
2. Its body is re-pointed into the target's spaces: callees, `call_indirect` types, globals, imports,
   table slots and data addresses, through both maps.
3. The re-pointed body is byte-identical to the target's piece.
4. Nothing in the re-pointing reads a target body.

**Operand kinds.** The final module has no relocations left, so an `i32.const`, an `i64.const` or a memarg offset
of ours may be an address, a table slot or a plain integer, and the bytes do not say which. Our side learns it from
a second link. `tools/round/link-kinds.sh` (`make -C src link-kinds`) reruns the plain link's own command with
`-sGLOBAL_BASE=1024+D` and `-sTABLE_BASE=1+T` (emcc passes them to wasm-ld as `--global-base` and `--table-base`),
with D = 1 MiB and T = 20,000: each larger than our data (1.46 MB) and our table (10,007 slots), and different from
each other. It names that *shifted link*'s functions the way link-map names the plain one. It refuses to start while
an object or archive on the link line is newer than the plain wasm, and fails unless its map lists the same input
sections as link-map's map of the plain link, in the same order and of the same sizes. The toolkit's `splat-kinds`
then aligns the two links by function name, not index, and writes `build/out/wasm-kinds/vota_web/kinds.json`. In a
function whose instructions are the same in both links, an operand that moved by exactly D is an **address**, by
exactly T a **table slot**, and one that did not move a **literal**. A `-fPIC` object (sqlite's) adds `__table_base`
to a slot relative to it, and binaryen leaves the sum unfolded when the relative part is not one constant
(`vdbeSorterSort`: `select(2035, 2036, 2037) + 1`). That base constant is followed to its use: the base reads as the
target's table base, and the constants added to it as target slots minus that base. Every other moved slot is
followed too, to see whether it is added to constants or to another slot.

Each operand is offered only the readings of its kind, and the target body picks among them only where our side
offers several:

- a literal: its own bytes, nothing else;
- an address: the target address of what it points at, plus every other target place holding the same content (a
  *copy*, below);
- a table slot: the target slot that holds its function. Binaryen merges identical functions, so the target's table
  can hold one function in several slots; each is a copy, all of them (binaryen's merged empty function fills 126). An
  operand of a kind-unverified function is offered a function's slots only up to 64 (`SAME_CONTENT_LIMIT`), and a
  function in more slots leaves its body `unresolved`;
- the table base and a constant relative to it: the target's base, and the target slots of the function in our slot
  base + it, minus the target's base.

An address or slot of ours whose object or function has no target image, or none our side can offer, makes the
body `unresolved`. It is never read as a number.

A function the kind table cannot vouch for is **kind-unverified**. That happens when it has another shape in the
shifted link, when an operand moved by neither shift, when a base constant's use cannot be followed (it is kept in a
local, carried out of a block, or added to anything but constants), or when a moved slot is added to constants or to
another slot (part of the base may be folded in, so nothing tells which part is the base). Its operands keep every
reading their value suggests: below 1024 the literal first, from 1024 the address, else the
table slot, else the literal. The target body then picks the kind as well, so the build lists every kind-unverified
match and fails on more of them than `SPLAT_MAX_KIND_UNVERIFIED` (`src/mk/splat-sub.mk`, 0). Today every function of
our link is kind-verified ([facts/splat.md](facts/splat.md) §S5).

An address of ours resolves through the symbol file's name for its object when there is one. Otherwise
the object's content places it:

- a C string by its bytes through the NUL; every place the target holds the same string is a copy (up to
  4,096 of them: tail merging leaves a short string such as `"\n"` in hundreds of places);
- a `std::source_location` record by what it records;
- a read-only object by its exact bytes, found once (up to 64 copies when found more often);
- **by its re-pointed bytes** (`content-repointed`): a `.data` object, or a read-only one whose exact
  bytes are not in the target (it holds pointers), is our object's bytes with every aligned word that is
  one of our addresses or table slots replaced by what it re-points to (any copy, or the plain integer
  it also is). A word whose pointer resolves nowhere matches anything. The object is placed where these
  bytes occur once, at its alignment, when they hold at least 4 telling bytes (non-zero fixed bytes, 4
  per pointer word at or above 1024, the non-zero bytes of a smaller one). Read-only content
  (`.rodata`, `.data.rel.ro`) found at several places offers each as a copy; a writable object found at
  several places stays unresolved. Placing one object can re-point a word of another, so the placement
  repeats until nothing more resolves. It reads only the two images and the names, never a function body.
  The matches count among the names, and a scan pass re-points with the previous pass's matches applied, so
  a function is first matched before its own name re-points any word of the objects it uses.

Two rules guard the evidence:

- **No circular evidence.** A data name whose only vote came from the function being matched checks
  nothing there. Such a match holds only if our object's bytes agree with the target's at that address,
  or if another holding match uses the same name. Otherwise it is `unresolved (self-voted)` in the scan
  and `UNCONFIRMED` in the build. A function name voted only by the match's own call or table operand holds
  only if another holding match calls it or has a constant the target reads as its table slot. A constant
  the target keeps as an integer does not count, even when it equals the function's slot in our table, so a
  unit that shifts our table cannot make or break a match.
- **A match names its function.** The build re-points with every match applied: the matched symbol names
  that target function, and no other symbol does. A match found by body therefore corrects what its
  callers' calls resolve to.

### 4. Assemble and check

`splat-build` re-points every matched function and compares it with its piece. It then assembles the split
from our bytes for the matched functions and from the original pieces for the rest. The function keeps
the manifest's type index, and sizes and LEBs are re-encoded around the bodies. The build writes nothing
when any match fails, when more matches are kind-unverified than `--max-kind-unverified` allows (make passes
`SPLAT_MAX_KIND_UNVERIFIED`; set it empty to list those matches without failing on them), or when the module is not
sha256 `8579ef8d…1007e2`. After the meter it prints the kind-unverified matches and the review warnings ("Reviewing a
match").

## The drift rule

A match is a promise that our build still produces that body. When a source, flag, link-order or
toolchain change breaks a matched function, `make -C src splat` exits 1 and lists every failing match.
Each failure names the `splat/matched.txt` line, the function, and the first differing instruction on both
sides, with what our operand resolves to. If the function does not resolve, it says why instead.

To repair drift, fix the source until the match verifies again. If the loss is deliberate, delete the
match line in the same commit and say why in the commit message. Never edit a piece, and never add a
symbol-file hand edit to make a match pass.

One failure is not drift: `our link has no function <symbol>` for a byte-identical twin whose surviving name moved,
because a new unit's identical body comes first in the link. The body still verifies under the new name, and
`make -C src splat-match` rewrites the line to it in place and prints the rename; say so in the commit
([../how-to-decompile.md](../how-to-decompile.md) step 10, "A twin whose name moved").

## Matching a function

1. **Pick one.** The work list is the committed scan `splat/scan.tsv`; rows that are not `match` are left to do.
   [../how-to-decompile.md](../how-to-decompile.md) step 1 explains its columns and filters the TSE rows, and its
   "Library functions" section reads the `detail` of a library row.

   Read the target body in its piece's `.wat`, or with the toolkit's `fn` / `lift` commands
   (`asmlift-wasm/README.md`). The piece is `<index>.wat` when the split has no name for the function, and the
   group can be one you do not expect, so search for it:
   `find build/out/splat/pieces/asm \( -name '9131.wat' -o -name '9131-*.wat' \)`.
2. **Write the C++.** Put it where the function lives: an `ECOURNA_SRCS` / `UENUX2_SRCS` line in a unit
   fragment `src/units/<NN>-<component>-<what>.mk` (src/units/README.md), plus a root if nothing keeps it alive.
   [../how-to-decompile.md](../how-to-decompile.md) covers getting there, from the readout and the lifter through
   `tools/round/try-tu.sh`.
3. **Land, census, symbols, match, build**, in that order: [../how-to-decompile.md](../how-to-decompile.md)
   steps 6 to 10 are the one statement of it, with why each step needs the one before. In short:
   `make -C src link`, `make -C src census`, `make -C src splat-symbols` (read its diff: a name that moves or
   disappears can break existing matches), `make -C src splat-match`, `make -C src splat`, each with `TMPDIR=/tmp`.
   A match is `<target index> <our symbol>`, where the symbol is `names.tsv`'s key (`name@object` when several
   objects define it); `splat-match` passes again with its matches applied until they stop changing.

To check one function before the whole scan, use `pnpm wasm splat-remap` from `asmlift-wasm/`:

```sh
URNA=<this checkout>
pnpm --silent wasm splat-remap $URNA/target/vota_web_wasm.wasm \
  $URNA/build/out/wasm/vota_web/vota_web_wasm.wasm sym:<our symbol> --against <target index>
```

The output is `IDENTICAL`, or the unresolved operands, or the first byte no reading explains. This needs
`make -C src link-map` and `make -C src link-kinds` first, which `splat-scan` and `splat` run when they are stale:
`splat-remap` reads the operand-kind table beside the plain link (`build/out/wasm-kinds/vota_web/kinds.json`) and
refuses to run without one, unless given `--no-kinds`, which re-points the body kind-unverified. `<our symbol>` is
the `name` column of `build/out/wasm-map/vota_web/names.tsv` (mangled; look it up there rather than guess: a name
that is not there fails with `no function of ours is called …`). For the 9132 match it prints:

```
ours 10518: re-pointed, 134 bytes; rewritten {"global":3,"func":4}; soft 0; operand kinds verified
target 9132: IDENTICAL (0 ambiguous operands read as -)
```

- `ours N` is our function's index in that link. `rewritten` counts the operands re-pointed, per kind (callees,
  globals, table slots, addresses). The line ends with `operand kinds verified`, or `KIND-UNVERIFIED (<why>)` (§3).
- `soft N` counts, in a kind-unverified function only, the operands that look like an address or a table slot (at
  or above 1024) but resolve to neither. They keep only their literal reading, and each one is listed under the
  line. In a kind-verified function such an operand is unresolved.
- `N ambiguous operands read as …` lists the reading the target body took for each address and table-slot operand
  (§3). A literal has one reading and is not listed: 9132's constants are all literals, and it re-points no address
  or slot. An operand's first reading is its preferred one, the one re-pointing alone writes: the address its object
  resolves to, or the first target slot of its function. An `address same-content 0x…` or `table slot copy …`
  reading is a copy the target body picked, which the meter counts as "which copy". In a kind-unverified function
  the literals are listed too, and the preferred reading goes by value: below 1024 the literal; at or above it the
  address, else the table slot, else the literal. Any other reading there counts as "another reading".

`splat-remap` reads the committed symbol file and none of the matches in `splat/matched.txt`. The scan and the
build re-point with every match applied (§3, "A match names its function"), so `splat-remap` can say `differs` or
`UNRESOLVED` for a function they match: target 6122 is matched, and `splat-remap` reports a soft operand (a vtable
address with no target symbol) and `differs at body byte 6`. The splat build is the verdict.

`pnpm wasm …` runs its command through a shell, which expands `$` even inside single quotes, so a binaryen-merged
symbol (`byn$mgfn-shared$…`) reaches the command cut short. Run the CLI directly for those:
`pnpm --silent exec tsx src/cli.ts splat-remap … 'sym:byn$mgfn-shared$…' --against <index>`.

To check a candidate before it lands, give `splat-remap` a try link instead of the plain one. First map it with
`tools/round/link-map.sh build/out/try/<name>/wasm/vota_web`, then relink it shifted with
`tools/round/link-kinds.sh build/out/try/<name>/wasm/vota_web` and derive its kind table with
`pnpm wasm splat-kinds ../build/out/try/<name>/wasm/vota_web/vota_web_wasm.wasm` (from `asmlift-wasm/`). Or skip the
kinds with `--no-kinds` ([../how-to-decompile.md](../how-to-decompile.md) step 5).

### Reviewing a match

The build is the check: `make -C src splat` re-points every matched function, compares it with its piece, and
writes nothing unless every match verifies, the kind-unverified matches are within their limit, and the module is
the target's sha256. Re-pointing never reads a target body (rule 4 of §3). Our side picks each operand's kind, and
the target body only picks the copy where our side offers several.

What the build lists after the meter is for the reviewer:

- **kind-unverified**: the matches whose operand kinds the shifted link could not tell, with the reason. Their
  operands kept every reading, so the target body picked their kinds too. There are none today.
- **review warnings**: literal operands of matches that may be a target number the source copied. An integer of
  ours is compared as itself, so a number read off the target and written into our source gives the target's bytes,
  and no byte check can reject it. Three kinds are listed: `past-data` (a value in the target's data, past the end of
  ours), `target-object` (the address of a target object the symbol file names, from 64 KiB, or the low word of a
  packed string view) and `past-table` (a value in the target's table, past the end of ours). Thousands of
  legitimate integers fall inside our own data range (OpenSSL's error codes such as 786688), so that range is not
  listed. The warnings are all in upstream library functions built from their own sources.

What no check here rejects, so a reviewer of a TSE function reads it in the source:

- which copy an address or a table slot of ours became. Where our side offers several target places of the same
  content (a C string's copies, read-only content found at several places by its bytes or its re-pointed bytes) or
  several target slots of one function (126 for binaryen's merged empty function), the target body picks one, and no check proves that the place
  holds the object our source names. A pointer swapped for one to another object of ours whose content also sits at
  the place the target body uses gives the target's bytes. Calls through any slot of a function are the same, but
  pointer identity differs;
- a number equal to the target's number of another kind: the target's slot number written as an integer inside our
  table (every small integer is some target slot), a pointer of ours whose re-pointed value equals an integer the
  target holds, or a target address written as an integer that lies inside our data range and is not the start of a
  named target object. The module is the target byte for byte, but our source means something else. Neither binary
  records the target's kind. The review warnings list only the target numbers outside our data and table.

[facts/splat.md](facts/splat.md) §S5 has the measurements behind these lists and the cases the tests pin.

A reviewer who wants to see the matched bytes come from our link, with the target's body gone, runs the
**blanked-target check**: `splat-review` (`asmlift-wasm/src/splat-review.ts`), from `asmlift-wasm/`, with the target
index of the match. It takes about five seconds, reuses the split in `build/out/splat/pieces` and relinks nothing:

1. it blanks the matched piece in a copy of the split: a valid body of the same size (a `0x00` local count,
   `unreachable` bytes, a final `0x0b`; random bytes would make the module fail to parse), with its sha256 in the
   copy's manifest. The copy is held in memory, so `build/out/splat/pieces` is never changed;
2. it takes our body for that function in two ways (below);
3. it assembles the blanked split with each body and compares the sha256 with the target's.

The two ways differ in what picks the readings of the ambiguous operands:

- **The match's own bytes.** `verifyMatches` on the real split gives the bytes the build substitutes, with the
  readings the target body picked. This checks that the build's output comes from our link, under the match rule.
  It is the gate: the command exits non-zero only when this way does not give the target's sha256 (or when the
  build would refuse the match: it differs, is unresolved, or is `UNCONFIRMED`).
- **Preferred readings.** The blanked split, assembled into a module with `target/vota_web_wasm.js` copied beside
  it, is loaded as the target side (`loadTargetSide` takes the import names from the glue next to the wasm it reads;
  without it every `invoke_*` call is `UNRESOLVED`), and `remapBody(...).bytes` sets every operand at its preferred
  (first) reading. No target body exists anywhere in that run, so this is stricter than the match rule. A match whose
  operands need the target body's choice among our readings (which copy, which table slot of a function, and in a
  kind-unverified function which kind: §3) fails it legitimately. The command then lists each such operand, the
  reading the target body picked and the first one. With operand kinds, the preferred bytes of 9131, 9132 and 9218 are
  their pieces. Without them (`--no-kinds`), 9218's are 313 B against its 311 B piece, because the target body then
  picks its operands' kinds.

In both ways the maps get the matches of `splat/matched.txt` first (`withMatches(maps, matches)` with the `matches` of
`parseMatchList`, both in `splat-sub.ts`), as the build does (`buildSplat`): each matched symbol then names its
target function, and a call to a function that only its match names re-points as in the build. Without them such a
call can resolve elsewhere or not at all, and the review checks something other than the build. `--matches <f>`
reviews against another match list, `--no-kinds` checks without the operand-kind table, and `-o <dir>` is where the
blanked module and the glue copy go (default `build/out/splat-review/<index>/`).

9218 passes both ways:

```sh
pnpm --silent wasm splat-review ../target/vota_web_wasm.wasm ../build/out/wasm/vota_web/vota_web_wasm.wasm 9218
# function 9218 ← _ZNK7ecourna3app5dados3asn27CConversorCabecalhoEntidade12DoDeconverteERKN21ModuloTiposEleitorais17CabecalhoEntidadeE (ours 6934), line 7210 of ../splat/matched.txt; operand kinds verified
# blanked: asm/ecourna/9218.bin (311 B) → no locals, unreachable, end; the blanked split is sha256 aea686604b44fb4816e9655670966099fe352a00ed5d9de77e5c6569b9b816c2, written to ../build/out/splat-review/9218/vota_web_wasm.wasm
# the match's own bytes: REPRODUCES the target, sha256 8579ef8d1a93819152cecdbf52906d2e4dd0f57999cb2e1e8c0d1161cd1007e2 (311 B; 16 ambiguous operands at the readings the target body picked, 0 of them not the first)
# preferred readings:    REPRODUCES the target, sha256 8579ef8d1a93819152cecdbf52906d2e4dd0f57999cb2e1e8c0d1161cd1007e2 (311 B; every operand at its first reading, the blanked module as the target side)
# the gate is the match's own bytes: pass (3.4 s)
```

12829 `sqlite3_bind_pointer` passes the gate and fails the preferred-readings way legitimately. Its `i32.const 2044`
points to the function in our slot 2044 (named `probe_client::done` in our link), which the target's table holds in
126 slots, binaryen's merged empty function ([facts/splat.md](facts/splat.md) §S5 item 4). The target body picked
slot 11892, and the first reading is slot 316:

```sh
pnpm --silent wasm splat-review ../target/vota_web_wasm.wasm ../build/out/wasm/vota_web/vota_web_wasm.wasm 12829
# function 12829 (sqlite3_bind_pointer) ← sqlite3_bind_pointer (ours 4854), line 7225 of ../splat/matched.txt; operand kinds verified
# blanked: asm/libc++/12829-sqlite3_bind_pointer.bin (135 B) → no locals, unreachable, end; the blanked split is sha256 737990be702d6a69184e193f64fe5b8ff96ec3db8960233f2f46ec8b1b1d6520, written to ../build/out/splat-review/12829/vota_web_wasm.wasm
# the match's own bytes: REPRODUCES the target, sha256 8579ef8d1a93819152cecdbf52906d2e4dd0f57999cb2e1e8c0d1161cd1007e2 (135 B; 3 ambiguous operands at the readings the target body picked, 1 of them not the first)
# preferred readings:    DIFFERS, sha256 96a6cbe5d01c21a6ab560dae90dbdddc6f4bf7587f40c9556c5a26c85f40017a (134 B against the match's 135 B, from body byte 43)
#   legitimately: the match needs the target body's choice among our readings of 1 operand (docs/reference/splat.md §3):
#   ours +0x2a i32.const 2044 (target body byte 43): the target body read table slot copy 11892 of symbol _ZN12_GLOBAL__N_112probe_client4doneEv; the first reading is table slot copy 316 of symbol _ZN12_GLOBAL__N_112probe_client4doneEv
# the gate is the match's own bytes: pass (1.7 s)
```

A preferred-readings failure with no such operand listed (`NOT explained by a reading the target picked`) is not
legitimate: report it. Say which way gave which result.

## The meter

`TMPDIR=/tmp make -C src splat` ends by printing the meter: the matched functions and code bytes, in total and per
component, then how often the target body had to choose among our readings of an operand. The latest meter, grouped
by origin, and the scan's verdicts are in [../status.md](../status.md), with the commands that regenerate them.

How to read the meter:

- A component is the target function's caller attribution (`ledger/func-attribution.tsv`), not its
  library. For example, libc's `pad` (target 788) counts as RHVoice.
- "another reading" and "which copy" count matches where the target body chose among our readings of an
  operand: "which copy" when the choice was among copies of the same content or among the table slots of one
  function, "another reading" when it chose an operand's kind, which happens only in a kind-unverified match (0
  today). "Re-pointing alone" counts matches where no choice was needed.
- The lines after it list the kind-unverified matches and the review warnings ("Reviewing a match").
- Matches of tiny bodies (8 B or less) and of bodies with a byte-identical twin in the target carry an arbitrary
  name, because binaryen merges identical bodies. [../status.md](../status.md) "Reading the meter" counts both,
  from the committed scan and the target.
