# OpenSSL 3.0.17 libcrypto block

Everything here was measured against `target/vota_web_wasm.wasm` (`docs/reference/facts/libraries.md` §B2–§B4).

| fact | value | how |
| --- | --- | --- |
| Configure | `Configure cc no-shared --release --prefix=/ --libdir=lib --openssldir=/etc/ssl threads no-unit-test no-tests -fPIC no-fips no-md2` with `CC=emcc AR=emar RANLIB=emranlib`, out of tree with `-ffile-prefix-map=<third_party/openssl>/=` (`__FILE__` = `crypto/...`, as conan's in-tree build gives) | conan-center `openssl/3.x.x`, `"Emscripten-*-*": "cc"`; target strings `//lib/ossl-modules`, `/etc/ssl` |
| object -O level | **-O3** (conan Release: the `CFLAGS` env replaces the `cc` target's `-O`) | `bn_exp.c` sweep: -O3 35/594 rows vs -O1 373/680 (`build/out/openssl/O1/sweep`) |
| bn_ops | `THIRTY_TWO_BIT`, **no** `BN_LLONG` (`cc` has no bn_ops; configuration.h `#undef BN_LLONG`) | bn_div.c fn 444 has 0 i64 ops; bn_exp.c's i64 ops are its own |
| -fPIC | changes the object (GOT imports) but not the linked body | pic/nopic sweep identical after link |
| link | `em++ -Oz -fexceptions` (the target's link line) | the pinned toolchain (`build/README.md`) |

## Pipeline

```
configure.sh O3                      # URNA_OSSL_CFLAGS=-O3 → the build directory build/out/openssl/O3 (sources read from third_party/openssl)
build.sh O3                          # make build_generated, make -j4 libcrypto.a   (~3 min)
ROOTS=tools/libs/openssl/roots.txt EXTRA_EXPORTS=tools/libs/openssl/roots/empty.txt census.sh O3 <tag>
                                     # link-probe (main() calls each root), dumps, names, census at depth 2 and depth 0
write-census-json.py build/out/openssl/O3-probe-<tag> O3 roots.txt census.json
```

`roots.txt` (258 symbols) is the libcrypto API the app calls, derived from the target (`derive-roots.py`: probe functions with ≤ 1
internal call site that the census pairs to a target function; `negative-symbols.py` / `prune-by-phantoms.py` / `object-closure.py`
remove everything the target's string table proves absent). Never export library functions to keep them alive: an exported
function is never inlined-and-removed by binaryen, so the probe's bodies drift from the target's (`docs/reference/facts/tool-traps.md` §X1).

`flag-sweep.sh O3 <probe-tag> <out-tag> "<cflags>" <src.c>...` recompiles a few sources with extra flags and relinks the probe,
to test a flag hypothesis function by function (`score ... --depth 0`).

## Result (census.json)

`census.json` is the record of the `r10` probe. Its `probe` field, `build/out/openssl/O3/probe-r10`, is a path inside
the build directory; `census.sh` and `link-probe.sh` write a probe to `build/out/openssl/<variant>-probe[-<tag>]`, so
that probe built again lands in `build/out/openssl/O3-probe-r10`.

depth 0: 1904 / 2183 openssl-class target functions pair body-for-body (655 fingerprint + 1160 address-blind + 89 shape);
depth 2: 1780. 1665 of the probe's 1702 libcrypto functions pair. Remaining differences are whole-program effects of the
final link (DAE / constant-argument propagation into `__multi3`, local renumbering after a differently inlined callee), not flags.
