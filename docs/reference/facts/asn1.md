# Facts: the ASN.1 runtime and the generated types

The app encodes and decodes its records with an ASN.1 runtime in namespace `ASN1` and with classes generated
from TSE's ASN.1 specifications (`Modulo*`). `src/asn1/README.md` describes the reconstructed fork and its current
census; this file holds the measurements that decided its flags and its generated types.

## A0. Lineage

`ASN1::` (`AVNEncoder`/`AVNDecoder`, `BEREncoder`/`BERDecoder`, `PEREncoder`/`PERDecoder`, `AbstractData`,
`ConstrainedObject`, `SEQUENCE_OF_Base`, `Constrained_INTEGER`, `ValidChecker`, `InvalidTracer`,
`DataLengthCounter`, `GeneralizedTime`, …) is the **III ASN.1 Tool** runtime (Institute for Information Industry,
Huang-Ming Huang; https://github.com/Kampbell/III-ASN.1, fetched into `third_party/III-ASN.1` (gitignored), last upstream commit
66e5d48). TSE's copy is a fork: `BERDecoderPartial`, `BufferSeeder`, `ISeeder`, `IAbstractVisitor` and `DataFree`
exist in the binary but not upstream, and the fork's error texts are Portuguese. The `Modulo*` classes
(`ModuloBoletimUrna`, `ModuloRegistroDigitalVoto`, …) are asnparser output.

## A1. The runtime TUs were compiled at -Os

The runtime is not a conan -O3 library: its sources sit with the app (namespace `ASN1` has no `__FILE__` strings).
Probe = upstream 66e5d48's 12 `.cxx` files (two portability patches, `-std=c++14`) plus a driver with
generated-style SEQUENCE/CHOICE/ENUMERATED/SEQUENCE_OF types (`tools/libs/asn1/{build.sh,driver.cpp}`), one build
per object -O level, census against the 320 asn1-labelled target functions:

```
tools/libs/asn1/build.sh up-Os -Os        # one build ≈ 6 s
pnpm --filter @asmlift/wasm exec tsx tools/libs/census_subset.ts target/vota_web_wasm.wasm \
    build/out/asn1/up-Os/probe.wasm tools/libs/cls2.json asn1 --depth 0
```

| level | matched (fingerprint / shape / byte-exact) | bytes |
| --- | --- | --- |
| **-Os** | **125** (96 / 29 / 17) | 13,272 / 46,914 (28.3 %) |
| -O2 | 72 (53 / 19 / 15) | 5,259 (11.2 %) |
| -O3 | 71 (52 / 19 / 15) | 5,115 (10.9 %) |
| -Oz | 66 (60 / 6 / 13) | 3,891 (8.3 %) |
| -O1 | 50 (40 / 10 / 13) | 2,045 (4.4 %) |

The -Os matches are large upstream bodies, not accessors: `PERDecoder::do_visit(CHOICE&)` 393 B,
`AVNEncoder::do_visit(BIT_STRING)` 482 B, `OBJECT_IDENTIFIER::encodeCommon` 360 B; `BERDecoder::do_visit(INTEGER&)`
scores 45 rows against the -O3 body, 46 against -Oz, 0 against -Os.

## A2. … and without exceptions

`tools/libs/asn1/build.sh up-Os-noexc -Os -fno-exceptions` → 157 matched (126 / 31 / 18), 19,560 B (41.7 %),
against 125 / 28.3 % for -Os alone. What `-fno-exceptions` buys: every call under an owning local loses its
landing pad (`BERDecoder::do_visit(SEQUENCE_OF_Base&)` 291 B goes from 54 rows to 0: the `asn_ptr<AbstractData>`
around `accept()` emits no `invoke_iii` + `__cxa_find_matching_catch_2`).

Independent evidence from the target (`ledger/callgraph.tsv` × the import names): none of the 320 asn1-labelled
functions calls an `invoke_*` import; one calls `__cxa_throw` (2706). Across the whole target each of the 38
`invoke_*` imports has exactly one caller, while `__cxa_begin_catch` has 652 callers and `__cxa_throw` 220: throw
and catch sites are common in app TUs, landing pads almost absent, so exception flags are per TU.

The fork's `BufferSeeder` throws `std::runtime_error("BufferSeed: no more buffer")`, so the TUs that include
`BufferSeeder.h` compile with `-fexceptions -fignore-exceptions` (a `throw` compiles, no landing pads; the target's
`BufferSeeder` calls `__cxa_throw` directly). `-DNDEBUG` alone is worth 15 more functions: the `assert()`s in
`asn1.h`'s inline accessors kept them out of line. The current flag set is `ASN1_CXXFLAGS` in `src/mk/asn1.mk`.

## A3. The whole schema of the 32 `Modulo*` modules is in the target's data

