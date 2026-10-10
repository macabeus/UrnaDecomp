#pragma once

#include "ecourna/api/exception/cbaseerror.hpp"

namespace api {

enum EUeGuiError {
    EUE_GUI_TEXTFIELDMULTILINE_TEXTO_NULO = 4972,
    // the value is the code api::IScreen::GetFontMetrics (target fn 8938) throws; the name is chosen from its message,
    // "Método não implementado": no evidence names it
    EUE_GUI_METODO_NAO_IMPLEMENTADO = 4982,
};

using CUeGuiError = ecourna::api::exception::CBaseError<EUeGuiError, ecourna::api::exception::SErrorLimits{4900, 5100}>;

} // namespace api
