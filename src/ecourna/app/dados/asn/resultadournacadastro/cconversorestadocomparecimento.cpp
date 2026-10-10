#include "ecourna/app/dados/asn/resultadournacadastro/cconversorestadocomparecimento.hpp"

#include "ecourna/app/dados/asn/cconversorregistroidentificacaoeleitor.hpp"
#include "ecourna/app/dados/asn/resultadournacadastro/cconversorapresentacaofotoeleitor.hpp"
#include "ecourna/app/dados/asn/resultadournacadastro/cconversorhabilitacaobiometrica.hpp"
#include "ecourna/app/dados/asn/resultadournacadastro/easnresultadournacadastroerror.hpp"

namespace ecourna::app::dados::asn {

CConversorEstadoComparecimento::TEntidade CConversorEstadoComparecimento::DoConverte(const TDado& dado) const
{
    CConversorHabilitacaoBiometrica conversorHabilitacaoBiometrica;
    CConversorRegistroIdentificacaoEleitor conversorRegistro;
    CConversorApresentacaoFotoEleitor conversorApresentacaoFoto;
    TEntidade entidade;
    entidade.set_situacaoComparecimentoEleitor(ConverteEstadoComparecimento(dado.GetSituacaoComparecimento()));
    entidade.set_identificacaoEleitor() = conversorRegistro.Converte(dado.GetIdentificacaoEleitor());
    if (dado.HasApresentacaoFoto()) {
        entidade.set_apresentacaoFoto() = conversorApresentacaoFoto.Converte(dado.GetApresentacaoFoto());
    } else {
        entidade.omit_apresentacaoFoto();
    }
    if (dado.HasSituacaoHabilitacaoAudio()) {
        entidade.set_situacaoHabilitacaoAudio(ConverteSituacaoHabilitacaoAudio(dado.GetSituacaoHabilitacaoAudio()));
    } else {
        entidade.omit_situacaoHabilitacaoAudio();
    }
    if (dado.HasHabilitacaoBiometrica()) {
        entidade.set_habilitacaoBiometrica() = conversorHabilitacaoBiometrica.Converte(dado.GetHabilitacaoBiometrica());
    } else {
        entidade.omit_habilitacaoBiometrica();
    }
    return entidade;
}

// (original lines 36-56: content not recovered)
#line 57
CConversorEstadoComparecimento::TDado CConversorEstadoComparecimento::DoDeconverte(const TEntidade& entidade) const
{
    CConversorRegistroIdentificacaoEleitor conversorRegistro;
    CConversorApresentacaoFotoEleitor conversorApresentacaoFoto;
    const CRegistroIdentificacaoEleitor identificacaoEleitor =
        conversorRegistro.Deconverte(entidade.get_identificacaoEleitor());
    const TDado::ESituacaoComparecimento situacaoComparecimento =
        DeconverteEstadoComparecimento(entidade.get_situacaoComparecimentoEleitor());

    const bool possuiHabilitacaoAudio = entidade.situacaoHabilitacaoAudio_isPresent();
    const bool possuiApresentacaoFoto = entidade.apresentacaoFoto_isPresent();
    const bool possuiHabilitacaoBiometrica = entidade.habilitacaoBiometrica_isPresent();
    if (!possuiApresentacaoFoto && (possuiHabilitacaoAudio || possuiHabilitacaoBiometrica)) {
        throw CAsnResultadoUrnaCadastroError(
            EARUCE_APRESENTACAO_FOTO_NAO_DEFINIDA,
            "Eleitor que votou não possui informações sobre a apresentacao de fotos do eleitor.");
    }
    if (!possuiHabilitacaoAudio && (possuiApresentacaoFoto || possuiHabilitacaoBiometrica)) {
        throw CAsnResultadoUrnaCadastroError(EARUCE_HABILITACAO_AUDIO_NAO_DEFINIDA,
                                             "Eleitor que votou não possui informações sobre habilitação por áudio.");
    }

    if (entidade.habilitacaoBiometrica_isPresent()) {
        CConversorHabilitacaoBiometrica conversorHabilitacaoBiometrica;
        const CHabilitacaoBiometrica habilitacaoBiometrica =
            conversorHabilitacaoBiometrica.Deconverte(entidade.get_habilitacaoBiometrica());
        const TDado::ESituacaoHabilitacaoAudio situacaoHabilitacaoAudio =
            DeconverteSituacaoHabilitacaoAudio(entidade.get_situacaoHabilitacaoAudio());
        const CApresentacaoFotoEleitor apresentacaoFoto = conversorApresentacaoFoto.Deconverte(entidade.get_apresentacaoFoto());
        return TDado(identificacaoEleitor, situacaoComparecimento, apresentacaoFoto, situacaoHabilitacaoAudio,
                     habilitacaoBiometrica);
    }
    if (possuiHabilitacaoAudio) {
        const TDado::ESituacaoHabilitacaoAudio situacaoHabilitacaoAudio =
            DeconverteSituacaoHabilitacaoAudio(entidade.get_situacaoHabilitacaoAudio());
        const CApresentacaoFotoEleitor apresentacaoFoto = conversorApresentacaoFoto.Deconverte(entidade.get_apresentacaoFoto());
        return TDado(identificacaoEleitor, situacaoComparecimento, apresentacaoFoto, situacaoHabilitacaoAudio);
    }
    return TDado(identificacaoEleitor, situacaoComparecimento);
}


ModuloResultadoUrnaCadastro::SituacaoComparecimentoEleitor CConversorEstadoComparecimento::ConverteEstadoComparecimento(
    TDado::ESituacaoComparecimento situacao) const
{
    switch (situacao) {
        case TDado::Faltou:
            return ModuloResultadoUrnaCadastro::SituacaoComparecimentoEleitor::faltou;
        case TDado::NaoVotou:
            return ModuloResultadoUrnaCadastro::SituacaoComparecimentoEleitor::naoVotou;
        case TDado::Votou:
            return ModuloResultadoUrnaCadastro::SituacaoComparecimentoEleitor::votou;
        case TDado::SemCargoParaVotar:
            return ModuloResultadoUrnaCadastro::SituacaoComparecimentoEleitor::semCargoParaVotar;
    }

    throw CAsnResultadoUrnaCadastroError(EARUCE_SITUACAO_COMPARECIMENTO_INVALIDA,
                                         "Situação de comparecimento inválida.");
}

// (original lines 117-118: content not recovered)
#line 119
CConversorEstadoComparecimento::TDado::ESituacaoComparecimento CConversorEstadoComparecimento::DeconverteEstadoComparecimento(
    ModuloResultadoUrnaCadastro::SituacaoComparecimentoEleitor situacao) const
{
    switch (situacao) {
        case ModuloResultadoUrnaCadastro::SituacaoComparecimentoEleitor::faltou:
            return TDado::Faltou;
        case ModuloResultadoUrnaCadastro::SituacaoComparecimentoEleitor::naoVotou:
            return TDado::NaoVotou;
        case ModuloResultadoUrnaCadastro::SituacaoComparecimentoEleitor::votou:
            return TDado::Votou;
        case ModuloResultadoUrnaCadastro::SituacaoComparecimentoEleitor::semCargoParaVotar:
            return TDado::SemCargoParaVotar;
        default:
            break;
    }

    throw CAsnResultadoUrnaCadastroError(EARUCE_SITUACAO_COMPARECIMENTO_ENTIDADE_INVALIDA,
                                         "Situação de comparecimento inválida.");
}


ModuloResultadoUrnaCadastro::SituacaoHabilitacaoAudio CConversorEstadoComparecimento::ConverteSituacaoHabilitacaoAudio(
    TDado::ESituacaoHabilitacaoAudio situacao) const
{
    switch (situacao) {
        case TDado::Automatica:
            return ModuloResultadoUrnaCadastro::SituacaoHabilitacaoAudio::automatica;
        case TDado::PeloMesario:
            return ModuloResultadoUrnaCadastro::SituacaoHabilitacaoAudio::peloMesario;
        case TDado::NaoHabilitado:
            return ModuloResultadoUrnaCadastro::SituacaoHabilitacaoAudio::naoHabilitado;
    }

    throw CAsnResultadoUrnaCadastroError(EARUCE_SITUACAO_HABILITACAO_AUDIO_INVALIDA,
                                         "Situação de habilitação do áudio inválida.");
}


CConversorEstadoComparecimento::TDado::ESituacaoHabilitacaoAudio
CConversorEstadoComparecimento::DeconverteSituacaoHabilitacaoAudio(
    ModuloResultadoUrnaCadastro::SituacaoHabilitacaoAudio situacao) const
{
    switch (situacao) {
        case ModuloResultadoUrnaCadastro::SituacaoHabilitacaoAudio::automatica:
            return TDado::Automatica;
        case ModuloResultadoUrnaCadastro::SituacaoHabilitacaoAudio::peloMesario:
            return TDado::PeloMesario;
        case ModuloResultadoUrnaCadastro::SituacaoHabilitacaoAudio::naoHabilitado:
            return TDado::NaoHabilitado;
        default:
            break;
    }

    throw CAsnResultadoUrnaCadastroError(EARUCE_SITUACAO_HABILITACAO_AUDIO_ENTIDADE_INVALIDA,
                                         "Situação de habilitação do áudio inválida.");
}

} // namespace ecourna::app::dados::asn
