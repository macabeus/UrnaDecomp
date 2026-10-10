// Roots: keep the app/dados/resultadournacadastro functions in the skeleton link the way the target keeps them
// (their callers - the dados/asn/resultadournacadastro converters and comum::CGravadorRCSecao (fn 11616) - are not
// reconstructed yet). Every constructor and throwing getter is reached through invoke_* in the target (a table slot),
// so it is address-taken here; CHabilitacaoBiometrica's 4-argument constructor (fn 5087) has no slot and TWO direct
// callers (9066, 11616), so two noinline callers below keep binaryen from inlining it. Declared through asm labels with
// the wasm-level signatures, so this TU instantiates nothing.
extern "C" {
int rrucdata_5091(int, int, int, int) __asm__("_ZN7ecourna3app5dados14CDadosCifracaoC2ENSt3__26vectorIhNS3_9allocatorIhEEEES7_S7_");
int rrucdata_3484(int, int, int) __asm__("_ZN7ecourna3app5dados27CDadosComparecimentoCifradoC2ENS1_14CDadosCifracaoENSt3__26vectorIhNS4_9allocatorIhEEEE");
int rrucdata_3485(int, int, int, int, int) __asm__("_ZN7ecourna3app5dados20CDadosComparecimentoC2ENSt3__26vectorINS1_27CIdentificacaoJustificativaENS3_9allocatorIS5_EEEENS1_20CComparecimentoSecaoENS3_8optionalINS4_INS1_22CComparecimentoMesarioENS6_ISB_EEEEEESE_");
int rrucdata_9024(int) __asm__("_ZNK7ecourna3app5dados20CDadosComparecimento19GetMesariosAberturaEv");
int rrucdata_9023(int) __asm__("_ZNK7ecourna3app5dados20CDadosComparecimento23GetMesariosEncerramentoEv");
int rrucdata_2658(int, int, int) __asm__("_ZN7ecourna3app5dados21CEstadoComparecimentoC2ERKNS1_29CRegistroIdentificacaoEleitorENS2_23ESituacaoComparecimentoE");
int rrucdata_2193(int, int, int, int, int) __asm__("_ZN7ecourna3app5dados21CEstadoComparecimentoC2ERKNS1_29CRegistroIdentificacaoEleitorENS2_23ESituacaoComparecimentoERKNS1_24CApresentacaoFotoEleitorENS2_25ESituacaoHabilitacaoAudioE");
int rrucdata_2192(int, int, int, int, int, int) __asm__("_ZN7ecourna3app5dados21CEstadoComparecimentoC2ERKNS1_29CRegistroIdentificacaoEleitorENS2_23ESituacaoComparecimentoERKNS1_24CApresentacaoFotoEleitorENS2_25ESituacaoHabilitacaoAudioERKNS1_22CHabilitacaoBiometricaE");
int rrucdata_9021(int) __asm__("_ZNK7ecourna3app5dados21CEstadoComparecimento19GetApresentacaoFotoEv");
int rrucdata_9020(int) __asm__("_ZNK7ecourna3app5dados21CEstadoComparecimento27GetSituacaoHabilitacaoAudioEv");
int rrucdata_9019(int) __asm__("_ZNK7ecourna3app5dados21CEstadoComparecimento24GetHabilitacaoBiometricaEv");
int rrucdata_5090(int, int) __asm__("_ZN7ecourna3app5dados27CEstadoHabilitacaoPorCodigoC2ENS2_30ESituacaoReconhecimentoMesarioE");
int rrucdata_5088(int, int, int) __asm__("_ZN7ecourna3app5dados27CEstadoHabilitacaoPorCodigoC2ENS2_30ESituacaoReconhecimentoMesarioERKNS1_29CRegistroIdentificacaoEleitorE");
int rrucdata_9017(int) __asm__("_ZNK7ecourna3app5dados27CEstadoHabilitacaoPorCodigo23GetIdentificacaoMesarioEv");
int rrucdata_5087(int, int, int, int, int) __asm__("_ZN7ecourna3app5dados22CHabilitacaoBiometricaC2Ej9CBaseTypeItLt0ELt999ELi38EENS1_5CDedo9ETipoDedoES3_INS1_21EErroLeituraBiometriaELS7_0ELS7_13ELi34EE");
int rrucdata_2657(int, int, int, int, int, int) __asm__("_ZN7ecourna3app5dados22CHabilitacaoBiometricaC2Ej9CBaseTypeItLt0ELt999ELi38EENS1_5CDedo9ETipoDedoES3_INS1_21EErroLeituraBiometriaELS7_0ELS7_13ELi34EERKNS1_27CEstadoHabilitacaoPorCodigoE");
int rrucdata_9016(int) __asm__("_ZNK7ecourna3app5dados22CHabilitacaoBiometrica29GetEstadoHabilitacaoPorCodigoEv");
int rrucdata_3483(int, int, int, int, int, int) __asm__("_ZN7ecourna3app5dados22CResultadoUrnaCadastroC2ENS1_18CCabecalhoEntidadeENS1_7CFaseIDENSt3__212basic_stringIcNS5_11char_traitsIcEENS5_9allocatorIcEEEENS2_16ESituacaoArquivoENS5_8optionalINS1_20CDadosComparecimentoEEE");
int rrucdata_3482(int, int, int, int, int, int) __asm__("_ZN7ecourna3app5dados22CResultadoUrnaCadastroC2ENS1_18CCabecalhoEntidadeENS1_7CFaseIDENSt3__212basic_stringIcNS5_11char_traitsIcEENS5_9allocatorIcEEEENS2_16ESituacaoArquivoENS5_8optionalINS1_27CDadosComparecimentoCifradoEEE");
int rrucdata_9015(int) __asm__("_ZNK7ecourna3app5dados22CResultadoUrnaCadastro22GetDadosComparecimentoEv");
int rrucdata_9014(int) __asm__("_ZNK7ecourna3app5dados22CResultadoUrnaCadastro29GetDadosComparecimentoCifradoEv");
int rrucdata_3486(int, int, int, int) __asm__("_ZN7ecourna3app5dados22CComparecimentoMesarioC2ERKNS1_29CRegistroIdentificacaoEleitorERKN5boost10posix_time5ptimeENS2_20EEstadoColetaDigitalE");
}

