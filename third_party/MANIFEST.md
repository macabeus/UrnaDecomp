# third_party MANIFEST — libraries linked into target/vota_web_wasm.wasm

Target: `target/vota_web_wasm.wasm` (sha256 8579ef8d…07e2, 14343 functions). Evidence commands were run
from a scratch dir holding `strings4.txt` (= `strings -n 4 target/vota_web_wasm.wasm`; `strings6.txt` is the same
with `-n 6`) and `mem.bin`
(linear-memory image rebuilt from the 4224 data segments by `tools/libs/memimg.py`). All numbers below
carry the command that produced them. Tooling and raw outputs: `tools/libs/`.

The string pool is tail-merged and sorted (wasm-ld + wasm-opt): a short string that is the suffix of a
longer one has NO standalone copy, so `strings` may miss a version string (zlib's "1.3.1" lives inside
the OID "…113549.1.3.1"). Pointer references (`i32.const <addr>` in the dump) are the ground truth.

Every source below is fetched by `tools/setup/bootstrap.sh`, which checks the commit or sha256 in the last column.
The library builds (`tools/libs/*/build.sh`, `src/Makefile`) compile the trees where they are and write their objects
under `build/out/`. OpenSSL is configured out of tree: its build directory `build/out/openssl/O3` also holds the headers
it generates (`tools/libs/openssl/configure.sh`).

