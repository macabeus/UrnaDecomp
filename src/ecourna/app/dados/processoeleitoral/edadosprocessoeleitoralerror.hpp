#pragma once
#include "ecourna/api/exception/cbaseerror.hpp"

namespace ecourna::app::dados {

// Names synthesised; values are the target's throw-site codes.
enum EDadosProcessoEleitoralError {
    EDPE_CONFIGURACOES_MUNICIPIOS_VAZIA = 3145,
    EDPE_INDICE_CONFIGURACAO_MUNICIPIO = 3146,
};

using CDadosProcessoEleitoralError =
    api::exception::CBaseError<EDadosProcessoEleitoralError, api::exception::SErrorLimits{3140, 3190}>;

} // namespace ecourna::app::dados
