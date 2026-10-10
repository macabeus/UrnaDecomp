#include "app/comum/dados/md/estadoaplicacao/cnumviasimpressasrelatorios.h"

namespace comum::md::estadoaplicacao {

CNumViasImpressasRelatorios::CNumViasImpressasRelatorios(uebyte numViasEstadoUrna, uebyte numViasEleitores,
                                                         uebyte numViasVersoesDados, uebyte numViasPU)
    : m_numViasEstadoUrna(numViasEstadoUrna),
      m_numViasEleitores(numViasEleitores),
      m_numViasVersoesDados(numViasVersoesDados),
      m_numViasPU(numViasPU)
{
}

} // namespace comum::md::estadoaplicacao
