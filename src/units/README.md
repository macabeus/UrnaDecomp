# src/units/ — one fragment per landed unit

A unit is a set of reconstructed translation units that landed together. Its fragment, `<name>.mk`, is a piece of
Makefile that `src/Makefile` reads: it appends the unit's sources to `ECOURNA_SRCS` (paths relative to
`src/ecourna`, compiled with `ECOURNA_CXXFLAGS`) or `UENUX2_SRCS` (paths relative to `src/uenux2`, compiled with
`UENUX2_CXXFLAGS`, the object keeping the path), and adds whatever the unit needs beyond that: its own compile rule
(`-fno-PIC`, or compiling from the `/work` path so `-ffile-prefix-map` rewrites `__FILE__`), and companion objects
appended to `EXTRA_OBJS`. The build machinery the fragments rely on (the ecourna flags every unit gets, the ASN.1
runtime, the splat build) is in `src/mk/`.

```
ECOURNA_SRCS += api/security/csha.cpp
UENUX2_SRCS += src/api/util/cwait.cpp
```

## What a fragment holds

`07-ecourna-app-dados.mk` has every part, in the order fragments write them:

1. A header comment naming the classes and the target functions the unit reconstructs.
2. The sources: `DADOS_SRCS := app/dados/tiposbasicos.cpp …` then `ECOURNA_SRCS += $(DADOS_SRCS)`. A variable of its
   own lets the next parts, and the `*_TRY` switches below, name the unit's files.
3. A compile rule of its own, when the generic one gives the wrong flags. For a non-PIC unit it is a `define
   …_NOPIC_RULE` template instantiated per source with `$(foreach u,$(DADOS_SRCS),$(eval $(call …)))`; it filters
   `-fPIC` out of `ECOURNA_CXXFLAGS`, adds `-fno-pic`, and compiles from `/work/src/…` so `-ffile-prefix-map`
   rewrites the `std::source_location` paths. Its prerequisites name the source, the fragment and the headers the
   unit includes, so an edit or a flag change here rebuilds the objects. A unit under `app/dados/` does not need
   one for `-fno-PIC`: `src/mk/ecourna-flags.mk` adds it to the generic rule for every `app_dados_%.o`
   (`tools/round/try-tu.sh --print-flags <source>` ends in `-fno-PIC`). What an own rule still gives (04, 07, 08
   and 09 have one) is the source as a prerequisite, which the generic rule lacks for deep sources:

   The generic ecourna rule in `src/Makefile` has no source prerequisite of its own. It depends on every
   `src/ecourna/*/*/*.cpp` and `*.hpp` and on `src/ecourna/*/*.hpp`, so an edit of a source three levels deep
   rebuilds every generic object, and an edit of a source or header deeper than that (`app/dados/asn/x.cpp`,
   `api/sql/sqlite/x.cpp`) rebuilds nothing: the link silently keeps the old object. A unit with such a source
   gives it a rule of its own (`16-…`) or a rule with prerequisites and no recipe, which leaves the generic
   recipe and flags alone (`15-…`):

   ```make
   $(foreach u,$(HORARIOS_SRCS),$(eval $(ROOT)/$(APP)/ecourna/$(subst /,_,$(u)).o: $(ROOT)/src/ecourna/$(u) \
     $(ROOT)/src/units/15-ecourna-cconversorhorariosurna.mk $(wildcard $(ROOT)/src/ecourna/app/dados/asn/*.hpp …)))
   ```

   Check it with `make -n -C src link -W $PWD/src/ecourna/<source> | grep -c 'em++.* -c '`: 0 means an edit
   would not rebuild. The generic ecourna and uenux2 rules do not depend on any fragment either: after changing a
   flag that only a target-specific variable carries (`06-…`'s `api_compression_%.o: ECOURNA_CXXFLAGS += -fno-PIC`),
   touch the unit's sources, or delete their objects by name when they are deeper than three levels.

   A rule of the fragment's own compiles the TSE source from its `/work/src/…` path, as the generic ecourna and
   uenux2 rules do: the tree's `-ffile-prefix-map` (`UENUX2_PREFIX_MAP`, `ECOURNA_PREFIX_MAP`) rewrites only that
   spelling, so the file names in the `std::source_location` records become the target's. A relative `src/…` path
   is never rewritten, and `make -C src check-srcloc` fails with `UNMAPPED` on such a record.
4. A rule for each companion file (next section) and `override EXTRA_OBJS += $(APP)/<fragment>-roots.o`. `override`
   keeps the companions in the link when `tools/round/try-tu.sh` passes its own `EXTRA_OBJS` on the make command line.

A fragment holds no flag that every unit of its tree needs; those go in `src/mk/ecourna-flags.mk` or in
`src/Makefile` (`docs/reference/app-flags.md` lists them with their evidence). Comments cite the facts that decided a
flag as `docs/reference/facts/app-flags.md §F5`. A flag that one function seems to need is decided for its tree;
a fragment carries it only when the evidence is per TU, as `-fno-PIC`'s is (§F5;
`docs/how-to-decompile.md` step 4, "When only a flag closes the gap").

