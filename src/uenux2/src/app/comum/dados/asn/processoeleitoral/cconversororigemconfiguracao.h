#pragma once

#include "app/comum/asn/iconversorasn.h"
#include "app/comum/dados/md/processoeleitoral/eorigemconfiguracao.h"
#include "asn1/modules/ModuloProcessoEleitoral.h"

namespace comum::asn {

class CConversorOrigemConfiguracao
    : public IConversorASN<ModuloProcessoEleitoral::OrigemConfiguracao, md::EOrigemConfiguracao> {
protected:
    TEntidade DoConverte(const TDado& origem) const override;
    TDado DoDesconverte(const TEntidade& entidade) const override;
};

} // namespace comum::asn
