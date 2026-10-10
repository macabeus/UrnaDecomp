#pragma once
#include "ModuloTiposEleitorais.h"
#include "ecourna/api/asn/iconversorasn.hpp"
#include "ecourna/app/dados/tiposbasicos.hpp"

namespace ecourna::app::dados::asn {

class CConversorTurno : public api::asn::IConversorASN<ModuloTiposEleitorais::Turno, TTurno> {
protected:
    TEntidade DoConverte(const TDado& dado) const override;
    TDado DoDeconverte(const TEntidade& entidade) const override;
};

} // namespace ecourna::app::dados::asn
