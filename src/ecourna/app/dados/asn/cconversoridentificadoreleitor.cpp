#include "ecourna/app/dados/asn/cconversoridentificadoreleitor.hpp"

#include "ecourna/app/dados/asn/easnerror.hpp"

namespace ecourna::app::dados::asn {

// (original lines 7-17: content not recovered)
#line 18
CConversorIdentificadorEleitor::TEntidade CConversorIdentificadorEleitor::DoConverte(const TDado& dado) const
{
    TEntidade entidade;
    switch (dado.Get()->GetTipo()) {
        case TipoIDInscricaoEleitoral:
            entidade.select_numeroInscricao() = dado.Get()->GetNumero();
            break;
        case TipoIDCPF:
            entidade.select_numeroCPF() = dado.Get()->GetNumero();
            break;
        case TipoIDNumeroLivre:
            entidade.select_identificacaoLivre() = dado.Get()->GetNumero();
            break;
    }
    return entidade;
}

CConversorIdentificadorEleitor::TDado CConversorIdentificadorEleitor::DoDeconverte(const TEntidade& entidade) const
{
    switch (entidade.currentSelection()) {
        case TEntidade::numeroInscricao0:
            return CIdentificadorEleitor(TipoIDInscricaoEleitoral, entidade.get_numeroInscricao());
        case TEntidade::numeroCPF1:
            return CIdentificadorEleitor(TipoIDCPF, entidade.get_numeroCPF());
        case TEntidade::identificacaoLivre2:
            return CIdentificadorEleitor(TipoIDNumeroLivre, entidade.get_identificacaoLivre());
        default:
            break;
    }
    throw CAsnError(EAE_IDENTIFICADOR_ELEITOR_INVALIDO, "Identificador do eleitor não definido corretamente.");
}

CConversorIdentificadorEleitor::~CConversorIdentificadorEleitor() {}

} // namespace ecourna::app::dados::asn
