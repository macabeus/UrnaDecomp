#!/bin/bash
# Try candidate TUs before they land: compile them with the flags of the component each belongs to,
# link the WHOLE skeleton with them, census, and print the delta against the previous census (ledger/functions.tsv).
#
#   tools/round/try-tu.sh <tu.cpp|dir>... [--name <n>] [--flags '<extra compile flags>'] [--targets <indices.txt>]
#                       [--component <dir under src/>] [--keep-weak] [--roots <roots.cpp>|none] [--rows N] [--no-named]
#                       [--skip-units <frag.mk>]  (repeatable: leave a landed src/units fragment out of the link)
#   tools/round/try-tu.sh --single <target fn> --extract export:<name>|index:<n> [--component <dir>] [--flags '…'] <tu.cpp>...
#   tools/round/try-tu.sh --print-flags [--component <dir>] <tu.cpp|dir>...   (each TU's compile command, nothing run)
#   tools/round/try-tu.sh --symbols [--component <dir>] [--flags '…'] <tu.cpp|dir>...   (compile; print each defined
#                       function symbol as <mangled> TAB <demangled>: the name --extract export:<mangled> takes)
#
# Whole-link mode writes build/out/try/<name>/{obj/,wasm/,wasm-named/,functions.tsv,targets.txt,report.md} and never touches
# ledger/ (you decide what lands in src/; `make -C src census` then moves the ledger). Each TU is compiled with the
# flags src/Makefile builds that source with, read at run time from the recipe of the object it would make (`make -n`):
# the component's rule (ecourna, uenux2, the asn1 runtime and modules, app) with its target-specific additions, or the
# landed unit's own rule in src/units/, plus --flags. A TU's path under src/ names its component; a TU outside src/
# needs --component, the directory under src/ it would land in (ecourna/app/dados, uenux2/src/api/util, asn1, app), which
# also overrides the path of every TU inside src/ (so it is how a src/units/*-pending.cpp, which has no rule here, gets
# compiled with the unit's flags). Library objects are the prebuilt ones (make rebuilds nothing of theirs).
# The candidate's functions are kept alive by a generated roots TU (tools/round/gen_try_roots.py, compiled with
# APP_CXXFLAGS) unless --roots gives your own callers (APP_CXXFLAGS + APP_INCLUDES + --flags) or `none`.
# Target functions the report details: --targets <file of target indices>, else every target function whose best pair
# in the try link is a function the TU(s) define (by symbol name through the --emit-symbol-map twin; --no-named skips the twin link
# and the names). They are what the TU is expected to produce, not splat matches (splat/matched.txt).
# A census tier of `none` does not mean the body differs. Map the try link and derive its operand kinds with
#   tools/round/link-map.sh build/out/try/<name>/wasm/vota_web
#   tools/round/link-kinds.sh build/out/try/<name>/wasm/vota_web
#   (asmlift-wasm/) pnpm wasm splat-kinds ../build/out/try/<name>/wasm/vota_web/vota_web_wasm.wasm
# and compare one function with `pnpm wasm splat-remap <target> build/out/try/<name>/wasm/vota_web/vota_web_wasm.wasm
# sym:<mangled> --against <fn>` (or skip the kinds with --no-kinds; docs/how-to-decompile.md step 5).
# --single: no link of the skeleton; the toolkit's rank adapter compiles the TU alone and scores one exported function
# (a candidate linked alone has other inlining/DAE decisions than inside the app — use it to iterate, not to conclude).
# Several <tu.cpp> are rival candidates ranked against each other, not one program: a callee in another TU stays an
# import. Undefined functions become imports but undefined data fails the link; pass
# --flags '-Wl,--unresolved-symbols=ignore-all' for a TU that uses another TU's data (theInfo, vtables, typeinfo).
# The --single template always compiles with -fexceptions; uenux2 is built without it (no landing pads), so pass
# --flags '-fignore-exceptions' for a uenux2 TU, or its score counts invoke_* thunks the build never has.
# A uenux2 TU with std::source_location records and no fragment yet compiles from its relative path, which
# UENUX2_PREFIX_MAP does not rewrite: pass --flags '-ffile-prefix-map=src/uenux2=/home/rubio/tse/uenux2' so the
# try link's records name the target's path (docs/how-to-decompile.md steps 4 and 5).
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/../.." && pwd)
cd "$ROOT"
WASM_KIT=${WASM_KIT:-$ROOT/asmlift-wasm}
W=$ROOT/target/vota_web_wasm.wasm
PROBE=$ROOT/build/probe/out-Oz/wasm/vota_web/vota_web_wasm.wasm
IMAGE=urna-emsdk:6.0.0
kit() { (cd "$WASM_KIT" && pnpm exec tsx src/cli.ts "$@"); }
t0=$(date +%s)
elapsed() { echo "$(( $(date +%s) - t0 )) s"; }

