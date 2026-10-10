#include "ecourna/app/dados/asn/midias/cconversorinformacaomidia.hpp"

#include "ecourna/app/dados/asn/cconversorfase.hpp"
#include "ecourna/app/dados/asn/cconversorturno.hpp"
#include "ecourna/app/dados/asn/midias/cconversoraplicativo.hpp"
#include "ecourna/app/dados/asn/midias/cconversordadosgeracaomidia.hpp"
#include "ecourna/app/dados/asn/midias/easnmidiaserror.hpp"

namespace ecourna::app::dados::asn {

ModuloInformacaoMidia::InformacaoMidia CConversorInformacaoMidia::DoConverte(const TDado& dado) const
{
    CConversorDadosGeracaoMidia conversorDadosGeracaoMidia;
    CConversorAplicativo conversorAplicativo;
    CConversorFase conversorFase;
    CConversorTurno conversorTurno;
    TEntidade entidade;
    entidade.set_tipoMidia(ConverteTipoMidia(dado.GetTipoMidia()));
    entidade.set_fase() = conversorFase.Converte(dado.GetFase());
    entidade.set_idPE(dado.GetProcessoEleitoral().Get());
    entidade.set_uf(dado.GetUF());
    entidade.set_turno() = conversorTurno.Converte(dado.GetTurno());
    entidade.set_dadosGeracaoMidia() = conversorDadosGeracaoMidia.Converte(dado.GetDadosGeracaoMidia());
    if (dado.GetTipoMidia() == TDado::TM_MR) {
        auto aplicativos = dado.GetAplicativos();
        entidade.set_aplicativos(ConverteAplicativos(aplicativos, conversorAplicativo));
    } else {
        entidade.omit_aplicativos();
    }
    return entidade;
}

CInformacaoMidia CConversorInformacaoMidia::DoDeconverte(const TEntidade& entidade) const
{
    CConversorDadosGeracaoMidia conversorDadosGeracaoMidia;
    CConversorAplicativo conversorAplicativo;
    CConversorFase conversorFase;
    CConversorTurno conversorTurno;
    TDado::ETipoMidia tipoMidia = DeconverteTipoMidia(entidade.get_tipoMidia());
    TFaseID fase = conversorFase.Deconverte(entidade.get_fase());
    TProcessoEleitoralID processoEleitoral(entidade.get_idPE().getValue());
    std::string uf = entidade.get_uf();
    TTurno turno = conversorTurno.Deconverte(entidade.get_turno());
    CDadosGeracaoMidia dadosGeracaoMidia = conversorDadosGeracaoMidia.Deconverte(entidade.get_dadosGeracaoMidia());
    if (entidade.aplicativos_isPresent()) {
        auto aplicativosAsn = entidade.get_aplicativos();
        const TVectorAplicativo aplicativos = DeconverteSequencia(aplicativosAsn, conversorAplicativo);
        return CInformacaoMidia(fase, processoEleitoral, uf, turno, dadosGeracaoMidia, aplicativos);
    }
    return CInformacaoMidia(tipoMidia, fase, processoEleitoral, uf, turno, dadosGeracaoMidia);
}

// (original lines 53-63: content not recovered)
#line 64
ModuloInformacaoMidia::TipoMidia CConversorInformacaoMidia::ConverteTipoMidia(TDado::ETipoMidia tipoMidia) const
{
    switch (tipoMidia) {
        case TDado::TM_MR:
            return ModuloInformacaoMidia::TipoMidia::mr;
        case TDado::TM_FC:
            return ModuloInformacaoMidia::TipoMidia::fc;
        case TDado::TM_FV:
            return ModuloInformacaoMidia::TipoMidia::fv;
        default:
            break;
    }


    throw CAsnMidiasError(EAME_TIPO_MIDIA_INVALIDO, "Tipo de mídia inválido.");
}

CInformacaoMidia::ETipoMidia CConversorInformacaoMidia::DeconverteTipoMidia(ModuloInformacaoMidia::TipoMidia tipoMidia) const
{
    switch (tipoMidia) {
        case ModuloInformacaoMidia::TipoMidia::mr:
            return TDado::TM_MR;
        case ModuloInformacaoMidia::TipoMidia::fc:
            return TDado::TM_FC;
        case ModuloInformacaoMidia::TipoMidia::fv:
            return TDado::TM_FV;
        default:
            break;
    }

    throw CAsnMidiasError(EAME_TIPO_MIDIA_ENTIDADE_INVALIDO, "Tipo de mídia inválido.");
}








ModuloInformacaoMidia::InformacaoMidia::aplicativos::value_type CConversorInformacaoMidia::ConverteAplicativos(
    const TVectorAplicativo& aplicativos, const CConversorAplicativo& conversor)
{
    TEntidade::aplicativos::value_type resultado;
    for (const auto& aplicativo : aplicativos) {
        resultado.push_back(conversor.Converte(aplicativo));
    }
    return resultado;
}

} // namespace ecourna::app::dados::asn