A flag a fragment does add gives its objects a flag set, and
the benchmark's registry `asmlift-wasm/benchmark/dataset/setups.json` must have a setup with exactly that set, as
`ecourna-nopic` has for `-fno-PIC`. Otherwise, from `asmlift-wasm/`, `pnpm bench setups --check` reports the tree's
setup as drift and `pnpm test:bench` fails; every splat, status and stubs check still passes, so run it before
the commit. A new setup is a new entry in `setups.json` and a re-selection of the dataset
(`asmlift-wasm/benchmark/README.md` "The compile setups").

## Names

`NN-<component>-<what>.mk`:

- `NN`, two digits, is the order `src/Makefile` reads the fragments in, and so the order their objects take in
  `ECOURNA_SRCS`, `UENUX2_SRCS` and `EXTRA_OBJS`, and their flags in `ECOURNA_CXXFLAGS`. That order reaches the link:
  wasm-ld numbers functions in input order. A new fragment takes the next free number; renumbering a landed one
  changes the link. When several people or agents land units at the same time, each takes its number before it
  starts: in a run of several agents the coordinator hands them out (`docs/agents.md`), and otherwise the
  number is said in the pull request or issue that announces the work. Whoever lands second has a link the first
  never measured: it runs link, census, symbols, match and the splat build again on top of the first
  (`docs/how-to-decompile.md` step 10, "Landing onto a moved main").
- `<component>` is the `src/` tree the sources live in: `ecourna` or `uenux2`.
- `<what>` is the source directory the unit's files share, with `/` as `-` (`api-security`, `app-dados-midias`), or
  the class when the unit is one class and what it calls (`cthreadvota`, `crdvvota`, `ctextfieldmultiline`). When
  the shared directory is already another fragment's name (`app/dados/asn/` and `app/dados/processoeleitoral/`
  share `app/dados`, which is 07's), use the class (`cconversorhorariosurna`).

A fragment's companion files carry its name with a suffix: `-roots.cpp`, `-pending.cpp`, `-externs.cpp` (below).
A file compiled through `ECOURNA_SRCS` cannot have `_` in its name: the ecourna rule turns `_` back into `/`.

