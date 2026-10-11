# Getting started

This page takes a fresh clone to a working checkout, then walks through the first three things to try:
reading and lifting one target function, scoring a rewrite of it alone, and running the splat build. The
words in *italics* are defined in the [glossary](glossary.md). The step after this page is
[how-to-decompile.md](how-to-decompile.md), which takes one function from its disassembly to a match.

Run every command from the repository root unless it starts with `cd asmlift-wasm`.

## 1. Prerequisites

| tool | version | used for |
| --- | --- | --- |
| Docker (Docker Desktop or OrbStack) | any recent; the daemon must be running | every compile, in the *pinned image* `urna-emsdk:6.0.0` (`build/Dockerfile`) |
| Node | 24 | the toolkit `asmlift-wasm/` and `tools/setup/unpack_fs.mjs` |
| pnpm | 9 (`asmlift-wasm/package.json` pins `pnpm@9.14.4`) | installing and running the toolkit |
| Python 3 | any 3.x | the scripts in `tools/` |
| wabt (`wasm-objdump`) | 1.0.42 recommended | the target dump and the library census; wabt 1.0.34, the image's, numbers locals differently |
| 7-Zip (`7z`, `7zz` or `7za`) | any | unpacking the 7-Zip 21.02 source release |
| git, curl, make, unzip, bzip2, shasum | GNU make 3.81 or later | fetching and building (`bzip2` unpacks the Boost release) |

On macOS, `brew install node@24 wabt p7zip` and then `corepack enable && corepack prepare pnpm@9.14.4 --activate`
cover all but Docker.

About 4 GB of disk is needed: a bootstrapped and linked clone takes 1.8 GB (0.45 GB of `third_party/` sources,
0.15 GB of downloads, 1 GB of `build/out/`), and the image 2.2 GB.

**Use `TMPDIR=/tmp` on macOS for every command that compiles.** macOS's default temp directory is under
`/var/folders`, which the Docker daemon cannot mount, so a compile that stages its files there fails. The
bootstrap sets it for its own steps; the commands below spell it out.

## 2. Bootstrap

```sh
tools/setup/bootstrap.sh
```

It runs these steps, and skips a step whose output is already there and checks out, so it is safe to run again:

| step | what it does | first run |
| --- | --- | ---: |
| host tools | checks every tool above and names what is missing or the wrong version | 1 s |
| target sums | `shasum -a 256 -c target/SHA256SUMS` | 1 s |
| docker image | builds `urna-emsdk:6.0.0` (`build/docker-build.sh`) | 3 min, plus the 2.2 GB pull of `emscripten/emsdk:6.0.0` |
| `third_party/` | clones OpenSSL, zlib and III-ASN.1 at the revisions `third_party/MANIFEST.md` pins, checking each commit | 15 s |
| `third_party/sqlite` | downloads the SQLite 3.50.4 amalgamation, checks its sha256 and extracts it into `third_party/sqlite/` | 1 s, plus the 2.8 MB download |
| `third_party/RHVoice` | clones RHVoice at tag 1.14.0 and its `external/libs/sonic` submodule at the commit 1.14.0 records, checking both | 9 s |
| `third_party/7zip` | downloads `7z2102-src.7z`, checks its sha256 and extracts it into `third_party/7zip/` | 3 s, plus the 1.2 MB download |
| `third_party/boost` | downloads the Boost 1.89.0 release, checks its sha256 and extracts its headers into `third_party/boost/` | 46 s, plus the 155 MB download |
| target inputs | unpacks the payload into `target/fs/` (checked against `target/fs-manifest.tsv`) and writes `build/dump/vota.objdump.txt` | 33 s |
| libraries | `make -C src libs`: SQLite, libcrypto, 7-Zip and RHVoice at their pinned flags, under `build/out/` | 46 min |
| toolkit | `pnpm install --frozen-lockfile` in `asmlift-wasm/` | 7 s |

The times are those of a fresh clone on an Apple M1 Max under heavy load (load average 100 to 200); an idle machine
is faster. The image's time is a `docker build --no-cache` with the base image already pulled, and the
`third_party/` rows assume the three archives are already in the download cache. The library objects are
reproducible: every clone builds them byte for byte the same, OpenSSL's `cversion.o` included, as its build date is
the OpenSSL checkout's commit time. The run ends with the time per step and the commands to run next.

`tools/setup/bootstrap.sh --check` checks the host tools and lists what is in place without changing anything.
`tools/setup/bootstrap.sh --sources` runs only the steps that fetch sources (`third_party/`).
`tools/setup/bootstrap.sh --build` also runs the commands below, compiling the link's objects `JOBS` at a time
(default 4).

Then link the skeleton and run the splat build once:

```sh
TMPDIR=/tmp make -C src -j4 $(make -s -C src print-LINK_OBJS)   # the app objects, 4 at a time   (5 min)
TMPDIR=/tmp make -C src link                                    # the plain link and its named twin   (31 s)
TMPDIR=/tmp make -C src splat                                   # the link map, the split, the build   (71 s)
```

