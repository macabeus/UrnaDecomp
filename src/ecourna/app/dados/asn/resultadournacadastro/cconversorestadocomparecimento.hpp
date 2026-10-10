#pragma once
#include "ModuloResultadoUrnaCadastro.h"
#include "ecourna/api/asn/iconversorasn.hpp"
#include "ecourna/app/dados/resultadournacadastro/cestadocomparecimento.hpp"

namespace ecourna::app::dados::asn {

class CConversorEstadoComparecimento
    : public api::asn::IConversorASN<ModuloResultadoUrnaCadastro::EstadoComparecimento, CEstadoComparecimento> {
protected:
    TEntidade DoConverte(const TDado& dado) const override;
    TDado DoDeconverte(const TEntidade& entidade) const override;

private:
    ModuloResultadoUrnaCadastro::SituacaoComparecimentoEleitor ConverteEstadoComparecimento(
        TDado::ESituacaoComparecimento situacao) const;
    TDado::ESituacaoComparecimento DeconverteEstadoComparecimento(
        ModuloResultadoUrnaCadastro::SituacaoComparecimentoEleitor situacao) const;
    ModuloResultadoUrnaCadastro::SituacaoHabilitacaoAudio ConverteSituacaoHabilitacaoAudio(
        TDado::ESituacaoHabilitacaoAudio situacao) const;
    TDado::ESituacaoHabilitacaoAudio DeconverteSituacaoHabilitacaoAudio(
        ModuloResultadoUrnaCadastro::SituacaoHabilitacaoAudio situacao) const;
};

} // namespace ecourna::app::dados::asn
