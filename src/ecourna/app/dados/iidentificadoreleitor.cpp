#include "ecourna/app/dados/iidentificadoreleitor.hpp"

#include <format>

#include "ecourna/api/util/cstringutils.hpp"
#include "ecourna/app/dados/edadoserror.hpp"

namespace ecourna::app::dados {

IIdentificadorEleitor::IIdentificadorEleitor(TTipoIdentificadorEleitor tipo, const std::string& numero, uebyte tamanho)
    : m_tipo(tipo), m_numero(api::util::CStringUtils::PreencheAEsquerda(numero, '0', tamanho)), m_tamanho(tamanho)
{
    const std::string zeros(tamanho, '0');
    if (m_numero == zeros || m_numero.find_first_not_of("0123456789") != std::string::npos) {
        throw CDadosError(EDE_IDENTIFICADOR_NAO_NUMERICO,
                          std::format("O número de identificação de eleitor deve ser numérico ({}).", numero));
    }
}

std::string IIdentificadorEleitor::GetNumero() const
{
    return m_numero;
}

} // namespace ecourna::app::dados
