#pragma once
#include <source_location>
#include <string>
#include <utility>

#include "ecourna/api/exception/cerror.hpp"
#include "ecourna/api/exception/serrorlimits.hpp"

namespace ecourna::api::exception {

template <typename E, SErrorLimits Limites>
class CBaseError : public CError {
public:
    CBaseError(E codigo, std::string mensagem, std::source_location local = std::source_location::current())
        : CError(static_cast<int>(codigo), std::move(mensagem), local)
    {
    }
};

} // namespace ecourna::api::exception