| dir | library | version compiled | fetched as | commit / sha256 |
| --- | --- | --- | --- | --- |
| `openssl/` | OpenSSL (libcrypto only) | 3.0.17 | `git clone --depth 1 --branch openssl-3.0.17 https://github.com/openssl/openssl` | 4d7dfa23 |
| `sqlite/` | SQLite | 3.50.4 | the amalgamation https://sqlite.org/2025/sqlite-amalgamation-3500400.zip, its one directory extracted: `sqlite3.c` (the file the build compiles), `sqlite3.h`, `sqlite3ext.h`, `shell.c` | zip sha256 1d3049dd0f830a025a53105fc79fd2ab9431aea99e137809d064d8ee8356b032 |
| `zlib/` | zlib | 1.3.1 | full clone https://github.com/madler/zlib, checked out `v1.3.1` | 51b7f2a |
| `7zip/` | 7-Zip / LZMA SDK (C++ 7z archive + LZMA/LZMA2 codecs + console update callbacks) | **21.02** (alpha, 2021-05-06; compile-diff, see below) | the source release `7z2102-src.7z` (https://sourceforge.net/projects/sevenzip/files/7-Zip/21.02/7z2102-src.7z/download) extracted whole: `Asm/ C/ CPP/ DOC/`, 1236 files, no git repository | archive sha256 ee9755feaa034e7075712bc6b1fad25440bd0a8f21f106a16e37bc4b409f1122 |
| `RHVoice/` | RHVoice TTS (+ bundled hts_engine fork, rapidxml, sonic, utf8cpp) | **1.14.0** (strings + compile-diff, see below) | full clone https://github.com/RHVoice/RHVoice checked out at tag `1.14.0`, with the one submodule the build compiles, `external/libs/sonic` (waywardgeek/sonic), at the commit 1.14.0 records | d1d5fc6c; sonic ba331411 |
| `boost/` | Boost (header-only use: regex, date_time, signals2, function, lexical_cast, optional, smart_ptr) | **1.89.0** (compile-diff of the signals2 control blocks, medium confidence; see below) | the release https://archives.boost.io/release/1.89.0/source/boost_1_89_0.tar.bz2, of which its `boost/` header tree and `LICENSE_1_0.txt` are extracted (15,965 files) | tarball sha256 85a33fa22621b4f314f8e85e1a5e2a9363d22e4f4992925d4bb3bc631b5a0c7a |
| `III-ASN.1/` | III ASN.1 runtime (namespace `ASN1`: BER/PER/AVN), TSE fork | upstream last commit 66e5d48 (2016) — TSE's copy is MODIFIED | full clone https://github.com/Kampbell/III-ASN.1 | 66e5d48 |
| — | libc++ / libc++abi / musl / compiler-rt / dlmalloc, `std::format` | ship with the emsdk (`docs/reference/toolchain.md`) | in the image's sysroot, not fetched | — |
| — | xz / liblzma | NOT LINKED (`docs/reference/facts/libraries.md` §B5) | — | — |
| — | fmt, nlohmann/json, rapidjson, libpng, libjpeg, freetype, pcre, libsodium | NOT LINKED (no strings, no RTTI) | — | — |

---------------------------------------------------------------------------------------------------

## SQLite 3.50.4 — confidence: certain (version), certain (option list)

Evidence
- `grep -nE '^20[0-9]{2}-' strings6.txt` → `2025-07-30 19:33:53 4d8adfb30e03f9cf27f800a2c1ba3c48fb4ca1b08b0f5ed59a4d5ecbf45e20a3` = SQLITE_SOURCE_ID of 3.50.4.
- Version string `3.50.4` is at mem 0x1694aa (1479978), `grep -c 'i32.const 1479978$' build/dump/vota.objdump.txt` → 2 refs; `strings` shows it as `-3.50.4` because the byte before is DER 0x2d.
- Compile-option table: `sqlite3azCompileOpt[]` recovered from memory (`python3 -I tools/libs/ptrarray.py 'ATOMIC_INTRINSICS=1'`, array at 0x1ba240, 41 entries):
  ATOMIC_INTRINSICS=1 COMPILER=clang-23.0.0 DEFAULT_AUTOVACUUM DEFAULT_CACHE_SIZE=-2000 DEFAULT_FILE_FORMAT=4
  DEFAULT_JOURNAL_SIZE_LIMIT=-1 DEFAULT_MMAP_SIZE=0 DEFAULT_PAGE_SIZE=4096 DEFAULT_PCACHE_INITSZ=20
  DEFAULT_RECURSIVE_TRIGGERS DEFAULT_SECTOR_SIZE=4096 DEFAULT_SYNCHRONOUS=2 DEFAULT_WAL_AUTOCHECKPOINT=1000
  DEFAULT_WAL_SYNCHRONOUS=2 DEFAULT_WORKER_THREADS=0 DIRECT_OVERFLOW_READ ENABLE_COLUMN_METADATA
  ENABLE_MATH_FUNCTIONS ENABLE_RTREE ENABLE_UNLOCK_NOTIFY MALLOC_SOFT_LIMIT=1024 MAX_ATTACHED=10 MAX_COLUMN=2000
  MAX_COMPOUND_SELECT=500 MAX_DEFAULT_PAGE_SIZE=8192 MAX_EXPR_DEPTH=1000 MAX_FUNCTION_ARG=1000 MAX_LENGTH=1000000000
  MAX_LIKE_PATTERN_LENGTH=50000 MAX_MMAP_SIZE=0 MAX_PAGE_COUNT=0xfffffffe MAX_PAGE_SIZE=65536 MAX_SQL_LENGTH=1000000000
  MAX_TRIGGER_DEPTH=1000 MAX_VARIABLE_NUMBER=32766 MAX_VDBE_OP=250000000 MAX_WORKER_THREADS=8 MUTEX_PTHREADS
  SYSTEM_MALLOC TEMP_STORE=1 THREADSAFE=1
- Reproduced EXACTLY (41/41, no diff) with
  `docker run --rm -v third_party/sqlite:/src:ro -v tools/libs:/t urna-emsdk:6.0.0 bash -c 'emcc -O2 -I/src /t/opts.c /src/sqlite3.c -DSQLITE_ENABLE_COLUMN_METADATA -DSQLITE_ENABLE_MATH_FUNCTIONS -DSQLITE_ENABLE_RTREE -DSQLITE_ENABLE_UNLOCK_NOTIFY -o /t/opts_em.js && node /t/opts_em.js'`
  (emcc 6.0.0 / clang 23.0.0). Everything else in the list is a default (MAX_MMAP_SIZE=0 because emscripten
  defines none of __linux__/_WIN32/__APPLE__; a host clang build differs only there and in COMPILER=).

Build inference
- Required -D: `SQLITE_ENABLE_COLUMN_METADATA SQLITE_ENABLE_MATH_FUNCTIONS SQLITE_ENABLE_RTREE SQLITE_ENABLE_UNLOCK_NOTIFY`
  (+ `SQLITE_THREADSAFE=1` is harmless, it is the default). No OMIT_*, no FTS, no JSON1 define (JSON is built in since 3.38), no DBSTAT, no SOUNDEX, no USE_URI.
- This is exactly conan-center `sqlite3` recipe's default option set (enable_column_metadata, enable_rtree,
  enable_math_functions, enable_unlock_notify = True, all else False) → TSE most likely consumed `sqlite3/3.50.4`
  from conan-center. That recipe also adds (not visible in the ctime list) `HAVE_FDATASYNC HAVE_GMTIME_R HAVE_LOCALTIME_R
  HAVE_POSIX_FALLOCATE HAVE_STRERROR_R HAVE_USLEEP` and `SQLITE_API=__attribute__((visibility("default")))`,
  compiles with `C_VISIBILITY_PRESET hidden`. Consistent with the imports `fdatasync`, `fallocate` in the wasm
  (`docs/reference/facts/toolchain.md` §T1). The builder must test these HAVE_* (they change os_unix.c codegen).
