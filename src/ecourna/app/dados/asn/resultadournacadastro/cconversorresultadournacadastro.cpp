#include "ecourna/app/dados/asn/resultadournacadastro/cconversorresultadournacadastro.hpp"

#include "ecourna/app/dados/asn/cconversorcabecalhoentidade.hpp"
#include "ecourna/app/dados/asn/cconversorfase.hpp"
#include "ecourna/app/dados/asn/resultadournacadastro/cconversordadoscomparecimento.hpp"
#include "ecourna/app/dados/asn/resultadournacadastro/cconversordadoscomparecimentocifrado.hpp"
#include "ecourna/app/dados/asn/resultadournacadastro/easnresultadournacadastroerror.hpp"

namespace ecourna::app::dados::asn {

// (original lines 11-25: content not recovered)
#line 26
CConversorResultadoUrnaCadastro::TEntidade CConversorResultadoUrnaCadastro::DoConverte(const TDado& dado) const
{
    CConversorCabecalhoEntidade conversorCabecalho;
    CConversorDadosComparecimento conversorDadosComparecimento;
    CConversorDadosComparecimentoCifrado conversorDadosComparecimentoCifrado;
    CConversorFase conversorFase;

    TEntidade entidade;
    entidade.set_cabecalho() = conversorCabecalho.Converte(dado.GetCabecalho());
    entidade.set_fase() = conversorFase.Converte(dado.GetFase());
    entidade.set_situacao() = ConverteSituacaoArquivo(dado.GetSituacao());
    entidade.set_versaoVotacao() = dado.GetVersaoVotacao();

    if (dado.HasDadosComparecimento()) {
        const CDadosComparecimento dadosComparecimento = dado.GetDadosComparecimento();
        entidade.set_infoDadosComparecimento().select_dadosComparecimento() =
            conversorDadosComparecimento.Converte(dadosComparecimento);
    } else if (dado.HasDadosComparecimentoCifrado()) {
        const CDadosComparecimentoCifrado dadosComparecimentoCifrado = dado.GetDadosComparecimentoCifrado();
        entidade.set_infoDadosComparecimento().select_dadosComparecimentoCifrado() =
            conversorDadosComparecimentoCifrado.Converte(dadosComparecimentoCifrado);
    } else {
        CAsnResultadoUrnaCadastroError(EARUCE_DADOS_COMPARECIMENTO_MAL_FORMADOS,
                                       "Classe de dados de comparecimento mal formada.");
    }
    return entidade;
}

// (original lines 54-58: content not recovered)
#line 59
CConversorResultadoUrnaCadastro::TDado CConversorResultadoUrnaCadastro::DoDeconverte(const TEntidade& entidade) const
{
    CConversorCabecalhoEntidade conversorCabecalho;
    CConversorDadosComparecimento conversorDadosComparecimento;
    CConversorDadosComparecimentoCifrado conversorDadosComparecimentoCifrado;
    CConversorFase conversorFase;

    const CCabecalhoEntidade cabecalho = conversorCabecalho.Deconverte(entidade.get_cabecalho());
    const CFaseID fase = conversorFase.Deconverte(entidade.get_fase());
    const std::string versaoVotacao = entidade.get_versaoVotacao();
    const TDado::ESituacaoArquivo situacao = DeconverteSituacaoArquivo(entidade.get_situacao());

    const TEntidade::infoDadosComparecimento::value_type& infoDadosComparecimento =
        entidade.get_infoDadosComparecimento();
    switch (infoDadosComparecimento.currentSelection()) {
        case TEntidade::infoDadosComparecimento::value_type::dadosComparecimento::eid: {
            const CDadosComparecimento dadosComparecimento =
                conversorDadosComparecimento.Deconverte(infoDadosComparecimento.get_dadosComparecimento());
            return TDado(cabecalho, fase, versaoVotacao, situacao, dadosComparecimento);
        }
        case TEntidade::infoDadosComparecimento::value_type::dadosComparecimentoCifrado::eid: {
            const CDadosComparecimentoCifrado dadosComparecimentoCifrado =
                conversorDadosComparecimentoCifrado.Deconverte(infoDadosComparecimento.get_dadosComparecimentoCifrado());
            return TDado(cabecalho, fase, versaoVotacao, situacao, dadosComparecimentoCifrado);
        }
        default:
            break;
    }

    throw CAsnResultadoUrnaCadastroError(EARUCE_DADOS_COMPARECIMENTO_NAO_DEFINIDOS,
                                         "Não foram definidos os dados de comparecimento.");
}

ModuloTiposEcoUrna::SituacaoArquivo CConversorResultadoUrnaCadastro::ConverteSituacaoArquivo(
    CResultadoUrnaCadastro::ESituacaoArquivo situacao) const
{
    switch (situacao) {
        case CResultadoUrnaCadastro::ArquivoFinal:
            return ModuloTiposEcoUrna::SituacaoArquivo::arquivoFinal;
        case CResultadoUrnaCadastro::ArquivoParcial:
            return ModuloTiposEcoUrna::SituacaoArquivo::arquivoParcial;
    }

    throw CAsnResultadoUrnaCadastroError(EARUCE_SITUACAO_ARQUIVO_INVALIDA, "Situação do arquivo inválida.");
}


CResultadoUrnaCadastro::ESituacaoArquivo CConversorResultadoUrnaCadastro::DeconverteSituacaoArquivo(
    ModuloTiposEcoUrna::SituacaoArquivo situacao) const
{
    switch (situacao) {
        case ModuloTiposEcoUrna::SituacaoArquivo::arquivoFinal:
            return CResultadoUrnaCadastro::ArquivoFinal;
        case ModuloTiposEcoUrna::SituacaoArquivo::arquivoParcial:
            return CResultadoUrnaCadastro::ArquivoParcial;
        case ModuloTiposEcoUrna::SituacaoArquivo::unknownEnumeration_:

            throw CAsnResultadoUrnaCadastroError(EARUCE_SITUACAO_ARQUIVO_DESCONHECIDA, "Situação arquivo inválida.");
        default:
            break;
    }
    throw CAsnResultadoUrnaCadastroError(EARUCE_SITUACAO_ARQUIVO_ENTIDADE_INVALIDA, "Situação arquivo inválida.");
}

} // namespace ecourna::app::dados::asn
