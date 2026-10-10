#pragma once
#include "ModuloInformacaoMidia.h"
#include "ecourna/api/asn/iconversorasn.hpp"
#include "ecourna/app/dados/midias/cautenticacao.hpp"

namespace ecourna::app::dados::asn {

class CConversorAutenticacao : public api::asn::IConversorASN<ModuloInformacaoMidia::Autenticacao, CAutenticacao> {
protected:
    TEntidade DoConverte(const TDado& dado) const override;
    TDado DoDeconverte(const TEntidade& entidade) const override;
};

} // namespace ecourna::app::dados::asn
