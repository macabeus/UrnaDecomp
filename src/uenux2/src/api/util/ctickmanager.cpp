#include "api/util/ctickmanager.h"

#include <format>

#include "api/util/eueutilerror.h"

namespace api {

namespace {

// intervalo mínimo de um tick
constexpr int MINIMO_MILISSEGUNDOS = 30;
constexpr ueint64 MILISSEGUNDOS_POR_SEGUNDO = 1000;
constexpr ueint64 MICROSSEGUNDOS_POR_MILISSEGUNDO = 1000;
constexpr int MICROSSEGUNDOS_POR_SEGUNDO = 1000000;

} // namespace

/**
 * Cria um tick, que dispara a cada intervalo a partir de agora.
 *
 * @param milissegundos intervalo do tick
 *
 * @return id do tick criado
 *
 * @throw CUeUtilError se o intervalo for menor que o mínimo ou se não houver id disponível
 */
uebyte CTickManager::AddTick(size_t milissegundos)
{
    if (milissegundos < MINIMO_MILISSEGUNDOS) {
        throw CUeUtilError(EUE_UTIL_TICK_TEMPO_INVALIDO, std::format("O tempo do tick deve ser >= {}", MINIMO_MILISSEGUNDOS));
    }

    const uebyte id = SearchNextId();

    STick tick;
    tick.m_parado = 0;
    tick.m_milissegundos = milissegundos;

    timeval agora;
    gettimeofday(&agora, nullptr);
    tick.m_proximo.tv_sec = agora.tv_sec + tick.m_milissegundos / MILISSEGUNDOS_POR_SEGUNDO;
    tick.m_proximo.tv_usec = agora.tv_usec + (tick.m_milissegundos % MILISSEGUNDOS_POR_SEGUNDO) * MICROSSEGUNDOS_POR_MILISSEGUNDO;
    tick.m_proximo.tv_sec += tick.m_proximo.tv_usec / MICROSSEGUNDOS_POR_SEGUNDO;
    tick.m_proximo.tv_usec %= MICROSSEGUNDOS_POR_SEGUNDO;

    m_ticks.emplace(id, tick);
    return id;
}

/**
 * Para um tick: ele deixa de disparar até ser iniciado de novo.
 *
 * @param id id do tick
 */
void CTickManager::StopTick(uebyte id)
{
    auto it = m_ticks.find(id);
    if (it == m_ticks.end()) {
        throw CUeUtilError(EUE_UTIL_TICK_NAO_EXISTE_PARAR, std::format("Não existe um tick com o id {}", id));
    }
    it->second.m_parado = 1;
}

void CTickManager::StartTick(uebyte id)
{
    auto it = m_ticks.find(id);
    if (it == m_ticks.end()) {
        throw CUeUtilError(EUE_UTIL_TICK_NAO_EXISTE_INICIAR, std::format("Não existe um tick com o id {}", id));
    }
    STick& tick = it->second;
    tick.m_parado = 0;

    timeval agora;
    gettimeofday(&agora, nullptr);
    tick.m_proximo.tv_sec = agora.tv_sec + tick.m_milissegundos / MILISSEGUNDOS_POR_SEGUNDO;
    tick.m_proximo.tv_usec = agora.tv_usec + (tick.m_milissegundos % MILISSEGUNDOS_POR_SEGUNDO) * MICROSSEGUNDOS_POR_MILISSEGUNDO;
    tick.m_proximo.tv_sec += tick.m_proximo.tv_usec / MICROSSEGUNDOS_POR_SEGUNDO;
    tick.m_proximo.tv_usec %= MICROSSEGUNDOS_POR_SEGUNDO;
}

uebyte CTickManager::SearchNextId()
{
    for (unsigned id = 0; id <= 0xff; ++id) {
        if (m_ticks.find(id) == m_ticks.end()) {
            return id;
        }
    }
    throw CUeUtilError(EUE_UTIL_TICK_SEM_ID_DISPONIVEL, "Não há mais id disponível para tick");
}

} // namespace api
