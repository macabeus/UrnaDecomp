#include "ecourna/app/dados/resultadournacadastro/chabilitacaobiometrica.hpp"

#include "ecourna/app/dados/resultadournacadastro/edadosresultadournacadastroerror.hpp"

namespace ecourna::app::dados {

CHabilitacaoBiometrica::CHabilitacaoBiometrica(uedword tentativas, TScoreHabilitacao ultimoScore,
                                               CDedo::ETipoDedo dedoHabilitado,
                                               TErroLeituraBiometria erroLeituraBiometria)
    : m_tentativas(tentativas), m_ultimoScore(ultimoScore), m_dedoHabilitado(dedoHabilitado),
      m_erroLeituraBiometria(erroLeituraBiometria)
{
}

CHabilitacaoBiometrica::CHabilitacaoBiometrica(uedword tentativas, TScoreHabilitacao ultimoScore,
                                               CDedo::ETipoDedo dedoHabilitado,
                                               TErroLeituraBiometria erroLeituraBiometria,
                                               const CEstadoHabilitacaoPorCodigo& estadoHabilitacaoPorCodigo)
    : m_tentativas(tentativas), m_ultimoScore(ultimoScore), m_dedoHabilitado(dedoHabilitado),
      m_erroLeituraBiometria(erroLeituraBiometria), m_estadoHabilitacaoPorCodigo(estadoHabilitacaoPorCodigo)
{
}

// (original lines 24-40: content not recovered)
#line 41
const CEstadoHabilitacaoPorCodigo& CHabilitacaoBiometrica::GetEstadoHabilitacaoPorCodigo() const
{
    if (!m_estadoHabilitacaoPorCodigo) {
        throw CDadosResultadoUrnaCadastroError(EDRUCE_ESTADO_HABILITACAO_POR_CODIGO_NAO_DEFINIDO,
                                               "Estado de habilitação por código não definido para este objeto.");
    }
    return *m_estadoHabilitacaoPorCodigo;
}

} // namespace ecourna::app::dados