SKIP_UNITS=${SKIP_UNITS:-}
name=; flags=; component=; targets=; keep_weak=; roots=; rows=12; named=1; single=; extract=; print_flags=; symbols=; srcs=()
while [ $# -gt 0 ]; do
  case "$1" in
    --name) name=$2; shift 2;;
    --flags) flags=$2; shift 2;;
    --component) component=${2%/}; shift 2;;
    --targets) targets=$2; shift 2;;
    --keep-weak) keep_weak=--keep-weak; shift;;
    --roots) roots=$2; shift 2;;
    --rows) rows=$2; shift 2;;
    --no-named) named=; shift;;
    --skip-units) SKIP_UNITS="$SKIP_UNITS $2"; shift 2;;
    --single) single=$2; shift 2;;
    --extract) extract=$2; shift 2;;
    --print-flags) print_flags=1; shift;;
    --symbols) symbols=1; shift;;
    -h|--help) sed -n '/^set -euo/q; 2,$p' "$0"; exit 0;;
    --*) echo "unknown option $1" >&2; exit 2;;
    *) srcs+=("$1"); shift;;
  esac
done
[ ${#srcs[@]} -gt 0 ] || { echo "usage: try-tu.sh <tu.cpp|dir>... (see the header)" >&2; exit 2; }

# the TU list: files as given, directories expanded to their *.cpp/*.cc/*.c in sorted order
tus=()
for s in "${srcs[@]}"; do
  if [ -d "$s" ]; then
    while IFS= read -r f; do tus+=("$f"); done < <(find "$s" -maxdepth 1 \( -name '*.cpp' -o -name '*.cc' -o -name '*.c' \) | sort)
  else
    tus+=("$s")
  fi
done
[ -n "$name" ] || { name=$(basename "${srcs[0]}"); name=${name%.*}; }

APP_CXXFLAGS=$(make -s -C src print-APP_CXXFLAGS)
APP_INCLUDES=$(make -s -C src print-APP_INCLUDES)
read -r APP ASN1_DIR ASN1MOD_DIR <<< "$(make -s -C src print-APP print-ASN1_DIR print-ASN1MOD_DIR | tr '\n' ' ')"

T=build/out/try/$name
mkdir -p "$T/obj" "$T/src"
# relative-to-root path of a source, copied under build/out/try when it lives outside the tree (the container sees /work only)
rel() {
  local p; p=$(cd "$(dirname "$1")" && pwd)/$(basename "$1")
  case "$p" in
    "$ROOT"/*) echo "${p#"$ROOT"/}";;
    *) cp "$p" "$T/src/$(basename "$p")"; echo "$T/src/$(basename "$p")";;
  esac
}

# the object src/Makefile builds from a root-relative source path, by the naming of its compile rules
object_of() {
  case "$1" in
    src/ecourna/*) local q=${1#src/ecourna/}; echo "$APP/ecourna/${q//\//_}.o";;
    src/uenux2/*) echo "$APP/uenux2/${1#src/uenux2/}.o";;
    src/asn1/modules/*.cxx) echo "$ASN1MOD_DIR/$(basename "$1" .cxx).o";;
    src/asn1/*.cxx) echo "$ASN1_DIR/$(basename "$1" .cxx).o";;
    src/app/*.cpp|src/web/*.cpp) echo "$APP/$(basename "$1" .cpp).o";;
    *) return 1;;
  esac
}

# the *_TRY switches (DADOS_MIDIAS_TRY=1, …) drop a fragment's rules along with its units; a TU re-tried that way still gets
# the flags of its landed rule, so they stay out of the flags query
query_env=(-u SKIP_UNITS)
for v in $(compgen -e); do case "$v" in *_TRY) query_env+=(-u "$v");; esac; done

# compile_of <tu>: sets tu_cc, tu_flags and tu_path (what the container compiles, spelled /work/… or root-relative as
# the rule spells its source) from the recipe `make -n` prints for the object the source builds. The source is declared
# a target with no recipe so the pattern rules match a candidate that does not exist at that path. A TU that is not at
# that path is copied there under $T, and the rule's -ffile-prefix-map follows it.
compile_of() {
  local f=$1 p r src obj line b
  p=$(cd "$(dirname "$f")" && pwd)/$(basename "$f")
  r=${p#"$ROOT"/}
  if [ -n "$component" ]; then
    b=$(basename "$f")
    case "$component" in asn1|asn1/*) b=${b%.*}.cxx;; esac
    src=src/$component/$b
  else
    case "$r" in
      src/*) src=$r;;
      *) echo "$f is outside src/: give --component <dir under src/ it would land in> (ecourna/app/dados, uenux2/src/api/util, asn1, app)" >&2
         exit 2;;
    esac
  fi
  obj=$(object_of "$src") || { echo "$src: no src/Makefile compile rule builds this path (ecourna/, uenux2/, asn1/*.cxx, asn1/modules/*.cxx, app/, web/)" >&2; exit 2; }
  printf '%s: ;\n' "$ROOT/$src" > "$T/flags-query.mk"
  line=$(env "${query_env[@]}" make -s -n -B -C src -f Makefile -f "$ROOT/$T/flags-query.mk" "$ROOT/$obj" \
    | grep -E 'docker-build\.sh (em\+\+|emcc) .* -c [^ ]+ -o ' | tail -1) || true
  [ -n "$line" ] || { echo "$src: make prints no compile recipe for $obj" >&2; exit 2; }
  tu_cc=$(sed -E 's/.*docker-build\.sh (em\+\+|emcc) .*/\1/' <<< "$line")
  tu_flags=$(sed -E 's/.*docker-build\.sh (em\+\+|emcc) (.*) -c [^ ]+ -o .*/\2/' <<< "$line")
  tu_path=$src
  if [ "$r" != "$src" ]; then
    mkdir -p "$T/$(dirname "$src")" && cp "$p" "$T/$src"
    tu_path=$T/$src
    tu_flags=$(sed -e "s#-ffile-prefix-map=/work/src/#-ffile-prefix-map=/work/$T/src/#g" -e "s#-ffile-prefix-map=src/#-ffile-prefix-map=$T/src/#g" <<< "$tu_flags")
  fi
  case "$line" in *" -c /work/"*) tu_path=/work/$tu_path;; esac
}

if [ -n "$print_flags" ]; then
  for f in "${tus[@]}"; do compile_of "$f"; echo "$f: $tu_cc $tu_flags${flags:+ $flags} -c $tu_path"; done
  exit 0
fi

if [ -n "$symbols" ]; then
  for f in "${tus[@]}"; do
    compile_of "$f"
    o=$T/obj/$(basename "${f%.*}").o
    # shellcheck disable=SC2086
    build/docker-build.sh $tu_cc $tu_flags $flags -c "$tu_path" -o "$o" > "$o.log" 2>&1 || { cat "$o.log" >&2; exit 1; }
    build/docker-build.sh bash -c "/emsdk/upstream/bin/llvm-nm --defined-only $o | awk '\$2 ~ /^[TW]\$/ {print \$3}' | LC_ALL=C sort -u > $o.symbols \
      && /emsdk/upstream/bin/llvm-cxxfilt < $o.symbols | paste $o.symbols -"
  done
  exit 0
fi

if [ -n "$single" ]; then
  [ -n "$extract" ] || { echo "--single needs --extract export:<name>|index:<n>" >&2; exit 2; }
  single_flags=
  for f in "${tus[@]}"; do
    compile_of "$f"
    [ -z "$single_flags" ] || [ "$tu_flags" = "$single_flags" ] || { echo "--single: $f compiles with other flags than ${tus[0]}" >&2; exit 2; }
    single_flags=$tu_flags
  done
  # the rank template mounts the urna root at /work so the rule's root-relative include paths resolve
  single_flags=$(sed -E 's#(^| )-I([^/ ])#\1-I/work/\2#g' <<< "$single_flags")
  echo "flags: $single_flags${flags:+ $flags}"
  tmpl="docker run --rm -v /tmp:/tmp -v $ROOT:/work -w /tmp $IMAGE em++ -Oz -fexceptions -sALLOW_MEMORY_GROWTH=1 -sSTACK_SIZE=8MB {{flags}} {{inputPath}} -o {{outputPath}}.js --no-entry -sERROR_ON_UNDEFINED_SYMBOLS=0 -sEXPORTED_FUNCTIONS=_{{symbol}} && mv {{outputPath}}.wasm {{outputPath}}"
  abs=(); for f in "${tus[@]}"; do abs+=("$(cd "$(dirname "$f")" && pwd)/$(basename "$f")"); done
  kit rank "$W" "$single" --extract "$extract" --template "$tmpl" --flags "$single_flags${flags:+ $flags}" "${abs[@]}"
  echo "single mode: $(elapsed)"
  exit 0
fi

# ---- 1. compile the TUs (≤ 4 at once) ----------------------------------------------------------------------------
objs=()
pids=()
tu_lines=()
for f in "${tus[@]}"; do
  compile_of "$f"
  o=$T/obj/$(basename "${f%.*}").o
  objs+=("$o")
  tu_lines+=("$f: $tu_cc $tu_flags${flags:+ $flags}")
  echo "flags: ${tu_lines[${#tu_lines[@]}-1]}"
  # shellcheck disable=SC2086
  build/docker-build.sh $tu_cc $tu_flags $flags -c "$tu_path" -o "$o" > "$o.log" 2>&1 &
  pids+=($!)
  if [ ${#pids[@]} -ge 4 ]; then wait "${pids[0]}" || { cat "$o.log"; exit 1; }; pids=("${pids[@]:1}"); fi
done
fail=0
for p in "${pids[@]}"; do wait "$p" || fail=1; done
for o in "${objs[@]}"; do
  if [ ! -s "$o" ]; then echo "compile failed: $o"; cat "$o.log"; fail=1; fi
  [ -s "$o.log" ] && { echo "-- $(basename "$o"):"; cat "$o.log"; }
done
[ $fail = 0 ] || exit 1
echo "compiled ${#objs[@]} object(s): $(elapsed)"

# ---- 2. the roots TU ---------------------------------------------------------------------------------------------
case "$roots" in
  none) ;;
  '') python3 -I tools/round/gen_try_roots.py "$T/obj/try_roots.cpp" $keep_weak "${objs[@]}"
      build/docker-build.sh em++ $APP_CXXFLAGS -c "$T/obj/try_roots.cpp" -o "$T/obj/try_roots.o"
      objs+=("$T/obj/try_roots.o");;
  *)  r=$(rel "$roots"); build/docker-build.sh em++ $APP_CXXFLAGS $APP_INCLUDES $flags -c "$r" -o "$T/obj/try_roots.o"
      objs+=("$T/obj/try_roots.o");;
