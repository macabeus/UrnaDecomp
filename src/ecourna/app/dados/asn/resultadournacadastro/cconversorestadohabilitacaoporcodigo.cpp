#include "ecourna/app/dados/asn/resultadournacadastro/cconversorestadohabilitacaoporcodigo.hpp"

#include "ecourna/app/dados/asn/cconversorregistroidentificacaoeleitor.hpp"
#include "ecourna/app/dados/asn/resultadournacadastro/easnresultadournacadastroerror.hpp"

namespace ecourna::app::dados::asn {

CConversorEstadoHabilitacaoPorCodigo::TEntidade CConversorEstadoHabilitacaoPorCodigo::DoConverte(const TDado& dado) const
{
    CConversorRegistroIdentificacaoEleitor conversorRegistro;
    TEntidade entidade;
    entidade.set_situacaoReconhecimentoMesario(
        ConverteSituacaoReconhecimentoMesario(dado.GetSituacaoReconhecimentoMesario()));
    if (dado.HasIdentificacaoMesario()) {
        entidade.set_identificacaoMesario() = conversorRegistro.Converte(dado.GetIdentificacaoMesario());
    } else {
        entidade.omit_identificacaoMesario();
    }
    return entidade;
}

CConversorEstadoHabilitacaoPorCodigo::TDado CConversorEstadoHabilitacaoPorCodigo::DoDeconverte(
    const TEntidade& entidade) const
{
    CConversorRegistroIdentificacaoEleitor conversorRegistro;
    const CEstadoHabilitacaoPorCodigo::ESituacaoReconhecimentoMesario situacao =
        DeconverteSituacaoReconhecimentoMesario(entidade.get_situacaoReconhecimentoMesario());
    if (entidade.identificacaoMesario_isPresent()) {
        return CEstadoHabilitacaoPorCodigo(situacao, conversorRegistro.Deconverte(entidade.get_identificacaoMesario()));
    }
    return CEstadoHabilitacaoPorCodigo(situacao);
}

// (original lines 34-50: content not recovered)
#line 51
ModuloResultadoUrnaCadastro::SituacaoReconhecimentoMesario
CConversorEstadoHabilitacaoPorCodigo::ConverteSituacaoReconhecimentoMesario(
    CEstadoHabilitacaoPorCodigo::ESituacaoReconhecimentoMesario situacao) const
{
    switch (situacao) {
        case CEstadoHabilitacaoPorCodigo::NaoReconhecido:
            return ModuloResultadoUrnaCadastro::SituacaoReconhecimentoMesario::naoReconhecido;
        case CEstadoHabilitacaoPorCodigo::ReconhecidoDigitalCadastroUrna:
            return ModuloResultadoUrnaCadastro::SituacaoReconhecimentoMesario::reconhecidoDigitalCadastroUrna;
        case CEstadoHabilitacaoPorCodigo::ReconhecidoDigitalRegistroMesarios:
            return ModuloResultadoUrnaCadastro::SituacaoReconhecimentoMesario::reconhecidoDigitalRegistroMesarios;
    }

    throw CAsnResultadoUrnaCadastroError(EARUCE_SITUACAO_RECONHECIMENTO_MESARIO_INVALIDA,
                                         "Situação de habilitação por código inválida.");
}

// (original lines 68-69: content not recovered)
#line 70
CEstadoHabilitacaoPorCodigo::ESituacaoReconhecimentoMesario
CConversorEstadoHabilitacaoPorCodigo::DeconverteSituacaoReconhecimentoMesario(
    ModuloResultadoUrnaCadastro::SituacaoReconhecimentoMesario situacao) const
{
    switch (situacao) {
        case ModuloResultadoUrnaCadastro::SituacaoReconhecimentoMesario::naoReconhecido:
            return CEstadoHabilitacaoPorCodigo::NaoReconhecido;
        case ModuloResultadoUrnaCadastro::SituacaoReconhecimentoMesario::reconhecidoDigitalCadastroUrna:
            return CEstadoHabilitacaoPorCodigo::ReconhecidoDigitalCadastroUrna;
        case ModuloResultadoUrnaCadastro::SituacaoReconhecimentoMesario::reconhecidoDigitalRegistroMesarios:
            return CEstadoHabilitacaoPorCodigo::ReconhecidoDigitalRegistroMesarios;
        default:
            break;
    }

    throw CAsnResultadoUrnaCadastroError(EARUCE_SITUACAO_RECONHECIMENTO_MESARIO_ENTIDADE_INVALIDA,
                                         "Situação de habilitação por código inválida.");
}

} // namespace ecourna::app::dados::asn
