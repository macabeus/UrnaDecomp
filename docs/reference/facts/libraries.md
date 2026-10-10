# Facts: the third-party libraries

Which upstream libraries are linked into the target, at which version, with which flags, and how each was
decided. `third_party/MANIFEST.md` is the table of record (versions, sources, revisions); this file holds the
measurements. The probe tools live in `tools/libs/`.

## B1. SQLite 3.50.4: `-O3 -fPIC` plus conan's defaults

```
emcc -O3 -fPIC -DSQLITE_ENABLE_COLUMN_METADATA -DSQLITE_ENABLE_MATH_FUNCTIONS -DSQLITE_ENABLE_RTREE -DSQLITE_ENABLE_UNLOCK_NOTIFY \
     -DHAVE_FDATASYNC -DHAVE_GMTIME_R -DHAVE_LOCALTIME_R -DHAVE_POSIX_FALLOCATE -DHAVE_STRERROR_R -DHAVE_USLEEP -c sqlite3.c
```

This is conan-center `sqlite3/3.50.4` with its default options, `build_type=Release` (CMake Release = -O3) and
`fPIC=True`. `-fvisibility=hidden -DSQLITE_API=…`, `-fexceptions` and `-DNDEBUG` change nothing.
`tools/libs/sqlite/build.sh` builds it (defaults = this line); `tools/libs/sqlite/census.sh <variant>` scores it.

Census of the 1,141 functions the classifier calls sqlite (paired = byte, exact or table tier):

| compile flags | paired / 1141 | instructions paired / 495,052 |
| --- | --- | --- |
| -Oz | 101 | 3,147 |
| -Os | 188 | 9,270 |
| -O2 | 393 | 41,931 |
| -O3 | 473 | 72,013 |
| -O3 -fPIC (newer census version, as are the rows below) | 863 | 374,145 |
| -O3 -fPIC + `keep()` in the probe's main | 902 | 424,721 |
| -O2 -fPIC + `keep()` | 629 | 122,472 |
| -O3 -fPIC -flto + `keep()` | 486 | 71,123 |
| -O3 -fPIC + `keep()`, no `HAVE_*` | 874 | 400,879 |

With `keep()`, 1,348 of the probe's 1,370 SQLite functions are found in the target (485,906 of 489,889
instructions). **-fPIC is the big lever**: it changes LLVM's output for most functions. Since conan sets
`fPIC=True` for every static library, expect `-fPIC` on every conan-built library.

The `HAVE_*` defines, one removed at a time:

- `HAVE_LOCALTIME_R`: without it target 6320 `toLocaltime` stops matching.
- `HAVE_POSIX_FALLOCATE`: without it 12688 `unixFileControl` and 12302 `posix_fallocate` stop matching.
- `HAVE_FDATASYNC`: the target's `unixSync` (12692) calls func 3992, musl's `fdatasync`. The census count does not
  move because `call N` normalises to `call F`; only the callee's identity tells.
- `HAVE_STRERROR_R`: target 6509 `unixLogErrorAtLine` calls `strerror_r(e, aErr, 79)`.
- `HAVE_GMTIME_R`, `HAVE_USLEEP`: dead in this configuration (`sqlite3.o` byte-identical with and without);
  kept because conan sets them.

What an isolated probe cannot reproduce (22 functions, 3,983 instructions) is whole-program binaryen work, not
flags: API functions whose return the app never uses (DAE drops it), libc callees specialised to one app call
site, and duplicate-function groups (one `end` body for `cdateFunc|ctimeFunc|ctimestampFunc`, where which copy
carries which name is undecidable). The probe's `keep()` gives every libc function `sqlite3.c` calls a second
caller with variable arguments and a used return, so DAE does not specialise them the other way.

## B2. libcrypto 3.0.17: objects at -O3, `THIRTY_TWO_BIT` without `BN_LLONG`

