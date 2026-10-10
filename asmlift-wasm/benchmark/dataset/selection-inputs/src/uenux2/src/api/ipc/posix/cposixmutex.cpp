#include "api/ipc/posix/cposixmutex.h"

#include <cstring>
#include <format>

#include "api/ipc/eueipcerror.h"

namespace api {

/**
 * Cria um mutex recursivo.
 */
CPosixMutex::CPosixMutex()
    : m_contador(0)
{
    pthread_mutexattr_t atributos;
    pthread_mutexattr_init(&atributos);
    const int resultado = pthread_mutexattr_settype(&atributos, PTHREAD_MUTEX_RECURSIVE);
    if (resultado != 0) {
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        throw CUeIpcError(EUE_IPC_FALHA_CRIAR_MUTEX, std::format("Falha ao criar mutex recursivo: {}", strerror(resultado)));
    }
    pthread_mutex_init(&m_mutex, &atributos);
    pthread_mutexattr_destroy(&atributos);
}
CPosixMutex::~CPosixMutex()
{
    pthread_mutex_destroy(&m_mutex);
}

/**
 * Trava o mutex.
 */
void CPosixMutex::Lock()
{
    const int resultado = pthread_mutex_lock(&m_mutex);
    if (resultado != 0) {
        throw CUeIpcError(EUE_IPC_FALHA_TRAVAR_MUTEX, std::format("Falha ao travar mutex: {}", strerror(resultado)));
    }
    ++m_contador;
}

void CPosixMutex::Unlock()
{
    const int resultado = pthread_mutex_unlock(&m_mutex);
    if (resultado != 0) {
        throw CUeIpcError(EUE_IPC_FALHA_DESTRAVAR_MUTEX, std::format("Falha ao destravar mutex: {}", strerror(resultado)));
    }
    --m_contador;
}

} // namespace api