`make -C src link` builds the *plain link* `build/out/wasm/vota_web/vota_web_wasm.wasm` and the *named twin*
beside it in `build/out/wasm-named/`. It would compile the objects too, but one at a time: about 180 compiles, each
a docker run. Do not pass `-j` to the link itself: its two outputs share one recipe, and under `-j` make runs that
recipe twice at once into the same directories. The plain link is reproducible too: a fresh clone links it byte for
byte as any other checkout at the same commit does. The splat build is section 5.

## 3. Read and lift a function

Function numbers are wasm function indices: imports are 0–135, so the first defined function is 136. Function
1931 is `comum::CRdvVota::Candidato`, a small uenux2 method already matched in `splat/matched.txt`; it is the example
[how-to-decompile.md](how-to-decompile.md) follows.

```sh
cd asmlift-wasm
pnpm wasm fn ../target/vota_web_wasm.wasm 1931
```

prints, in 10 to 20 s, a header, the strings, the callees, the disassembly and a folded readout:

```
;; function 1931 (i32 i32 i32 i32)->(i32) locals(i32 i32 i32 i32) body@0xe929f size 343 instrs 172
;; fingerprint 18d7e932242e2a23 shape 2561f43badbf4c3a (depth 2)
;; table slots 2427
;; owner app  component comum (vtable: comum::CRdvVota)  uses 5
;; callers (5): 11253 11254 11255 11972 11973
;; vtable comum::CRdvVota slot 3 (vtable @1560164, table[2427])

;; --- strings referenced (body offset, address, text)
000067  4725 (+13 into 4712)  "Tipo de identificador inválido: {}"
...
;; --- folded readout (approximate)
function f1931(i32 p0, i32 p1, i32 p2, i32 p3) -> (i32) {
```

Then the lifter:

```sh
TMPDIR=/tmp pnpm wasm lift ../target/vota_web_wasm.wasm 1931 -o /tmp/lift --check
```

```
names: 6998 (glue ../target/vota_web_wasm.js, ledger 6928), mangled pairs: 15375, attribution: 14343
lifted 1/1 in 12 ms, 0 threw
       1 functions: goto
       1 functions: arm tail kept from the join
       1 functions: frame object escapes at entry
       1 functions: inlined callee outlined
compiled 1/1 with em++ -c
```

`/tmp/lift/1931.cpp` is C++ that em++ compiles, written in wasm terms (`int32_t` parameters, `volatile` stores,
empty `asm` barriers that steer clang). It is a starting point to rewrite into the original source, not code to
commit. The indented lines name the workarounds it contains; `asmlift-wasm/README.md` "lift" explains each.
The `mangled pairs:` count comes from the symbol lists `make -C src link` writes beside the plain link
(`build/out/wasm/vota_web/symbols.{mangled,demangled}.txt`, by `tools/round/link-symbols.sh`): with them, a callee
the ledger names is declared by its mangled symbol, so the lifted file links. Before the first link the count is 0,
and such callees get readable identifiers that do not link.

## 4. Score one function alone

`tools/round/try-tu.sh --single` compiles one translation unit at the flags the build uses for its path, keeps
one exported function, and scores it against the target. Here it scores the landed source of 1931:

```sh
TMPDIR=/tmp tools/round/try-tu.sh --single 1931 --extract export:_ZNK5comum8CRdvVota9CandidatoEhjh \
  src/uenux2/src/app/comum/dados/crdvvota.cpp
```

```
flags: -O2 -fPIC -std=c++20 -DNDEBUG -ffile-prefix-map=/work/src/uenux2=/home/rubio/tse/uenux2 -I/work/src …
names for vota_web_wasm.wasm: …/target/vota_web_wasm.js
   458 / 520    crdvvota.cpp

winner crdvvota.cpp
score 458 over 520 rows (same 62, replaced 76 [14 operand-only], inserted 347, deleted 35)
target                                                     candidate
;; (i32 i32 i32 i32)->(i32) locals(i32 i32 i32 i32)      ~ ;; (i32 i32 i32 i32)->(i32) locals(i32 i32 i32 i32 i32 i32 i32 i32 i32)
global.get 0                                             | global.get 0
...
single mode: 35 s
```

The first line is the compile command's flags, read from the recipe `src/Makefile` has for this source's object
(uenux2 here). The score counts the rows of the aligned diff below it that differ: 0 means the two bodies are the
same instruction for instruction, and lower is closer. `|` marks a row that is the same, `~` one that differs only
in an operand, `>` and `<` rows only one side has.

The score is high although 1931 is matched. Compiled alone, the TU gets other inlining decisions than inside the
whole program: here the candidate opens two more stack frames (inlined callees) and ends in an exception landing
pad (`call import:__resumeException`) that the target's body does not have. That is the normal case
for TSE code, not a broken setup: the matched `CStringUtils::ToQWord` (target 1141) scores 982 over 999 alone, because
its template helper is inlined. Use `--single` to iterate on one function's shape; conclude with the whole-link mode
([how-to-decompile.md](how-to-decompile.md) step 5), because only the whole link decides a match.