Every asnparser-generated class has a static `theInfo` record (III-ASN.1's `InfoType`), and TSE's asnparser appends
the type identifier to it. A SEQUENCE `InfoType` is 13 words: `{create, tag, 0, extendable, fieldInfos*,
fieldIds*, numFields, knownExtensions, numOptional, nonOptionalExtensions, tags*, fieldNames*, typeName*}` — e.g.
at 1144192, `EntidadeBoletimUrna` = `[7054, 16, 0, 0, 1625120, 1625168, 11, 0, 2, 0, 1625216, 1625264,
"EntidadeBoletimUrna"]`. A SEQUENCE_OF `InfoType` is 8 words (`…, elementInfo*, typeName*`).

Method: dump the memory image (`tools/libs/memimg.py`), find for each RTTI SEQUENCE class the pointer to its short
name (99 of 99), then walk `fieldInfos`, `selectionInfos` and `elementInfo` recursively. Result: **297 records**
(118 SEQUENCE, 58 ENUMERATED, 45 SEQUENCE_OF, 41 strings, 22 INTEGER, 10 CHOICE, 2 OCTET_STRING, 1 BOOLEAN) with
field names, tags, optional ids, enumerator names, constraints, element types and CHOICE selections. A record's
kind is its `create` table index: 7054 SEQUENCE, 7053 ENUMERATED, 7055 CHOICE, 2445 SEQUENCE_OF_Base, 2284 INTEGER
(shared by every `Constrained_INTEGER<…>`), 7052 GeneralString/NumericString, 4444 OCTET_STRING, 7115 BOOLEAN. The
41 string records live in BSS and are filled at startup; their values are replayed from the static initialiser
(func 14478: straight-line `i32/i64.const` stores, where one `i64.const` packs two words).

Consequences:

- A generated class has no code of its own. All 99 SEQUENCE classes share the 7 vtable functions
  `[181 246 8957 8958 8961 8960 11627]`; ENUMERATED, CHOICE and SEQUENCE_OF likewise share theirs. The generated
  types are data: the function census does not move when they land.
- `AbstractData`'s 7th virtual (slot 6) is a `getTypeName()`-like hook: `AbstractData` returns "AbstractData"
  (5075), SEQUENCE returns `info()->typeName` (11627), SEQUENCE_OF_Base `typeName ? typeName : AbstractData::…`
  (9008).
- `SEQUENCE_OF<T>`'s D0 destructor comes in two copies: 859 (no landing pad) for the instantiations uenux2 uses,
  1240 (`invoke_vi` + catch) for the ecourna ones (`-fexceptions`). The first definer on the link line decides.
- Type names: 99 SEQUENCE names come from `typeName`; 19 more SEQUENCE types are never constructed by the app (no
  RTTI, name only in the record); ENUMERATED/CHOICE/string class names come from vtable co-reference in app code
  (a ctor stores vptr and info as one `i64.const`: split `i64` constants when scanning), from
  `ledger/source-locations.tsv`, or are synthesised from the field name (marked "SYNTHESIZED" in the headers).

The generated headers are `src/asn1/modules/<Modulo>.h` + `.cxx`.

## A4. Traps when probing the runtime

1. Upstream III-ASN.1 does not compile in the pinned image at any `-std` without patches: `Platform.h` has no
   `__wasm__` branch (`#error "Unknown Hardware Architecture."`); `std::auto_ptr`, `unary_function`, `bind2nd` and
   `not1` are gone from libc++ 23 at C++17+ (`-std=c++14` keeps them); a C cast of `string::const_iterator` to
   `const char*` fails on libc++'s `__wrap_iter`; and `std::swap(auto_ptr&, auto_ptr&)` is avoided upstream only
   under `#ifdef __linux__`, which emscripten does not define (it defines `__EMSCRIPTEN__`/`__unix__`).
2. `tools/libs/vtable_compare.ts` pairs by symbol-name prefix, so for a 7-slot type class every slot shows the same
   best function: those rows mean "no probe function of this class". Read only the visitor classes' rows; a `-`
   means the class is absent from the probe.
3. The census at depth 0 still symbolises `call fn:<hash>` and `data:<segment>+off`: two bodies that differ only in
   a callee hash or a string address stay unpaired. The per-slot score is the honest measure.
4. A symbol-map lookup (`grep -F 'ASN1::BERDecoder::do_visit(ASN1::UTCTime&)' probe.js.symbols | head -1`) can
   return a 2-row stub (an outlined or merged body): check the row count in the score header first.
5. `tools/libs/cls2.json`'s 320 'asn1' functions include ~15 app functions (`api::AudioCollector`,
   `simulador::CWasm*`, `itanium_demangle` slots); filter by `ledger/func-attribution.tsv`'s component.
6. A runtime probe build takes 6 s; a full skeleton link takes minutes. Test flags on the probe.
