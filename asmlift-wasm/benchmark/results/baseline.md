# Benchmark: baseline

Target `target/vota_web_wasm.wasm` (sha256 `8579ef8d1a938191…`), image `urna-emsdk:6.0.0`, 190 rows (rows.json `44761a2a53a4`, setups.json `3d4d1ab2e594`), urna `37d004d+dirty`, 2026-10-11T14:11:46.309Z. Lift 232 ms, compile and optimise 9.6 s, total 20.2 s.

Each row is lifted, compiled with `em++ -c` at its setup's flags, and scored against the target at depth 0 with callees, data addresses, the stack pointer and the indirect table canonicalised on both sides (raw); then the object is linked alone and put through the target link's wasm-opt -Oz passes and scored again (-Oz). The ratio is differing rows over target rows; exact is a -Oz score of 0. An isolated function misses what the whole-program link does to it, so no score here means byte-identical.

## Overall

| rows | lifted | compiled | -Oz linked | exact raw | exact -Oz | exact share | median ratio | median -Oz | mean -Oz |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 190 | 190 | 190 | 190 | 37 | 101 | 53.2% | 0.377 | 0.000 | 0.161 |

## By compile setup

| setup | rows | lifted | compiled | -Oz linked | exact raw | exact -Oz | exact share | median ratio | median -Oz | mean -Oz | failures |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| `uenux2` uenux2, the TSE application | 12 | 12 | 12 | 12 | 2 | 5 | 41.7% | 0.540 | 0.100 | 0.151 | – |
| `ecourna` ecourna (conan libecea1da310e5107), PIC translation units | 10 | 10 | 10 | 10 | 1 | 9 | 90.0% | 0.671 | 0.000 | 0.022 | – |
| `ecourna-nopic` ecourna, non-PIC translation units (app/dados, api/io, api/compression, app/dados/asn converters) | 12 | 12 | 12 | 12 | 1 | 6 | 50.0% | 0.732 | 0.003 | 0.043 | – |
| `asn1` TSE fork of the III ASN.1 runtime and its generated Modulo* modules | 12 | 12 | 12 | 12 | 4 | 10 | 83.3% | 0.260 | 0.000 | 0.063 | – |
| `asn1-eh` ASN.1 runtime translation units that throw (BERDecoderPartial) | 11 | 11 | 11 | 11 | 8 | 10 | 90.9% | 0.000 | 0.000 | 0.009 | – |
| `zlib` zlib 1.3.1 + minizip | 12 | 12 | 12 | 12 | 2 | 5 | 41.7% | 0.358 | 0.156 | 0.179 | – |
| `sqlite` SQLite 3.50.4 amalgamation | 12 | 12 | 12 | 12 | 2 | 5 | 41.7% | 0.416 | 0.213 | 0.193 | – |
| `openssl` OpenSSL 3.0.17 libcrypto | 12 | 12 | 12 | 12 | 3 | 5 | 41.7% | 0.423 | 0.082 | 0.178 | – |
| `7zip` 7-Zip / LZMA SDK 21.02, single-threaded | 12 | 12 | 12 | 12 | 2 | 4 | 33.3% | 0.541 | 0.240 | 0.462 | – |
| `rhvoice` RHVoice 1.14.0 core (+ hts_engine and sonic in C) | 12 | 12 | 12 | 12 | 4 | 8 | 66.7% | 0.271 | 0.000 | 0.112 | – |
| `boost` Boost 1.89.0, header-only | 12 | 12 | 12 | 12 | 3 | 11 | 91.7% | 0.137 | 0.000 | 0.016 | – |
| `libc` emscripten musl libc (libc.a) | 12 | 12 | 12 | 12 | 2 | 4 | 33.3% | 0.446 | 0.213 | 0.242 | – |
| `libc-optz` emscripten musl libc, the -Oz link's size-optimised members (libc_optz.a) | 5 | 5 | 5 | 5 | 0 | 1 | 20.0% | 0.417 | 0.222 | 0.200 | – |
| `dlmalloc` emscripten dlmalloc (libdlmalloc.a) | 8 | 8 | 8 | 8 | 0 | 3 | 37.5% | 0.583 | 0.268 | 0.221 | – |
| `compiler-rt` compiler-rt builtins (libcompiler_rt.a) | 12 | 12 | 12 | 12 | 0 | 2 | 16.7% | 0.641 | 0.522 | 0.415 | – |
| `libcxx` libc++ (libc++.a) | 12 | 12 | 12 | 12 | 2 | 5 | 41.7% | 0.340 | 0.043 | 0.153 | – |
| `libcxxabi` libc++abi (libc++abi.a) | 12 | 12 | 12 | 12 | 1 | 8 | 66.7% | 0.271 | 0.000 | 0.084 | – |

