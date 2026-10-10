// Roots: keep the app/dados/asn/midias converters in the skeleton link the way the target keeps them. Their
// users (comum/vota code that reads and writes the media descriptor) are not reconstructed yet; in the target each
// converter's vtable is data whose DoConverte/DoDeconverte slots put the bodies in the table, so rooting the vtables
// keeps the same set (CConversorInformacaoMidia 1126676, CConversorAplicativo 1125708, CConversorAutenticacao 1125412,
// CConversorDadosGeracaoMidia 1125148, CConversorIdentificadorGeradorMidia 1123132). Declared through asm labels, so
// this TU needs no ecourna or ASN.1 header and instantiates nothing.
extern "C" {
extern const char midias_vt_informacao[] __asm__("_ZTVN7ecourna3app5dados3asn25CConversorInformacaoMidiaE");
extern const char midias_vt_aplicativo[] __asm__("_ZTVN7ecourna3app5dados3asn20CConversorAplicativoE");
extern const char midias_vt_autenticacao[] __asm__("_ZTVN7ecourna3app5dados3asn22CConversorAutenticacaoE");
extern const char midias_vt_dadosgeracao[] __asm__("_ZTVN7ecourna3app5dados3asn27CConversorDadosGeracaoMidiaE");
extern const char midias_vt_gerador[] __asm__("_ZTVN7ecourna3app5dados3asn35CConversorIdentificadorGeradorMidiaE");
void midias_bytes_hex(int, int) __asm__("_ZN7ecourna3api4util12CStringUtils16BytesToHexStringERKNSt3__26vectorIhNS3_9allocatorIhEEEE");
int midias_gerador_dtor(int) __asm__("_ZN7ecourna3app5dados26CIdentificadorGeradorMidiaD2Ev");
int midias_fase_deconverte(int, int) __asm__("_ZNK7ecourna3api3asn13IConversorASNIN21ModuloTiposEleitorais4FaseENS_3app5dados7CFaseIDEE10DeconverteERKS4_");
}

namespace {

const void* volatile midias_sink;

// IConversorASN<Fase, CFaseID>::Deconverte (fn 5103) has a second direct caller in the target (fn 9052, a
// resultadournacadastro converter); with one caller binaryen would inline it into fn 9179.
__attribute__((noinline)) int UsaFaseDeconverte(int conversor, int entidade) { return midias_fase_deconverte(conversor + 4, entidade) * 3; }

// CStringUtils::BytesToHexString (fn 1243) has eight callers in the target; CSerialMidia::ToString (fn 9257) is the
// only one linked so far, and binaryen would inline it there.
__attribute__((noinline)) void UsaBytesToHex(int texto, int bytes) { midias_bytes_hex(texto + 8, bytes - 4); }

// ~CIdentificadorGeradorMidia (fn 2240 in the target, shared by four callers) stays a call in the landing pad of
// CConversorDadosGeracaoMidia::DoDeconverte (fn 9202); with one caller binaryen would inline it there. The result is
// dropped, as by every target caller, so binaryen's DAE removes the this-return as in the target.
__attribute__((noinline)) void UsaGeradorDtor(int identificador) { midias_gerador_dtor(identificador + 36); }

struct SMidiasRoots {
    SMidiasRoots()
    {
        midias_sink = midias_vt_informacao;
        midias_sink = midias_vt_aplicativo;
        midias_sink = midias_vt_autenticacao;
        midias_sink = midias_vt_dadosgeracao;
        midias_sink = midias_vt_gerador;
        midias_sink = reinterpret_cast<const void*>(&UsaFaseDeconverte);
        midias_sink = reinterpret_cast<const void*>(&UsaBytesToHex);
        midias_sink = reinterpret_cast<const void*>(&UsaGeradorDtor);
    }
} midias_roots;

} // namespace
