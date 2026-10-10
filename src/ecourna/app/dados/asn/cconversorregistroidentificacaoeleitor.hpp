#pragma once
#include "ModuloTiposEcoUrna.h"
#include "ecourna/api/asn/iconversorasn.hpp"
#include "ecourna/app/dados/cregistroidentificacaoeleitor.hpp"

namespace ecourna::app::dados::asn {

class CConversorRegistroIdentificacaoEleitor
    : public api::asn::IConversorASN<ModuloTiposEcoUrna::RegistroIdentificacaoEleitor, CRegistroIdentificacaoEleitor> {
public:
    // Out of line: its users' calls stay invokes while a local converter is alive (fns 9075, 9110 invoke the entity
    // constructor; 9073 invokes its Deconverte helper); binaryen inlines the empty body afterwards.
    ~CConversorRegistroIdentificacaoEleitor() override;

protected:
    TEntidade DoConverte(const TDado& dado) const override;
    TDado DoDeconverte(const TEntidade& entidade) const override;
};

} // namespace ecourna::app::dados::asn
