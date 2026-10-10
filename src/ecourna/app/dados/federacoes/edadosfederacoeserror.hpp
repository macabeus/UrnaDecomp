#pragma once
#include "ecourna/api/exception/cbaseerror.hpp"

namespace ecourna::app::dados {

// Names synthesised; values are the target's throw-site codes.
enum EDadosFederacoesError {
    EDFE_LISTA_PARTIDOS_VAZIA = 2940,
};

using CDadosFederacoesError = api::exception::CBaseError<EDadosFederacoesError, api::exception::SErrorLimits{2940, 2965}>;

} // namespace ecourna::app::dados