namespace {

void* volatile rrucdata_sink;

__attribute__((noinline)) int UsaHabilitacaoBiometrica1(int obj, int a) { return rrucdata_5087(obj, a, a + 1, a + 2, a + 3) + 1; }
__attribute__((noinline)) int UsaHabilitacaoBiometrica2(int obj, int a) { return rrucdata_5087(obj + 48, a + 4, a, a + 5, a + 6) * 3; }

int UsaHabilitacaoBiometrica(int a)
{
    return UsaHabilitacaoBiometrica1(a, a + 2) + UsaHabilitacaoBiometrica2(a + 8, a);
}

struct SRrucdataRoots {
    SRrucdataRoots()
    {
        rrucdata_sink = reinterpret_cast<void*>(&rrucdata_5091);
        rrucdata_sink = reinterpret_cast<void*>(&rrucdata_3484);
        rrucdata_sink = reinterpret_cast<void*>(&rrucdata_3485);
        rrucdata_sink = reinterpret_cast<void*>(&rrucdata_9024);
        rrucdata_sink = reinterpret_cast<void*>(&rrucdata_9023);
        rrucdata_sink = reinterpret_cast<void*>(&rrucdata_2658);
        rrucdata_sink = reinterpret_cast<void*>(&rrucdata_2193);
        rrucdata_sink = reinterpret_cast<void*>(&rrucdata_2192);
        rrucdata_sink = reinterpret_cast<void*>(&rrucdata_9021);
        rrucdata_sink = reinterpret_cast<void*>(&rrucdata_9020);
        rrucdata_sink = reinterpret_cast<void*>(&rrucdata_9019);
        rrucdata_sink = reinterpret_cast<void*>(&rrucdata_5090);
        rrucdata_sink = reinterpret_cast<void*>(&rrucdata_5088);
        rrucdata_sink = reinterpret_cast<void*>(&rrucdata_9017);
        rrucdata_sink = reinterpret_cast<void*>(&rrucdata_2657);
        rrucdata_sink = reinterpret_cast<void*>(&rrucdata_9016);
        rrucdata_sink = reinterpret_cast<void*>(&rrucdata_3483);
        rrucdata_sink = reinterpret_cast<void*>(&rrucdata_3482);
        rrucdata_sink = reinterpret_cast<void*>(&rrucdata_9015);
        rrucdata_sink = reinterpret_cast<void*>(&rrucdata_9014);
        rrucdata_sink = reinterpret_cast<void*>(&rrucdata_3486);
        rrucdata_sink = reinterpret_cast<void*>(&UsaHabilitacaoBiometrica);
    }
} rrucdata_roots;

} // namespace
