#include "app/vota/cthreadvota.h"

namespace vota {

void CThreadVota::StopTick(uebyte id)
{
    m_ticks.StopTick(id);
}

void CThreadVota::StartTick(uebyte id)
{
    m_ticks.StartTick(id);
}

} // namespace vota
