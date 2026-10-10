// Roots: keep the app/dados/asn/resultadournacadastro converters (and CConversorRegistroIdentificacaoEleitor)
// in the skeleton link the way the target keeps them: their users (CConversorComparecimentoSecao,
// CConversorDadosComparecimento and the comum/vota code that builds a CResultadoUrnaCadastro) are not reconstructed
// yet. In the target each converter's vtable is data whose DoConverte/DoDeconverte slots put the bodies in the table:
// rooting the vtables keeps the same set. Declared through asm labels, so this TU needs no ecourna or ASN.1 header and
// instantiates nothing.
extern "C" {
extern const char rrucconv_vt_0[] __asm__("_ZTVN7ecourna3app5dados3asn33CConversorApresentacaoFotoEleitorE");
extern const char rrucconv_vt_1[] __asm__("_ZTVN7ecourna3app5dados3asn31CConversorComparecimentoMesarioE");
extern const char rrucconv_vt_2[] __asm__("_ZTVN7ecourna3app5dados3asn30CConversorEstadoComparecimentoE");
extern const char rrucconv_vt_3[] __asm__("_ZTVN7ecourna3app5dados3asn36CConversorEstadoHabilitacaoPorCodigoE");
extern const char rrucconv_vt_4[] __asm__("_ZTVN7ecourna3app5dados3asn31CConversorHabilitacaoBiometricaE");
extern const char rrucconv_vt_5[] __asm__("_ZTVN7ecourna3app5dados3asn38CConversorRegistroIdentificacaoEleitorE");
int rrucconv_apresentacao_ctor(int, int, int) __asm__("_ZN7ecourna3app5dados24CApresentacaoFotoEleitorC1ENS2_7EEstadoENS2_10EResultadoE");
int rrucconv_score_ctor(int, int) __asm__("_ZN9CBaseTypeItLt0ELt999ELi38EEC2Et");
}

namespace {

const void* volatile rrucconv_sink;

// CApresentacaoFotoEleitor's constructor: binaryen folds it into fn 1081 with other two-word constructors (8 callers);
// alone it has one caller (9087) and would be inlined.
__attribute__((noinline)) int UsaApresentacao(int obj, int estado) { return rrucconv_apresentacao_ctor(obj, estado, estado + 2) + 7; }
// CBaseType<unsigned short, 0, 999, 38>'s constructor (TScoreHabilitacao, fn 2850) has two callers in the target
// (9066 and comum's 11616): a second noinline caller keeps binaryen from inlining it into 9066.
__attribute__((noinline)) int UsaScore(int obj, int valor) { return rrucconv_score_ctor(obj, valor + 3) * 5; }

struct SRucConvRoots {
    SRucConvRoots()
    {
        rrucconv_sink = reinterpret_cast<const void*>(&UsaScore);
        rrucconv_sink = reinterpret_cast<const void*>(&UsaApresentacao);
        rrucconv_sink = rrucconv_vt_0;
        rrucconv_sink = rrucconv_vt_1;
        rrucconv_sink = rrucconv_vt_2;
        rrucconv_sink = rrucconv_vt_3;
        rrucconv_sink = rrucconv_vt_4;
        rrucconv_sink = rrucconv_vt_5;
    }
} rrucconv_roots;

} // namespace
