// Roots: vota::CThreadVota::StartTick (target fn 700) is called on the CThreadEleitor and CThreadOperador
// singletons from 16 sites in vota TUs not reconstructed yet. Address-taken from a static constructor (as in
// 06-ecourna-api-compression-roots.cpp) so the link keeps it out of line. CThreadVota::StopTick is left unrooted: its one caller here
// would let binaryen inline CTickManager::StopTick, which the target keeps out of line.
// The target's callers of api::CWait's ctor (fn 5447) use the object after constructing it, so the ctor keeps its
// `this` result (binaryen's dead-argument pass drops it when no caller reads it): EsperaUsando stands for them.
// Compiled as uenux2 code.
#include <cstddef>

#include "api/util/cwait.h"

extern "C" {
void five700_root_0(int, int) __asm__("_ZN4vota11CThreadVota9StartTickEh");
}

namespace {

void* volatile five700_sink;

__attribute__((noinline)) void EsperaUsando(size_t milissegundos, void (*usa)(api::CWait&))
{
    api::CWait espera(milissegundos);
    usa(espera);
}

struct SFive700Roots {
    SFive700Roots()
    {
        five700_sink = reinterpret_cast<void*>(&five700_root_0);
        five700_sink = reinterpret_cast<void*>(&EsperaUsando);
    }
} five700_roots;

} // namespace