esac
# the TU's own function symbols, DEMANGLED like emcc's symbol map spells them (for the automatic targets and the unmatched list)
: > "$T/symbols.txt"
for o in "${objs[@]}"; do
  case "$o" in */try_roots.o) continue;; esac
  build/docker-build.sh /emsdk/upstream/bin/llvm-nm -gC --defined-only "$o" | grep -E '^[^ ]+ [TW] ' | sed -E 's/^[^ ]+ [TW] //' >> "$T/symbols.txt"
done
sort -u -o "$T/symbols.txt" "$T/symbols.txt"
echo "TU symbols: $(wc -l < "$T/symbols.txt")"

# ---- 3. the whole-skeleton link (plain + named twin), prebuilt libraries untouched -----------------------------------
rm -rf "$T/wasm" "$T/wasm-named"
tl=$(date +%s)
if [ -n "$named" ]; then
  make -s -C src link SKIP_UNITS="$SKIP_UNITS" EXTRA_OBJS="${objs[*]}" OUT="$T/wasm/vota_web" OUT_NAMED="$T/wasm-named/vota_web" > "$T/link.log" 2>&1 \
    || { tail -40 "$T/link.log"; exit 1; }
else
  # plain only: run the Makefile's link command with the twin's half cut off
  make -s -C src -n link SKIP_UNITS="$SKIP_UNITS" EXTRA_OBJS="${objs[*]}" OUT="$T/wasm/vota_web" OUT_NAMED="$T/wasm-named/vota_web" 2>/dev/null \
    | sed -e 's/&& em++ [^"]*--emit-symbol-map//' > "$T/link.sh"
  bash "$T/link.sh" > "$T/link.log" 2>&1 || { tail -40 "$T/link.log"; exit 1; }
