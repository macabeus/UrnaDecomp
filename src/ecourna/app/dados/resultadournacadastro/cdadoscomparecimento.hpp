#pragma once
#include <optional>

#include "ecourna/app/dados/resultadournacadastro/ccomparecimentomesario.hpp"
#include "ecourna/app/dados/resultadournacadastro/ccomparecimentosecao.hpp"
#include "ecourna/app/dados/resultadournacadastro/cidentificacaojustificativa.hpp"

namespace ecourna::app::dados {

// The section's attendance data: the justified absences (@0), the section's attendance (@12, 28 B) and, optionally,
// the poll workers present at opening (@40, engaged @52) and at closing (@56, engaged @68). 72 bytes.
class CDadosComparecimento {
public:
    CDadosComparecimento(TVectorIdentificacaoJustificativa justificativas, CComparecimentoSecao identificacaoComparecimento,
                         std::optional<TVectorComparecimentoMesario> mesariosAbertura,
                         std::optional<TVectorComparecimentoMesario> mesariosEncerramento);

    const TVectorIdentificacaoJustificativa& GetJustificativas() const { return m_justificativas; }
    const CComparecimentoSecao& GetIdentificacaoComparecimento() const { return m_identificacaoComparecimento; }
    bool HasMesariosAbertura() const { return m_mesariosAbertura.has_value(); }
    const TVectorComparecimentoMesario& GetMesariosAbertura() const;
    bool HasMesariosEncerramento() const { return m_mesariosEncerramento.has_value(); }
    const TVectorComparecimentoMesario& GetMesariosEncerramento() const;

private:
    TVectorIdentificacaoJustificativa m_justificativas;
    CComparecimentoSecao m_identificacaoComparecimento;
    std::optional<TVectorComparecimentoMesario> m_mesariosAbertura;
    std::optional<TVectorComparecimentoMesario> m_mesariosEncerramento;
};

} // namespace ecourna::app::dados
