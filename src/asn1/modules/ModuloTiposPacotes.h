#ifndef __MODULOTIPOSPACOTES_H
#define __MODULOTIPOSPACOTES_H

#include "asn1.h"

namespace ModuloTiposPacotes {

//
// ModuloTiposPacotes::TipoPacote   // named by exact field/RTTI match
//

class TipoPacote : public ASN1::ENUMERATED
{
    typedef ASN1::ENUMERATED Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    TipoPacote(const void* info) : Inherited(info) {}
public:
    TipoPacote() : Inherited(&theInfo) {}
    enum NamedNumber {
        unknownEnumeration_ = -1,
        pacoteEleitores = 1,
        pacoteMunicipiosZonas,
        pacoteImpedidos = 5,
        pacoteCandidatosTRE = 7,
        pacoteFotosCandidatosTRE,
        pacoteCorrespondencia,
        pacoteSecoes,
        pacoteNotificacaoPacote = 13,
        pacoteConfiguracaoProcessoEleitoral,
        pacoteValidacaoCandidatos,
        pacoteAlteracaoCandidaturaTRE,
        pacoteResultadoTotalizacao,
        pacoteRespostaVersaoPacote,
        pacoteJuntas = 21,
        pacoteEleicoesSegundoTurno,
        pacoteParametrosUrna,
        pacoteNotificacaoImportacao,
        pacotePrestacaoContas,
        pacoteSolicitacaoEstadoSistema,
        pacoteRespostaEstadoSistema,
        pacoteSolicitacaoVersaoPacote,
        pacoteProcessamentoResultadoUrnaCadastro,
        pacoteNotificacaoDisponibilidadeSistema,
        pacoteCandidatosTSE,
        pacoteFotosCandidatosTSE,
        pacoteAlteracaoCandidaturaTSE,
        pacoteSolicitacaoDadosCadastro,
        pacoteDadosEleitor,
        pacoteDadosDocumentacaoCandidatura,
        pacoteComplementosMunicipios,
        pacoteEleicao = 39,
        pacoteSituacoesEleicoes,
        pacoteRespostaAlteracaoCandidatura,
        pacoteSolicitacaoResultadoUrnaCadastro,
        pacoteRespostaSolicitacaoResultadoUrnaCadastro,
        pacoteSolicitacaoMunicipiosZonasParcial,
        pacoteConsolidadoEleicao,
        pacoteConsolidadoSituacoesEleicoes,
        pacoteCandidatosPrestacaoContas,
        pacoteTransferenciaTemporariaEleitores,
        pacoteResumoMunicipios,
        pacoteAlteracoesCadastro,
        pacoteSolicitacaoResumoMunicipios = 52,
        pacoteIndicarVersaoImportacaoPacotes,
        pacoteRespostaIndicarVersaoImportacaoPacotes,
        pacoteNotificacaoRecebimento,
        pacoteSituacaoCorrespondencia,
        pacoteSolicitacaoValidacaoPacotes,
        pacoteRespostaValidacaoPacotes,
        pacoteSolicitacaoResumoIntegracao,
        pacoteRespostaResumoIntegracao,
        pacoteMigracaoMunicipioEleicao,
        pacoteOperacaoCorrespondencia,
        pacoteRespostaProcessamentoResultadoUrnaCadastro,
        pacoteApagaResultadoTotalizacao,
        pacoteAlteracaoEleicoesPleitoFechado
    };

    bool is_pacoteEleitores() const { return value == pacoteEleitores; }
    void set_pacoteEleitores() { value = pacoteEleitores; }

    bool is_pacoteMunicipiosZonas() const { return value == pacoteMunicipiosZonas; }
    void set_pacoteMunicipiosZonas() { value = pacoteMunicipiosZonas; }

    bool is_pacoteImpedidos() const { return value == pacoteImpedidos; }
    void set_pacoteImpedidos() { value = pacoteImpedidos; }

    bool is_pacoteCandidatosTRE() const { return value == pacoteCandidatosTRE; }
    void set_pacoteCandidatosTRE() { value = pacoteCandidatosTRE; }

    bool is_pacoteFotosCandidatosTRE() const { return value == pacoteFotosCandidatosTRE; }
    void set_pacoteFotosCandidatosTRE() { value = pacoteFotosCandidatosTRE; }