- Not OMIT_LOAD_EXTENSION → sqlite3_load_extension and its dlopen path are compiled in.

## OpenSSL 3.0.17 (libcrypto only) — confidence: high (version), medium (configure line)

Evidence
- `grep -n '3.0.17' strings4.txt` → line 38908 `3.0.17` (standalone string; OPENSSL_VERSION_STR is exported by the
  default provider's `OSSL_PROV_PARAM_VERSION`). No `OpenSSL 3.0.17 1 Jul 2025` banner (cversion.c not linked).
- 349 source paths `crypto/…`, `providers/…`, `ssl/record/tls_pad.c` (= libcrypto member) present via ERR_raise
  __FILE__ (`grep -oE '^(crypto|providers|ssl)/[A-Za-z0-9_/.-]+\.(c|h|inc)' strings4.txt | sort -u | wc -l` → 349;
  list in tools/libs/ossl_files.txt). ZERO libssl paths; `SSL routines` is only the ERR library-name table → libssl NOT linked.
- Provider strings present: `OpenSSL Default Provider`, `OpenSSL Base Provider`, `OpenSSL Null Provider`,
  `OpenSSL Legacy Provider` → the legacy provider is built INTO libcrypto (STATIC_LEGACY, i.e. `module` disabled).
- Compiled implementations include legacy ciphers/digests (providers/implementations/ciphers/cipher_{blowfish,cast5,
  idea,rc2,rc4,rc4_hmac_md5,seed,des}.c, digests/{md4,mdc2,wp,ripemd}_prov.c) and aria, camellia, sm4, sm2/sm3,
  chacha20(-poly1305), blake2, siphash, poly1305, scrypt, ocb, siv, cts, xts, wrap, ec2m (bn_gf2m.c, ec2_smpl.c),
  engines (crypto/engine/*.c: 17 files incl. eng_dyn.c), threads_pthread.c, mem_sec.c, ui_openssl.c, conf_mod.c.
- Absent: MD2 and RC5 (OpenSSL defaults no-md2 no-rc5), `OpenSSL 'dlfcn' shared library method` (only
  `NULL shared library method` is present; `grep -c 'filename(%s)' strings4.txt` → 0) → DSO disabled / no dso_scheme.
- BN word size: functions referencing crypto/bn/*.c paths use no i64 arithmetic except BN_LLONG-style widening muls
  (e.g. func 981 bn_exp.c: 22 i64 ops, 2 i64.mul; func 444 bn_div.c: 0 i64) → THIRTY_TWO_BIT + BN_LLONG, i.e. a
  32-bit generic target, NOT SIXTY_FOUR_BIT.
- Install dirs: strings `//lib/ossl-modules` (MODULESDIR with `--prefix=/ --libdir=lib`) and `/etc/ssl` (OPENSSLDIR).
  `ENGINESDIR`/`engines-3` not referenced.

Build inference (matches conan-center `openssl/3.x.x` recipe for os=Emscripten: target `cc`):
  `./Configure cc no-shared --release --prefix=/ --libdir=lib --openssldir=/etc/ssl threads no-unit-test no-tests -fPIC no-fips no-md2`
  Target `cc` has thread_scheme "(unknown)" (threads still on because conan passes CFLAGS → pthread scheme compiled:
  `crypto/threads_pthread.c` string present) and no dso_scheme (→ dso/module/dynamic-engine disabled → legacy
  built in, no dlfcn). conan default `no_zlib=True`. NOTE `/etc/ssl` is conan's Linux default, Emscripten would give
  `<package>/res`; so either `-o openssl/*:openssldir=/etc/ssl` was used or OpenSSL was configured by hand — same flags.
  Only libcrypto is linked (static archive; members pulled by reference).

## zlib 1.3.1 — confidence: high

- z_errmsg table present (`need dictionary`, `stream end`, `file error`, `stream error`, `data error`,
  `insufficient memory`, `buffer error`, `incompatible version` all in strings4.txt).
- No standalone `1.3.1`: `python3 -I tools/libs/tails.py 1.3.1` → tail of `DH:dhKeyAgreement:1.2.840.113549.1.3.1`
  at mem 355903; `grep -c 'i32.const 355903$' build/dump/vota.objdump.txt` → 1, and the code there is
  `i32.load8_u; i32.const 49; i32.ne` = `version[0] != '1'`, the `inflateInit2_/deflateInit2_` ZLIB_VERSION check
  (inlined into ecourna's czip). Candidates 1.2.11/1.2.12/1.2.13/1.3/1.3.0.1 have no referenced tail → 1.3.1
  (or a 1.3.1.x dev snapshot, unlikely). conan-center zlib default options: shared=False.

## 7-Zip / LZMA SDK 21.02 — confidence: high (family), high (exact, by compile-diff)

- RTTI: `NArchive::N7z::{CHandler,COutHandler,CFolderInStream,…}`, `NArchive::NParser::CHandler`,
  `NCompress::NLzma::{CEncoder,CDecoder}`, `NCompress::NLzma2::{CEncoder,CDecoder}`, `NCompress::NBcj::CCoder`,
  `NCompress::CCopyCoder`, `NCoderMixer2::{CMixer,CMixerST}`, `NWindows::NFile::NIO::*`, `CUpdateCallbackConsole`,
  `COpenCallbackConsole`, `CArchiveUpdateCallback`, `CFilterCoder` … (`grep -E '^(NCompress|NArchive|…)' tools/libs/rtti.tsv`).
  This is the C++ 7z writer/reader of the LZMA SDK "Alone7z/7zr" subset (UI/Common/Update.cpp + UI/Console callbacks),
  not xz/liblzma. The app wrapper is `ecourna::api::compression::CLzmaCompress` (clzmacompress.cpp).
- Version bounds (`python3 -I tools/libs/finediff.py <clone of https://github.com/ip7z/7zip> C,CPP 21.07,22.00,22.01,23.01,24.05`):
  `There are unclosed input file:` (singular) is in ≤ 23.01, plural from 24.05 → v ≤ 23.01.
  7zHandler.cpp 23.01 adds the literal `"ARM64"` (kpidMethod name) which is ABSENT from mem.bin (`BCJ`, `BCJ2`,
  `LZMA`, `LZMA2`, `PPMD`, `Delta`, `7zAES` are present) → v ≤ 22.01. All RTTI class names exist since 21.07.
  No string/RTTI discriminates 21.07 / 22.00 / 22.01 → decide by compile-diff of LzmaEnc.c / 7zOut.cpp.
- No conan recipe fits (conan `lzma_sdk` only ships 9.20) → TSE used the 7-Zip source directly (the console callbacks
  are not in the LZMA SDK).
- **Compile-diff (`docs/reference/facts/libraries.md` §B5, method §B9; `tools/libs/7zip/build.sh <ver> st-O3 -O3` over each
  release's source + `tools/libs/census_subset.ts`, results in `tools/libs/7zip/census.json`)**: 21.07 is only the first
  tag of the ip7z/7zip git mirror, so the string bounds above say nothing below it. Probes of 21.02,
  21.03, 21.07, 22.00, 22.01 against the 954 target functions labelled 7zip (depth-0 census): 21.02 **451** matched /
  101 byte-exact / 63,192 B, 21.03 446 / 97, 21.07 435 / 93, 22.00 = 22.01 426 / 90. Version-specific bodies, scored
  (differing rows / rows): `LzmaEnc_CodeOneBlock` (target 2603, 13.5 KB) 21.02 142/7047 vs 21.03+ 2523/7345 (LzmaEnc.c and
  LzFind.c were rewritten between 21.02 and 21.03: 318 and 994 changed lines); `GetMatchesSpec1`, `LzmaEnc_WriteProperties`,
  `FillDistancesPrices` byte-exact only with 21.02; `StringToDictSize` 0 vs 48/162; `COutFileStream::Seek` (8 rows in
  ≤ 21.03, 48 in 21.07), `COpenOptions::COpenOptions`, `CMultiMethodProps::Init`, `MakeCheckOrder` 0 vs 21.07 3–36;
  `CFolderInStream::AddFileInfo`, `CUInt64DefVector::SetItem`, `FileTimeToUnixTime` 0 vs 22.01 148–220.
  19.00 does not build outside Windows (no myWindows layer); 21.04 and 21.06 do not compile with `-D_7ZIP_ST`
  (`7zHandlerOut.cpp`: `methodFull.NumThreads = methodMode.NumThreads`, a member that exists only `#ifndef _7ZIP_ST`), so
  TSE's single-threaded build (CMixerST, no CMixerMT) could not have used them unpatched; 21.00/21.01/21.05 sources are
  not published on SourceForge. Compile flags from the same sweep: objects at **-O3** (22.01: -Oz 199, -Os 270, -O2 318,
  -O3 323 matched), no LTO (297), `-D_7ZIP_ST` (mt 417 < st 426); `-std=c++20` and `-DUNICODE` change nothing measurable.

## RHVoice 1.14.0 — confidence: high (library), high (exact: strings + compile-diff)

- RTTI: 296 `RHVoice::*` names (`grep -c '^RHVoice' rtti_dem.txt`), `RHVoice::str_hts_engine_impl`, `hts_engine_call`;
  `HTS_*` error strings of the bundled hts_engine fork; `rapidxml::parse_error`; `utf8::{invalid_utf8,not_enough_room,
  invalid_code_point}` (utfcpp); config keys `min_sonic_rate` (sonic), `RHVOICE_CONFIG_PATH`, `/etc/RHVoice`.
  The app class is `api::CRHVoiceTextToSpeech`. Voice data is not in vota_web_wasm.data (`strings -n 5 …data | grep RHVoice` → nothing).
- Bounds (`finediff.py third_party/RHVoice src 1.14.0,1.16.4,1.16.5,1.18.1,1.18.4`): 380 own literals match for
  1.14.0–1.16.5 vs 379 for 1.18.x; the literal `foreign` (language.cpp `word.has_feature("foreign")`) exists only
  ≤ 1.16.5. `str_hts_engine_impl` absent in 1.8.0 → ≥ 1.14.0. The strings and the compile-diff below pick 1.14.0
  within those bounds.
- Not in conan-center → TSE built it themselves (CMake; only `RHVoice_core` + hts_engine + third-party are needed; no tclap/cldr).
- **1.16.x excluded by strings (`docs/reference/facts/libraries.md` §B6)**: the HTS feature names added between 1.14.0 and 1.16.4 in `hts_labeller.cpp`
  (`syl_pos_type`, `rel_syl_in`, `rel_syl_out`, `syl_lex_tone`, `prev_syl_lex_tone`, `prev_prev_syl_lex_tone`,
  `next_next_syl_lex_tone`) are literals in 1.16.5 and absent from the target (`strings -n 6 … | grep -cx <name>` → 0 for
  each). 1.16.4 and 1.16.5 are identical in every compiled source (`git diff 1.16.4 1.16.5 -- src/core src/hts_engine
  src/include src/third-party` is empty); there is no release tag between 1.14.0 and 1.16.4.
- **Compile-diff** (`tools/libs/rhvoice/build.sh <tag> cxx14-O3 -O3 -std=c++14`, driver = engine::create + document
  synthesis, results in `tools/libs/rhvoice/census.json`): of the 746 functions labelled rhvoice, 1.14.0 matches 264
  (117 fingerprint), 1.16.5 245 (111). The lemon-generated user-dictionary parser (`yy_destructor`, target 2211: 1.14.0
  0/61 differing rows, 1.16.5 11/63) and `userdict::word_break::apply` (0/20 vs 1/20) carry the 1.14.0 bodies.
- Build facts: RHVoice uses `std::unary_function`/`std::not1` → with the pinned libc++ it compiles at `-std=c++14`
  (or c++17 + `-D_LIBCPP_ENABLE_CXX17_REMOVED_UNARY_BINARY_FUNCTION`), **not** at C++20 (the CMake default) → TSE
  compiled it at ≤ C++17. Objects at -O3 (1.16.5: -Oz 110, -O2 240, -O3 245 matched). `voice.cpp` needs Boost headers.
  `CONFIG_PATH=/etc/RHVoice`, `DATA_PATH=/share/RHVoice`, `ENABLE_SONIC=1`, `ENABLE_PKG=0`; the C API (`src/lib/lib.cpp`)
  is not linked (none of its literals is in the target) — the app drives the C++ core directly.

## Boost 1.89.0 — confidence: high (bound), medium (exact)

- RTTI `boost::re_detail_600::parser_buf<…>`: `BOOST_REGEX_DETAIL_NS = re_detail_<BOOST_RE_VERSION>`; `git grep
  'define BOOST_RE_VERSION' <tag> -- include` over the tags of a clone of https://github.com/boostorg/regex → 500 for
  1.76–1.88, 600 from boost-1.89.0.
  Regex error texts `Invalid regex for POSIX-style matching`, `Expression complexity exceeded.` also appear only ≥ 1.89.
- Used (header-only) parts: regex, date_time (gregorian/posix_time: `boost::gregorian::bad_{year,month,day_of_month}`),
  signals2, function (`call to empty boost::function`), lexical_cast (`bad lexical cast: …`), optional, smart_ptr.
- 1.89.0 (2025-08), 1.90.0 (2025-12), 1.91.0 (2026-04) all in conan-center; nothing in the binary separates them → compile-diff.
- **Compile-diff (`docs/reference/facts/libraries.md` §B7, method §B9; `tools/libs/boost/build.sh <v> O3 -O3` on `tools/libs/boost/probe.cpp`
  over each release's headers, results in `tools/libs/boost/census.json`)**: `boost/regex` headers are byte-identical
  across 1.89.0–1.91.0 (`git diff --stat boost-1.89.0 boost-1.91.0 -- include` in a clone of boostorg/regex is empty),
  so the regex-dominated slice (166 functions; 41/40/40 matched) cannot tell them apart. What changed is `smart_ptr` (1.90: sp_counted_base/sp_counted_impl/shared_count
  rewritten) and `signals2`/`optional`/`variant` (1.91). The app's signal is `signals2::signal<void (unsigned long,
  unsigned long, std::string const&)>`; the target's vtable slot functions of its control blocks
  (`sp_counted_impl_pd<signal_impl<…>>::dispose`, `sp_counted_impl_p<grouped_list<…>>::dispose`,
  `sp_counted_impl_p<signal_impl<…>>::dispose`: target 9555/9554/9553/9549/9519) score 17–29 differing rows of ~140
  against the 1.89.0 probe and 46–68 against 1.90.0 and 1.91.0 (`tools/libs/vtable_compare.ts 'signals2|sp_counted'`),
  which are mutually indistinguishable → 1.89.0. Objects at -O3 (1.89.0: -Oz 17, -O2 37, -O3 41 matched; -flto 26).

## III ASN.1 runtime (TSE fork) — confidence: high (lineage), the exact source is TSE-internal

- RTTI namespace `ASN1`: `AVNEncoder AVNDecoder BERDecoder BEREncoder PERDecoder PEREncoder AbstractData
  ConstrainedObject SEQUENCE_OF_Base Constrained_INTEGER<…> ValidChecker StrictlyValidChecker InvalidTracer
  PrimitiveChecker DataLengthCounter GeneralizedTime …` match https://github.com/Kampbell/III-ASN.1 (asn1/asn1.h,
  AVNEncoder.cxx, ValidChecker.cxx, InvalidTracer.cxx, BEREncoder.cxx). TSE additions not in upstream:
  `BERDecoderPartial BufferSeeder ISeeder IAbstractVisitor DataFree`. Generated modules `Modulo*` (ModuloBoletimUrna,
  ModuloRegistroDigitalVoto, …) come from TSE's `asnparser` run over the urna ASN.1 specs — that source is not public.

## Function budget (14343 functions)

Method (`tools/libs/classify-seed.py` writes the seed, `tools/libs/classify.py` extends and propagates it): seed = (a) functions whose `i32.const` operands point at strings that occur in
exactly one library's source literal set (literal sets extracted from the vendored sources + emscripten system/lib),
(b) vtable slots: RTTI typeinfo → vtables → elem-table → function, class namespace → library (2398 functions);
then propagate along the call graph (callee called only by one class) and smooth within use-count strata
(binaryen's reorder-functions sorts by use count, stable, so link order survives inside each stratum; use-count
increases happen only 1936/14342 times along the index). Output `cls2.json`. The table below (`tools/libs/budget.tsv`)
counts the classes without `classify.py`'s last step, which makes every function that loads a TSE `std::source_location`
record `app` (4,469 in `cls2.json`); `classify.py` prints the counts with it.

| class | funcs | code bytes | notes |
| --- | --- | --- | --- |
| app (vota/comum/api/ecourna/simulador/Modulo*) | 4291 | 2,158,173 | includes generated ASN.1 modules' methods |
| unknown | 3151 | 650,125 | mostly string-free leaf code: musl, compiler-rt, libc++ internals, zlib inflate/deflate, std::function thunks |
| openssl | 2183 | 655,721 | |
| libcxx (libc++/libc++abi/demangler) | 1360 | 294,514 | |
| sqlite | 1141 | 1,048,800 | biggest code share after the app |
| 7zip | 954 | 327,945 | |
| rhvoice (+hts_engine, rapidxml, utf8, sonic) | 746 | 573,210 | |
| asn1 (III ASN.1 runtime) | 320 | 46,594 | |
| boost (header-only instantiations) | 166 | 101,559 | |
| libc | 30 | 12,645 | under-counted (no strings) |
| zlib | 1 | 274 | under-counted: inflate/deflate (~30 fns) are in unknown |

Seeds only: app 1287, libcxx 728, rhvoice 367, openssl 350, sqlite 349, 7zip 332, asn1 244, boost 64, libc 14, zlib 1.
