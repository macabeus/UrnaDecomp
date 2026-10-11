# How to decompile a function

This page walks one target function from its disassembly to a match in `splat/matched.txt`. It assumes a working
checkout: the pinned image built, `asmlift-wasm/` installed and the skeleton linked once
([getting-started.md](getting-started.md)). Terms in *italics* are defined in the [glossary](glossary.md).

Commands run from the repository root, except `pnpm wasm …`, which runs from `asmlift-wasm/`. Every command that
compiles goes through docker and needs `TMPDIR=/tmp` on macOS, because docker cannot mount the default
`/var/folders` temp directory; the commands below carry it.

The example throughout is target function 1931, `comum::CRdvVota::Candidato`, which landed as
`src/units/13-uenux2-crdvvota.mk`. It is already matched, so repeating it needs the landed copy out of the way
(step 5 says how).

To match several functions at once with Claude Code, run `/decompile-functions`
([.claude/commands/decompile-functions.md](../.claude/commands/decompile-functions.md)): it runs this loop in one
worktree per function, verifies each match with a second agent, lands the matches as step 10 says, and collects
where this page fell short.

## The loop at a glance

| step | command | done when |
| --- | --- | --- |
| 1. pick | `splat/scan.tsv`, `ledger/source-locations.tsv` | you have a target index and know its source file |
| 2. read | `pnpm wasm fn` / `pnpm wasm bundle` | you know what it calls, loads and belongs to |
| 3. lift | `pnpm wasm lift … --check` | you have compilable C++ to rewrite |
| 4. iterate alone | `tools/round/try-tu.sh --single` | the body is close enough to try in the whole program |
| 5. conclude in the whole link | `tools/round/try-tu.sh <tu> --targets …` | the report pairs the target function with your symbol, or `splat-remap` against the try link says `IDENTICAL` |
| 6. land | sources under `src/`, a fragment in `src/units/` and its row in the README's table, `make -C src link` | the skeleton links with the unit |
| 7. census | `make -C src census` | `ledger/functions.tsv` and its snapshot are rewritten |
| 8. symbols | `make -C src splat-symbols` | the symbol file names the unit's new functions and data |
| 9. match | `make -C src splat-match` | `git diff splat/matched.txt` shows your function, and its stub is gone |
| 10. build and commit | `make -C src splat`, `git commit` | the build prints the target's sha256 with the new match count |

Steps 6 to 10 run in this order, and this page is the one place that states it. Each step reads what the one
before it wrote: the census pairs the new link with the target; the symbol file and the scan take names and
candidates from that census (`ledger/functions.tsv`); the new matches come from the scan; the build verifies every match.

## 1. Pick a function