Configured in `urna-emsdk:6.0.0` (`tools/libs/openssl/{configure.sh,urna.conf}`), out of tree in
`build/out/openssl/O3`:

```
perl ../../../../third_party/openssl/Configure --config=urna.conf urna-wasm no-shared --release --prefix=/ --libdir=lib \
     --openssldir=/etc/ssl threads no-unit-test no-tests -fPIC no-fips no-md2 -ffile-prefix-map=../../../../third_party/openssl/=
```

This is conan-center `openssl/3.x.x`'s line for `os=Emscripten` (its target map sends `Emscripten-*-*` to
`"cc"`). conan builds in tree, so the target's `__FILE__` strings are relative (`crypto/…`); the path map gives the
out-of-tree build the same strings, and every libcrypto object it compiles is byte-identical to an in-tree build's
but `cversion.o` (its build date and flags text; not linked).

- **-O3.** `crypto/bn/bn_exp.c` recompiled at each level and scored against target 980 (`BN_mod_exp_mont`, 594
  rows): -O0 682/870, -O1 373/680, -O2 117/602, **-O3 35/594**, -Os 259/614, -Oz 381/670. At -O3 the row count
  equals the target's and every remaining row is a callee fingerprint or a data address. conan Release's
  `CFLAGS="-O3 …"` replaces the `cc` target's `-O`.
- **bn_ops.** The `cc` target has no `bn_ops`, so `configuration.h` has `#undef BN_LLONG` and
  `#define THIRTY_TWO_BIT`. The i64 operations in the target's BN code come from `bn_exp.c` itself (13 `i64.`
  instructions without `BN_LLONG`); `bn_div.c`'s function 444 has none, as in the probe.
- **-fPIC** changes the object (`GOT.mem.*` imports, `__memory_base`), but after a static -Oz link the bodies are
  identical: undecidable from the wasm, harmless either way.
- `-ftrivial-auto-var-init=zero` (tested on 8 files with `flag-sweep.sh`) improves nothing: not a TSE flag.

## B3. The target's string table decides which library functions were linked

Every OpenSSL 3 `ERR_raise` expands to `ERR_set_debug(__FILE__, __LINE__, OPENSSL_FUNC)` with
`OPENSSL_FUNC = __func__`, so a function that raises carries its own name as a literal. wasm-ld's `--gc-sections`
drops an unreferenced function together with the literals only it references. So, for a function with an
`ERR_raise`:

- its name is in the target's data → the function (or whatever it was inlined into) is in the target;
- its name is absent → the function was not linked.

The same holds for a member's `__FILE__` path: 190 of libcrypto's 535 ERR-carrying members have no path in the
target (`tools/libs/openssl/negative-symbols.py`), so none of their functions may be a probe root. In an
export-everything probe, 1,173 functions reference their own name; 933 of the names are in the target and 240 are
not (phantoms, `fn-strings.py`, `prune-by-phantoms.py`). Three phantoms live in members the target does have
(`OPENSSL_sk_reserve`, `ASN1_BIT_STRING_set_bit`, `s2i_ASN1_INTEGER`): the member was pulled for other symbols and
wasm-ld dropped those functions.

**Trap: wasm-ld tail-merges literals.** A name that is a suffix of another target string (`update`, `rsa_sign`,
`kdf_derive`) may be present only as that suffix. Only a name that is no suffix of any target string is provably
absent (`tails = {s[i:] for s in strings}`).

The same test works on the app: any function carrying `__func__` or `__PRETTY_FUNCTION__` can be proven linked or
not from `ledger/strings.tsv`.

## B4. libcrypto's census and the app's root set

