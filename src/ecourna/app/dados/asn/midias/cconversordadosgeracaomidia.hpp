#pragma once
#include "ModuloInformacaoMidia.h"
#include "ecourna/api/asn/iconversorasn.hpp"
#include "ecourna/app/dados/midias/cdadosgeracaomidia.hpp"

namespace ecourna::app::dados::asn {

class CConversorDadosGeracaoMidia
    : public api::asn::IConversorASN<ModuloInformacaoMidia::DadosGeracaoMidia, CDadosGeracaoMidia> {
protected:
    TEntidade DoConverte(const TDado& dado) const override;
    TDado DoDeconverte(const TEntidade& entidade) const override;
};

} // namespace ecourna::app::dados::asn
