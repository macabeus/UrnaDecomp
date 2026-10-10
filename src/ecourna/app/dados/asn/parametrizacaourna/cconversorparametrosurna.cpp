#include "ecourna/app/dados/asn/parametrizacaourna/cconversorparametrosurna.hpp"

#include "ecourna/app/dados/asn/parametrizacaourna/cconversorlabelparametrizado.hpp"
#include "ecourna/app/dados/asn/parametrizacaourna/cconversortitulorelatorio.hpp"
#include "ecourna/app/dados/asn/parametrizacaourna/easnparametrizacaournaerror.hpp"

namespace ecourna::app::dados::asn {

namespace {

// Built by this object's static initialiser (BSS 1912244) but referenced by no code in the target.
const std::string ERRO_FORMA_VALIDAR_TITULO = "Forma de validar título inválido.";

} // namespace

// (original lines 16-18: content not recovered)
#line 19
CConversorParametrosUrna::TEntidade CConversorParametrosUrna::DoConverte(const TDado& dado) const
{
    TEntidade entidade;
    CConversorLabelParametrizado conversorLabel;
    entidade.set_formaSuspensaoSemVoto(ConverterFormaSuspenderSemVoto(dado.GetFormaSuspensaoSemVoto()));
    entidade.set_formaSuspensaoComVoto(ConverterFormaSuspenderComVoto(dado.GetFormaSuspensaoComVoto()));
    entidade.set_numZeresimas() = dado.GetNumZeresimas();
    entidade.set_numBUVotaObrigatorios() = dado.GetNumBUVotaObrigatorios();
    entidade.set_numBUVotaAdicionais() = dado.GetNumBUVotaAdicionais();
    entidade.set_numBUFinalREDObrigat() = dado.GetNumBUFinalREDObrigat();
    entidade.set_numBUFinalREDAdic() = dado.GetNumBUFinalREDAdic();
    entidade.set_numBUParcialREDObrigat() = dado.GetNumBUParcialREDObrigat();
    entidade.set_numBUParcialREDAdic() = dado.GetNumBUParcialREDAdic();
    entidade.set_numBUFinalSAObrigat() = dado.GetNumBUFinalSAObrigat();
    entidade.set_numBUFinalSAAdic() = dado.GetNumBUFinalSAAdic();
    entidade.set_numBUParcialSAObrigat() = dado.GetNumBUParcialSAObrigat();
    entidade.set_numTentativasHabilitacao() = dado.GetNumTentativasHabilitacao();
    entidade.set_numTentativasVerificacao() = dado.GetNumTentativasVerificacao();
    entidade.set_numRelatorioEstado() = dado.GetNumRelatorioEstado();
    entidade.set_numRelatorioEleitores() = dado.GetNumRelatorioEleitores();
    entidade.set_numRelatorioVersoesDados() = dado.GetNumRelatorioVersoesDados();
    entidade.set_numRelatorioPU() = dado.GetNumRelatorioPU();
    entidade.set_criptografarBU() = dado.GetCriptografarBU();
    entidade.set_criptografarJUFA() = dado.GetCriptografarJUFA();
    entidade.set_numDigitosPartido() = dado.GetNumDigitosPartido();
    entidade.set_tempoDispararSuspensao() = dado.GetTempoDispararSuspensao();
    entidade.set_tempoDispararSuspensaoTE() = dado.GetTempoDispararSuspensaoTE();
    entidade.set_tempoConfirmacaoVoto() = dado.GetTempoConfirmacaoVoto();
    entidade.set_tempoDesligamentoAutomatico() = dado.GetTempoDesligamentoAutomatico();
    entidade.set_tempoAvisoDesligamentoAutomatico() = dado.GetTempoAvisoDesligamentoAutomatico();
    entidade.set_cabecalho() = ConverterCabecalho(dado.GetCabecalho());
    entidade.set_rodapeZEVOTA() = ConverterCabecalho(dado.GetRodapeZEVOTA());
    entidade.set_rodapeBUVOTA() = ConverterCabecalho(dado.GetRodapeBUVOTA());
    entidade.set_rodapeZESA() = ConverterCabecalho(dado.GetRodapeZESA());
    entidade.set_rodapeBUSA() = ConverterCabecalho(dado.GetRodapeBUSA());
    entidade.set_rodapeBUParcial() = ConverterCabecalho(dado.GetRodapeBUParcial());
    entidade.set_telaEmissaoBU() = dado.GetTelaEmissaoBU();
    entidade.set_labelMunicipio() = conversorLabel.Converte(dado.GetLabelMunicipio());
    entidade.set_labelZona() = conversorLabel.Converte(dado.GetLabelZona());
    entidade.set_labelSecao() = conversorLabel.Converte(dado.GetLabelSecao());
    entidade.set_labelPartido() = conversorLabel.Converte(dado.GetLabelPartido());
    entidade.set_imprimirZeradosBU() = dado.GetImprimirZeradosBU();
    entidade.set_gravarZeradosBU() = dado.GetGravarZeradosBU();
    entidade.set_aceitarJustificativa() = dado.GetAceitarJustificativa();
    entidade.set_aceitarBrancoNulo() = dado.GetAceitarBrancoNulo();
    entidade.set_apresentarPartido() = dado.GetApresentarPartido();
    entidade.set_permitirHabManualAudio() = dado.GetPermitirHabManualAudio();
    entidade.set_imprimirQrCodeNoBU() = dado.GetImprimirQrCodeNoBU();
    entidade.set_exibirScoreBiometria() = dado.GetExibirScoreBiometria();
    entidade.set_registrarMesarios() = dado.GetRegistrarMesarios();
    entidade.set_pedeAnoNascimentoEleitor() = dado.GetPedeAnoNascimentoEleitor();
    return entidade;
}

CConversorParametrosUrna::TDado CConversorParametrosUrna::DoDeconverte(const TEntidade& entidade) const
{
    CConversorLabelParametrizado conversorLabel;
    return CParametrosUrna(DesconverterFormaSuspenderSemVoto(entidade), DesconverterFormaSuspenderComVoto(entidade),
                           entidade.get_numZeresimas().getValue(), entidade.get_numBUVotaObrigatorios().getValue(),
                           entidade.get_numBUVotaAdicionais().getValue(),
                           entidade.get_numBUFinalREDObrigat().getValue(),
                           entidade.get_numBUFinalREDAdic().getValue(),
                           entidade.get_numBUParcialREDObrigat().getValue(),
                           entidade.get_numBUParcialREDAdic().getValue(),
                           entidade.get_numBUFinalSAObrigat().getValue(), entidade.get_numBUFinalSAAdic().getValue(),
                           entidade.get_numBUParcialSAObrigat().getValue(),
                           entidade.get_numTentativasHabilitacao().getValue(),
                           entidade.get_numTentativasVerificacao().getValue(),
                           entidade.get_numRelatorioEstado().getValue(),
                           entidade.get_numRelatorioEleitores().getValue(),
                           entidade.get_numRelatorioVersoesDados().getValue(),
                           entidade.get_numRelatorioPU().getValue(), entidade.get_criptografarBU(),
                           entidade.get_criptografarJUFA(), entidade.get_numDigitosPartido().getValue(),
                           entidade.get_tempoDispararSuspensao().getValue(),
                           entidade.get_tempoDispararSuspensaoTE().getValue(),
                           entidade.get_tempoConfirmacaoVoto().getValue(),
                           entidade.get_tempoDesligamentoAutomatico().getValue(),
                           entidade.get_tempoAvisoDesligamentoAutomatico().getValue(),
                           DesconverterCabecalho(entidade.get_cabecalho()),
                           DesconverterCabecalho(entidade.get_rodapeZEVOTA()),
                           DesconverterCabecalho(entidade.get_rodapeBUVOTA()),
                           DesconverterCabecalho(entidade.get_rodapeZESA()),
                           DesconverterCabecalho(entidade.get_rodapeBUSA()),
                           DesconverterCabecalho(entidade.get_rodapeBUParcial()), entidade.get_telaEmissaoBU(),
                           conversorLabel.Deconverte(entidade.get_labelMunicipio()),
                           conversorLabel.Deconverte(entidade.get_labelZona()),
                           conversorLabel.Deconverte(entidade.get_labelSecao()),
                           conversorLabel.Deconverte(entidade.get_labelPartido()), entidade.get_imprimirZeradosBU(),
                           entidade.get_gravarZeradosBU(), entidade.get_aceitarJustificativa(),
                           entidade.get_aceitarBrancoNulo(), entidade.get_apresentarPartido(),
                           entidade.get_permitirHabManualAudio(), entidade.get_imprimirQrCodeNoBU(),
                           entidade.get_exibirScoreBiometria(), entidade.get_registrarMesarios(),
                           entidade.get_pedeAnoNascimentoEleitor());
}

ModuloParametrizacaoUrna::Cabecalho CConversorParametrosUrna::ConverterCabecalho(
    const CParametrosUrna::TVectorTituloRelatorio& titulos) const
{
    ModuloParametrizacaoUrna::Cabecalho cabecalho;
    CConversorTituloRelatorio conversorTitulo;
    for (size_t i = 0; i < titulos.size(); ++i) {
        cabecalho.push_back(conversorTitulo.Converte(titulos[i]));
    }
    return cabecalho;
}

CParametrosUrna::TVectorTituloRelatorio CConversorParametrosUrna::DesconverterCabecalho(
    const ModuloParametrizacaoUrna::Cabecalho& cabecalho) const
{
    CParametrosUrna::TVectorTituloRelatorio titulos;
    CConversorTituloRelatorio conversorTitulo;
    for (size_t i = 0; i < cabecalho.size(); ++i) {
        titulos.push_back(conversorTitulo.Deconverte(cabecalho[i]));
    }
    return titulos;
}

ModuloParametrizacaoUrna::FormaSuspenderComVoto CConversorParametrosUrna::ConverterFormaSuspenderComVoto(
    CParametrosUrna::EFormaSuspenderComVoto forma) const
{
    switch (forma) {
        case CParametrosUrna::EFSCV_TORNA_OUTROS_BRANCO:
            return ModuloParametrizacaoUrna::FormaSuspenderComVoto::tornaOutrosBranco;
        case CParametrosUrna::EFSCV_TORNA_OUTROS_NULO:
            return ModuloParametrizacaoUrna::FormaSuspenderComVoto::tornaOutrosNulo;
        case CParametrosUrna::EFSCV_TORNA_NAO_VOTOU_PARCIAL:
            return ModuloParametrizacaoUrna::FormaSuspenderComVoto::tornaNaoVotouParcial;
    }
    throw CAsnParametrizacaoUrnaError(EAPUE_FORMA_SUSPENDER_COM_VOTO_INVALIDA, "Forma de suspender com voto inválido.");
}

// (original lines 150-151: content not recovered)
#line 152
CParametrosUrna::EFormaSuspenderComVoto CConversorParametrosUrna::DesconverterFormaSuspenderComVoto(
    const TEntidade& entidade) const
{
    switch (entidade.get_formaSuspensaoComVoto()) {
        case ModuloParametrizacaoUrna::FormaSuspenderComVoto::tornaNaoVotouParcial:
            return CParametrosUrna::EFSCV_TORNA_NAO_VOTOU_PARCIAL;
        case ModuloParametrizacaoUrna::FormaSuspenderComVoto::tornaOutrosBranco:
            return CParametrosUrna::EFSCV_TORNA_OUTROS_BRANCO;
        case ModuloParametrizacaoUrna::FormaSuspenderComVoto::tornaOutrosNulo:
            return CParametrosUrna::EFSCV_TORNA_OUTROS_NULO;
        case ModuloParametrizacaoUrna::FormaSuspenderComVoto::unknownEnumeration_:
            throw CAsnParametrizacaoUrnaError(EAPUE_FORMA_SUSPENDER_COM_VOTO_DESCONHECIDA, "Forma de suspender com voto inválido.");
        default:
            break;
    }
    throw CAsnParametrizacaoUrnaError(EAPUE_FORMA_SUSPENDER_COM_VOTO_ENTIDADE_INVALIDA, "Forma de suspender com voto inválido.");
}

// (original lines 170-171: content not recovered)
#line 172
ModuloParametrizacaoUrna::FormaSuspenderSemVoto CConversorParametrosUrna::ConverterFormaSuspenderSemVoto(
    CParametrosUrna::EFormaSuspenderSemVoto forma) const
{
    switch (forma) {
        case CParametrosUrna::EFSSV_TORNA_NAO_VOTOU:
            return ModuloParametrizacaoUrna::FormaSuspenderSemVoto::tornaNaoVotou;
        case CParametrosUrna::EFSSV_TORNA_TODOS_BRANCO:
            return ModuloParametrizacaoUrna::FormaSuspenderSemVoto::tornaTodosBranco;
        case CParametrosUrna::EFSSV_TORNA_TODOS_NULO:
            return ModuloParametrizacaoUrna::FormaSuspenderSemVoto::tornaTodosNulo;
    }
    throw CAsnParametrizacaoUrnaError(EAPUE_FORMA_SUSPENDER_SEM_VOTO_INVALIDA, "Forma de suspender sem voto inválido.");
}

// (original lines 186-187: content not recovered)
#line 188
CParametrosUrna::EFormaSuspenderSemVoto CConversorParametrosUrna::DesconverterFormaSuspenderSemVoto(
    const TEntidade& entidade) const
{
    switch (entidade.get_formaSuspensaoSemVoto()) {
        case ModuloParametrizacaoUrna::FormaSuspenderSemVoto::tornaNaoVotou:
            return CParametrosUrna::EFSSV_TORNA_NAO_VOTOU;
        case ModuloParametrizacaoUrna::FormaSuspenderSemVoto::tornaTodosBranco:
            return CParametrosUrna::EFSSV_TORNA_TODOS_BRANCO;
        case ModuloParametrizacaoUrna::FormaSuspenderSemVoto::tornaTodosNulo:
            return CParametrosUrna::EFSSV_TORNA_TODOS_NULO;
        case ModuloParametrizacaoUrna::FormaSuspenderSemVoto::unknownEnumeration_:
            throw CAsnParametrizacaoUrnaError(EAPUE_FORMA_SUSPENDER_SEM_VOTO_DESCONHECIDA, "Forma de suspender sem voto inválido.");
        default:
            break;
    }
    throw CAsnParametrizacaoUrnaError(EAPUE_FORMA_SUSPENDER_SEM_VOTO_ENTIDADE_INVALIDA, "Forma de suspender sem voto inválido.");
}

} // namespace ecourna::app::dados::asn