| fragment | holds |
| --- | --- |
| `00-ecourna-app-dados-identificadoreleitor` | the voter-identifier classes of `app/dados` and the `CError`, base64 and `CStringUtils` code they call, the units the ecourna flags were measured on; `src/app/roots_ecourna.cpp` calls them |
| `01-uenux2-api-util-io-ipc` | `CWait`, `CIniKey`, `CPosixMutex`, `CPosixSemaphore` and the wasm semaphore shim, the units the uenux2 flags were measured on |
| `02-uenux2-cthreadvota` | `vota::CThreadVota` and `api::CTickManager` (target fn 700) |
| `03-ecourna-api-io-util` | `CFile`, `CIoError`, `CBitArray`, `CSynchronizer`, non-PIC |
| `04-ecourna-app-dados-asn` | the core ASN.1 converters and the data classes they build, non-PIC; puts the generated ASN.1 modules in the link |
| `05-ecourna-api-security-cepesc` | the CEPESC envelope classes and `CKeyLoader` |
| `06-ecourna-api-compression` | `ICompressor`, `CZip`, `CLzmaCompress`, non-PIC, and the zlib + minizip build they need |
| `07-ecourna-app-dados` | `app/dados` value types: `tiposbasicos`, `CRegistroIdentificacaoEleitor`, `CSerialMidia`, `CFederacao`, `CConfiguracaoMunicipios`, non-PIC |
| `08-ecourna-app-dados-midias` | the media descriptor classes and their converters, non-PIC |
| `09-ecourna-app-dados-asn-resultadournacadastro` | the voter-register result converters, non-PIC |
| `10-ecourna-app-dados-resultadournacadastro` | the voter-register result data classes, non-PIC |
| `11-ecourna-api-security` | the OpenSSL wrappers (`CSha`, `CSipHashMac`, `CHkdfSeed`, `CKey`, `CAesCipher`, …) |
| `12-ecourna-api-sql-sqlite` | the SQLite DAO (`CSqlConnection`, `CSqlResultSet`, `CSqlStatement`) |
| `13-uenux2-crdvvota` | `comum::CRdvVota::Candidato` (target fn 1931) and the `CVotos*` totals it calls |
| `14-uenux2-ctextfieldmultiline` | the `api::CTextFieldMultiLine` constructor (target fn 5498) |
| `15-ecourna-cconversorhorariosurna` | `CConversorHorariosUrna::DoDeconverte` (target fn 9132) and the `CHorariosUrna` it builds, non-PIC; `DoConverte` (9133) pending |
| `16-ecourna-cconversormunicipiozona` | `CConversorMunicipioZona::DoConverte` (target fn 9131) and the `CMunicipioZona` data class it reads; `DoDeconverte` (9130) pending |
| `17-uenux2-cconversororigemconfiguracao` | `comum::asn::CConversorOrigemConfiguracao::DoConverte` (target fn 11367), the `comum::asn::IConversorASN` base and the `md::EOrigemConfiguracao` enum; `DoDesconverte` (11366) pending |
| `18-uenux2-cthread` | the `api::CThread` destructor (target fn 2721) and the `IThreadImpl` interface its `std::unique_ptr` members delete |
| `19-uenux2-iscreen` | `api::IScreen::GetFontMetrics` (target fn 8938), the inline default of a virtual member in `api/gui/iscreen.h`; no source of its own, the roots file emits and address-takes it |
| `20-uenux2-cconversornumviasimpressasrelatorios` | `comum::asn::CConversorNumViasImpressasRelatorios::DoConverte` and `DoDesconverte` (target fns 11386, 11387) and the `md::estadoaplicacao::CNumViasImpressasRelatorios` data class with its constructor (5629) |
| `21-uenux2-cdirreader` | `api::CDirReader::NextEntry` (target fn 1260), the `CDirReader` class header and the 7010 enumerator of `EUeUtilError`; the constructor (1914) waits for its callee 5470 |
| `22-uenux2-cbmpconversor` | `api::CBmpConversor::VerticalFlip` and `InvertColors` (target fns 3664 and 3663), the `CBmpConversor` class header and the 4900, 4901 and 4903 enumerators of `EUeGuiError` |

## Companion files

**Roots** (`-roots.cpp`). Until a unit's real callers are reconstructed, nothing in the skeleton calls it, and
wasm-ld drops what nothing references. A roots TU stands in for those callers, and it must reach each function the
way the target does, because binaryen's inlining and dead-argument removal depend on the callers it sees
(`docs/reference/facts/tool-traps.md` §X1):

- a function the target reaches through the table (an `invoke_*` thunk, a vtable) has its address taken;
- a function with direct callers in the target gets at least two callers in our link, because binaryen inlines a
  function it sees called once. The reconstructed callers count: the roots TU adds `noinline` ones for the rest.
  5629 has two direct callers in the target (`awk -F'\t' '$2==5629' ledger/callgraph.tsv` prints 5324 and 11387);
  unit 20 reconstructs 11387, so `20-…-roots.cpp` adds one caller standing in for 5324;
