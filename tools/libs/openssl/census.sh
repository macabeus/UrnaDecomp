#!/bin/bash
# Link a probe (link-probe.sh, honouring ROOTS/EXTRA_EXPORTS), dump + name its functions, run the @asmlift/wasm census
# against the target and write census JSON + summary into the probe dir.
#   usage: tools/libs/openssl/census.sh <variant> [tag]
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
URNA="$(cd "$HERE/../../.." && pwd)"
WT="$URNA/asmlift-wasm"
W="$URNA/target/vota_web_wasm.wasm"
variant="${1:?variant}"; tag="${2:-}"
P="build/out/openssl/$variant-probe${tag:+-$tag}"
cd "$URNA"
"$HERE/link-probe.sh" "$variant" ${tag:+"$tag"} 2>&1 | grep -v Wdeprecated
docker run --rm -v "$PWD:/work" -w "/work/$P" urna-emsdk:6.0.0 bash -c 'wasm-objdump -d probe.wasm > probe.objdump.txt; wasm-objdump -d probe-map.wasm > probe-map.objdump.txt; wasm-objdump -x probe.wasm > probe.objdump-x.txt'
python3 -I tools/round/wasmfn.py dump "$P/probe.objdump.txt" "$P/probe.fns.jsonl" >/dev/null
python3 -I tools/round/wasmfn.py dump "$P/probe-map.objdump.txt" "$P/probe-map.fns.jsonl" >/dev/null
python3 -I "$HERE/name-probe.py" "$P"
(cd "$WT" && pnpm exec tsx src/cli.ts census "$W" "$URNA/$P/probe.wasm" --json > "$URNA/$P/census.json" 2>/dev/null)
(cd "$WT" && pnpm exec tsx src/cli.ts census "$W" "$URNA/$P/probe.wasm" --depth 0 --json > "$URNA/$P/census-d0.json" 2>/dev/null)
python3 - "$P" "$URNA/tools/libs/cls2.json" <<'PY'
import json, sys, collections
cls = {int(k): v for k, v in json.load(open(sys.argv[2])).items()}
for depth, f in (('depth 2', 'census.json'), ('depth 0', 'census-d0.json')):
    cen = json.load(open(f'{sys.argv[1]}/{f}'))
    c = collections.Counter(); byk = collections.Counter()
    for p in cen['pairs']:
        byk[p['by']] += 1
        if cls.get(p['a']) == 'openssl': c[p['by']] += 1
    print(f"{depth}: target openssl-class paired {sum(c.values())}/2183 {dict(c)}; all {len(cen['pairs'])} pairs {dict(byk)}; probe fns {cen['counts']['b']}")
PY
