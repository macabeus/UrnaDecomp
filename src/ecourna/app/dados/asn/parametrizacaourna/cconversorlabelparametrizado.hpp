#pragma once
#include "ModuloParametrizacaoUrna.h"
#include "ecourna/api/asn/iconversorasn.hpp"
#include "ecourna/app/dados/parametrizacaourna/clabelparametrizado.hpp"

namespace ecourna::app::dados::asn {

class CConversorLabelParametrizado
    : public api::asn::IConversorASN<ModuloParametrizacaoUrna::LabelParametrizado, CLabelParametrizado> {
protected:
    TEntidade DoConverte(const TDado& dado) const override;
    TDado DoDeconverte(const TEntidade& entidade) const override;

private:
    ModuloParametrizacaoUrna::GeneroLabel::NamedNumber ConverterGeneroLabel(CLabelParametrizado::EGeneroLabel genero) const;
    CLabelParametrizado::EGeneroLabel DesconverterGeneroLabel(const TEntidade& entidade) const;
};

} // namespace ecourna::app::dados::asn
