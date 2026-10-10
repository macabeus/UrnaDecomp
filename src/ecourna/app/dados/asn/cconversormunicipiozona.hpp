#pragma once
#include "ModuloTiposEleitorais.h"
#include "ecourna/api/asn/iconversorasn.hpp"
#include "ecourna/app/dados/cmunicipiozona.hpp"

namespace ecourna::app::dados::asn {

class CConversorMunicipioZona
    : public api::asn::IConversorASN<ModuloTiposEleitorais::MunicipioZona, CMunicipioZona> {
protected:
    TEntidade DoConverte(const TDado& dado) const override;
    TDado DoDeconverte(const TEntidade& entidade) const override;
};

} // namespace ecourna::app::dados::asn
