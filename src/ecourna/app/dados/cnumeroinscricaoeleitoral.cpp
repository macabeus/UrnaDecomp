#include "ecourna/app/dados/cnumeroinscricaoeleitoral.hpp"

#include <format>

#include "ecourna/app/dados/edadoserror.hpp"

namespace ecourna::app::dados {

// (original lines 9-16: content not recovered)
#line 17
CNumeroInscricaoEleitoral::CNumeroInscricaoEleitoral(const std::string& numeroInscricao)
    : IIdentificadorEleitor(TTipoIdentificadorEleitor(TipoIDInscricaoEleitoral), numeroInscricao, 12)
{
    if (GetTamanho() < numeroInscricao.size()) {
        throw CDadosError(EDE_TAMANHO_INSCRICAO,
                          std::format("O tamanho do número de inscrição eleitoral não pode ser maior do que {} ({}).",
                                      GetTamanho(), numeroInscricao));
    }
}

std::string CNumeroInscricaoEleitoral::Formatado() const
{
    std::string numero = GetNumero();
    return numero.insert(8, " ").insert(4, " ");
}

} // namespace ecourna::app::dados

// <stubs>
// f5841 · 600 B · unmatched · placement: link order · name unknown
// void f5841(i32, i32) { ... }
// </stubs>
