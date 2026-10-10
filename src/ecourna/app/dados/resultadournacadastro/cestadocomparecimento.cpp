#include "ecourna/app/dados/resultadournacadastro/cestadocomparecimento.hpp"

#include "ecourna/app/dados/resultadournacadastro/edadosresultadournacadastroerror.hpp"

namespace ecourna::app::dados {

CEstadoComparecimento::CEstadoComparecimento(const CRegistroIdentificacaoEleitor& identificacaoEleitor,
                                             ESituacaoComparecimento situacaoComparecimento)
    : m_identificacaoEleitor(identificacaoEleitor), m_situacaoComparecimento(situacaoComparecimento)
{
}

CEstadoComparecimento::CEstadoComparecimento(const CRegistroIdentificacaoEleitor& identificacaoEleitor,
                                             ESituacaoComparecimento situacaoComparecimento,
                                             const CApresentacaoFotoEleitor& apresentacaoFoto,
                                             ESituacaoHabilitacaoAudio situacaoHabilitacaoAudio)
    : m_identificacaoEleitor(identificacaoEleitor), m_situacaoComparecimento(situacaoComparecimento),
      m_apresentacaoFoto(apresentacaoFoto), m_situacaoHabilitacaoAudio(situacaoHabilitacaoAudio)
{
}

CEstadoComparecimento::CEstadoComparecimento(const CRegistroIdentificacaoEleitor& identificacaoEleitor,
                                             ESituacaoComparecimento situacaoComparecimento,
                                             const CApresentacaoFotoEleitor& apresentacaoFoto,
                                             ESituacaoHabilitacaoAudio situacaoHabilitacaoAudio,
                                             const CHabilitacaoBiometrica& habilitacaoBiometrica)
    : m_identificacaoEleitor(identificacaoEleitor), m_situacaoComparecimento(situacaoComparecimento),
      m_apresentacaoFoto(apresentacaoFoto), m_situacaoHabilitacaoAudio(situacaoHabilitacaoAudio),
      m_habilitacaoBiometrica(habilitacaoBiometrica)
{
}

// (original lines 33-50: content not recovered)
#line 51
// The target's two messages below are swapped (the photo getter says audio and vice versa): kept as they are.
const CApresentacaoFotoEleitor& CEstadoComparecimento::GetApresentacaoFoto() const
{
    if (!m_apresentacaoFoto) {
        throw CDadosResultadoUrnaCadastroError(EDRUCE_APRESENTACAO_FOTO_NAO_DEFINIDA,
                                               "Habilitação de áudio não definido para este objeto.");
    }
    return *m_apresentacaoFoto;
}


CEstadoComparecimento::ESituacaoHabilitacaoAudio CEstadoComparecimento::GetSituacaoHabilitacaoAudio() const
{
    if (!m_situacaoHabilitacaoAudio) {
        throw CDadosResultadoUrnaCadastroError(EDRUCE_SITUACAO_HABILITACAO_AUDIO_NAO_DEFINIDA,
                                               "Dados de apresentação de foto de eleitor não definidos para este objeto.");
    }
    return *m_situacaoHabilitacaoAudio;
}


const CHabilitacaoBiometrica& CEstadoComparecimento::GetHabilitacaoBiometrica() const
{
    if (!m_habilitacaoBiometrica) {
        throw CDadosResultadoUrnaCadastroError(EDRUCE_HABILITACAO_BIOMETRICA_NAO_DEFINIDA,
                                               "Habilitação biométrica não definida para este objeto.");
    }
    return *m_habilitacaoBiometrica;
}

} // namespace ecourna::app::dados
