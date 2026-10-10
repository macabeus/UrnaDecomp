#pragma once

#include "ecourna/api/exception/cbaseerror.hpp"

namespace api {

enum EUeIoError {
    EUE_IO_NOME_CHAVE_MODIFICADO = 6005,
};

using CUeIoError = ecourna::api::exception::CBaseError<EUeIoError, ecourna::api::exception::SErrorLimits{5950, 6150}>;

} // namespace api