Recipe (`tools/libs/openssl/`): `configure.sh O3`, `build.sh O3` (910 objects, ~3 min), `link-probe.sh` with
`ROOTS=roots.txt` (258 symbols) and **no extra exports**: `em++ -Oz -fexceptions probe.c <345 member objects>
libcrypto.a`, where `probe.c` is a generated `main()` that calls every root once (`gen-probe-main.py`). Result
(`census.sh O3 r10`): at depth 0, **1,904 of the 2,183 openssl-class target functions pair body for body**
(fingerprint 655, address-blind 1,160, shape 89); 1,665 of the probe's 1,702 libcrypto functions pair. Of the 279
unpaired target functions, 230 carry no string.

How the root set is derived (968 → 1,904 pairs):

1. Exporting all 3,119 member symbols gives 968 pairs; calling 122 derived roots from `main` gives 1,403; dropping
   the exports altogether gives 1,782 at depth 2. An exported function is never inlined ([tool-traps.md §X1](tool-traps.md)).
2. Roots = probe functions with ≤ 1 internal call site that the census pairs to a target function
   (`derive-roots.py`); each pass's pairs name more roots (122 → 258), converging in two passes.
3. Never a root: the 190 members without a `__FILE__` in the target and the 240 phantoms with their call-graph
   ancestors (B3). The archive closure of the 346 named members holds 70 such members, which is expected: wasm-ld
   pulls a member for one symbol and GCs the rest.

What still differs is whole-program work, not flags: dead-argument/constant propagation (`gf_mul` pushes 96 extra
`i64.const 0` before `__multi3` in the target; `CRYPTO_ocb128_init` has 3 parameters in the target and 6 in the
probe) and local renumbering after a differently inlined callee. `tools/libs/openssl/roots.txt` is also the best
estimate of the libcrypto API the app calls.

## B5. 7-Zip is 21.02, the C++ 7-Zip code, not xz

The compressor behind `ecourna::api::compression::CLzmaCompress` is **7-Zip's C++ code** (RTTI
`NCompress::NLzma2::CEncoder`, `NArchive::N7z::CHandler`, `CUpdateCallbackConsole`), not xz/liblzma: there is no
`lzma_*` text, no "XZ Utils", no liblzma error string. Matching against xz would never converge.

The version is **21.02** (alpha, 2021-05-06). Releases come from SourceForge
(`https://sourceforge.net/projects/sevenzip/files/7-Zip/<ver>/7z<XXXX>-src.7z/download`; 21.00, 21.01 and 21.05
do not exist), and `third_party/7zip` is the 21.02 one; `tools/libs/7zip/build.sh <ver> st-O3 -O3` builds a probe of
the release in `third_party/7zip` (Alone7z source list minus Windows-only
and Bench files, `-D_7ZIP_ST`, objects at -O3, link `-Oz -fexceptions`, driver calling `UpdateArchive`,
`CArchiveLink::Open3` and `IInArchive::Extract`). Census over the 954 target functions labelled 7zip
(`tools/libs/census_subset.ts`, depth 0; `tools/libs/7zip/census.json`):

| version | paired | fingerprint | byte-exact |
| --- | --- | --- | --- |
| **21.02** | **451** | 360 | 101 (63,192 B) |
| 21.03 | 446 | 357 | 97 |
| 21.07 | 435 | 347 | 93 |
| 22.00 = 22.01 | 426 | 334 | 90 |

Discriminators (`pnpm wasm score`, depth 0, rows differing / rows):

- target 2603 `LzmaEnc_CodeOneBlock` (13,555 B): 21.02 142/7047, 21.03 and 21.07 2523/7345 (`LzmaEnc.c` and
  `LzFind.c` were rewritten between 21.02 and 21.03);
- 3424 `GetMatchesSpec1` byte-exact with 21.02, 71/231 with 21.03; 3418 and 3422 byte-exact with 21.02 only;
- 4928 `StringToDictSize` 0/133 vs 48/162 (`MethodProps.cpp` changed 21.02 → 21.03);
- ≤ 21.07 from `CFolderInStream::AddFileInfo` and `FileTimeToUnixTime` (renamed in 22.00).

