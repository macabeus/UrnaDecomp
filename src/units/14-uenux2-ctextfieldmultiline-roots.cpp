// Roots: the CTextFieldMultiLine ctor has two callers in the target (fns 2782 and 12055, both a
// std::make_shared<CTextFieldMultiLine> with the colours 2 and 1, which binaryen's dead-argument elimination folds
// into the ctor); these two stand-ins give it the same call sites and are address-taken from a static constructor.
// Compiled as uenux2 code.
#include <memory>
#include <string>

#include "api/gui/ctextfieldmultiline.h"
#include "api/gui/eueguierror.h"

namespace {

__attribute__((noinline)) std::shared_ptr<api::IFormField<api::IScreen>> CampoMultiLinhaA(const api::SRect& rect, const api::SharedIText& text,
                                                                                          const api::SFont& font)
{
    return std::make_shared<api::CTextFieldMultiLine>(rect, text, font, static_cast<api::TColor>(2), static_cast<api::TColor>(1));
}

__attribute__((noinline)) std::shared_ptr<api::IFormField<api::IScreen>> CampoMultiLinhaB(api::TPosition left, api::TPosition top, api::TPosition right,
                                                                                          api::TPosition bottom, const api::SharedIText& text,
                                                                                          const api::SFont& font)
{
    const api::SRect rect{left, top, right, bottom};
    return std::make_shared<api::CTextFieldMultiLine>(rect, text, font, static_cast<api::TColor>(2), static_cast<api::TColor>(1));
}

// two more CUeGuiError throw sites (the target has 36 callers of its ctor thunk, fn 406): with one, binaryen inlines
// the CBaseError<EUeGuiError> ctor into the ctextfieldmultiline.cpp throw
__attribute__((noinline)) void LancaGui1(int c, const std::string& m) { throw api::CUeGuiError(static_cast<api::EUeGuiError>(c), m); }
__attribute__((noinline)) void LancaGui2(int c, const std::string& m) { throw api::CUeGuiError(static_cast<api::EUeGuiError>(c), m + "2"); }

void* volatile w5498_sink;

struct SW5498Roots {
    SW5498Roots()
    {
        w5498_sink = reinterpret_cast<void*>(&CampoMultiLinhaA);
        w5498_sink = reinterpret_cast<void*>(&CampoMultiLinhaB);
        w5498_sink = reinterpret_cast<void*>(&LancaGui1);
        w5498_sink = reinterpret_cast<void*>(&LancaGui2);
    }
} w5498_roots;

} // namespace
