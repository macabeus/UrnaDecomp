#pragma once
#include "ModuloTiposEleitorais.h"
#include "ecourna/api/asn/iconversorasn.hpp"
#include "ecourna/app/dados/cfoto.hpp"

namespace ecourna::app::dados::asn {

class CConversorFoto : public api::asn::IConversorASN<ModuloTiposEleitorais::Foto, CFoto> {
protected:
    TEntidade DoConverte(const TDado& dado) const override;
    TDado DoDeconverte(const TEntidade& entidade) const override;

private:
    ModuloTiposEleitorais::FormatoImagem::NamedNumber ConverteFormatoImagem(CFoto::EFormatoImagem formato) const;
    CFoto::EFormatoImagem DeconverteFormatoImagem(ModuloTiposEleitorais::FormatoImagem::NamedNumber formato) const;
};

} // namespace ecourna::app::dados::asn
