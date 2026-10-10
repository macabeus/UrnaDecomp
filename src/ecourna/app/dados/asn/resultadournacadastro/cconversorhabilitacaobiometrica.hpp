#pragma once
#include "ModuloResultadoUrnaCadastro.h"
#include "ModuloTiposEleitorais.h"
#include "ecourna/api/asn/iconversorasn.hpp"
#include "ecourna/app/dados/resultadournacadastro/chabilitacaobiometrica.hpp"

namespace ecourna::app::dados::asn {

class CConversorHabilitacaoBiometrica
    : public api::asn::IConversorASN<ModuloResultadoUrnaCadastro::HabilitacaoBiometrica, CHabilitacaoBiometrica> {
protected:
    TEntidade DoConverte(const TDado& dado) const override;
    TDado DoDeconverte(const TEntidade& entidade) const override;

private:
    ModuloResultadoUrnaCadastro::ErroLeituraBiometria ConverteErroLeituraBiometria(TErroLeituraBiometria erro) const;
    TErroLeituraBiometria DeconverteErroLeituraBiometria(const ModuloResultadoUrnaCadastro::ErroLeituraBiometria& erro) const;
    ModuloTiposEleitorais::TipoDedo ConverteTipoDedo(const CDedo::ETipoDedo& dedo) const;
    CDedo::ETipoDedo DeconverteTipoDedo(const ModuloTiposEleitorais::TipoDedo& dedo) const;
};

} // namespace ecourna::app::dados::asn
