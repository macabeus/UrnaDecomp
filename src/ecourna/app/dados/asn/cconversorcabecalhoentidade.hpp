#pragma once
#include "ModuloTiposEleitorais.h"
#include "ecourna/api/asn/iconversorasn.hpp"
#include "ecourna/app/dados/ccabecalhoentidade.hpp"

namespace ecourna::app::dados::asn {

class CConversorCabecalhoEntidade
    : public api::asn::IConversorASN<ModuloTiposEleitorais::CabecalhoEntidade, CCabecalhoEntidade> {
protected:
    TEntidade DoConverte(const TDado& dado) const override;
    TDado DoDeconverte(const TEntidade& entidade) const override;
};

} // namespace ecourna::app::dados::asn
