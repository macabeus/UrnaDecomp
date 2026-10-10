#include "ecourna/app/dados/asn/resultadournacadastro/cconversorapresentacaofotoeleitor.hpp"

#include "ecourna/app/dados/asn/resultadournacadastro/easnresultadournacadastroerror.hpp"

namespace ecourna::app::dados::asn {

CConversorApresentacaoFotoEleitor::TEntidade CConversorApresentacaoFotoEleitor::DoConverte(const TDado& dado) const
{
    TEntidade entidade;
    entidade.set_estadoApresentacao(ConverteEstado(dado.GetEstado()));
    entidade.set_resultadoApresentacao(ConverteResultado(dado.GetResultado()));
    return entidade;
}

CConversorApresentacaoFotoEleitor::TDado CConversorApresentacaoFotoEleitor::DoDeconverte(const TEntidade& entidade) const
{
    return CApresentacaoFotoEleitor(DeconverteEstado(entidade.get_estadoApresentacao()),
                                    DeconverteResultado(entidade.get_resultadoApresentacao()));
}

// (original lines 21-31: content not recovered)
#line 32
ModuloResultadoUrnaCadastro::EstadoApresentacaoFotoEleitor CConversorApresentacaoFotoEleitor::ConverteEstado(
    CApresentacaoFotoEleitor::EEstado estado)
{
    switch (estado) {
        case CApresentacaoFotoEleitor::EleitorSemFoto:
            return ModuloResultadoUrnaCadastro::EstadoApresentacaoFotoEleitor::eleitorSemFoto;
        case CApresentacaoFotoEleitor::Apresentada:
            return ModuloResultadoUrnaCadastro::EstadoApresentacaoFotoEleitor::apresentada;
        case CApresentacaoFotoEleitor::NaoApresentadaPorErro:
            return ModuloResultadoUrnaCadastro::EstadoApresentacaoFotoEleitor::naoApresentadaPorErro;
    }

    throw CAsnResultadoUrnaCadastroError(EARUCE_ESTADO_APRESENTACAO_FOTO_INVALIDO,
                                         "Estado de apresentação da foto do eleitor inválido.");
}

CApresentacaoFotoEleitor::EEstado CConversorApresentacaoFotoEleitor::DeconverteEstado(
    ModuloResultadoUrnaCadastro::EstadoApresentacaoFotoEleitor estado)
{
    switch (estado) {
        case ModuloResultadoUrnaCadastro::EstadoApresentacaoFotoEleitor::eleitorSemFoto:
            return CApresentacaoFotoEleitor::EleitorSemFoto;
        case ModuloResultadoUrnaCadastro::EstadoApresentacaoFotoEleitor::apresentada:
            return CApresentacaoFotoEleitor::Apresentada;
        case ModuloResultadoUrnaCadastro::EstadoApresentacaoFotoEleitor::naoApresentadaPorErro:
            return CApresentacaoFotoEleitor::NaoApresentadaPorErro;
        default:
            break;
    }

    throw CAsnResultadoUrnaCadastroError(EARUCE_ESTADO_APRESENTACAO_FOTO_ENTIDADE_INVALIDO,
                                         "Estado de apresentação da foto do eleitor inválido.");
}

ModuloResultadoUrnaCadastro::ResultadoApresentacaoFotoEleitor CConversorApresentacaoFotoEleitor::ConverteResultado(
    CApresentacaoFotoEleitor::EResultado resultado)
{
    switch (resultado) {
        case CApresentacaoFotoEleitor::SemErro:
            return ModuloResultadoUrnaCadastro::ResultadoApresentacaoFotoEleitor::semErro;
        case CApresentacaoFotoEleitor::ErroFormato:
            return ModuloResultadoUrnaCadastro::ResultadoApresentacaoFotoEleitor::erroFormato;
        case CApresentacaoFotoEleitor::ErroFormacaoImagem:
            return ModuloResultadoUrnaCadastro::ResultadoApresentacaoFotoEleitor::erroFormacaoImagem;
        case CApresentacaoFotoEleitor::ErroResolucao:
            return ModuloResultadoUrnaCadastro::ResultadoApresentacaoFotoEleitor::erroResolucao;
        case CApresentacaoFotoEleitor::ErroCompressao:
            return ModuloResultadoUrnaCadastro::ResultadoApresentacaoFotoEleitor::erroCompressao;
        case CApresentacaoFotoEleitor::ErroProfundidadeCores:
            return ModuloResultadoUrnaCadastro::ResultadoApresentacaoFotoEleitor::erroProfundidadeCores;
        case CApresentacaoFotoEleitor::ErroApresentacao:
            return ModuloResultadoUrnaCadastro::ResultadoApresentacaoFotoEleitor::erroApresentacao;
        case CApresentacaoFotoEleitor::ErroDecifrandoBuffer:
            return ModuloResultadoUrnaCadastro::ResultadoApresentacaoFotoEleitor::erroDecifrandoBuffer;
        case CApresentacaoFotoEleitor::ErroDesconhecidoDeCriptografia:
            return ModuloResultadoUrnaCadastro::ResultadoApresentacaoFotoEleitor::erroDesconhecidoDeCriptografia;
    }

    throw CAsnResultadoUrnaCadastroError(EARUCE_RESULTADO_APRESENTACAO_FOTO_INVALIDO,
                                         "Resultado da apresentação da foto do eleitor inválido.");
}

CApresentacaoFotoEleitor::EResultado CConversorApresentacaoFotoEleitor::DeconverteResultado(
    ModuloResultadoUrnaCadastro::ResultadoApresentacaoFotoEleitor resultado)
{
    switch (resultado) {
        case ModuloResultadoUrnaCadastro::ResultadoApresentacaoFotoEleitor::semErro:
            return CApresentacaoFotoEleitor::SemErro;
        case ModuloResultadoUrnaCadastro::ResultadoApresentacaoFotoEleitor::erroFormato:
            return CApresentacaoFotoEleitor::ErroFormato;
        case ModuloResultadoUrnaCadastro::ResultadoApresentacaoFotoEleitor::erroFormacaoImagem:
            return CApresentacaoFotoEleitor::ErroFormacaoImagem;
        case ModuloResultadoUrnaCadastro::ResultadoApresentacaoFotoEleitor::erroResolucao:
            return CApresentacaoFotoEleitor::ErroResolucao;
        case ModuloResultadoUrnaCadastro::ResultadoApresentacaoFotoEleitor::erroCompressao:
            return CApresentacaoFotoEleitor::ErroCompressao;
        case ModuloResultadoUrnaCadastro::ResultadoApresentacaoFotoEleitor::erroProfundidadeCores:
            return CApresentacaoFotoEleitor::ErroProfundidadeCores;
        case ModuloResultadoUrnaCadastro::ResultadoApresentacaoFotoEleitor::erroApresentacao:
            return CApresentacaoFotoEleitor::ErroApresentacao;
        case ModuloResultadoUrnaCadastro::ResultadoApresentacaoFotoEleitor::erroDecifrandoBuffer:
            return CApresentacaoFotoEleitor::ErroDecifrandoBuffer;
        case ModuloResultadoUrnaCadastro::ResultadoApresentacaoFotoEleitor::erroDesconhecidoDeCriptografia:
            return CApresentacaoFotoEleitor::ErroDesconhecidoDeCriptografia;
        default:
            break;
    }

    throw CAsnResultadoUrnaCadastroError(EARUCE_RESULTADO_APRESENTACAO_FOTO_ENTIDADE_INVALIDO,
                                         "Resultado da apresentação da foto do eleitor inválido.");
}

} // namespace ecourna::app::dados::asn
