// Pending members: the api::CTextFieldMultiLine members src/uenux2/src/api/gui/ctextfieldmultiline.cpp does not hold
// yet. The class's vtable (emitted with GetTypeName in that TU) needs them defined; their bodies are NOT the target's,
// they trap. Target functions: Draw 10925, Rect 10924, Move 2783 (binaryen's one body for CFillField, CRectField,
// CTextRectField and this class), and GetTextLines 5497, which the first two call (declared, not defined here).
// Move each into ctextfieldmultiline.cpp when it is reconstructed; delete this file with the last one.
#include "api/gui/ctextfieldmultiline.h"

namespace api {

void CTextFieldMultiLine::Draw(IScreen&) const
{
    __builtin_trap();
}

SRect CTextFieldMultiLine::Rect() const
{
    __builtin_trap();
}

void CTextFieldMultiLine::Move(const SPoint&)
{
    __builtin_trap();
}

} // namespace api
