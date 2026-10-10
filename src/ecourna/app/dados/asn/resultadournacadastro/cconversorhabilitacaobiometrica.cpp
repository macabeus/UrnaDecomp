#include "ecourna/app/dados/asn/resultadournacadastro/cconversorhabilitacaobiometrica.hpp"

#include "ecourna/app/dados/asn/resultadournacadastro/cconversorestadohabilitacaoporcodigo.hpp"
#include "ecourna/app/dados/asn/resultadournacadastro/easnresultadournacadastroerror.hpp"

namespace ecourna::app::dados::asn {

CConversorHabilitacaoBiometrica::TEntidade CConversorHabilitacaoBiometrica::DoConverte(const TDado& dado) const
{
    TEntidade entidade;
    entidade.set_tentativasHabilitacaoBiometrica() = dado.GetTentativas();
    entidade.set_dedoHabilitado() = ConverteTipoDedo(dado.GetDedoHabilitado());
    entidade.set_erroLeituraBiometria(ConverteErroLeituraBiometria(dado.GetErroLeituraBiometria()));
    if (dado.GetUltimoScore() != 0) {
        entidade.set_ultimoScore(dado.GetUltimoScore());
    } else {
        entidade.omit_ultimoScore();
    }
    if (dado.HasEstadoHabilitacaoPorCodigo()) {
        CConversorEstadoHabilitacaoPorCodigo conversorEstadoHabilitacao;
        entidade.set_habilitacaoPorCodigo() = conversorEstadoHabilitacao.Converte(dado.GetEstadoHabilitacaoPorCodigo());
    } else {
        entidade.omit_habilitacaoPorCodigo();
    }
    return entidade;
}

// (original lines 28-41: content not recovered)
#line 42
CConversorHabilitacaoBiometrica::TDado CConversorHabilitacaoBiometrica::DoDeconverte(const TEntidade& entidade) const
{
    const uedword tentativas = entidade.get_tentativasHabilitacaoBiometrica().getValue();
    const CDedo::ETipoDedo dedo = DeconverteTipoDedo(entidade.get_dedoHabilitado());
    const TErroLeituraBiometria erro = DeconverteErroLeituraBiometria(entidade.get_erroLeituraBiometria());
    const TScoreHabilitacao ultimoScore =
        entidade.ultimoScore_isPresent() ? TScoreHabilitacao(entidade.get_ultimoScore().getValue()) : TScoreHabilitacao(0);

    if (entidade.get_dedoHabilitado() != ModuloTiposEleitorais::TipoDedo::naoIdentificado) {
        if (entidade.habilitacaoPorCodigo_isPresent() ||
            entidade.get_erroLeituraBiometria() != ModuloResultadoUrnaCadastro::ErroLeituraBiometria::semErro) {
            throw CAsnResultadoUrnaCadastroError(EARUCE_HABILITACAO_BIOMETRICA_MAL_FORMADA,
                                                 "Estrutura de dados mal formada.");
        }
        return CHabilitacaoBiometrica(tentativas, ultimoScore, dedo, erro);
    }


    if (!entidade.habilitacaoPorCodigo_isPresent()) {
        throw CAsnResultadoUrnaCadastroError(EARUCE_HABILITACAO_POR_CODIGO_ESPERADA,
                                             "Informação habilitacaoPorCodigo era esperada neste ponto.");
    }
    CConversorEstadoHabilitacaoPorCodigo conversorEstadoHabilitacao;
    return CHabilitacaoBiometrica(tentativas, ultimoScore, dedo, erro,
                                  conversorEstadoHabilitacao.Deconverte(entidade.get_habilitacaoPorCodigo()));
}

// (original lines 69-76: content not recovered)
#line 77
ModuloResultadoUrnaCadastro::ErroLeituraBiometria CConversorHabilitacaoBiometrica::ConverteErroLeituraBiometria(
    TErroLeituraBiometria erro) const
{
    switch (erro) {
        case ErroBioSemErro:
            return ModuloResultadoUrnaCadastro::ErroLeituraBiometria::semErro;
        case ErroBioTipoArquivoInvalido:
            return ModuloResultadoUrnaCadastro::ErroLeituraBiometria::tipoArquivoInvalido;
        case ErroBioIdCriptografiaInvalido:
            return ModuloResultadoUrnaCadastro::ErroLeituraBiometria::idCriptografiaInvalido;
        case ErroBioChaveSecretaVazia:
            return ModuloResultadoUrnaCadastro::ErroLeituraBiometria::chaveSecretaVazia;
        case ErroBioChaveSecaoVazia:
            return ModuloResultadoUrnaCadastro::ErroLeituraBiometria::chaveSecaoVazia;
        case ErroBioConteudoVazio:
            return ModuloResultadoUrnaCadastro::ErroLeituraBiometria::conteudoVazio;
        case ErroBioChaveSecaoComTamanhoInvalido:
            return ModuloResultadoUrnaCadastro::ErroLeituraBiometria::chaveSecaoComTamanhoInvalido;
        case ErroBioChaveSecretaComTamanhoInvalido:
            return ModuloResultadoUrnaCadastro::ErroLeituraBiometria::chaveSecretaComTamanhoInvalido;
        case ErroBioNaoFoiPossivelIniciarADecifragem:
            return ModuloResultadoUrnaCadastro::ErroLeituraBiometria::naoFoiPossivelIniciarADecifragem;
        case ErroBioErroDecifrandoBuffer:
            return ModuloResultadoUrnaCadastro::ErroLeituraBiometria::erroDecifrandoBuffer;
        case ErroBioErroDesconhecidoDeCriptografia:
            return ModuloResultadoUrnaCadastro::ErroLeituraBiometria::erroDesconhecidoDeCriptografia;
        case ErroBioErroFormacaoTemplate:
            return ModuloResultadoUrnaCadastro::ErroLeituraBiometria::erroFormacaoTemplate;
        case ErroBioErroCodificacaoMinuncias:
            return ModuloResultadoUrnaCadastro::ErroLeituraBiometria::erroCodificacaoMinuncias;
        default:
            break;
    }

    throw CAsnResultadoUrnaCadastroError(EARUCE_ERRO_LEITURA_BIOMETRIA_INVALIDO,
                                         "Tipo de erro de biomoetria inválido.");
}

// (original lines 115-127: content not recovered)
#line 128
TErroLeituraBiometria CConversorHabilitacaoBiometrica::DeconverteErroLeituraBiometria(
    const ModuloResultadoUrnaCadastro::ErroLeituraBiometria& erro) const
{
    switch (erro) {
        case ModuloResultadoUrnaCadastro::ErroLeituraBiometria::semErro:
        case ModuloResultadoUrnaCadastro::ErroLeituraBiometria::tipoArquivoInvalido:
        case ModuloResultadoUrnaCadastro::ErroLeituraBiometria::idCriptografiaInvalido:
        case ModuloResultadoUrnaCadastro::ErroLeituraBiometria::chaveSecretaVazia:
        case ModuloResultadoUrnaCadastro::ErroLeituraBiometria::chaveSecaoVazia:
        case ModuloResultadoUrnaCadastro::ErroLeituraBiometria::conteudoVazio:
        case ModuloResultadoUrnaCadastro::ErroLeituraBiometria::chaveSecaoComTamanhoInvalido:
        case ModuloResultadoUrnaCadastro::ErroLeituraBiometria::chaveSecretaComTamanhoInvalido:
        case ModuloResultadoUrnaCadastro::ErroLeituraBiometria::naoFoiPossivelIniciarADecifragem:
        case ModuloResultadoUrnaCadastro::ErroLeituraBiometria::erroDecifrandoBuffer:
        case ModuloResultadoUrnaCadastro::ErroLeituraBiometria::erroDesconhecidoDeCriptografia:
        case ModuloResultadoUrnaCadastro::ErroLeituraBiometria::erroFormacaoTemplate:
        case ModuloResultadoUrnaCadastro::ErroLeituraBiometria::erroCodificacaoMinuncias:
            return CErroLeituraBiometria(static_cast<EErroLeituraBiometria>(erro.asInt()));
        default:
            break;
    }

    throw CAsnResultadoUrnaCadastroError(EARUCE_ERRO_LEITURA_BIOMETRIA_ENTIDADE_INVALIDO,
                                         "Tipo de erro de biomoetria inválido.");
}


ModuloTiposEleitorais::TipoDedo CConversorHabilitacaoBiometrica::ConverteTipoDedo(const CDedo::ETipoDedo& dedo) const
{
    switch (dedo) {
        case CDedo::NaoIdentificado:
            return ModuloTiposEleitorais::TipoDedo::naoIdentificado;
        case CDedo::PolegarDireito:
            return ModuloTiposEleitorais::TipoDedo::polegarDireito;
        case CDedo::IndicadorDireito:
            return ModuloTiposEleitorais::TipoDedo::indicadorDireito;
        case CDedo::MedioDireito:
            return ModuloTiposEleitorais::TipoDedo::medioDireito;
        case CDedo::AnelarDireito:
            return ModuloTiposEleitorais::TipoDedo::anelarDireito;
        case CDedo::MinimoDireito:
            return ModuloTiposEleitorais::TipoDedo::minimoDireito;
        case CDedo::PolegarEsquerdo:
            return ModuloTiposEleitorais::TipoDedo::polegarEsquerdo;
        case CDedo::IndicadorEsquerdo:
            return ModuloTiposEleitorais::TipoDedo::indicadorEsquerdo;
        case CDedo::MedioEsquerdo:
            return ModuloTiposEleitorais::TipoDedo::medioEsquerdo;
        case CDedo::AnelarEsquerdo:
            return ModuloTiposEleitorais::TipoDedo::anelarEsquerdo;
        case CDedo::MinimoEsquerdo:
            return ModuloTiposEleitorais::TipoDedo::minimoEsquerdo;
    }

    throw CAsnResultadoUrnaCadastroError(EARUCE_TIPO_DEDO_INVALIDO, "Tipo dedo inválido.");
}

// (original lines 185-186: content not recovered)
#line 187
CDedo::ETipoDedo CConversorHabilitacaoBiometrica::DeconverteTipoDedo(const ModuloTiposEleitorais::TipoDedo& dedo) const
{
    switch (dedo) {
        case ModuloTiposEleitorais::TipoDedo::naoIdentificado:
            return CDedo::NaoIdentificado;
        case ModuloTiposEleitorais::TipoDedo::polegarDireito:
            return CDedo::PolegarDireito;
        case ModuloTiposEleitorais::TipoDedo::indicadorDireito:
            return CDedo::IndicadorDireito;
        case ModuloTiposEleitorais::TipoDedo::medioDireito:
            return CDedo::MedioDireito;
        case ModuloTiposEleitorais::TipoDedo::anelarDireito:
            return CDedo::AnelarDireito;
        case ModuloTiposEleitorais::TipoDedo::minimoDireito:
            return CDedo::MinimoDireito;
        case ModuloTiposEleitorais::TipoDedo::polegarEsquerdo:
            return CDedo::PolegarEsquerdo;
        case ModuloTiposEleitorais::TipoDedo::indicadorEsquerdo:
            return CDedo::IndicadorEsquerdo;
        case ModuloTiposEleitorais::TipoDedo::medioEsquerdo:
            return CDedo::MedioEsquerdo;
        case ModuloTiposEleitorais::TipoDedo::anelarEsquerdo:
            return CDedo::AnelarEsquerdo;
        case ModuloTiposEleitorais::TipoDedo::minimoEsquerdo:
            return CDedo::MinimoEsquerdo;
        default:
            break;
    }

    throw CAsnResultadoUrnaCadastroError(EARUCE_TIPO_DEDO_ENTIDADE_INVALIDO, "Tipo de dedo inválido.");
}

} // namespace ecourna::app::dados::asn
