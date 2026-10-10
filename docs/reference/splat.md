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

This runs link (if stale), link-map (if stale), split (if stale), then the build. It writes
`build/out/splat/vota_web_wasm.wasm` and prints the meter. It fails unless the result has the
target's sha256, `8579ef8d1a93819152cecdbf52906d2e4dd0f57999cb2e1e8c0d1161cd1007e2`.

## The pipeline

| step | command | reads | writes |
| --- | --- | --- | --- |
| split | `make -C src splat-split` | `target/vota_web_wasm.wasm`, `ledger/functions.tsv`, `ledger/func-attribution.tsv` | `build/out/splat/pieces/` |
| our link | `TMPDIR=/tmp make -C src link` | `src/`, `third_party/`, `src/mk/*.mk`, `src/units/*.mk` | `build/out/wasm/vota_web/vota_web_wasm.wasm` (the plain link) |
| our symbols | `make -C src link-map` | the plain link's own `link.cmd` | `build/out/wasm-map/vota_web/{names.tsv,vota_web_wasm.map}` |
| target symbols | `make -C src splat-symbols` | the target, the plain link, its map, `ledger/` | `splat/symbol_addrs.txt` (committed) |
| scan | `make -C src splat-scan` | all of the above | `splat/scan.tsv` (committed) and the progress blocks (`make -C src status`) |
| claim | `make -C src splat-claim` | the scan | appends its new matches to `splat/matched.txt` (committed) |
| build | `make -C src splat` | pieces, plain link, maps, symbol file, claim list | `build/out/splat/vota_web_wasm.wasm` + meter |

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
keeps data where wasm-ld put it.

**The target side is evidence**, in `splat/symbol_addrs.txt`. Each line is
`<name> = func:N; | import:N; | 0xADDR; // [type:data] [size:0x…] evidence:… [by:…]`. The evidence kinds:

- `glue`, `export`: import and export names from `target/vota_web_wasm.js`.
- `pair-fingerprint`, `pair-addr`: a census pair that is unique on both sides.
- `call`, `table`: aligned call operands and function-pointer constants.
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
hand edit needs evidence that does not come from the claim it enables. A name that only makes a claim
match proves nothing; see the AudioCollector trap in [facts/splat.md](facts/splat.md) §S4.

### 3. Substitute

A target function is **matched** when four things hold:

1. Our whole-program plain link contains the function.
2. Its body is re-pointed into the target's spaces: callees, `call_indirect` types, globals, imports,
   table slots and data addresses, through both maps.
3. The re-pointed body is byte-identical to the target's piece.
4. Nothing in the re-pointing reads a target body.

An `i32.const` can be an address, a table slot or a plain integer, and so can a memarg offset of 1024 or
more. Content can also sit at several places in the target, such as a tail-merged string. In these cases
every reading from our side is kept, and the target body only picks among them. The meter reports how
often it had to pick.

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
  The claims count among the names, and a scan pass re-points with the previous pass's matches claimed, so
  a function is first matched before its own name re-points any word of the objects it uses.

A table slot reading names the slot the target's table holds the function in. When binaryen merged
identical functions, the target's table holds the survivor in several slots, and each slot is a copy.

Two rules guard the evidence:

- **No circular evidence.** A data name whose only vote came from the function being matched checks
  nothing there. Such a match holds only if our object's bytes agree with the target's at that address,
  or if another holding match uses the same name. Otherwise it is `unresolved (self-voted)` in the scan
  and `UNCONFIRMED` in the build.
- **A claim names its function.** The build re-points with every claim applied: the claimed symbol names
  that target function, and no other symbol does. A match found by body therefore corrects what its
  callers' calls resolve to.

### 4. Assemble and check

`splat-build` re-points every claimed function and compares it with its piece. It then assembles the split
from our bytes for the claimed functions and from the original pieces for the rest. The function keeps
the manifest's type index, and sizes and LEBs are re-encoded around the bodies. The build writes nothing
in two cases: when any claim fails, or when the module is not sha256 `8579ef8d…1007e2`.

## The drift rule

A claim is a promise that our build still produces that body. When a source, flag, link-order or
toolchain change breaks a claimed function, `make -C src splat` exits 1 and lists every failing claim.
Each failure names the `splat/matched.txt` line, the function, and the first differing instruction on both
sides, with what our operand resolves to. If the function does not resolve, it says why instead.

To repair drift, fix the source until the claim verifies again. If the loss is deliberate, delete the
claim line in the same commit and say why in the commit message. Never edit a piece, and never add a
symbol-file hand edit to make a claim pass.

## Claiming a function

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
3. **Land, census, symbols, claim, build**, in that order: [../how-to-decompile.md](../how-to-decompile.md)
   steps 6 to 10 are the one statement of it, with why each step needs the one before. In short:
   `make -C src link`, `make -C src census`, `make -C src splat-symbols` (read its diff: a name that moves or
   disappears can break existing claims), `make -C src splat-claim`, `make -C src splat`, each with `TMPDIR=/tmp`.
   A claim is `<target index> <our symbol>`, where the symbol is `names.tsv`'s key (`name@object` when several
   objects define it); `splat-claim` passes again with its matches claimed until they stop changing.

To check one function before the whole scan, use `pnpm wasm splat-remap` from `asmlift-wasm/`:

```sh
URNA=<this checkout>
pnpm --silent wasm splat-remap $URNA/target/vota_web_wasm.wasm \
  $URNA/build/out/wasm/vota_web/vota_web_wasm.wasm sym:<our symbol> --against <target index>
```

