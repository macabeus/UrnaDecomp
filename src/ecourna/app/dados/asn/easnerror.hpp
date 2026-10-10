#pragma once
#include "ecourna/api/exception/cbaseerror.hpp"

namespace ecourna::app::dados::asn {

// Names synthesised from the messages; values are the target's throw-site codes.
enum EAsnError {
    EAE_TIPO_ID_CABECALHO_INDEFINIDO = 2235,
    EAE_TIPO_ID_CABECALHO_INVALIDO = 2236,
    EAE_CODIGO_CARGO_INVALIDO = 2245,
    EAE_ESTRUTURA_CODIGO_CARGO_INVALIDA = 2246,
    EAE_FASE_INDEFINIDA = 2248,
    EAE_FASE_INVALIDA = 2249,
    EAE_FASE_DESCONHECIDA = 2250,
    EAE_FASE_ENTIDADE_INVALIDA = 2251,
    EAE_TURNO_INVALIDO = 2252,
    EAE_TURNO_ENTIDADE_INVALIDO = 2253,
    EAE_FORMATO_FOTO_INVALIDO = 2254,
    EAE_FORMATO_FOTO_ENTIDADE_INVALIDO = 2255,
    EAE_IDENTIFICADOR_ELEITOR_INVALIDO = 2268,
};

using CAsnError = api::exception::CBaseError<EAsnError, api::exception::SErrorLimits{2235, 2335}>;

} // namespace ecourna::app::dados::asn