Build facts: 19.00 has no POSIX layer; 21.04 and 21.06 do not compile with `-D_7ZIP_ST`, which the target's
single-threaded code (`CMixerST`, no `CMixerMT`) needs. Objects at -O3 (-Oz 199, -Os 270, -O2 318, -O3 323 on the
bare library), no LTO (297 with `-flto`), `-D_7ZIP_ST` (multi-threaded 417 < 426); `-std=c++20` and `-DUNICODE`
change nothing. The `ip7z/7zip` git mirror starts at 21.07, so a "≥ 21.07" bound read from its tags is no evidence.

## B6. RHVoice 1.14.0, and what the app links of it

The app speaks: `api::CRHVoiceTextToSpeech` wraps RHVoice (296 `RHVoice::*` RTTI names), including the bundled
hts_engine fork (`HTS_*` errors), rapidxml, utf8cpp (`utf8::invalid_utf8`, …) and sonic (`min_sonic_rate`). The
packaged `.data` file has no voice data.

Version **1.14.0**:

- **Strings.** `hts_labeller.cpp` gained feature functions between 1.14.0 and 1.16.4 whose names are literals
  (`syl_pos_type`, `rel_syl_in`, `rel_syl_out`, `syl_lex_tone`, …); `strings -n 6 target/vota_web_wasm.wasm | grep
  -cx <name>` is 0 for each, while the 1.14.0-era names are present. 1.16.4 and 1.16.5 are identical in every
  compiled source; there is no tag between 1.14.0 and 1.16.4.
- **Compile-diff** (`tools/libs/rhvoice/build.sh <tag> cxx14-O3 -O3 -std=c++14`, census over 746 functions): 1.14.0
  264 paired, 1.16.5 245. Target 2211 `yy_destructor` (lemon-generated `userdict_parser.c`): 0/61 rows differ for
  1.14.0, 11/63 for 1.16.5.

What is linked: `src/core/*.cpp` minus the data files they `#include`, `src/hts_engine/HTS_*.c`, sonic at
`ba331411` (the `external/libs/sonic` submodule commit 1.14.0 records), all compiled in `third_party/RHVoice`, with
the two generated `config.h` (`CONFIG_PATH=/etc/RHVoice`, `DATA_PATH=/share/RHVoice`, `ENABLE_SONIC=1`,
`ENABLE_PKG=0`). The C API `src/lib/lib.cpp` is not linked (none of its 12 error literals is in the target): the
app drives the C++ core. `voice.cpp` needs Boost headers.

Build facts: RHVoice uses `std::unary_function`/`std::not1`, so with the image's libc++ it compiles at
`-std=c++14` (or C++17 with `-D_LIBCPP_ENABLE_CXX17_REMOVED_UNARY_BINARY_FUNCTION`), not at C++20 although CMake
asks for it: TSE compiled RHVoice at ≤ C++17. Objects at -O3.

## B7. Boost 1.89.0 (medium confidence), header-only

Boost is used header-only: regex, date_time (gregorian `bad_year`/`bad_month`/`bad_day_of_month`,
`posix_time::ptime`), signals2, function, lexical_cast, optional, smart_ptr.

- **Lower bound.** RTTI `boost::re_detail_600::parser_buf<…>`; `BOOST_RE_VERSION` is 500 for 1.76.0–1.88.0 and
  600 from 1.89.0. The regex texts "Invalid regex for POSIX-style matching" and "Expression complexity exceeded."
  also appear only from 1.89.