    bool is_pacoteCorrespondencia() const { return value == pacoteCorrespondencia; }
    void set_pacoteCorrespondencia() { value = pacoteCorrespondencia; }

    bool is_pacoteSecoes() const { return value == pacoteSecoes; }
    void set_pacoteSecoes() { value = pacoteSecoes; }

    bool is_pacoteNotificacaoPacote() const { return value == pacoteNotificacaoPacote; }
    void set_pacoteNotificacaoPacote() { value = pacoteNotificacaoPacote; }

    bool is_pacoteConfiguracaoProcessoEleitoral() const { return value == pacoteConfiguracaoProcessoEleitoral; }
    void set_pacoteConfiguracaoProcessoEleitoral() { value = pacoteConfiguracaoProcessoEleitoral; }

    bool is_pacoteValidacaoCandidatos() const { return value == pacoteValidacaoCandidatos; }
    void set_pacoteValidacaoCandidatos() { value = pacoteValidacaoCandidatos; }

    bool is_pacoteAlteracaoCandidaturaTRE() const { return value == pacoteAlteracaoCandidaturaTRE; }
    void set_pacoteAlteracaoCandidaturaTRE() { value = pacoteAlteracaoCandidaturaTRE; }

    bool is_pacoteResultadoTotalizacao() const { return value == pacoteResultadoTotalizacao; }
    void set_pacoteResultadoTotalizacao() { value = pacoteResultadoTotalizacao; }

    bool is_pacoteRespostaVersaoPacote() const { return value == pacoteRespostaVersaoPacote; }
    void set_pacoteRespostaVersaoPacote() { value = pacoteRespostaVersaoPacote; }

    bool is_pacoteJuntas() const { return value == pacoteJuntas; }
    void set_pacoteJuntas() { value = pacoteJuntas; }

    bool is_pacoteEleicoesSegundoTurno() const { return value == pacoteEleicoesSegundoTurno; }
    void set_pacoteEleicoesSegundoTurno() { value = pacoteEleicoesSegundoTurno; }

    bool is_pacoteParametrosUrna() const { return value == pacoteParametrosUrna; }
    void set_pacoteParametrosUrna() { value = pacoteParametrosUrna; }

    bool is_pacoteNotificacaoImportacao() const { return value == pacoteNotificacaoImportacao; }
    void set_pacoteNotificacaoImportacao() { value = pacoteNotificacaoImportacao; }

    bool is_pacotePrestacaoContas() const { return value == pacotePrestacaoContas; }
    void set_pacotePrestacaoContas() { value = pacotePrestacaoContas; }

    bool is_pacoteSolicitacaoEstadoSistema() const { return value == pacoteSolicitacaoEstadoSistema; }
    void set_pacoteSolicitacaoEstadoSistema() { value = pacoteSolicitacaoEstadoSistema; }

    bool is_pacoteRespostaEstadoSistema() const { return value == pacoteRespostaEstadoSistema; }
    void set_pacoteRespostaEstadoSistema() { value = pacoteRespostaEstadoSistema; }

    bool is_pacoteSolicitacaoVersaoPacote() const { return value == pacoteSolicitacaoVersaoPacote; }
    void set_pacoteSolicitacaoVersaoPacote() { value = pacoteSolicitacaoVersaoPacote; }

    bool is_pacoteProcessamentoResultadoUrnaCadastro() const { return value == pacoteProcessamentoResultadoUrnaCadastro; }
    void set_pacoteProcessamentoResultadoUrnaCadastro() { value = pacoteProcessamentoResultadoUrnaCadastro; }

    bool is_pacoteNotificacaoDisponibilidadeSistema() const { return value == pacoteNotificacaoDisponibilidadeSistema; }
    void set_pacoteNotificacaoDisponibilidadeSistema() { value = pacoteNotificacaoDisponibilidadeSistema; }

    bool is_pacoteCandidatosTSE() const { return value == pacoteCandidatosTSE; }
    void set_pacoteCandidatosTSE() { value = pacoteCandidatosTSE; }

    bool is_pacoteFotosCandidatosTSE() const { return value == pacoteFotosCandidatosTSE; }
    void set_pacoteFotosCandidatosTSE() { value = pacoteFotosCandidatosTSE; }

