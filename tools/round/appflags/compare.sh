#!/bin/bash
# Score every pair in pairs.tsv against one probe: tools/round/appflags/compare.sh <name> [--full <target_fn>]
set -euo pipefail
NAME=$1; shift || true
ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
cd "$ROOT/asmlift-wasm" && pnpm --silent exec tsx "$ROOT/tools/round/appflags/score_pairs.ts" "$ROOT/target/vota_web_wasm.wasm" "$ROOT/build/out/appflags/$NAME/x.wasm" "$ROOT/tools/round/appflags/pairs.tsv" "$@" 2> >(grep -v '^names for' >&2)
