#include "ecourna/app/dados/asn/resultadournacadastro/cconversorcomparecimentomesario.hpp"

#include "ecourna/app/dados/asn/cconversorregistroidentificacaoeleitor.hpp"
#include "ecourna/app/dados/asn/datahoraasn.hpp"
#include "ecourna/app/dados/asn/resultadournacadastro/easnresultadournacadastroerror.hpp"

namespace ecourna::app::dados::asn {

CConversorComparecimentoMesario::TEntidade CConversorComparecimentoMesario::DoConverte(const TDado& dado) const
{
    CConversorRegistroIdentificacaoEleitor conversorRegistro;
    TEntidade entidade;
    entidade.set_identificacaoMesario() = conversorRegistro.Converte(dado.GetIdentificacaoMesario());
    entidade.set_dataHoraColeta() = ConverteDataHora(dado.GetDataHoraColeta());
    if (dado.GetEstadoColetaDigital() != CComparecimentoMesario::EstadoColetaDigitalNaoInformado) {
        entidade.set_estadoColetaDigital(ConverteEstadoColetaDigital(dado.GetEstadoColetaDigital()));
    } else {
        entidade.omit_estadoColetaDigital();
    }
    return entidade;
}

CConversorComparecimentoMesario::TDado CConversorComparecimentoMesario::DoDeconverte(const TEntidade& entidade) const
{
    CConversorRegistroIdentificacaoEleitor conversorRegistro;
    const CRegistroIdentificacaoEleitor identificacaoMesario =
        conversorRegistro.Deconverte(entidade.get_identificacaoMesario());
    const boost::posix_time::ptime dataHoraColeta = DesconverteDataHora(entidade.get_dataHoraColeta());
    const CComparecimentoMesario::EEstadoColetaDigital estadoColetaDigital =
        entidade.estadoColetaDigital_isPresent() ? DeconverteEstadoColetaDigital(entidade.get_estadoColetaDigital())
                                                 : CComparecimentoMesario::EstadoColetaDigitalNaoInformado;

    return CComparecimentoMesario(identificacaoMesario, dataHoraColeta, estadoColetaDigital);
}

// (original lines 36-51: content not recovered)
#line 52
ModuloResultadoUrnaCadastro::EstadoColetaDigital CConversorComparecimentoMesario::ConverteEstadoColetaDigital(
    CComparecimentoMesario::EEstadoColetaDigital estado) const
{
    switch (estado) {
        case CComparecimentoMesario::NaoColetadaReconhecidaNoCadastro:
            return ModuloResultadoUrnaCadastro::EstadoColetaDigital::naoColetadaReconhecidaNoCadastro;
        case CComparecimentoMesario::ColetadaMesarioNaoPossuiDigitalNoCadastro:
            return ModuloResultadoUrnaCadastro::EstadoColetaDigital::coletadaMesarioNaoPossuiDigitalNoCadastro;
        case CComparecimentoMesario::ColetadaDigitalNaoReconhecidaNoCadastro:
            return ModuloResultadoUrnaCadastro::EstadoColetaDigital::coletadaDigitalNaoReconhecidaNoCadastro;
        case CComparecimentoMesario::ColetadaMesarioNaoPertenceASecao:
            return ModuloResultadoUrnaCadastro::EstadoColetaDigital::coletadaMesarioNaoPertenceASecao;
        default:
            break;
    }

    throw CAsnResultadoUrnaCadastroError(EARUCE_ESTADO_COLETA_DIGITAL_INVALIDO, "Estado de coleta de digital inválido.");
}

// (original lines 71-72: content not recovered)
#line 73
CComparecimentoMesario::EEstadoColetaDigital CConversorComparecimentoMesario::DeconverteEstadoColetaDigital(
    ModuloResultadoUrnaCadastro::EstadoColetaDigital estado) const
{
    switch (estado) {
        case ModuloResultadoUrnaCadastro::EstadoColetaDigital::naoColetadaReconhecidaNoCadastro:
            return CComparecimentoMesario::NaoColetadaReconhecidaNoCadastro;
        case ModuloResultadoUrnaCadastro::EstadoColetaDigital::coletadaMesarioNaoPossuiDigitalNoCadastro:
            return CComparecimentoMesario::ColetadaMesarioNaoPossuiDigitalNoCadastro;
        case ModuloResultadoUrnaCadastro::EstadoColetaDigital::coletadaDigitalNaoReconhecidaNoCadastro:
            return CComparecimentoMesario::ColetadaDigitalNaoReconhecidaNoCadastro;
        case ModuloResultadoUrnaCadastro::EstadoColetaDigital::coletadaMesarioNaoPertenceASecao:
            return CComparecimentoMesario::ColetadaMesarioNaoPertenceASecao;
        default:
            break;
    }

    throw CAsnResultadoUrnaCadastroError(EARUCE_ESTADO_COLETA_DIGITAL_ENTIDADE_INVALIDO,
                                         "Estado de coleta de digital inválido.");
}

} // namespace ecourna::app::dados::asn