A *function index* counts imports first: the target's imports are 0–135, so its first defined function is 136
([the subject](glossary.md#the-subject)).

The work list is `splat/scan.tsv`, committed with every new match: one row per target function with its verdict
against our current link ([status.md](status.md) "What is left" counts them). Its columns:

| column | holds |
| --- | --- |
| `index` | the target function index |
| `symbol` | the name of our candidate body, or the target's own symbol when there is no candidate |
| `ours` | the candidate's function index in our plain link, `-` when there is none |
| `source` | how the scan found the candidate: `symbol` (the symbol file names it), `ledger` (the census paired it) or `content` (its re-pointed bytes are the target's) |
| `verdict` | `match`, `differs`, `unresolved` or `no-candidate` ([scan verdicts](glossary.md#the-splat-build)) |
| `detail` | for `differs`, the first instruction that differs on both sides, with what our operand resolves to; for `unresolved`, the operand with no target counterpart |
| `size` | the target body's size in bytes |
| `ledger_kind` | the census *tier* of the target function's best pair |
| `group` | its component, else its owner (`app`, `unknown`) |
| `tried` | how many candidates the scan re-pointed |
| `object` | the input our candidate is compiled from, as the link map names it |

Start with TSE code: its `differs` rows have a body of ours already, and the work is C++ (the components are
listed in [status.md](status.md) "By component"):

```sh
python3 -I -B tools/round/pick_pool.py --differs | less     # index, symbol, detail, size, group, object; smallest first
```

It keeps the `differs` rows of TSE groups whose body was compiled from `src/uenux2`, `src/ecourna` or `src/asn1`
(the `object` column). The group alone would also list library bodies that only TSE code calls, since `group` says
who uses a function (below): over a third of the TSE-group `differs` rows are not TSE code but RHVoice, OpenSSL,
7-Zip and libc++ bodies, binaryen's merged ones and the `src/app` stand-ins', and they are most of the smallest.

The other pool is the functions not written yet, which have a comment stub in their tree:

```sh
python3 -I -B tools/round/pick_pool.py src/ecourna      # or src/uenux2, src/asn1, --tse for all three; --min/--max in B
```

It prints `index verdict size group file:line` for each stub of 40 to 600 B, smallest first. Their verdict is mostly
`no-candidate`: the scan found no body of ours for it, as the symbol file names none, the census paired none, and no
unmatched body of ours re-points into it. `no-candidate` is not the same as unwritten. A function can be written and still pair with nothing: target 1883,
`CSqlResultSet::VerifyColumnIndex`, is defined in `src/ecourna/api/sql/sqlite/csqlresultset.cpp` and is
`no-candidate`. The stubs tell the two kinds apart. `tools/round/stubs.py` writes no stub for a function it finds
code of ours for, and `python3 -I -B tools/round/stubs.py --report` counts those by reason. So a `no-candidate` row
with a stub is usually unwritten. A row without one usually has code to fix, or is a pending member (step 6).

The `group` column is the function's *component*, and a component says who uses a function, not where its code
lives. 238 uenux2 functions are in group `ecourna`, because they throw ecourna's `CBaseError<…>` template with a
uenux2 enum (`1708 comum::CCargos::Next`, for one); their stubs are in `src/uenux2/`. Go by the tree that holds
the stub, not by the group ([component and owner](glossary.md#component-and-owner)).

A `differs` row of a library component is a different job: see [Library functions](#library-functions) below
before taking one.

Then find the function's source file. The target's `std::source_location` records usually name the file, its line
and the full signature:

```sh
awk -F'\t' '$6 ~ /(^|,)1931(,|$)/ {print $2, $3, $5}' ledger/source-locations.tsv
```

The path under `/home/rubio/tse/uenux2/` or `…/libecea1da310e5107/b/src/ecourna/` is where the file goes under
`src/uenux2/` or `src/ecourna/`, and that tree's flags compile it; the *component* (`ledger/func-attribution.tsv`)
can name another tree ([component and owner](glossary.md#component-and-owner)).

A TSE function not decompiled yet usually has a *stub* in that tree, a generated comment where the evidence says the
function sat ([src/README.md](../src/README.md) "Stubs"). Find it by its index:

```sh
LC_ALL=C grep -rn '^// f7010 ' src/uenux2 src/ecourna src/asn1     # the files are Latin-1: LC_ALL=C reads them as bytes
# src/uenux2/src/app/vota/eleitor/cvotacaostateaudio.cpp:11:// f7010 · 683 B · unmatched · name unknown
```

(In an agent's shell `grep` can be a shell function that prints nothing here; `/usr/bin/grep` is the real one.)

The line under it is the stub's signature. It names the function only when `splat/symbol_addrs.txt` does; today
no stub does, so each reads `f<index>(<wasm types>)`. Get the name from the records (the `awk` above), from the
vtable slot the `fn` readout gives (step 2), or from `ledger/vtables.json`. A record names the function that holds
the throw, and that can be an inlined callee: f7010 has records for both `CVotacaoStateAudio::ProcessTick` and
`CVotacaoStateAudio::IniciarEsperaFimAudio`, so one of the two is inlined into it.

A stub marked `placement: link order` has no record that gives it a line: its file comes from its class's vtable
or vptr. A stub in a tree's `unplaced.cpp` has no evidence for any file (step 3 says where its code goes then).
In `src/ecourna/` every stub is one of these two kinds. Most ecourna functions that have records are written
already, so the record leads to code, not to a stub. A stub can also sit over code we have: `stubs.py` cannot see
a non-virtual function that has no record. The stub f9449 at the end of `csqlresultset.cpp` is the
`CSqlResultSet` constructor that the same file defines. Before you write a stub's function, read its `fn` readout
and search our sources for that class's constructor, destructor or method.

A stub is only a comment: the target's bytes never enter the build, and there is no `INCLUDE_ASM`
([reference/include-asm.md](reference/include-asm.md) says why). Until your C++ replaces it, the function's bytes
come from its piece in the splat build.

## 2. Read it

```sh
cd asmlift-wasm
pnpm wasm fn ../target/vota_web_wasm.wasm 1931
```

`fn` prints the disassembly, a *folded readout* (the stack replayed into expressions), the strings it loads, its
callers and callees with their components and best pairs, and the vtable class and slot it fills
([the toolkit](glossary.md#the-toolkit)). The "strings referenced" list reads every `i32.const` in the data range
as a pointer, so an error code can show up as a string (1931's `4725` is an error code, not "Tipo de
identificador…"). A call made inside a `try` goes through an emscripten `invoke_<sig>` import whose first argument
is a table slot, and `fn` lists such a slot as a string too, with only `import:invoke_…` among the callees. The
lifter (step 3) names the function behind each slot: `invoke_iii(6708 /* &f339 */, …)`. `pnpm wasm bundle`
writes the same readout for a list of functions into `ledger/bundles/<name>.md`, which is how to read a whole
class at once. `ledger/classes.json` and `ledger/vtables.json` give the class's RTTI name, bases and virtual slots,
the starting point for its layout.

The wasm parameters follow the C++ ABI. A member function takes `this` first. A function that returns a class by
value returns nothing in wasm and takes the address of the result as a hidden parameter before everything else, so
target 9131, `MunicipioZona CConversorMunicipioZona::DoConverte(const CMunicipioZona&) const`, has the wasm type
`(i32 i32 i32)->()`: `p0` is the result, `p1` is `this`, `p2` is the argument. A stub's `void`
says only that the wasm function returns nothing. For a virtual function, take the signature from the base class
that declares the slot (the `fn` header names the vtable class and slot). For the ASN.1 converters there are two
bases, and the namespace of the base's RTTI name in `ledger/classes.json` says which: ecourna's
`ecourna::api::asn::IConversorASN<ENTIDADE, DADO>` in `src/ecourna/api/asn/iconversorasn.hpp` (slot 3 is
`DoDeconverte`), and comum's `comum::asn::IConversorASN<ENTIDADE, DADO>` in
`src/uenux2/src/app/comum/asn/iconversorasn.h` (slot 3 is `DoDesconverte`). Slot 2 is `DoConverte` in both.

## 3. Start from the lifter

```sh
TMPDIR=/tmp pnpm wasm lift ../target/vota_web_wasm.wasm 1931 -o /tmp/lift --check
```

This writes `/tmp/lift/1931.cpp`, which `em++` compiles. It is a starting point, not code to commit. Rewrite it
into plausible original source: the project's real classes and headers, source types for integers and pointers,
no `volatile` stores, no empty `asm` barriers, no `goto` that a loop or an `if` would express. Save the result where
it will land, at the path the `std::source_location` record names: here
`src/uenux2/src/app/comum/dados/crdvvota.cpp`. Its path then tells `try-tu.sh` its component and flags.
`asmlift-wasm/README.md` "lift" lists each workaround the lifter emits and how to take it back out.

The lifter names a callee from the ledger's pairs, not from the symbol file, so a callee can be `extern "C" …
f219(…)` although `splat/symbol_addrs.txt` names it (`_ZN4ASN18SEQUENCEC2EPKv = func:219;`, the
`ASN1::SEQUENCE` constructor). Look a callee up there by `func:<index>;` before you guess.

**A callee can be a twin.** binaryen merges byte-identical functions, in the target and in our link alike, and keeps
one name for the body. So a callee's name in `splat/matched.txt` or the symbol file can be another function's that
compiles to the same bytes: 760 is matched as `std::vector<std::pair<unsigned long, unsigned long>>::push_back(&&)`,
and in the target it is also the `push_back` of another vector of an 8-byte element: 11361, the converter of
`comum::asn::CConversorSituacoesEleicoes`, which builds a `std::vector<comum::md::CSituacoesEleicoes>`, calls 760
(`pnpm wasm fn ../target/vota_web_wasm.wasm 11361`, from `asmlift-wasm/`, shows `call 760` at +0x93). Write the call your source
makes. If your instantiation compiles to the same bytes, our link merges it into the matched body and the call
re-points to the target's function; if it does not, the target has no such body and the caller cannot match
with that call. When your object comes first in the link, its symbol becomes the survivor's name and the existing
match line has to follow it (step 10, "A twin whose name moved"; [facts/splat.md](reference/facts/splat.md) §S6).

**When no record names the file**, as for every stub in `unplaced.cpp`, choose the path the way the landed
units do, and say why in the fragment's header comment (step 6):

1. a header of the class that already exists names the directory (`src/ecourna/app/dados/asn/resultadournacadastro/`
   for `CConversorDadosComparecimentoCifrado`);
2. else the directory where the class's family has records or landed sources: the `CConversor*` converters
   are in `app/dados/asn/` (or its subdirectory for their family), and the data classes they build in
   `app/dados/`;
3. the file name is the class name in lower case (`cconversormunicipiozona.cpp`, `cmunicipiozona.hpp`).

The `unplaced.cpp` comment over a cluster (`neighbours' file ecourna/app/dados/asn/…`) comes from
`ledger/tu-clusters.tsv`, which guesses the class name under a fixed directory. Take the file name from it, and
the directory from the rules above.

**Keep every throw on its record's line.** A decompiled function's `std::source_location` records are bytes of the
target, so each throw site must sit at the line and column its record gives; above it, blank lines and *gaps* hold
the place of the code not written yet. A gap is a generated comment and a `#line`,
`// (original lines 9-16: content not recovered)` over `#line 17`, that stands for a run of blank lines
([src/README.md](../src/README.md) "Gaps"). The compiler numbers lines through each `#line`, so an error or warning
below a gap names the line of TSE's file, not the line an editor shows. If the file holds stubs or gaps, turn them
back into blank lines before editing it: `python3 -I -B tools/round/stubs.py --strip <file>` moves no line. Write the
function where the stub or the gap was, delete the blank lines it replaces, add some where a throw would land too
early. `make -C src stubs` (step 9 runs it) writes the stubs and gaps again around it, each gap comment over the
blank lines that are left, and `TMPDIR=/tmp make -C src check-srcloc` (after a link) lists every record of ours that
is not the target's.

The record's function name is bytes of the target too: clang's pretty-function text, with the typedef names and
qualifiers the source spelled. Spell the definition's return and parameter types as the record does. 3663's record
(`awk -F'\t' '$6 ~ /(^|,)3663(,|$)/ {print $5}' ledger/source-locations.tsv`) reads `static void
api::CBmpConversor::InvertColors(unsigned char *, const int, const int)`, so its definition takes `unsigned char*`
and `const int` by value, although the same tree spells bytes `uebyte` elsewhere; 144 records do spell `uebyte`
(`cut -f5 ledger/source-locations.tsv | /usr/bin/grep -c uebyte`). [reference/app-flags.md](reference/app-flags.md)
"The error records are std::source_location, not a macro" has the background.

**A stub-only file.** A file the evidence names that does not exist yet holds its stubs only, written from line 2
on, not at their records' lines ([src/README.md](../src/README.md) "Stubs"). `--strip` leaves next to nothing of
it, so there is no "where the stub was": lay the file out yourself so that each throw lands on its record's line,
with blank lines where the original lines are unknown. Before step 9 writes gaps into it, `awk 'NR==<line>' <file>`
prints the line a record names; after a `--strip` of a file that has gaps, it does again
(`awk 'NR==60' <file>` prints `InvertColors`' throw in the stripped `src/uenux2/src/api/gui/cbmpconversor.cpp`).

**Save TSE sources as Latin-1.** TSE's files are ISO-8859-1, and their string literals are bytes of the target: an
editor that saves `"inválido"` as UTF-8 changes the data the census and the splat scan compare. `.editorconfig`
declares the charset for `src/uenux2/` and `src/ecourna/` (with the few exceptions it lists), and CI's
`tools/round/check_encoding.py` fails on a file that was re-encoded. Write a new file in Latin-1 too when its
strings have accents.

The compile line in the lifted file's header is the flag set of the tree the function's code comes from, and the
line under it says what decided (`liftFlags` in `asmlift-wasm/src/lift.ts`): the function's `std::source_location`
records when they name one tree (a path under `/home/rubio/tse/uenux2/` is uenux2, one under the conan package's
`…/b/src/ecourna/` is ecourna), else the tree whose file holds its stub (`src/uenux2`, `src/ecourna`, `src/asn1`),
else its *component*. The component says which code calls a function, not where it is written, so a uenux2 function
in group `ecourna` (step 1) still gets uenux2's flags from its record. After
`TMPDIR=/tmp pnpm wasm lift ../target/vota_web_wasm.wasm 1260 -o /tmp/lift` (1260 is `api::CDirReader::NextEntry`,
group `ecourna`), `head -3 /tmp/lift/1260.cpp` prints:

```
// wasm function 1260
// em++ -O2 -fPIC -std=c++20 -DNDEBUG -fno-strict-aliasing
// uenux2's flags: a std::source_location record names /home/rubio/tse/uenux2/src/api/util/cdirreader.cpp
```

A function with no record and no stub outside `unplaced.cpp`, or whose records name both trees, gets its component's
flags, and the third line says so (`component ecourna's flags: no record or stub names its tree`). The lifter's copy
of the flags has no per-directory `-fno-PIC` either. The next steps use `try-tu.sh`, which reads the flags from
`src/Makefile` by the source's path; trust those.

## 4. Iterate on the function alone

```sh
TMPDIR=/tmp tools/round/try-tu.sh --single 1931 --extract export:_ZNK5comum8CRdvVota9CandidatoEhjh \
  src/uenux2/src/app/comum/dados/crdvvota.cpp
```

This compiles your translation unit with the flags of the *compile setup* its component uses, links the one function
alone through the toolkit's *rank adapter*, and scores it against the target: about 7 s on an idle machine, minutes
under heavy load. The flags come from the recipe `src/Makefile` has for the object the file would become. A file
outside `src/` needs `--component` with the directory under `src/` it would land in (`uenux2/src/app/comum/dados`,
`ecourna/app/dados`, `asn1`, …). `--print-flags` shows the compile command without running it.

The lone compile and link is one `em++` with `-fexceptions`, as the build's link is. uenux2 code is compiled
without it (no landing pads, [facts/app-flags.md](reference/facts/app-flags.md) §F6), so for a TU whose own flags
lack `-fexceptions` `--single` adds `-fignore-exceptions`, and the `flags:` line it prints ends with it: its throw
sites call `__cxa_throw` directly, as in the build, instead of growing `call import:invoke_…` and
`__resumeException` rows ([facts/tool-traps.md](reference/facts/tool-traps.md) §X6).

Several TUs after `--single` are rival candidates, ranked against each other, not one program: a callee defined
in a second TU stays an import (`call import:<symbol>`), and the second TU is dropped when it does not define the
`--extract` symbol. A body that is identical up to such a call is ready for step 5.

The lone link turns undefined functions into imports, but not undefined data. A TU that uses a data object
defined elsewhere (an ASN.1 entity's `theInfo`, a vtable or typeinfo of another class) fails with
`wasm-ld: error: … undefined symbol: …`. Let the linker leave them unresolved:

```sh
TMPDIR=/tmp tools/round/try-tu.sh --single 9131 \
  --extract export:_ZNK7ecourna3app5dados3asn23CConversorMunicipioZona10DoConverteERKNS1_14CMunicipioZonaE \
  --flags '-Wl,--unresolved-symbols=ignore-all' src/ecourna/app/dados/asn/cconversormunicipiozona.cpp
# score 3 over 20 rows (same 17, replaced 3 [3 operand-only], inserted 0, deleted 0)
```

The rows that differ only in an operand are the unresolved data address (`i32.const 0`), the callee as an import,
and our own data addresses. They are expected in a lone link, and target 9131 is matched.

`--extract export:<symbol>` takes the function's mangled symbol. To find it, compile the TU and list what it
defines:

```sh
TMPDIR=/tmp tools/round/try-tu.sh --symbols src/uenux2/src/app/comum/dados/crdvvota.cpp
# _ZNK5comum8CRdvVota9CandidatoEhjh	comum::CRdvVota::Candidato(unsigned char, unsigned int, unsigned char) const
```

Each line is the mangled symbol and its demangled form; pick the one whose demangled form is the target function's
signature (the `std::source_location` record, or the ledger's `best_name`). A symbol the link already references is
also in `build/out/wasm/vota_web/symbols.{mangled,demangled}.txt`, two line-aligned lists `make -C src link` writes.

A function linked alone gets different inlining and dead-argument decisions from the same function inside the
program, so a score of 0 here is a reason to try the whole link, not a match. The reverse holds too: target 1931 is
matched, yet its body linked alone still differs from the target's.

**When only a flag closes the gap.** A body that differs in a way no C++ form you can find explains (a load the
target repeats, a branch where ours has a `select`) may need a flag its tree's set lacks. Try it with `--flags`
here, then find out whether the flag is the tree's: a flag is decided for a whole tree, on a site no source change
can explain, never on the one function that needs it. [facts/app-flags.md](reference/facts/app-flags.md) §F9 item
8 has a trap here (a reload that an out-of-line call gives as well), and §F6 the evidence for uenux2's
`-fno-strict-aliasing`. A flag set changes in `src/Makefile` (or `src/mk/ecourna-flags.mk`) with its evidence in that facts
file, and every match must still verify in a private link with it (§F9 item 6 has the command). A fragment may carry a target-specific flag only where the evidence is
per TU, as `-fno-PIC` is (§F5), and then the objects' flag set must be a setup of
`asmlift-wasm/benchmark/dataset/setups.json`, as `ecourna-nopic` is: `pnpm bench setups --check` fails on an
object that compiles with flags no setup has ([asmlift-wasm/benchmark/README.md](../asmlift-wasm/benchmark/README.md)
"The compile setups"). If neither holds, leave the function pending and report the measurement.

## 5. Conclude in the whole link

```sh
echo 1931 > /tmp/targets.txt
TMPDIR=/tmp tools/round/try-tu.sh src/uenux2/src/app/comum/dados/crdvvota.cpp src/uenux2/src/app/comum/dados/md/rdv \
  --targets /tmp/targets.txt --skip-units 13-uenux2-crdvvota.mk
```

A directory stands for the `.cpp` files in it. This links the whole *skeleton* with your TUs, keeps their functions
alive with a generated *roots TU*, runs the census against the target and writes `build/out/try/<name>/report.md`:
per target function in `--targets`, its tier before and after and the first rows that still differ. It takes about
a minute on an idle machine (57 s at a load average of 40) and never writes `ledger/`; two whole links running at
once take about seven minutes each. `--skip-units <fragment>.mk` leaves a landed unit out of the link so its TUs can be
tried again; it leaves out all of the unit's sources and companion files, so pass every one of them (here
`crdvvota.cpp` and the `md/rdv` files it calls). A new function does not need it.

**A class with virtual members.** The TU that defines a class's first non-inline virtual member emits its vtable,
and the vtable needs every virtual member defined. A member you have not written goes in a `-pending.cpp` with a
trapping body (`src/units/README.md` "Pending members"), and the vtable needs a root that takes its address. try-tu
has no compile rule for a path under `src/units/`, so pass `--component` with the directory the class lands in;
it puts every TU of the run there, with that directory's flags. The roots file goes through `--roots`. Here is the
landed unit 16 tried again:

```sh
echo 9131 > /tmp/targets.txt
TMPDIR=/tmp tools/round/try-tu.sh --name munzona --component ecourna/app/dados/asn \
  src/ecourna/app/dados/asn/cconversormunicipiozona.cpp src/units/16-ecourna-cconversormunicipiozona-pending.cpp \
  --roots src/units/16-ecourna-cconversormunicipiozona-roots.cpp --targets /tmp/targets.txt \
  --skip-units 16-ecourna-cconversormunicipiozona.mk
```

Because `--component` moves every TU, it also moves the `std::source_location` paths of a TU from another
directory. Use it when the TUs share a directory's flags and the moved ones have no records.

The report's census *tiers* are diagnostics ([pairing and progress](glossary.md#pairing-and-progress)). A target function
paired with your symbol is worth landing and checking with the splat scan at any tier: most matched functions pair
only at `addr` ([status.md](status.md) "Diagnostics: the census tiers"). No tier is a match. The example shows both
sides of that: the report says `1931 343 B app/comum: none → none`, because a caller of `std::to_string` pairs at
`none` at depth 2 however exact its body is ([facts/tool-traps.md](reference/facts/tool-traps.md) §X5), and 1931 is
matched all the same. The unit-16 run above prints `9131 54 B app/ecourna: none → none` too, for a body that is
the target's once re-pointed.

**When the report pairs nothing, re-point against the try link.** Map it once, relink it with the data and the
table moved to learn what each of its constants is (the *operand kinds*, [reference/splat.md](reference/splat.md)
§3), then run `splat-remap` ([reference/splat.md](reference/splat.md) "Matching a function") on its wasm:

```sh
TMPDIR=/tmp tools/round/link-map.sh build/out/try/munzona/wasm/vota_web     # about 20 s
TMPDIR=/tmp tools/round/link-kinds.sh build/out/try/munzona/wasm/vota_web   # about 40 s
cd asmlift-wasm
pnpm --silent wasm splat-kinds ../build/out/try/munzona/wasm/vota_web/vota_web_wasm.wasm
pnpm --silent wasm splat-remap ../target/vota_web_wasm.wasm ../build/out/try/munzona/wasm/vota_web/vota_web_wasm.wasm \
  sym:_ZNK7ecourna3app5dados3asn23CConversorMunicipioZona10DoConverteERKNS1_14CMunicipioZonaE --against 9131
# ours 10517: re-pointed, 54 bytes; rewritten {"address":2,"func":1}; soft 0; operand kinds verified
# target 9131: IDENTICAL (2 ambiguous operands read as address content-repointed)
```

`link-kinds.sh` refuses a try link whose objects were rebuilt after it was linked: run it right after `try-tu.sh`.
To skip the two kind steps, pass `--no-kinds` to `splat-remap`; it then says `KIND-UNVERIFIED (no operand-kind
table)` and offers every operand every reading its value suggests. `IDENTICAL` there is the
signal to land. `splat-remap` reads the committed symbol file and none of the matches, so
`differs` or `UNRESOLVED` can still turn into a match once steps 8 and 9 add names; the scan in step 9 decides.
The `ours` index is the function's index in the try link; `pnpm wasm fn` on the try link's wasm with that index
shows its body beside the target's.

A TSE TU's `std::source_location` records name the target's files in a try link too: every uenux2 and ecourna
compile runs from the source's `/work/src/…` path, which the tree's `-ffile-prefix-map` rewrites to
`/home/rubio/tse/uenux2/…` or the conan path (`--print-flags` shows the compile line;
[facts/tool-traps.md](reference/facts/tool-traps.md) §X7).

## 6. Land the unit

1. Keep the sources at the paths the `std::source_location` records name, or that step 3 chose.
2. Write the fragment `src/units/<NN>-<component>-<what>.mk` with the next free `NN`, named by content: the source
   directory the files share, or the class. `src/units/README.md` describes what a fragment holds, its naming
   scheme, and the *roots*, *pending* and *externs* companion files. The generic rules make each object depend on
   its own source and on every header of its tree, so a fragment needs a compile rule of its own only for other
   flags ("What a fragment holds" item 3). A rule of your own compiles the TSE source from its
   `/work/src/…` path, as the generic rules do, so the tree's `-ffile-prefix-map` rewrites the file name its
   `std::source_location` records hold; a relative path is never rewritten, and
   `TMPDIR=/tmp make -C src check-srcloc` then fails with `UNMAPPED` and the record. Add the fragment's row to the
   table in `src/units/README.md`.
3. If nothing in the skeleton calls the unit yet, add a `-roots.cpp` that reaches each function the way the target
   does (address taken; at least two direct callers, counting the reconstructed ones; or none so it inlines;
   `src/units/README.md` "Companion files").
4. Link: `TMPDIR=/tmp make -C src link`. Do not pass `-j` to the link: its two outputs share one recipe. make does
   not rebuild an object whose flags changed but whose prerequisites did not. To see whether an edit of a source
   recompiles anything, count the compiles make would run:
   `make -n -C src link -W $PWD/<source> | grep -c 'em++.* -c '` (0 means the link would keep the old object).

## 7. Census

```sh
TMPDIR=/tmp make -C src census
```

This rewrites `ledger/functions.tsv` and saves a new snapshot in `ledger/history/` (about a minute). It prints
the paths of its reports; the target functions whose census tier moved are under `moved:` in
`build/out/census/progress.md`. Commit the new snapshot and `git rm` the previous one by name
(`ledger/README.md`). Expect thousands of changed lines in `ledger/functions.tsv` for a small unit: its
`best_index` column holds our function indices, and every function after the unit's place in the link shifts.
The next three steps read this census.

## 8. Symbols

```sh
TMPDIR=/tmp make -C src splat-symbols
git diff --stat splat/symbol_addrs.txt
```

The *symbol file* names target functions and data from evidence that includes the census's unique pairs, so a new
unit's functions and data get their names here. Run it after every landing that adds functions or data, and read
the diff: a name that moves can break existing matches (step 10 would say so). It also re-picks the arbitrary name
of byte-identical *twins*, so expect some churn in lines that have nothing to do with your unit; keep it, do not
pick lines by hand. Lines below the hand-edit marker survive; never add one whose only evidence is the match it
enables ([reference/splat.md](reference/splat.md) §2).

## 9. Match

```sh
TMPDIR=/tmp make -C src splat-match
git diff splat/matched.txt
```

`splat-match` scans every target function, re-points each candidate body of ours into the target's index and
address spaces, appends every new byte-identical match to `splat/matched.txt` as `<target index> <our symbol>`
(and renames a listed match whose symbol left our link to its twin: step 10, "A twin whose name moved"), rewrites `splat/scan.tsv`, regenerates the status blocks of `docs/status.md`
(`make -C src status`), and rewrites the stubs and gaps (`make -C src stubs`): a function with a match or a body of ours
has no stub, so yours goes, and a stub-only file whose last stub goes is removed. A pending member's stub goes too,
although its body is only a trap: `stubs.py` counts a function in a slot of a vtable our link holds as code of
ours. The scan still lists it as `no-candidate`; the `-pending.cpp` header is then the only note that it is left
to do, so name it there. For one function, `pnpm wasm splat-remap` shows the comparison directly;
[reference/splat.md](reference/splat.md) "Matching a function" has the command and explains `IDENTICAL`, unresolved
operands and the first differing byte. It applies none of the matches, so it can say `differs` for a function the
scan matches.

`splat-match` also reports a listed line whose function the symbol file names after another function of our link:
the body verifies, but the evidence says another function sits there, as with 11611, matched as
`wrapexcept<bad_weekday>::rethrow` while the target's `wrapexcept<bad_month>` vtable holds it
([facts/splat.md](reference/facts/splat.md) §S7). It changes nothing; find which side is right, and correct the
match line by hand when the evidence is.

A match the scan found by body has `content` in the `source` column of `splat/scan.tsv`. Once the match names the
function, a later scan writes `symbol` there, so a rescan of the committed state differs from the committed
`scan.tsv` in one such line per new match. That is not drift; commit what the generator writes.

A *match* is a promise that the build keeps producing that body. Never edit a piece, and never add a symbol-file
hand edit whose only evidence is the match it enables: a name whose only use is to make a function match proves
nothing.

## 10. Build and commit

```sh
TMPDIR=/tmp make -C src splat
```

The build substitutes our bytes for every matched function, prints the meter and fails unless the module's sha256 is
the target's, `8579ef8d1a93819152cecdbf52906d2e4dd0f57999cb2e1e8c0d1161cd1007e2`. If an existing match breaks
(*drift*), the build names it and the first differing instruction; fix the source until it verifies again
([reference/splat.md](reference/splat.md) "The drift rule").

It also fails when a match is *kind-unverified* (the shifted link cannot say what its constants are), and names
each one with the reason. Read it before anything else ([reference/splat.md](reference/splat.md) "Reviewing a
match"); raising `SPLAT_MAX_KIND_UNVERIFIED` in `src/mk/splat-sub.mk` is a decision for review, with the reason
written beside it. The *review warnings* list the literals of matches that may be a target number copied into the
source. For a TSE function, check that each one of yours is a number its source has a reason to hold.

**A twin whose name moved** is the one break that is not drift. When your unit brings in a body byte-identical to a
matched one (another class's deleting destructor, another instantiation), binaryen keeps the first in link order,
and if yours comes first, the matched symbol leaves our link. A build run before `splat-match` says `function 246
matched to _ZN21ModuloTiposEleitorais4FotoD0Ev: our link has no function _ZN21ModuloTiposEleitorais4FotoD0Ev`. Step
9's `splat-match` repairs it: when a listed symbol is missing from our link and the scan matches the same index under
another symbol (the survivor, whose body re-points into the target's bytes), it rewrites that line in place and
prints the rename:

```
…/splat/matched.txt: 0 matches added, 1 renamed
  line 7218: function 246 renamed _ZN21ModuloTiposEleitorais4FotoD0Ev → _ZN21ModuloEstadoGeralDefs26NumViasImpressasRelatoriosD0Ev: …
```

Any other missing symbol is left as written and printed (`… left as written, so the splat build fails on it`): that
one is drift or a deleted function, and yours to fix. Say in the commit message which lines were renamed
([facts/splat.md](reference/facts/splat.md) §S6). `git diff splat/matched.txt` shows each rename as a `-` and a `+`
line with the same index at the same place, and `git diff --word-diff splat/matched.txt` as one line,
`246 [-_ZN21ModuloTiposEleitorais4FotoD0Ev-]{+_ZN21ModuloEstadoGeralDefs26NumViasImpressasRelatoriosD0Ev+}`: a
rename, not a lost match. So count lost matches by index, not by line; against `main` (or any commit):

```sh
git show main:splat/matched.txt | cut -d' ' -f1 | sort > build/out/main-matched.idx
cut -d' ' -f1 splat/matched.txt | sort | comm -23 build/out/main-matched.idx -    # prints nothing when none is lost
```

Commit in one commit: the sources, the fragment and its companion files, the fragment's row in
`src/units/README.md`, the ledger files the census rewrote (with the `git rm` of the previous snapshot from step 7),
`splat/symbol_addrs.txt`, `splat/matched.txt`, `splat/scan.tsv`, `docs/status.md` and the stub and gap files step 9
rewrote (`git status src/`). CI checks that the match list, the scan and `docs/status.md` agree
(`python3 -I -B tools/round/status.py --check`) and that every stub is the one the evidence gives and every gap the one
the generator writes (`python3 -I -B tools/round/stubs.py --check`, also `make -C src stubs-check`). Run both before
you commit, with `python3 -I -B tools/round/check_encoding.py` (step 3) and, from `asmlift-wasm/`,
`TMPDIR=/tmp pnpm test:bench`: the benchmark's tests check that every object compiles with the flags of a setup in
`asmlift-wasm/benchmark/dataset/setups.json`, which a flag added in a fragment breaks while every other check passes
([src/units/README.md](../src/units/README.md) "What a fragment holds").

**Landing onto a moved main.** When another unit landed after your branch started, merging conflicts in every
generated file and in the census snapshot, which both branches renamed. Resolve only the files people write:
`src/units/README.md` keeps both rows, and `splat/matched.txt` keeps both branches' match lines (step 10 checks
every one again). The stub file keeps both branches' removals. Every other generated file takes one side whole, with
`git checkout HEAD -- <file>` (it stages that side): a `git add` of a conflicted file stages its conflict markers.
The census snapshot is a rename/rename conflict (`DD` on the old name, `AU` and `UA` on the two new ones), and git
has already merged both snapshots into your side's name, markers included. Keep one name and drop the others from
the index:

```sh
git checkout HEAD -- ledger/history/<your snapshot>
git rm -q --cached ledger/history/<the old snapshot>
git rm -q ledger/history/<the other branch's snapshot>
```

Then run steps 6 to 10 again on the merge: link, census (`git rm` the snapshot you kept, add the new one), symbols,
match, build and the checks. Commit it all as the merge commit.
Two branches that each took "the next free NN" collide too; `src/units/README.md` "Names" says how to avoid that.

## Library functions

Most `differs` rows belong to upstream libraries (OpenSSL, SQLite, libc++, 7-Zip, RHVoice, …). We do not write
their C++: it is built from the library's own sources at the version `third_party/MANIFEST.md` pins. When such a row
differs, the code is usually right and something around it is not. The scan's `detail` column says which:

| `detail` says | what is usually wrong | how to look into it |
| --- | --- | --- |
| `an address of ours: … (.data) has no target symbol`, `… (.bss) has no target symbol` | the symbol file has no name for a data object the body uses, and its content does not place it either: what follows the `;` says why its re-pointed bytes did not (not in the target, several places of a writable object, too little content); a `.bss` object is zeros, so only a name places it | find independent evidence for that object (RTTI, a unique string or content, a census pair) and see whether `make -C src splat-symbols` picks it up; [reference/splat.md](reference/splat.md) §2 lists the evidence kinds and §3 how content places an object |
| `a table slot of ours: X has no target function` | the shifted link says the operand is a pointer to our function X, and the symbol file names no target function for X | the same as a callee with no target function: X's own match or name comes first (step 8) |
| `a table slot of ours: the target function fills N slots` | in a kind-unverified function, the operand may be a pointer to a function the target's table holds in more than 64 slots (binaryen's merged empty function fills 126); without its kind, re-pointing offers a function's slots only up to 64 | the source is not at fault: the check cannot re-point such an operand ([reference/splat.md](reference/splat.md) §3 "Operand kinds") |
| `the address resolves (symbol)/(string) to another place` | our data sits elsewhere: a different string merge, a missing or extra object, or another version's constants | compare the two strings or objects at the addresses the detail prints; check the version evidence in `third_party/MANIFEST.md` |
| `target i32.const N \| ours i32.const M` with small numbers | a table slot: the function-pointer table holds functions in another order, because another set of functions is address-taken | the roots TUs (`src/app/roots*.cpp`, the units' `-roots.cpp`) and the link order decide the table; [reference/link-order.md](reference/link-order.md) |
| `target call X \| ours call Y` | the callee resolves to another target function: a callee without a name, a twin, or an inlining difference | `pnpm wasm splat-remap … --against <index>` prints how every operand resolves |
| `the locals`, or an instruction that differs in kind | codegen: the compile flags (`-O` level, `-fPIC`, `-DNDEBUG`), the library version, or inlining | the library's flags and their evidence are in `docs/reference/facts/libraries.md` and `tools/libs/<lib>/census.json`; a root that exports or calls a function once changes what binaryen inlines ([facts/tool-traps.md](reference/facts/tool-traps.md) §X1) |
| `read-only object: bytes not in the target` | our constant data is not the target's: another version or another configuration (`#define`s) | `tools/libs/discriminate.py` lists the target functions that tell two builds apart |

To compare the two bodies side by side, read the target with `pnpm wasm fn ../target/vota_web_wasm.wasm <index>`
and ours with `pnpm wasm fn ../build/out/wasm/vota_web/vota_web_wasm.wasm <ours>` (the scan's `ours` column). A fix
to a library row is a change of flags, roots, link order or symbol evidence, so it can move many rows at once, and
it is checked the same way: link, census, symbols, match, splat build. If you are new to the project, start with
TSE functions; the library rows need the background in [reference/toolchain.md](reference/toolchain.md) and
`docs/reference/facts/`.
