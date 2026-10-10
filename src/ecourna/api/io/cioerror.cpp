#include "ecourna/api/io/cioerror.hpp"

#include <cstring>
#include <string>

namespace ecourna::api::io {

const char* Description(EFileOperation operacao)
{
    const char* descricao = "Description(EFileOperation) - não reconhecida";
    switch (operacao) {
    case EFileOperation::None: descricao = "None"; break;
    case EFileOperation::Open: descricao = "Open"; break;
    case EFileOperation::Close: descricao = "Close"; break;
    case EFileOperation::Read: descricao = "Read"; break;
    case EFileOperation::Write: descricao = "Write"; break;
    case EFileOperation::Seek: descricao = "Seek"; break;
    case EFileOperation::Tell: descricao = "Tell"; break;
    case EFileOperation::Sync: descricao = "Sync"; break;
    case EFileOperation::Mode: descricao = "Mode"; break;
    }
    return descricao;
}

namespace {

std::string MontaMensagem(EFileOperation operacao, const std::string& mensagem, int erro)
{
    if (erro != 0) {
        return mensagem + " [" + Description(operacao) + "] - " + std::to_string(erro) + " - " + strerror(erro);
    }
    return mensagem + " [" + Description(operacao) + "]";
}

} // namespace

CIoError::CIoError(EIoError codigo, EFileOperation operacao, const std::string& mensagem, int erro, std::source_location local)
    : CBaseError(codigo, MontaMensagem(operacao, mensagem, erro), local)
{
}

} // namespace ecourna::api::io
