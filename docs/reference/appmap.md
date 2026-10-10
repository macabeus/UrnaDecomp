# vota_web_wasm — application map

Target: `target/vota_web_wasm.wasm` (7,713,699 B; 14,343 defined functions, 136 imports, 4,224 data segments = 1,732,475 B at 1024..1811674).
Everything below is derived from the binary alone by the scripts in `tools/extract/` (run each with `python3 -I`); every number names the file it was read from.

| artefact | produced by | what it holds |
| --- | --- | --- |
| `ledger/source-files.txt` | `tools/extract/extract_strings.py` + grep | 759 distinct source paths seen as `__FILE__` strings |
| `ledger/strings.tsv` | `tools/extract/extract_strings.py` | 16375 NUL-terminated data strings: addr, len, encoding (ascii/latin1/utf8), text |
| `ledger/ui-strings.tsv` | `tools/extract/ui_strings.py` | 2,691 user-facing pt-BR strings (2,010 Latin-1) with the components that load them |
| `ledger/functions-pretty.txt` | strings filter | 1,394 `__PRETTY_FUNCTION__` texts (dead data, see §6) |
| `ledger/classes.json` | `tools/extract/rtti.py` | 2,162 typeinfo objects: name, kind (class/si_class/vmi_class/pointer/…), bases with offset/virtual/public |
| `ledger/vtables.json` | `tools/extract/rtti.py` | 1851 vtables: address, class, offset_to_top, table indices and resolved function indices |
| `ledger/func-strings.tsv` | `tools/extract/func_refs.py` | per function: every data address it loads (i32.const or folded load/store offset) that is a string start, a vptr or a typeinfo |
| `ledger/callgraph.tsv` | `tools/extract/func_refs.py` | 113,299 direct `call` edges |
| `ledger/func-attribution.tsv` | `tools/extract/attribute.py` | every function → component, evidence (file/vtable/vptr/typeinfo/str/callers), detail |
| `target/fs/`, `target/fs-manifest.tsv` | `tools/setup/unpack_fs.mjs` | the 70 preloaded files (path, size, sha256) |
| `tools/extract/wasmmod.py` | — | the wasm reader the scripts share (sections, imports, elem table, data image) |

## 1. Components and their size

Function counts from `python3 -I tools/extract/attribute.py ledger/func-strings.tsv ledger/vtables.json ledger/classes.json target/vota_web_wasm.wasm ledger/func-attribution.tsv ledger/functions-by-file.tsv` (direct evidence = __FILE__ string, vtable slot, vptr store, typeinfo ref or owner-specific string; "propagated" = inherits the single component of all its attributed callers, iterated to a fixpoint):

| component | what it is | source files seen | functions (direct) | functions (after propagation) | RTTI classes |
| --- | --- | ---: | ---: | ---: | ---: |
| **vota** | uenux2 `src/app/vota`: the voting application (states, screens, operator flows) | 75 | 529 | 780 | 219 |
| **comum** | uenux2 `src/app/comum`: shared urna app code (model `comum::md`, ASN converters `comum::asn`, reports, DAO) | 184 | 377 | 599 | 239 |
| **api** | uenux2 `src/api/*`: GUI toolkit (form builders, fields), io, ipc, hwil, util, hash, uelog, persistencia, audio | 80 | 449 | 516 | 185 |
| **ecourna** | conan package `libecea1da310e5107` (`src/ecourna/{api,app}`): security (AES, SHA, siphash, HKDF, base64, CEPESC, ASN.1 key loader), sql/sqlite wrapper, compression (zip, lzma), io, util, `app/dados` model + ASN converters | 64 | 563 | 799 | 147 |
| **simulador** | the web-simulator shim (`CWasm*` devices implementing `api::I*`), `wasm/vota_web/vota_web_wasm.cpp` | 1 | 126 | 144 | 21 |
| **vota-web** | anonymous-namespace web engine (`CVotaWebEngine`, `CPoliticaExecucaoEleitorWeb`, `CSincronismoVotoEleitorWeb`, `CWasmSavd`) | — | 10 | 11 | 3 |
| asn1-modules | asnparser-generated ASN.1 types `Modulo*::` | — | 48 | 59 | ~110 |
| ASN1 | asnparser C++ runtime (`ASN1::`) | — | 281 | 362 | 64 |
| RHVoice | RHVoice text-to-speech engine | — | 470 | 666 | 295 |
| 7zip | 7-Zip C++ (NArchive::N7z, NCompress::NLzma/NLzma2, COM-style streams) | — | 601 | 878 | 102 |
| boost | signals2, regex (re_detail_600), lexical_cast, date_time gregorian, exception | — | 114 | 125 | 31 |
| openssl | OpenSSL 3 libcrypto incl. the complete default provider set | 347 | 1,504 | 2,381 | — |
| sqlite | SQLite 3.50.4 (source id 2025-07-30 19:33:53 4d8adfb30e03…) | (os_unix.c…) | 50 | 500 | — |
| libc++ / libc++abi | emscripten system libs (`/emsdk/emscripten/system/lib/libcxxabi/src/*`) | 5 | 1,314 + 174 | 1,564 + 234 | 576 (std) + 90 (demangler) + 9 |
| utf8cpp, rapidxml | header-only helpers | — | 10, 6 | 10, 6 | — |
| **unattributed** | musl libc, compiler-rt, small templates, library internals without strings | | 7,716 | 4,709 | |

RTTI census (`tools/extract/rtti.py`): 2149 class-kind typeinfos; 1,756 classes have a vtable; vtables name 3,908 distinct functions.

## 2. Source tree

The TSE build machine had the uenux2 tree at `/home/rubio/tse/uenux2` and the ecourna conan package at `/home/rubio/.conan2/p/b/libecea1da310e5107/b`; OpenSSL was compiled in-tree so its `ERR` file strings are relative (`crypto/…`, `providers/…`). Only files that emit a `__FILE__` string are visible (asserts/uelog), so headers and silent TUs are missing. The full tree with per-file function counts is in Appendix A; the shape:

- `uenux2/src/app/vota` (75 files; `grep '^/home/rubio/tse/uenux2/src/app/vota/' ledger/source-files.txt | awk -F/ …`): `eleitor/` 53 (voter flow: votaproporcional, votamajoritario, confirma*, justificativa…), `operador/` 17 (mesário flows: confirmaidentidade/cregistradigitaloperador, registramesarios, auxiliares/cmenuvisualizarcandidatos, votacao/cverificahorariozeresima…), `comum/` 2, `log/` 1, `monitor/` 1, 1 at the root; plus `uenux2/wasm/vota_web/vota_web_wasm.cpp`.
- `uenux2/src/app/comum` (184 files): `dados/` 86 (`md/` 47 — the domain model: candidaturas, cargos, eleitores, rdv/cvoto, fotos, abrangência; `asn/` 23 — CConversor* to/from the ASN.1 modules; `dao/` 1; 15 at its root), `gravadores/` 24, `comparecimentomesario/` 22, `relatorios/` 14 (BU, zerésima, RDV, QR codes), `md/` 9, `asn/` 7, `appinfo/` 4, `justificativa/` 2, `log/` 2, `util/` 2, `validamidia/` 2, `informacao/`, `nomearquivo/`, `reconhecimentobiometrico/` 1 each, 7 at the root.
- `uenux2/src/api` (80 files): `gui/` (39: capplication, cformbuilder, cinteractiveformbuilder, cpaperformbuilder, text/image/movie/qrcode/progress fields), `util/` (14), `io/` (7, asn file readers), `ipc/` (5), `hwil/` (4), `pattern/`, `hash/`, `uelog/`, `persistencia/`, `audio/alsa/cwavfile.cpp`.
- `ecourna/api` (26 files): `security/` (14: caescipher, cblockcipher.hpp, isymmetriccipher, ckey, csha, csiphashmac, imacalgorithm, chkdfseed, base64, asn1/ckeyloader, cepesc/{ccipheredin,ccipheredout,cinfosalt,cplaintext}), `compression/` (czip, clzmacompress, icompressor), `sql/sqlite/`, `io/cfile`, `util/`, `asn/iconversorasn.hpp`, `pattern/cbasetype.hpp`.
- `ecourna/app/dados` (35 files): the shared data model (CParametrosUrna, CConfiguracaoMunicipio, CFederacao, CInformacaoMidia, CResultadoUrnaCadastro, identificadores…) and its `asn/` converters.

## 3. Main subsystems

### 3.1 Voting-flow state machine
`api::CState` is the root (199 descendants, the largest hierarchy); `comum::CAppState` (9 virtual slots) has **132 direct subclasses**, each one screen/step of the flow. Examples (vtables.json → function indices): `vota::CPedeDigital` (vtable @1591888, slots [2739, 10469, 10468, 7480, 1661, 10467, 425, 10466, 10465]), `vota::CAguardaInicio`, `CDefineRotaPreVotacao`, `CDigitalReconhecida`/`CDigitalNaoReconhecida*`, `CEleitorEncontrado`, `CConfirmaVotoNominal`, `CCompletaProporcional`, `CConfereVotoEmCargo<State, ETelaVotacao N>` (10 template instantiations: candidato inapto/inexistente, voto legenda/nominal, majoritário branco/nulo/repetido/válido, proporcional branco/nulo), `CConfirmaEncerramento`, `CCopiaResultadoParaMR`, mesário registration states in `comum::` (`CPedeTituloMesario`, `CPedeDigitalMesario`, `CRegistrarMesarios`, `CMesarioRegistrado`…). `comum::CAppStateContext` / `api::CStateContext<comum::…>` drive them; `vota::CControladorRegistraMesariosVota` has the largest app vtable (37 slots). Operator/voter threads: `vota::CThreadEleitor`, `vota::impl::CInformacaoThreadOperador` (31 slots), `vota::CThreadMonitor`, `api::CThread`.
The string `vota::CVotacaoStateAudio::PlayKey(const char) const` and the `CConfirmaAudio`/`CDesabilitaAudioEleitor` states show the audio-assisted voting path; `IQRCodeBUDS`, `CGeradorBUQRCodeVota` the BU QR output.

### 3.2 GUI: form builders and fields
`api::IScreen` (39 virtual slots) is implemented only by `simulador::CWasmScreen` (drawing goes to the JS canvas through the EM_JS imports js_fill/js_line/js_text/js_image/js_round_rect/js_path/js_gray_gradient/js_circle). `api::CFormBuilder`, `api::CInteractiveFormBuilder`, `api::CPaperFormBuilder` (printer/paper variant), `api::IFormFieldBase<IScreen|IScreenMT|IPaper>` (24+11+5 descendants: CTextField*, CImageField, CMovieField, CQRCodeImage, CProgressBar, CStepsProgressBar, CInputField/CInputMenuField, CMaskedTextField, battery icon data sources), `api::IText` (19), `api::CDataText<…>/CDataTextFmt<…>/CDataImage<…>` templates binding model objects to fields. `CTelasVota::CriaTela*` lambdas (pretty strings) build the voter screens.

### 3.3 Persistence: SQLite DAO
`ecourna::api::sql::sqlite::{CSqlConnection(10 slots), CSqlStatement, CSqlResultSet}` over SQLite 3.50.4; `api::persistencia::IDAO` (9 descendants) and `comum` DAOs (`CEleitores`, RDV `comum::CRdvVota`, `CGeradorBUBase<CRdvVota, CEleitores>`). SQL text lives in `ledger/strings.tsv` (CREATE/INSERT/SELECT strings).

### 3.4 ASN.1
Runtime `ASN1::` (AVN/BER/PER encoders and decoders, `BERDecoderPartial` 28 slots, Constrained_INTEGER/SEQUENCE_OF templates, Visitor/ConstVisitor) with generated modules `ModuloTiposEleitorais`, `ModuloResultadoUrnaCadastro`, `ModuloBoletimUrna`, `ModuloRegistroDigitalVoto`, `ModuloEleitores`, `ModuloEstadoGeralUrna`/`Gap`, `ModuloParametrizacaoUrna`, `ModuloFederacoes`, `ModuloInformacaoMidia`, `ModuloConfiguracaoMunicipios`, `ModuloTiposEcoUrna`, `ModuloTiposResultadosEcoUrna`. Converters: `comum::asn::CConversor*` (uenux2 side) and `ecourna::app::dados::asn::CConversor*` via `ecourna::api::asn::IConversorASN<Asn, Model>` (~45 instantiations). `api::io::asn` reads packages (`cfileasn.h`, `cpartialfileasn.h`).

### 3.5 Security
`ecourna::api::security`: `CBlockCipher<CAesCipher, CTrng>` (ISymmetricCipher), `CSymmetricCipherFactory`, `CSiphashMac`, `CPrng`/`CTrng` (IRng), `cepesc/` (CCipheredIn/Out — the CEPESC cipher streams), `asn1/ckeyloader`, SHA/HKDF/base64; `comum::CAssinador`, `vota::CAssinadorVota` (signing), `api::hash` (CHashArquivo, CHashDiretorio, CMontadorHash). All on OpenSSL 3 (EVP, provider API; RSA/EC/DSA/DH, PKCS7/PKCS12, every default-provider algorithm is linked).