- **Regex cannot decide further**: boostorg/regex's `include/` is identical from 1.89.0 to 1.91.0.
- **The app's signal can.** Its type is `signals2::signal<void (unsigned long, unsigned long, std::string
  const&)>`, and the target's vtables name its smart_ptr control blocks, which were rewritten in 1.90.
  `tools/libs/vtable_compare.ts` scores each slot (rows differing):

  | target function | 1.89.0 | 1.90.0 = 1.91.0 |
  | --- | --- | --- |
  | 9555/9554/9553 `sp_counted_impl_pd<signal_impl…>::dispose` | 29, 29, 17 | 67, 68, 61 |
  | 9549 `sp_counted_impl_p<grouped_list…>::dispose` | 14 | 46 |
  | 9519 `sp_counted_impl_p<signal_impl…>::dispose` | 17 | 61 |

  1.89.0 is nearest in all five bodies, but the residual rows are inlined context: a ranking, not byte identity.
  It also fits the SQLite snapshot's date (1.89.0 was current in August 2025).

`third_party/boost` holds the 1.89.0 release's headers (`tools/setup/bootstrap.sh`); a probe of 1.90.0 or 1.91.0 puts
that release's `boost/` from `https://archives.boost.io/release/<v>/source/` there (`tools/libs/boost/build.sh`).
`tools/libs/boost/probe.cpp` instantiates what the app does: `basic_regex<char>` search/match/replace/iterator, the
signal, date_time, lexical_cast, function, optional.

## B8. zlib 1.3.1, libc++ `std::format`, and what is absent

- **zlib 1.3.1.** The version literal has no standalone copy: it is the tail of the OID string
  `"DH:dhKeyAgreement:1.2.840.113549.1.3.1"` at memory 355903, referenced once by `i32.load8_u; i32.const 49;
  i32.ne` = the inlined `version[0] != '1'` check of `inflateInit2_`/`deflateInit2_`. No other candidate's tail
  (1.2.11–1.3.0.1) is referenced.
- **`std::format` is libc++'s**, not fmt: the error texts are libc++'s ("The format string contains an invalid
  escape sequence"); fmt's are absent.
- **Absent** (no strings, no RTTI): nlohmann/json, rapidjson, libpng, libjpeg, freetype, pcre, libsodium, xz.
  Drawing and audio go through the `js_*` imports. "WSQ" strings are the app's own fingerprint-file code
  (`comum::CGravadorWSQ`).

## B9. Pinning a library version by compile-diff

The recipe used for 7-Zip, RHVoice and Boost:

1. **Probe** = the library's sources at one release (the tree in `third_party/<lib>`: a checkout of the tag, or for
   7-Zip the extracted release; `build.sh <ver>` refuses another), compiled in `urna-emsdk:6.0.0` with the library's own defines,
   objects at -O3, linked `em++ -Oz -fexceptions --no-entry -sERROR_ON_UNDEFINED_SYMBOLS=0 -sALLOW_MEMORY_GROWTH=1
   -sSTACK_SIZE=8MB --emit-symbol-map -o probe.js`, plus a `driver.cpp` whose `EMSCRIPTEN_KEEPALIVE` functions call
   what the app calls. The symbol map names every probe function; it changes only the function order of an -Oz
   link, never a body.
2. **Census slice**: `tools/libs/census_subset.ts <target> probe.wasm tools/libs/cls2.json <label> --depth 0`.
   Depth 0 (the body alone) is right for version work; depth 2 halves the pairs because callees differ.
3. **Discriminate**: `tools/libs/discriminate.py <label> <tag>=<census.json>:<symbols>…` lists target functions
   paired by some tags only; confirm each with `pnpm wasm score`. A function that differs with no source change
   between the tags is an inlining artefact of the probe's root set, not a version fact.
4. **Vtable slots**: `tools/libs/vtable_compare.ts` scores the target's RTTI-named vtable slots against the same
   class in each probe, reaching functions the census cannot name.
5. `tools/libs/census_summary.py` folds the census JSONs into `tools/libs/<lib>/census.json`.

On all three libraries objects at -O3 beat -O2 > -Os > -Oz, `-flto` loses, and `-std=c++20` changes nothing
measurable. The paired fraction stays low (7-Zip 19 % of bytes, RHVoice 4 %, Boost 4 %) because the probe's
inlining context is not the app's: enough to rank releases, not a byte reproduction.