    bool is_pacoteAlteracaoCandidaturaTSE() const { return value == pacoteAlteracaoCandidaturaTSE; }
    void set_pacoteAlteracaoCandidaturaTSE() { value = pacoteAlteracaoCandidaturaTSE; }

    bool is_pacoteSolicitacaoDadosCadastro() const { return value == pacoteSolicitacaoDadosCadastro; }
    void set_pacoteSolicitacaoDadosCadastro() { value = pacoteSolicitacaoDadosCadastro; }

    bool is_pacoteDadosEleitor() const { return value == pacoteDadosEleitor; }
    void set_pacoteDadosEleitor() { value = pacoteDadosEleitor; }

    bool is_pacoteDadosDocumentacaoCandidatura() const { return value == pacoteDadosDocumentacaoCandidatura; }
    void set_pacoteDadosDocumentacaoCandidatura() { value = pacoteDadosDocumentacaoCandidatura; }

    bool is_pacoteComplementosMunicipios() const { return value == pacoteComplementosMunicipios; }
    void set_pacoteComplementosMunicipios() { value = pacoteComplementosMunicipios; }

    bool is_pacoteEleicao() const { return value == pacoteEleicao; }
    void set_pacoteEleicao() { value = pacoteEleicao; }

    bool is_pacoteSituacoesEleicoes() const { return value == pacoteSituacoesEleicoes; }
    void set_pacoteSituacoesEleicoes() { value = pacoteSituacoesEleicoes; }

    bool is_pacoteRespostaAlteracaoCandidatura() const { return value == pacoteRespostaAlteracaoCandidatura; }
    void set_pacoteRespostaAlteracaoCandidatura() { value = pacoteRespostaAlteracaoCandidatura; }

    bool is_pacoteSolicitacaoResultadoUrnaCadastro() const { return value == pacoteSolicitacaoResultadoUrnaCadastro; }
    void set_pacoteSolicitacaoResultadoUrnaCadastro() { value = pacoteSolicitacaoResultadoUrnaCadastro; }

    bool is_pacoteRespostaSolicitacaoResultadoUrnaCadastro() const { return value == pacoteRespostaSolicitacaoResultadoUrnaCadastro; }
    void set_pacoteRespostaSolicitacaoResultadoUrnaCadastro() { value = pacoteRespostaSolicitacaoResultadoUrnaCadastro; }

    bool is_pacoteSolicitacaoMunicipiosZonasParcial() const { return value == pacoteSolicitacaoMunicipiosZonasParcial; }
    void set_pacoteSolicitacaoMunicipiosZonasParcial() { value = pacoteSolicitacaoMunicipiosZonasParcial; }

    bool is_pacoteConsolidadoEleicao() const { return value == pacoteConsolidadoEleicao; }
    void set_pacoteConsolidadoEleicao() { value = pacoteConsolidadoEleicao; }

    bool is_pacoteConsolidadoSituacoesEleicoes() const { return value == pacoteConsolidadoSituacoesEleicoes; }
    void set_pacoteConsolidadoSituacoesEleicoes() { value = pacoteConsolidadoSituacoesEleicoes; }

    bool is_pacoteCandidatosPrestacaoContas() const { return value == pacoteCandidatosPrestacaoContas; }
    void set_pacoteCandidatosPrestacaoContas() { value = pacoteCandidatosPrestacaoContas; }

    bool is_pacoteTransferenciaTemporariaEleitores() const { return value == pacoteTransferenciaTemporariaEleitores; }
    void set_pacoteTransferenciaTemporariaEleitores() { value = pacoteTransferenciaTemporariaEleitores; }

    bool is_pacoteResumoMunicipios() const { return value == pacoteResumoMunicipios; }
    void set_pacoteResumoMunicipios() { value = pacoteResumoMunicipios; }

    bool is_pacoteAlteracoesCadastro() const { return value == pacoteAlteracoesCadastro; }
    void set_pacoteAlteracoesCadastro() { value = pacoteAlteracoesCadastro; }

    bool is_pacoteSolicitacaoResumoMunicipios() const { return value == pacoteSolicitacaoResumoMunicipios; }
    void set_pacoteSolicitacaoResumoMunicipios() { value = pacoteSolicitacaoResumoMunicipios; }

    bool is_pacoteIndicarVersaoImportacaoPacotes() const { return value == pacoteIndicarVersaoImportacaoPacotes; }
    void set_pacoteIndicarVersaoImportacaoPacotes() { value = pacoteIndicarVersaoImportacaoPacotes; }

