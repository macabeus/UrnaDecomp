#pragma once

#include "ecourna/api/exception/cbaseerror.hpp"

namespace api {

enum EUeRdvError {
    EUE_RDV_CANDIDATO_SEM_ELEICAO = 4725,
    EUE_RDV_LEGENDA_SEM_ELEICAO = 4726,
    EUE_RDV_PARTIDO_SEM_ELEICAO = 4727,
    EUE_RDV_NOMINAIS_SEM_ELEICAO = 4728,
    EUE_RDV_LEGENDAS_SEM_ELEICAO = 4729,
    EUE_RDV_NULOS_SEM_ELEICAO = 4730,
    EUE_RDV_BRANCOS_SEM_ELEICAO = 4731,
    EUE_RDV_CARGO_SEM_ELEICAO = 4732,
    EUE_RDV_VOTOS_SEM_ELEICAO = 4733,
    EUE_RDV_COMPARECIMENTO_SEM_ELEICAO = 4734,
};

using CUeRdvError = ecourna::api::exception::CBaseError<EUeRdvError, ecourna::api::exception::SErrorLimits{4650, 4850}>;

} // namespace api
