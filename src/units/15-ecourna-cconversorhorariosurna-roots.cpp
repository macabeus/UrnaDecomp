// Roots: keep CConversorHorariosUrna in the skeleton link the way the target keeps it. Its user (the
// CConfiguracaoMunicipios converter, fns 9137/9141) is not reconstructed yet; in the target the converter's vtable
// (1130320) is data whose DoConverte/DoDeconverte slots put fns 9133 and 9132 in the table, so rooting the vtable keeps
// the same set. DoDeconverte is the CHorariosUrna constructor's only caller, so binaryen inlines the constructor
// there as in the target. Declared through an asm label, so this TU needs no ecourna or ASN.1 header.
extern "C" {
extern const char horarios_vt_conversor[] __asm__("_ZTVN7ecourna3app5dados3asn22CConversorHorariosUrnaE");
}

namespace {

const void* volatile horarios_sink;

struct SHorariosUrnaRoots {
    SHorariosUrnaRoots() { horarios_sink = horarios_vt_conversor; }
} horarios_roots;

} // namespace
