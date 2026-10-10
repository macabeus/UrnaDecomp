#pragma once
#include "ecourna/api/exception/cbaseerror.hpp"

namespace ecourna::api::pattern {

enum EPatternError {
    EPE_VALOR_INVALIDO = 1300,
};

using CPatternError = exception::CBaseError<EPatternError, exception::SErrorLimits{1300, 1325}>;

} // namespace ecourna::api::pattern