fi
grep -E 'wasm-ld|error|undefined symbol' "$T/link.log" | grep -v 'signature mismatch: HTS_Audio_initialize' | head -20 || true
M=$T/wasm/vota_web/vota_web_wasm.wasm
N=$T/wasm-named/vota_web/vota_web_wasm.wasm
echo "linked $(stat -f %z "$M") B ($(( $(date +%s) - tl )) s): $(elapsed)"

# ---- 4. the census against the target and the delta vs the previous one ------------------------------------------------
if [ -n "$named" ]; then spec="skeleton=$ROOT/$M@$ROOT/$N"; else spec="skeleton=$ROOT/$M"; fi
kit ledger "$W" "probe=$PROBE" "$spec" -o "$ROOT/$T/functions.tsv" 2> "$T/ledger.log"
echo "census: $(elapsed)"
{
  echo "# try-tu $name — $(date -u +%Y-%m-%dT%H:%M:%SZ)"
  echo
  echo "TUs: ${tus[*]}"
  echo "flags:"
  for l in "${tu_lines[@]}"; do echo "- \`$l\`"; done
  echo
  echo "## Census tier delta vs ledger/functions.tsv (per owner)"
  echo
  kit progress --tsv "$ROOT/$T/functions.tsv" --against "$ROOT/ledger/functions.tsv" --by owner --moves 40 2>/dev/null
  echo
  echo "## Per component (app side)"
  echo
  kit progress --tsv "$ROOT/$T/functions.tsv" --against "$ROOT/ledger/functions.tsv" --by component --moves 0 2>/dev/null | sed -n '3,$p'
} > "$T/report.md"

