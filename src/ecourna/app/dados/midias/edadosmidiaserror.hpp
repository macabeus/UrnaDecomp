#pragma once
#include "ecourna/api/exception/cbaseerror.hpp"

namespace ecourna::app::dados {

// Values are the target's throw-site codes (typeinfo 1138092); names synthesised from the messages.
enum EDadosMidiasError {
    EDME_AUTENTICACAO_NAO_DEFINIDA = 3040,
    EDME_DATA_HORA_INICIAL_NAO_DEFINIDA = 3041,
    EDME_DATA_HORA_FINAL_NAO_DEFINIDA = 3042,
    EDME_TIPO_MIDIA_INVALIDO = 3043,
    EDME_APLICATIVOS_SOMENTE_MIDIA_RESULTADO = 3044,
};

using CDadosMidiasError = api::exception::CBaseError<EDadosMidiasError, api::exception::SErrorLimits{3040, 3065}>;

} // namespace ecourna::app::dados
