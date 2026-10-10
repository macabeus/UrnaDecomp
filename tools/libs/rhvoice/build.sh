#!/bin/bash
# Probe build of RHVoice's core library (RHVoice_core + bundled hts_engine + sonic) from third_party/RHVoice, pinned toolchain.
#   tools/libs/rhvoice/build.sh <tag> [<variant>] [extra compile flags...]      (from the urna root)
# <tag>     the RHVoice tag third_party/RHVoice is checked out at (1.14.0, tools/setup/bootstrap.sh); the script refuses
#           another commit, a sonic submodule off the commit the tag records, and local changes to what it compiles.
# <variant> free name for the output dir build/out/rhvoice/<tag>-<variant>; the extra flags go after the common
#           ones (a later -O3 overrides the -Oz of the compile step; -std=c++NN is given to C++ files only).
# What the app links (docs/reference/facts/libraries.md §B6): src/core/*.cpp (minus the #included data files
# emoji_data.cpp, unidata.cpp, userdict_parser.c and the MAGE impl), src/hts_engine/HTS_*.c, sonic (the
# external/libs/sonic submodule, waywardgeek/sonic @ ba331411f17702e01f6c2d7016eefebaa695871f in 1.14.0).
# Config headers (upstream's SCons/CMake generate them into the source tree): CONFIG_PATH=/etc/RHVoice and
# DATA_PATH=/share/RHVoice are the target's strings; ENABLE_SONIC=1 (min_sonic_rate + no curl ⇒ ENABLE_PKG=0);
# VERSION=<tag> (no version literal is in the target). They are written to <out>/cfg, and a clang VFS overlay
# (<out>/cfg/overlay.yaml) shows them at src/core/config.h and src/include/core/config.h, so the checkout is never written.
# voice.cpp needs Boost headers (boost/algorithm/string.hpp): BOOST=<dir holding boost/> (default third_party/boost, the
# Boost 1.89.0 headers tools/setup/bootstrap.sh extracts). Defines: -DRHVOICE=1 -DPACKAGE="RHVoice" (SConstruct). Output: build/out/rhvoice/<tag>-<variant>/probe.{js,wasm,js.symbols}.
# LINK_EXTRA="<flags>" in the environment is appended to the link line (e.g. LINK_EXTRA=-flto with -flto in the compile flags).
set -euo pipefail
TAG=${1:?usage: build.sh <tag> [variant] [flags...]}
VARIANT=${2:-base}
shift; [ $# -gt 0 ] && shift
EXTRA=("$@")
ROOT=$(cd "$(dirname "$0")/../../.." && pwd)
cd "$ROOT"
SRC=third_party/RHVoice
SONIC=$SRC/external/libs/sonic
OUT=build/out/rhvoice/$TAG-$VARIANT
if [ -z "${INSIDE_DOCKER:-}" ]; then
  refuse() { echo "$SRC: $* (tools/setup/bootstrap.sh fetches it)" >&2; exit 2; }
  head=$(git -C "$SRC" rev-parse HEAD 2>/dev/null) || refuse "not a git checkout"
  [ "$head" = "$(git -C "$SRC" rev-parse --verify --quiet "$TAG^{commit}")" ] || refuse "HEAD ${head:0:12} is not $TAG"
  case "$(git -C "$SRC" submodule status external/libs/sonic)" in
    ' '*) ;;
    *) refuse "the sonic submodule is not at the commit $TAG records" ;;
  esac
  changed=$(git -C "$SRC" status --porcelain --untracked-files=no -- src external/libs/sonic)
  [ -z "$changed" ] || refuse "local changes to compiled files: $changed"
  exec build/docker-build.sh env INSIDE_DOCKER=1 LINK_EXTRA="${LINK_EXTRA:-}" BOOST="${BOOST:-}" DRIVER="${DRIVER:-}" bash tools/libs/rhvoice/build.sh "$TAG" "$VARIANT" ${EXTRA[@]+"${EXTRA[@]}"}
