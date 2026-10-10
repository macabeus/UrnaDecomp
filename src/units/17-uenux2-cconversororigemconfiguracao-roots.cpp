// Roots: keep comum::asn::CConversorOrigemConfiguracao in the skeleton link the way the target keeps it. Its users are
// not reconstructed yet; in the target the converter's vtable (@1570240) is data whose DoConverte/DoDesconverte slots
// put the bodies in the table (fns 11367, 11366), so rooting the vtable keeps the same set.
// Declared through an asm label, so this TU needs no uenux2 or ASN.1 header and instantiates nothing.
extern "C" {
extern const char origemconfiguracao_vt[] __asm__("_ZTVN5comum3asn28CConversorOrigemConfiguracaoE");
}

namespace {

const void* volatile origemconfiguracao_sink;

struct SOrigemConfiguracaoRoots {
    SOrigemConfiguracaoRoots() { origemconfiguracao_sink = origemconfiguracao_vt; }
} origemconfiguracao_roots;

} // namespace
