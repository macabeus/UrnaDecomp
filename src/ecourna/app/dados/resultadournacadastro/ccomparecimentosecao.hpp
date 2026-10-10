#pragma once
#include <utility>
#include <vector>

#include "ecourna/app/dados/cidentificacaosecaoeleitoral.hpp"
#include "ecourna/app/dados/resultadournacadastro/cestadocomparecimento.hpp"

namespace ecourna::app::dados {

using TVectorEstadoComparecimento = std::vector<CEstadoComparecimento>;

// The section's attendance: the section (@0, 16 B) and one CEstadoComparecimento (96 B) per voter (@16). 28 bytes.
// Header-only: its implicit destructor (fn 5097) is the only function of the class in the target.
class CComparecimentoSecao {
public:
    CComparecimentoSecao(const CIdentificacaoSecaoEleitoral& identificacao, TVectorEstadoComparecimento eleitores)
        : m_identificacao(identificacao), m_eleitores(std::move(eleitores))
    {
    }

    const CIdentificacaoSecaoEleitoral& GetIdentificacao() const { return m_identificacao; }
    const TVectorEstadoComparecimento& GetEleitores() const { return m_eleitores; }

private:
    CIdentificacaoSecaoEleitoral m_identificacao;
    TVectorEstadoComparecimento m_eleitores;
};

} // namespace ecourna::app::dados
