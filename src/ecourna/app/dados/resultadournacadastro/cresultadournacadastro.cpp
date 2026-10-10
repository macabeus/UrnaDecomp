#include "ecourna/app/dados/resultadournacadastro/cresultadournacadastro.hpp"

#include <utility>

#include "ecourna/app/dados/resultadournacadastro/edadosresultadournacadastroerror.hpp"

namespace ecourna::app::dados {

CResultadoUrnaCadastro::CResultadoUrnaCadastro(CCabecalhoEntidade cabecalho, CFaseID fase, std::string versaoVotacao,
                                               ESituacaoArquivo situacao,
                                               std::optional<CDadosComparecimento> dadosComparecimento)
    : m_cabecalho(cabecalho), m_fase(fase), m_versaoVotacao(std::move(versaoVotacao)), m_situacao(situacao),
      m_dadosComparecimento(std::move(dadosComparecimento))
{
}

CResultadoUrnaCadastro::CResultadoUrnaCadastro(CCabecalhoEntidade cabecalho, CFaseID fase, std::string versaoVotacao,
                                               ESituacaoArquivo situacao,
                                               std::optional<CDadosComparecimentoCifrado> dadosComparecimentoCifrado)
    : m_cabecalho(cabecalho), m_fase(fase), m_versaoVotacao(std::move(versaoVotacao)), m_situacao(situacao),
      m_dadosComparecimentoCifrado(std::move(dadosComparecimentoCifrado))
{
}

// (original lines 25-42: content not recovered)
#line 43
const CDadosComparecimento& CResultadoUrnaCadastro::GetDadosComparecimento() const
{
    if (!m_dadosComparecimento) {
        throw CDadosResultadoUrnaCadastroError(EDRUCE_DADOS_COMPARECIMENTO_NAO_DEFINIDOS,
                                               "Dados de comparecimento não definidos para este objeto.");
    }
    return *m_dadosComparecimento;
}

// (original lines 52-53: content not recovered)
#line 54
const CDadosComparecimentoCifrado& CResultadoUrnaCadastro::GetDadosComparecimentoCifrado() const
{
    if (!m_dadosComparecimentoCifrado) {
        throw CDadosResultadoUrnaCadastroError(EDRUCE_DADOS_COMPARECIMENTO_CIFRADOS_NAO_DEFINIDOS,
                                               "Dados de comparecimento cifrados não definidos para este objeto.");
    }
    return *m_dadosComparecimentoCifrado;
}

} // namespace ecourna::app::dados
