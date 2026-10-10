#include "ecourna/app/dados/cnumerocpf.hpp"

#include <format>

#include "ecourna/app/dados/edadoserror.hpp"

namespace ecourna::app::dados {

// (original lines 9-16: content not recovered)
#line 17
CNumeroCPF::CNumeroCPF(const std::string& numeroCPF) : IIdentificadorEleitor(TTipoIdentificadorEleitor(TipoIDCPF), numeroCPF, 11)
{
    if (GetTamanho() < numeroCPF.size()) {
        throw CDadosError(EDE_TAMANHO_CPF, std::format("O tamanho do número de CPF não pode ser maior do que {} ({}).",
                                                       GetTamanho(), numeroCPF));
    }
}

std::string CNumeroCPF::Formatado() const
{
    std::string numero = GetNumero();
    return numero.insert(9, "-").insert(6, ".").insert(3, ".");
}

} // namespace ecourna::app::dados
