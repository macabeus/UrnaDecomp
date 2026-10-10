#!/bin/bash
# Regenerate the Modulo* ASN.1 modules from the schema in the target's data: build/out/asn1-schema/modules/ by
# default, or the directory given. Never point it at src/asn1/modules without reviewing the diff: the committed
# modules carry hand edits made after generation (src/asn1/README.md).
set -euo pipefail
cd "$(dirname "$0")/../../.."
W=build/out/asn1-schema
mkdir -p "$W"
[ -f build/dump/vota.objdump.txt ] || { echo "missing build/dump/vota.objdump.txt (wasm-objdump -d target/vota_web_wasm.wasm)"; exit 2; }
python3 -I tools/libs/memimg.py target/vota_web_wasm.wasm "$W/mem.bin" > /dev/null
python3 -I tools/extract/asn1/consts.py           # i32/i64 constants per function that look like data addresses
python3 -I tools/extract/asn1/bssinit.py          # mem-init.bin: mem.bin plus the static initialiser's constant stores
python3 -I tools/extract/asn1/schema.py           # schema-seq.json: the SEQUENCE records with RTTI
python3 -I tools/extract/asn1/schema2.py          # schema.json: every record reachable from them
grep -o -E 'Modulo[A-Za-z0-9]+(::[A-Za-z0-9_]+)+' ledger/source-locations.tsv | LC_ALL=C sort -u > "$W/modulo-names-in-code.txt"
python3 -I tools/extract/asn1/gen_modules.py "${1:-$W/modules}"
