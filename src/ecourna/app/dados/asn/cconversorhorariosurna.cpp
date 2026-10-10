#include "ecourna/app/dados/asn/cconversorhorariosurna.hpp"

#include "ecourna/app/dados/asn/datahoraasn.hpp"

namespace ecourna::app::dados::asn {

CHorariosUrna CConversorHorariosUrna::DoDeconverte(const TEntidade& entidade) const
{
    return CHorariosUrna(DesconverteDataHora(entidade.get_emissaoZeresima()),
                         DesconverteDataHora(entidade.get_inicioVotacao()),
                         DesconverteDataHora(entidade.get_encerramentoVotacao()),
                         DesconverteDataHora(entidade.get_terminoVotacao()));
}

} // namespace ecourna::app::dados::asn