    bool is_pacoteRespostaIndicarVersaoImportacaoPacotes() const { return value == pacoteRespostaIndicarVersaoImportacaoPacotes; }
    void set_pacoteRespostaIndicarVersaoImportacaoPacotes() { value = pacoteRespostaIndicarVersaoImportacaoPacotes; }

    bool is_pacoteNotificacaoRecebimento() const { return value == pacoteNotificacaoRecebimento; }
    void set_pacoteNotificacaoRecebimento() { value = pacoteNotificacaoRecebimento; }

    bool is_pacoteSituacaoCorrespondencia() const { return value == pacoteSituacaoCorrespondencia; }
    void set_pacoteSituacaoCorrespondencia() { value = pacoteSituacaoCorrespondencia; }

    bool is_pacoteSolicitacaoValidacaoPacotes() const { return value == pacoteSolicitacaoValidacaoPacotes; }
    void set_pacoteSolicitacaoValidacaoPacotes() { value = pacoteSolicitacaoValidacaoPacotes; }

    bool is_pacoteRespostaValidacaoPacotes() const { return value == pacoteRespostaValidacaoPacotes; }
    void set_pacoteRespostaValidacaoPacotes() { value = pacoteRespostaValidacaoPacotes; }

    bool is_pacoteSolicitacaoResumoIntegracao() const { return value == pacoteSolicitacaoResumoIntegracao; }
    void set_pacoteSolicitacaoResumoIntegracao() { value = pacoteSolicitacaoResumoIntegracao; }

    bool is_pacoteRespostaResumoIntegracao() const { return value == pacoteRespostaResumoIntegracao; }
    void set_pacoteRespostaResumoIntegracao() { value = pacoteRespostaResumoIntegracao; }

    bool is_pacoteMigracaoMunicipioEleicao() const { return value == pacoteMigracaoMunicipioEleicao; }
    void set_pacoteMigracaoMunicipioEleicao() { value = pacoteMigracaoMunicipioEleicao; }

    bool is_pacoteOperacaoCorrespondencia() const { return value == pacoteOperacaoCorrespondencia; }
    void set_pacoteOperacaoCorrespondencia() { value = pacoteOperacaoCorrespondencia; }

    bool is_pacoteRespostaProcessamentoResultadoUrnaCadastro() const { return value == pacoteRespostaProcessamentoResultadoUrnaCadastro; }
    void set_pacoteRespostaProcessamentoResultadoUrnaCadastro() { value = pacoteRespostaProcessamentoResultadoUrnaCadastro; }

    bool is_pacoteApagaResultadoTotalizacao() const { return value == pacoteApagaResultadoTotalizacao; }
    void set_pacoteApagaResultadoTotalizacao() { value = pacoteApagaResultadoTotalizacao; }

    bool is_pacoteAlteracaoEleicoesPleitoFechado() const { return value == pacoteAlteracaoEleicoesPleitoFechado; }
    void set_pacoteAlteracaoEleicoesPleitoFechado() { value = pacoteAlteracaoEleicoesPleitoFechado; }

    TipoPacote(const NamedNumber v) : Inherited(&theInfo) { setFromInt(v); }
    TipoPacote & operator=(const NamedNumber v) { setFromInt(v);  return *this; }
    operator NamedNumber() const { return NamedNumber(asInt()); }

    bool operator == (NamedNumber rhs) const { return value == rhs; }
    bool operator != (NamedNumber rhs) const { return value != rhs; }
    bool operator <  (NamedNumber rhs) const { return value <  rhs; }
    bool operator >  (NamedNumber rhs) const { return value >  rhs; }
    bool operator <= (NamedNumber rhs) const { return value <= rhs; }
    bool operator >= (NamedNumber rhs) const { return value >= rhs; }
    bool operator == (const TipoPacote &rhs) const { return value == rhs.value; }
    bool operator != (const TipoPacote &rhs) const { return value != rhs.value; }

    void swap (TipoPacote & that) { Inherited::swap(that); }
    TipoPacote * clone() const
    { return static_cast<TipoPacote*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const char * nameList[];
}; // end class TipoPacote

} // namespace ModuloTiposPacotes

#endif // __MODULOTIPOSPACOTES_H
