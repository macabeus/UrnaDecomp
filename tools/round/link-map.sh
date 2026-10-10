#!/bin/bash
# Our side of the splat symbol maps: names every function of the PLAIN link and places its data symbols, without
# touching the plain link itself.
#
#   tools/round/link-map.sh [<plain out dir, default build/out/wasm/vota_web>]   (make -C src link-map)
#
# 1. Re-runs the plain link's own em++ line (<plain>/link.cmd) into <plain>/../../wasm-map/<name>/ with wasm-ld's
#    --Map (data and function symbols at their wasm-ld addresses), --no-demangle (mangled names in the map and in the
#    wasm-ld name section) and EMCC_DEBUG=1 (every intermediate module kept under …/wasm-map/tmp/), then
#    FAILS unless the scratch wasm is byte-identical to the plain one: only then do the scratch maps describe it.
# 2. Replays binaryen on the kept intermediates (tools/round/splat_names.py): wasm-opt -Oz again with --symbolmap (post-Oz
#    index → wasm-ld index), metadce + the final wasm-opt again with index names and -g (final index → post-Oz index),
#    each replay checked against emcc's own intermediate / the plain module.
# Writes build/out/wasm-map/vota_web/{vota_web_wasm.map,names.tsv,replay.log} for the default plain link; names.tsv is
# final function index → mangled wasm-ld name, object, wasm-ld index, post-Oz index, demangled name (a binaryen
# merged-function thunk is byn$mgfn-shared$<the first merged function's name>).
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/../.." && pwd)
cd "$ROOT"
PLAIN=${1:-build/out/wasm/vota_web}
PLAIN_REL=${PLAIN#build/out/}
parent=$(dirname "$(dirname "$PLAIN_REL")")
MAP_REL=$([ "$parent" = . ] && echo wasm-map || echo "$parent/wasm-map")
TMP_REL=$MAP_REL/tmp
MAP_REL=$MAP_REL/$(basename "$PLAIN_REL")
OUT=build/out/$MAP_REL
[ -f "$PLAIN/link.cmd" ] || { echo "no $PLAIN/link.cmd: run make -C src link first" >&2; exit 2; }
grep -q -- " -o $PLAIN_REL/vota_web_wasm.js " "$PLAIN/link.cmd" || { echo "$PLAIN/link.cmd does not write $PLAIN_REL/vota_web_wasm.js" >&2; exit 2; }
mkdir -p "$OUT"
# emcc numbers its kept intermediates per run: a previous run's would be read as this run's
rm -rf "build/out/$TMP_REL"
mkdir -p "build/out/$TMP_REL"
cmd=$(sed "s# -o $PLAIN_REL/vota_web_wasm.js # -o $MAP_REL/vota_web_wasm.js #" "$PLAIN/link.cmd")
cmd="$cmd -Wl,--Map=$MAP_REL/vota_web_wasm.map -Wl,--no-demangle"
printf '%s\n' "$cmd" > "$OUT/link.cmd"
build/docker-build.sh bash -c "cd build/out && EMCC_DEBUG=1 EMCC_TEMP_DIR=/work/build/out/$TMP_REL $cmd" > "$OUT/link.log" 2>&1 \
  || { tail -20 "$OUT/link.log" >&2; exit 1; }
if ! cmp -s "$OUT/vota_web_wasm.wasm" "$PLAIN/vota_web_wasm.wasm"; then
  echo "FAIL: the --Map/EMCC_DEBUG scratch link differs from $PLAIN/vota_web_wasm.wasm: its maps do not describe the plain link" >&2
  cmp "$OUT/vota_web_wasm.wasm" "$PLAIN/vota_web_wasm.wasm" >&2 || true
  exit 1
fi
echo "scratch link byte-identical to $PLAIN/vota_web_wasm.wasm"
python3 -I -B tools/round/splat_names.py replay-script "$OUT" "build/out/$TMP_REL" > "$OUT/replay.sh"
build/docker-build.sh bash "$OUT/replay.sh" > "$OUT/replay.log" 2>&1 || { tail -20 "$OUT/replay.log" >&2; exit 1; }
python3 -I -B tools/round/splat_names.py compose "$OUT" "build/out/$TMP_REL" "$PLAIN/vota_web_wasm.wasm"
# a demangled column (llvm-cxxfilt of the pinned toolchain), the spelling of --emit-symbol-map files and the ledger
tail -n +2 "$OUT/names.tsv" | cut -f2 > "$OUT/names.mangled.txt"
build/docker-build.sh bash -c "/emsdk/upstream/bin/llvm-cxxfilt < $OUT/names.mangled.txt > $OUT/names.demangled.txt"
{ printf 'demangled\n'; cat "$OUT/names.demangled.txt"; } | paste "$OUT/names.tsv" - > "$OUT/names.tsv.tmp"
mv "$OUT/names.tsv.tmp" "$OUT/names.tsv"
