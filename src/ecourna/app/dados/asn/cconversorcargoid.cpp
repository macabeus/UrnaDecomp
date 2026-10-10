#include "ecourna/app/dados/asn/cconversorcargoid.hpp"

#include "ecourna/app/dados/asn/easnerror.hpp"

namespace ecourna::app::dados::asn {

// (original lines 7-9: content not recovered)
#line 10
namespace {

using TCargoConstitucional = ModuloTiposEleitorais::CodigoCargoConsulta::cargoConstitucional::value_type;

// The first code of a free consultation post (numeroCargoConsultaLivre is INTEGER (25..99)).
const TCargoID::TValor CODIGO_CARGO_CONSULTA_LIVRE = 25;

} // namespace

ModuloTiposEleitorais::CodigoCargoConsulta CConversorCargoID::DoConverte(const TCargoID& dado) const
{
    TEntidade entidade;
    if (dado >= CODIGO_CARGO_CONSULTA_LIVRE) {
        entidade.select_numeroCargoConsultaLivre() = dado;
    } else {
        switch (dado) {
        case 1:
            entidade.select_cargoConstitucional() = TCargoConstitucional::presidente;
            break;
        case 2:
            entidade.select_cargoConstitucional() = TCargoConstitucional::vicePresidente;
            break;
        case 3:
            entidade.select_cargoConstitucional() = TCargoConstitucional::governador;
            break;
        case 4:
            entidade.select_cargoConstitucional() = TCargoConstitucional::viceGovernador;
            break;
        case 5:
            entidade.select_cargoConstitucional() = TCargoConstitucional::senador;
            break;
        case 6:
            entidade.select_cargoConstitucional() = TCargoConstitucional::deputadoFederal;
            break;
        case 7:
            entidade.select_cargoConstitucional() = TCargoConstitucional::deputadoEstadual;
            break;
        case 8:
            entidade.select_cargoConstitucional() = TCargoConstitucional::deputadoDistrital;
            break;
        case 9:
            entidade.select_cargoConstitucional() = TCargoConstitucional::primeiroSuplenteSenador;
            break;
        case 10:
            entidade.select_cargoConstitucional() = TCargoConstitucional::segundoSuplenteSenador;
            break;
        case 11:
            entidade.select_cargoConstitucional() = TCargoConstitucional::prefeito;
            break;
        case 12:
            entidade.select_cargoConstitucional() = TCargoConstitucional::vicePrefeito;
            break;
        case 13:
            entidade.select_cargoConstitucional() = TCargoConstitucional::vereador;
            break;
        default:
            throw CAsnError(EAE_CODIGO_CARGO_INVALIDO, "Código de cargo inválido.");
        }
    }
    return entidade;
}

CConversorCargoID::TDado CConversorCargoID::DoDeconverte(const TEntidade& entidade) const
{
    switch (entidade.currentSelection()) {
    case TEntidade::cargoConstitucional0:
        return entidade.get_cargoConstitucional().asInt();
    case TEntidade::numeroCargoConsultaLivre1:
        return entidade.get_numeroCargoConsultaLivre().getValue();
    }
    throw CAsnError(EAE_ESTRUTURA_CODIGO_CARGO_INVALIDA, "Estrutura do código de cargo inválida.");
}

} // namespace ecourna::app::dados::asn
