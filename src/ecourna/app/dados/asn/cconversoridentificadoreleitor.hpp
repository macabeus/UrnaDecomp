#pragma once
#include "ModuloTiposEleitorais.h"
#include "ecourna/api/asn/iconversorasn.hpp"
#include "ecourna/app/dados/cidentificadoreleitor.hpp"

namespace ecourna::app::dados::asn {

class CConversorIdentificadorEleitor
    : public api::asn::IConversorASN<ModuloTiposEleitorais::IdentificadorEleitor, CIdentificadorEleitor> {
public:
    // Out of line: a local CConversorIdentificadorEleitor keeps its callers' calls invokes (fn 9114 invokes the entity
    // constructor); binaryen inlines the empty body afterwards.
    ~CConversorIdentificadorEleitor() override;

protected:
    TEntidade DoConverte(const TDado& dado) const override;
    TDado DoDeconverte(const TEntidade& entidade) const override;
};

} // namespace ecourna::app::dados::asn