- a function the target inlines into its only caller is left unrooted, so it is inlined here too;
- the roots TU declares what it calls through `__asm__` labels with the wasm-level signatures
  (`extern "C" int f(int, int) __asm__("_ZN7ecourna…")`) instead of including the unit's headers, so it
  instantiates no template of its own: a COMDAT copy defined first by the roots object would win the link.

`ledger/callgraph.tsv` and the elem segment give the target's call shapes; each roots file's header says which shape
it reproduces for which function. Roots TUs compile with `APP_CXXFLAGS`, like the other stand-ins in `src/app/`;
their own bodies are not the target's.

**Pending members** (`-pending.cpp`). A class's vtable is emitted in the TU that defines its first non-inline
virtual member, and it needs every virtual member defined, including the ones not reconstructed yet.
`14-uenux2-ctextfieldmultiline-pending.cpp` defines those with a body of `__builtin_trap()`: the class links and
the reconstructed constructor can be measured, and a trapping body (one `unreachable`) cannot pass for the target's
code, so no match can land on it. It compiles with the unit's own flags, as the real members will, through a
rule of its own in the fragment (it is not in `ECOURNA_SRCS`) that is added to `EXTRA_OBJS`. Each member
moves into the class's real source when it is reconstructed, and the file and its rule go with the last one.

A pending member has no stub: `tools/round/stubs.py` counts a function in a slot of a vtable our link holds as code
of ours, so `make -C src stubs` removes its stub when the unit lands, and nothing else lists it as left to do (the
scan still says `no-candidate`). The `-pending.cpp` header names each member with its target index and what it
waits for. To try a candidate with its pending file before the fragment exists, see
`docs/how-to-decompile.md` step 5, "A class with virtual members": try-tu has no rule for a path under
`src/units/`.

**Externs** (`-externs.cpp`). Definitions a unit calls that belong to a unit not landed yet:
`12-ecourna-api-sql-sqlite-externs.cpp` holds `CStringUtils::ToLower` and `Replace`, reconstructed from their target
functions (5158, 1879, 9398) and compiled with the ecourna flags (listed in `ECOURNA_SRCS` as `../units/…`). Unlike a
pending member, an extern is a real reconstruction; it moves into the owning source (`api/util/cstringutils.cpp`)
when that unit lands. None of the three is matched: none is a function of its own in our plain link
(`build/out/wasm-map/vota_web/names.tsv` has no `ToLower` or `Replace`), so the scan lists them as `no-candidate`,
and 5158 and 1879 still have stubs in `src/ecourna/unplaced.cpp`. Matching them needs callers that keep their
bodies, not new code.

## Landing a unit

1. Measure the candidate TUs with `tools/round/try-tu.sh` (the whole link; `--single` for one function) until the
   target functions you expect pair with your symbols.
2. Put the sources under `src/ecourna/` or `src/uenux2/` at the path the target's `std::source_location` records
   name (`docs/how-to-decompile.md` step 3 when none does), and write `src/units/<NN>-<component>-<what>.mk` with
   the next free `NN` (or the one you reserved, "Names"). Its header comment says why the files are where they are
   when no record names them. Check that an edit of each source rebuilds it ("What a fragment holds" item 3).
3. Add a roots file if nothing in the skeleton calls the unit, following the shapes above, and a pending file if
   the class's vtable needs members you have not written.
4. Add the fragment's row to the table under "Names".
5. Link, census, symbols, match and build, in the order and with the reasons `docs/how-to-decompile.md` steps 6 to
   10 give, and commit the fragment, its companions and its table row with them.

## Switches

- `SKIP_UNITS="<fragment>.mk …"` on the make command line leaves those fragments out; `tools/round/try-tu.sh
  --skip-units` uses it to re-try TUs a landed fragment already links.
- `DADOS_ASN_TRY=1`, `DADOS_MIDIAS_TRY=1`, `DADOS_RUC_ASN_TRY=1` in the environment leave the units of fragments 04,
  08 and 09 and their roots out (04 keeps the ASN.1 modules), so try-tu can compile the same TUs as candidates.
