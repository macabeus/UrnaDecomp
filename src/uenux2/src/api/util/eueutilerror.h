#pragma once

#include "ecourna/api/exception/cbaseerror.hpp"

namespace api {

enum EUeUtilError {
    EUE_UTIL_DIRETORIO_NAO_ABERTO = 7010,
    EUE_UTIL_TICK_TEMPO_INVALIDO = 7072,
    EUE_UTIL_TICK_NAO_EXISTE_PARAR = 7074,
    EUE_UTIL_TICK_NAO_EXISTE_INICIAR = 7075,
    EUE_UTIL_TICK_SEM_ID_DISPONIVEL = 7077,
    EUE_UTIL_TEMPO_EXCESSIVO = 7083,
};

using CUeUtilError = ecourna::api::exception::CBaseError<EUeUtilError, ecourna::api::exception::SErrorLimits{7000, 7200}>;

} // namespace api
