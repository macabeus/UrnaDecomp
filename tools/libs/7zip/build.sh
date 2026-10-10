#!/bin/bash
# Probe build of the 7-Zip / LZMA SDK C++ 7z writer from third_party/7zip, with the pinned toolchain.
#   tools/libs/7zip/build.sh <version> [<variant>] [extra compile flags...]      (from the urna root)
# <version>  the 7-Zip release third_party/7zip holds (21.02: the source release tools/setup/bootstrap.sh extracts
#            there); the script reads it from C/7zVersion.h and refuses another. A probe of another release puts
#            that release's C/ and CPP/ in third_party/7zip.
# <variant>  st… (default st): -D_7ZIP_ST, the single-threaded build (the target has NCoderMixer2::CMixerST and
#            no CMixerMT, so USE_MIXER_ST alone was compiled); mt…: no _7ZIP_ST, adds the MT-only objects.
#            The rest of the name is free (st-O2, st-cxx20): it names the output dir for the extra flags given,
#            which go after the common flags (a later -O2 overrides the -Oz of the compile step).
# Sources: tools/libs/7zip/sources.txt (the Alone7z/7zr object list) + driver.cpp (the entry points the app
# calls, exported, so wasm-opt keeps the app's reachable set; -sEXPORT_ALL does not keep anything here —
# measured: 11 exports either way). Output: build/out/7zip/<version>-<variant>/probe.{js,wasm,js.symbols}.
# LINK_EXTRA="<flags>" in the environment is appended to the link line (e.g. LINK_EXTRA=-flto with -flto in the compile flags).
set -euo pipefail
VER=${1:?usage: build.sh <version> [variant] [flags...]}
VARIANT=${2:-st}
shift; [ $# -gt 0 ] && shift
EXTRA=("$@")
ROOT=$(cd "$(dirname "$0")/../../.." && pwd)
cd "$ROOT"
SRC=third_party/7zip
OUT=build/out/7zip/$VER-$VARIANT
if [ -z "${INSIDE_DOCKER:-}" ]; then
  have=$(sed -n 's/^#define MY_VERSION_NUMBERS "\([0-9.]*\).*/\1/p' "$SRC/C/7zVersion.h" 2>/dev/null || true)
  [ "$have" = "$VER" ] || { echo "$SRC holds 7-Zip ${have:-nothing}, not $VER (tools/setup/bootstrap.sh fetches it)" >&2; exit 2; }
  exec build/docker-build.sh env INSIDE_DOCKER=1 LINK_EXTRA="${LINK_EXTRA:-}" bash tools/libs/7zip/build.sh "$VER" "$VARIANT" ${EXTRA[@]+"${EXTRA[@]}"}
fi

mkdir -p "$OUT/obj"
COMMON=(-Oz -fexceptions -DNDEBUG -D_REENTRANT -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE)
case "$VARIANT" in
  st*) COMMON+=(-D_7ZIP_ST) ;;
  mt*) ;;
  *) echo "variant must start with st or mt: $VARIANT" >&2; exit 2 ;;
esac
COMMON+=(${EXTRA[@]+"${EXTRA[@]}"})
echo "compile flags: ${COMMON[*]}"

LIST=$(mktemp)
while read -r path kind; do
  case "$path" in ''|'#'*) continue ;; esac
  if [ "$kind" = mt ] && [[ "$VARIANT" == st* ]]; then continue; fi
  if [ ! -f "$SRC/$path" ]; then echo "skip (not in $VER): $path" >&2; continue; fi   # the list is 22.01's; 21.02 lacks C/LzFindOpt.c and CPP/Common/LzFindPrepare.cpp
  echo "$path"
done < tools/libs/7zip/sources.txt > "$LIST"

compile_one() {
  local path=$1 obj f CFLAGS=()
  obj="$OUT/obj/$(echo "$path" | tr '/' '_').o"
  if [ "$path" != "${path%.c}" ]; then
    for f in "${COMMON[@]}"; do case "$f" in -std=c++*|-std=gnu++*) ;; *) CFLAGS+=("$f") ;; esac; done
    emcc "${CFLAGS[@]}" -c "$SRC/$path" -o "$obj"
  else
    em++ "${COMMON[@]}" -DPROG_VARIANT_R -c "$SRC/$path" -o "$obj"
  fi
}
export -f compile_one; export OUT SRC
printf '%s\0' "${COMMON[@]}" > "$OUT/flags.nul"   # arrays do not export: each worker re-reads the flags NUL-separated
xargs -P4 -I{} bash -c 'COMMON=(); while IFS= read -r -d "" f; do COMMON+=("$f"); done < "$OUT/flags.nul"; compile_one "$1"' _ {} < "$LIST"
rm -f "$LIST"
# the keep-alive driver (see driver.cpp); DRIVER=none builds the bare library (functions then survive only through vtables)
if [ "${DRIVER:-driver}" != none ]; then
  em++ "${COMMON[@]}" -I"$SRC" -c tools/libs/7zip/driver.cpp -o "$OUT/obj/driver.o"
fi

OBJS=("$OUT"/obj/*.o)
# Link to a .js (+ .wasm) exactly like the target: `-o x.wasm` alone implies STANDALONE_WASM (a `_initialize`
# export, WASI libc paths, EXPORT_ALL silently dropped — measured: 11 exports, functions kept only via the table).
em++ -Oz -fexceptions "${OBJS[@]}" -o "$OUT/probe.js" --no-entry -sERROR_ON_UNDEFINED_SYMBOLS=0 -sEXPORT_ALL=1 \
  -sALLOW_MEMORY_GROWTH=1 -sSTACK_SIZE=8MB --emit-symbol-map ${LINK_EXTRA:-}
# --emit-symbol-map changes only the function ORDER of a -Oz link (docs/reference/facts/link-order.md §L1), never a
# body, and the census pairs by fingerprint, so the names are free: <out>/probe.js.symbols maps index -> name.
ls -l "$OUT"/probe.wasm*
