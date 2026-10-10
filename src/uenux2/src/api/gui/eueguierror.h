#pragma once

#include "ecourna/api/exception/cbaseerror.hpp"

namespace api {

enum EUeGuiError {
    EUE_GUI_TEXTFIELDMULTILINE_TEXTO_NULO = 4972,
};

using CUeGuiError = ecourna::api::exception::CBaseError<EUeGuiError, ecourna::api::exception::SErrorLimits{4900, 5100}>;

} // namespace api
