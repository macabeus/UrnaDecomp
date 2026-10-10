#pragma once
#include "ModuloInformacaoMidia.h"
#include "ecourna/api/asn/iconversorasn.hpp"
#include "ecourna/app/dados/midias/caplicativo.hpp"

namespace ecourna::app::dados::asn {

class CConversorAplicativo : public api::asn::IConversorASN<ModuloInformacaoMidia::Aplicativo, CAplicativo> {
protected:
    TEntidade DoConverte(const TDado& dado) const override;
    TDado DoDeconverte(const TEntidade& entidade) const override;

private:
    ModuloInformacaoMidia::TipoAplicativo ConverteTipoAplicativo(CAplicativo::ETipoAplicativo tipo) const;
    CAplicativo::ETipoAplicativo DeconverteTipoAplicativo(ModuloInformacaoMidia::TipoAplicativo tipo) const;
};

} // namespace ecourna::app::dados::asn
