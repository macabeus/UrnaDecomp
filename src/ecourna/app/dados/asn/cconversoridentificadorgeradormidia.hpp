#pragma once
#include "ModuloTiposEcoUrna.h"
#include "ecourna/api/asn/iconversorasn.hpp"
#include "ecourna/app/dados/cidentificadorgeradormidia.hpp"

namespace ecourna::app::dados::asn {

class CConversorIdentificadorGeradorMidia
    : public api::asn::IConversorASN<ModuloTiposEcoUrna::IdentificadorGeradorMidia, CIdentificadorGeradorMidia> {
public:
    ~CConversorIdentificadorGeradorMidia() override;

protected:
    TEntidade DoConverte(const TDado& dado) const override;
    TDado DoDeconverte(const TEntidade& entidade) const override;
};

} // namespace ecourna::app::dados::asn
