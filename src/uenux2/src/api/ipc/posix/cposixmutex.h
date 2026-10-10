#pragma once

#include <pthread.h>

#include "api/ipc/isyncctl.h"

namespace api {

/**
 * Mutex recursivo POSIX.
 */
class CPosixMutex : public ISyncCtl {
public:
    CPosixMutex();
    virtual ~CPosixMutex();

    virtual void Lock();
    virtual void Unlock();

private:
    pthread_mutex_t m_mutex;
    int m_contador;
};

} // namespace api
