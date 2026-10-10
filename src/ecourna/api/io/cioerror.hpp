#pragma once
#include <source_location>
#include <string>

#include "ecourna/api/exception/cbaseerror.hpp"
#include "ecourna/api/io/efileoperation.hpp"
#include "ecourna/api/io/eioerror.hpp"

namespace ecourna::api::io {

// what() = "<mensagem> [<Description(operacao)>] - <erro> - <strerror(erro)>" (or just "<mensagem> [<op>]" when erro == 0).
class CIoError : public exception::CBaseError<EIoError, exception::SErrorLimits{1175, 1275}> {
public:
    CIoError(EIoError codigo, EFileOperation operacao, const std::string& mensagem, int erro = 0,
             std::source_location local = std::source_location::current());
};

} // namespace ecourna::api::io
