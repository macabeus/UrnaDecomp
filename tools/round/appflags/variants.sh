#!/bin/bash
# Score source VARIANTS of one target function: <file.cpp> defines functions V1..Vn with the target's C++ signature
# and a first line `// TARGET <index> <call expression using V( and ARG>`. Each variant is linked on its own (so
# binaryen cannot merge identical variants) with a main that calls it through a volatile selector.
# usage: tools/round/appflags/variants.sh <file.cpp> "<CXXFLAGS>"
set -euo pipefail
SRC=$1; FLAGS=$2
ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"; cd "$ROOT"
NAME=var-$(basename "$SRC" .cpp)
OUT=build/out/appflags/$NAME; mkdir -p "$OUT"
TARGET=$(sed -n '1s/^\/\/ TARGET \([0-9]*\).*/\1/p' "$SRC")
CALL=$(sed -n '1s/^\/\/ TARGET [0-9]* //p' "$SRC")
N=$(grep -cE '^[A-Za-z_:<>, ]+ V[0-9]+\(' "$SRC")
grep -E '^[A-Za-z_:<>, ]+ V[0-9]+\(' "$SRC" | sed 's/$/;/' > "$OUT/variants.hpp"
MAP=-ffile-prefix-map=/work/src/ecourna=/home/rubio/.conan2/p/b/libecea1da310e5107/b/src/ecourna
LINKS=""
for i in $(seq 1 "$N"); do
  {
    echo '#include <string>'; echo '#include <vector>'; echo '#include <cstdio>'; echo '#include "variants.hpp"'
    echo 'int main(int argc, char** argv) {'
    echo '  std::string ARG = argc > 1 ? argv[1] : "  12ab ";'
    echo '  volatile int sel = argc; std::printf("%d\n", sel);'
    echo "  if (sel == 1) { $(echo "$CALL" | sed "s/V(/V$i(/g"); }"
    echo "  if (sel == 2) { $(echo "$CALL" | sed "s/V(/V$i(/g"); }"
    echo "  if (sel == 3) { volatile auto fp = &V$i; std::printf(\"%p\\n\", (void*)fp); }"
    echo '  return 0; }'
  } > "$OUT/main$i.cpp"
  LINKS="$LINKS em++ -Oz -std=c++20 -I/work/$OUT -c /work/$OUT/main$i.cpp -o $OUT/main$i.o && em++ -Oz -fexceptions -sALLOW_MEMORY_GROWTH=1 -sSTACK_SIZE=8MB $OUT/main$i.o $OUT/v.o -o $OUT/x$i.js --emit-symbol-map &"
done
build/docker-build.sh bash -c "em++ $FLAGS $MAP -I/work/src -c /work/$SRC -o $OUT/v.o && { $LINKS wait; }" 2>&1 | grep -v 'warning\|^ *[0-9]* |\|\^\|generated' || true
for i in $(seq 1 "$N"); do
  printf '%s\tV%d(\tvariant V%d\n' "$TARGET" "$i" "$i" > "$OUT/pairs$i.tsv"
  (cd "$ROOT/asmlift-wasm" && pnpm --silent exec tsx "$ROOT/tools/round/appflags/score_pairs.ts" "$ROOT/target/vota_web_wasm.wasm" "$ROOT/$OUT/x$i.wasm" "$ROOT/$OUT/pairs$i.tsv" 2>/dev/null | grep -v identical)
done