### 3.6 Devices (hwil) and the web shim
`api::I{Input,Sound,Timer,TimerScheduler,SystemDateTime,Paper,Impressora,TextToSpeech,EsperaAudio}` each have exactly one real implementation in `simulador::CWasm*` (CWasmInputKbd/MT, CWasmWebSound/NullSound, CWasmTimer(Scheduler), CWasmSystemDateTime, CWasmNullPaper/NullPrinter, CWasmNullTextToSpeech, CWasmBeep, CWasmLogd, CWasmResource, CWasmThread, CWasmImageSurfaceOps, CFingerPrepareSimulador). `api::CRHVoiceTextToSpeech` wraps RHVoice (linked, no voice data in the payload). `api::teste::CPowerMock`/`CUrnaMock` (test doubles) are linked too.

## 4. Preloaded filesystem (`target/fs/`, `target/fs-manifest.tsv`)
70 files, 14,855,585 B: 12 animated GIF sign-language avatars (`/resource/gifs/*Mulher4.gif`, 249×240, 14.36 MB = 97%), `/font-variable.ttf` (65,072 B, the only font), 54 images under `/uenux/app/img/` (JPEG/SVG/XBM/XCF battery, biometrics, marcaJE, armas, confirmado/cancelado), 3 empty `/etc/*.properties|vst`. No database, no ASN.1 package, no voice data: TSE data is synthesised in the simulator (`target/manifest.json` scenarios). Fonts/images are decoded by the browser (js_image/js_text), not by C++.

## 5. Encodings and strings
Of 16375 strings 14,353 are ASCII, 2,010 Latin-1 (`Conteúdo inválido para {}:{}`), **1 UTF-8** (`Turno inválido.` @367628) → sources are ISO-8859-1 except the TU emitting that string. Format strings are `{}`-style (fmt/std::format) in the app, `%` in OpenSSL/SQLite.

## 6. What remains unattributed, and known blind spots
- 4,709 functions (33%) have no owner: musl, compiler-rt, printf/locale internals, libc++ templates without RTTI, OpenSSL/SQLite helpers. Function **index order is sorted by use count** (binaryen reorder-functions at -Oz; ties by descending decimal string of the link ordinal, names stripped) — see [facts/link-order.md](facts/link-order.md) §L1; within a use-count band neighbours are link-order neighbours up to a digit scramble, which can be used to attribute the rest.
- The 1,382 `__PRETTY_FUNCTION__` strings are referenced by nothing (no i32.const, no load offset, no data word): their call sites were optimised away but the bytes remain — the rebuild needs the same macro usage to re-emit them.
- vtables name the class of 3,908 functions but not their slot's meaning; slot→method names need the headers (comum::CAppState has 9 slots: dtor pair, OnEnter/OnExit/ProcessTick/… per the pretty strings `virtual void vota::CPedeDigital::ProcessTick(uebyte)`).
- Indirect calls (`call_indirect`) are not in the call graph; 12,854 table entries.

---
# Appendix A — Source tree (from __FILE__ strings; count = functions whose code loads that path)

### /home/rubio/tse/uenux2/src
- api/  (80 files)
  - audio/alsa/  (1 files)
    - cwavfile.cpp
  - gui/  (39 files)
    - capplication.cpp
    - capplicationcontextstack.cpp
    - cbmpconversor.cpp
    - cdsimagefield.cpp
    - cfixedimage.cpp
    - cformbuilder.cpp
    - cframedtext.cpp
    - cgrayedframedtext.cpp
    - cimagefield.cpp
    - cimagefieldupdate.cpp
    - cinputfield.h
    - cinputmenufield.cpp
    - cinteractiveform.h
    - cinteractiveformbuilder.cpp
    - cledfieldmt.cpp
    - cmovie.cpp
    - cmoviefield.cpp
    - cpaperformbuilder.h
    - cpowerinformation.cpp
    - cprogressbar.cpp
    - cqrcodeguard.cpp
    - cqrcodeimage.cpp
    - cqrcodeimagepaper.cpp
    - cstepsprogressbar.cpp
    - ctextbox.cpp
    - ctextfield.cpp
    - ctextfieldblinking.cpp
    - ctextfielddoubleline.cpp
    - ctextfieldmt.cpp
    - ctextfieldmultiline.cpp
    - ctextfieldpaper.cpp
    - ctextfieldupdate.cpp
    - ctextrectfield.cpp
    - ctextsource.h
    - iform.h
    - iresource.h
    - iscreen.h
    - primitives.cpp
    - reports/  (1 files)
      - creport.cpp
  - hash/  (3 files)
    - chasharquivo.cpp
    - chashdiretorio.cpp
    - cmontadorhash.cpp
  - hwil/  (4 files)
    - iimpressora.h
    - iimpressorarelatorios.cpp
    - iinput.h
    - ipower.h
  - io/  (7 files)
    - asn/  (2 files)
      - cfileasn.h
      - cpartialfileasn.h
    - cdatamap.h
    - cencryptedfile.cpp
    - cinikey.cpp
    - cinisection.cpp
    - cinistrings.cpp
  - ipc/  (5 files)
    - cmessagequeue.h
    - cthread.cpp
    - posix/  (3 files)
      - cposixmutex.cpp
      - cposixrwmutex.cpp
      - cposixsemaphore.cpp
  - pattern/  (3 files)
    - cpolysingleton.h
    - cpolysingletonlist.h
    - igenericfactory.h
  - persistencia/  (2 files)
    - cdaorepositorio.hpp
    - iuenuxgenericdao.h
  - uelog/  (2 files)
    - cescritorlog.cpp
    - cloga.cpp
  - util/  (14 files)
    - cdate.cpp
    - cdatetime.cpp
    - cdirreader.cpp
    - cgenerictags.cpp
    - cstringutils.cpp
    - csynchronizer.cpp
    - csystem.cpp
    - ctickmanager.cpp
    - ctime.cpp
    - cwait.cpp
    - isystemdatetime.cpp
    - isystemdatetime.h
    - itimerscheduler.cpp
    - itimerscheduler.h
- app/  (259 files)
  - comum/  (184 files)
    - appinfo/  (4 files)
      - cappinfo.cpp
      - servicos/  (3 files)
        - cservicoestadogeralgap.cpp
        - cservicoestadogeralsa.cpp
        - cservicoestadogeralvota.cpp
    - asn/  (7 files)
      - cconversorabrangencia.cpp
      - cconversorcabecalhoentidade.cpp
      - iconversorasn.h
      - iconversorbiometriaasn.h
      - iconversorparcialasn.h
      - legravaentidade.h
      - util.cpp
    - carquivosresultado.cpp
    - carquivossavd.cpp
    - cinfomtlcd.cpp
    - comparecimentomesario/  (22 files)
      - ccontroladorreconhecimetomesario.cpp
      - cregistradormesario.cpp
      - dao/  (1 files)
        - ccomparecimentomesariodao.cpp
      - estados/  (19 files)
        - cconfirmafimregistromesarios.cpp
        - cdigitalmesarionaoreconhecida.cpp
        - cencerraregistromesarios.cpp
        - cgestordadomesario.cpp
        - cimprimiridentificacaomesariosfinal.cpp
        - cimprimiridentificacaomesariosfinal.h
        - climitemesariosregistradosatingido.cpp
        - cmesarioregistrado.cpp
        - cpededigitalmesario.cpp
        - cpedetitulomesario.cpp
        - cpedetitulomesariofinal.cpp
        - cpedetitulomesarioinicial.cpp
        - cpedetitulomesariovotacao.cpp
        - cregistrarmesarios.cpp
        - ctitulomesarioinvalido.cpp
        - ctitulomesariojaregistrado.cpp
        - ctitulomesariovazio.cpp
        - igestordadomesario.cpp
        - ipedetitulomesario.cpp
    - cpacotearquivos.cpp
    - cpath.cpp
    - dados/  (86 files)
      - asn/  (23 files)
        - candidatura/  (1 files)
          - cvisitantefoto.cpp
        - cconversorlocal.cpp
        - eleitor/  (6 files)
          - cconversorbiometriaeleitor.cpp
          - cconversorbiometriaeleitorcifrada.cpp
          - cconversordedo.cpp
          - cconversorentidadeeleitores.cpp
          - cconversorimpedido.cpp
          - cvisitanteeleitor.cpp
        - estadoaplicacao/  (6 files)
          - cconversorajustedatahora.cpp
          - cconversordadocarga.cpp
          - cconversorestadogeral.cpp
          - cconversorestadogeralgap.cpp
          - cconversorestadogeralsa.cpp
          - cconversorestadogeralvota.cpp
        - municipiozona/  (1 files)
          - cconversorcomplementosmunicipios.cpp
        - processoeleitoral/  (5 files)
          - cconversorcargo.cpp
          - cconversordetalhecandidato.cpp
          - cconversororigemconfiguracao.cpp
          - cconversorprocessoeleitoral.cpp
          - cconversortipoidentificadoreleitor.cpp
        - rdv/  (3 files)
          - cconversoreleicoesvota.cpp
          - cconversorvoto.cpp
          - cconversorvotoscargo.cpp
      - ccandidaturas.cpp
      - ccargods.cpp
      - ccargos.cpp
      - cconfiguracaoeleicao.cpp
      - celeitordetalhe.cpp
      - celeitores.cpp
      - cfederacoes.cpp
      - cfotos.cpp
      - chv.cpp
      - cintegridadereferencial.cpp
      - clocal.cpp
      - cpe.cpp
      - crdv.cpp
      - crdvvota.cpp
      - crespostas.cpp
      - dao/  (1 files)
        - celeitordinamicodao.cpp
      - md/  (47 files)
        - candidatura/  (3 files)
          - ccandidatura.cpp
          - cdadoscandidato.cpp
          - cindicesfotos.cpp
        - cdataje.cpp
        - chorarioverao.cpp
        - chorarioveraomunicipio.cpp
        - cinfomunicipio.cpp
        - clocal.cpp
        - correspondencia/  (3 files)
          - ccarga.cpp
          - ccorrespondenciaresultado.cpp
          - cdadosdisponiveiscarga.cpp
        - cpartido.cpp
        - csecaoeleitoral.cpp
        - cvalidadoridentidade.cpp
        - eleitor/  (8 files)
          - cbiometriaeleitor.cpp
          - cdedo.cpp
          - celeitor.cpp
          - celeitordadoshabilitacaobiometrica.cpp
          - celeitordecorator.cpp
          - celeitordinamico.cpp
          - cimpedido.cpp
          - ctransferenciatemporaria.cpp
        - estadoaplicacao/  (6 files)
          - cajustedatahora.cpp
          - cdadocarga.cpp
          - cestadogeral.cpp
          - cestadogeralvota.cpp
          - cestadogeralvota.h
          - clocalidadeeleitoral.cpp
        - iregraidentidade.cpp
        - municipiozona/  (3 files)
          - ccomplementomunicipio.cpp
          - ccomplementosmunicipiosuf.cpp
          - cmunicipio.cpp
        - parametrizacaourna/  (1 files)
          - ctradutorfrase.cpp
        - processoeleitoral/  (10 files)
          - ccargo.cpp
          - cdetalhecandidato.cpp
          - cdetalheconsulta.cpp
          - celeicaodatahora.cpp
          - celeicaope.cpp
          - cnomescargo.cpp
          - cpleito.cpp
          - cprocessoeleitoral.cpp
          - cprocessoeleitoraldto.cpp
          - crespostaconsulta.cpp
        - rdv/  (4 files)
          - cvoto.cpp
          - cvotos.cpp
          - cvotoscargos.cpp
          - cvotoseleicoesvota.cpp
    - gravadores/  (24 files)
      - asn/  (7 files)
        - cassinavotavelbu.cpp
        - cconversorcorrespresultado.cpp
        - cconversorentidadebu.cpp
        - cconversorenvelopegenerico.cpp
        - cconversortipoapuracaosa.cpp
        - cconversorurna.cpp
        - util.cpp
      - cassinador.cpp
      - cgeracaoversoescontratos.cpp
      - cgravadorbu.cpp
      - cgravadorhashes.cpp
      - cgravadorrcsecao.cpp
      - cgravadorutil.cpp
      - cgravadorwsq.cpp
      - iresultado.cpp
      - md/  (9 files)
        - cdadosbuvota.cpp
        - cdependenciascontratos.cpp
        - centidadebu.cpp
        - centidadehashes.cpp
        - cenvelopegenerico.cpp
        - ctipoapuracaosa.cpp
        - curna.cpp
        - cversoesarquivos.cpp
        - cversoescontratos.cpp
    - iinterfaceinit.cpp
    - iinterfacesavd.cpp
    - informacao/  (1 files)
      - cinformacaoeleicao.cpp
    - justificativa/  (2 files)
      - cjustificador.cpp
      - dao/  (1 files)
        - cjustificadordao.cpp
    - log/  (2 files)
      - clogcomum.cpp
      - ieventoslog.cpp
    - md/  (9 files)
      - cabrangencia.cpp
      - ccabecalhoentidade.cpp
      - ccabecalhopacote.cpp
      - cideleitoral.cpp
      - cidentificacaosecao.cpp
      - cidentificacaourna.cpp
      - cidentificacaournacontingencia.cpp
      - cidpacote.cpp
      - cseguranca.cpp
    - nomearquivo/  (1 files)
      - cnomearquivo.cpp
    - reconhecimentobiometrico/  (1 files)
      - ccontrolaarmazenamentodeimagens.cpp
    - relatorios/  (14 files)
      - ccabecalhoqrcodebuilder.cpp
      - ccalculacv.cpp
      - cdatasourcesrelatorio.h
      - cgeradorbu.cpp
      - cgeradorbubase.h
      - cgeradorbuj.cpp
      - cgeradorbuqrcode.cpp
      - cgeradorrelbase.h
      - cgeradorrellistaeleitores.cpp
      - cpartecandidatos.cpp
      - crelatoriotesteimpressora.cpp
      - crelutil.cpp
      - csigverifier.cpp
      - csubstituidortitulo.cpp  [1 fn]
    - util/  (2 files)
      - cmonitoraalimentacao.cpp
      - util.cpp
    - validamidia/  (2 files)
      - cfabricaconteudomidiastart.cpp
      - cvalidamidia.cpp
  - vota/  (75 files)
    - comum/  (2 files)
      - csincronizavota.cpp
      - votadefs.cpp
    - eleitor/  (53 files)
      - cajusteinicial.cpp
      - cconferevotoemcargo.cpp
      - cconfirmavotoemcargo.cpp
      - cconfirmavotosemcandidato.cpp
      - cdefinerotaposreinicio.cpp
      - celeitorvotando.cpp
      - cestadocomdesligamentoautomatico.cpp
      - cfimvotoeleitor.cpp
      - ciniciodeciclo.cpp
      - cinstrucaovotacaoacessibilidade.cpp
      - comum/  (4 files)
        - cinformacaoeleitor.cpp
        - cpoliticaexecucaoeleitor.cpp
        - ctelascargo.cpp
        - ctelasvota.cpp
      - csincronismovotoeleitor.cpp
      - cthreadeleitor.cpp
      - cvotacaostateaudio.cpp
      - fimvotacao/  (16 files)
        - ccopiaresultadoparamr.cpp
        - cemitirmaisbu.cpp
        - cgerabu.cpp
        - cgerarelatorios.cpp
        - cgravaresultado.cpp
        - cimprimindobehb.cpp
        - cimprimindobim.cpp
        - cimprimindobu.cpp
        - cimprimirbjust.cpp
        - cimprimirbuoutrasobrigatorias.cpp
        - climitecopiasbuatingido.cpp
        - cmostraqrcodebu.cpp
        - cmostraqrcodecertificado.cpp
        - cquerimprimirbu.cpp
        - cretirarmr.cpp
        - cverificaqtdbusadicionais.cpp
      - iniciovotacao/  (16 files)
        - auxiliares/  (4 files)
          - cmenufiltrarcandidatosporcargo.cpp
          - cmenufiltrarcandidatosporpartido.cpp
          - cmenuvisualizarcandidatos.cpp
          - cvisualizarcandidatos.cpp
        - cexibealertadesligamento.cpp
        - cgeradadosdinamicos.cpp
        - cgeradorresumozeresima.cpp
        - cimprimindozeresima.cpp
        - cinformacaozeresimatardia.cpp
        - cquerreimprimirzeresima.cpp
        - creimprimindoresumozeresima.cpp
        - creimprimindozeresima.cpp
        - cverificaeleicaopassou.cpp
        - cverificahorariozeresima.cpp
        - testeteclado/  (2 files)
          - cprezeresima.cpp
          - ctesteteclado.cpp
      - votamajoritario/  (1 files)
        - cpedemajoritario.cpp
      - votaproporcional/  (3 files)
        - ccompletaproporcional.cpp
        - cpedenulo.cpp
        - cpedeproporcional.cpp
    - iexecucaovota.cpp
    - log/  (1 files)
      - clogvota.cpp
    - monitor/  (1 files)
      - cthreadmonitor.cpp
    - operador/  (17 files)
      - aguardaeleitor/  (2 files)
        - cmostraeleitorvotando.cpp
        - csuspensaoautomaticaeleitor.cpp
      - comum/  (1 files)
        - cinformacaothreadoperador.cpp
      - confirmaidentidade/  (6 files)
        - ccontrolareconhecimento.cpp
        - cdigitalnaoreconhecida.cpp
        - cdigitalreconhecida.cpp
        - cnomeeleitor.cpp
        - cpededigital.cpp
        - cregistradigitaloperador.cpp
      - cthreadoperador.cpp
      - justificativa/  (2 files)
        - iconfirmajustificativa.cpp
        - iiniciajustificativa.cpp
      - leidentidade/  (2 files)
        - celeitorencontrado.cpp
        - cpedeidentidade.cpp
      - outrasopcoes/  (3 files)
        - caguardaeleitoresvotarem.cpp
        - cencerramentohorarioinvalido.cpp
        - cescolheopcao.cpp

