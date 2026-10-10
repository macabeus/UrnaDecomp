#include "ecourna/app/dados/federacoes/cfederacao.hpp"

#include "ecourna/app/dados/federacoes/edadosfederacoeserror.hpp"

namespace ecourna::app::dados {

// (original lines 7-24: content not recovered)
#line 25
CFederacao::CFederacao(TFederacaoID id, const std::string& sigla, const std::string& nome, const TVectorNumeroPartido& partidos)
    : m_id(id), m_sigla(sigla), m_nome(nome), m_partidos(partidos)
{
    if (m_partidos.empty()) {
        throw CDadosFederacoesError(EDFE_LISTA_PARTIDOS_VAZIA, "A lista de partidos não pode ser vazia.");
    }
}

} // namespace ecourna::app::dados
