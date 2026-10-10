#pragma once

#include "app/comum/asn/iconversorasn.h"
#include "app/comum/dados/md/estadoaplicacao/cnumviasimpressasrelatorios.h"
#include "asn1/modules/ModuloEstadoGeralDefs.h"

namespace comum::asn {

class CConversorNumViasImpressasRelatorios
    : public IConversorASN<ModuloEstadoGeralDefs::NumViasImpressasRelatorios, md::estadoaplicacao::CNumViasImpressasRelatorios> {
protected:
    TEntidade DoConverte(const TDado& dado) const override;
    TDado DoDesconverte(const TEntidade& entidade) const override;
};

} // namespace comum::asn
