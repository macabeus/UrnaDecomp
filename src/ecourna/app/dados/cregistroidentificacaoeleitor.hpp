#pragma once
#include <optional>

#include "ecourna/app/dados/iidentificadoreleitor.hpp"

namespace ecourna::app::dados {

// The voter's identification record: the identifier the voter is enabled by (required) and, optionally, the main one.
class CRegistroIdentificacaoEleitor {
public:
    CRegistroIdentificacaoEleitor(TSharedIdentificadorEleitor identificadorHabilitacao);
    CRegistroIdentificacaoEleitor(TSharedIdentificadorEleitor identificadorHabilitacao,
                                  TSharedIdentificadorEleitor identificadorPrincipal);

    TSharedIdentificadorEleitor GetIdentificadorHabilitacao() const;
    TSharedIdentificadorEleitor GetIdentificadorPrincipal() const;
    bool HasIdentificadorPrincipal() const { return m_identificadorPrincipal.has_value(); }

private:
    TSharedIdentificadorEleitor m_identificadorHabilitacao;
    std::optional<TSharedIdentificadorEleitor> m_identificadorPrincipal;
};

} // namespace ecourna::app::dados
