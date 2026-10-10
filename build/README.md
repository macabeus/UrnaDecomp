# build/ — pinned toolchain and link recipe for `vota_web_wasm.wasm`

Everything here was measured against `target/vota_web_wasm.{wasm,js,data}`; the evidence is in
`docs/reference/facts/toolchain.md`. Nothing below is a guess about what a compiler "would" emit — each statement was compiled.

## Toolchain (pinned)

| component  | value                                                                      | how it was pinned |
| ---------- | -------------------------------------------------------------------------- | ----------------- |
| emsdk      | **6.0.0** (docker `emscripten/emsdk:6.0.0`, 2026-06-04)                     | clang 23 ⇒ emsdk 5.0.0…6.0.3; glue has `programArgs` (≥ 6.0.0) and the synchronous `run()` (≤ 6.0.0) |
| emscripten | `afa15e0c56d1292e073c2c91bafc1d5e0cdf0dd3`                                 | emscripten-releases DEPS at tag 6.0.0 |
| LLVM       | `c6c2ad52586c28e74a724570985a48dc0022d250` → clang 23.0.0git               | `cmake/Modules/LLVMVersion.cmake` at that revision; SQLite string `COMPILER=clang-23.0.0` in the target |
| binaryen   | `b9c72894fba2ee5bef8e4002fb176c1917a556ad`                                 | same DEPS |
| musl       | 1.2.6 (6.0.0 bumped 1.2.5 → 1.2.6)                                          | `system/lib/libc/musl/VERSION` in the image |
| system libs| the **prebuilt** sysroot shipped in the image (`/emsdk/upstream/emscripten/cache/sysroot`) | its libc++abi carries `__FILE__ = /emsdk/emscripten/system/lib/libcxxabi/...` exactly like the target — do not rebuild it |

`Dockerfile` is `FROM emscripten/emsdk:6.0.0` plus `wabt` and the two source mount points. Build/run with
`build/docker-build.sh` (builds `urna-emsdk:6.0.0` once, then runs a command with `$PWD` mounted at `/work`).

Never run `embuilder clear`, `embuilder build …` or `emcc --clear-cache` in the image: the shipped archives are
the ones whose bytes match; a rebuild from `/emsdk/upstream/emscripten` changes the `__FILE__` strings.

## Link line (derived from the glue + wasm)

```
em++ -Oz -fexceptions <objects...> -o wasm/vota_web/vota_web_wasm.js \
  -sALLOW_MEMORY_GROWTH=1 \
  -sSTACK_SIZE=8MB \
  -sEXPORTED_FUNCTIONS=_main,_votaInit,_votaPressKey,_votaTick,_votaSetAudioEnabled,_votaGetStateJson,_uenux_wasm_web_sound_wait_cancel_requested,_uenux_wasm_web_sound_wait_finished \
  -sEXPORTED_RUNTIME_METHODS=ccall,FS,callMain \
  --preload-file <assets-dir>@/
```

Run it from the directory two levels above the output so that the file packager embeds
`PACKAGE_NAME = "wasm/vota_web/vota_web_wasm.data"` (that string is in the glue). Everything else is a 6.0.0 default:

| setting | value in target | evidence |
| ------- | --------------- | -------- |
| `-O` level (link) | **-Oz** | dlmalloc `malloc` (2567 instr), `free`, `__stdio_read`, `memchr`, `__stdio_write`, `sbrk`, `dispose_chunk` are byte-identical (modulo addresses) only at -Oz; glue has `TEXTDECODER=2` (default at SHRINK_LEVEL ≥ 2) and the no-mirror `getWasmTableEntry` (-Os/-Oz) |
| exceptions | legacy JS EH: `-fexceptions` (= `-sDISABLE_EXCEPTION_CATCHING=0`), no `-fwasm-exceptions` | `invoke_*` imports, `___cxa_find_matching_catch_*`, `setThrew` export, no `tag` section |
| `ALLOW_MEMORY_GROWTH` | 1 | memory max 32768 pages, `_emscripten_resize_heap` import |
| `STACK_SIZE` | 8 MB (8388608) | `__stack_pointer` init 10 326 000 − 8 388 608 = 1 937 392 = end of static data+bss (data ends 1 811 674, bss follows) |
| `INITIAL_MEMORY` | default (INITIAL_HEAP 16 MB) | ceil((10 326 000 + 16 777 216)/65 536) = 414 pages = the target's initial memory |
| `MINIFY_WASM_IMPORTS_AND_EXPORTS` / `..._IMPORTED_MODULES` | on (automatic at -O2+ without ASSERTIONS) | imports module `a`, exports `Cb…Yb` |
| `MAIN_READS_PARAMS` | 0 ⇒ `int main()` takes **no** `argc/argv` | glue `callMain()` has no `args`; `run()` has no parameter |
| `EXPORTED_RUNTIME_METHODS` | `ccall, FS, callMain` (+ `FS_*`, `addRunDependency`, `removeRunDependency` added automatically by `--preload-file` ⇒ `FORCE_FILESYSTEM`) | the `Module["…"] =` block is identical to the probe's |
| `ENVIRONMENT` | default (`web,webview,worker,node`) | identical shell code |
| `MODULARIZE` / `EXPORT_ES6` / `--closure` / `ASSERTIONS` | off / off / off (whitespace-minified only) / 0 | plain `var Module`, readable names, 2 `assert(` only inside the packager |
| `--emit-symbol-map` | irrelevant to the wasm bytes (measured: identical wasm with/without) | `cmp` of the two probe builds |
| `TEXTDECODER` | 2 (default at -Oz) | `var UTF8Decoder = new TextDecoder();` with no fallback |
| sockets | plain BSD sockets → `SOCKFS` (default FS); **no** `-lwebsocket.js` | `___syscall_socket/connect/sendto` imports only |
| LTO | the image ships no LTO library variants, so libc bodies come from the non-LTO archives regardless | `ls cache/sysroot/lib/wasm32-emscripten` |

