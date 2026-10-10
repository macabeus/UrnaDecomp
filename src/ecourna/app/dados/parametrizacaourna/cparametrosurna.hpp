#pragma once
#include <string>
#include <vector>

#include "ecourna/api/uetypes.hpp"
#include "ecourna/app/dados/parametrizacaourna/clabelparametrizado.hpp"
#include "ecourna/app/dados/parametrizacaourna/ctitulorelatorio.hpp"

namespace ecourna::app::dados {

// The voting machine's parameters (ModuloParametrizacaoUrna::ParametrosUrna, field for field). 404 bytes. Layout from
// the constructor (fn 5093: scalars stored, the vectors/string/labels taken by value and moved in) and the converter
// (app/dados/asn/parametrizacaourna/cconversorparametrosurna.cpp, fns 9164/9156); member and enumerator names follow
// the ASN.1 field names, the enumerators the ENUMERATED values minus one.
class CParametrosUrna {
public:
    enum EFormaSuspenderSemVoto {
        EFSSV_TORNA_NAO_VOTOU,
        EFSSV_TORNA_TODOS_BRANCO,
        EFSSV_TORNA_TODOS_NULO,
    };

    enum EFormaSuspenderComVoto {
        EFSCV_TORNA_OUTROS_BRANCO,
        EFSCV_TORNA_OUTROS_NULO,
        EFSCV_TORNA_NAO_VOTOU_PARCIAL,
    };

    typedef std::vector<CTituloRelatorio> TVectorTituloRelatorio;

    CParametrosUrna(EFormaSuspenderSemVoto formaSuspensaoSemVoto,
                    EFormaSuspenderComVoto formaSuspensaoComVoto,
                    uedword numZeresimas,
                    uedword numBUVotaObrigatorios,
                    uedword numBUVotaAdicionais,
                    uedword numBUFinalREDObrigat,
                    uedword numBUFinalREDAdic,
                    uedword numBUParcialREDObrigat,
                    uedword numBUParcialREDAdic,
                    uedword numBUFinalSAObrigat,
                    uedword numBUFinalSAAdic,
                    uedword numBUParcialSAObrigat,
                    uedword numTentativasHabilitacao,
                    uedword numTentativasVerificacao,
                    uedword numRelatorioEstado,
                    uedword numRelatorioEleitores,
                    uedword numRelatorioVersoesDados,
                    uedword numRelatorioPU,
                    bool criptografarBU,
                    bool criptografarJUFA,
                    uedword numDigitosPartido,
                    uedword tempoDispararSuspensao,
                    uedword tempoDispararSuspensaoTE,
                    uedword tempoConfirmacaoVoto,
                    uedword tempoDesligamentoAutomatico,
                    uedword tempoAvisoDesligamentoAutomatico,
                    TVectorTituloRelatorio cabecalho,
                    TVectorTituloRelatorio rodapeZEVOTA,
                    TVectorTituloRelatorio rodapeBUVOTA,
                    TVectorTituloRelatorio rodapeZESA,
                    TVectorTituloRelatorio rodapeBUSA,
                    TVectorTituloRelatorio rodapeBUParcial,
                    std::string telaEmissaoBU,
                    CLabelParametrizado labelMunicipio,
                    CLabelParametrizado labelZona,
                    CLabelParametrizado labelSecao,
                    CLabelParametrizado labelPartido,
                    bool imprimirZeradosBU,
                    bool gravarZeradosBU,
                    bool aceitarJustificativa,
                    bool aceitarBrancoNulo,
                    bool apresentarPartido,
                    bool permitirHabManualAudio,
                    bool imprimirQrCodeNoBU,
                    bool exibirScoreBiometria,
                    bool registrarMesarios,
                    bool pedeAnoNascimentoEleitor);

