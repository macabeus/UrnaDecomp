#include "ecourna/app/dados/cfabricaidentificadoreleitor.hpp"

#include "ecourna/app/dados/cnumerocpf.hpp"
#include "ecourna/app/dados/cnumeroidentificacaolivre.hpp"
#include "ecourna/app/dados/cnumeroinscricaoeleitoral.hpp"

namespace ecourna::app::dados {

std::shared_ptr<IIdentificadorEleitor> CFabricaIdentificadorEleitor::Cria(int tipo, const std::string& numero)
{
    switch (tipo) {
    case TipoIDInscricaoEleitoral:
        return std::make_shared<CNumeroInscricaoEleitoral>(numero);
    case TipoIDCPF:
        return std::make_shared<CNumeroCPF>(numero);
    default:
        return std::make_shared<CNumeroIdentificacaoLivre>(numero);
    }
}

} // namespace ecourna::app::dados
