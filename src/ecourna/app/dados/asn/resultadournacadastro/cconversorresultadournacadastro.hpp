#pragma once
#include "ModuloResultadoUrnaCadastro.h"
#include "ModuloTiposEcoUrna.h"
#include "ecourna/api/asn/iconversorasn.hpp"
#include "ecourna/app/dados/resultadournacadastro/cresultadournacadastro.hpp"

namespace ecourna::app::dados::asn {

class CConversorResultadoUrnaCadastro
    : public api::asn::IConversorASN<ModuloResultadoUrnaCadastro::EntidadeResultadoUrnaCadastro, CResultadoUrnaCadastro> {
protected:
    TEntidade DoConverte(const TDado& dado) const override;
    TDado DoDeconverte(const TEntidade& entidade) const override;

private:
    ModuloTiposEcoUrna::SituacaoArquivo ConverteSituacaoArquivo(CResultadoUrnaCadastro::ESituacaoArquivo situacao) const;
    CResultadoUrnaCadastro::ESituacaoArquivo DeconverteSituacaoArquivo(ModuloTiposEcoUrna::SituacaoArquivo situacao) const;
};

} // namespace ecourna::app::dados::asn
