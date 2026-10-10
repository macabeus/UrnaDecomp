#pragma once

#include <string>

namespace api {

/**
 * Uma chave (nome = valor) de um arquivo INI.
 */
class CIniKey {
public:
    CIniKey(const std::string& nome, const std::string& valor)
        : m_nome(nome)
        , m_valor(valor)
    {
    }

    CIniKey& operator=(const CIniKey& chave);

    const std::string& GetNome() const { return m_nome; }
    const std::string& GetValor() const { return m_valor; }

private:
    std::string m_nome;
    std::string m_valor;
};

} // namespace api
