# Facts: compile flags of the application TUs, per component

The flags each part of TSE's application code (uenux2, ecourna) was compiled with, and the measurement behind each
one. The current flag sets live in `src/Makefile` (`ECOURNA_CXXFLAGS`, `UENUX2_CXXFLAGS`, `APP_CXXFLAGS`) and
`src/mk/ecourna-flags.mk`; [../app-flags.md](../app-flags.md) gives them with their evidence. The ASN.1 runtime's
flags are in [asn1.md §A1–A2](asn1.md), the libraries' in [libraries.md](libraries.md), the link line in
[toolchain.md §T3](toolchain.md).

How a flag is measured: compile the reconstructed TU under each flag set in `urna-emsdk:6.0.0`, link, and score
each function against the target with `pnpm wasm score <target> <fn> <candidate.wasm> <index> --depth 0` (from
`asmlift-wasm/`; the index comes from the `--emit-symbol-map` twin's `.js.symbols`). A score is "rows differing /
rows"; **operand-only** means every remaining row differs only in an address, a table slot or the `__THREW__`
address, which a separate link can never share, so the body is structurally identical. Prefer a whole-skeleton
link over a lone link (`tools/round/try-tu.sh --single`): a lone link changes inlining (see F9).

| component | flags |
| --- | --- |
| ecourna (`src/ecourna`) | `-O3 -fPIC -fexceptions -std=c++20 -DNDEBUG -fno-strict-aliasing -fno-delete-null-pointer-checks -fno-strict-overflow`; `-fno-PIC` for `api/io`, `api/compression`, `app/dados` |
| uenux2 (`src/uenux2`) | `-O2 -fPIC -std=c++20 -DNDEBUG`, without `-fexceptions` |
| ASN.1 runtime (`src/asn1`) | `-Os -fno-exceptions -std=c++14 -DNDEBUG` ([asn1.md](asn1.md)) |

## F1. ecourna: `-O3 -fPIC -fexceptions -std=c++20 -DNDEBUG`

Compared on three reconstructed units (`api/security/base64.cpp`, `api/util/cstringutils.cpp`, the `app/dados`
identifier units) with `tools/round/appflags/compare.sh`:

- **-O3.** 14 of 23 compared functions become structurally identical at -O3 (the `CError` ctor, func 1143: 0 rows
  at -O3, ≥ 173 at -O1/-O2/-Os/-Oz; the `To*`/`HexToInt` thunks; the `CNumero*` `Formatado` bodies).
- **-fexceptions.** 134 of 194 ecourna functions that load a `source_location` record call an `invoke_*` import
  (landing pads).
- **-std=c++20, not 23.** C++23's rvalue `substr()` changes func 3703 (116 rows vs 53); `gnu++20` equals `c++20`
  byte for byte.
- **-fPIC** holds for `api/security` (F5).
- **Paths.** `-ffile-prefix-map=/work/src/ecourna=/home/rubio/.conan2/p/b/libecea1da310e5107/b/src/ecourna`
  produces an object byte-identical to compiling at the real path (`cmp` of three objects). The include root is
  `-Isrc` (`#include "ecourna/api/…"` = the conan package's `b/src`).

## F2. ecourna: `-fno-strict-aliasing`

Symptom: in every ecourna function that copies a `std::vector` (66 call sites of func 306), the target keeps
libc++'s vector-copy exception guard in the frame (`i32.store8 [sp+44]=0; i32.store [sp+40]=&vec`) and its landing
pads call func 306 = `__exception_guard_exceptions<__destroy_vector>::~__exception_guard_exceptions()` out of line.
At plain -O3 clang inlines the guard's destructor into the landing pad, and every such body keeps ~22 inserted and
22 deleted rows.

Scored on `CInfoSalt::CInfoSalt(const vector&, const vector&)` (target 5169), one lone link per variant:

| flags | score |
| --- | --- |
| -O3 | 164 / 534 (22 inserted, 22 deleted) |
| -O2 / -Os / -Oz | 188 / 268 / 350 |
| -O3 -flto, -flto=thin | 164 / 166 |
| -O3 -fno-inline-functions-called-once, -fno-unroll-loops -fno-vectorize | 164 |
| **-O3 -fno-strict-aliasing** | **75 / 512, all operand-only** |

At the object level (`llvm-nm | grep -c 'exception_guard\|destroy_vector'`) every other variant inlines the guard;
only `-fno-strict-aliasing` reproduces the target's shape at -O3 (TBAA changes the inliner's view of the guard's
`__completed_` store/load pair).

## F3. ecourna: `-fno-delete-null-pointer-checks`

Symptom: where the target reads a `std::string` member of `this` through the SSO test (`c_str()`, `size()`,
`empty()`), it spells an `if/else` around the long-string load. Candidates with the other ecourna flags spell a
`select`: LLVM speculated the load because `this` carries `dereferenceable(N)`, and binaryen never turns a select
back into an if. `-fno-delete-null-pointer-checks` drops the nonnull/dereferenceable attributes from `this`, so the
load is not speculated.

| fn | function | without | with |
| --- | --- | --- | --- |
| 9504 | `CKey::CKey` (3 SSO tests) | 118 (25 ins, 8 del) | 105 (19 ins, 0 del) |
| 5177 | `CSha::Reset` | 72 (11, 16) | 60 (6, 7) |
| 9497 | `CSiphashMac::DoVerify` | 128 (29, 32) | 69 (6, 6) |
| 9498 | `CSiphashMac::DoMac` | 294 (68, 48) | 288 (68, 42) |
| 3518 | `CSha::Finish` | 97 | 97 |

Tried on 9504 without effect: -O2 (worse), `-fwrapv`, `-fno-semantic-interposition -fno-plt`,
`-fno-assume-sane-operator-new`, `-fno-slp-vectorize`. With F2 this looks like a deliberate "safe C++" flag pair
in TSE's toolchain file.

## F4. ecourna: `-fno-strict-overflow`

Mechanism (on `app/dados/midias/caplicativo.cpp`, func 9042, `llvm-objdump -d` of the object): without the flag
LLVM folds `this+64` into the load (`i32.load8_u offset=64`). With `-fno-strict-overflow` or `-fwrapv-pointer`
(not `-fwrapv` alone) the GEPs lose `inbounds`/`nuw`, the add stays, and binaryen later folds every such constant
into the memarg **except 64**, which it first rewrites as `x - (-64)` (a one-byte SLEB). The target's
`i32.const -64; i32.sub; load offset=0` rows are this. The same flag changes load scheduling (field loads, the
order of a vector's begin and end loads).

Whole-link evidence: 9164, 9156, 9226 ("begin before end"), 9219 (field order), 9042, 9174, 9200 and 9185 all drop
to operand-only; the byte-exact 5093 and 5112 stay byte-exact; no other function gets worse. These measurements do
not separate `-fno-strict-overflow` (= `-fwrapv` + `-fwrapv-pointer` in clang 23) from `-fwrapv-pointer` alone: no
measured function depends on signed-overflow wrapping.

## F5. `-fPIC` is per TU (per CMake target), not per component

Conan builds every static library with `fPIC=True` by default and `POSITION_INDEPENDENT_CODE` is a CMake target
property, so each ecourna sub-library can differ. The telltale is data addressing: **non-PIC** code folds a
private table's address into the load's memarg (`i32.load offset=ADDR`) and packs a `std::format` string's
{ptr, len} into one folded `i64` constant; **PIC** code goes through `global.get __memory_base + off`
(`i32.const; i32.add; i32.load`) and reorders the surrounding stores.

| TUs | verdict | evidence |
| --- | --- | --- |
| `api/security` (`base64.cpp`) | PIC | func 3703's `encoding_table`/`mod_table` reads are `i32.const 1112848; i32.add; i32.load8_u`; `--single` 271 rows -fPIC vs 281 without |
| `api/io` | non-PIC | `CIoError::CIoError` 3520: 78 rows (12 ins, 9 del) with -fPIC, 71 operand-only without; `CFile::RawWrite` 1886: 121 (9, 9) vs 106 operand-only; `CFile::RawRead` 598: 140 (18, 18) vs 146 operand-only. The ctor's `Description(EFileOperation)` table is `i32.load align=2 offset=1112652` |
| `api/compression` | non-PIC | `CZip::ConvertToCompressLevel` 9539 reads its switch table `{0,1,-1,9}` as `i32.load offset=1110872`: 42/118 (2 ins) with -fPIC, 21/116 operand-only without. `CZip::AtualizaProgresso` 9537: 50/361 vs 32/356 |
| `app/dados` | non-PIC | every `std::format` string's {ptr, len} is one folded `i64` (2666/2668: 214748740548 = 50<<32 \| 375748). `--single` -fPIC vs -fno-pic: 9047 70 vs 58, 2669 36 vs 20, 2666 114 vs 85 |
| `app/dados/resultadournacadastro` | non-PIC | 9024: 39 rows with -fPIC, 20 without (both operand-only) |
| `app/dados/asn` converters | non-PIC | 9192 / 9191: 23 / 20 rows operand-only without -fPIC, 34 / 29 with 3 / 2 structural rows with it |

The fragments compile those TUs with `$(filter-out -fPIC,$(ECOURNA_CXXFLAGS)) -fno-pic` or a target-specific
`ECOURNA_CXXFLAGS += -fno-PIC` (the last `-f` wins); `src/mk/ecourna-flags.mk` covers every
`app/dados` object.

## F6. uenux2: `-O2 -fPIC -std=c++20 -DNDEBUG`, without `-fexceptions`

Compared on three reconstructed units (`api/util/cwait.cpp`, `api/io/cinikey.cpp`,
`api/ipc/posix/cposixmutex.cpp` + `cposixsemaphore.cpp`; 13 target functions), each flag set in a private
whole-skeleton link (`make -s -C src link APP=… OUT=… OUT_NAMED=… UENUX2_CXXFLAGS="…"`):

| fn | function | -Oz | -Os | -O2 | -O3 | -O2 -fPIC |
| --- | --- | --- | --- | --- | --- | --- |
| 5447 | `CWait::CWait` | 194 | 24 | 24 | 24 | 14 operand-only |
| 5483 | `CIniKey::operator=` | 109 | 90 | 5 op | 5 op | 5 op |
| 10254 | `CPosixMutex` ctor | 112 | 26 | 26 | 26 | 11 operand-only |
| 10250 | `CPosixSemaphore` | 117 | 27 | 27 | 27 | 12 operand-only |

- **Not -Oz.** At -Oz the `std::format` throw path stays as calls with a 496-byte frame; the target has the inlined
  `__format_arg_store` constants and a 480-byte frame. The copy of `CError::~CError` (func 4636) that wins the link
  looks like -Oz code, but it comes from the **first** object defining it (an executable TU, possibly -Oz), not from
  the uenux2 library trees: it says nothing about uenux2's level.
- **Not -Os.** -Os loses `CIniKey::operator=` (string `operator==` stays a call).
- **-O2 = -O3** on every surviving function (the objects differ; a loop-bearing unit must separate them). -O2 is
  kept.
- **-fPIC** removes the last structural rows of all three ctors.
- **No `-fexceptions`.** 0 of 1,645 uenux2 functions that load a `source_location` record call `invoke_*`; uenux2
  throw sites call `__cxa_throw` directly. Without `-fexceptions` emcc defaults to `-fignore-exceptions`
  (throw → direct `__cxa_throw`, no landing pads). That is **not** `-fno-exceptions`, which fails to compile every
  uenux2 TU with a `throw`; write the flag set as "without `-fexceptions`".
- `-fno-strict-aliasing` changes some uenux2 objects but no surviving function: undecided, not added.
- `-std=c++17` does not compile (`source_location`, class NTTPs).
- Paths: `-ffile-prefix-map=/work/src/uenux2=/home/rubio/tse/uenux2`.

## F7. `-DNDEBUG` everywhere

The target imports no `__assert_fail`; see [toolchain.md §T6](toolchain.md). The application units have no
`assert`, so the flag is pinned by the import, not by their bodies.

## F8. Source idioms the target forced in ecourna

Each was compiled, not guessed:

1. **A message built before the throw is a local.** When `__cxa_allocate_exception` comes after the string
   building in the target (`CFile::Sync`), the source is `std::string mensagem = …; throw CIoError(…, mensagem,
   errno);`. When the allocation comes first, the expression is inside the `throw`.
2. **An out-of-line ctor keeps its returned `this`.** Every caller of `CIoError`'s ctor keeps the returned `this`
   (`local.set`); a same-TU or inline definition makes LLVM drop it. Hence `api/io/cioerror.cpp` is its own TU (its
   data sits between `cfile.cpp`'s records and `base64.cpp`'s).
3. **An inline ctor can win the link from a uenux2 TU.** `CFile`'s ctor (func 517) is a branchy
   `if (!nome.empty())` with no load speculation: the COMDAT copy of a uenux2 user won the link, not the ecourna
   one. It is inline in `cfile.hpp`, and the skeleton gets that copy from `src/units/03-ecourna-api-io-util-roots.cpp`.
4. **A validator called through a slot is external.** `CSerialMidia`'s validator (func 9258) is a static member; a
   file-local one is inlined. Its body is `std::regex formato("([A-Fa-f0-9]{8})"); return
   std::regex_match(serial, formato);`, the overload without a caller `smatch` (the target's frame zeroes the
   56-byte `match_results` at +40 after the regex ctor).
5. **Throwing getters are out of line, plain accessors inline.** In `app/dados/resultadournacadastro` every
   throwing getter is in the `.cpp` and reached through `invoke`; every non-throwing accessor is inline in the
   header (`GetX()` for plain members, `HasX()` = `m_x.has_value()` beside each throwing `GetX()`).
6. **Pair messages by function, not by text.** The target's `CEstadoComparecimento` getters have their photo and
   audio messages swapped (9021 `GetApresentacaoFoto` says "Habilitação de áudio…", 9020
   `GetSituacaoHabilitacaoAudio` says "Dados de apresentação de foto…").
7. `errno` reads are `i32.load[1931660]` (the inlined `__errno_location()`), right before the ctor call.
8. `CBaseType<…>`'s ctor formats `"O tipo '{}' deve ter valores no intervalo [{},{}]."` with an
   `extern const std::string` name defined by one TU; the known names and their data addresses are in
   `src/ecourna/api/pattern/cbasetype.hpp`.

The recovered class layouts are in the sources themselves (`src/ecourna/app/dados/…`).

## F9. Traps when measuring flags

1. **A lone link inlines what the target keeps out of line.** binaryen inlines a function of any size that has one
   call site and is not exported or in the table. A probe whose TUs throw one exception of a type shows the
   `CBaseError<E,L>` ctor inlined into the throw site (44 extra rows in 5447); in the target every such thunk has
   many callers. Give each function the target's caller count: ≥ 2 extra throw sites per error type
   (`src/app/roots_uenux2.cpp`), and a hand-written roots file per unit (`src/units/<unit>-roots.cpp`).
2. **Generated roots that address-take every symbol stop one-caller inlining.** `tools/round/gen_try_roots.py`
   roots every defined symbol, so `SetFileMode→Open`, `CreateInst→Sync`, `Description→ctor` stay out of line
   (Open 251 rows, Sync 736); a roots file that keeps only what the target keeps lets them inline. App-provided
   libc shims (`sem_getvalue`) need no roots at all: let binaryen inline them.
3. **`--single` cannot judge a function whose helper has two callers in the TU**: the lone link exports only the
   scored function, the helper drops to one caller and is inlined (+86 rows on 5088/5090 with
   `~optional<CRegistroIdentificacaoEleitor>`, func 5089).
4. **The census tier hides local renumbering.** `CEstadoHabilitacaoPorCodigo(ESituacaoReconhecimentoMesario, const
   CRegistroIdentificacaoEleitor&)` (5088) scores 41/174 and the census says "none", yet every differing row is a
   local index (the target shares SP's local with the caught exception, ours with the rethrow flag). Read the
   score, not the tier, before calling a function unmatched.
5. **A direct call can sit inside a cleanup scope.** func 9066 does `i32.store 1936852 0; call 2657; drop;
   i32.load 1936852 …` for a ctor that cannot throw: it is still an invoke in the source (a call inside a cleanup
   scope), emitted as a direct call with the `__THREW__` clear and test around it.
6. **The flag rule does not rebuild stale objects.** An object rule depends on the Makefile's mtime, not on the
   flag value: never write the shared `build/out/app/…` objects with sweep flags, and touch the sources after a
   flag change in a `src/units/*.mk` fragment. Measure a flag set in a private link
   (`make -C src link APP=build/out/<name>/app OUT=… OUT_NAMED=…`), which rebuilds all app objects.
7. **A lone-link residue of whole-program DAE.** Extra `drop`s after `~shared_ptr` in a landing pad, or a
   `memcpy` return value kept or dropped, come from callers elsewhere in the program; they close only in the
   whole link.
