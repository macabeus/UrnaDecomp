#pragma once

#include <cstddef>
#include <sys/time.h>

namespace api {

/**
 * Espera por um intervalo de tempo.
 */
class CWait {
public:
    explicit CWait(const size_t milissegundos);

private:
    size_t m_microssegundos;
    timeval m_inicio;
};

} // namespace api