The app's JavaScript glue functions (`js_*`, `wasm_input_*`) are **EM_JS** functions compiled into the C++ objects,
not a `--js-library`: emcc emits them as plain `function name(args){…}` declarations right after the
`Module["FS_createLazyFile"]=…` line and before `var _votaInit,…`, with un-prefixed import names (`Ia: js_init`),
exactly as the probe's `EM_JS(void, js_probe_log, …)` is emitted. Their bodies (whitespace-minified by emcc's
acorn pass) are extracted verbatim in `src/web/library_vota.js` (`library_vota.pretty.js` for reading).

`--preload-file <dir>@/` with the directory tree `etc/, font-variable.ttf, resource/gifs/, uenux/app/img/{,bateria,biometria}`
reproduces the packager prologue byte for byte: file_packager walks the tree and sorts full paths, so the
`FS_createPath` calls and the `files:` list (and the `.data` concatenation order) follow from the tree alone.
The target's three `/etc/*` files are empty (`start == end`).

The probe pins the link step only. The compile flags of each component (uenux2, ecourna, the ASN.1 runtime and the
third-party libraries) are pinned separately: `docs/reference/toolchain.md` §3 lists them with their evidence;
`docs/reference/app-flags.md` gives the uenux2 and ecourna flags and the evidence for each. `src/Makefile` applies them
(`UENUX2_CXXFLAGS`, `ECOURNA_CXXFLAGS`), with the ecourna additions in `src/mk/ecourna-flags.mk` and the ASN.1
runtime's in `src/mk/asn1.mk`.

## Export order (matters for `Eb…Yb` names and the glue's `assignWasmExports`)

Measured with three objects (`docs/reference/facts/toolchain.md` §T5): wasm-ld emits `memory`, `__wasm_call_ctors`,
then the exported functions **in object order on the link line**, inserting `__indirect_function_table` right after
the exports of the *first* object. In the target the table (`Fb`) sits between `_votaInit` (`Eb`) and
`_votaPressKey` (`Gb`): the first object on the TSE link line defines `votaInit` and no other exported function.

## Probe

`probe/` is the hello-world used for all of the above (`throw/catch`, `std::string`, `shared_ptr`, `malloc/free`,
`printf`, sockets, `stat`, time functions, one `EM_JS`, one `EMSCRIPTEN_KEEPALIVE` + one `EXPORTED_FUNCTIONS` symbol).

```
cd build/probe && ../docker-build.sh bash build.sh -Oz      # out-Oz/wasm/vota_web/{js,wasm,data,js.symbols}
wasm-objdump -d out-Oz/wasm/vota_web/vota_web_wasm.wasm > out-Oz/probe.objdump.txt
python3 -I ../../tools/round/wasmfn.py dump out-Oz/probe.objdump.txt out-Oz/probe.fns.jsonl
python3 -I ../../tools/round/wasmfn.py dump ../dump/vota.objdump.txt target.fns.jsonl
python3 -I ../../tools/round/wasmfn.py cmp out-Oz/probe.fns.jsonl out-Oz/wasm/vota_web/vota_web_wasm.js.symbols target.fns.jsonl --all
```

`tools/round/wasmfn.py` normalises `wasm-objdump -d` bodies (drops addresses, `call N` → `call F`, `i32.const ≥ 1024` and
load/store offsets ≥ 1024 → `A`) and finds target functions with the same body. `tools/extract/jsdefs.py a.js b.js` compares
same-named top-level definitions of two prettier-formatted glues. Expect two kinds of non-matches that are *not*
toolchain differences: binaryen whole-program passes (dead-argument elimination drops `memcpy`'s return value when no
caller uses it; duplicate-function elimination merges identical bodies), and functions whose inlining differs
because the probe's call graph differs from the app's.
