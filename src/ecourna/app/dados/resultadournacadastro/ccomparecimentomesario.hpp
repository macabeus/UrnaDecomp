#pragma once
#include <vector>

#include <boost/date_time/posix_time/ptime.hpp>

#include "ecourna/app/dados/cregistroidentificacaoeleitor.hpp"

namespace ecourna::app::dados {

// A poll worker's attendance: who (@0, 20 B), when the fingerprint was taken (@24, boost ptime, 8 B) and the
// fingerprint collection state (@32). 40 bytes, 8-aligned. EEstadoColetaDigital's values are the ASN.1 ones (the
// converter's DeconverteEstadoColetaDigital, fn 9103, returns the ASN.1 value unchanged after a 1..4 range check);
// 0 is "not informed": the converter omits the optional ASN.1 field for it (fn 9110) and passes it when the field is
// absent (fn 9106). Its name is synthesised.
class CComparecimentoMesario {
public:
    enum EEstadoColetaDigital {
        EstadoColetaDigitalNaoInformado = 0,
        NaoColetadaReconhecidaNoCadastro = 1,
        ColetadaMesarioNaoPossuiDigitalNoCadastro,
        ColetadaDigitalNaoReconhecidaNoCadastro,
        ColetadaMesarioNaoPertenceASecao,
    };

    CComparecimentoMesario(const CRegistroIdentificacaoEleitor& identificacaoMesario,
                           const boost::posix_time::ptime& dataHoraColeta, EEstadoColetaDigital estadoColetaDigital);

    const CRegistroIdentificacaoEleitor& GetIdentificacaoMesario() const { return m_identificacaoMesario; }
    const boost::posix_time::ptime& GetDataHoraColeta() const { return m_dataHoraColeta; }
    EEstadoColetaDigital GetEstadoColetaDigital() const { return m_estadoColetaDigital; }

private:
    CRegistroIdentificacaoEleitor m_identificacaoMesario;
    boost::posix_time::ptime m_dataHoraColeta;
    EEstadoColetaDigital m_estadoColetaDigital;
};

using TVectorComparecimentoMesario = std::vector<CComparecimentoMesario>;

} // namespace ecourna::app::dados
