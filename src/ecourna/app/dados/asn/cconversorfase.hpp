#pragma once
#include "ModuloTiposEleitorais.h"
#include "ecourna/api/asn/iconversorasn.hpp"
#include "ecourna/app/dados/cfaseid.hpp"

namespace ecourna::app::dados::asn {

class CConversorFase : public api::asn::IConversorASN<ModuloTiposEleitorais::Fase, CFaseID> {
protected:
    TEntidade DoConverte(const TDado& dado) const override;
    TDado DoDeconverte(const TEntidade& entidade) const override;
};

} // namespace ecourna::app::dados::asn
