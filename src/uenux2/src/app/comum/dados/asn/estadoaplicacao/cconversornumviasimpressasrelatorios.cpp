#include "app/comum/dados/asn/estadoaplicacao/cconversornumviasimpressasrelatorios.h"

namespace comum::asn {

CConversorNumViasImpressasRelatorios::TEntidade CConversorNumViasImpressasRelatorios::DoConverte(const TDado& dado) const
{
    TEntidade entidade;
    entidade.set_numViasEstadoUrna(dado.GetNumViasEstadoUrna());
    entidade.set_numViasEleitores(dado.GetNumViasEleitores());
    entidade.set_numViasVersoesDados(dado.GetNumViasVersoesDados());
    entidade.set_numViasPU(dado.GetNumViasPU());
    return entidade;
}

CConversorNumViasImpressasRelatorios::TDado CConversorNumViasImpressasRelatorios::DoDesconverte(const TEntidade& entidade) const
{
    return TDado(entidade.get_numViasEstadoUrna().getValue(), entidade.get_numViasEleitores().getValue(),
                 entidade.get_numViasVersoesDados().getValue(), entidade.get_numViasPU().getValue());
}

} // namespace comum::asn
