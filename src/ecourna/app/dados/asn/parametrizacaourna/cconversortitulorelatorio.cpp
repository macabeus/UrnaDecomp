#include "ecourna/app/dados/asn/parametrizacaourna/cconversortitulorelatorio.hpp"

#include "ecourna/app/dados/asn/parametrizacaourna/easnparametrizacaournaerror.hpp"

namespace ecourna::app::dados::asn {

namespace {

const std::string ERRO_ESTILO_TITULO = "Estilo do título inválido.";
const std::string ERRO_ALINHAMENTO_TITULO = "Alinhamento do título inválido.";

} // namespace

// (original lines 14-22: content not recovered)
#line 23
CConversorTituloRelatorio::TEntidade CConversorTituloRelatorio::DoConverte(const TDado& dado) const
{
    TEntidade entidade;
    entidade.set_alinhamento(ConverterAlinhamentoTitulo(dado.GetAlinhamento()));
    entidade.set_estilo(ConverterEstiloTitulo(dado.GetEstilo()));
    entidade.set_texto() = dado.GetTexto();
    return entidade;
}

CConversorTituloRelatorio::TDado CConversorTituloRelatorio::DoDeconverte(const TEntidade& entidade) const
{
    return CTituloRelatorio(DesconverterAlinhamentoTitulo(entidade), DesconverterEstiloTitulo(entidade),
                            entidade.get_texto());
}

ModuloParametrizacaoUrna::EstiloTitulo::NamedNumber CConversorTituloRelatorio::ConverterEstiloTitulo(
    CTituloRelatorio::EEstiloTitulo estilo) const
{
    switch (estilo) {
        case CTituloRelatorio::EET_NORMAL:
            return ModuloParametrizacaoUrna::EstiloTitulo::normal;
        case CTituloRelatorio::EET_EXPANDIDO:
            return ModuloParametrizacaoUrna::EstiloTitulo::expandido;
    }
    throw CAsnParametrizacaoUrnaError(EAPUE_ESTILO_TITULO_INVALIDO, ERRO_ESTILO_TITULO);
}

// (original lines 50-51: content not recovered)
#line 52
CTituloRelatorio::EEstiloTitulo CConversorTituloRelatorio::DesconverterEstiloTitulo(const TEntidade& entidade) const
{
    switch (entidade.get_estilo()) {
        case ModuloParametrizacaoUrna::EstiloTitulo::expandido:
            return CTituloRelatorio::EET_EXPANDIDO;
        case ModuloParametrizacaoUrna::EstiloTitulo::normal:
            return CTituloRelatorio::EET_NORMAL;
        case ModuloParametrizacaoUrna::EstiloTitulo::unknownEnumeration_:
            throw CAsnParametrizacaoUrnaError(EAPUE_ESTILO_TITULO_DESCONHECIDO, ERRO_ESTILO_TITULO);
        default:
            break;
    }
    throw CAsnParametrizacaoUrnaError(EAPUE_ESTILO_TITULO_ENTIDADE_INVALIDO, ERRO_ESTILO_TITULO);
}

// (original lines 67-68: content not recovered)
#line 69
ModuloParametrizacaoUrna::AlinhamentoTitulo::NamedNumber CConversorTituloRelatorio::ConverterAlinhamentoTitulo(
    CTituloRelatorio::EAlinhamentoTitulo alinhamento) const
{
    switch (alinhamento) {
        case CTituloRelatorio::EAT_ESQUERDO:
            return ModuloParametrizacaoUrna::AlinhamentoTitulo::esquerdo;
        case CTituloRelatorio::EAT_DIREITO:
            return ModuloParametrizacaoUrna::AlinhamentoTitulo::direito;
        case CTituloRelatorio::EAT_CENTRO:
            return ModuloParametrizacaoUrna::AlinhamentoTitulo::centro;
    }
    throw CAsnParametrizacaoUrnaError(EAPUE_ALINHAMENTO_TITULO_INVALIDO, ERRO_ALINHAMENTO_TITULO);
}

// (original lines 83-85: content not recovered)
#line 86
CTituloRelatorio::EAlinhamentoTitulo CConversorTituloRelatorio::DesconverterAlinhamentoTitulo(const TEntidade& entidade) const
{
    switch (entidade.get_alinhamento()) {
        case ModuloParametrizacaoUrna::AlinhamentoTitulo::centro:
            return CTituloRelatorio::EAT_CENTRO;
        case ModuloParametrizacaoUrna::AlinhamentoTitulo::direito:
            return CTituloRelatorio::EAT_DIREITO;
        case ModuloParametrizacaoUrna::AlinhamentoTitulo::esquerdo:
            return CTituloRelatorio::EAT_ESQUERDO;
        case ModuloParametrizacaoUrna::AlinhamentoTitulo::unknownEnumeration_:
            throw CAsnParametrizacaoUrnaError(EAPUE_ALINHAMENTO_TITULO_DESCONHECIDO, ERRO_ALINHAMENTO_TITULO);
        default:
            break;
    }
    throw CAsnParametrizacaoUrnaError(EAPUE_ALINHAMENTO_TITULO_ENTIDADE_INVALIDO, ERRO_ALINHAMENTO_TITULO);
}

} // namespace ecourna::app::dados::asn
