// Roots: keep api::CDirReader::NextEntry (target fn 1260) in the skeleton link. The target calls it directly from six
// functions (2762 5360 5458 5572 7160 11167, ledger/callgraph.tsv), none reconstructed yet and none through the table;
// two noinline callers give it more than one call site, so binaryen keeps its body out of line as the target does.
// They are address-taken from a static constructor. Declared through an asm label with the wasm-level signature;
// compiled as uenux2 code.
extern "C" {
int w1260_root_0(int) __asm__("_ZN3api10CDirReader9NextEntryEv");
}

namespace {

__attribute__((noinline)) int ContaEntradas(int leitor)
{
    int n = 0;
    while (w1260_root_0(leitor)) {
        ++n;
    }
    return n;
}

__attribute__((noinline)) int TemEntrada(int leitor)
{
    return w1260_root_0(leitor);
}

void* volatile w1260_sink;

struct SW1260Roots {
    SW1260Roots()
    {
        w1260_sink = reinterpret_cast<void*>(&ContaEntradas);
        w1260_sink = reinterpret_cast<void*>(&TemEntrada);
    }
} w1260_roots;

} // namespace
