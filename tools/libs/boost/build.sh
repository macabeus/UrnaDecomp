#!/bin/bash
# Probe build of the header-only Boost parts the app uses, from third_party/boost, with the pinned toolchain.
#   tools/libs/boost/build.sh <version> [<variant>] [extra compile flags...]      (from the urna root)
# <version>  the Boost release third_party/boost holds (1.89.0: the headers tools/setup/bootstrap.sh extracts there);
#            the script reads it from boost/version.hpp and refuses another. A probe of another release (1.90.0,
#            1.91.0) puts that release's boost/ (https://archives.boost.io/release/<version>/source/) in third_party/boost.
# <variant>  free name for the output dir build/out/boost/<version>-<variant>; extra flags after the common ones.
# Source: tools/libs/boost/probe.cpp. Output: build/out/boost/<version>-<variant>/probe.{js,wasm,js.symbols}.
# LINK_EXTRA="<flags>" in the environment is appended to the link line (e.g. LINK_EXTRA=-flto with -flto in the compile flags).
set -euo pipefail
VER=${1:?usage: build.sh <version> [variant] [flags...]}
VARIANT=${2:-base}
shift; [ $# -gt 0 ] && shift
EXTRA=("$@")
ROOT=$(cd "$(dirname "$0")/../../.." && pwd)
cd "$ROOT"
HDR=third_party/boost
OUT=build/out/boost/$VER-$VARIANT
if [ -z "${INSIDE_DOCKER:-}" ]; then
  have=$(sed -n 's/^#define BOOST_VERSION \([0-9]*\)$/\1/p' "$HDR/boost/version.hpp" 2>/dev/null || true)
  [ -n "$have" ] && have=$((have / 100000)).$((have / 100 % 1000)).$((have % 100))
  [ "$have" = "$VER" ] || { echo "$HDR holds Boost ${have:-nothing}, not $VER (tools/setup/bootstrap.sh fetches 1.89.0)" >&2; exit 2; }
  exec build/docker-build.sh env INSIDE_DOCKER=1 LINK_EXTRA="${LINK_EXTRA:-}" bash tools/libs/boost/build.sh "$VER" "$VARIANT" ${EXTRA[@]+"${EXTRA[@]}"}
fi
mkdir -p "$OUT"
COMMON=(-Oz -fexceptions -I"$HDR")
COMMON+=(${EXTRA[@]+"${EXTRA[@]}"})
echo "compile flags: ${COMMON[*]}"
grep -E 'define BOOST_LIB_VERSION' "$HDR/boost/version.hpp"
em++ "${COMMON[@]}" -c tools/libs/boost/probe.cpp -o "$OUT/probe.o"
em++ -Oz -fexceptions "$OUT/probe.o" -o "$OUT/probe.js" --no-entry -sERROR_ON_UNDEFINED_SYMBOLS=0 \
  -sALLOW_MEMORY_GROWTH=1 -sSTACK_SIZE=8MB --emit-symbol-map ${LINK_EXTRA:-}
ls -l "$OUT"/probe.wasm*