## By size band

| band | rows | lifted | compiled | -Oz linked | exact raw | exact -Oz | exact share | median ratio | median -Oz | mean -Oz |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| tiny | 57 | 57 | 57 | 57 | 34 | 51 | 89.5% | 0.000 | 0.000 | 0.042 |
| small | 55 | 55 | 55 | 55 | 2 | 40 | 72.7% | 0.300 | 0.000 | 0.063 |
| medium | 36 | 36 | 36 | 36 | 1 | 10 | 27.8% | 0.491 | 0.190 | 0.242 |
| large | 27 | 27 | 27 | 27 | 0 | 0 | 0.0% | 0.634 | 0.312 | 0.336 |
| huge | 15 | 15 | 15 | 15 | 0 | 0 | 0.0% | 0.708 | 0.359 | 0.462 |

## By feature

A row counts under every feature it has.

| feature | rows | lifted | compiled | -Oz linked | exact raw | exact -Oz | exact share | median ratio | median -Oz | mean -Oz |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| leaf | 101 | 101 | 101 | 101 | 31 | 60 | 59.4% | 0.308 | 0.000 | 0.158 |
| loop | 56 | 56 | 56 | 56 | 0 | 7 | 12.5% | 0.583 | 0.307 | 0.346 |
| switch | 21 | 21 | 21 | 21 | 0 | 1 | 4.8% | 0.617 | 0.287 | 0.296 |
| call_indirect | 34 | 34 | 34 | 34 | 3 | 19 | 55.9% | 0.313 | 0.000 | 0.107 |
| invoke | 8 | 8 | 8 | 8 | 0 | 3 | 37.5% | 0.711 | 0.010 | 0.086 |
| throw | 10 | 10 | 10 | 10 | 1 | 2 | 20.0% | 0.710 | 0.135 | 0.179 |
| frame | 46 | 46 | 46 | 46 | 0 | 7 | 15.2% | 0.611 | 0.257 | 0.289 |
| alloca | 2 | 2 | 2 | 2 | 0 | 0 | 0.0% | 0.688 | 0.385 | 0.385 |
| inlined-frame | 9 | 9 | 9 | 9 | 0 | 0 | 0.0% | 0.737 | 0.260 | 0.301 |
| i64 | 68 | 68 | 68 | 68 | 3 | 15 | 22.1% | 0.594 | 0.219 | 0.280 |
| float | 12 | 12 | 12 | 12 | 2 | 2 | 16.7% | 0.529 | 0.268 | 0.297 |
| bulk-memory | 21 | 21 | 21 | 21 | 0 | 4 | 19.0% | 0.527 | 0.255 | 0.250 |
| select | 60 | 60 | 60 | 60 | 5 | 13 | 21.7% | 0.568 | 0.274 | 0.317 |

## Failures

None: every row lifted, compiled, linked and was found.

## Largest -Oz ratios

| id | size | target rows | raw | ratio | -Oz | ratio -Oz | features |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 7zip/4943 | 2246 | 678 | 1654 | 2.439 | 1570 | 2.316 | leaf loop i64 select |
| compiler-rt/1733 | 219 | 101 | 102 | 1.010 | 89 | 0.881 | leaf i64 select |
| 7zip/2646 | 526 | 281 | 277 | 0.986 | 213 | 0.758 | leaf loop frame i64 |
| compiler-rt/6225 | 192 | 83 | 66 | 0.795 | 61 | 0.735 | leaf i64 select |
| libc/2968 | 24 | 11 | 4 | 0.364 | 8 | 0.727 | leaf frame float |
| 7zip/2630 | 421 | 211 | 172 | 0.815 | 145 | 0.687 | loop call_indirect frame i64 select |
| compiler-rt/12119 | 16 | 9 | 8 | 0.889 | 6 | 0.667 | leaf |
| compiler-rt/522 | 1361 | 675 | 451 | 0.668 | 448 | 0.664 | frame i64 select |
| zlib/8121 | 81 | 43 | 40 | 0.930 | 27 | 0.628 | select |
| 7zip/1859 | 366 | 195 | 152 | 0.779 | 121 | 0.621 | leaf loop |
| libc/1169 | 65 | 34 | 20 | 0.588 | 20 | 0.588 | leaf i64 |
| sqlite/1184 | 414 | 215 | 155 | 0.721 | 125 | 0.581 | leaf loop frame i64 |
| openssl/2526 | 2088 | 1109 | 845 | 0.762 | 631 | 0.569 | loop frame alloca i64 bulk-memory select |
| openssl/920 | 985 | 529 | 344 | 0.650 | 299 | 0.565 | leaf loop select |
| compiler-rt/907 | 1236 | 588 | 379 | 0.645 | 326 | 0.554 | frame i64 select |
