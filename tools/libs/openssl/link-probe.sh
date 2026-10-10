#!/bin/bash
# Link a probe module from a built variant: the 346 libcrypto members the target is known to contain
# (tools/libs/ossl_files.txt, ERR_raise __FILE__ evidence) as explicit objects, the rest of libcrypto.a by
# reference, with the target's link flags (-Oz -fexceptions, emsdk 6.0.0).  Every function those members
# define is exported so wasm-opt keeps it.  With ROOTS=<file> (one symbol per line, from derive-roots.py) nothing but
# main is exported and main CALLS each root once (gen-probe-main.py), which is how the app keeps libcrypto alive — an
# exported function is never inlined-and-removed by binaryen, a called one behaves as in the app.  EXTRA_EXPORTS=<file>
# adds symbols to keep exported besides main (derive-roots.py's "unknown entry points").
#   usage: tools/libs/openssl/link-probe.sh <variant> [tag]
#   out:   build/out/openssl/<variant>-probe[-<tag>]/{probe.wasm,probe.js,probe-map.wasm,probe-map.js.symbols,objs.txt,exports.json}
# probe-map.* is the SAME link with --emit-symbol-map, which only names functions: its function order differs
# from probe.wasm (names survive into wasm-opt's reorder), so bodies are paired by fingerprint, never by index.
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
URNA="$(cd "$HERE/../../.." && pwd)"
variant="${1:?variant}"; tag="${2:-}"
src="build/out/openssl/$variant"
out="build/out/openssl/$variant-probe${tag:+-$tag}"
cd "$URNA"
mkdir -p "$out"
: > "$out/objs.txt"
while read -r f; do
  case "$f" in *.c) ;; *) continue;; esac
  d="$(dirname "$f")"; b="$(basename "$f" .c)"
  o="$(ls "$src/$d/"*"-lib-$b.o" 2>/dev/null | head -1 || true)"
  if [ -z "$o" ]; then echo "no object for $f" >&2; continue; fi
  echo "${o#$src/}" >> "$out/objs.txt"
done < tools/libs/ossl_files.txt
echo "objects: $(wc -l < "$out/objs.txt")"
cat > "$out/probe.c" <<'EOC'
int main(void) { return 0; }
EOC
ROOTS="${ROOTS:-}"; EXTRA_EXPORTS="${EXTRA_EXPORTS:-}"
docker run --rm -v "$PWD:/work" -w "/work/$src" -e ROOTS="$ROOTS" -e EXTRA_EXPORTS="$EXTRA_EXPORTS" -e OUT="/work/$out" urna-emsdk:6.0.0 bash -c '
  set -e
  if [ -n "$ROOTS" ]; then
    for o in $(find . -name "*.o"); do wasm-objdump -x "$o"; done > "$OUT/objdump-x.txt" 2>/dev/null
    python3 -I /work/tools/libs/openssl/gen-probe-main.py "/work/$ROOTS" "$OUT/objdump-x.txt" "$OUT/probe.c"
    if [ -n "$EXTRA_EXPORTS" ]; then
      python3 -c "import json,sys; print(json.dumps([\"_main\"]+[\"_\"+l.strip() for l in open(sys.argv[1]) if l.strip()]))" "/work/$EXTRA_EXPORTS" > "$OUT/exports.json"
    else
      echo "[\"_main\"]" > "$OUT/exports.json"
    fi
  else
    /emsdk/upstream/bin/llvm-nm --defined-only -g $(cat "$OUT/objs.txt") 2>/dev/null | awk "\$2==\"T\"{print \$3}" | sort -u > "$OUT/defined-functions.txt"
    python3 -c "import json,sys; print(json.dumps([\"_main\"]+[\"_\"+l.strip() for l in open(sys.argv[1]) if l.strip()]))" "$OUT/defined-functions.txt" > "$OUT/exports.json"
  fi
  echo "exports: $(python3 -c "import json,sys; print(len(json.load(open(sys.argv[1]))))" "$OUT/exports.json")"
  common=(-Oz -fexceptions "$OUT/probe.c" $(cat "$OUT/objs.txt") libcrypto.a -sALLOW_MEMORY_GROWTH=1 -sSTACK_SIZE=8MB "-sEXPORTED_FUNCTIONS=@$OUT/exports.json")
  time em++ "${common[@]}" -o "$OUT/probe.js"
  em++ "${common[@]}" -o "$OUT/probe-map.js" --emit-symbol-map
  ls -la "$OUT"/probe*.wasm
'
