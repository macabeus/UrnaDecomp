#!/bin/bash
# Recompile a few libcrypto sources with extra CFLAGS, relink the probe of a given root-mode probe dir with those objects
# swapped in, and emit a symbol map so the functions can be scored by name against the target.
#   usage: tools/libs/openssl/flag-sweep.sh <variant> <probe-tag> <out-tag> "<extra cflags>" <src.c>...
#   out:   build/out/openssl/<variant>-sweep-<out-tag>/{probe.wasm,probe.js.symbols,objs.txt}
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"; URNA="$(cd "$HERE/../../.." && pwd)"
variant="$1"; ptag="$2"; otag="$3"; extra="$4"; shift 4
src="build/out/openssl/$variant"; P="build/out/openssl/$variant-probe-$ptag"; O="build/out/openssl/$variant-sweep-$otag"
cd "$URNA"; mkdir -p "$O"
docker run --rm -v "$PWD:/work" -w "/work/$src" -e EXTRA="$extra" -e P="/work/$P" -e O="/work/$O" urna-emsdk:6.0.0 bash -c '
  set -e
  cp "$P/objs.txt" "$O/objs.txt"
  base=$(grep -E "^CFLAGS=" Makefile | sed "s/^CFLAGS=//")
  for s in "$@"; do
    o=$(ls $(dirname "$s")/*-lib-$(basename "$s" .c).o 2>/dev/null | head -1); if [ -z "$o" ]; then echo "no object for $s" >&2; continue; fi
    line=$(grep -A1 "^$o:" Makefile | tail -1 | sed "s/^\t//; s/\$(CC)/emcc/; s/\$(LIB_CFLAGS)/$base $EXTRA/; s/\$(LIB_CPPFLAGS)/-DOPENSSLDIR=\"\\\\\"\/etc\/ssl\\\\\"\" -DENGINESDIR=\"\\\\\"\/\/lib\/engines-3\\\\\"\" -DMODULESDIR=\"\\\\\"\/\/lib\/ossl-modules\\\\\"\" -DOPENSSL_BUILDING_OPENSSL -DNDEBUG/; s/-MMD -MF [^ ]*//")
    mkdir -p "$O/$(dirname "$o")"
    eval "${line/-o \$@/-o $O/$o}"
    if grep -q "^$o\$" "$O/objs.txt"; then sed -i "s#^$o\$#$O/$o#" "$O/objs.txt"; else echo "$O/$o" >> "$O/objs.txt"; fi
  done
  em++ -Oz -fexceptions "$P/probe.c" $(cat "$O/objs.txt") libcrypto.a -sALLOW_MEMORY_GROWTH=1 -sSTACK_SIZE=8MB "-sEXPORTED_FUNCTIONS=@$P/exports.json" -o "$O/probe.js" --emit-symbol-map 2>&1 | grep -v Wdeprecated || true
  ls -la "$O/probe.wasm"
' -- "$@"
