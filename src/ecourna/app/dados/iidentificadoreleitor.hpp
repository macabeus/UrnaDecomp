#pragma once
#include <memory>
#include <string>

#include "ecourna/api/uetypes.hpp"
#include "ecourna/app/dados/tiposbasicos.hpp"

namespace ecourna::app::dados {

class IIdentificadorEleitor {
public:
    IIdentificadorEleitor(TTipoIdentificadorEleitor tipo, const std::string& numero, uebyte tamanho);
    virtual ~IIdentificadorEleitor() = default;

    virtual std::string Formatado() const = 0;

    int GetTipo() const { return m_tipo; }
    std::string GetNumero() const;
    uebyte GetTamanho() const { return m_tamanho; }

private:
    int m_tipo;
    std::string m_numero;
    uebyte m_tamanho;
};

using TSharedIdentificadorEleitor = std::shared_ptr<IIdentificadorEleitor>;

} // namespace ecourna::app::dados
