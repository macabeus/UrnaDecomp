#!/bin/bash
# The symbol names of a link's inputs, mangled and demangled, for the lifter: it spells a callee by the mangled
# symbol whose demangled form is the ledger's name (asmlift-wasm/src/lift-inputs.ts).
#
#   tools/round/link-symbols.sh [<plain out dir, default build/out/wasm/vota_web>]   (run by make -C src link)
#
# Reads the objects and archives of <plain>/link.cmd and the emscripten system libraries the link pulls members from,
# and writes <plain>/symbols.mangled.txt, every symbol any of them defines or references (llvm-nm, sorted, unique),
# and <plain>/symbols.demangled.txt, the same lines through llvm-cxxfilt: two line-aligned lists of the pinned
# toolchain.
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/../.." && pwd)
cd "$ROOT"
PLAIN=${1:-build/out/wasm/vota_web}
[ -f "$PLAIN/link.cmd" ] || { echo "no $PLAIN/link.cmd: run make -C src link first" >&2; exit 2; }
inputs=$(tr ' ' '\n' < "$PLAIN/link.cmd" | grep -E '\.(o|a)$' | tr '\n' ' ')
[ -n "$inputs" ] || { echo "$PLAIN/link.cmd names no object" >&2; exit 2; }
out=${PLAIN#build/out/}
sysroot=/emsdk/upstream/emscripten/cache/sysroot/lib/wasm32-emscripten
inputs="$inputs $(printf "$sysroot/%s " libc.a libc_optz.a libc++.a libc++abi.a libcompiler_rt.a libdlmalloc.a)"
build/docker-build.sh bash -c "cd build/out \
  && /emsdk/upstream/bin/llvm-nm -j $inputs 2>/dev/null | grep -v -e '^\$' -e ':\$' | LC_ALL=C sort -u > $out/symbols.mangled.txt \
  && /emsdk/upstream/bin/llvm-cxxfilt < $out/symbols.mangled.txt > $out/symbols.demangled.txt"
echo "$PLAIN/symbols.{mangled,demangled}.txt: $(wc -l < "$PLAIN/symbols.mangled.txt" | tr -d ' ') symbols"
