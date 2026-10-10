#pragma once
#include "ecourna/api/exception/cbaseerror.hpp"

namespace ecourna::api::compression {

// Codes are the target's (the names are ours): 1013 clzmacompress.cpp, 1039-1046 czip.cpp, 1047 icompressor.cpp.
enum ECompressionError {
    ECE_LZMA_INCLUIR_ARQUIVO = 1013,
    ECE_ZIP_FECHAR = 1039,
    ECE_ZIP_MODO_ABERTURA = 1040,
    ECE_ZIP_CRIAR = 1041,
    ECE_ZIP_FECHADO = 1042,
    ECE_ZIP_MODO_COMPRESSAO = 1043,
    ECE_ZIP_ADICIONAR = 1044,
    ECE_ZIP_ESCREVER = 1045,
    ECE_ZIP_CONCLUIR = 1046,
    ECE_GLOB_INVALIDO = 1047,
};

using CCompressionError = exception::CBaseError<ECompressionError, exception::SErrorLimits{1000, 1100}>;

} // namespace ecourna::api::compression
