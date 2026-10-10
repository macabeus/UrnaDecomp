#include "ecourna/app/dados/cidentificadorgeradormidia.hpp"

namespace ecourna::app::dados {

CIdentificadorGeradorMidia::CIdentificadorGeradorMidia(const std::string& nome, const std::string& serialCertificadoTPM,
                                                       const std::string& serialInstalacao)
    : m_nome(nome)
    , m_serialCertificadoTPM(serialCertificadoTPM)
    , m_serialInstalacao(serialInstalacao)
{
}

} // namespace ecourna::app::dados
