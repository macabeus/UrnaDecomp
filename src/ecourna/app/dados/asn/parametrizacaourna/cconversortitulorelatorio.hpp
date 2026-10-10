#pragma once
#include "ModuloParametrizacaoUrna.h"
#include "ecourna/api/asn/iconversorasn.hpp"
#include "ecourna/app/dados/parametrizacaourna/ctitulorelatorio.hpp"

namespace ecourna::app::dados::asn {

class CConversorTituloRelatorio
    : public api::asn::IConversorASN<ModuloParametrizacaoUrna::TituloRelatorio, CTituloRelatorio> {
protected:
    TEntidade DoConverte(const TDado& dado) const override;
    TDado DoDeconverte(const TEntidade& entidade) const override;

private:
    ModuloParametrizacaoUrna::EstiloTitulo::NamedNumber ConverterEstiloTitulo(CTituloRelatorio::EEstiloTitulo estilo) const;
    CTituloRelatorio::EEstiloTitulo DesconverterEstiloTitulo(const TEntidade& entidade) const;
    ModuloParametrizacaoUrna::AlinhamentoTitulo::NamedNumber ConverterAlinhamentoTitulo(
        CTituloRelatorio::EAlinhamentoTitulo alinhamento) const;
    CTituloRelatorio::EAlinhamentoTitulo DesconverterAlinhamentoTitulo(const TEntidade& entidade) const;
};

} // namespace ecourna::app::dados::asn
