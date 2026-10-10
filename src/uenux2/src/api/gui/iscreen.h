#pragma once

// api::IScreen (/home/rubio/tse/uenux2/src/api/gui/iscreen.h): the screen the forms draw on.
// Its vtable (@1529092, 39 slots, ledger/vtables.json) holds the destructors (slot 0, fn 5071; slot 1, the trap an
// abstract class's deleting destructor gets), GetFontMetrics in slot 3 (target fn 8938, here: its throw is the record at
// line 69, column 15), 33 pure virtual members and three members with an empty body (fn 218, slots 13, 37 and 38).
// No record or symbol names those 36 other members, so they are not recovered. simulador::CWasmScreen overrides slot 3
// (fn 9261), so no target function calls this GetFontMetrics: it is reached only through IScreen's vtable.

#include <string>

#include "api/gui/eueguierror.h"
#include "api/gui/primitives.h"

namespace api {

/**
 * Tela onde os formulários são desenhados.
 */
class IScreen {
public:
    virtual ~IScreen() = default;

    // (original lines 24-61: content not recovered)
#line 62
    /**
     * Obtém a largura e a altura que @p texto ocupa escrito com a fonte @p font.
     *
     * @throw CUeGuiError se a tela não implementa a medida
     */
    virtual void GetFontMetrics(TPosition& largura, TPosition& altura, const SFont& font, const std::string& texto)
    {
        throw CUeGuiError(EUE_GUI_METODO_NAO_IMPLEMENTADO, "Método não implementado");
    }
};

} // namespace api
