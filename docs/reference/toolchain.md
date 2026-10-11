# Toolchain report: how `vota_web_wasm.wasm` was built

This report gives what the project knows about the compiler, linker and flags that produced
`target/vota_web_wasm.wasm` (sha256 `8579ef8d1a93819152cecdbf52906d2e4dd0f57999cb2e1e8c0d1161cd1007e2`, 7,713,699 bytes).
Every statement below is checked by compiling with the pinned toolchain and comparing against the target. The detailed
evidence lives in `build/README.md`, `docs/reference/app-flags.md`, `third_party/MANIFEST.md` and `docs/reference/link-order.md`.

## 1. Compiler and toolchain

The module was built with **Emscripten 6.0.0**, whose bundled compiler is **clang 23.0.0**.

| component | pinned value |
| --- | --- |
| emsdk | 6.0.0, docker image `emscripten/emsdk:6.0.0` |
| emscripten | `afa15e0c56d1292e073c2c91bafc1d5e0cdf0dd3` |
| LLVM / clang | `c6c2ad52586c28e74a724570985a48dc0022d250` = clang 23.0.0git |
| binaryen (wasm-opt) | `b9c72894fba2ee5bef8e4002fb176c1917a556ad` |
| musl | 1.2.6 |
| system libraries | the prebuilt sysroot shipped in the image |

### How the version is pinned

1. **Compiler family and major version.** SQLite embeds its compile options, and the target contains the string
   `COMPILER=clang-23.0.0`. The libc++abi assert strings carry `/emsdk/emscripten/system/lib/libcxxabi/...`, an
   Emscripten system-library path. GCC is ruled out.
2. **Release range.** Only emsdk releases 5.0.0 through 6.0.3 ship clang 23, read from each release tag's LLVM revision.
3. **Exact release.** Three markers in the JavaScript glue narrow the range to one release. `CppException` is always
   present, which needs 5.0.5 or later. `programArgs` exists, which needs 6.0.0 or later. `run()` is synchronous, which
   rules out anything after 6.0.0.
4. **Byte confirmation.** dlmalloc `malloc` (2,567 instructions), `free`, `dispose_chunk`, `sbrk`, `memchr`,
   `__stdio_read` and `__stdio_write` match the target byte for byte, modulo addresses, only when linked with the stock
   6.0.0 image at `-Oz`.

### System library paths

The stock `emscripten/emsdk:6.0.0` image already produces the `/emsdk/emscripten/system/lib/libcxxabi/...` strings,
because its prebuilt archives were built there. Do not run `embuilder clear`, `embuilder build` or
`emcc --clear-cache`. Rebuilding the system libraries changes those `__FILE__` bytes.

## 2. Link step

```
em++ -Oz -fexceptions <objects...> -o wasm/vota_web/vota_web_wasm.js \
  -sALLOW_MEMORY_GROWTH=1 \
  -sSTACK_SIZE=8MB \
  -sEXPORTED_FUNCTIONS=_main,_votaInit,_votaPressKey,_votaTick,_votaSetAudioEnabled,_votaGetStateJson,_uenux_wasm_web_sound_wait_cancel_requested,_uenux_wasm_web_sound_wait_finished \
  -sEXPORTED_RUNTIME_METHODS=ccall,FS,callMain \
  --preload-file <assets-dir>@/
```

Run it from the directory two levels above the output. The glue then embeds
`PACKAGE_NAME = "wasm/vota_web/vota_web_wasm.data"`, as the target's glue does.

| setting | target value | evidence |
| --- | --- | --- |
| link optimisation | `-Oz` | the libc bodies above match only at `-Oz`; the glue has `TEXTDECODER=2` and the no-mirror `getWasmTableEntry` |
| exception handling | legacy JavaScript EH: `-fexceptions`, not `-fwasm-exceptions` | `invoke_*` imports, `__cxa_find_matching_catch_2/3/4`, `__resumeException`, `llvm_eh_typeid_for`, a `setThrew` export, no `tag` section |
| memory growth | on, maximum 2 GB | memory max 32,768 pages, `emscripten_resize_heap` import |
| stack size | 8 MB | stack pointer init 10,326,000 − 8,388,608 = 1,937,392, the end of static data |
| initial memory | default (16 MB heap) | ceil((10,326,000 + 16,777,216) / 65,536) = 414 pages, the target's initial memory |
| import/export minification | on (automatic at `-O2` and above without assertions) | import module `a`, fields `a…kb`; exports `Cb…Yb` |
| `main` signature | `int main()` with no `argc/argv` | `callMain()` takes no arguments in the glue |
| runtime methods | `ccall`, `FS`, `callMain`; `--preload-file` adds the `FS_*` helpers | the glue's `Module[...]` block matches a probe built this way |
| assertions, Closure, `MODULARIZE` | off | readable glue names, plain `var Module`, only two `assert(` calls |
| sockets | plain BSD sockets through the default `SOCKFS`, no `-lwebsocket.js` | only `__syscall_socket/connect/sendto` imports |
| name and producers sections | absent | the default `--strip-debug` result at `-Oz` |

