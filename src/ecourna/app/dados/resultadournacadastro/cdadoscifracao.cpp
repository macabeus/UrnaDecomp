#include "ecourna/app/dados/resultadournacadastro/cdadoscifracao.hpp"

#include <utility>

#include "ecourna/app/dados/resultadournacadastro/edadosresultadournacadastroerror.hpp"

namespace ecourna::app::dados {

// (original lines 9-22: content not recovered)
#line 23
CDadosCifracao::CDadosCifracao(std::vector<uebyte> chave, std::vector<uebyte> salt, std::vector<uebyte> informacaoAdicional)
    : m_chave(std::move(chave)), m_salt(std::move(salt)), m_informacaoAdicional(std::move(informacaoAdicional))
{
    if (m_salt.size() < 16) {
        throw CDadosResultadoUrnaCadastroError(EDRUCE_TAMANHO_SALT, "O tamanho de salt não pode ser menor do que 16.");
    }


    if (m_informacaoAdicional.size() < 16) {
        throw CDadosResultadoUrnaCadastroError(EDRUCE_TAMANHO_INFORMACAO_ADICIONAL,
                                               "O tamanho de informacaoAdicional não pode ser menor do que 16.");
    }
}

} // namespace ecourna::app::dados
