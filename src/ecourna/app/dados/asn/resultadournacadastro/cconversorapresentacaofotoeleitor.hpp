#pragma once
#include "ModuloResultadoUrnaCadastro.h"
#include "ecourna/api/asn/iconversorasn.hpp"
#include "ecourna/app/dados/resultadournacadastro/capresentacaofotoeleitor.hpp"

namespace ecourna::app::dados::asn {

class CConversorApresentacaoFotoEleitor
    : public api::asn::IConversorASN<ModuloResultadoUrnaCadastro::ApresentacaoFotoEleitor, CApresentacaoFotoEleitor> {
protected:
    TEntidade DoConverte(const TDado& dado) const override;
    TDado DoDeconverte(const TEntidade& entidade) const override;

private:
    static ModuloResultadoUrnaCadastro::EstadoApresentacaoFotoEleitor ConverteEstado(CApresentacaoFotoEleitor::EEstado estado);
    static CApresentacaoFotoEleitor::EEstado DeconverteEstado(ModuloResultadoUrnaCadastro::EstadoApresentacaoFotoEleitor estado);
    static ModuloResultadoUrnaCadastro::ResultadoApresentacaoFotoEleitor ConverteResultado(
        CApresentacaoFotoEleitor::EResultado resultado);
    static CApresentacaoFotoEleitor::EResultado DeconverteResultado(
        ModuloResultadoUrnaCadastro::ResultadoApresentacaoFotoEleitor resultado);
};

} // namespace ecourna::app::dados::asn
