#!/bin/bash
# Build one APPFLAGS probe: the reconstructed ecourna units + tools/round/appflags/driver.cpp, linked with the target's line.
# usage: tools/round/appflags/probe.sh <name> "<ecourna CXXFLAGS>" ["<driver CXXFLAGS>"]
# Output: build/out/appflags/<name>/x.{js,wasm,js.symbols}. Paths are remapped so std::source_location::file_name()
# gives the conan build path of the original (-ffile-prefix-map; measured identical to compiling at that path).
set -euo pipefail
NAME=$1; ECO_FLAGS=$2; DRV_FLAGS=${3:-$2}
ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
cd "$ROOT"
OUT=build/out/appflags/$NAME
mkdir -p "$OUT/obj"
UNITS="api/exception/cerror.cpp api/security/base64.cpp api/util/cstringutils.cpp app/dados/iidentificadoreleitor.cpp app/dados/cnumerocpf.cpp app/dados/cnumeroinscricaoeleitoral.cpp app/dados/cnumeroidentificacaolivre.cpp app/dados/cfabricaidentificadoreleitor.cpp"
MAP=-ffile-prefix-map=/work/src/ecourna=/home/rubio/.conan2/p/b/libecea1da310e5107/b/src/ecourna
CMDS=""
for u in $UNITS; do
  o=$OUT/obj/$(echo "$u" | tr / _).o
  CMDS="$CMDS em++ $ECO_FLAGS $MAP -I/work/src -c /work/src/ecourna/$u -o $o &&"
done
CMDS="$CMDS em++ $DRV_FLAGS -I/work/src -c /work/tools/round/appflags/driver.cpp -o $OUT/obj/driver.o"
OBJS="$OUT/obj/driver.o $(for u in $UNITS; do printf '%s ' "$OUT/obj/$(echo "$u" | tr / _).o"; done)"
LINK="em++ -Oz -fexceptions -sALLOW_MEMORY_GROWTH=1 -sSTACK_SIZE=8MB -sEXPORTED_FUNCTIONS=_main -sEXPORTED_RUNTIME_METHODS=ccall,FS,callMain $OBJS -o $OUT/x.js --emit-symbol-map"
echo "$CMDS && $LINK" > "$OUT/cmd.sh"
build/docker-build.sh bash -c "$CMDS && $LINK"
ls -la "$OUT/x.wasm" | awk '{print $5, $9}'
