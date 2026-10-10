#!/bin/bash
# Reproduce the SQLite block of vota_web_wasm.wasm: compile the 3.50.4 amalgamation (third_party/sqlite) with the
# pinned toolchain and link it into a probe module with the target's link line.
# Run from the urna root inside the pinned image:
#   build/docker-build.sh bash tools/libs/sqlite/build.sh <name> [compile flags...]
# With no compile flags the best-known set (census.json; docs/reference/facts/libraries.md §B1) is used.
# Output: build/out/sqlite/<name>/plain/… (byte-comparable, NO symbol map) and
#         build/out/sqlite/<name>/named/… (same link + --emit-symbol-map, names only: the map
#         changes the -Oz function order, never byte-compare it).
set -euo pipefail
NAME=${1:?name}; shift || true
HERE=tools/libs/sqlite
SRC=third_party/sqlite
OUT=build/out/sqlite/$NAME
grep -q '^#define SQLITE_VERSION  *"3\.50\.4"' "$SRC/sqlite3.h" 2>/dev/null \
  || { echo "$SRC is not the SQLite 3.50.4 amalgamation (tools/setup/bootstrap.sh fetches it)" >&2; exit 2; }
# -O3 -fPIC = conan-center's sqlite3 recipe built as Release with fPIC=True (CMake adds -fPIC through
# POSITION_INDEPENDENT_CODE); measured against the target: -O3 -fPIC finds 1385 of the probe's 1407 sqlite
# functions in the target, -O3 alone 683 of 1141 budgeted, -O2/-Os/-Oz/-flto far fewer (census.json).
DEFAULT_CFLAGS=(-O3 -fPIC
  -DSQLITE_ENABLE_COLUMN_METADATA -DSQLITE_ENABLE_MATH_FUNCTIONS -DSQLITE_ENABLE_RTREE -DSQLITE_ENABLE_UNLOCK_NOTIFY
  -DHAVE_FDATASYNC -DHAVE_GMTIME_R -DHAVE_LOCALTIME_R -DHAVE_POSIX_FALLOCATE -DHAVE_STRERROR_R -DHAVE_USLEEP)
if [ $# -gt 0 ]; then CFLAGS=("$@"); else CFLAGS=("${DEFAULT_CFLAGS[@]}"); fi
mkdir -p "$OUT/plain/wasm/vota_web" "$OUT/named/wasm/vota_web"
printf '%s\n' "${CFLAGS[@]}" > "$OUT/cflags.txt"
emcc "${CFLAGS[@]}" -I"$SRC" -c "$SRC/sqlite3.c" -o "$OUT/sqlite3.o"
emcc -Oz -fexceptions -I"$SRC" -c "$HERE/main.c" -o "$OUT/main.o"
LINK=(-Oz -fexceptions "$OUT/sqlite3.o" "$OUT/main.o"
  -sALLOW_MEMORY_GROWTH=1 -sSTACK_SIZE=8MB
  -sEXPORTED_FUNCTIONS=@"$HERE/exports.txt"
  -sEXPORTED_RUNTIME_METHODS=ccall,FS,callMain)
em++ "${LINK[@]}" -o "$OUT/plain/wasm/vota_web/vota_web_wasm.js"
em++ "${LINK[@]}" -o "$OUT/named/wasm/vota_web/vota_web_wasm.js" --emit-symbol-map
ls -la "$OUT/plain/wasm/vota_web/vota_web_wasm.wasm" "$OUT/named/wasm/vota_web/vota_web_wasm.wasm"
