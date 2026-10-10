#pragma once
#include <exception>
#include <source_location>
#include <string>

namespace ecourna::api::exception {

class CError : public std::exception {
public:
    CError(int codigo, std::string mensagem, std::source_location local);

    const char* what() const noexcept override { return m_descricao.c_str(); }
    int GetCodigo() const { return m_codigo; }
    const std::string& GetMensagem() const { return m_mensagem; }
    const std::source_location& GetLocal() const { return m_local; }

private:
    int m_codigo;
    std::string m_mensagem;
    std::source_location m_local;
    unsigned int m_reservado;
    std::string m_descricao;
};

} // namespace ecourna::api::exception
