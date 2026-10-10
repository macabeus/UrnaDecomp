#!/bin/bash
# Census of one build variant against the target. Run from the urna root (host, not docker):
#   tools/libs/sqlite/census.sh <name> [scratch-dir]
# Needs wasm-objdump (brew wabt) and the vendored toolkit asmlift-wasm/.
set -euo pipefail
NAME=${1:?name}
S=${2:-build/out/sqlite/census-work}
TK=$PWD/asmlift-wasm
W=$PWD/target/vota_web_wasm.wasm
O=$PWD/build/out/sqlite/$NAME
mkdir -p "$S"
[ -f "$S/target.fns.jsonl" ] || python3 -I tools/round/wasmfn.py dump build/dump/vota.objdump.txt "$S/target.fns.jsonl"
[ -f "$S/target.functions.jsonl" ] || (cd "$TK" && pnpm exec tsx src/cli.ts parse "$W" --functions > "$S/target.functions.jsonl")
for k in plain named; do
  wasm-objdump -d "$O/$k/wasm/vota_web/vota_web_wasm.wasm" > "$S/$NAME.$k.objdump.txt"
  python3 -I tools/round/wasmfn.py dump "$S/$NAME.$k.objdump.txt" "$S/$NAME.$k.fns.jsonl" >/dev/null
done
(cd "$TK" && pnpm exec tsx src/cli.ts parse "$O/plain/wasm/vota_web/vota_web_wasm.wasm" --functions > "$S/$NAME.plain.functions.jsonl")
(cd "$TK" && pnpm exec tsx src/cli.ts census "$W" "$O/plain/wasm/vota_web/vota_web_wasm.wasm" --depth 0 --json > "$S/$NAME.census0.json" 2>/dev/null)
python3 -I tools/libs/sqlite/census.py "$S" "$NAME" "$W" "$O/plain/wasm/vota_web/vota_web_wasm.wasm" "$O/named/wasm/vota_web/vota_web_wasm.js.symbols" tools/libs/cls2.json "$O/census.json"