### /home/rubio/tse/uenux2/wasm
- vota_web/  (1 files)
  - vota_web_wasm.cpp

### /home/rubio/.conan2/p/b/libecea1da310e5107/b/src/ecourna
- api/  (26 files)
  - asn/  (1 files)
    - iconversorasn.hpp
  - compression/  (3 files)
    - clzmacompress.cpp
    - czip.cpp
    - icompressor.cpp
  - io/  (1 files)
    - cfile.cpp
  - pattern/  (1 files)
    - cbasetype.hpp
  - security/  (14 files)
    - asn1/  (1 files)
      - ckeyloader.cpp
    - base64.cpp
    - caescipher.cpp
    - cblockcipher.hpp
    - cepesc/  (4 files)
      - ccipheredin.cpp
      - ccipheredout.cpp
      - cinfosalt.cpp
      - cplaintext.cpp
    - chkdfseed.cpp
    - ckey.cpp
    - csha.cpp
    - csiphashmac.cpp
    - imacalgorithm.cpp
    - isymmetriccipher.cpp
  - sql/sqlite/  (3 files)
    - csqlconnection.cpp
    - csqlresultset.cpp
    - csqlstatement.cpp
  - util/  (3 files)
    - cbitarray.cpp
    - cstringutils.cpp
    - csynchronizer.cpp
- app/dados/  (35 files)
  - asn/  (17 files)
    - cconversorcabecalhoentidade.cpp
    - cconversorcargoid.cpp
    - cconversorfase.cpp
    - cconversorfoto.cpp
    - cconversoridentificadoreleitor.cpp
    - cconversorturno.cpp
    - midias/  (2 files)
      - cconversoraplicativo.cpp
      - cconversorinformacaomidia.cpp
    - parametrizacaourna/  (3 files)
      - cconversorlabelparametrizado.cpp
      - cconversorparametrosurna.cpp
      - cconversortitulorelatorio.cpp
    - resultadournacadastro/  (6 files)
      - cconversorapresentacaofotoeleitor.cpp
      - cconversorcomparecimentomesario.cpp
      - cconversorestadocomparecimento.cpp
      - cconversorestadohabilitacaoporcodigo.cpp
      - cconversorhabilitacaobiometrica.cpp
      - cconversorresultadournacadastro.cpp
  - cnumerocpf.cpp
  - cnumeroidentificacaolivre.cpp
  - cnumeroinscricaoeleitoral.cpp
  - cregistroidentificacaoeleitor.cpp
  - cserialmidia.cpp
  - federacoes/  (1 files)
    - cfederacao.cpp
  - iidentificadoreleitor.cpp
  - midias/  (3 files)
    - caplicativo.cpp
    - cautenticacao.cpp
    - cinformacaomidia.cpp
  - processoeleitoral/  (1 files)
    - cconfiguracaomunicipios.cpp
  - resultadournacadastro/  (6 files)
    - cdadoscifracao.cpp
    - cdadoscomparecimento.cpp
    - cestadocomparecimento.cpp
    - cestadohabilitacaoporcodigo.cpp
    - chabilitacaobiometrica.cpp
    - cresultadournacadastro.cpp
  - tiposbasicos.cpp

### OpenSSL (relative paths, ERR file strings) — files per directory
crypto/asn1 (28), crypto/bio (9), crypto/bn (19), crypto/buffer (1), crypto/cmac (1), crypto/conf (5), crypto/context.c (1), crypto/core_algorithm.c (1), crypto/core_fetch.c (1), crypto/core_namemap.c (1), crypto/dh (9), crypto/dsa (7), crypto/dso (1), crypto/ec (25), crypto/encode_decode (3), crypto/engine (17), crypto/err (4), crypto/evp (38), crypto/ex_data.c (1), crypto/ffc (3), crypto/hmac (1), crypto/init.c (1), crypto/initthread.c (1), crypto/lhash (1), crypto/mem_sec.c (1), crypto/modes (1), crypto/o_str.c (1), crypto/objects (4), crypto/packet.c (1), crypto/param_build.c (1), crypto/params.c (1), crypto/params_dup.c (1), crypto/params_from_text.c (1), crypto/passphrase.c (1), crypto/pem (3), crypto/pkcs12 (5), crypto/property (4), crypto/provider_child.c (1), crypto/provider_conf.c (1), crypto/provider_core.c (1), crypto/rand (3), crypto/rsa (18), crypto/self_test_core.c (1), crypto/sm2 (3), crypto/sparse_array.c (1), crypto/stack (1), crypto/threads_pthread.c (1), crypto/ui (3), crypto/x509 (4), providers/common (3), providers/implementations (101), ssl/record (1)

### emscripten system libs
- /emsdk/emscripten/system/lib/libcxxabi/src/cxa_demangle.cpp
- /emsdk/emscripten/system/lib/libcxxabi/src/demangle/ItaniumDemangle.h
- /emsdk/emscripten/system/lib/libcxxabi/src/demangle/Utility.h
- /emsdk/emscripten/system/lib/libcxxabi/src/fallback_malloc.cpp
- /emsdk/emscripten/system/lib/libcxxabi/src/private_typeinfo.cpp

# Appendix B — Class hierarchies (RTTI inheritance; number = virtual slots in the primary vtable)

### Voting-flow state machine: states derived from comum::CAppState  (`comum::CAppState`, 9 slots, 132 direct subclasses)
- comum::CConfirmaFimRegistroMesarios
- comum::CDigitalMesarioNaoReconhecida
- comum::CEncerraRegistroMesarios
- comum::CGestorDadoMesario
- comum::CLimiteMesariosRegistradosAtingido
- comum::CMesarioRegistrado
- comum::CPedeDigitalMesario
- comum::CPedeTituloMesario
- comum::CRegistrarMesarios
- comum::CTituloMesarioInvalido
- comum::CTituloMesarioJaRegistrado
- comum::CTituloMesarioVazio
- comum::IGestorDadoMesario
  - comum::CGestorDadoMesarioFinal
  - comum::CGestorDadoMesarioInicial
  - comum::CGestorDadoMesarioVotacao
- comum::IPedeTituloMesario
  - comum::CPedeTituloMesarioFinal
  - comum::CPedeTituloMesarioInicial
  - comum::CPedeTituloMesarioVotacao
- vota::CAguardaEleitoresVotarem
- vota::CAguardaInicio
- vota::CAguardaInspecao
- vota::CAguardaMensagem
- vota::CAnoInformadoInvalido
- vota::CCancelaHabilitacaoEleitor
- vota::CConfirmaAudio
- vota::CConfirmaEncerramento
- vota::CConfirmaImpressaoZeresima
- vota::CConfirmaInspecionada
- vota::CConfirmaRegerarZeresima
- vota::CContadoresBiometria
- vota::CControlaReconhecimento
- vota::CCopiaResultadoParaMR
- vota::CDadoEleitorNaoConfere
- vota::CDefineRotaPosReinicio
- vota::CDefineRotaPreVotacao
- vota::CDesabilitaAudioEleitor
- vota::CDigitalNaoCapturada
- vota::CDigitalNaoReconhecida
- vota::CDigitalNaoReconhecidaDecBiometria
- vota::CDigitalNaoReconhecidaPorTempo
- vota::CDigitalReconhecida
- vota::CEleitorDemorando
- vota::CEleitorEncontrado
- vota::CEleitorImpedidoJustificarVotoCPF
- vota::CEleitorJaVotou
- vota::CEleitorMenor16Anos
- vota::CEleitorVotando
- vota::CEleitorVotouNaoVotou
- vota::CEmitirMaisBU
- vota::CEncerramentoAntecipado
- vota::CEncerramentoHorarioInvalido
- vota::CEscolheOpcao
- vota::CEstadoComDesligamentoAutomatico
  - vota::CAplicacaoEncerrada
  - vota::CMostraQRCodeBU
  - vota::CMostraQRCodeCertificado
  - vota::CVerificaHorarioZeresima
  - vota::testeteclado::CBase
    - vota::testeteclado::CPreZeresima
    - vota::testeteclado::CRetomada
  - vota::testeteclado::CEnviarManutencao
  - vota::testeteclado::CErroTesteTecladoFim
  - vota::testeteclado::CEsperaRetestar
  - vota::testeteclado::CTesteFalhou
  - vota::testeteclado::CTesteTeclado
- vota::CExibeAlertaDesligamento
- vota::CFimAquisicaoVotos
- vota::CFimVotoEleitor
- vota::CFinalizaAquisicao
- vota::CFinalizaOperador
- vota::CGeraBU
- vota::CGeraDadosDinamicos
- vota::CGeraRelatorios
- vota::CGeraResumoZeresimaBase
  - vota::CGeraResumoZeresima
  - vota::CRegeraResumoZeresima
- vota::CGeraZeresimaBase
  - vota::CGeraZeresima
  - vota::CRegerarZeresima
