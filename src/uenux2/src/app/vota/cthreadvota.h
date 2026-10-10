#pragma once

#include <cstddef>

#include "api/ipc/cthread.h"
#include "api/util/ctickmanager.h"

namespace vota {

/**
 * Base das threads da votação (eleitor, operador, monitor): cada uma tem os seus ticks.
 */
class CThreadVota : public api::CThread {
public:
    void StopTick(uebyte id);
    void StartTick(uebyte id);

private:
    api::CTickManager m_ticks;
};

} // namespace vota
