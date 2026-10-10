#include "ecourna/app/dados/cnumeroidentificacaolivre.hpp"

#include <format>

#include "ecourna/app/dados/edadoserror.hpp"

namespace ecourna::app::dados {

// (original lines 9-17: content not recovered)
#line 18
CNumeroIdentificacaoLivre::CNumeroIdentificacaoLivre(const std::string& numeroIdentificacao)
    : IIdentificadorEleitor(TTipoIdentificadorEleitor(TipoIDNumeroLivre), numeroIdentificacao, 12)
{
    if (GetTamanho() < numeroIdentificacao.size()) {
        throw CDadosError(EDE_TAMANHO_IDENTIFICACAO_LIVRE,
                          std::format("O tamanho do número de identificação livre não pode ser maior do que {} ({}).",
                                      GetTamanho(), numeroIdentificacao));
    }
}

std::string CNumeroIdentificacaoLivre::Formatado() const
{
    return GetNumero();
}

} // namespace ecourna::app::dados