**App JavaScript.** The app's JavaScript functions (`js_*`, `wasm_input_*`, 32 functions) are `EM_JS` functions compiled
into the C++ objects, not a `--js-library`. Their bodies are extracted verbatim in `src/web/library_vota.js`.

**Packager payload.** `--preload-file <dir>@/` over the unpacked asset tree reproduces `vota_web_wasm.data` byte for byte.

**Export order.** wasm-ld emits exports in object order on the link line. It places the function table right after the
first object's exports. In the target the table sits between `votaInit` and `votaPressKey`. So the first object on the
link line defines `votaInit` and no other exported function.

## 3. Compile flags per component

Each row is measured by reconstructing or building that component and scoring its functions against the target.

| component | source root embedded in the binary | compile flags |
| --- | --- | --- |
| uenux2 (the application) | `/home/rubio/tse/uenux2/src/...` | `-O2 -fPIC -std=c++20 -DNDEBUG -fno-strict-aliasing`, no `-fexceptions` |
| ecourna (conan package `libecea1da310e5107`) | `/home/rubio/.conan2/p/b/libecea1da310e5107/b/src/ecourna/...` | `-O3 -fexceptions -std=c++20 -DNDEBUG -fno-strict-aliasing -fno-delete-null-pointer-checks`; `-fPIC` per file |
| III ASN.1 runtime (TSE fork) | none | `-Os -fno-exceptions -std=c++17 -DNDEBUG` |
| SQLite 3.50.4 amalgamation | none | `-O3 -fPIC` + `-DSQLITE_ENABLE_COLUMN_METADATA -DSQLITE_ENABLE_MATH_FUNCTIONS -DSQLITE_ENABLE_RTREE -DSQLITE_ENABLE_UNLOCK_NOTIFY` + `-DHAVE_FDATASYNC -DHAVE_GMTIME_R -DHAVE_LOCALTIME_R -DHAVE_POSIX_FALLOCATE -DHAVE_STRERROR_R -DHAVE_USLEEP` |
| OpenSSL 3.0.17 (libcrypto only) | relative `crypto/...` paths | objects at `-O3`; `./Configure cc no-shared --release --prefix=/ --libdir=lib --openssldir=/etc/ssl threads no-unit-test no-tests -fPIC no-fips no-md2`, `CC=emcc` |
| 7-Zip / LZMA SDK 21.02 | none | `-O3 -D_7ZIP_ST -DNDEBUG` |
| RHVoice 1.14.0 (+ hts_engine, sonic) | none | `-O3 -std=c++14 -DNDEBUG` |
| Boost 1.89.0 | header-only | instantiated by the including TU's flags |
| zlib 1.3.1 + minizip | none | `-O3 -fPIC -DNDEBUG -D_LARGEFILE64_SOURCE=1` (stopgap, not yet confirmed) |

### Evidence behind the application flags

**uenux2.** A whole-link sweep over reconstructed units decides these flags.
- `-O2` and `-O3` give identical results on every unit measured so far. `-Os` loses `CIniKey::operator=`, and `-Oz` loses all the units.
- `-fPIC` removes the last structural differences in the constructors.
- `-fno-strict-aliasing`: comum's converters reload a vector's begin right after storing a constant, which only code without TBAA does (fn 10289).
- None of the 1,645 uenux2 functions that load an error record call an `invoke_*` thunk. Their `throw` sites call `__cxa_throw` directly, so the code was built without `-fexceptions`.