fi

mkdir -p "$OUT/obj" "$OUT/cfg/core"
printf 'const char CONFIG_PATH[] = "/etc/RHVoice";\nconst char DATA_PATH[] = "/share/RHVoice";\n' > "$OUT/cfg/config.h"
printf '#pragma once\n#ifndef RHVOICE_GLOBAL_CONFIG_INCLUDED\n#define RHVOICE_GLOBAL_CONFIG_INCLUDED\n#define ENABLE_SONIC 1\n#define ENABLE_PKG 0\nconst char VERSION[] = "%s";\n#endif\n' "$TAG" > "$OUT/cfg/core/config.h"
# Both headers are config.h and both are included as "config.h" (src/core/engine.cpp its own, src/include/core/engine.hpp
# its own), so no include path tells them apart: the overlay puts each in its directory. Its paths are the container's.
printf '{ "version": 0, "roots": [\n  { "type": "file", "name": "%s", "external-contents": "%s" },\n  { "type": "file", "name": "%s", "external-contents": "%s" }\n] }\n' \
  "$PWD/$SRC/src/core/config.h" "$PWD/$OUT/cfg/config.h" "$PWD/$SRC/src/include/core/config.h" "$PWD/$OUT/cfg/core/config.h" > "$OUT/cfg/overlay.yaml"

COMMON=(-Oz -fexceptions -DRHVOICE=1 '-DPACKAGE="RHVoice"' -I"$SRC/src/include" -I"$SRC/src/hts_engine" -I"$SRC/src/third-party/utf8" -I"$SRC/src/third-party/rapidxml" -I"$SONIC" -I"${BOOST:-third_party/boost}" -ivfsoverlay "$OUT/cfg/overlay.yaml")
COMMON+=(${EXTRA[@]+"${EXTRA[@]}"})
echo "compile flags: ${COMMON[*]}"

LIST=$(mktemp)
for f in "$SRC"/src/core/*.cpp "$SRC"/src/hts_engine/HTS_*.c "$SONIC"/sonic.c; do
  case "$(basename "$f")" in emoji_data.cpp|unidata.cpp|mage_hts_engine_impl.cpp) continue ;; esac
  echo "$f"
done > "$LIST"

compile_one() {
  local path=$1 obj f CFLAGS=()
  obj="$OUT/obj/$(basename "$path").o"
  if [ "$path" != "${path%.c}" ]; then
    for f in "${COMMON[@]}"; do case "$f" in -std=c++*|-std=gnu++*) ;; *) CFLAGS+=("$f") ;; esac; done
    emcc "${CFLAGS[@]}" -c "$path" -o "$obj"
  else
    em++ "${COMMON[@]}" -c "$path" -o "$obj"
  fi
}
export -f compile_one; export OUT
printf '%s\0' "${COMMON[@]}" > "$OUT/flags.nul"
xargs -P4 -I{} bash -c 'COMMON=(); while IFS= read -r -d "" f; do COMMON+=("$f"); done < "$OUT/flags.nul"; compile_one "$1"' _ {} < "$LIST"
rm -f "$LIST"
# the keep-alive driver (see driver.cpp); DRIVER=none builds the bare library
if [ "${DRIVER:-driver}" != none ]; then
  em++ "${COMMON[@]}" -c tools/libs/rhvoice/driver.cpp -o "$OUT/obj/driver.o"
fi

OBJS=("$OUT"/obj/*.o)
em++ -Oz -fexceptions "${OBJS[@]}" -o "$OUT/probe.js" --no-entry -sERROR_ON_UNDEFINED_SYMBOLS=0 \
  -sALLOW_MEMORY_GROWTH=1 -sSTACK_SIZE=8MB --emit-symbol-map ${LINK_EXTRA:-}
ls -l "$OUT"/probe.wasm*
