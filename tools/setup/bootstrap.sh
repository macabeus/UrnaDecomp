#!/bin/bash
# Turn a fresh clone into a working checkout: `make -C src link` and `make -C src splat` work afterwards.
#
#   tools/setup/bootstrap.sh            # every step; a step whose output is already there and checks out is skipped
#   tools/setup/bootstrap.sh --check    # host tools and what is already in place; changes nothing
#   tools/setup/bootstrap.sh --sources  # host tools, then only the steps that fetch sources (third_party/)
#   tools/setup/bootstrap.sh --build    # every step, then the link's objects JOBS at a time (default 4), the link
#                                       # itself and `make -C src splat`
#
# Steps: host tools → target sums → docker image → third_party/ at the versions of third_party/MANIFEST.md (the git
# checkouts, RHVoice 1.14.0 with its sonic submodule, the SQLite 3.50.4 amalgamation, the 7-Zip 21.02 source release,
# the Boost 1.89.0 headers) → target/fs → build/dump/vota.objdump.txt → the pinned library builds (`make -C src libs`)
# → `pnpm install` in asmlift-wasm/.
# Downloads are kept in build/out/setup/downloads/ and checked against their sha256 before use.
#
# Every docker-backed command runs with TMPDIR=/tmp. On macOS the default TMPDIR is a /var/folders path the docker
# daemon cannot mount, so a compile that stages files there fails; export TMPDIR=/tmp for your own make runs too.
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/../.." && pwd)
cd "$ROOT"
export TMPDIR=/tmp

MODE=all
case "${1:-}" in
  '') ;;
  --check) MODE=check ;;
  --build) MODE=build ;;
  --sources) MODE=sources ;;
  -h|--help) sed -n '2,16p' "$0" | sed 's/^# \{0,1\}//'; exit 0 ;;
  *) echo "unknown option: $1 (see --help)" >&2; exit 2 ;;
esac

DL=build/out/setup/downloads
TIMES=()
step_start=0
step() { step_name=$1; step_start=$SECONDS; printf '\n== %s\n' "$step_name"; }
step_done() { TIMES+=("$(printf '%5ss  %s' $((SECONDS - step_start)) "$step_name")"); }
say() { printf '   %s\n' "$*"; }
die() { printf 'bootstrap: %s\n' "$*" >&2; exit 1; }
sha256_of() { shasum -a 256 "$1" | cut -d' ' -f1; }

# ---- host tools ------------------------------------------------------------------------------------------------
step "host tools"
missing=()
need() {  # need <command> <what it is for / how to get it>
  if command -v "$1" >/dev/null 2>&1; then say "ok   $1"; else say "MISSING $1: $2"; missing+=("$1"); fi
}
need docker  "Docker Desktop or OrbStack; every compile runs in the pinned image urna-emsdk:6.0.0"
need node    "Node 24 (https://nodejs.org, or nvm install 24)"
need pnpm    "pnpm 9 (corepack enable && corepack prepare pnpm@9.14.4 --activate)"
need python3 "Python 3 (the extraction and census scripts)"
need wasm-objdump "wabt (brew install wabt / apt install wabt): the target dump and the library census"
need git     "git"
need curl    "curl"
need make    "make (GNU make 3.81 or later)"
need shasum  "shasum (perl; ships with macOS and most Linux distributions)"
need unzip   "unzip (unpacks the SQLite amalgamation into third_party/sqlite)"
need bzip2   "bzip2 (unpacks the Boost headers into third_party/boost)"
SEVENZ=
for c in 7z 7zz 7za; do command -v "$c" >/dev/null 2>&1 && { SEVENZ=$c; break; }; done
if [ -n "$SEVENZ" ]; then say "ok   $SEVENZ"; else say "MISSING 7z: p7zip or 7-Zip (brew install p7zip / apt install p7zip-full): unpacks 7z2102-src.7z into third_party/7zip"; missing+=(7z); fi
if command -v node >/dev/null 2>&1; then
  v=$(node --version); [ "${v%%.*}" = v24 ] || { say "WRONG node $v: the toolkit is tested on Node 24"; missing+=(node24); }
