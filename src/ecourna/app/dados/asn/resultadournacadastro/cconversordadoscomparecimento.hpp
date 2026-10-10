#pragma once
#include "ModuloResultadoUrnaCadastro.h"
#include "ecourna/api/asn/iconversorasn.hpp"
#include "ecourna/app/dados/resultadournacadastro/cdadoscomparecimento.hpp"

namespace ecourna::app::dados::asn {

class CConversorDadosComparecimento : public api::asn::IConversorASN<ModuloResultadoUrnaCadastro::DadosComparecimento, CDadosComparecimento> {
protected:
    TEntidade DoConverte(const TDado& dado) const override;
    TDado DoDeconverte(const TEntidade& entidade) const override;
};

} // namespace ecourna::app::dados::asn
