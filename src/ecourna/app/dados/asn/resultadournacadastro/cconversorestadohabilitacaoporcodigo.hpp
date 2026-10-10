#pragma once
#include "ModuloResultadoUrnaCadastro.h"
#include "ecourna/api/asn/iconversorasn.hpp"
#include "ecourna/app/dados/resultadournacadastro/cestadohabilitacaoporcodigo.hpp"

namespace ecourna::app::dados::asn {

class CConversorEstadoHabilitacaoPorCodigo
    : public api::asn::IConversorASN<ModuloResultadoUrnaCadastro::EstadoHabilitacaoPorCodigo,
                                     CEstadoHabilitacaoPorCodigo> {
protected:
    TEntidade DoConverte(const TDado& dado) const override;
    TDado DoDeconverte(const TEntidade& entidade) const override;

private:
    ModuloResultadoUrnaCadastro::SituacaoReconhecimentoMesario ConverteSituacaoReconhecimentoMesario(
        CEstadoHabilitacaoPorCodigo::ESituacaoReconhecimentoMesario situacao) const;
    CEstadoHabilitacaoPorCodigo::ESituacaoReconhecimentoMesario DeconverteSituacaoReconhecimentoMesario(
        ModuloResultadoUrnaCadastro::SituacaoReconhecimentoMesario situacao) const;
};

} // namespace ecourna::app::dados::asn
