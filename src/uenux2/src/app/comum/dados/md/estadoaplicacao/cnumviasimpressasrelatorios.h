#pragma once

#include "ecourna/api/uetypes.hpp"

namespace comum::md::estadoaplicacao {

// How many copies of each report the urna prints. comum::asn::CConversorNumViasImpressasRelatorios::DoConverte
// (target fn 11386) reads the four counts as bytes 0-3, in the order of ModuloEstadoGeralDefs::NumViasImpressasRelatorios's
// fields; the member names follow those fields.
class CNumViasImpressasRelatorios {
public:
    CNumViasImpressasRelatorios(uebyte numViasEstadoUrna, uebyte numViasEleitores, uebyte numViasVersoesDados,
                                uebyte numViasPU);

    uebyte GetNumViasEstadoUrna() const { return m_numViasEstadoUrna; }
    uebyte GetNumViasEleitores() const { return m_numViasEleitores; }
    uebyte GetNumViasVersoesDados() const { return m_numViasVersoesDados; }
    uebyte GetNumViasPU() const { return m_numViasPU; }

private:
    uebyte m_numViasEstadoUrna;
    uebyte m_numViasEleitores;
    uebyte m_numViasVersoesDados;
    uebyte m_numViasPU;
};

} // namespace comum::md::estadoaplicacao
