#!/bin/bash
# Build the probe exactly the way the target was linked. Run inside the image (see ../docker-build.sh):
#   cd build/probe && ../docker-build.sh bash build.sh -Oz
# -O level is the only free argument; the target was linked at -Oz (see README "Opt level").
set -e
O=${1:--Oz}
TAG=${O#-}
mkdir -p out-$TAG/wasm/vota_web
cd out-$TAG
em++ $O -fexceptions -c ../probe.cpp -o probe.o
em++ $O -fexceptions probe.o -o wasm/vota_web/vota_web_wasm.js \
  -sALLOW_MEMORY_GROWTH=1 -sSTACK_SIZE=8MB \
  -sEXPORTED_FUNCTIONS=_main,_probeExported \
  -sEXPORTED_RUNTIME_METHODS=ccall,FS,callMain \
  --preload-file ../assets@/ --emit-symbol-map
ls -la wasm/vota_web
echo "--- libc++abi __FILE__ strings in the probe:"
strings -n 8 wasm/vota_web/vota_web_wasm.wasm | grep -E '^/emsdk' | sort -u
