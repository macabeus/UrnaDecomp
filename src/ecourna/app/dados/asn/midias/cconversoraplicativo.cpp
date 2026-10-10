#include "ecourna/app/dados/asn/midias/cconversoraplicativo.hpp"

#include "ecourna/app/dados/asn/midias/cconversorautenticacao.hpp"
#include "ecourna/app/dados/asn/midias/easnmidiaserror.hpp"

namespace ecourna::app::dados::asn {

ModuloInformacaoMidia::Aplicativo CConversorAplicativo::DoConverte(const TDado& dado) const
{
    TEntidade entidade;
    entidade.set_tipoAplicativo(ConverteTipoAplicativo(dado.GetTipo()));
    if (dado.PossuiAutenticacao()) {
        CConversorAutenticacao conversorAutenticacao;
        entidade.set_autenticacao() = conversorAutenticacao.Converte(dado.GetAutenticacao());
    } else {
        entidade.omit_autenticacao();
    }
    return entidade;
}

CAplicativo CConversorAplicativo::DoDeconverte(const TEntidade& entidade) const
{
    const CAplicativo::ETipoAplicativo tipo = DeconverteTipoAplicativo(entidade.get_tipoAplicativo());
    if (entidade.autenticacao_isPresent()) {
        CConversorAutenticacao conversorAutenticacao;
        return CAplicativo(tipo, conversorAutenticacao.Deconverte(entidade.get_autenticacao()));
    }
    return CAplicativo(tipo);
}

// (original lines 31-48: content not recovered)
#line 49
ModuloInformacaoMidia::TipoAplicativo CConversorAplicativo::ConverteTipoAplicativo(CAplicativo::ETipoAplicativo tipo) const
{
    switch (tipo) {
        case CAplicativo::TA_VOTA:
            return ModuloInformacaoMidia::TipoAplicativo::vota;
        case CAplicativo::TA_SA:
            return ModuloInformacaoMidia::TipoAplicativo::sa;
        case CAplicativo::TA_RED:
            return ModuloInformacaoMidia::TipoAplicativo::red;
        case CAplicativo::TA_VPP:
            return ModuloInformacaoMidia::TipoAplicativo::vpp;
        case CAplicativo::TA_STE:
            return ModuloInformacaoMidia::TipoAplicativo::ste;
        case CAplicativo::TA_ADH:
            return ModuloInformacaoMidia::TipoAplicativo::adh;
        case CAplicativo::TA_ATUE:
            return ModuloInformacaoMidia::TipoAplicativo::atue;
    }
    throw CAsnMidiasError(EAME_TIPO_APLICATIVO_INVALIDO, "Aplicativo inválido");
}

CAplicativo::ETipoAplicativo CConversorAplicativo::DeconverteTipoAplicativo(ModuloInformacaoMidia::TipoAplicativo tipo) const
{
    switch (tipo) {
        case ModuloInformacaoMidia::TipoAplicativo::vota:
            return CAplicativo::TA_VOTA;
        case ModuloInformacaoMidia::TipoAplicativo::sa:
            return CAplicativo::TA_SA;
        case ModuloInformacaoMidia::TipoAplicativo::red:
            return CAplicativo::TA_RED;
        case ModuloInformacaoMidia::TipoAplicativo::vpp:
            return CAplicativo::TA_VPP;
        case ModuloInformacaoMidia::TipoAplicativo::ste:
            return CAplicativo::TA_STE;
        case ModuloInformacaoMidia::TipoAplicativo::adh:
            return CAplicativo::TA_ADH;
        case ModuloInformacaoMidia::TipoAplicativo::atue:
            return CAplicativo::TA_ATUE;
        case ModuloInformacaoMidia::TipoAplicativo::unknownEnumeration_:
            throw CAsnMidiasError(EAME_TIPO_APLICATIVO_ENTIDADE_INVALIDO, "Aplicativo inválido");
        default:
            break;
    }
    throw CAsnMidiasError(EAME_TIPO_APLICATIVO_ENTIDADE_INVALIDO, "Aplicativo inválido");
}

} // namespace ecourna::app::dados::asn
