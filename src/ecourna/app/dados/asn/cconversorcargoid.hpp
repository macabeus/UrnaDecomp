#pragma once
#include "ModuloTiposEleitorais.h"
#include "ecourna/api/asn/iconversorasn.hpp"
#include "ecourna/app/dados/tiposbasicos.hpp"

namespace ecourna::app::dados::asn {

class CConversorCargoID : public api::asn::IConversorASN<ModuloTiposEleitorais::CodigoCargoConsulta, TCargoID> {
protected:
    TEntidade DoConverte(const TDado& dado) const override;
    TDado DoDeconverte(const TEntidade& entidade) const override;
};

} // namespace ecourna::app::dados::asn
