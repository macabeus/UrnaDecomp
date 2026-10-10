// Roots: keep the core app/dados/asn converters in the skeleton link the way the target keeps them (their
// users - the bigger converters and the comum/vota code that builds them - are not reconstructed yet). In the target
// each converter's vtable is data (CConversorTurno 1126336, CConversorFase 1126128, ...) whose DoConverte/DoDeconverte
// slots put the bodies in the table: rooting the vtables keeps the same set. Library-ish helpers the target calls from
// two places keep a second noinline caller here (one caller would let binaryen inline them): CFoto's ctor (fn 5111:
// 3616 + 9226), CCabecalhoEntidade's ctor (5112: 11616 + 9218), the date parser (1877: 9218 + 9132) and
// CBaseType<unsigned short, 1, 99, 3>'s ctor (5682: 9228 + another). CConversorParametrosUrna has no vtable in the
// target: CConversorParametrizacaoUrna (9173/9171) reaches its DoConverte/DoDeconverte (9164/9156) through invoke_*
// (table slots), so they are address-taken here. Declared through asm labels, so this TU needs
// no ecourna or ASN.1 header and instantiates nothing.
extern "C" {
extern const char asncore_vt_turno[] __asm__("_ZTVN7ecourna3app5dados3asn15CConversorTurnoE");
extern const char asncore_vt_fase[] __asm__("_ZTVN7ecourna3app5dados3asn14CConversorFaseE");
extern const char asncore_vt_cargo[] __asm__("_ZTVN7ecourna3app5dados3asn17CConversorCargoIDE");
extern const char asncore_vt_foto[] __asm__("_ZTVN7ecourna3app5dados3asn14CConversorFotoE");
extern const char asncore_vt_cabecalho[] __asm__("_ZTVN7ecourna3app5dados3asn27CConversorCabecalhoEntidadeE");
extern const char asncore_vt_identificador[] __asm__("_ZTVN7ecourna3app5dados3asn30CConversorIdentificadorEleitorE");
extern const char asncore_vt_label[] __asm__("_ZTVN7ecourna3app5dados3asn28CConversorLabelParametrizadoE");
extern const char asncore_vt_titulo[] __asm__("_ZTVN7ecourna3app5dados3asn25CConversorTituloRelatorioE");
int asncore_foto_ctor(int, int, int) __asm__("_ZN7ecourna3app5dados5CFotoC1ENS2_14EFormatoImagemERKNSt3__26vectorIhNS4_9allocatorIhEEEE");
int asncore_cabecalho_ctor(int, int, int, int) __asm__("_ZN7ecourna3app5dados18CCabecalhoEntidadeC1ERKN5boost10posix_time5ptimeEjNS2_16ETipoIDEleitoralE");
long long asncore_desconverte_data(int) __asm__("_ZN7ecourna3app5dados3asn19DesconverteDataHoraERKNSt3__212basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEE");
int asncore_cargo_ctor(int, int) __asm__("_ZN9CBaseTypeItLt1ELt99ELi3EEC2Et");
void asncore_parametros_converte(int, int, int) __asm__("_ZNK7ecourna3app5dados3asn24CConversorParametrosUrna10DoConverteERKNS1_15CParametrosUrnaE");
void asncore_parametros_deconverte(int, int, int) __asm__("_ZNK7ecourna3app5dados3asn24CConversorParametrosUrna12DoDeconverteERKN24ModuloParametrizacaoUrna14ParametrosUrnaE");
}

namespace {

const void* volatile asncore_sink;

__attribute__((noinline)) int UsaFoto(int obj, int imagem) { return asncore_foto_ctor(obj, 2, imagem) + 1; }
__attribute__((noinline)) int UsaCabecalho(int obj, int data) { return asncore_cabecalho_ctor(obj, data, 7, 1) * 3; }
__attribute__((noinline)) long long UsaData(int texto) { return asncore_desconverte_data(texto + 4) + 5; }
__attribute__((noinline)) int UsaCargo(int obj, int valor) { return asncore_cargo_ctor(obj, valor + 9) - 2; }

struct SAsnCoreRoots {
    SAsnCoreRoots()
    {
        asncore_sink = asncore_vt_turno;
        asncore_sink = asncore_vt_fase;
        asncore_sink = asncore_vt_cargo;
        asncore_sink = asncore_vt_foto;
        asncore_sink = asncore_vt_cabecalho;
        asncore_sink = asncore_vt_identificador;
        asncore_sink = asncore_vt_label;
        asncore_sink = asncore_vt_titulo;
        asncore_sink = reinterpret_cast<const void*>(&UsaFoto);
        asncore_sink = reinterpret_cast<const void*>(&UsaCabecalho);
        asncore_sink = reinterpret_cast<const void*>(&UsaData);
        asncore_sink = reinterpret_cast<const void*>(&UsaCargo);
        asncore_sink = reinterpret_cast<const void*>(&asncore_parametros_converte);
        asncore_sink = reinterpret_cast<const void*>(&asncore_parametros_deconverte);
    }
} asncore_roots;

} // namespace