    EFormaSuspenderSemVoto GetFormaSuspensaoSemVoto() const { return m_formaSuspensaoSemVoto; }
    EFormaSuspenderComVoto GetFormaSuspensaoComVoto() const { return m_formaSuspensaoComVoto; }
    uedword GetNumZeresimas() const { return m_numZeresimas; }
    uedword GetNumBUVotaObrigatorios() const { return m_numBUVotaObrigatorios; }
    uedword GetNumBUVotaAdicionais() const { return m_numBUVotaAdicionais; }
    uedword GetNumBUFinalREDObrigat() const { return m_numBUFinalREDObrigat; }
    uedword GetNumBUFinalREDAdic() const { return m_numBUFinalREDAdic; }
    uedword GetNumBUParcialREDObrigat() const { return m_numBUParcialREDObrigat; }
    uedword GetNumBUParcialREDAdic() const { return m_numBUParcialREDAdic; }
    uedword GetNumBUFinalSAObrigat() const { return m_numBUFinalSAObrigat; }
    uedword GetNumBUFinalSAAdic() const { return m_numBUFinalSAAdic; }
    uedword GetNumBUParcialSAObrigat() const { return m_numBUParcialSAObrigat; }
    uedword GetNumTentativasHabilitacao() const { return m_numTentativasHabilitacao; }
    uedword GetNumTentativasVerificacao() const { return m_numTentativasVerificacao; }
    uedword GetNumRelatorioEstado() const { return m_numRelatorioEstado; }
    uedword GetNumRelatorioEleitores() const { return m_numRelatorioEleitores; }
    uedword GetNumRelatorioVersoesDados() const { return m_numRelatorioVersoesDados; }
    uedword GetNumRelatorioPU() const { return m_numRelatorioPU; }
    bool GetCriptografarBU() const { return m_criptografarBU; }
    bool GetCriptografarJUFA() const { return m_criptografarJUFA; }
    uedword GetNumDigitosPartido() const { return m_numDigitosPartido; }
    uedword GetTempoDispararSuspensao() const { return m_tempoDispararSuspensao; }
    uedword GetTempoDispararSuspensaoTE() const { return m_tempoDispararSuspensaoTE; }
    uedword GetTempoConfirmacaoVoto() const { return m_tempoConfirmacaoVoto; }
    uedword GetTempoDesligamentoAutomatico() const { return m_tempoDesligamentoAutomatico; }
    uedword GetTempoAvisoDesligamentoAutomatico() const { return m_tempoAvisoDesligamentoAutomatico; }
    const TVectorTituloRelatorio& GetCabecalho() const { return m_cabecalho; }
    const TVectorTituloRelatorio& GetRodapeZEVOTA() const { return m_rodapeZEVOTA; }
    const TVectorTituloRelatorio& GetRodapeBUVOTA() const { return m_rodapeBUVOTA; }
    const TVectorTituloRelatorio& GetRodapeZESA() const { return m_rodapeZESA; }
    const TVectorTituloRelatorio& GetRodapeBUSA() const { return m_rodapeBUSA; }
    const TVectorTituloRelatorio& GetRodapeBUParcial() const { return m_rodapeBUParcial; }
    const std::string& GetTelaEmissaoBU() const { return m_telaEmissaoBU; }
    const CLabelParametrizado& GetLabelMunicipio() const { return m_labelMunicipio; }
    const CLabelParametrizado& GetLabelZona() const { return m_labelZona; }
    const CLabelParametrizado& GetLabelSecao() const { return m_labelSecao; }
    const CLabelParametrizado& GetLabelPartido() const { return m_labelPartido; }
    bool GetImprimirZeradosBU() const { return m_imprimirZeradosBU; }
    bool GetGravarZeradosBU() const { return m_gravarZeradosBU; }
    bool GetAceitarJustificativa() const { return m_aceitarJustificativa; }
    bool GetAceitarBrancoNulo() const { return m_aceitarBrancoNulo; }
    bool GetApresentarPartido() const { return m_apresentarPartido; }
    bool GetPermitirHabManualAudio() const { return m_permitirHabManualAudio; }
    bool GetImprimirQrCodeNoBU() const { return m_imprimirQrCodeNoBU; }
    bool GetExibirScoreBiometria() const { return m_exibirScoreBiometria; }
    bool GetRegistrarMesarios() const { return m_registrarMesarios; }
    bool GetPedeAnoNascimentoEleitor() const { return m_pedeAnoNascimentoEleitor; }

private:
    EFormaSuspenderSemVoto m_formaSuspensaoSemVoto;
    EFormaSuspenderComVoto m_formaSuspensaoComVoto;
    uedword m_numZeresimas;
    uedword m_numBUVotaObrigatorios;
    uedword m_numBUVotaAdicionais;
    uedword m_numBUFinalREDObrigat;
    uedword m_numBUFinalREDAdic;
    uedword m_numBUParcialREDObrigat;
    uedword m_numBUParcialREDAdic;
    uedword m_numBUFinalSAObrigat;
    uedword m_numBUFinalSAAdic;
    uedword m_numBUParcialSAObrigat;
    uedword m_numTentativasHabilitacao;
    uedword m_numTentativasVerificacao;
    uedword m_numRelatorioEstado;
    uedword m_numRelatorioEleitores;
    uedword m_numRelatorioVersoesDados;
    uedword m_numRelatorioPU;
    bool m_criptografarBU;
    bool m_criptografarJUFA;
    uedword m_numDigitosPartido;
    uedword m_tempoDispararSuspensao;
    uedword m_tempoDispararSuspensaoTE;
    uedword m_tempoConfirmacaoVoto;
    uedword m_tempoDesligamentoAutomatico;
    uedword m_tempoAvisoDesligamentoAutomatico;
    TVectorTituloRelatorio m_cabecalho;
    TVectorTituloRelatorio m_rodapeZEVOTA;
    TVectorTituloRelatorio m_rodapeBUVOTA;
    TVectorTituloRelatorio m_rodapeZESA;
    TVectorTituloRelatorio m_rodapeBUSA;
    TVectorTituloRelatorio m_rodapeBUParcial;
    std::string m_telaEmissaoBU;
    CLabelParametrizado m_labelMunicipio;
    CLabelParametrizado m_labelZona;
    CLabelParametrizado m_labelSecao;
    CLabelParametrizado m_labelPartido;
    bool m_imprimirZeradosBU;
    bool m_gravarZeradosBU;
    bool m_aceitarJustificativa;
    bool m_aceitarBrancoNulo;
    bool m_apresentarPartido;
    bool m_permitirHabManualAudio;
    bool m_imprimirQrCodeNoBU;
    bool m_exibirScoreBiometria;
    bool m_registrarMesarios;
    bool m_pedeAnoNascimentoEleitor;
};

} // namespace ecourna::app::dados