**ecourna.** A sweep over reconstructed units decides these flags.
- `-O3` is the only level at which the `CError` constructor scores 0.
- 134 of the 194 ecourna functions that load an error record call `invoke_*`, which needs `-fexceptions`.
- `-std=c++23` is excluded: it changes `EncodeBase64(...).substr(0, 8)` through the rvalue `substr() &&` overload.
- `-fno-strict-aliasing` keeps `std::vector`'s exception guard out of line: the `CInfoSalt` constructor drops from 164 to 75 differing rows, all operand-only.
- `-fno-delete-null-pointer-checks` keeps the short-string `if/else` shape instead of a `select`.

**`-fPIC` in ecourna is per file.**
- `api/security/base64.cpp` is PIC: the table address is added, never folded into the load offset.
- `api/io/*` and `api/compression/*` are non-PIC: their table addresses are folded into the load offset.

**ASN.1 runtime.** `-Os -fno-exceptions -DNDEBUG` gives the best census, 208 of 320 functions in a probe. No ASN.1 function calls an `invoke_*` import.

### Source paths in the binary

**Path mapping.** The original build directories are embedded through `std::source_location` records. To reproduce
them from another directory, use `-ffile-prefix-map`. The resulting object is byte-identical to one compiled at the
real path:

```
-ffile-prefix-map=/work/src/ecourna=/home/rubio/.conan2/p/b/libecea1da310e5107/b/src/ecourna
-ffile-prefix-map=/work/src/uenux2=/home/rubio/tse/uenux2
```

**Error records.** The file, line and function strings are C++20 `std::source_location` records, not an assert macro.
Exception constructors take `std::source_location::current()` as a defaulted parameter.
`ledger/source-locations.tsv` lists all 1,939 records with the functions that reference them.

**Encoding.** The application sources are Latin-1 (ISO-8859-1), with one UTF-8 string.

## 4. Post-link optimiser behaviour that shapes the bytes

The link runs wasm-ld, then `wasm-opt -Oz`, then `wasm-metadce`. These stages fix the function order and several
whole-program effects, as rebuilds show:

1. wasm-ld lays out functions in link order. emcc then removes the name section with `llvm-objcopy` unless `-g`,
   `--profiling-funcs` or `--emit-symbol-map` keeps it; names change what `-Oz` does, which is why a symbol-map
   build has another order ([include-asm.md](include-asm.md)).
2. `wasm-opt -Oz` keeps link order. It inlines, removes dead arguments (DAE) and merges duplicate functions. On the
   frozen skeleton of [link-order.md](link-order.md) §1 it deletes 5,826 functions (15,824 → 9,998) and appends 216
   merged-function thunks (`byn$mgfn`).
3. `wasm-metadce` names each function by its post-optimisation index, removes unreachable ones, and sorts the code
   section by use count, descending. Ties are broken by the descending decimal string of that index.

So inside each use-count band, functions appear in a scrambled link order. A build with `--emit-symbol-map` has a
different function order, so never byte-compare one. The law reproduces that skeleton's order with 0 of 10,212
functions out of place.

Because of step 2, a function's final bytes depend on the whole program. Its callers decide inlining and dead-argument
removal, and data addresses and call indices come from the full link. An isolated build can match a function's
structure, but byte identity needs the complete link.

## 5. Reproducing the toolchain

| file | purpose |
| --- | --- |
| `build/Dockerfile` | `FROM emscripten/emsdk:6.0.0` plus wabt and the source mount points |
| `build/docker-build.sh` | builds the `urna-emsdk:6.0.0` image once, then runs a command with `$PWD` mounted at `/work` |
| `build/probe/` | the probe program used to pin the version and link flags |
| `tools/libs/<library>/build.sh` | exact build recipe for each third-party library |
| `src/Makefile` | the whole-module link with per-component flags |

```
build/docker-build.sh em++ --version          # clang version 23.0.0git
make -C src link                               # whole-module link into build/out/wasm/vota_web/
```

## 6. The matching toolkit

Every measurement above that scores functions (the library censuses, the flag sweeps, the per-function pairing state)
comes from `asmlift-wasm/`, the toolkit in this repository. `asmlift-wasm/README.md` documents its commands, options
and tests; the library census results are in `tools/libs/*/census.json`, the records of the probe runs that pin
each library (the paths inside them are not kept up to date).
