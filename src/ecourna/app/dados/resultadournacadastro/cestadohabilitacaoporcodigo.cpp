#include "ecourna/app/dados/resultadournacadastro/cestadohabilitacaoporcodigo.hpp"

#include "ecourna/app/dados/resultadournacadastro/edadosresultadournacadastroerror.hpp"

namespace ecourna::app::dados {

// (original lines 7-18: content not recovered)
#line 19
CEstadoHabilitacaoPorCodigo::CEstadoHabilitacaoPorCodigo(ESituacaoReconhecimentoMesario situacao)
    : m_situacaoReconhecimentoMesario(situacao)
{
    if (situacao != NaoReconhecido) {
        throw CDadosResultadoUrnaCadastroError(EDRUCE_SITUACAO_EXIGE_IDENTIFICACAO_MESARIO,
            "Para situação diferente de NaoReconhecido é preciso informar o título de eleitor do mesário.");
    }
}

// (original lines 28-31: content not recovered)
#line 32
CEstadoHabilitacaoPorCodigo::CEstadoHabilitacaoPorCodigo(ESituacaoReconhecimentoMesario situacao,
                                                         const CRegistroIdentificacaoEleitor& identificacaoMesario)
    : m_situacaoReconhecimentoMesario(situacao), m_identificacaoMesario(identificacaoMesario)
{
    if (situacao == NaoReconhecido) {
        throw CDadosResultadoUrnaCadastroError(EDRUCE_SITUACAO_PROIBE_IDENTIFICACAO_MESARIO,
            "Para situação igual a NaoReconhecido não deve ser informado o título de eleitor do mesário.");
    }
}


const CRegistroIdentificacaoEleitor& CEstadoHabilitacaoPorCodigo::GetIdentificacaoMesario() const
{
    if (!m_identificacaoMesario) {
        throw CDadosResultadoUrnaCadastroError(EDRUCE_IDENTIFICACAO_MESARIO_NAO_DEFINIDA,
                                               "Não existe título de eleitor associado a esta habiliação.");
    }
    return *m_identificacaoMesario;
}

} // namespace ecourna::app::dados