It takes about 7 s on an idle machine. The 35 s above is a run with other builds going on, and heavy load
stretches its docker runs to several minutes. A file outside `src/` needs `--component <dir under src/>`
(for example `--component uenux2/src/app/comum/dados`), which picks the same flags, and `--print-flags` prints the
compile command without running it.

## 5. Run the splat build

```sh
TMPDIR=/tmp make -C src splat
```

The *splat build* re-assembles the target from its pieces, using our re-pointed bytes for every function matched
in `splat/matched.txt`, and fails unless the result is the target byte for byte. It also reads what each constant
of ours is (an address, a table slot or an integer) from a second link with the data and the table moved,
`make -C src link-kinds` ([reference/splat.md](reference/splat.md) §3). A first run builds the link map and the
pieces (about 71 s with the build) and that shifted link (under a minute); later runs reuse them until the plain
link changes, and then take about 47 s. It prints (paths shortened; the counts move with every match):

```
scratch link byte-identical to build/out/wasm/vota_web/vota_web_wasm.wasm
build/out/wasm-map/vota_web/names.tsv: 10836 functions named (223 binaryen thunks), 16290 wasm-ld functions
14343 functions (6255 named, 12820 grouped; names …/target/vota_web_wasm.js + …/ledger/functions.tsv), 4224 data segments, 11 sections → …
round trip: the pieces reassemble to sha256 8579ef8d1a93819152cecdbf52906d2e4dd0f57999cb2e1e8c0d1161cd1007e2 (7713699 bytes), the input's
…/build/out/splat/vota_web_wasm.wasm: 7713699 bytes, sha256 8579ef8d1a93819152cecdbf52906d2e4dd0f57999cb2e1e8c0d1161cd1007e2 (expected, 7390 matches from …/splat/matched.txt)
substituted: 7390/14343 functions (51.52%), 2134232/5883903 code bytes (36.27%)
component                     functions           code bytes
openssl                       3124/3258       943452/1014603  92.99%
sqlite                          878/927        700394/772965  90.61%
...
vota                              8/780          2977/470204  0.63%
...
readings the target chose (ambiguous operands: address, table slot or literal; which copy of identical bytes):
  re-pointing alone     5309 matches (844076 B)
  another reading       1649 matches (915984 B), 7115 operands: literal 5647, table slot 1468
  which copy             253 matches (359296 B), 491 operands
```

Since 2026-10-11 the build also reads what each constant of ours is (an address, a table slot or an integer) from a
second link with the data and the table moved, `make -C src link-kinds` (under a minute, run on the first build and
whenever the plain link changes; [reference/splat.md](reference/splat.md) §3). With it, "another reading" is 0, and
the output ends with two more lists. With the 7,217 matches of 2026-10-11 it ends:

```
readings the target chose (ambiguous operands: address, table slot or literal; which copy of identical bytes):
  re-pointing alone     6964 matches (1758167 B)
  another reading          0 matches (0 B), 0 operands: -
  which copy             253 matches (358580 B), 491 operands
kind-unverified: 0/7217 matches (0 B) whose operand kinds the shifted link could not tell; their operands kept every reading
review warnings: 55 literal operands of matches that may be a target number the source copied (the check cannot tell): 16 past-data, 11 target-object, 28 past-table
  target-object line 694: function 1307 allocateBtreePage (ours 957) +0x529 i32.const 78143 = str.7fce96d5a99b
...
```

The line with `sha256` and `expected` is the check: the result must be the target's, `8579ef8d…1007e2`. The
`substituted` line is the meter, the project's one progress number: the matches in `splat/matched.txt`; the table
under it splits it by component. The build also fails when a match is kind-unverified; the review warnings never
fail it ([reference/splat.md](reference/splat.md) "Reviewing a match"). [status.md](status.md) has
today's numbers, generated from the committed scan. The link map is `build/out/wasm-map/vota_web/names.tsv` (a
relink of the plain link), the shifted link is under `build/out/wasm-kinds/vota_web/`, and the pieces are under
`build/out/splat/pieces/`. [reference/splat.md](reference/splat.md) covers the pipeline and how to match a
function.

## When something fails

| symptom | cause |
| --- | --- |
| a compile through docker cannot find its input or output files | `TMPDIR` is not `/tmp` (section 1) |
| `make -C src link` fails with `assert names`, or says "Nothing to be done" after a failure | a `-j` run; delete `build/out/wasm/vota_web/vota_web_wasm.wasm` and `build/out/wasm-named/vota_web/vota_web_wasm.wasm` by name and link again without `-j` |
| a flag change in `src/units/*.mk`, or an edit of a source, does not change the link | make does not rebuild objects when only a fragment's flags change; touch the sources. The generic ecourna rule does not depend on a source deeper than `src/ecourna/*/*/` (`api/sql/sqlite/csqlstatement.cpp`), so touching it does nothing: delete its object by name (`build/out/app/ecourna/api_sql_sqlite_csqlstatement.cpp.o`), or give the fragment a prerequisite rule (`src/units/README.md` "What a fragment holds") |
| the bootstrap refuses a `third_party/` checkout | that directory is at another commit; it is never reset for you, so move it aside and run the bootstrap again |
| a step takes many times longer than the table says | the host is loaded; every compile is a docker run |
