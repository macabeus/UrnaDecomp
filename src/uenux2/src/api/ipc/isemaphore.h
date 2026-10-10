#pragma once

#include <cstddef>

#include "api/ipc/isyncctl.h"

namespace api {

enum ESemaphoreResult {
    ESR_OK = 0,
    ESR_TIMEOUT,
};

/**
 * Semáforo contador.
 */
class ISemaphore : public ISyncCtl {
public:
    virtual void Wait() { Lock(); }
    virtual ESemaphoreResult Wait(size_t milissegundos) = 0;
    virtual int GetValue() const = 0;
};

} // namespace api
