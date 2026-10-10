// Roots: keep CConversorMunicipioZona in the skeleton link the way the target keeps it. Its users are not
// reconstructed yet; in the target the converter's vtable (@1130576) is data whose DoConverte/DoDeconverte slots put
// the bodies in the table (fns 9131, 9130; uses 0 besides the table), so rooting the vtable keeps the same set.
// Declared through an asm label, so this TU needs no ecourna or ASN.1 header and instantiates nothing.
extern "C" {
extern const char municipiozona_vt[] __asm__("_ZTVN7ecourna3app5dados3asn23CConversorMunicipioZonaE");
}

namespace {

const void* volatile municipiozona_sink;

struct SMunicipioZonaRoots {
    SMunicipioZonaRoots() { municipiozona_sink = municipiozona_vt; }
} municipiozona_roots;

} // namespace
