#include "ecourna/app/dados/asn/parametrizacaourna/cconversorlabelparametrizado.hpp"

#include "ecourna/app/dados/asn/parametrizacaourna/easnparametrizacaournaerror.hpp"

namespace ecourna::app::dados::asn {

namespace {

const std::string ERRO_GENERO_LABEL = "Gênero do label inválido.";

} // namespace

// (original lines 13-26: content not recovered)
#line 27
CConversorLabelParametrizado::TEntidade CConversorLabelParametrizado::DoConverte(const TDado& dado) const
{
    TEntidade entidade;
    entidade.set_curto() = dado.GetCurto();
    entidade.set_curtoPlural() = dado.GetCurtoPlural();
    entidade.set_longo() = dado.GetLongo();
    entidade.set_longoPlural() = dado.GetLongoPlural();
    entidade.set_genero(ConverterGeneroLabel(dado.GetGenero()));
    return entidade;
}

CConversorLabelParametrizado::TDado CConversorLabelParametrizado::DoDeconverte(const TEntidade& entidade) const
{
    return CLabelParametrizado(DesconverterGeneroLabel(entidade), entidade.get_longo(), entidade.get_curto(),
                               entidade.get_longoPlural(), entidade.get_curtoPlural());
}

ModuloParametrizacaoUrna::GeneroLabel::NamedNumber CConversorLabelParametrizado::ConverterGeneroLabel(
    CLabelParametrizado::EGeneroLabel genero) const
{
    switch (genero) {
        case CLabelParametrizado::EGL_NEUTRO:
            return ModuloParametrizacaoUrna::GeneroLabel::neutro;
        case CLabelParametrizado::EGL_MASCULINO:
            return ModuloParametrizacaoUrna::GeneroLabel::masculino;
        case CLabelParametrizado::EGL_FEMININO:
            return ModuloParametrizacaoUrna::GeneroLabel::feminino;
    }
    throw CAsnParametrizacaoUrnaError(EAPUE_GENERO_LABEL_INVALIDO, ERRO_GENERO_LABEL);
}

// (original lines 58-60: content not recovered)
#line 61
CLabelParametrizado::EGeneroLabel CConversorLabelParametrizado::DesconverterGeneroLabel(const TEntidade& entidade) const
{
    switch (entidade.get_genero()) {
        case ModuloParametrizacaoUrna::GeneroLabel::neutro:
            return CLabelParametrizado::EGL_NEUTRO;
        case ModuloParametrizacaoUrna::GeneroLabel::masculino:
            return CLabelParametrizado::EGL_MASCULINO;
        case ModuloParametrizacaoUrna::GeneroLabel::feminino:
            return CLabelParametrizado::EGL_FEMININO;
        case ModuloParametrizacaoUrna::GeneroLabel::unknownEnumeration_:
            throw CAsnParametrizacaoUrnaError(EAPUE_GENERO_LABEL_DESCONHECIDO, ERRO_GENERO_LABEL);
        default:
            break;
    }
    throw CAsnParametrizacaoUrnaError(EAPUE_GENERO_LABEL_ENTIDADE_INVALIDO, ERRO_GENERO_LABEL);
}

} // namespace ecourna::app::dados::asn
