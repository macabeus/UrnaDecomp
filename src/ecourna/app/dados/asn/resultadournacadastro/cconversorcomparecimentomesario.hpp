#pragma once
#include "ModuloResultadoUrnaCadastro.h"
#include "ecourna/api/asn/iconversorasn.hpp"
#include "ecourna/app/dados/resultadournacadastro/ccomparecimentomesario.hpp"

namespace ecourna::app::dados::asn {

class CConversorComparecimentoMesario
    : public api::asn::IConversorASN<ModuloResultadoUrnaCadastro::ComparecimentoMesario, CComparecimentoMesario> {
protected:
    TEntidade DoConverte(const TDado& dado) const override;
    TDado DoDeconverte(const TEntidade& entidade) const override;

private:
    ModuloResultadoUrnaCadastro::EstadoColetaDigital ConverteEstadoColetaDigital(
        CComparecimentoMesario::EEstadoColetaDigital estado) const;
    CComparecimentoMesario::EEstadoColetaDigital DeconverteEstadoColetaDigital(
        ModuloResultadoUrnaCadastro::EstadoColetaDigital estado) const;
};

} // namespace ecourna::app::dados::asn