fi
if command -v pnpm >/dev/null 2>&1; then
  v=$(pnpm --version); [ "${v%%.*}" = 9 ] || { say "WRONG pnpm $v: asmlift-wasm pins pnpm 9 (packageManager pnpm@9.14.4)"; missing+=(pnpm9); }
fi
if command -v wasm-objdump >/dev/null 2>&1; then
  v=$(wasm-objdump --version)
  [ "$v" = 1.0.42 ] || say "note: wabt $v; the scripts are checked against wabt 1.0.42's dump (1.0.34, the image's, numbers locals differently)"
fi
if command -v docker >/dev/null 2>&1 && ! docker info >/dev/null 2>&1; then
  say "docker is installed but its daemon does not answer: start Docker Desktop or OrbStack"; missing+=(dockerd)
fi
[ ${#missing[@]} -eq 0 ] || die "fix the host tools above, then run this again"
step_done

# ---- the target ------------------------------------------------------------------------------------------------
step "target sums"
shasum -a 256 -c target/SHA256SUMS | sed 's/^/   /' || die "target/ does not match target/SHA256SUMS"
step_done

if [ "$MODE" = check ]; then
  step "what is in place"
  docker image inspect urna-emsdk:6.0.0 >/dev/null 2>&1 && say "image urna-emsdk:6.0.0: built" || say "image urna-emsdk:6.0.0: missing"
  for d in openssl zlib III-ASN.1 sqlite/sqlite3.c RHVoice RHVoice/external/libs/sonic/sonic.c 7zip boost/boost/version.hpp; do
    [ -e "third_party/$d" ] && say "third_party/$d: present" || say "third_party/$d: missing"
  done
  for p in target/fs build/dump/vota.objdump.txt asmlift-wasm/node_modules; do
    [ -e "$p" ] && say "$p: present" || say "$p: missing"
  done
  make -s -C src -q libs 2>/dev/null && say "library builds: up to date" || say "library builds: missing or stale (make -C src libs)"
  exit 0
fi

# ---- the pinned image ------------------------------------------------------------------------------------------
if [ "$MODE" != sources ]; then
  step "docker image urna-emsdk:6.0.0"
  if docker image inspect urna-emsdk:6.0.0 >/dev/null 2>&1; then say "already built"; else build/docker-build.sh; fi
  step_done
fi

# ---- third_party/ ----------------------------------------------------------------------------------------------
# clone_at <dir> <url> <ref> <commit prefix> [shallow]: clone (shallow: only <ref>) and check out <ref>; an existing
# checkout is accepted when its HEAD is that commit, and refused otherwise (it is never reset).
clone_at() {
  local dir=third_party/$1 url=$2 ref=$3 want=$4 shallow=${5:-}
  if [ ! -d "$dir/.git" ]; then
    if [ -n "$shallow" ]; then
      git -c advice.detachedHead=false clone --quiet --depth 1 --branch "$ref" "$url" "$dir"
    else
      git clone --quiet "$url" "$dir"
      git -C "$dir" -c advice.detachedHead=false checkout --quiet "$ref"
    fi
  fi
  local head; head=$(git -C "$dir" rev-parse HEAD)
  case "$head" in
    "$want"*) say "$dir at $ref (${head:0:12})" ;;
    *) die "$dir is at ${head:0:12}, not $ref ($want): move it aside and run this again" ;;
  esac
}
# fetch <url> <file> <sha256>: download into $DL once; the sha256 is checked every time
fetch() {
  local url=$1 file=$DL/$2 want=$3
  mkdir -p "$DL"
  if [ ! -f "$file" ] || [ "$(sha256_of "$file")" != "$want" ]; then
    say "downloading $url"
    if [ -t 2 ]; then curl -fL --retry 3 --progress-bar -o "$file.part" "$url"; else curl -fsSL --retry 3 -o "$file.part" "$url"; fi
    mv "$file.part" "$file"
  fi
  [ "$(sha256_of "$file")" = "$want" ] || die "$file: sha256 $(sha256_of "$file"), expected $want"
}

