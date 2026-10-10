#!/bin/bash
# Configure an OpenSSL 3.0.17 libcrypto build of third_party/openssl inside the pinned emsdk image. The build is out of
# tree: Configure runs in <urna>/build/out/openssl/<variant>, which receives the Makefile, the generated headers, the
# objects and libcrypto.a; third_party/openssl is only read.
#   usage: tools/libs/openssl/configure.sh <variant> [extra Configure args...]
#   env:   URNA_OSSL_CFLAGS (default "-O"), URNA_OSSL_PIC (default "-fPIC", conan passes it; "no-pic" is its alternative),
#          URNA_OSSL_CNF_CFLAGS (""), URNA_OSSL_BN_OPS ("")
# The Configure line is conan-center's for os=Emscripten (target "cc"), with the target wrapped as "urna-wasm"
# (urna.conf) so CFLAGS/bn_ops can be varied.
# The target's ERR records name sources relative to the OpenSSL tree (OPENSSL_FILE = __FILE__: "crypto/err/err.c",
# "crypto/err/err_local.h"), as conan's in-tree build compiles them. Here the Makefile names them from the build
# directory (../../../../third_party/openssl/crypto/err/err.c), so -ffile-prefix-map strips that prefix and __FILE__
# is the target's string again; it changes nothing else in an object.
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
URNA="$(cd "$HERE/../../.." && pwd)"
variant="${1:?variant}"; shift
case "$variant" in */*|.|..) echo "variant must be one directory name: $variant" >&2; exit 2 ;; esac
out="build/out/openssl/$variant"
srcrel="../../../../third_party/openssl"   # third_party/openssl as seen from $out
have=$(sed -n -E 's/^(MAJOR|MINOR|PATCH)=([0-9]+)$/\2/p' "$URNA/third_party/openssl/VERSION.dat" 2>/dev/null | paste -sd. - || true)
[ "$have" = 3.0.17 ] || { echo "third_party/openssl holds OpenSSL ${have:-nothing}, not 3.0.17 (tools/setup/bootstrap.sh fetches it)" >&2; exit 2; }
export URNA_OSSL_CFLAGS="${URNA_OSSL_CFLAGS:--O}"
export URNA_OSSL_CNF_CFLAGS="${URNA_OSSL_CNF_CFLAGS:-}"
export URNA_OSSL_BN_OPS="${URNA_OSSL_BN_OPS:-}"
export URNA_OSSL_PIC="${URNA_OSSL_PIC:--fPIC}"
mkdir -p "$URNA/$out"
cd "$URNA"
exec docker run --rm -v "$PWD:/work" -w "/work/$out" \
  -e CC=emcc -e CXX=em++ -e AR=emar -e RANLIB=emranlib \
  -e URNA_OSSL_CFLAGS -e URNA_OSSL_CNF_CFLAGS -e URNA_OSSL_BN_OPS -e URNA_OSSL_PIC -e SRCREL="$srcrel" \
  urna-emsdk:6.0.0 bash -c '
    set -e
    perl "$SRCREL/Configure" --config=/work/tools/libs/openssl/urna.conf urna-wasm \
      no-shared --release --prefix=/ --libdir=lib --openssldir=/etc/ssl threads no-unit-test no-tests "$URNA_OSSL_PIC" no-fips no-md2 \
      "-ffile-prefix-map=$SRCREL/=" "$@"
    perl configdata.pm --dump > configdata.dump.txt
    grep -E "^(SRCDIR|CFLAGS|CNF_CFLAGS|CNF_CPPFLAGS|CPPFLAGS|LIB_CFLAGS|LIB_CPPFLAGS)=" Makefile
    grep -E "BN_LLONG|THIRTY_TWO_BIT|SIXTY_FOUR" include/openssl/configuration.h
  ' -- "$@"
