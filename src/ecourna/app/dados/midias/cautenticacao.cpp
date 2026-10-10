#include "ecourna/app/dados/midias/cautenticacao.hpp"

#include "ecourna/app/dados/midias/edadosmidiaserror.hpp"

namespace ecourna::app::dados {

CAutenticacao::CAutenticacao(uedword numeroTentativas, uedword tamanhoSenha, const std::vector<uebyte>& hashSenha)
    : m_numeroTentativas(numeroTentativas)
    , m_tamanhoSenha(tamanhoSenha)
    , m_hashSenha(hashSenha)
{
}

CAutenticacao::CAutenticacao(const boost::posix_time::ptime& dataHoraInicial,
                             const boost::posix_time::ptime& dataHoraFinal, uedword numeroTentativas,
                             uedword tamanhoSenha, const std::vector<uebyte>& hashSenha)
    : m_dataHoraInicial(dataHoraInicial)
    , m_dataHoraFinal(dataHoraFinal)
    , m_numeroTentativas(numeroTentativas)
    , m_tamanhoSenha(tamanhoSenha)
    , m_hashSenha(hashSenha)
{
}

// (original lines 25-68: content not recovered)
#line 69
const boost::posix_time::ptime& CAutenticacao::GetDataHoraInicial() const
{
    if (!PossuiDataHora()) {
        throw CDadosMidiasError(EDME_DATA_HORA_INICIAL_NAO_DEFINIDA, "Data e hora de início não definidos para este objeto.");
    }
    return *m_dataHoraInicial;
}

// (original lines 77-78: content not recovered)
#line 79
const boost::posix_time::ptime& CAutenticacao::GetDataHoraFinal() const
{
    if (!PossuiDataHora()) {
        throw CDadosMidiasError(EDME_DATA_HORA_FINAL_NAO_DEFINIDA, "Data e hora de fim não definidos para este objeto.");
    }
    return *m_dataHoraFinal;
}

} // namespace ecourna::app::dados