step "third_party/: OpenSSL, zlib, III-ASN.1"
clone_at openssl     https://github.com/openssl/openssl      openssl-3.0.17  4d7dfa23 shallow
clone_at zlib        https://github.com/madler/zlib          v1.3.1          51b7f2a
clone_at III-ASN.1   https://github.com/Kampbell/III-ASN.1   66e5d48         66e5d48
step_done

# SQLite: the 3.50.4 amalgamation (sqlite3.c, the one file the build compiles, and its headers), extracted into
# third_party/sqlite
step "third_party/sqlite: the SQLite 3.50.4 amalgamation"
SQ=third_party/sqlite
if [ -d "$SQ" ]; then
  v=$(sed -n 's/^#define SQLITE_VERSION  *"\([0-9.]*\)".*/\1/p' "$SQ/sqlite3.h" 2>/dev/null || true)
  [ "$v" = 3.50.4 ] && [ -f "$SQ/sqlite3.c" ] || die "$SQ is not the SQLite 3.50.4 amalgamation (its sqlite3.h says ${v:-no version}): move it aside and run this again"
  say "$SQ holds the 3.50.4 amalgamation"
else
  fetch https://sqlite.org/2025/sqlite-amalgamation-3500400.zip sqlite-amalgamation-3500400.zip \
    1d3049dd0f830a025a53105fc79fd2ab9431aea99e137809d064d8ee8356b032
  rm -rf "$SQ.part" && mkdir -p "$SQ.part"
  unzip -q "$DL/sqlite-amalgamation-3500400.zip" -d "$SQ.part"
  mv "$SQ.part/sqlite-amalgamation-3500400" "$SQ" && rmdir "$SQ.part"
  say "$SQ unpacked from sqlite-amalgamation-3500400.zip ($(ls "$SQ" | paste -sd' ' -))"
fi
step_done

# RHVoice: a full clone (the version probes of third_party/MANIFEST.md read its other tags) at 1.14.0, and of its
# submodules the one the build compiles, sonic, at the commit 1.14.0 records
step "third_party/RHVoice: 1.14.0 and its sonic submodule"
clone_at RHVoice     https://github.com/RHVoice/RHVoice      1.14.0          d1d5fc6c
git -C third_party/RHVoice submodule update --init --quiet external/libs/sonic
sonic=$(git -C third_party/RHVoice submodule status external/libs/sonic)
case "$sonic" in
  ' '*) say "third_party/RHVoice/external/libs/sonic at ${sonic:1:12}" ;;
  *) die "third_party/RHVoice/external/libs/sonic is not at the commit RHVoice 1.14.0 records: $sonic" ;;
esac
step_done

# 7-Zip: the 21.02 source release from SourceForge, extracted whole into third_party/7zip (no git repository has 21.02)
step "third_party/7zip: the 7-Zip 21.02 source release"
SZ=third_party/7zip
if [ -d "$SZ" ]; then
  v=$(sed -n 's/^#define MY_VERSION_NUMBERS "\([0-9.]*\).*/\1/p' "$SZ/C/7zVersion.h" 2>/dev/null || true)
  [ "$v" = 21.02 ] || die "$SZ holds 7-Zip ${v:-nothing recognisable}, not 21.02: move it aside and run this again"
  say "$SZ holds 21.02"
else
  fetch https://sourceforge.net/projects/sevenzip/files/7-Zip/21.02/7z2102-src.7z/download 7z2102-src.7z \
    ee9755feaa034e7075712bc6b1fad25440bd0a8f21f106a16e37bc4b409f1122
  rm -rf "$SZ.part" && mkdir -p "$SZ.part"
  "$SEVENZ" x -y -o"$SZ.part" "$DL/7z2102-src.7z" >/dev/null
  mv "$SZ.part" "$SZ"
  say "$SZ unpacked from 7z2102-src.7z ($(find "$SZ" -type f | wc -l | tr -d ' ') files)"
fi
step_done

