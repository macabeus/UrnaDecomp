// Roots: keep CRdvVota::Candidato (target fn 1931, CRdvVota vtable slot 3) and the CVotosEleicoesVota totals
// that share its GetEleicao helper, and GetVotos (the second CUeRdvError thrower) in the skeleton link. Their real callers (the CRdvVota vtable, the BU generators) are
// not reconstructed yet, so they are address-taken from a static constructor (bodies intact, no export, no inlining).
// CVotosEleicoesVota::Candidato is deliberately not rooted: its only caller is CRdvVota::Candidato, which wasm-opt
// inlines it into, as in the target. Declared through asm labels with the wasm-level signatures; compiled as uenux2 code.
extern "C" {
int w1931_root_0(int, int, int, int) __asm__("_ZNK5comum8CRdvVota9CandidatoEhjh");
int w1931_root_1(int, int, int) __asm__("_ZNK5comum2md18CVotosEleicoesVota7LegendaEhRKNSt3__212basic_stringIcNS2_11char_traitsIcEENS2_9allocatorIcEEEE");
int w1931_root_2(int, int, int) __asm__("_ZNK5comum2md18CVotosEleicoesVota7PartidoEhRKNSt3__212basic_stringIcNS2_11char_traitsIcEENS2_9allocatorIcEEEE");
int w1931_root_3(int, int) __asm__("_ZNK5comum2md18CVotosEleicoesVota8NominaisEh");
int w1931_root_4(int, int) __asm__("_ZNK5comum2md18CVotosEleicoesVota8LegendasEh");
int w1931_root_5(int, int) __asm__("_ZNK5comum2md18CVotosEleicoesVota5NulosEh");
int w1931_root_6(int, int) __asm__("_ZNK5comum2md18CVotosEleicoesVota7BrancosEh");
int w1931_root_7(int, int) __asm__("_ZNK5comum2md18CVotosEleicoesVota5CargoEh");
int w1931_root_8(int, int) __asm__("_ZNK5comum2md18CVotosEleicoesVota8GetVotosEj");
}

namespace {

void* volatile w1931_sink;

struct SW1931Roots {
    SW1931Roots()
    {
        w1931_sink = reinterpret_cast<void*>(&w1931_root_0);
        w1931_sink = reinterpret_cast<void*>(&w1931_root_1);
        w1931_sink = reinterpret_cast<void*>(&w1931_root_2);
        w1931_sink = reinterpret_cast<void*>(&w1931_root_3);
        w1931_sink = reinterpret_cast<void*>(&w1931_root_4);
        w1931_sink = reinterpret_cast<void*>(&w1931_root_5);
        w1931_sink = reinterpret_cast<void*>(&w1931_root_6);
        w1931_sink = reinterpret_cast<void*>(&w1931_root_7);
        w1931_sink = reinterpret_cast<void*>(&w1931_root_8);
    }
} w1931_roots;

} // namespace
