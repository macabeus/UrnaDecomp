// Roots: keep comum::asn::CConversorNumViasImpressasRelatorios and the CNumViasImpressasRelatorios constructor in the
// skeleton link the way the target keeps them. Their users are not reconstructed yet.
// - The converter's vtable (@1569288) is data whose DoConverte/DoDesconverte slots put the bodies in the table (fns
//   11386, 11387), so rooting the vtable keeps the same set.
// - The constructor (fn 5629) is called directly by two functions in the target (11387 and 5324) and by no table
//   entry; with DoDesconverte as its only caller wasm-opt would inline it, so a noinline caller here is the second
//   one. It drops the returned `this`, as both target callers do, so wasm-opt removes the return value as there.
// Declared through asm labels with the wasm-level signatures, so this TU needs no uenux2 or ASN.1 header.
extern "C" {
extern const char numvias_vt[] __asm__("_ZTVN5comum3asn36CConversorNumViasImpressasRelatoriosE");
void* numvias_ctor(void*, int, int, int, int) __asm__("_ZN5comum2md15estadoaplicacao27CNumViasImpressasRelatoriosC1Ehhhh");
}

namespace {

const void* volatile numvias_sink;

__attribute__((noinline)) void numvias_build(void* dado)
{
    numvias_ctor(dado, 1, 1, 1, 1);
}

struct SNumViasRoots {
    SNumViasRoots()
    {
        numvias_sink = numvias_vt;
        numvias_sink = reinterpret_cast<const void*>(&numvias_build);
    }
} numvias_roots;

} // namespace
