#pragma once
#include <boost/date_time/posix_time/ptime.hpp>

namespace ecourna::app::dados {

// The urna's schedule for a municipality: when the zeroth report is issued, when voting opens, closes and ends.
// 32 bytes, four ptimes in the ASN.1 HorariosUrna field order. Layout from the converter
// (app/dados/asn/cconversorhorariosurna.cpp, fn 9132); file and member names synthesised from the ASN.1 fields.
class CHorariosUrna {
public:
    CHorariosUrna(const boost::posix_time::ptime& emissaoZeresima, const boost::posix_time::ptime& inicioVotacao,
                  const boost::posix_time::ptime& encerramentoVotacao, const boost::posix_time::ptime& terminoVotacao);

    const boost::posix_time::ptime& GetEmissaoZeresima() const { return m_emissaoZeresima; }
    const boost::posix_time::ptime& GetInicioVotacao() const { return m_inicioVotacao; }
    const boost::posix_time::ptime& GetEncerramentoVotacao() const { return m_encerramentoVotacao; }
    const boost::posix_time::ptime& GetTerminoVotacao() const { return m_terminoVotacao; }

private:
    boost::posix_time::ptime m_emissaoZeresima;
    boost::posix_time::ptime m_inicioVotacao;
    boost::posix_time::ptime m_encerramentoVotacao;
    boost::posix_time::ptime m_terminoVotacao;
};

} // namespace ecourna::app::dados
