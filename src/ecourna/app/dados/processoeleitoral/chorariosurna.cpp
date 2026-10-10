#include "ecourna/app/dados/processoeleitoral/chorariosurna.hpp"

namespace ecourna::app::dados {

CHorariosUrna::CHorariosUrna(const boost::posix_time::ptime& emissaoZeresima,
                             const boost::posix_time::ptime& inicioVotacao,
                             const boost::posix_time::ptime& encerramentoVotacao,
                             const boost::posix_time::ptime& terminoVotacao)
    : m_emissaoZeresima(emissaoZeresima), m_inicioVotacao(inicioVotacao), m_encerramentoVotacao(encerramentoVotacao),
      m_terminoVotacao(terminoVotacao)
{
}

} // namespace ecourna::app::dados
