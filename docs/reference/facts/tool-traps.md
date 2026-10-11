# Facts: traps of probe links and of the shell

Commands and links that give a plausible wrong answer, collected where more than one topic hits them. Topic-specific
traps stay with their topic: [toolchain.md §T7](toolchain.md) (comparing bodies), [link-order.md §L7](link-order.md),
[app-flags.md §F9](app-flags.md), [asn1.md §A4](asn1.md), [splat.md §S3–S4](splat.md).

## X1. binaryen never inlines an exported function, so exporting library code to keep it alive changes its bodies

binaryen's Inlining pass inlines a function of **any** size when it has exactly one call site and is not used
globally (exported, or in the table). With `-sEXPORTED_FUNCTIONS` covering a library's symbols that rule never
fires. Measured: target func 444 (`BN_div`) is 866 rows = `BN_div` with its only callee `bn_div_fixed_top` inlined;
in a probe that exports `bn_div_fixed_top`, `BN_div` stays 65 rows at every -O level (817/866 rows differ). Target
981 is `BN_mod_exp` with `BN_mod_exp_mont_word` and `BN_mod_exp_recp` inlined (their ERR strings share the
function); they are separate functions in an exporting probe.

So a byte-for-byte reproduction needs the app's real root set: each function the app calls, called for real, so
that internal one-caller functions keep one caller. Not `-sEXPORTED_FUNCTIONS=<everything>`, not
`-Wl,--export-all`, not a table of function pointers (table slots are "used globally" too). Functions matched by
fingerprint only need their own body and callees to agree, so census counts under an exporting probe are a lower
bound. The same holds for the app's own roots TUs (`src/app/roots.cpp`, `src/units/*-roots.cpp`): an uncalled
root keeps the target's bodies, a called one behaves as in the app.

## X2. Probe links

1. `em++ … -o probe.wasm` (no `.js`) silently selects `STANDALONE_WASM`: an `_initialize` export, WASI-flavoured
   libc paths, and a passed `-sEXPORT_ALL` is dropped. Link to `-o probe.js` like the target and read the
   `probe.wasm` next to it.
2. `-sEXPORT_ALL=1` does not keep library functions alive (11 function exports with and without it). The
   1,230–1,850 functions that survive a **bare link** of a C++ library are those reachable from vtables (the elem
   table), so a bare probe covers only virtual-reachable code (RHVoice: 101 functions). Give the probe a driver
   whose `EMSCRIPTEN_KEEPALIVE` functions call the app's entry points into the library; that also keeps one-caller
   inlining behaving as in the app (X1).
3. `score`'s `byteExact` can be true while the score is > 0: rows symbolise `i32.const` data addresses as
   `data:<segment hash>+rel`, and segment hashes differ between modules even when the function bytes are identical.
   `byteExact` is the stronger statement.
4. A SourceForge 7-Zip `.7z` source has CRLF line endings: diff it against the LF git mirror with
   `--strip-trailing-cr`, or every file "changes".

## X3. zsh, the default shell here

1. zsh does not word-split an unquoted `$var`: `set -- $var`, `$C arg` or `em++ $FLAGS` pass one argument (errors
   like `invalid integral value '2 -fPIC …'` or `no such file or directory: pnpm --filter …`). Use an array
   (`F=(-O2 -fPIC); em++ $F`) or `${=var}`.
2. Building a list as a string (`A="$A x=y"`) and passing `$A` gives an argument with a leading space
   (`ENOENT ' /…/probe.wasm'`): use `A=(); A+=("x=y"); "${A[@]}"`.
3. A word starting with `=` is `=cmd` expansion: `echo ====` fails with `=== not found`. Quote it.
4. In `until for h in …; do grep -q … || exit 1; done; do sleep 5; done` the `exit` leaves the whole script at
   the first miss; it is not a loop break.

## X4. Fetching from the TSE site

A plain `curl` to `https://www.justicaeleitoral.jus.br/simulador-votacao/…` returns HTTP 403 "Access Denied" (an
Akamai edge rule) for every path, including ones that exist. A 403 there does not mean the file is absent. The
target files were fetched through a browser.

## X5. Callers of `std::to_string` pair at `none` at depth 2, and the readout reads error codes as strings

