#!/bin/bash
# Whole-module census of the skeleton link against the target. Run from the urna root (make -C src census).
# Writes build/out/census/{census-d2.json,census-d0.json,census-d2-pairs.txt,ledger.log,skeleton-named.fns.jsonl,
# wasmfn-cmp.txt,report.md,progress.md}, ledger/functions.tsv and a snapshot ledger/history/<stamp>.tsv (the toolkit's
# `progress` prints the census tiers and their delta against the previous snapshot). Needs the vendored toolkit asmlift-wasm/ (pnpm install there once; WASM_KIT points at another copy) and wabt.
set -euo pipefail
ROOT=$(pwd)
WASM_KIT=${WASM_KIT:-$ROOT/asmlift-wasm}
W=$ROOT/target/vota_web_wasm.wasm
M=$ROOT/build/out/wasm/vota_web/vota_web_wasm.wasm
N=$ROOT/build/out/wasm-named/vota_web/vota_web_wasm.wasm
OUT=$ROOT/build/out/census
mkdir -p "$OUT"
kit() { (cd "$WASM_KIT" && pnpm exec tsx src/cli.ts "$@"); }
kit census "$W" "$M" --json > "$OUT/census-d2.json"
kit census "$W" "$M" --depth 0 --json > "$OUT/census-d0.json"
kit census "$W" "$M" --pairs > "$OUT/census-d2-pairs.txt"
# the ledger: the harness probe (libc) and the skeleton — the plain link (the byte tier needs it) named through its
# --emit-symbol-map twin (label=plain@twin: a fingerprint census between the two carries the names over)
kit ledger "$W" probe="$ROOT/build/probe/out-Oz/wasm/vota_web/vota_web_wasm.wasm" skeleton="$M@$N" -o "$ROOT/ledger/functions.tsv" 2> "$OUT/ledger.log"
# wasmfn: exact normalised bodies (addresses and call targets blinded, call_indirect by signature) of the named twin found in the target
wasm-objdump -d "$N" > "$OUT/skeleton-named.objdump.txt"
python3 -I tools/round/wasmfn.py dump "$OUT/skeleton-named.objdump.txt" "$OUT/skeleton-named.fns.jsonl" "$N"
# the target's dump is regenerated when wasmfn.py's normalisation changed (the file is newer than the dump)
[ -f build/probe/target.fns.jsonl ] && [ ! tools/round/wasmfn.py -nt build/probe/target.fns.jsonl ] \
  || python3 -I tools/round/wasmfn.py dump build/dump/vota.objdump.txt build/probe/target.fns.jsonl "$W"
python3 -I tools/round/wasmfn.py cmp "$OUT/skeleton-named.fns.jsonl" "${N%.wasm}.js.symbols" build/probe/target.fns.jsonl --all > "$OUT/wasmfn-cmp.txt"
{
  python3 -I tools/round/census_report.py "$W" "$M"
  echo; echo '## Census (toolkit)'; echo
  python3 -I - "$OUT/census-d2.json" "$OUT/census-d0.json" <<'PY'
import json, sys
for f in sys.argv[1:]:
    d = json.load(open(f)); c = d['counts']
    print(f"- `{f.split('/')[-1]}`: target {c['a']} fns, skeleton {c['b']}; fingerprint {c['byFingerprint']} + addr {c['byAddr']} + shape {c['byShape']} = {c['byFingerprint']+c['byAddr']+c['byShape']} paired; unmatched target {len(d['unmatchedA'])}, unmatched skeleton {len(d['unmatchedB'])}")
PY
  echo; echo '### Census tiers (ledger/functions.tsv, per owner class of tools/libs/cls2.json)'; echo; echo '```'; grep -v '^names for' "$OUT/ledger.log"; echo '```'
  echo; echo '### wasmfn exact bodies (named twin vs target)'; echo
  python3 -I - "$OUT/wasmfn-cmp.txt" <<'PY'
import re, sys
ex = no = big = uniq = 0
for l in open(sys.argv[1]):
    m = re.match(r'(\S+): probe func\[(\d+)\] n=(\d+) (EXACT|NO)', l)
    if not m: continue
    n = int(m.group(3))
    if m.group(4) == 'EXACT':
        ex += 1
        if n >= 8: big += 1; uniq += l.count('func[') == 2
    else: no += 1
print(f"EXACT {ex} (bodies of ≥ 8 instructions: {big}, of which exactly one target function has that body: {uniq}), NO {no}")
PY
} > "$OUT/report.md"
# the census tiers per owner and per component with the delta since the previous snapshot, then snapshot this ledger
{
  kit progress --ledger "$ROOT/ledger" --by owner --moves 40 --snapshot census 2>/dev/null
  echo
  kit progress --ledger "$ROOT/ledger" --by component --moves 0 2>/dev/null | sed -n '3,$p'
} > "$OUT/progress.md"
echo "report: $OUT/report.md  progress: $OUT/progress.md"
