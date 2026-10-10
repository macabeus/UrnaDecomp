// Roots: keep api::CThread::~CThread (target fn 2721) in the skeleton link. The target reaches it through slot 0 of
// the CThread vtable and from the destructors of CThreadVota, CThreadEleitor and CThreadOperador (fns 2126, 2438,
// 2717), none of them reconstructed yet; it is address-taken from a static constructor (body intact, `this` result
// kept as a table entry keeps it), which also brings in the vtable its vptr store names. Declared through an asm
// label with the wasm-level signature; compiled as uenux2 code.
extern "C" {
int w2721_root_0(int) __asm__("_ZN3api7CThreadD2Ev");
}

namespace {

void* volatile w2721_sink;

struct SW2721Roots {
    SW2721Roots() { w2721_sink = reinterpret_cast<void*>(&w2721_root_0); }
} w2721_roots;

} // namespace
