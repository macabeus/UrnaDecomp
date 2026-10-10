#include "ecourna/api/exception/cerror.hpp"

namespace ecourna::api::exception {

CError::CError(int codigo, std::string mensagem, std::source_location local)
    : m_codigo(codigo), m_mensagem(std::move(mensagem)), m_local(local), m_descricao(m_mensagem)
{
    try {
        m_descricao = m_local.function_name() + std::string(":") + std::to_string(m_local.line()) + std::string(":") +
                      std::to_string(m_codigo) + std::string(" - ") + m_mensagem;
    } catch (...) {
    }
}

} // namespace ecourna::api::exception
