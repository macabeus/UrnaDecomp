#include "api/ipc/posix/cposixsemaphore.h"

#include <cerrno>
#include <cstring>
#include <ctime>
#include <format>

#include "api/ipc/eueipcerror.h"

namespace api {

namespace {

constexpr size_t MILISSEGUNDOS_POR_SEGUNDO = 1000;
constexpr long NANOSSEGUNDOS_POR_MILISSEGUNDO = 1000000;
constexpr long NANOSSEGUNDOS_POR_SEGUNDO = 1000000000;

} // namespace

CPosixSemaphore::CPosixSemaphore(unsigned int valor)
{
    if (sem_init(&m_semaforo, 0, valor) == -1) {
        throw CUeIpcError(EUE_IPC_FALHA_CRIAR_SEMAFORO, std::format("Falha ao criar semáforo: {}", strerror(errno)));
    }
}

CPosixSemaphore::~CPosixSemaphore()
{
    sem_destroy(&m_semaforo);
}

/**
 * Aguarda o semáforo por até um certo tempo.
 *
 * Sem limite de tempo a espera é feita com sem_wait; com limite, com sem_timedwait
 * a partir do relógio de tempo real. Um tempo de espera nulo aguarda sem limite;
 * o resultado distingue a obtenção do semáforo do esgotamento do tempo.
 *
 * @param milissegundos tempo máximo de espera (0 = sem limite)
 * @return ESR_OK quando o semáforo foi obtido, ESR_TIMEOUT quando o tempo esgotou
 */
ESemaphoreResult CPosixSemaphore::Wait(size_t milissegundos)
{
    int resultado = 0;
    if (milissegundos > 0) {
        timespec limite;
        if (clock_gettime(CLOCK_REALTIME, &limite) == -1) {
            throw CUeIpcError(EUE_IPC_FALHA_RELOGIO, std::format("Falha ao obter tempo de relógio: {}", strerror(errno)));
        }
        if (milissegundos >= MILISSEGUNDOS_POR_SEGUNDO) {
            const size_t segundos = milissegundos / MILISSEGUNDOS_POR_SEGUNDO;
            limite.tv_sec += segundos;
            milissegundos -= segundos * MILISSEGUNDOS_POR_SEGUNDO;
        }
        if (milissegundos > 0) {
            limite.tv_nsec += milissegundos * NANOSSEGUNDOS_POR_MILISSEGUNDO;
            if (limite.tv_nsec >= NANOSSEGUNDOS_POR_SEGUNDO) {
                limite.tv_nsec -= NANOSSEGUNDOS_POR_SEGUNDO;
                ++limite.tv_sec;
            }
        }
        resultado = sem_timedwait(&m_semaforo, &limite);
    } else {
        resultado = sem_wait(&m_semaforo);
    }
    if (resultado == -1) {
        if (errno == ETIMEDOUT) {
            return ESR_TIMEOUT;
        }
        //
        //
        //
        //
        //
        //
        throw CUeIpcError(EUE_IPC_FALHA_AGUARDAR_SEMAFORO, std::format("Falha ao aguardar semáforo: {}", strerror(errno)));
    }
    return ESR_OK;
}

/**
 * Aguarda o semáforo sem limite de tempo.
 *
 * Equivale a Wait(0), sem o resultado: uma falha é sempre
 * reportada por exceção.
 */
void CPosixSemaphore::Lock()
{
    if (sem_wait(&m_semaforo) == -1) {
        throw CUeIpcError(EUE_IPC_FALHA_TRAVAR_SEMAFORO, std::format("Falha ao aguardar semáforo: {}", strerror(errno)));
    }
}

/** Libera o semáforo. */

void CPosixSemaphore::Unlock()
{
    if (sem_post(&m_semaforo) == -1) {
        throw CUeIpcError(EUE_IPC_FALHA_DESTRAVAR_SEMAFORO, std::format("Falha ao liberar semáforo: {}", strerror(errno)));
    }
}

/**
 * Valor corrente do semáforo.
 *
 * O valor é lido sem travar o semáforo.
 *
 * @return valor corrente
 */
int CPosixSemaphore::GetValue() const
{
    int valor;
    if (sem_getvalue(&m_semaforo, &valor) == -1) {
        throw CUeIpcError(EUE_IPC_FALHA_VALOR_SEMAFORO, std::format("Falha ao obter valor do semáforo: {}", strerror(errno)));
    }
    return valor;
}

} // namespace api
