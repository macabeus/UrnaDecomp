# Facts: the toolchain and the link

What the compiler, linker and Emscripten settings behind `target/vota_web_wasm.wasm` are, and how each was
measured. Every claim was checked by building a probe in the pinned image (`urna-emsdk:6.0.0`, `build/Dockerfile`,
run through `build/docker-build.sh`) and comparing it with the target. `docs/reference/toolchain.md` is the summary;
this file holds the evidence.

## T1. Emscripten 6.0.0 (clang 23) built the target

The release is emsdk **6.0.0**: emscripten `afa15e0c56d1292e073c2c91bafc1d5e0cdf0dd3`, llvm-project
`c6c2ad52586c28e74a724570985a48dc0022d250`, binaryen `b9c72894fba2ee5bef8e4002fb176c1917a556ad`; docker image
`emscripten/emsdk:6.0.0` (released 2026-06-04).

Evidence, each step reproducible:

1. **clang major.** SQLite's compile-option table in the data says `COMPILER=clang-23.0.0`
   (`strings target/vota_web_wasm.wasm | grep COMPILER=`). Mapping each emsdk tag to its LLVM version
   (`emscripten-releases-tags.json` → the release's `DEPS` → `llvm_project_revision` → `LLVMVersion.cmake`) gives
   4.0.23 → LLVM 22, 5.0.0 … 6.0.3 → LLVM 23, 6.0.4 … 6.0.11 → LLVM 24. So emsdk ∈ [5.0.0, 6.0.3].
2. **Glue text against emscripten's `src/*.js` at each tag:**
   - `___cxa_throw` does `exceptionLast = new CppException(ptr)` unconditionally: from 5.0.5 on.
   - `var programArgs = [];` (glue line 1): `arguments_` was renamed between 5.0.7 and 6.0.0, so ≥ 6.0.0.
   - `run()` has the synchronous shape (`dependenciesFulfilled = run; return;` and the nested `setTimeout`);
     6.0.1 awaits a Promise there, so ≤ 6.0.0.
   - `Module["preloadPlugins"]`, `Module["wasmBinary"]` and the main-loop hooks are read; 6.0.2 dropped them from
     the default incoming API (consistent).
   - `___syscall_umask` exists: from 5.0.7 on (consistent).
   The intersection is exactly 6.0.0. The libc bodies (T3) are the tie-breaker against a trunk build between
   6.0.0 and 6.0.1.
3. **Dating.** SQLite's source id is `2025-07-30 19:33:53 4d8adfb3…` (3.50.4); `target/manifest.json` says
   `generatedAt` 2026-09-10. TSE pinned a three-month-old release.

Other properties read off the module and the glue:

- Exceptions are the legacy JS-based kind (`-fexceptions`, `DISABLE_EXCEPTION_CATCHING=0`): imports
  `___cxa_throw`, `___cxa_find_matching_catch_{2,3,4}`, `___resumeException`, `_llvm_eh_typeid_for` and ~40
  `invoke_*` thunks, export `setThrew`, no `tag` section (so not `-fwasm-exceptions`).
- No Asyncify/JSPI: the `_emscripten_sleep` import is the abort stub.
- `_emscripten_tempret_set` exists: the default legacy i64 pass-through.
- The glue is minified but not Closure-compiled; only two `assert(` sites remain, so `ASSERTIONS=0`.
- The syscall imports include `fdatasync` and `fallocate` (relevant to SQLite's `HAVE_*` defines,
  [libraries.md §B1](libraries.md)), and `socket`/`connect`/`sendto`: sockets are linked as plain BSD sockets
  (SOCKFS, no `-lwebsocket.js`).

## T2. The stock image already has the target's `__FILE__` paths; never rebuild its cache

The target's libc++abi assert strings read `/emsdk/emscripten/system/lib/libcxxabi/src/…`
(`strings -n 8 target/vota_web_wasm.wasm | grep '^/emsdk' | sort -u`: `cxa_demangle.cpp`,
`demangle/ItaniumDemangle.h`, `demangle/Utility.h`, `fallback_malloc.cpp`, `private_typeinfo.cpp`). A probe built in
the unmodified `emscripten/emsdk:6.0.0` image prints the same prefix. The image's emscripten lives at
`/emsdk/upstream/emscripten`, but its prebuilt sysroot (`cache/sysroot/lib/wasm32-emscripten/*.a`) was compiled
from `/emsdk/emscripten`, and the `__FILE__` bytes are frozen inside the shipped `libc++abi.a`. No symlink or move
is needed; TSE most likely used this very image.

So never run `embuilder clear`, `embuilder build …` or `emcc --clear-cache` in the image, and never build system
libraries with a changed `EMSCRIPTEN_ROOT`: rebuilt libraries would take `/emsdk/upstream/emscripten` paths and
every assert string would diverge. A port that is not cached yet triggers a lazy build, which is fine for ports;
if the cache ever looks modified, re-pull the image instead of repairing it.

## T3. The link line is `em++ -Oz -fexceptions`

```
em++ -Oz -fexceptions <objs> -o wasm/vota_web/vota_web_wasm.js -sALLOW_MEMORY_GROWTH=1 -sSTACK_SIZE=8MB \
  -sEXPORTED_FUNCTIONS=_main,_votaInit,_votaPressKey,_votaTick,_votaSetAudioEnabled,_votaGetStateJson,_uenux_wasm_web_sound_wait_cancel_requested,_uenux_wasm_web_sound_wait_finished \
  -sEXPORTED_RUNTIME_METHODS=ccall,FS,callMain --preload-file <assets>@/
```

It runs two directories above the output: the packager embeds `PACKAGE_NAME="wasm/vota_web/vota_web_wasm.data"`.
Any of the eight exports may equally be `EMSCRIPTEN_KEEPALIVE`; the glue cannot tell.

**-Oz, from the libc bodies.** `build/probe/probe.cpp` built at -O2, -O3, -Os and -Oz, bodies normalised with
`tools/round/wasmfn.py` (addresses dropped, `call N` → `call F`, `i32.const` and load/store offsets ≥ 1024 → `A`)
and looked up in the target's disassembly. Exact matches of named probe functions: -O2 47/98, -O3 51/94, -Os
58/100, -Oz 62/103. Only -Oz matches `emscripten_builtin_malloc` (target func 262) and `__stdio_read` (12326).
`free` (func 136, the first defined function) matches at -O3 and -Oz. `__memcpy`/`__memset` match at no level
(binaryen's dead-argument elimination is program-dependent, T7).

**-Oz, from the glue.** Same-named top-level glue definitions compared with `tools/extract/jsdefs.py`: against the
-Oz probe 7 of 212 differ, all of them symbol-set dependent (ATINIT order, `mmapAlloc`, the export/import
tables, the packager file list, `callMain`/`run`). Against -O3, 4 more differ, and those are settings: the
`UTF8Decoder` family (`TEXTDECODER=2`, the default only at `SHRINK_LEVEL ≥ 2`) and the mirror-less
`getWasmTableEntry` (emitted only for -Os/-Oz). -O2 is excluded twice: it keeps unminified import/export names
and does not reorder functions.

**Memory.** `__stack_pointer` starts at 10,326,000; 10,326,000 − 8,388,608 = 1,937,392 = the static end including
BSS, so `STACK_SIZE=8MB`. The initial memory of 414 pages = ceil((10,326,000 + 16 MiB) / 64 KiB): `INITIAL_MEMORY`
is the default. There is no start section; the glue calls `__wasm_call_ctors` (export `Db`).

**Defaults read off the glue.** `ENVIRONMENT` default, no `MODULARIZE`/`EXPORT_ES6`, no Closure, `ASSERTIONS=0`,
`WASM_BIGINT` on, `SUPPORT_LONGJMP=emscripten`, `FORCE_FILESYSTEM` set by `--preload-file` (which is what exports
`FS_createPath`, `FS_createDataFile`, `FS_preloadFile`, …). The explicit `EXPORTED_RUNTIME_METHODS` are exactly
`ccall, FS, callMain`: a probe with that list reproduces the `Module["…"]=` block verbatim.

The image ships no LTO variants of libc/libc++, so matching libc bodies say nothing about `-flto` on app objects.
`--emit-symbol-map` leaves an -O2 wasm unchanged but changes an -Oz one ([link-order.md §L1](link-order.md)).

## T4. EM_JS, the packaged files and `main()`

1. **The app's JS functions are `EM_JS`, not a `--js-library`.** In the glue they are plain
   `function js_init(width,height){…}` declarations emitted after `Module["FS_createLazyFile"]=FS_createLazyFile;`
   and imported under their bare names. A `--js-library` symbol would be `var _js_init = …` imported as
   `_js_init`. The extracted text is `src/web/library_vota.js` (minified, 18,411 bytes, sha256
   `bc57f1643991f00f7c93bf8f816fe3abf74c44df991c24ce8ec2a828dc39589d`; prettier copy `library_vota.pretty.js`),
   32 functions by name in the glue.
2. **emcc emits an EM_JS function into the glue only when the final wasm imports it.** The `__em_js_ref_*` pointer
   does not keep it: a TU with 34 EM_JS functions and a `main` that calls none gives a glue with none of them.
   The target imports all 34, so the skeleton's roots call all 34.
3. **Formatting survives.** EM_JS bodies written as prettier-formatted JS inside `EM_JS(ret, name, (int w, int h),
   { … })` come out of emcc's acorn minifier byte-identical to the target's text, including the regex literal,
   single-quoted strings and the `0.2 → .2`, `1000 → 1e3` renumbering. Parameter names come from the C parameter
   list, types from the import signatures (`i32 → int`, `f64 → double`): `tools/extract/gen_wasm_shim.py` generates
   `src/web/wasm_shim.cpp` that way.
4. **file_packager.** `--preload-file <dir>@/` over `etc/`, `font-variable.ttf`, `resource/gifs/` and
   `uenux/app/img/{,bateria,biometria}` reproduces the glue's packager prologue byte for byte except the file list.
   Paths are sorted lexicographically; the `.data` file is their concatenation in that order (70 files,
   14,855,585 bytes; `/etc/avetc.vst`, `/etc/dependencias.properties` and `/etc/versoes.properties` are empty).
5. **`main()` takes no parameters.** The glue's `callMain` has `var argc=0; var argv=0;`; a probe with
   `int main(int argc, char** argv)` gets argv marshalling and a wasm export named `__main_argc_argv`. So
   `MAIN_READS_PARAMS=0`: the TSE source declares `int main()`.

## T5. Export order, the table's position and the export letters

wasm-ld emits `memory`, `__wasm_call_ctors`, then the exported functions **in symbol-table order, which is object
order on the link line** (reordering `EXPORTED_FUNCTIONS` changes nothing). `__indirect_function_table` enters the
symbol table when wasm-ld parses the first input that **imports** it (LLD's `addUndefinedTable`), so its export
sits right after the exports of the first object that makes an indirect call.

Measured twice:

- Three objects `a.o{aInit,aPress,aTick}`, `b.o{bHelper,main+fn-pointer}`, `c.o{cOne}` linked in each order put
  the table after the first object's exports, because in that experiment the first object imported the table.
- On the skeleton link with `vota_init.cpp` = `extern "C" int votaInit(int a) { urna_roots(a); return a; }` (no
  `call_indirect`), the table landed after `main` (the `int main()` TU imports the table through libc's
  `main(argc,argv)` → `__main_void`). Calling through `int (*volatile urna_roots_ptr)(int) = urna_roots;` makes
  `vota_init.o` import the table and gives the target's order exactly: `Cb memory, Db ctors, Eb votaInit, Fb TABLE,
  Gb votaPressKey, … Kb main, Lb, Mb`.

So in the target, votaInit's TU is the first object on the link line, exports only `votaInit` and makes an
indirect call.

Two more facts from the same links:

- Minified export names continue the import alphabet: 136 imports → exports start at `Cb`. A link with 135
  imports (no `__assert_fail`) got `memory=Bb`. Export letters agree with the target only when the import count
  is 136.
- The library tail is symbol-table order too. Target: `__cxa_decrement_exception_refcount,
  __cxa_increment_exception_refcount, ntohs, htons, emscripten_builtin_memalign, setThrew, …`. So an app object
  that references the exception-refcount functions (`std::exception_ptr`/`current_exception`) is parsed before the
  first reference to `memalign`; `ntohs`/`htons` exist only because the app links socket code.

## T6. `-DNDEBUG` everywhere; what the 7-Zip object set needs

1. The target imports no `__assert_fail`. A skeleton built without `-DNDEBUG` imports it, from RHVoice objects
   (`hts_labeller.cpp`, `phoneme_set.cpp`, `userdict.cpp`) and from app TUs that include Boost/RHVoice headers.
   RHVoice built with it (`tools/libs/rhvoice/build.sh 1.14.0 cxx14-O3-ndebug -O3 -std=c++14 -DNDEBUG`) and the stubs
   with `-DNDEBUG` remove the import. The import is the evidence, not a match count: a match count need not move
   with the flag, since a function holding an assert pairs only once the rest of its body does. Conan's Release
   profile passes `-DNDEBUG`, consistent with the libraries' `-O3`. SQLite and OpenSSL use no `assert()`.
2. 7-Zip's object set (`tools/libs/7zip/sources.txt`), linked with the exact target line (without
   `-sERROR_ON_UNDEFINED_SYMBOLS=0`), is undefined on `sysinfo`, `sched_setaffinity`, `Bench` and `GetCpuName`;
   `src/app/stubs_7zip.cpp` defines them. None is a target import, so TSE patched, defined or GC'd them —
   undecided. `MainAr.cpp` (7-Zip's own `main`) is excluded. `Console/Main.cpp` must stay or be replaced: it is the
   TU that includes `Common/MyInitGuid.h` and so defines every `IID_*`. `Windows/FileLink.cpp` brings `symlink` (a
   `__syscall_symlinkat` import the target lacks), so that function is not in the app's reachable set.

## T7. Comparing a body across two links

- Absolute addresses hide in load/store **offset immediates**, not only in `i32.const`: binaryen folds
  `i32.const ADDR; i32.load` into `i32.const 0; i32.load offset=ADDR`. Target and probe `free()` differ only in
  two such offsets: they look like a 0.997 near-miss unless offsets ≥ 1024 (`GLOBAL_BASE`) are normalised too.
- binaryen's whole-program passes change library bodies per program at -Oz: dead-argument elimination drops an
  unused return value (the probe's `__memcpy` lacks the target's `local.get 0` tail, func 504),
  duplicate-function elimination merges identical bodies (target func 6263 is both `ntohs` and `htons`), and
  inlining depends on callers. A library body that does not match is not evidence of a wrong toolchain unless a
  caller-independent function (`malloc`, `free`, `strlen`, `__stdio_read`, `memchr`, `sbrk`, `__cxa_*`) fails too.
- `wasm-objdump -d` of the target is 174 MB: parse it once into normalised per-function bodies
  (`tools/round/wasmfn.py dump`) and hash.
- prettier re-wraps the packager's long file array with a lone `],` whose indentation depends on content length:
  a diff artefact, not a glue difference.

## T8. Candidate compiles use 6.0.0 at -Oz

A rank template that links candidates with another release (`emscripten/emsdk:latest`, 6.0.11 or later, is
clang 24) at -O2 measures the wrong thing: at -O2 emcc neither minifies names nor reorders functions, and dlmalloc's
`free` scores 0.86 similarity instead of byte-identical. Candidates link as

```
em++ -Oz -fexceptions <flags> <input> -o <out>.wasm --no-entry -sERROR_ON_UNDEFINED_SYMBOLS=0 -sEXPORTED_FUNCTIONS=_<symbol>
```

in `urna-emsdk:6.0.0` (or `emscripten/emsdk:6.0.0`). `-fexceptions` changes the codegen of any function with a
call inside a `try` or with destructors on the unwind path, so it belongs in the compile step of the components
that use it (see [app-flags.md](app-flags.md) for which ones).
