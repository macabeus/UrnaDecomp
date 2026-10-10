#include "ecourna/app/dados/resultadournacadastro/cdadoscomparecimentocifrado.hpp"

#include <utility>

namespace ecourna::app::dados {

CDadosComparecimentoCifrado::CDadosComparecimentoCifrado(CDadosCifracao dadosCifracao, std::vector<uebyte> conteudo)
    : m_dadosCifracao(std::move(dadosCifracao)), m_conteudo(std::move(conteudo))
{
}

} // namespace ecourna::app::dados
