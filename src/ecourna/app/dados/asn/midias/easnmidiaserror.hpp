#pragma once
#include "ecourna/api/exception/cbaseerror.hpp"

namespace ecourna::app::dados::asn {

// Values are the target's throw-site codes (typeinfo 1126536); names synthesised from the messages. 2487 is not
// thrown by any function the target links.
enum EAsnMidiasError {
    EAME_TIPO_APLICATIVO_INVALIDO = 2485,
    EAME_TIPO_APLICATIVO_ENTIDADE_INVALIDO = 2486,
    EAME_TIPO_MIDIA_INVALIDO = 2488,
    EAME_TIPO_MIDIA_ENTIDADE_INVALIDO = 2489,
};

using CAsnMidiasError = api::exception::CBaseError<EAsnMidiasError, api::exception::SErrorLimits{2485, 2510}>;

} // namespace ecourna::app::dados::asn
