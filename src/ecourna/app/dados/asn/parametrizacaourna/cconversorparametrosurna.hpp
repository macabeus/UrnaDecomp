#pragma once
#include "ModuloParametrizacaoUrna.h"
#include "ecourna/api/asn/iconversorasn.hpp"
#include "ecourna/app/dados/parametrizacaourna/cparametrosurna.hpp"

namespace ecourna::app::dados::asn {

// Used only as a local of CConversorParametrizacaoUrna: the target has no vtable for it (its DoConverte/DoDeconverte,
// fns 9164/9156, are called directly).
class CConversorParametrosUrna
    : public api::asn::IConversorASN<ModuloParametrizacaoUrna::ParametrosUrna, CParametrosUrna> {
protected:
    TEntidade DoConverte(const TDado& dado) const override;
    TDado DoDeconverte(const TEntidade& entidade) const override;

private:
    ModuloParametrizacaoUrna::Cabecalho ConverterCabecalho(const CParametrosUrna::TVectorTituloRelatorio& titulos) const;
    CParametrosUrna::TVectorTituloRelatorio DesconverterCabecalho(const ModuloParametrizacaoUrna::Cabecalho& cabecalho) const;
    ModuloParametrizacaoUrna::FormaSuspenderComVoto ConverterFormaSuspenderComVoto(
        CParametrosUrna::EFormaSuspenderComVoto forma) const;
    CParametrosUrna::EFormaSuspenderComVoto DesconverterFormaSuspenderComVoto(const TEntidade& entidade) const;
    ModuloParametrizacaoUrna::FormaSuspenderSemVoto ConverterFormaSuspenderSemVoto(
        CParametrosUrna::EFormaSuspenderSemVoto forma) const;
    CParametrosUrna::EFormaSuspenderSemVoto DesconverterFormaSuspenderSemVoto(const TEntidade& entidade) const;
};

} // namespace ecourna::app::dados::asn
