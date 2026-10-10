#!/bin/bash
# Build libcrypto.a of a configured variant (see configure.sh) with 4 jobs inside the pinned image, in its build
# directory build/out/openssl/<variant>.
# The generated headers go first: the object rules do not depend on them, so a parallel `make libcrypto.a` from scratch races.
# SOURCE_DATE_EPOCH is the checkout's commit time, so the "built on" date crypto/buildinf.h gives cversion.c is the
# same in every build and libcrypto.a is reproducible (cversion.o is not in the target's link).
#   usage: tools/libs/openssl/build.sh <variant> [make args...]   (default make target: libcrypto.a)
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
URNA="$(cd "$HERE/../../.." && pwd)"
variant="${1:?variant}"; shift
out="build/out/openssl/$variant"
cd "$URNA"
epoch=$(git -C third_party/openssl log -1 --format=%ct HEAD)
exec docker run --rm -v "$PWD:/work" -w "/work/$out" -e SOURCE_DATE_EPOCH="$epoch" urna-emsdk:6.0.0 \
  bash -c 'make -j4 build_generated >>make.log 2>&1 && make -j4 "$@" 2>&1 | tee -a make.log | grep -v "^emcc\|^emar\|^rm \|^perl" ; test "${PIPESTATUS[0]}" = 0 && ls -la libcrypto.a' -- "${@:-libcrypto.a}"
