#include "ecourna/app/dados/asn/cconversormunicipiozona.hpp"

namespace ecourna::app::dados::asn {

ModuloTiposEleitorais::MunicipioZona CConversorMunicipioZona::DoConverte(const CMunicipioZona& dado) const
{
    TEntidade entidade;
    entidade.set_municipio(dado.GetMunicipio());
    entidade.set_zona(dado.GetZona());
    return entidade;
}

} // namespace ecourna::app::dados::asn
