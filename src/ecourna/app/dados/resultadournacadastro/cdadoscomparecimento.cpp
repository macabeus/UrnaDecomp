#include "ecourna/app/dados/resultadournacadastro/cdadoscomparecimento.hpp"

#include <utility>

#include "ecourna/app/dados/resultadournacadastro/edadosresultadournacadastroerror.hpp"

namespace ecourna::app::dados {

CDadosComparecimento::CDadosComparecimento(TVectorIdentificacaoJustificativa justificativas,
                                           CComparecimentoSecao identificacaoComparecimento,
                                           std::optional<TVectorComparecimentoMesario> mesariosAbertura,
                                           std::optional<TVectorComparecimentoMesario> mesariosEncerramento)
    : m_justificativas(std::move(justificativas)), m_identificacaoComparecimento(std::move(identificacaoComparecimento)),
      m_mesariosAbertura(std::move(mesariosAbertura)), m_mesariosEncerramento(std::move(mesariosEncerramento))
{
}

// (original lines 18-34: content not recovered)
#line 35
const TVectorComparecimentoMesario& CDadosComparecimento::GetMesariosAbertura() const
{
    if (!m_mesariosAbertura) {
        throw CDadosResultadoUrnaCadastroError(EDRUCE_MESARIOS_ABERTURA_NAO_DEFINIDOS,
                                               "Comparecimento de mesários na abertura não definido para este objeto.");
    }
    return *m_mesariosAbertura;
}

// (original lines 44-45: content not recovered)
#line 46
const TVectorComparecimentoMesario& CDadosComparecimento::GetMesariosEncerramento() const
{
    if (!m_mesariosEncerramento) {
        throw CDadosResultadoUrnaCadastroError(EDRUCE_MESARIOS_ENCERRAMENTO_NAO_DEFINIDOS,
                                               "Comparecimento de mesários no encerramento não definido para este objeto.");
    }
    return *m_mesariosEncerramento;
}

} // namespace ecourna::app::dados
