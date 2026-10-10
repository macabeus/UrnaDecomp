#pragma once
#include "ModuloResultadoUrnaCadastro.h"
#include "ecourna/api/asn/iconversorasn.hpp"
#include "ecourna/app/dados/resultadournacadastro/cdadoscomparecimentocifrado.hpp"

namespace ecourna::app::dados::asn {

class CConversorDadosComparecimentoCifrado : public api::asn::IConversorASN<ModuloResultadoUrnaCadastro::DadosComparecimentoCifrado, CDadosComparecimentoCifrado> {
protected:
    TEntidade DoConverte(const TDado& dado) const override;
    TDado DoDeconverte(const TEntidade& entidade) const override;
};

} // namespace ecourna::app::dados::asn
