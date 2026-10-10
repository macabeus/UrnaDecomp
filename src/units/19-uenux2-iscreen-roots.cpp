// Roots: api::IScreen::GetFontMetrics (target fn 8938) is an inline virtual member of api/gui/iscreen.h. The target
// emits it with IScreen's vtable (@1529092) in the TU that builds simulador::CWasmScreen, which is not reconstructed,
// and reaches it only through that vtable (slot 3, table[604]): CWasmScreen overrides the slot and no target function
// calls it (ledger/callgraph.tsv has no caller of 8938). So it is address-taken here, which keeps it in the table with
// its five parameters, and nothing calls it.
// An inline member is emitted only by a TU that uses it: the optnone function below makes the non-virtual call, so
// clang emits the COMDAT body here (the way 03-ecourna-api-io-util-roots.cpp gets CFile's inline constructor); it is not
// rooted, so wasm-ld drops it and binaryen sees no direct caller.
// Compiled as uenux2 code, from its /work path and with -iquote /work/src/uenux2/src (fragment 19), so that the header
// is found as /work/src/uenux2/src/api/gui/iscreen.h and -ffile-prefix-map gives the throw's std::source_location the
// target's file name, /home/rubio/tse/uenux2/src/api/gui/iscreen.h (the relative -Isrc/uenux2/src would leave it
// src/uenux2/src/api/gui/iscreen.h).
#include <string>

#include "api/gui/iscreen.h"

extern "C" {
void iscreen_get_font_metrics(int, int, int, int, int) __asm__(
    "_ZN3api7IScreen14GetFontMetricsERsS1_RKNS_5SFontERKNSt3__212basic_stringIcNS5_11char_traitsIcEENS5_9allocatorIcEEEE");
}

__attribute__((optnone, noinline)) void iscreen_emite_get_font_metrics(api::IScreen& tela, api::TPosition& largura, api::TPosition& altura,
                                                                       const api::SFont& font, const std::string& texto)
{
    tela.api::IScreen::GetFontMetrics(largura, altura, font, texto);
}

namespace {

void* volatile iscreen_sink;

struct SIScreenRoots {
    SIScreenRoots() { iscreen_sink = reinterpret_cast<void*>(&iscreen_get_font_metrics); }
} iscreen_roots;

} // namespace