# Boost: header-only use, so of the 1.89.0 release only its boost/ header tree (and its licence) is extracted, into
# third_party/boost
step "third_party/boost: the Boost 1.89.0 headers"
BOOST=third_party/boost
if [ -d "$BOOST" ]; then
  v=$(sed -n 's/^#define BOOST_VERSION \([0-9]*\)$/\1/p' "$BOOST/boost/version.hpp" 2>/dev/null || true)
  [ "$v" = 108900 ] || die "$BOOST is not the Boost 1.89.0 headers (its boost/version.hpp says ${v:-no version}): move it aside and run this again"
  say "$BOOST holds 1.89.0"
else
  fetch https://archives.boost.io/release/1.89.0/source/boost_1_89_0.tar.bz2 boost_1_89_0.tar.bz2 \
    85a33fa22621b4f314f8e85e1a5e2a9363d22e4f4992925d4bb3bc631b5a0c7a
  rm -rf "$BOOST.part" && mkdir -p "$BOOST.part"
  tar -xjf "$DL/boost_1_89_0.tar.bz2" -C "$BOOST.part" --strip-components 1 boost_1_89_0/boost boost_1_89_0/LICENSE_1_0.txt
  mv "$BOOST.part" "$BOOST"
  say "$BOOST unpacked from boost_1_89_0.tar.bz2 ($(find "$BOOST" -type f | wc -l | tr -d ' ') files)"
fi
step_done

if [ "$MODE" = sources ]; then
  printf '\n== time per step\n'
  printf '%s\n' "${TIMES[@]}"
  exit 0
fi

# ---- inputs read from the target -------------------------------------------------------------------------------
step "target/fs and build/dump/vota.objdump.txt"
mkdir -p build/out/setup
if [ -d target/fs ] && [ -n "$(ls -A target/fs)" ]; then
  say "target/fs present"
else
  node tools/setup/unpack_fs.mjs target/vota_web_wasm.js target/vota_web_wasm.data target/fs build/out/setup/fs-manifest.tsv | sed 's/^/   /'
  cmp -s build/out/setup/fs-manifest.tsv target/fs-manifest.tsv || die "the unpacked payload differs from target/fs-manifest.tsv"
  say "target/fs unpacked; its files match target/fs-manifest.tsv"
fi
if [ -s build/dump/vota.objdump.txt ]; then
  say "build/dump/vota.objdump.txt present"
else
  mkdir -p build/dump
  wasm-objdump -d target/vota_web_wasm.wasm > build/dump/vota.objdump.txt.part
  mv build/dump/vota.objdump.txt.part build/dump/vota.objdump.txt
  say "build/dump/vota.objdump.txt written ($(wc -l < build/dump/vota.objdump.txt | tr -d ' ') lines)"
fi
step_done

# ---- the pinned library builds ---------------------------------------------------------------------------------
step "library builds (make -C src libs: SQLite, libcrypto, 7-Zip, RHVoice)"
if make -s -C src -q libs 2>/dev/null; then say "up to date"; else make -C src libs; fi
step_done

# ---- the toolkit -----------------------------------------------------------------------------------------------
step "asmlift-wasm: pnpm install"
(cd asmlift-wasm && pnpm install --frozen-lockfile --reporter=silent) && say "installed"
step_done

if [ "$MODE" = build ]; then
  # the objects in parallel, the link without -j: its two outputs share one recipe, which -j would run twice at once
  step "the link's objects (make -j${JOBS:-4})"
  read -r -a objs <<< "$(make -s -C src print-LINK_OBJS)"
  make -C src -j"${JOBS:-4}" "${objs[@]}" "$ROOT/$(make -s -C src print-OSSL_OBJS_FILE)"
  step_done
  step "make -C src link"
  make -C src link
  step_done
  step "make -C src splat"
  make -C src splat
  step_done
fi

printf '\n== time per step\n'
printf '%s\n' "${TIMES[@]}"
if [ "$MODE" != build ]; then
  cat <<'EOF'

Next (from the repository root; docs/getting-started.md says what each prints):
  TMPDIR=/tmp make -C src -j4 $(make -s -C src print-LINK_OBJS)   # the app objects, 4 at a time
  TMPDIR=/tmp make -C src link                                    # the plain link and its named twin (never -j)
  TMPDIR=/tmp make -C src splat                                   # must end with the target's sha256 and the match count
EOF
fi