- vota::CGravaResultado
- vota::CHabilitaAudioEleitor
- vota::CHabilitaAudioManualmente
- vota::CHabilitacaoAudioNaoPermitida
- vota::CHorarioVotacaoTerminou
- vota::CIdentidadeInvalida
- vota::CImpressaoEstadoUrna
- vota::CImpressaoListaEleitores
- vota::CImpressaoPU
- vota::CImpressaoVersaoPacotes
- vota::CImpressaoZeresimaTardia
- vota::CImprimindoBEHB
- vota::CImprimindoBU
- vota::CImprimindoBim
- vota::CImprimindoZeresima
- vota::CImprimirBJust
- vota::CImprimirBUOutrasObrigatorias
- vota::CInformaAnoDesabilitadoDemo
- vota::CInformaAnoNascimentoErrado
- vota::CInformaBioDesabilitadaDemo
- vota::CInformaEleitorPodeVotar
- vota::CInformacaoZeresimaTardia
- vota::CIniciaFinalizacao
- vota::CInicioBU
- vota::CInicioVotacao
- vota::CInicioZeresima
- vota::CIniciodeCiclo
- vota::CInspecionaUrna
- vota::CJustificativaEfetuada
- vota::CLimiteCopiasBUAtingido
- vota::CMaisInformacoes
- vota::CMenuFiltrarCandidatosPorCargo
- vota::CMenuFiltrarCandidatosPorNumero
- vota::CMenuFiltrarCandidatosPorPartido
- vota::CMenuVisualizarCandidatos
- vota::CMostraEleitorVotando
- vota::CMostraTelaContinuaVotacao
- vota::CNomeEleitor
- vota::CPedeAnoNascimento

### Screen abstraction api::IScreen  (`api::IScreen`, 39 slots, 1 direct subclasses)
- simulador::CWasmScreen

### Text to speech  (`api::ITextToSpeech`, 12 slots, 2 direct subclasses)
- api::CRHVoiceTextToSpeech
- simulador::CWasmNullTextToSpeech

### Security: symmetric ciphers  (`ecourna::api::security::ISymmetricCipher`, 4 slots, 1 direct subclasses)
- ecourna::api::security::CBlockCipher<ecourna::api::security::CAesCipher, ecourna::api::security::CTrng>

### SQLite DAO  (`ecourna::api::sql::sqlite::CSqlConnection`, 10 slots, 0 direct subclasses)


### ASN.1 runtime data model (asnparser)  (`ASN1::AbstractData`, 7 slots, 7 direct subclasses)
- ASN1::BOOLEAN
- ASN1::CHOICE
  - ModuloRegistroDigitalVoto::Eleicoes
  - ModuloTiposEleitorais::CodigoCargoConsulta
  - ModuloTiposEleitorais::IdentificadorEleitor
  - ModuloTiposResultadosEcoUrna::TipoApuracaoSA
- ASN1::ConstrainedObject
  - ASN1::AbstractString
    - ASN1::GeneralString
      - ModuloFederacoes::Federacao::nome::value_type
      - ModuloFederacoes::Federacao::sigla::value_type
      - ModuloTiposEleitorais::DataHoraJE
      - ModuloTiposEleitorais::SiglaUF
    - ASN1::NumericString
      - ModuloRegistroDigitalVoto::VotoDigitado
      - ModuloTiposEleitorais::DataJE
      - ModuloTiposEleitorais::HoraJE
      - ModuloTiposEleitorais::VersaoMontagem
  - ASN1::INTEGER
    - ASN1::Constrained_INTEGER<(ASN1::ConstraintType)2, 0, 99999999u>
    - ASN1::Constrained_INTEGER<(ASN1::ConstraintType)2, 0, 99999u>
    - ASN1::Constrained_INTEGER<(ASN1::ConstraintType)2, 0, 9999u>
    - ASN1::Constrained_INTEGER<(ASN1::ConstraintType)2, 0, 999u>
    - ASN1::Constrained_INTEGER<(ASN1::ConstraintType)2, 0, 99u>
    - ASN1::Constrained_INTEGER<(ASN1::ConstraintType)2, 1, 99999u>
    - ASN1::Constrained_INTEGER<(ASN1::ConstraintType)2, 1, 9999u>
    - ASN1::Constrained_INTEGER<(ASN1::ConstraintType)2, 100, 999u>
  - ASN1::OCTET_STRING
    - ASN1::Constrained_OCTET_STRING<ASN1::SizeConstraint<2u, 4, 4u> >
  - ASN1::SEQUENCE_OF_Base
    - ASN1::SEQUENCE_OF<ASN1::Constrained_INTEGER<(ASN1::ConstraintType)2, 0, 99u>, ASN1::EmptyConstraint>
    - ASN1::SEQUENCE_OF<ASN1::GeneralString, ASN1::EmptyConstraint>
    - ASN1::SEQUENCE_OF<ModuloBoletimUrna::HistoricoVotoImpresso, ASN1::EmptyConstraint>
    - ASN1::SEQUENCE_OF<ModuloBoletimUrna::ResultadoVotacao, ASN1::EmptyConstraint>
    - ASN1::SEQUENCE_OF<ModuloBoletimUrna::ResultadoVotacaoPorEleicao, ASN1::EmptyConstraint>
    - ASN1::SEQUENCE_OF<ModuloBoletimUrna::TotalVotosCargo, ASN1::EmptyConstraint>
    - ASN1::SEQUENCE_OF<ModuloBoletimUrna::TotalVotosVotavel, ASN1::EmptyConstraint>
    - ASN1::SEQUENCE_OF<ModuloConfiguracaoMunicipios::ConfiguracaoMunicipio, ASN1::EmptyConstraint>
    - ASN1::SEQUENCE_OF<ModuloDadosDisponiveisCarga::MunicipioDisponivel, ASN1::EmptyConstraint>
    - ASN1::SEQUENCE_OF<ModuloEleitores::Dedo, ASN1::EmptyConstraint>
      - ModuloEleitores::BiometriaEleitor::dedos::value_type
    - ASN1::SEQUENCE_OF<ModuloEstadoGeralUrna::DadoCorrespondencia, ASN1::SizeConstraint<2u, 0, 10u> >
    - ASN1::SEQUENCE_OF<ModuloFederacoes::Federacao, ASN1::EmptyConstraint>
    - ASN1::SEQUENCE_OF<ModuloInformacaoMidia::Aplicativo, ASN1::EmptyConstraint>
    - ASN1::SEQUENCE_OF<ModuloLocal::IdentificacaoAgregada, ASN1::EmptyConstraint>
    - ASN1::SEQUENCE_OF<ModuloLocal::SecaoEleitoral, ASN1::EmptyConstraint>
    - ASN1::SEQUENCE_OF<ModuloParametrizacaoUrna::TituloRelatorio, ASN1::EmptyConstraint>
    - ASN1::SEQUENCE_OF<ModuloPartidos::Partido, ASN1::EmptyConstraint>
    - ASN1::SEQUENCE_OF<ModuloResultadoUrnaCadastro::ComparecimentoMesario, ASN1::EmptyConstraint>
      - ModuloResultadoUrnaCadastro::DadosComparecimento::mesariosAbertura::value_type
      - ModuloResultadoUrnaCadastro::DadosComparecimento::mesariosEncerramento::value_type
    - ASN1::SEQUENCE_OF<ModuloResultadoUrnaCadastro::EstadoComparecimento, ASN1::EmptyConstraint>
    - ASN1::SEQUENCE_OF<ModuloResultadoUrnaCadastro::IdentificacaoJustificativa, ASN1::EmptyConstraint>
    - ASN1::SEQUENCE_OF<ModuloTiposEleitorais::ArquivoAssinatura, ASN1::EmptyConstraint>
    - ASN1::SEQUENCE_OF<ModuloTiposEleitorais::CabecalhoPacote, ASN1::EmptyConstraint>
    - ASN1::SEQUENCE_OF<ModuloTiposEleitorais::IdentificadorEleitor, ASN1::EmptyConstraint>
- ASN1::ENUMERATED
  - ModuloEleitores::NecessidadeEspecial
  - ModuloEstadoGeralGap::UrnaAplicativo
  - ModuloEstadoGeralSA::EstadoSA
  - ModuloEstadoGeralUrna::EstadoUrna
  - ModuloEstadoGeralUrna::TipoAjusteDataHora
  - ModuloEstadoGeralUrna::TipoUrnaOperacao
  - ModuloEstadoGeralVota::EstadoEncerramento
  - ModuloEstadoGeralVota::EstadoVota
  - ModuloImpedidos::TipoImpedimento
  - ModuloInformacaoMidia::TipoAplicativo
  - ModuloInformacaoMidia::TipoMidia
  - ModuloParametrizacaoUrna::FormaSuspenderComVoto
  - ModuloParametrizacaoUrna::FormaSuspenderSemVoto
  - ModuloProcessoEleitoral::OrigemConfiguracao
  - ModuloRegistroDigitalVoto::TipoVoto
  - ModuloResultadoUrnaCadastro::ErroLeituraBiometria
  - ModuloResultadoUrnaCadastro::EstadoApresentacaoFotoEleitor
  - ModuloResultadoUrnaCadastro::EstadoColetaDigital
  - ModuloResultadoUrnaCadastro::ResultadoApresentacaoFotoEleitor
  - ModuloResultadoUrnaCadastro::SituacaoComparecimentoEleitor
  - ModuloResultadoUrnaCadastro::SituacaoHabilitacaoAudio
  - ModuloResultadoUrnaCadastro::SituacaoReconhecimentoMesario
  - ModuloTiposCadastro::TipoLocalVotacao
  - ModuloTiposEcoUrna::ModeloEquipamento
  - ModuloTiposEcoUrna::SituacaoArquivo
  - ModuloTiposEleitorais::Fase
  - ModuloTiposEleitorais::FormatoImagem
  - ModuloTiposEleitorais::Sistema
  - ModuloTiposEleitorais::TipoCargoConsulta
  - ModuloTiposEleitorais::TipoDedo
  - ModuloTiposEleitorais::TipoIdentificadorEleitor
  - ModuloTiposEleitorais::Turno
  - ModuloTiposPacotes::TipoPacote
  - ModuloTiposResultadosEcoUrna::MotivoApuracaoEletronica
  - ModuloTiposResultadosEcoUrna::MotivoApuracaoManual
  - ModuloTiposResultadosEcoUrna::MotivoApuracaoMistaComBU
  - ModuloTiposResultadosEcoUrna::MotivoApuracaoMistaComMR
- ASN1::GeneralizedTime
- ASN1::SEQUENCE
  - ModuloBoletimUrna::DadosSA
  - ModuloBoletimUrna::DadosSecao
  - ModuloBoletimUrna::DetalhamentoComparecimento
  - ModuloBoletimUrna::EntidadeBoletimUrna
  - ModuloBoletimUrna::HistoricoVotoImpresso
  - ModuloBoletimUrna::IdentificacaoVotavel
  - ModuloBoletimUrna::ResultadoVotacao
  - ModuloBoletimUrna::ResultadoVotacaoPorEleicao
  - ModuloBoletimUrna::TotalVotosCargo
  - ModuloBoletimUrna::TotalVotosVotavel
  - ModuloCandidatos::EntidadeCandidatos
  - ModuloComplementosMunicipios::ComplementoMunicipio
  - ModuloComplementosMunicipios::EntidadeComplementosMunicipios
  - ModuloComplementosMunicipios::HorarioVerao
  - ModuloConfiguracaoMunicipios::ConfiguracaoMunicipio
  - ModuloConfiguracaoMunicipios::EntidadeConfiguracaoMunicipios
  - ModuloConfiguracaoMunicipios::HorariosUrna
  - ModuloDadosDisponiveisCarga::DadosDisponiveisCarga
  - ModuloDadosDisponiveisCarga::MunicipioDisponivel
  - ModuloEleicao::EntidadeEleicao
  - ModuloEleitores::BiometriaEleitor
  - ModuloEleitores::BiometriaEleitorCifrada
  - ModuloEleitores::Dedo
  - ModuloEleitores::EleitorSequencia

### Input devices  (`api::IInput`, 0 slots, 2 direct subclasses)
- api::IInputKbd
  - simulador::CWasmInputKbd
- api::IInputMT
  - simulador::CWasmInputMT

### Sound  (`api::ISound`, 0 slots, 2 direct subclasses)
- simulador::CWasmNullSound
- simulador::CWasmWebSound

### Timers  (`api::ITimer`, 0 slots, 2 direct subclasses)
- api::CTimer
- simulador::CWasmTimer

### Largest app class hierarchies (root → descendants)
- api::CState: 199
- api::IFormFieldBase<api::IScreen>: 24
- api::IText: 19
- ecourna::api::pattern::NonCopyable: 16
- api::IFormFieldBase<api::IScreenMT>: 11
- comum::IResultado: 10
- comum::CItemMenu: 9
- api::persistencia::IDAO: 9
- api::IReportPart: 8
- api::IImage: 5
- api::IFormFieldBase<api::IPaper>: 5
- api::IPreShow<api::IScreen>: 4
- api::IInputValidation: 4
- api::IInput: 4
- api::CThread: 4
- ecourna::app::dados::IIdentificadorEleitor: 3
- ecourna::api::pattern::IObservableProgressWithDescription: 3
- comum::md::IRegraIdentidade: 3
- api::ISyncCtl: 3
- vota::impl::ISincronismoVotoEleitor: 2
- vota::impl::IPoliticaExecucaoEleitor: 2
- vota::IExecucaoVota: 2
- ecourna::api::security::IRng: 2
- comum::CGeradorRelBase<comum::CRdvVota, comum::CEleitores>: 2
- api::ITimerScheduler: 2
- api::ITimer: 2
- api::ITextToSpeech: 2
- api::ISystemDateTime: 2
- api::ISound: 2
- api::IPaper: 2
- api::IObserver<std::__2::shared_ptr<api::IImage> >: 2
- api::IObservable<std::__2::shared_ptr<api::IImage> >: 2
- api::IImpressora: 2
- api::IEsperaAudio: 2
- api::CPriorityMessageQueue<api::SMessage>: 2
- api::CMessageInterface: 2
- vota::testeteclado::impl::IGeradorTeclas: 1
- vota::impl::IInformacaoThreadOperador: 1
- ecourna::api::sql::ISqlStatement: 1
- ecourna::api::sql::ISqlResultSet: 1

