#!/bin/bash
# Probe build of the ASN.1 runtime (TSE's fork of III-ASN.1, reconstructed in src/asn1), pinned toolchain.
#   tools/libs/asn1/build.sh <variant> [extra compile flags...]      (from the urna root)
# <variant>  free name for the output dir build/out/asn1/<variant>; the extra flags go after the common ones
#            (-Oz -fexceptions -std=c++14 -Isrc/asn1; a later -O3 overrides the -Oz of the compile step).
# SRC=<dir>  the runtime sources to compile (default src/asn1; e.g. a patched copy of third_party/III-ASN.1/asn1).
# DRIVER=none builds the bare runtime (vtable-reachable code only); default links tools/libs/asn1/driver.cpp,
#            whose EMSCRIPTEN_KEEPALIVE roots exercise every visitor the way the generated Modulo* types do.
# LINK_EXTRA="<flags>" is appended to the link line. Output: build/out/asn1/<variant>/probe.{js,wasm,wasm.symbols}.
# Then: (cd asmlift-wasm && pnpm exec tsx ../tools/libs/census_subset.ts) target/vota_web_wasm.wasm \
#         build/out/asn1/<variant>/probe.wasm tools/libs/cls2.json asn1 --depth 0
set -euo pipefail
VARIANT=${1:?usage: build.sh <variant> [flags...]}
shift
EXTRA=("$@")
ROOT=$(cd "$(dirname "$0")/../../.." && pwd)
cd "$ROOT"
SRC=${SRC:-src/asn1}
OUT=build/out/asn1/$VARIANT
if [ -z "${INSIDE_DOCKER:-}" ]; then
  exec build/docker-build.sh env INSIDE_DOCKER=1 SRC="$SRC" DRIVER="${DRIVER:-}" LINK_EXTRA="${LINK_EXTRA:-}" bash tools/libs/asn1/build.sh "$VARIANT" ${EXTRA[@]+"${EXTRA[@]}"}
fi
mkdir -p "$OUT/obj"
COMMON=(-Oz -fexceptions -std=c++17 -I"$SRC")
COMMON+=(${EXTRA[@]+"${EXTRA[@]}"})
echo "compile flags: ${COMMON[*]}  (sources: $SRC)"
printf '%s\0' "${COMMON[@]}" > "$OUT/flags.nul"
LIST=$(mktemp)
ls "$SRC"/*.cxx > "$LIST"
[ "${DRIVER:-driver}" != none ] && echo tools/libs/asn1/driver.cpp >> "$LIST"
export OUT
# per-file extra flags: tools/libs/asn1/flags.txt lines "<basename><TAB><flags>" (e.g. the TUs that throw cannot take -fno-exceptions)
xargs -P4 -I{} bash -c 'COMMON=(); while IFS= read -r -d "" f; do COMMON+=("$f"); done < "$OUT/flags.nul"; extra=$(awk -F"\t" -v b="$(basename "$1")" "\$1==b{print \$2}" tools/libs/asn1/flags.txt 2>/dev/null); em++ "${COMMON[@]}" $extra -c "$1" -o "$OUT/obj/$(basename "$1").o"' _ {} < "$LIST"
OBJS=()
while IFS= read -r f; do OBJS+=("$OUT/obj/$(basename "$f").o"); done < "$LIST"
rm -f "$LIST"
em++ -Oz -fexceptions "${OBJS[@]}" -o "$OUT/probe.js" --no-entry -sERROR_ON_UNDEFINED_SYMBOLS=0 \
  -sALLOW_MEMORY_GROWTH=1 -sSTACK_SIZE=8MB --emit-symbol-map ${LINK_EXTRA:-}
ls -l "$OUT"/probe.wasm*
