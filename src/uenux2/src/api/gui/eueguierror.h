#pragma once

#include "ecourna/api/exception/cbaseerror.hpp"

namespace api {

enum EUeGuiError {
    // the values are the codes api::CBmpConversor::VerticalFlip (target fn 3664) throws; the names are chosen from its
    // messages, "Dado de entrada nulo" and "Dimensões inválidas": no evidence names them
    EUE_GUI_VERTICALFLIP_DADO_NULO = 4900,
    EUE_GUI_VERTICALFLIP_DIMENSOES_INVALIDAS = 4901,
    // the value is the code api::CBmpConversor::InvertColors (target fn 3663) throws; the name is chosen from its message,
    // "Dado de entrada nulo": no evidence names it
    EUE_GUI_INVERTCOLORS_DADO_NULO = 4903,
    EUE_GUI_TEXTFIELDMULTILINE_TEXTO_NULO = 4972,
    // the value is the code api::IScreen::GetFontMetrics (target fn 8938) throws; the name is chosen from its message,
    // "Método não implementado": no evidence names it
    EUE_GUI_METODO_NAO_IMPLEMENTADO = 4982,
};

using CUeGuiError = ecourna::api::exception::CBaseError<EUeGuiError, ecourna::api::exception::SErrorLimits{4900, 5100}>;

} // namespace api