### App classes per namespace (RTTI count)
api (94), api::(anonymous namespace) (1), api::BatteryIconDataSource<(api (4), api::CApplication (1), api::CDataImage<comum (1), api::CDataImage<std (2), api::CDataMap<comum (1), api::CDataText<api (2), api::CDataText<comum (5), api::CDataText<std (3), api::CDataText<vota (2), api::CDataTextFmt<api (1), api::CDataTextFmt<comum (3), api::CDataTextFmt<std (3), api::CDefaultGenericFactory<api (4), api::CFormStack<api (3), api::CImageFieldUpdate (1), api::CInputField<api (1), api::CInputFieldControl<api (2), api::CInputFieldControlBase<api (2), api::CInputMenuField (1), api::CInteractiveForm<api (2), api::CMaskedTextField<api (2), api::CMovieField (1), api::CPaperFormBuilder (1), api::CPreShowNothing<api (2), api::CPriorityMessageQueue<api (1), api::CStateContext<comum (1), api::CSystem (1), api::CTextFieldBlinking (1), api::CTextFieldUpdate (1), api::CTextFieldUpdateMT (1), api::IForm<api (3), api::IFormField<api (3), api::IFormFieldBase<api (3), api::IFormImpressao<api (1), api::IGenericFactory<api (4), api::IInputField<api (4), api::IObservable<std (4), api::IObserver<std (1), api::IPreShow<api (3), api::ISound (2), api::persistencia (4), api::teste (2), comum (77), comum::(anonymous namespace) (2), comum::CControladorReconhecimentoMesario (2), comum::CDataSourcesRelatorio<comum (1), comum::CEleitores (2), comum::CGeradorBUBase<comum (1), comum::CGeradorRelBase<comum (1), comum::CGeradorRelVersaoPacoteDados (1), comum::CriaHeader(std (1), comum::IServicoEstado<comum (4), comum::asn (120), comum::dao (6), comum::impl (2), comum::md (17), comum::servico (2), ecourna::api (108), ecourna::app (39), simulador (19), simulador::(anonymous namespace) (1), simulador::CWasmScreenMT (1), vota (182), vota::CConfereVotoEmCargo<vota (10), vota::CRelVotaUtil (2), vota::CTelasVota (1), vota::CVotacaoStateAudio (2), vota::IConfereVotoEmCargo (6), vota::impl (6), vota::testeteclado (10)

### vota:: classes
CAguardaEleitoresVotarem, CAguardaInicio, CAguardaInspecao, CAguardaMensagem, CAjusteInicial, CAnoInformadoInvalido, CAplicacaoEncerrada, CAssinadorVota, CCancelaHabilitacaoEleitor, CCandidatoInapto, CCandidatoInexistente, CCompletaProporcional, CConfereVotoEmCargo<vota::CCandidatoInapto, (vota::ETelaVotacao)14>, CConfereVotoEmCargo<vota::CCandidatoInexistente, (vota::ETelaVotacao)12>, CConfereVotoEmCargo<vota::CConfirmaVotoLegenda, (vota::ETelaVotacao)10>, CConfereVotoEmCargo<vota::CConfirmaVotoNominal, (vota::ETelaVotacao)2>, CConfereVotoEmCargo<vota::CMajoritarioBranco, (vota::ETelaVotacao)4>, CConfereVotoEmCargo<vota::CMajoritarioNulo, (vota::ETelaVotacao)7>, CConfereVotoEmCargo<vota::CMajoritarioRepetido, (vota::ETelaVotacao)16>, CConfereVotoEmCargo<vota::CMajoritarioValido, (vota::ETelaVotacao)2>, CConfereVotoEmCargo<vota::CProporcionalBranco, (vota::ETelaVotacao)4>, CConfereVotoEmCargo<vota::CProporcionalNulo, (vota::ETelaVotacao)7>, CConfirmaAudio, CConfirmaEncerramento, CConfirmaImpressaoZeresima, CConfirmaInspecionada, CConfirmaJustificativa, CConfirmaJustificativaTemporario, CConfirmaJustificativaTransito, CConfirmaMajoritario, CConfirmaProporcional, CConfirmaRegerarZeresima, CConfirmaVotoEmCargo, CConfirmaVotoLegenda, CConfirmaVotoNominal, CConfirmaVotoSemCandidato, CContadoresBiometria, CControlaReconhecimento, CControladorRegistraMesariosVota, CCopiaResultadoParaMR, CDadoEleitorNaoConfere, CDefineRotaPosReinicio, CDefineRotaPreVotacao, CDesabilitaAudioEleitor, CDigitalNaoCapturada, CDigitalNaoReconhecida, CDigitalNaoReconhecidaDecBiometria, CDigitalNaoReconhecidaPorTempo, CDigitalReconhecida, CEleitorDemorando, CEleitorEncontrado, CEleitorImpedidoJustificar, CEleitorImpedidoJustificarVotoCPF, CEleitorImpedidoJustificarVotoTransito, CEleitorJaVotou, CEleitorMenor16Anos, CEleitorNaoEncontrado, CEleitorNaoPossuiCargosParaVotar, CEleitorNaoTemIdadeMinima, CEleitorOptouPorVotarEmTransito, CEleitorVotando, CEleitorVotouNaoVotou, CEmitirMaisBU, CEncerramentoAntecipado, CEncerramentoHorarioInvalido, CEscolheOpcao, CEstadoComDesligamentoAutomatico, CExecucaoVota, CExecucaoVotaCooperativa, CExibeAlertaDesligamento, CFimAquisicaoVotos, CFimVotoEleitor, CFinalizaAquisicao, CFinalizaOperador, CGeraBU, CGeraDadosDinamicos, CGeraRelatorios, CGeraResumoZeresima, CGeraResumoZeresimaBase, CGeraZeresima, CGeraZeresimaBase, CGravaResultado, CHabilitaAudioEleitor, CHabilitaAudioManualmente, CHabilitacaoAudioNaoPermitida, CHorarioVotacaoTerminou, CIdentidadeInvalida, CImpressaoEstadoUrna, CImpressaoListaEleitores, CImpressaoPU, CImpressaoVersaoPacotes, CImpressaoZeresimaTardia, CImprimindoBEHB, CImprimindoBU, CImprimindoBim, CImprimindoZeresima, CImprimirBJust, CImprimirBUOutrasObrigatorias, CInformaAnoDesabilitadoDemo, CInformaAnoNascimentoErrado, CInformaBioDesabilitadaDemo, CInformaEleitorPodeVotar, CInformacaoZeresimaTardia, CIniciaFinalizacao, CIniciaJustificativa, CIniciaJustificativaTemporario, CIniciaJustificativaTransito, CInicioBU, CInicioVotacao, CInicioZeresima, CIniciodeCiclo, CInspecionaUrna, CInstrucaoVotacaoAcessibilidade, CItemImprimeEstadoUrnaVota, CItemImprimeListaEleitoresVota, CItemParametrosUrnaVota, CItemVersoesPacotesVota, CItemVisualizarCandidatosVota, CJustificativaEfetuada, CLimiteCopiasBUAtingido, CLogVota, CMaisInformacoes, CMajoritarioBranco, CMajoritarioNulo, CMajoritarioRepetido, CMajoritarioValido, CMenuFiltrarCandidatosPorCargo, CMenuFiltrarCandidatosPorNumero, CMenuFiltrarCandidatosPorPartido, CMenuMaisInformacoesVota, CMenuVisualizarCandidatos, CMessageEleitor, CMessageOperador, CMostraEleitorVotando, CMostraQRCodeBU, CMostraQRCodeCertificado, CMostraTelaContinuaVotacao, CNomeEleitor, CPedeAnoNascimento, CPedeAnoNascimentoSemBiometria, CPedeDigital, CPedeIdentidade, CPedeMajoritario, CPedeNominal, CPedeNulo, CPedeProporcional, CPedeTituloEncerramento, CPerguntaCodigoSuspensao, CPerguntaEleitorVotando, CPerguntaFilaEleitorVazia, CPreShowFormVota, CPreShowProgressBar, CProcuraEleitor, CProgressoEncerramento, CProporcionalBranco, CProporcionalNulo, CQuerImprimirBU, CQuerImprimirZeresima, CQuerReimprimirZeresima, CRegeraResumoZeresima, CRegerarZeresima, CRegistraDigitalOperador, CRegistroMesarioEncerrado, CReimprimindoResumoZeresima, CReimprimindoZeresima, CReinicioComparecimentoMesario, CReinicioVotacao, CRelVotaUtil::CortaPapel()::$_0, CRelVotaUtil::CriaCabecalho(std::__2::basic_string<char, std::__2::char_traits<char>, std::__2::allocator<char> > const&)::$_0, CRetirarMR, CSincronismoEleitor, CSincronismoOperador, CSuspensaoAutomaticaEleitor, CTelasVota::CriaTelaVisualizacaoCandidato(comum::md::CCandidatura const&, unsigned long, unsigned long)::$_2, CTentativaCapturaDigitalEsgotada, CThreadEleitor, CThreadMonitor, CThreadOperador, CThreadVota, CTituloEncerramentoInvalido, CUrnaInspecionada, CValidaIdentidade, CVerificaDadoEleitor, CVerificaEleicaoPassou, CVerificaHorarioZeresima, CVerificaQtdBUsAdicionais, CVisualizarCandidatos, CVotacaoStateAudio, CVotacaoStateAudio::IniciarEsperaFimAudio()::$_0, CVotacaoStateAudio::IniciarEsperaFimAudio()::$_1, IAjusteInicial, IConfereVotoEmCargo, IConfereVotoEmCargo::ExecutarAposAudioAtual(std::__2::function<void ()>)::$_0, IConfereVotoEmCargo::ExecutarAposAudioAtual(std::__2::function<void ()>)::$_1, IConfereVotoEmCargo::ExecutarAposAudioAtual(std::__2::function<void ()>)::$_2, IConfereVotoEmCargo::ProcessTickAudio(unsigned char)::$_0, IConfereVotoEmCargo::StartStateAudio()::$_0, IConfereVotoEmCargo::StartStateAudio()::$_1, IConfirmaJustificativa, IEleitorImpedidoVotar, IExecucaoVota, IIniciaJustificativa, IQRCodeBUDS, impl::CInformacaoThreadOperador, impl::CPoliticaExecucaoEleitor, impl::CSincronismoVotoEleitor, impl::IInformacaoThreadOperador, impl::IPoliticaExecucaoEleitor, impl::ISincronismoVotoEleitor, testeteclado::CBase, testeteclado::CEnviarManutencao, testeteclado::CErroTesteTecladoFim, testeteclado::CEsperaRetestar, testeteclado::CPreZeresima, testeteclado::CRetomada, testeteclado::CTesteFalhou, testeteclado::CTesteTeclado, testeteclado::impl::CGeradorTeclasAleatorio, testeteclado::impl::IGeradorTeclas

### comum:: classes
(anonymous namespace)::MontaCabecalho()::$_0, (anonymous namespace)::MontaCabecalho(comum::CGeradorRelEleitoresHabBiograf::EGeradorRelatorio)::$_0, CAbstractTelaProgresso, CAppState, CAppStateContext, CAssinador, CCalculaCV, CConfirmaFimRegistroMesarios, CControladorReconhecimentoMesario::SalvarBiometriaMesarioNaoRegistrado(std::__2::vector<unsigned char, std::__2::allocator<unsigned char> > const&)::$_0, CControladorReconhecimentoMesario::SalvarBiometriaMesarioRegistrado(std::__2::vector<unsigned char, std::__2::allocator<unsigned char> > const&, std::__2::basic_string<char, std::__2::char_traits<char>, std::__2::allocator<char> > const&)::$_0, CCopiadorMR, CCopiadorWSQMR, CDataSourcesRelatorio<comum::CRdvVota, comum::CEleitores>::GetLinhasEleitoresAptos()::{lambda()#1}, CDigitalMesarioNaoReconhecida, CEleitores, CEleitores::GetEleitoresEstaticos(std::__2::basic_string<char, std::__2::char_traits<char>, std::__2::allocator<char> > const&)::$_0, CEleitores::GetEleitoresImpedidos(std::__2::vector<std::__2::basic_string<char, std::__2::char_traits<char>, std::__2::allocator<char> >, std::__2::allocator<std::__2::basic_string<char, std::__2::char_traits<char>, std::__2::allocator<char> > > > const&)::$_0, CEncerraRegistroMesarios, CGeracaoVersoesContratos, CGeradorBU, CGeradorBUBase<comum::CRdvVota, comum::CEleitores>, CGeradorBUQRCode, CGeradorBUQRCodeVota, CGeradorRelBase<comum::CRdvVota, comum::CEleitores>, CGeradorRelPU, CGeradorRelVersaoPacoteDados, CGeradorRelVersaoPacoteDados::ImprimeTituloJE()::$_0, CGestorDadoMesario, CGestorDadoMesarioFinal, CGestorDadoMesarioInicial, CGestorDadoMesarioVotacao, CGravadorBU, CGravadorEnvelopeArquivo, CGravadorHashes, CGravadorLog, CGravadorRCSecao, CGravadorRDV, CGravadorVersoesArquivos, CGravadorWSQ, CImprimirExtratoCarga, CInfoMTLCD, CInterfaceMensagemVazia, CItemImprimeEstadoUrna, CItemImprimeListaEleitores, CItemMenu, CItemParametrosUrna, CItemVersoesPacotes, CLimiteMesariosRegistradosAtingido, CMenuBase, CMesarioRegistrado, CParteCandidatos, CParteCandidatosConsultas, CParteCandidatosMajoritarios, CParteCandidatosProporcionais, CParteCargos, CParteEleitores, CParteRdv, CPedeDigitalMesario, CPedeTituloMesario, CPedeTituloMesarioFinal, CPedeTituloMesarioInicial, CPedeTituloMesarioVotacao, CQRCodeDS, CRdv, CRdvPosicionadorVota, CRdvVota, CRegistrarMesarios, CRelatorioTesteImpressora, CServicoEstadoGeral, CServicoEstadoGeralGap, CServicoEstadoGeralSA, CServicoEstadoGeralVota, CSigVerifier, CTituloMesarioInvalido, CTituloMesarioJaRegistrado, CTituloMesarioVazio, CriaHeader(std::__2::basic_string<char, std::__2::char_traits<char>, std::__2::allocator<char> > const&, unsigned int, unsigned short, unsigned short, std::__2::basic_string<char, std::__2::char_traits<char>, std::__2::allocator<char> > const&, comum::md::estadoaplicacao::CDadoCorrespondencia const&, bool, api::CDateTime)::$_0, IControladorRegistraMesarios, IDataSourceAptos, IEventosLog, IGestorDadoMesario, IGravador, IGravadorEnvelope, IInterfaceInit, IInterfaceMensagem, IInterfaceSavd, IPedeTituloMesario, IResultado, IServicoEstado<comum::md::estadoaplicacao::CEstadoGeral, comum::asn::CConversorEstadoGeral>, IServicoEstado<comum::md::estadoaplicacao::CEstadoGeralGap, comum::asn::CConversorEstadoGeralGap>, IServicoEstado<comum::md::estadoaplicacao::CEstadoGeralSA, comum::asn::CConversorEstadoGeralSA>, IServicoEstado<comum::md::estadoaplicacao::CEstadoGeralVota, comum::asn::CConversorEstadoGeralVota>, asn::CConversorAbrangencia, asn::CConversorAjusteDataHora, asn::CConversorBiometriaEleitor, asn::CConversorBiometriaEleitorCifrada, asn::CConversorCabecalhoEntidade, asn::CConversorCabecalhoPacote, asn::CConversorCandidatura, asn::CConversorCandidaturas, asn::CConversorCargo, asn::CConversorComplementoMunicipio, asn::CConversorComplementosMunicipios, asn::CConversorCorrespResultado, asn::CConversorDadoCarga, asn::CConversorDadoCorrespondencia, asn::CConversorDadoLocal, asn::CConversorDadosCandidato, asn::CConversorDadosDisponiveisCarga, asn::CConversorDedo, asn::CConversorDetalheCandidato, asn::CConversorDetalheConsulta, asn::CConversorEleicaoPE, asn::CConversorEleicoesVota, asn::CConversorEntidadeBU, asn::CConversorEntidadeEleitores, asn::CConversorEntidadeHashes, asn::CConversorEntidadePartidos, asn::CConversorEnvelopeGenerico, asn::CConversorEstadoGeral, asn::CConversorEstadoGeralGap, asn::CConversorEstadoGeralSA, asn::CConversorEstadoGeralVota, asn::CConversorFotoCandidato, asn::CConversorHashArquivo, asn::CConversorHistoricoVotoImpresso, asn::CConversorHorarioVerao, asn::CConversorIdentificacaoAgregada, asn::CConversorImpedido, asn::CConversorLocal, asn::CConversorLocalidadeEleitoral, asn::CConversorMunicipio, asn::CConversorNomesCargo, asn::CConversorNumViasImpressasRelatorios, asn::CConversorOrigemConfiguracao, asn::CConversorPartido, asn::CConversorPleito, asn::CConversorProcessoEleitoral, asn::CConversorRegistroDigitalVoto<comum::asn::CConversorEleicoesVota>, asn::CConversorSecaoEleitoral, asn::CConversorSeguranca, asn::CConversorSituacoesEleicoes, asn::CConversorSuplencia, asn::CConversorTipoApuracaoSA, asn::CConversorTipoIdentificadorEleitor, asn::CConversorUrna, asn::CConversorVersoesArquivos, asn::CConversorVoto, asn::CConversorVotosCargo, asn::CIndexadorFotos, asn::CVisitanteEleitor, asn::CVisitanteFoto, asn::IConversorASN<ModuloBoletimUrna::EntidadeBoletimUrna, comum::md::CEntidadeBU>, asn::IConversorASN<ModuloBoletimUrna::HistoricoVotoImpresso, comum::md::CHistoricoVotoImpresso>, asn::IConversorASN<ModuloCandidatos::Candidatura, comum::md::CCandidatura>, asn::IConversorASN<ModuloCandidatos::DadosCandidato, comum::md::CDadosCandidato>, asn::IConversorASN<ModuloCandidatos::EntidadeCandidatos, std::__2::vector<comum::md::CCandidatura, std::__2::allocator<comum::md::CCandidatura> > >, asn::IConversorASN<ModuloComplementosMunicipios::ComplementoMunicipio, comum::md::CComplementoMunicipio>, asn::IConversorASN<ModuloComplementosMunicipios::EntidadeComplementosMunicipios, comum::md::CComplementosMunicipiosUF>, asn::IConversorASN<ModuloComplementosMunicipios::HorarioVerao, comum::md::CHorarioVerao>, asn::IConversorASN<ModuloDadosDisponiveisCarga::DadosDisponiveisCarga, comum::md::CDadosDisponiveisCarga>, asn::IConversorASN<ModuloEleicao::CargoPergunta, comum::md::CCargo>, asn::IConversorASN<ModuloEleicao::DetalheCargo, comum::md::CDetalheCandidato>, asn::IConversorASN<ModuloEleicao::DetalhePergunta, comum::md::CDetalheConsulta>, asn::IConversorASN<ModuloEleicao::EntidadeEleicao, comum::md::CEleicaoPE>, asn::IConversorASN<ModuloEleicao::NomesCargo, comum::md::CNomesCargo>, asn::IConversorASN<ModuloEleicao::Suplencias, comum::md::CSuplencia>, asn::IConversorASN<ModuloEleitores::BiometriaEleitor, comum::md::CBiometriaEleitor>, asn::IConversorASN<ModuloEleitores::Dedo, comum::md::CDedo>, asn::IConversorASN<ModuloEnvelopeGenerico::EntidadeEnvelopeGenerico, comum::md::CEnvelopeGenerico>, asn::IConversorASN<ModuloEstadoGeralDefs::NumViasImpressasRelatorios, comum::md::estadoaplicacao::CNumViasImpressasRelatorios>, asn::IConversorASN<ModuloEstadoGeralGap::EstadoGeralGap, comum::md::estadoaplicacao::CEstadoGeralGap>, asn::IConversorASN<ModuloEstadoGeralSA::EstadoGeralSA, comum::md::estadoaplicacao::CEstadoGeralSA>, asn::IConversorASN<ModuloEstadoGeralUrna::AjusteDataHora, comum::md::estadoaplicacao::CAjusteDataHora>, asn::IConversorASN<ModuloEstadoGeralUrna::DadoCarga, comum::md::estadoaplicacao::CDadoCarga>, asn::IConversorASN<ModuloEstadoGeralUrna::DadoCorrespondencia, comum::md::estadoaplicacao::CDadoCorrespondencia>, asn::IConversorASN<ModuloEstadoGeralUrna::DadoLocal, comum::md::estadoaplicacao::CDadoLocal>, asn::IConversorASN<ModuloEstadoGeralUrna::DadoSecao, comum::md::estadoaplicacao::CLocalidadeEleitoral>, asn::IConversorASN<ModuloEstadoGeralUrna::EstadoGeralUrna, comum::md::estadoaplicacao::CEstadoGeral>, asn::IConversorASN<ModuloEstadoGeralVota::EstadoGeralVota, comum::md::estadoaplicacao::CEstadoGeralVota>, asn::IConversorASN<ModuloFotosCandidatos::FotoCandidato, comum::md::CFotoCandidato>, asn::IConversorASN<ModuloHashes::EntidadeHashes, comum::md::CEntidadeHashes>, asn::IConversorASN<ModuloImpedidos::EntidadeImpedidos, std::__2::vector<comum::md::CImpedido, std::__2::allocator<comum::md::CImpedido> > >, asn::IConversorASN<ModuloLocal::IdentificacaoAgregada, comum::md::CIdentificacaoAgregada>, asn::IConversorASN<ModuloLocal::Local, comum::md::CLocal>, asn::IConversorASN<ModuloLocal::SecaoEleitoral, comum::md::CSecaoEleitoral>, asn::IConversorASN<ModuloPartidos::EntidadePartidos, comum::md::CEntidadePartidos>, asn::IConversorASN<ModuloPartidos::Partido, comum::md::CPartido>, asn::IConversorASN<ModuloProcessoEleitoral::EntidadeProcessoEleitoral, comum::md::CProcessoEleitoralDTO>, asn::IConversorASN<ModuloProcessoEleitoral::OrigemConfiguracao, comum::md::EOrigemConfiguracao>, asn::IConversorASN<ModuloProcessoEleitoral::Pleito, comum::md::CPleitoDTO>, asn::IConversorASN<ModuloRegistroDigitalVoto::Eleicoes, comum::md::CVotosEleicoesVota>, asn::IConversorASN<ModuloRegistroDigitalVoto::EntidadeRegistroDigitalVoto, comum::md::CVotosEleicoesVota>, asn::IConversorASN<ModuloRegistroDigitalVoto::Voto, comum::md::CVoto>, asn::IConversorASN<ModuloRegistroDigitalVoto::VotosCargo, std::__2::pair<comum::md::CVotosCargos::SCargoInfo, comum::md::CVotos> >, asn::IConversorASN<ModuloSituacoesEleicoes::EntidadeSituacoesEleicoes, std::__2::vector<comum::md::CSituacoesEleicoes, std::__2::allocator<comum::md::CSituacoesEleicoes> > >, asn::IConversorASN<ModuloTiposCadastro::Municipio, comum::md::CMunicipio>, asn::IConversorASN<ModuloTiposEcoUrna::Carga, comum::md::CCarga>, asn::IConversorASN<ModuloTiposEleitorais::Abrangencia, comum::md::CAbrangencia>, asn::IConversorASN<ModuloTiposEleitorais::ArquivoAssinatura, api::hash::CHashArquivo>, asn::IConversorASN<ModuloTiposEleitorais::CabecalhoEntidade, comum::md::CCabecalhoEntidade>, asn::IConversorASN<ModuloTiposEleitorais::CabecalhoPacote, comum::md::CCabecalhoPacote>, asn::IConversorASN<ModuloTiposEleitorais::CodigoCargoConsulta, unsigned char>, asn::IConversorASN<ModuloTiposEleitorais::Seguranca, comum::md::CSeguranca>, asn::IConversorASN<ModuloTiposEleitorais::TipoIdentificadorEleitor, ecourna::app::dados::ETipoIdentificadorEleitor>, asn::IConversorASN<ModuloTiposResultadosEcoUrna::CorrespondenciaResultado, comum::md::CCorrespondenciaResultado>, asn::IConversorASN<ModuloTiposResultadosEcoUrna::TipoApuracaoSA, comum::md::CTipoApuracaoSA>, asn::IConversorASN<ModuloTiposResultadosEcoUrna::Urna, comum::md::CUrna>, asn::IConversorASN<ModuloVersaoArquivos::EntidadeVersaoArquivos, comum::md::CVersoesArquivos>, asn::IConversorBiometriaASN<ModuloEleitores::BiometriaEleitorCifrada, comum::md::CBiometriaEleitor>, asn::IConversorParcialASN<ModuloEleitores::EntidadeEleitores, comum::md::CEntidadeEleitores, comum::asn::CVisitanteEleitor>, asn::IConversorParcialASN<ModuloFotosCandidatos::EntidadeFotosCandidatos, comum::md::CIndicesFotos, comum::asn::CVisitanteFoto>, dao::CComparecimentoMesarioDAO, dao::CEleitorDinamicoDAO, dao::CJustificadorDAO, dao::IComparecimentoMesarioDAO, dao::IEleitorDinamicoDAO, dao::IJustificadorDAO, impl::CValidaMidia, impl::IValidaMidia, md::CRdvPosicionador, md::CRegraCPF, md::CRegraIdentidadeLivre, md::CRegraTitulo, md::CVotos::Candidato(std::__2::basic_string<char, std::__2::char_traits<char>, std::__2::allocator<char> > const&) const::$_0, md::CVotos::Legenda(std::__2::basic_string<char, std::__2::char_traits<char>, std::__2::allocator<char> > const&) const::$_0, md::CVotos::Partido(std::__2::basic_string<char, std::__2::char_traits<char>, std::__2::allocator<char> > const&) const::$_0, md::CVotos::TotalQue(bool (comum::md::CVoto::*)() const) const::$_0, md::CVotosEleicoesVota::Brancos(unsigned char) const::$_0, md::CVotosEleicoesVota::Candidato(unsigned char, std::__2::basic_string<char, std::__2::char_traits<char>, std::__2::allocator<char> > const&) const::$_0, md::CVotosEleicoesVota::Cargo(unsigned char) const::$_0, md::CVotosEleicoesVota::Legenda(unsigned char, std::__2::basic_string<char, std::__2::char_traits<char>, std::__2::allocator<char> > const&) const::$_0, md::CVotosEleicoesVota::Legendas(unsigned char) const::$_0, md::CVotosEleicoesVota::Nominais(unsigned char) const::$_0, md::CVotosEleicoesVota::Nulos(unsigned char) const::$_0, md::CVotosEleicoesVota::Partido(unsigned char, std::__2::basic_string<char, std::__2::char_traits<char>, std::__2::allocator<char> > const&) const::$_0, md::IRegraIdentidade, servico::CComparecimentoMesarioServico, servico::CJustificadorServico

### api:: classes
(anonymous namespace)::DoPrintJob(api::CReport const&, std::__2::basic_string<char, std::__2::char_traits<char>, std::__2::allocator<char> > const&, std::__2::basic_string<char, std::__2::char_traits<char>, std::__2::allocator<char> > const&)::$_0, AudioCollector, BatteryIconDataSource<(api::CPowerInformation::IconOrientation)0>, BatteryIconDataSource<(api::CPowerInformation::IconOrientation)0>::BatteryIconDataSource()::{lambda()#1}, BatteryIconDataSource<(api::CPowerInformation::IconOrientation)1>, BatteryIconDataSource<(api::CPowerInformation::IconOrientation)1>::BatteryIconDataSource()::{lambda()#1}, CAjusteDataHora, CApplication::InitApplication(std::__2::basic_string<char, std::__2::char_traits<char>, std::__2::allocator<char> > const&, std::__2::basic_string<char, std::__2::char_traits<char>, std::__2::allocator<char> > const&, std::__2::basic_string<char, std::__2::char_traits<char>, std::__2::allocator<char> > const&, ELogAplicativos)::commaAsDecimalSeparator, CBeepFieldMT, CBuzzFieldMT, CClockFieldMT, CControlValidation, CCutFieldPaper, CDSImageField, CDataImage<comum::CCandidaturasDSFoto>, CDataImage<std::__2::__bind<vota::CTelasVota::CriaTelaVisualizacaoCandidato(comum::md::CCandidatura const&, unsigned long, unsigned long)::$_0&, comum::md::CDadosCandidato const&> >, CDataImage<std::__2::vector<unsigned char, std::__2::allocator<unsigned char> > (*)()>, CDataMap<comum::md::CEleitorIdentidade, comum::CEleitorDetalhe>, CDataText<api::CFormBuilder::AddStatusHeader(unsigned int)::$_0>, CDataText<api::CTextSource>, CDataText<comum::CCandidaturasDSNome>, CDataText<comum::CCandidaturasDSNumero>, CDataText<comum::CCargoDSNomeSexoCandidato>, CDataText<comum::CPadDS<comum::CToUpperDS<comum::CCargoDSNome> > >, CDataText<comum::CRespostasDSNumero>, CDataText<std::__2::basic_string<char, std::__2::char_traits<char>, std::__2::allocator<char> > (*)()>, CDataText<std::__2::basic_string<char, std::__2::char_traits<char>, std::__2::allocator<char> > const& (*)()>, CDataText<std::__2::function<std::__2::basic_string<char, std::__2::char_traits<char>, std::__2::allocator<char> > ()> >, CDataText<vota::(anonymous namespace)::DS_NomeCargoNeutroComEscolha>, CDataText<vota::CEscolheOpcao::COpcaoDS>, CDataTextFmt<api::CTextSource>, CDataTextFmt<comum::(anonymous namespace)::CComparecimentoMesariosDS>, CDataTextFmt<comum::(anonymous namespace)::CComparecimentoMesariosDS>, CDataTextFmt<comum::(anonymous namespace)::CComparecimentoMesariosDS>, CDataTextFmt<std::__2::basic_string<char, std::__2::char_traits<char>, std::__2::allocator<char> > (*)(std::__2::basic_string<char, std::__2::char_traits<char>, std::__2::allocator<char> > const&)>, CDataTextFmt<std::__2::basic_string<char, std::__2::char_traits<char>, std::__2::allocator<char> > const (*)(std::__2::basic_string<char, std::__2::char_traits<char>, std::__2::allocator<char> > const&)>, CDataTextFmt<std::__2::function<std::__2::basic_string<char, std::__2::char_traits<char>, std::__2::allocator<char> > (std::__2::basic_string<char, std::__2::char_traits<char>, std::__2::allocator<char> > const&)> >, CDefaultGenericFactory<api::IRWSyncCtl, api::CPosixRWMutex>, CDefaultGenericFactory<api::ISemaphore, api::CPosixSemaphore>, CDefaultGenericFactory<api::ISyncCtl, api::CPosixMutex>, CDefaultGenericFactory<api::IThreadImpl, simulador::CWasmThread>, CEscritorLog, CEsperaAudio, CFileSeeder, CFillField, CFixedImage, CFixedText, CFormPart, CFormStack<api::IPaper>::CObservable, CFormStack<api::IScreen>::CObservable, CFormStack<api::IScreenMT>::CObservable, CFramedText, CGrayedFramedText, CImageField, CImageFieldUpdate, CImageFieldUpdate::CImageFieldUpdate(api::SPoint const&, std::__2::shared_ptr<api::IImage> const&, std::__2::chrono::duration<long long, std::__2::ratio<1ll, 1000ll> > const&, api::EAnchorPoint)::$_0, CInputField<api::CFramedText>, CInputFieldControl<api::IScreen>, CInputFieldControl<api::IScreenMT>, CInputFieldControlBase<api::IScreen>, CInputFieldControlBase<api::IScreenMT>, CInputFieldMT, CInputMenuField, CInputMenuField::AddItem(std::__2::basic_string<char, std::__2::char_traits<char>, std::__2::allocator<char> > const&, std::__2::function<void (api::CMenuItem&)>)::{default arg#1}::{lambda(api::CMenuItem&)#1}, CInteractiveForm<api::IScreen, api::IInputKbd>, CInteractiveForm<api::IScreenMT, api::IInputMT>, CLedFieldMT, CLineField, CMaskedTextField<api::CFramedText>, CMaskedTextField<api::CGrayedFramedText>, CMenuValidation, CMessageInterface, CMovieField, CMovieField::CMovieField(api::SPoint const&, std::__2::shared_ptr<api::CMovie> const&, api::EAnchorPoint)::$_0, CNewLineFieldPaper, CNumberValidation, COptionValidation, CPaperFormBuilder::Show(std::__2::basic_string<char, std::__2::char_traits<char>, std::__2::allocator<char> > const&, std::__2::basic_string<char, std::__2::char_traits<char>, std::__2::allocator<char> > const&, bool)::$_0, CPosixMutex, CPosixRWMutex, CPosixSemaphore, CPreShowClearMT, CPreShowClearScreen, CPreShowNothing<api::IPaper>, CPreShowNothing<api::IScreen>, CPriorityMessageQueue<api::SMessage>, CProgressBar, CQRCodeImage, CQRCodeImageFieldPaper, CRHVoiceTextToSpeech, CRectField, CState, CStateContext<comum::CAppState>, CStepsProgressBar, CSubReport, CSystem::ZeroFill(std::__2::basic_string<char, std::__2::char_traits<char>, std::__2::allocator<char> > const&)::$_0, CSystemDateTime, CTextBox, CTextField, CTextFieldBlinking, CTextFieldBlinking::CTextFieldBlinking(api::SPoint const&, std::__2::shared_ptr<api::IText> const&, api::SFont const&, api::TColor, api::TColor, api::TColor)::$_0, CTextFieldDoubleLine, CTextFieldMT, CTextFieldMultiLine, CTextFieldPaper, CTextFieldUpdate, CTextFieldUpdate::CTextFieldUpdate(api::SPoint const&, std::__2::shared_ptr<api::IText> const&, std::__2::chrono::duration<long long, std::__2::ratio<1ll, 1000ll> > const&, api::SFont const&, api::TColor, api::TColor)::$_0, CTextFieldUpdateMT, CTextFieldUpdateMT::CTextFieldUpdateMT(api::SPoint const&, std::__2::shared_ptr<api::IText> const&, std::__2::chrono::duration<long long, std::__2::ratio<1ll, 1000ll> > const&)::$_0, CTextRectField, CThread, CTimer, CTimerScheduler, CUeCodedPrinterError, CUeDesligandoError, CUePrinterError, CWavFile, IAjusteDataHora, IBeep, IEsperaAudio, IFingerPrepare, IForm<api::IPaper>, IForm<api::IScreen>, IForm<api::IScreenMT>, IFormField<api::IPaper>, IFormField<api::IScreen>, IFormField<api::IScreenMT>, IFormFieldBase<api::IPaper>, IFormFieldBase<api::IScreen>, IFormFieldBase<api::IScreenMT>, IFormImpressao<api::IPaperRelatorios>, IGenericFactory<api::IRWSyncCtl>, IGenericFactory<api::ISemaphore>, IGenericFactory<api::ISyncCtl>, IGenericFactory<api::IThreadImpl>, IImage, IImageSurfaceOps, IImpressora, IImpressoraRelatorios, IInput, IInputField<api::IScreen>, IInputField<api::IScreen>::IInputField(std::__2::shared_ptr<api::IInputValidation> const&, unsigned long, bool, bool, bool, bool, bool)::{lambda()#1}, IInputField<api::IScreenMT>, IInputField<api::IScreenMT>::IInputField(std::__2::shared_ptr<api::IInputValidation> const&, unsigned long, bool, bool, bool, bool, bool)::{lambda()#1}, IInputKbd, IInputMT, IInputValidation, IObservable<std::__2::shared_ptr<api::IImage> >, IObservable<std::__2::vector<api::FormHandle<api::IPaper>, std::__2::allocator<api::FormHandle<api::IPaper> > > >, IObservable<std::__2::vector<api::FormHandle<api::IScreen>, std::__2::allocator<api::FormHandle<api::IScreen> > > >, IObservable<std::__2::vector<api::FormHandle<api::IScreenMT>, std::__2::allocator<api::FormHandle<api::IScreenMT> > > >, IObserver<std::__2::shared_ptr<api::IImage> >, IPaper, IPaperRelatorios, IPower, IPreShow<api::IPaper>, IPreShow<api::IScreen>, IPreShow<api::IScreenMT>, IRWSyncCtl, IReportPart, IResource, IScreen, IScreenMT, ISemaphore, ISigVerifier, ISound, ISound::Wait()::{lambda()#1}, ISound::WaitAsync(std::__2::function<bool ()>, std::__2::function<void (bool)>)::{lambda()#1}, ISubReport, ISyncCtl, ISystemDateTime, IText, ITextToSpeech, IThreadImpl, ITimer, ITimerScheduler, IUrna, Logger, QRCodeSRC, persistencia::IDAO, persistencia::IUenuxGenericDAO<comum::md::CComparecimentoMesario, comum::md::CComparecimentoMesarioPK>, persistencia::IUenuxGenericDAO<comum::md::CEleitorDinamico, std::__2::basic_string<char, std::__2::char_traits<char>, std::__2::allocator<char> > >, persistencia::IUenuxGenericDAO<ecourna::app::dados::CIdentificacaoJustificativa, std::__2::basic_string<char, std::__2::char_traits<char>, std::__2::allocator<char> > >, teste::CPowerMock, teste::CUrnaMock

### ecourna:: classes
api::asn::IConversorASN<ModuloBoletimUrna::DetalhamentoComparecimento, ecourna::app::dados::CDetalhamentoComparecimento>, api::asn::IConversorASN<ModuloConfiguracaoMunicipios::ConfiguracaoMunicipio, ecourna::app::dados::CConfiguracaoMunicipio>, api::asn::IConversorASN<ModuloConfiguracaoMunicipios::EntidadeConfiguracaoMunicipios, ecourna::app::dados::CConfiguracaoMunicipios>, api::asn::IConversorASN<ModuloConfiguracaoMunicipios::HorariosUrna, ecourna::app::dados::CHorariosUrna>, api::asn::IConversorASN<ModuloFederacoes::EntidadeFederacoes, ecourna::app::dados::CFederacoes>, api::asn::IConversorASN<ModuloFederacoes::Federacao, ecourna::app::dados::CFederacao>, api::asn::IConversorASN<ModuloInformacaoMidia::Aplicativo, ecourna::app::dados::CAplicativo>, api::asn::IConversorASN<ModuloInformacaoMidia::Autenticacao, ecourna::app::dados::CAutenticacao>, api::asn::IConversorASN<ModuloInformacaoMidia::DadosGeracaoMidia, ecourna::app::dados::CDadosGeracaoMidia>, api::asn::IConversorASN<ModuloInformacaoMidia::InformacaoMidia, ecourna::app::dados::CInformacaoMidia>, api::asn::IConversorASN<ModuloParametrizacaoUrna::EntidadeParametrizacaoUrna, ecourna::app::dados::CParametrizacaoUrna>, api::asn::IConversorASN<ModuloParametrizacaoUrna::LabelParametrizado, ecourna::app::dados::CLabelParametrizado>, api::asn::IConversorASN<ModuloParametrizacaoUrna::ParametrosUrna, ecourna::app::dados::CParametrosUrna>, api::asn::IConversorASN<ModuloParametrizacaoUrna::TituloRelatorio, ecourna::app::dados::CTituloRelatorio>, api::asn::IConversorASN<ModuloResultadoUrnaCadastro::ApresentacaoFotoEleitor, ecourna::app::dados::CApresentacaoFotoEleitor>, api::asn::IConversorASN<ModuloResultadoUrnaCadastro::ComparecimentoMesario, ecourna::app::dados::CComparecimentoMesario>, api::asn::IConversorASN<ModuloResultadoUrnaCadastro::ComparecimentoSecao, ecourna::app::dados::CComparecimentoSecao>, api::asn::IConversorASN<ModuloResultadoUrnaCadastro::DadosCifracao, ecourna::app::dados::CDadosCifracao>, api::asn::IConversorASN<ModuloResultadoUrnaCadastro::DadosComparecimento, ecourna::app::dados::CDadosComparecimento>, api::asn::IConversorASN<ModuloResultadoUrnaCadastro::DadosComparecimentoCifrado, ecourna::app::dados::CDadosComparecimentoCifrado>, api::asn::IConversorASN<ModuloResultadoUrnaCadastro::EntidadeResultadoUrnaCadastro, ecourna::app::dados::CResultadoUrnaCadastro>, api::asn::IConversorASN<ModuloResultadoUrnaCadastro::EstadoComparecimento, ecourna::app::dados::CEstadoComparecimento>, api::asn::IConversorASN<ModuloResultadoUrnaCadastro::EstadoHabilitacaoPorCodigo, ecourna::app::dados::CEstadoHabilitacaoPorCodigo>, api::asn::IConversorASN<ModuloResultadoUrnaCadastro::HabilitacaoBiometrica, ecourna::app::dados::CHabilitacaoBiometrica>, api::asn::IConversorASN<ModuloResultadoUrnaCadastro::IdentificacaoJustificativa, ecourna::app::dados::CIdentificacaoJustificativa>, api::asn::IConversorASN<ModuloTiposEcoUrna::IdentificadorGeradorMidia, ecourna::app::dados::CIdentificadorGeradorMidia>, api::asn::IConversorASN<ModuloTiposEcoUrna::RegistroIdentificacaoEleitor, ecourna::app::dados::CRegistroIdentificacaoEleitor>, api::asn::IConversorASN<ModuloTiposEleitorais::CabecalhoEntidade, ecourna::app::dados::CCabecalhoEntidade>, api::asn::IConversorASN<ModuloTiposEleitorais::CodigoCargoConsulta, CBaseType<unsigned short, (unsigned short)1, (unsigned short)99, 3> >, api::asn::IConversorASN<ModuloTiposEleitorais::Fase, ecourna::app::dados::CFaseID>, api::asn::IConversorASN<ModuloTiposEleitorais::Foto, ecourna::app::dados::CFoto>, api::asn::IConversorASN<ModuloTiposEleitorais::IdentificacaoSecaoEleitoral, ecourna::app::dados::CIdentificacaoSecaoEleitoral>, api::asn::IConversorASN<ModuloTiposEleitorais::IdentificadorEleitor, ecourna::app::dados::CIdentificadorEleitor>, api::asn::IConversorASN<ModuloTiposEleitorais::MunicipioZona, ecourna::app::dados::CMunicipioZona>, api::asn::IConversorASN<ModuloTiposEleitorais::Turno, CBaseType<unsigned short, (unsigned short)1, (unsigned short)2, 11> >, api::cepesc::CCepescCipher, api::compression::CLzmaCompress, api::compression::CZip, api::compression::ICompressor, api::exception::CBaseError<api::EUeAssertError, ecourna::api::exception::SErrorLimits{3400, 4200}>, api::exception::CBaseError<api::EUeAudioError, ecourna::api::exception::SErrorLimits{4850, 4900}>, api::exception::CBaseError<api::EUeDesligandoError, ecourna::api::exception::SErrorLimits{4200, 4250}>, api::exception::CBaseError<api::EUeGuiError, ecourna::api::exception::SErrorLimits{4900, 5100}>, api::exception::CBaseError<api::EUeHashError, ecourna::api::exception::SErrorLimits{5100, 5150}>, api::exception::CBaseError<api::EUeHwilError, ecourna::api::exception::SErrorLimits{5150, 5950}>, api::exception::CBaseError<api::EUeIoError, ecourna::api::exception::SErrorLimits{5950, 6150}>, api::exception::CBaseError<api::EUeIpcError, ecourna::api::exception::SErrorLimits{6150, 6350}>, api::exception::CBaseError<api::EUePatternError, ecourna::api::exception::SErrorLimits{6750, 6800}>, api::exception::CBaseError<api::EUePersistenciaError, ecourna::api::exception::SErrorLimits{6800, 6850}>, api::exception::CBaseError<api::EUePrinterError, ecourna::api::exception::SErrorLimits{4450, 4650}>, api::exception::CBaseError<api::EUeRdvError, ecourna::api::exception::SErrorLimits{4650, 4850}>, api::exception::CBaseError<api::EUeUeLogError, ecourna::api::exception::SErrorLimits{6950, 7000}>, api::exception::CBaseError<api::EUeUtilError, ecourna::api::exception::SErrorLimits{7000, 7200}>, api::exception::CBaseError<comum::EUeComumAppInfoError, ecourna::api::exception::SErrorLimits{7600, 7650}>, api::exception::CBaseError<comum::EUeComumAsnError, ecourna::api::exception::SErrorLimits{7650, 7750}>, api::exception::CBaseError<comum::EUeComumComparecimentoMesarioError, ecourna::api::exception::SErrorLimits{7750, 7800}>, api::exception::CBaseError<comum::EUeComumDadosError, ecourna::api::exception::SErrorLimits{7800, 8600}>, api::exception::CBaseError<comum::EUeComumError, ecourna::api::exception::SErrorLimits{7200, 7600}>, api::exception::CBaseError<comum::EUeComumGravadoresError, ecourna::api::exception::SErrorLimits{8600, 8800}>, api::exception::CBaseError<comum::EUeComumJustificativaError, ecourna::api::exception::SErrorLimits{8800, 8850}>, api::exception::CBaseError<comum::EUeComumLogError, ecourna::api::exception::SErrorLimits{8850, 8900}>, api::exception::CBaseError<comum::EUeComumNomeArquivoError, ecourna::api::exception::SErrorLimits{8950, 9000}>, api::exception::CBaseError<comum::EUeComumReconhecimentoBiometricoError, ecourna::api::exception::SErrorLimits{9000, 9050}>, api::exception::CBaseError<comum::EUeComumRelatoriosError, ecourna::api::exception::SErrorLimits{9050, 9150}>, api::exception::CBaseError<comum::EUeComumUtilError, ecourna::api::exception::SErrorLimits{9150, 9200}>, api::exception::CBaseError<comum::EUeComumValidaMidiaError, ecourna::api::exception::SErrorLimits{9200, 9250}>, api::exception::CBaseError<ecourna::api::asn::EApiAsnError, ecourna::api::exception::SErrorLimits{1900, 1910}>, api::exception::CBaseError<ecourna::api::compression::ECompressionError, ecourna::api::exception::SErrorLimits{1000, 1100}>, api::exception::CBaseError<ecourna::api::io::EIoError, ecourna::api::exception::SErrorLimits{1175, 1275}>, api::exception::CBaseError<ecourna::api::pattern::EPatternError, ecourna::api::exception::SErrorLimits{1300, 1325}>, api::exception::CBaseError<ecourna::api::security::ESecurityError, ecourna::api::exception::SErrorLimits{1325, 1725}>, api::exception::CBaseError<ecourna::api::sql::ESqlError, ecourna::api::exception::SErrorLimits{1725, 1775}>, api::exception::CBaseError<ecourna::api::util::EUtilError, ecourna::api::exception::SErrorLimits{1875, 1900}>, api::exception::CBaseError<ecourna::app::dados::EDadosError, ecourna::api::exception::SErrorLimits{1935, 2135}>, api::exception::CBaseError<ecourna::app::dados::EDadosFederacoesError, ecourna::api::exception::SErrorLimits{2940, 2965}>, api::exception::CBaseError<ecourna::app::dados::EDadosMidiasError, ecourna::api::exception::SErrorLimits{3040, 3065}>, api::exception::CBaseError<ecourna::app::dados::EDadosProcessoEleitoralError, ecourna::api::exception::SErrorLimits{3140, 3190}>, api::exception::CBaseError<ecourna::app::dados::EDadosResultadoUrnaCadastroError, ecourna::api::exception::SErrorLimits{3265, 3315}>, api::exception::CBaseError<ecourna::app::dados::asn::EAsnError, ecourna::api::exception::SErrorLimits{2235, 2335}>, api::exception::CBaseError<ecourna::app::dados::asn::EAsnMidiasError, ecourna::api::exception::SErrorLimits{2485, 2510}>, api::exception::CBaseError<ecourna::app::dados::asn::EAsnParametrizacaoUrnaError, ecourna::api::exception::SErrorLimits{2560, 2585}>, api::exception::CBaseError<ecourna::app::dados::asn::EAsnResultadoUrnaCadastroError, ecourna::api::exception::SErrorLimits{2635, 2665}>, api::exception::CBaseError<vota::EUeVotaError, ecourna::api::exception::SErrorLimits{9300, 9500}>, api::exception::CError, api::io::CIoError, api::pattern::IObservableProgressWithDescription, api::pattern::NonCopyable, api::security::CBlockCipher<ecourna::api::security::CAesCipher, ecourna::api::security::CTrng>, api::security::CHKDFSeed, api::security::CPrng, api::security::CSha, api::security::CSha512, api::security::CSiphashMac, api::security::CSymmetricCipherFactory, api::security::CTrng, api::security::IAsymmetricCipher<ecourna::api::cepesc::CPlainText, ecourna::api::cepesc::CCipheredOut, ecourna::api::cepesc::CCipheredIn, std::__2::vector<unsigned char, std::__2::allocator<unsigned char> > >, api::security::IHash, api::security::IMacAlgorithm, api::security::IRng, api::security::ISymmetricCipher, api::security::ISymmetricCipherFactory, api::sql::ISqlConnection, api::sql::ISqlResultSet, api::sql::ISqlStatement, api::sql::sqlite::CSqlConnection, api::sql::sqlite::CSqlResultSet, api::sql::sqlite::CSqlStatement, api::util::CBitArray, app::dados::CNumeroCPF, app::dados::CNumeroIdentificacaoLivre, app::dados::CNumeroInscricaoEleitoral, app::dados::IIdentificadorEleitor, app::dados::asn::CConversorAplicativo, app::dados::asn::CConversorApresentacaoFotoEleitor, app::dados::asn::CConversorAutenticacao, app::dados::asn::CConversorCabecalhoEntidade, app::dados::asn::CConversorCargoID, app::dados::asn::CConversorComparecimentoMesario, app::dados::asn::CConversorComparecimentoSecao, app::dados::asn::CConversorConfiguracaoMunicipio, app::dados::asn::CConversorConfiguracaoMunicipios, app::dados::asn::CConversorDadosCifracao, app::dados::asn::CConversorDadosComparecimento, app::dados::asn::CConversorDadosComparecimentoCifrado, app::dados::asn::CConversorDadosGeracaoMidia, app::dados::asn::CConversorDetalhamentoComparecimento, app::dados::asn::CConversorEstadoComparecimento, app::dados::asn::CConversorEstadoHabilitacaoPorCodigo, app::dados::asn::CConversorFase, app::dados::asn::CConversorFederacao, app::dados::asn::CConversorFederacoes, app::dados::asn::CConversorFoto, app::dados::asn::CConversorHabilitacaoBiometrica, app::dados::asn::CConversorHorariosUrna, app::dados::asn::CConversorIdentificacaoJustificativa, app::dados::asn::CConversorIdentificacaoSecaoEleitoral, app::dados::asn::CConversorIdentificadorEleitor, app::dados::asn::CConversorIdentificadorGeradorMidia, app::dados::asn::CConversorInformacaoMidia, app::dados::asn::CConversorLabelParametrizado, app::dados::asn::CConversorMunicipioZona, app::dados::asn::CConversorParametrizacaoUrna, app::dados::asn::CConversorParametrosUrna, app::dados::asn::CConversorRegistroIdentificacaoEleitor, app::dados::asn::CConversorResultadoUrnaCadastro, app::dados::asn::CConversorTituloRelatorio, app::dados::asn::CConversorTurno

### simulador:: classes
(anonymous namespace)::CEsperaAudioWasm, CFingerPrepareSimulador, CWasmBeep, CWasmImageSurfaceOps, CWasmInit, CWasmInputKbd, CWasmInputMT, CWasmLogd, CWasmNullPaper, CWasmNullPrinter, CWasmNullSound, CWasmNullTextToSpeech, CWasmResource, CWasmScreen, CWasmScreenMT, CWasmScreenMT::ShowClock(api::SPoint const&)::WasmText, CWasmSystemDateTime, CWasmThread, CWasmTimer, CWasmTimerScheduler, CWasmWebSound
