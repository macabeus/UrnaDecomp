#pragma once

#include <cstddef>
#include <map>
#include <sys/time.h>

#include "ecourna/api/uetypes.hpp"

namespace api {

/**
 * Controla os ticks (temporizadores periódicos) de uma thread, identificados por id.
 */
class CTickManager {
public:
    uebyte AddTick(size_t milissegundos);
    void StopTick(uebyte id);
    void StartTick(uebyte id);

private:
    struct STick {
        ueint64 m_milissegundos;
        timeval m_proximo;
        ueint32 m_parado;
    };

    uebyte SearchNextId();

    std::map<uebyte, STick> m_ticks;
};

} // namespace api