1. A function that calls `std::to_string(int)` or `std::to_string(unsigned)` (targets 296 and 327, 128 callers
   between them) pairs at tier `none` at depth 2 in the skeleton however exact its own body is. `to_string` calls
   `string::__init<const char*>` (target 1544, 16 B), which in the target wraps an out-of-line
   `__init_with_size` (5607, called by 1544 and 679). While 679's owner is not built, the skeleton has one caller
   and wasm-opt inlines it, so the depth-2 fingerprints differ. `to_string` itself is identical at depth 0
   (`pnpm wasm score <target> 327 <a named link> <to_string(unsigned)> --depth 0` gives 0 differing rows of 27). Shown
   on 1931 (`CRdvVota::Candidato`) in a whole link: depth 0 `addr` with 3 differing data addresses, depth 1 `shape`
   with 8 operand-only rows, depth 2 `none`. This is why the symbol file names `to_string(unsigned)` by its depth-0
   pair (`evidence:pair-depth0`).
2. `fn`'s "strings referenced" reads an error-code immediate in the data range as a string pointer: 1931 shows
   `4725 (+13 into 4712) "Tipo de identificador inválido: {}"`, but 4725 is an `EUeRdvError` code (the second
   argument of the `CBaseError` constructor, through 3707), and its siblings 3749, 2814, 2813 and 5732 list 4726 to
   4729 the same way. The `addr` tier does not see it either: the row is `i32.const data:<seg>+3701` on both sides.

## X6. `try-tu.sh --single` and exceptions

The rank template of `--single` compiles and links in one `em++ -Oz -fexceptions …` (the build's link has
`-fexceptions` too). A uenux2 TU is compiled without it (app-flags.md §F6), so `--single` adds `-fignore-exceptions`
to a TU whose own flags lack `-fexceptions`. Without it a uenux2 TU scored alone grows landing pads. With the
reconstructed `api::CDirReader::NextEntry`:

```sh
TMPDIR=/tmp tools/round/try-tu.sh --single 1260 --extract export:_ZN3api10CDirReader9NextEntryEv \
  src/uenux2/src/api/util/cdirreader.cpp
# flags: -O2 -fPIC -std=c++20 -DNDEBUG -fno-strict-aliasing … -I/work/src/asn1  -fignore-exceptions
# score 224 over 250 rows (same 26, replaced 13 [6 operand-only], inserted 211, deleted 0)
```

No `invoke_*` row is left (without `-fignore-exceptions` the same TU scores 162 over 184 rows, with `invoke_iii`,
`invoke_iiiii`, `invoke_viii` and `__resumeException` calls). The rest of the difference is what binaryen inlines
into a body linked alone; the splat build matches this source.

## X7. A TSE file name the path map does not rewrite

`-ffile-prefix-map` rewrites a path as clang spells it. A source compiled from a relative `src/uenux2/…` path, or a
header found through `-Isrc` (`src/ecourna/api/asn/iconversorasn.hpp`), is spelled relatively, so a map of
`/work/src/…` alone leaves `__FILE__` and the `std::source_location` records at `src/…` instead of the target's
`/home/rubio/tse/uenux2/…` or conan path. Compiled from relative paths with only `/work/src/…` mapped, three
objects of unit 01 (`cinikey`, `cposixmutex`, `cposixsemaphore`) hold 10 such records, and eleven ecourna objects
35 more from those two headers. So every uenux2 and ecourna compile runs from `/work/src/…`, `UENUX2_PREFIX_MAP` and
`ECOURNA_PREFIX_MAP` map both spellings, and `check-srcloc` fails with `UNMAPPED` on a record named `src/…`:

```sh
TMPDIR=/tmp tools/round/try-tu.sh --print-flags src/uenux2/src/app/vota/eleitor/cvotacaostateaudio.cpp
# … -ffile-prefix-map=/work/src/uenux2=/home/rubio/tse/uenux2 -ffile-prefix-map=src/uenux2=/home/rubio/tse/uenux2
#   -Isrc -Isrc/uenux2/src -Isrc/asn1  -c /work/src/uenux2/src/app/vota/eleitor/cvotacaostateaudio.cpp
TMPDIR=/tmp make -C src check-srcloc
# prints, for example, 280 source_location records in 184 objects (69 files): 259 are records of the target;
#   0 that matched functions load are not; 21 that only unmatched functions load are not; 0 name an unmapped file
```

A function that loads a record with the wrong file name does not match: most of the functions that need the mapped
records are `ecourna::api::asn::IConversorASN<…>::Converte` and `Deconverte` instantiations. A uenux2 TU that
includes an ecourna header needs a third spelling: the target names those `…/libecea1da310e5107/p/include/ecourna/…`,
the conan package's installed headers (13 records in `ledger/source-locations.tsv`). No uenux2 TU in `src/uenux2`
holds such a record, so no map has that spelling.
