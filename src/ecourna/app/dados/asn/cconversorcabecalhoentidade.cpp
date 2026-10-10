#include "ecourna/app/dados/asn/cconversorcabecalhoentidade.hpp"

#include "ecourna/app/dados/asn/datahoraasn.hpp"
#include "ecourna/app/dados/asn/easnerror.hpp"

namespace ecourna::app::dados::asn {

// (original lines 8-32: content not recovered)
#line 33
ModuloTiposEleitorais::CabecalhoEntidade CConversorCabecalhoEntidade::DoConverte(const CCabecalhoEntidade& dado) const
{
    TEntidade entidade;
    entidade.set_dataGeracao() = ConverteDataHora(dado.GetDataGeracao());
    switch (dado.GetTipoIDEleitoral()) {
        case CCabecalhoEntidade::TIPO_ID_PROCESSO_ELEITORAL:
            entidade.set_idEleitoral().select_idProcessoEleitoral() = dado.GetIDEleitoral();
            break;
        case CCabecalhoEntidade::TIPO_ID_PLEITO:
            entidade.set_idEleitoral().select_idPleito() = dado.GetIDEleitoral();
            break;
        case CCabecalhoEntidade::TIPO_ID_ELEICAO:
            entidade.set_idEleitoral().select_idEleicao() = dado.GetIDEleitoral();
            break;
        case CCabecalhoEntidade::TIPO_ID_INDEFINIDO:
            throw CAsnError(EAE_TIPO_ID_CABECALHO_INDEFINIDO, "Tipo de id de cabeçalho inválido.");
    }
    return entidade;
}

// (original lines 53-78: content not recovered)
#line 79
CCabecalhoEntidade CConversorCabecalhoEntidade::DoDeconverte(const TEntidade& entidade) const
{
    const ModuloTiposEleitorais::IDEleitoral& idEleitoral = entidade.get_idEleitoral();
    const int selecao = idEleitoral.currentSelection();
    if (selecao < ModuloTiposEleitorais::IDEleitoral::idProcessoEleitoral0 ||
        selecao > ModuloTiposEleitorais::IDEleitoral::idEleicao2) {
        throw CAsnError(EAE_TIPO_ID_CABECALHO_INVALIDO, "Tipo de id de cabeçalho inválido.");
    }
    const uedword id = static_cast<const ASN1::INTEGER*>(idEleitoral.getSelection())->getValue();
    return CCabecalhoEntidade(DesconverteDataHora(entidade.get_dataGeracao()), id,
                              static_cast<CCabecalhoEntidade::ETipoIDEleitoral>(selecao));
}

} // namespace ecourna::app::dados::asn