# ---- 5. the target functions: the TU's functions in the target ------------------------------------------------------
if [ -n "$targets" ]; then
  grep -oE '^[0-9]+' "$targets" > "$T/targets.txt" || true
elif [ -n "$named" ]; then
  # target rows whose best pair is one of the TU's symbols
  awk -F'\t' 'NR==FNR{s[$1]=1; next} FNR>1 && $6=="skeleton" && ($10 in s){print $1}' "$T/symbols.txt" "$T/functions.tsv" > "$T/targets.txt"
  # TU functions the named link has that matched NO target function
  awk -F'\t' 'NR==FNR{s[$1]=1; next} FNR>1 && $6=="skeleton" && ($10 in s){m[$10]=1} END{for(k in s) if(!(k in m)) print k}' "$T/symbols.txt" "$T/functions.tsv" \
    | grep -Fxf <(sed 's/^[0-9]*://' "${N%.wasm}.js.symbols") | sort > "$T/unmatched-symbols.txt" || true
else
  : > "$T/targets.txt"
fi
{
  echo
  echo "## Target functions ($(wc -l < "$T/targets.txt" | tr -d ' ')${targets:+, from $targets})"
  echo
  if [ -s "$T/targets.txt" ]; then
    echo '```'
    kit targets "$W" --tsv "$ROOT/$T/functions.tsv" --against "$ROOT/ledger/functions.tsv" --file "$ROOT/$T/targets.txt" --rows "$rows" "skeleton=$ROOT/$M" 2>/dev/null
    echo '```'
  else
    echo "none (no target function's best pair is a TU function; see unmatched-symbols.txt)"
  fi
  if [ -s "$T/unmatched-symbols.txt" ]; then
    echo
    echo "## TU functions in the link that match no target function ($(wc -l < "$T/unmatched-symbols.txt" | tr -d ' '))"
    echo
    echo '```'; head -60 "$T/unmatched-symbols.txt"; echo '```'
  fi
  echo
  echo "total $(elapsed) (link $(( $(date +%s) - tl )) s incl. census); outputs in $T"
} >> "$T/report.md"
cat "$T/report.md"
