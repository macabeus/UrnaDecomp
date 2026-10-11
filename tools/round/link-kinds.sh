#!/bin/bash
# Our side of the operand kinds (asmlift-wasm/src/splat-kinds.ts): links the PLAIN link's objects a second time with
# the data and the table moved, so that every constant of ours that is an address or a table slot moves and every
# plain integer does not. The plain link itself is untouched.
#
#   tools/round/link-kinds.sh [<plain out dir, default build/out/wasm/vota_web>]   (make -C src link-kinds)
#
# 1. Re-runs the plain link's own em++ line (<plain>/link.cmd) into <plain>/../../wasm-kinds/<name>/ with
#    -sGLOBAL_BASE=1024+D and -sTABLE_BASE=1+T (emcc passes them to wasm-ld as --global-base / --table-base),
#    wasm-ld's --Map, --no-demangle and EMCC_DEBUG=1 (every intermediate module kept under …/wasm-kinds/tmp/).
#    D (1 MiB) and T (20,000) are larger than our data (1.4 MB) and table (10,007 slots), and differ, so a moved
#    constant says which of the two it is.
#    It refuses to start while an object or archive on that line is newer than the plain wasm (the relink would read
#    other objects than the plain link did), and FAILS unless its map lists the same input sections, each the same size,
#    in the same order, as the plain link's map (<plain>/../../wasm-map/<name>/, which link-map.sh proved describes the
#    plain wasm): only the addresses and the sizes of the output sections that hold a base (TABLE, GLOBAL, ELEM, DATA)
#    may differ.
# 2. Names every function of that link the way link-map.sh names the plain one: binaryen replayed on the kept
#    intermediates (tools/round/splat_names.py), each replay checked against emcc's own output.
# Writes build/out/wasm-kinds/vota_web/{vota_web_wasm.wasm,vota_web_wasm.map,names.tsv,names.sha256,link.cmd,link.log,
# replay.log} for the default plain link (names.sha256: the sha256 of the wasm names.tsv names, which splat-kinds
# checks); `splat-kinds` (make -C src link-kinds) then compares it with the plain link and writes kinds.json beside
# them.
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/../.." && pwd)
cd "$ROOT"
PLAIN=${1:-build/out/wasm/vota_web}
PLAIN_REL=${PLAIN#build/out/}
DATA_SHIFT=1048576
TABLE_SHIFT=20000
parent=$(dirname "$(dirname "$PLAIN_REL")")
KINDS_REL=$([ "$parent" = . ] && echo wasm-kinds || echo "$parent/wasm-kinds")
TMP_REL=$KINDS_REL/tmp
KINDS_REL=$KINDS_REL/$(basename "$PLAIN_REL")
OUT=build/out/$KINDS_REL
PLAIN_MAP=build/out/$([ "$parent" = . ] && echo wasm-map || echo "$parent/wasm-map")/$(basename "$PLAIN_REL")/vota_web_wasm.map
[ -f "$PLAIN/link.cmd" ] || { echo "no $PLAIN/link.cmd: run make -C src link first" >&2; exit 2; }
if [ ! -f "$PLAIN_MAP" ] || [ "$PLAIN/vota_web_wasm.wasm" -nt "$PLAIN_MAP" ]; then
  echo "no $PLAIN_MAP of the current plain link: run make -C src link-map first" >&2
  exit 2
fi
# the objects and archives the link line names (relative to build/out), newer than the link they made
newer=$(cd build/out && for f in $(cat "$ROOT/$PLAIN/link.cmd"); do
  if [[ $f == *.o || $f == *.a ]] && [ -f "$f" ] && [ "$f" -nt "$PLAIN_REL/vota_web_wasm.wasm" ]; then echo "$f"; fi
done | head -3)
if [ -n "$newer" ]; then
  echo "objects newer than $PLAIN/vota_web_wasm.wasm (run make -C src link first):" $newer >&2
  exit 2
fi
grep -q -- " -o $PLAIN_REL/vota_web_wasm.js " "$PLAIN/link.cmd" || { echo "$PLAIN/link.cmd does not write $PLAIN_REL/vota_web_wasm.js" >&2; exit 2; }
if grep -qE -- '-s(GLOBAL|TABLE)_BASE' "$PLAIN/link.cmd"; then
  echo "$PLAIN/link.cmd sets GLOBAL_BASE or TABLE_BASE itself" >&2
  exit 2
fi
mkdir -p "$OUT"
# emcc numbers its kept intermediates per run: a previous run's would be read as this run's
rm -rf "build/out/$TMP_REL"
mkdir -p "build/out/$TMP_REL"
cmd=$(sed "s# -o $PLAIN_REL/vota_web_wasm.js # -o $KINDS_REL/vota_web_wasm.js #" "$PLAIN/link.cmd")
cmd="$cmd -sGLOBAL_BASE=$((1024 + DATA_SHIFT)) -sTABLE_BASE=$((1 + TABLE_SHIFT)) -Wl,--Map=$KINDS_REL/vota_web_wasm.map -Wl,--no-demangle"
printf '%s\n' "$cmd" > "$OUT/link.cmd"
build/docker-build.sh bash -c "cd build/out && EMCC_DEBUG=1 EMCC_TEMP_DIR=/work/build/out/$TMP_REL $cmd" > "$OUT/link.log" 2>&1 \
  || { tail -20 "$OUT/link.log" >&2; exit 1; }
# the map without its address and offset columns, less the output sections a base changes the size of
inputs() { awk 'NR > 1 && !(NF == 4 && $4 ~ /^(TABLE|GLOBAL|ELEM|DATA)$/) { $1 = ""; $2 = ""; print }' "$1"; }
if ! cmp -s <(inputs "$PLAIN_MAP") <(inputs "$OUT/vota_web_wasm.map"); then
  echo "FAIL: the relink's map lists other input sections than $PLAIN_MAP: it did not link the plain link's objects" >&2
  diff <(inputs "$PLAIN_MAP") <(inputs "$OUT/vota_web_wasm.map") | head -10 >&2 || true
  exit 1
fi
echo "relink inputs the same as $PLAIN_MAP's"
python3 -I -B tools/round/splat_names.py replay-script "$OUT" "build/out/$TMP_REL" > "$OUT/replay.sh"
build/docker-build.sh bash "$OUT/replay.sh" > "$OUT/replay.log" 2>&1 || { tail -20 "$OUT/replay.log" >&2; exit 1; }
python3 -I -B tools/round/splat_names.py compose "$OUT" "build/out/$TMP_REL" "$OUT/vota_web_wasm.wasm"
# the same demangled column link-map.sh adds, so the two names.tsv read alike
tail -n +2 "$OUT/names.tsv" | cut -f2 > "$OUT/names.mangled.txt"
build/docker-build.sh bash -c "/emsdk/upstream/bin/llvm-cxxfilt < $OUT/names.mangled.txt > $OUT/names.demangled.txt"
{ printf 'demangled\n'; cat "$OUT/names.demangled.txt"; } | paste "$OUT/names.tsv" - > "$OUT/names.tsv.tmp"
mv "$OUT/names.tsv.tmp" "$OUT/names.tsv"
python3 -I -B -c 'import hashlib, sys; print(hashlib.sha256(open(sys.argv[1], "rb").read()).hexdigest())' \
  "$OUT/vota_web_wasm.wasm" > "$OUT/names.sha256"
# the kept intermediates are only the replay's input
rm -rf "build/out/$TMP_REL"
echo "shifted link (data +$DATA_SHIFT, table +$TABLE_SHIFT) named: $OUT/names.tsv"
