#pragma once
#include "ecourna/api/exception/cbaseerror.hpp"

namespace ecourna::api::util {

enum EUtilError {
    EUE_VALOR_INVALIDO = 1875,
    EUE_VALOR_NAO_NUMERICO = 1876,
    EUE_VALOR_FORA_DA_FAIXA = 1877,
    EUE_HEX_INVALIDO = 1878,
    // cbitarray.cpp
    EUE_BITARRAY_SEM_ESCRITA = 1882,
    EUE_BITARRAY_NAO_ALOCADO = 1883,
    EUE_BITARRAY_POSICAO_INVALIDA = 1884,
    // csynchronizer.cpp
    EUE_SYNCHRONIZER_INSTANCIA_JA_CRIADA = 1889,
};

using CUtilError = exception::CBaseError<EUtilError, exception::SErrorLimits{1875, 1900}>;

} // namespace ecourna::api::util
