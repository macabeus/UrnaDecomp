#pragma once

#include "ecourna/api/exception/cbaseerror.hpp"

namespace api {

enum EUeIpcError {
    EUE_IPC_FALHA_CRIAR_MUTEX = 6150,
    EUE_IPC_FALHA_TRAVAR_MUTEX = 6151,
    EUE_IPC_FALHA_DESTRAVAR_MUTEX = 6152,
    EUE_IPC_FALHA_CRIAR_SEMAFORO = 6159,
    EUE_IPC_FALHA_RELOGIO = 6160,
    EUE_IPC_FALHA_AGUARDAR_SEMAFORO = 6161,
    EUE_IPC_FALHA_TRAVAR_SEMAFORO = 6162,
    EUE_IPC_FALHA_DESTRAVAR_SEMAFORO = 6163,
    EUE_IPC_FALHA_VALOR_SEMAFORO = 6164,
};

using CUeIpcError = ecourna::api::exception::CBaseError<EUeIpcError, ecourna::api::exception::SErrorLimits{6150, 6350}>;

} // namespace api
