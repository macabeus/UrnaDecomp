# src/ — the reproduction project

```
TMPDIR=/tmp make -C src libs     # the upstream libraries, through tools/libs/*/build.sh
TMPDIR=/tmp make -C src link     # the app objects, the plain link and its --emit-symbol-map twin
TMPDIR=/tmp make -C src census   # census of the plain link → ledger/functions.tsv and a snapshot in ledger/history/
TMPDIR=/tmp make -C src splat    # the official check: the target's sha256 and the meter (docs/reference/splat.md)
make -C src status               # the progress blocks of README.md and docs/status.md, from splat/scan.tsv
make -C src stubs                # the stub of every TSE function not decompiled yet, and the gaps (below); stubs-check checks them
TMPDIR=/tmp make -C src check-srcloc   # the source_location records of the app objects against the target's
make -C src print-APP_CXXFLAGS   # any Makefile variable, for scripts (print-APP_INCLUDES, print-LINK_OBJS, …)
```

Outputs: `build/out/wasm/vota_web/vota_web_wasm.{js,wasm,data}` (byte-comparable with `target/`), its `link.cmd`
(the exact link command) and `symbols.{mangled,demangled}.txt` (every symbol of the link's inputs, for the lifter),
and `build/out/wasm-named/vota_web/` (names only: `--emit-symbol-map` changes the -Oz function order, so never
byte-compare it).

| path | what |
| --- | --- |
| `Makefile` | the link order (`LINK_OBJS`), the compile flags per tree, the library targets, the link |
| `mk/` | the build machinery the unit fragments rely on: `ecourna-flags.mk`, `asn1.mk` (the ASN.1 runtime and modules), `splat-*.mk` (the splat build) |
| `units/` | one fragment per landed unit, `<NN>-<component>-<what>.mk`, with its roots, pending and externs files (`units/README.md`) |
| `ecourna/`, `uenux2/`, `asn1/` | the reconstructed TUs: ecourna mirrors the conan package's `b/src/ecourna`, uenux2 mirrors `/home/rubio/tse/uenux2` (`src/…`, `wasm/…`), asn1 the III ASN.1 fork. TSE's sources are Latin-1 (`.editorconfig`) |
| `app/vota_init.cpp` … `wasm_sound.cpp` | the 8 exported functions as hand-written stand-ins (the glossary's second meaning of *stub*), signatures from the target export types; TU order = export order |
| `app/roots.cpp` | one function the stubs call that reaches every library entry point once (keeps the app's reachable set without exporting) |
| `app/roots_ecourna.cpp`, `app/roots_uenux2.cpp` | callers of the reconstructed ecourna / uenux2 units with the target's call-site counts (binaryen inlines a function it sees called once) |
| `app/roots_{7zip,rhvoice,boost}.cpp`, `stubs_7zip.cpp` | the library probes' drivers compiled as app code; definitions 7-Zip needs that emscripten lacks |
| `web/wasm_shim.cpp` | the 34 EM_JS functions (generated: `../tools/extract/gen_wasm_shim.py` from `web/library_vota.pretty.js` + the target import types) |

The scripts the build calls are in `../tools/round/` (`census.sh`, `census_report.py`, `link-map.sh`,
`link-symbols.sh`, `status.py`) and `../tools/libs/`; `../tools/README.md` describes each.

## Stubs: the TSE functions not decompiled yet

Every unmatched TSE function that `tools/round/stubs.py` finds no code of ours for has a stub in `ecourna/`,
`uenux2/` or `asn1/`: two comment lines generated where the evidence says the function sat, between `// <stubs>`
and `// </stubs>` markers. Code of ours, to the generator, is a body in our link, a record that falls on a line of
our code, or a slot of a vtable our link holds (`stubs.py --report` counts each reason). So a pending member's
trapping body (`units/README.md`) removes its stub. A non-virtual function we wrote that has no record, a
constructor for one, keeps its stub.

```
// <stubs>
// f7010 · 683 B · unmatched · name unknown
// void f7010(i32, i32) { ... }
// </stubs>
```

The first line gives the target function index, its body size and its status, then `placement: link order` when no
`std::source_location` record gives the function a line (its place in the file comes from the recovered link order, or
it follows the file's code), and `name unknown` when `splat/symbol_addrs.txt` names no function at that index. The
second is its signature: the demangled name, or `f<index>` and the wasm parameter types; the return type is `?` unless
the wasm signature settles it (`void`). For `f<index>`, `void` says only that the wasm function returns nothing: a C++
function that returns a class by value does that too, through a hidden first parameter that holds the result's address
(`docs/how-to-decompile.md` step 2). The file comes from the function's records, else from its class's vtable or vptr
(`ledger/vtables.json`, `ledger/func-attribution.tsv`); a function with neither is in its tree's `unplaced.cpp`,
grouped by link-order cluster. A file the evidence names that does not exist yet holds its stubs only. The stub files
and `unplaced.cpp` are never compiled: no fragment lists them, and the generator never creates a `.cxx` in `asn1/`,
where `mk/asn1.mk` compiles every one. A stub is a comment, never the target's code: there is no `INCLUDE_ASM`, and an
unmatched function's bytes come from its piece in the splat build
([../docs/reference/include-asm.md](../docs/reference/include-asm.md)).

Stubs only take blank lines (the ones that keep a decompiled function's throw sites at their recorded lines) or go
after the end of a file, so no object changes; where a run of blank lines is too short for the stubs its records put
there, a `#line N` after them gives the next line its number back, the same directive a gap ends with (below). The
markers say the runs are generated: `make -C src stubs` rewrites them (`splat-scan` and `splat-match` run it), and
`make -C src stubs-check` (CI) fails on a stub that is missing, extra, stale or out of place. To edit a file that holds
stubs or gaps, `python3 -I -B tools/round/stubs.py --strip <file>` turns them back into blank lines first;
[../docs/how-to-decompile.md](../docs/how-to-decompile.md) step 3 has the rest.

## Gaps: the original lines not recovered

A decompiled function's throw sites must stay on the lines their `std::source_location` records give, so the lines of
TSE's file we have not written (comments, includes, other formatting, inlined helpers) keep their place. The same
generator, `tools/round/stubs.py`, writes each such run as a gap: one blank line, a comment naming the original lines
the run stands for, and a `#line` that gives the next line its original number:

```cpp
namespace ecourna::app::dados {

// (original lines 9-16: content not recovered)
#line 17
CNumeroCPF::CNumeroCPF(const std::string& numeroCPF) : IIdentificadorEleitor(TTipoIdentificadorEleitor(TipoIDCPF), numeroCPF, 11)
```

The comment is ASCII (a `-` between the numbers: Latin-1 has no dash), indented like the line under it, and sits on
the first line it names; the `#line` names the line after the last. A `#line` without a file name keeps the file
name, so every object compiles byte-identical, and the compiler numbers the lines as TSE's file did: an error or a
warning below a gap names the original line, which can be larger than the line number an editor shows.

Which runs are gaps: 3 or more empty lines of code (outside comments, directive continuations and preprocessor
conditionals) with a line before and after them, and below them a line that a record of the file names
(`ledger/source-locations.tsv`). Runs of 1 or 2 lines are formatting and stay blank. The threshold comes from the
sources: of the 146 runs of 2 blank lines, 119 have no record below them, while 124 of the 126 runs of 3 or more hold
a record's line in place. The other two hold none and stay blank: 3 lines in `asn1/asn1_useful.cxx`, asnparser's
own spacing, and 8 in `ecourna/app/dados/asn/midias/cconversorinformacaomidia.cpp`, above code no record names. The
124 gaps are in 61 files, 26 of them inside a function or class body.

`make -C src stubs` writes the gaps after the stubs (a stub in a long run leaves a gap above and below it), and
`make -C src stubs-check` fails on a run of 3 or more blank lines left blank, a gap comment where the generator writes
none, a range that does not end at its `#line`, a comment or a `#line` without the other, and a comment that no
longer sits on the first line it names. That last one means lines were added or removed above it: the `#line` holds
the code below where it was, and `make -C src stubs` writes the comment again over the lines that are left, which is
what a recovered function written into a gap needs. To move the code below instead, `--strip` the file first and edit
the blank lines. Code added between a `#line` and the line under it moves that code; `TMPDIR=/tmp make -C src
check-srcloc` lists every record that moved, after a link.

## Trying a candidate: tools/round/try-tu.sh

```
tools/round/try-tu.sh --print-flags cand.cpp|dir                                  # each TU's compile command, nothing run
TMPDIR=/tmp tools/round/try-tu.sh --single <target fn> --extract export:<mangled> cand.cpp   # one function, linked alone
TMPDIR=/tmp tools/round/try-tu.sh --symbols cand.cpp                                       # the mangled symbols it defines
TMPDIR=/tmp tools/round/try-tu.sh cand.cpp|dir [--flags '…'] [--targets idx.txt] [--roots mine.cpp|none]  # the whole link
```

[../docs/how-to-decompile.md](../docs/how-to-decompile.md) uses both modes in the decompile loop. What try-tu does:

- **Flags.** Each candidate TU compiles with the flags the Makefile builds that source with (plus `--flags`): try-tu
  asks `make -n` for the recipe of the object the source would become, so a TU gets its tree's rule
  (`ECOURNA_CXXFLAGS` with `mk/ecourna-flags.mk`'s additions and the `app/dados` `-fno-PIC`, `UENUX2_CXXFLAGS`, the
  ASN.1 runtime's and modules' rules from `mk/asn1.mk`) or, for a landed unit, its fragment's own rule. The path under
  `src/` names the component; a TU outside `src/` needs `--component <dir under src/>` (`ecourna/app/dados`,
  `uenux2`, `asn1`, …), the directory it would land in.
- **`--single`** compiles the TU alone, keeps one exported function through the toolkit's rank adapter, and scores
  it against the target. A lone link makes other inlining and dead-argument decisions than the whole program, so
  use it to iterate on a body and the whole link to conclude. Several TUs are rival candidates, ranked against
  each other, never linked together: a callee in another TU stays an import. Undefined functions become imports,
  but undefined data fails the link; `--flags '-Wl,--unresolved-symbols=ignore-all'` lets it through
  (`docs/how-to-decompile.md` step 4).
- **`--component`** overrides the path of every TU of the run, not only the ones outside `src/`. It is also the
  way to compile a `src/units/*-pending.cpp`, which has no rule of its own that try-tu can find.
- **The whole link** generates a roots TU (`gen_try_roots.py`: every strong function symbol of the objects is
  address-taken from a static constructor, because nothing in the skeleton calls a candidate and wasm-ld drops what
  nothing references; `--keep-weak` roots the inline and template instantiations too, `--roots your.cpp` supplies
  real callers instead, `none` skips it), then links the whole skeleton with the TUs through
  `make link EXTRA_OBJS=… OUT=build/out/try/<name>/wasm/… OUT_NAMED=…` (the libraries prebuilt, nothing of theirs
  rebuilt), runs the census against the target, and writes `build/out/try/<name>/report.md`: the census tiers per
  owner and per component against `ledger/functions.tsv`, the functions that moved, and for each target function
  in `--targets` (default: every target function whose best pair in the try link is one of the TU's own symbols) its tier
  before and after, the skeleton function it pairs with and the first differing rows. `unmatched-symbols.txt` lists
  the TU functions that reached the link and paired with no target function. try-tu never writes `ledger/`.
  A census tier of `none` does not mean the body differs. `tools/round/link-map.sh build/out/try/<name>/wasm/vota_web`
  maps the try link the way `make -C src link-map` maps the plain one, and `pnpm wasm splat-remap` then compares
  one function of it with the target (`docs/how-to-decompile.md` step 5).

`--skip-units <fragment>.mk` leaves a landed fragment out of the link, with its companion objects, so its TUs can be
tried again without a duplicate symbol.

Times: `--single` takes about 7 s on an idle machine and the whole link about a minute (compile, both links,
census). Both are mostly docker runs, so they grow with the machine's load: `--single` takes about 35 s while a fresh
clone's setup runs and ten minutes at a load average above 100, and two whole links running at once take about seven
minutes each.
