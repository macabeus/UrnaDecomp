#pragma once
#include "ModuloConfiguracaoMunicipios.h"
#include "ecourna/api/asn/iconversorasn.hpp"
#include "ecourna/app/dados/processoeleitoral/chorariosurna.hpp"

namespace ecourna::app::dados::asn {

class CConversorHorariosUrna
    : public api::asn::IConversorASN<ModuloConfiguracaoMunicipios::HorariosUrna, CHorariosUrna> {
protected:
    TEntidade DoConverte(const TDado& dado) const override;
    TDado DoDeconverte(const TEntidade& entidade) const override;
};

} // namespace ecourna::app::dados::asn
