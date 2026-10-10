#pragma once

#include <semaphore.h>

#include "api/ipc/isemaphore.h"

namespace api {

/**
 * Semáforo POSIX.
 */
class CPosixSemaphore : public ISemaphore {
public:
    explicit CPosixSemaphore(unsigned int valor);
    virtual ~CPosixSemaphore();

    virtual void Lock();
    virtual void Unlock();
    virtual ESemaphoreResult Wait(size_t milissegundos);
    virtual int GetValue() const;

private:
    mutable sem_t m_semaforo{};
};

} // namespace api
