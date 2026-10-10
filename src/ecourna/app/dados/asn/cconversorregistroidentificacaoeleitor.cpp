#include "ecourna/app/dados/asn/cconversorregistroidentificacaoeleitor.hpp"

#include "ecourna/app/dados/asn/cconversoridentificadoreleitor.hpp"
#include "ecourna/app/dados/cidentificadoreleitor.hpp"

namespace ecourna::app::dados::asn {

CConversorRegistroIdentificacaoEleitor::~CConversorRegistroIdentificacaoEleitor() {}

CConversorRegistroIdentificacaoEleitor::TEntidade CConversorRegistroIdentificacaoEleitor::DoConverte(
    const TDado& dado) const
{
    CConversorIdentificadorEleitor conversorIdentificador;
    TEntidade entidade;
    entidade.set_identificacaoUtilizada() =
        conversorIdentificador.Converte(CIdentificadorEleitor(dado.GetIdentificadorHabilitacao()));
    if (dado.HasIdentificadorPrincipal()) {
        entidade.set_identificacaoPrincipal() =
            conversorIdentificador.Converte(CIdentificadorEleitor(dado.GetIdentificadorPrincipal()));
    } else {
        entidade.omit_identificacaoPrincipal();
    }
    return entidade;
}

CConversorRegistroIdentificacaoEleitor::TDado CConversorRegistroIdentificacaoEleitor::DoDeconverte(
    const TEntidade& entidade) const
{
    CConversorIdentificadorEleitor conversorIdentificador;
    const CIdentificadorEleitor identificadorHabilitacao =
        conversorIdentificador.Deconverte(entidade.get_identificacaoUtilizada());
    if (entidade.identificacaoPrincipal_isPresent()) {
        const CIdentificadorEleitor identificadorPrincipal =
            conversorIdentificador.Deconverte(entidade.get_identificacaoPrincipal());
        return CRegistroIdentificacaoEleitor(identificadorHabilitacao.Get(), identificadorPrincipal.Get());
    }
    return CRegistroIdentificacaoEleitor(identificadorHabilitacao.Get());
}

} // namespace ecourna::app::dados::asn