The output is `IDENTICAL`, or the unresolved operands, or the first byte no reading explains. This needs
`make -C src link-map` first, which `splat-scan` and `splat` run when it is stale. `<our symbol>` is the `name`
column of `build/out/wasm-map/vota_web/names.tsv` (mangled; look it up there rather than guess: a name that is not
there fails with `no function of ours is called …`). For the 9132 claim it printed:

```
ours 10514: re-pointed, 134 bytes; rewritten {"global":3,"table":2,"func":4}; soft 0
target 9132: IDENTICAL (2 ambiguous operands read as literal)
```

- `ours N` is our function's index in that link. `rewritten` counts the operands re-pointed, per kind (callees,
  globals, table slots, addresses).
- `soft N` counts the operands that look like an address or a table slot (at or above 1024) but resolve to
  neither. They keep only their literal reading, and each one is listed under the line.
- `N ambiguous operands read as …` lists the readings the target body took for its ambiguous operands (§3). An
  operand's preferred reading is the one re-pointing alone writes: below 1024 the literal; at or above it the
  address, else the table slot, else the literal. 9132's two operands are below 1024, so `literal` was the preferred
  reading and the match needed no choice. Any other reading (a `literal` at or above 1024, a table slot where an
  address or a literal leads) counts as "another reading" in the meter. The output does not say which case it
  was, and neither does the scan's `readings:` detail, so look at the operand's value in the body.

`splat-remap` reads the committed symbol file and none of the claims in `splat/matched.txt`. The scan and the
build re-point with every claim applied (§3, "A claim names its function"), so `splat-remap` can say `differs` or
`UNRESOLVED` for a function they match: target 6122 is claimed, and `splat-remap` reports a soft operand (a vtable
address with no target symbol) and `differs at body byte 6`. The splat build is the verdict.

`pnpm wasm …` runs its command through a shell, which expands `$` even inside single quotes, so a binaryen-merged
symbol (`byn$mgfn-shared$…`) reaches the command cut short. Run the CLI directly for those:
`pnpm --silent exec tsx src/cli.ts splat-remap … 'sym:byn$mgfn-shared$…' --against <index>`.

To check a candidate before it lands, give `splat-remap` a try link instead of the plain one, after
`tools/round/link-map.sh build/out/try/<name>/wasm/vota_web` has mapped it
([../how-to-decompile.md](../how-to-decompile.md) step 5).

### Reviewing a claim

The build is the check: `make -C src splat` re-points every claimed function, compares it with its piece, and
writes nothing unless every claim verifies and the module is the target's sha256. Re-pointing never reads a target
body (rule 4 of §3); the target body only picks among the readings our side offers.

A reviewer who wants to see the claimed bytes come from our link, with the target's body gone, has no command for
it. The pieces are checked against `manifest.json`, so blanking a piece makes `splat-build` refuse, and
`splat-assemble --substitute` takes a `<index>.bin` body that no command writes. The reviews of 9131 and 9132 used
a script against the toolkit's exports (`remapBody`, `verifyClaims` and `loadTargetSide` from
`asmlift-wasm/src/splat-*.ts`, `assembleSplit` from `splat.ts`):

1. copy the pieces, replace the claimed piece with a valid body of the same size (a `0x00` local count,
   `unreachable` bytes, a final `0x0b`: random bytes make the module fail to parse), and write its new sha256
   into the copy's `manifest.json`;
2. take our body for it, in one of two ways (below);
3. assemble the blanked split with that body, and compare the sha256 with the target's.

The two ways differ in what picks the readings of the ambiguous operands:

- **The claim's own bytes.** `verifyClaims` on the real split gives the bytes the build substitutes, with the
  readings the target body picked. This checks that the build's output comes from our link, under the claim rule.
- **Preferred readings.** Assemble the blanked split into a module, copy `target/vota_web_wasm.js` beside it
  (`loadTargetSide` takes the import names from the glue next to the wasm it reads; without it every `invoke_*`
  call is `UNRESOLVED`), load that module as the target side, and take `remapBody(...).bytes`, every operand at
  its preferred reading. No target body exists anywhere in that run, so this is stricter than the claim rule: 9132
  passes it, while 9218, a claim that holds, gives 313 B against its 311 B piece because it needs the target to
  pick readings.

Say which way a review used. A TypeScript script with top-level `await` runs under `pnpm exec tsx` only as an
`.mts` file.

## The meter

`TMPDIR=/tmp make -C src splat` ends by printing the meter: the claimed functions and code bytes, in total and per
component, then how often the target body had to choose among our readings of an operand. The latest meter, grouped
by origin, and the scan's verdicts are in [../status.md](../status.md), with the commands that regenerate them.

How to read the meter:

- A component is the target function's caller attribution (`ledger/func-attribution.tsv`), not its
  library. For example, libc's `pad` (target 788) counts as RHVoice.
- "another reading" and "which copy" count claims where the target body chose among our readings of an
  ambiguous operand: "which copy" when the choice was among copies of the same content or among the table
  slots of one function. "Re-pointing alone" counts claims where no choice was needed.
- Claims of tiny bodies (8 B or less) and of bodies with a byte-identical twin in the target carry an arbitrary
  name, because binaryen merges identical bodies. [../status.md](../status.md) "Reading the meter" counts both,
  from the committed scan and the target.
