#pragma once
#include "ecourna/api/exception/cbaseerror.hpp"

namespace ecourna::api::asn {

enum EApiAsnError {
    EAAE_ENTIDADE_INVALIDA = 1900,
    EAAE_DADO_INVALIDO = 1902,
};

using CApiAsnError = exception::CBaseError<EApiAsnError, exception::SErrorLimits{1900, 1910}>;

} // namespace ecourna::api::asn
