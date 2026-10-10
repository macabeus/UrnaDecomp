#pragma once
#include "ecourna/api/exception/cbaseerror.hpp"

namespace ecourna::app::dados {

// Values are the target's (each throw site's code); the names of 1951, 2014 and 2027..2030 are synthesised.
enum EDadosError {
    EDE_SERIAL_MIDIA_INVALIDO = 1951,
    EDE_ERRO_LEITURA_BIOMETRIA_INVALIDO = 2014,
    EDE_IDENTIFICADOR_NAO_NUMERICO = 2023,
    EDE_TAMANHO_INSCRICAO = 2024,
    EDE_TAMANHO_CPF = 2025,
    EDE_TAMANHO_IDENTIFICACAO_LIVRE = 2026,
    EDE_IDENTIFICADOR_PRINCIPAL_NAO_DEFINIDO = 2027,
    EDE_IDENTIFICADOR_HABILITACAO_NAO_DEFINIDO = 2028,
    EDE_REGISTRO_SEM_HABILITACAO = 2029,
    EDE_REGISTRO_SEM_HABILITACAO_COM_PRINCIPAL = 2030,
};

using CDadosError = api::exception::CBaseError<EDadosError, api::exception::SErrorLimits{1935, 2135}>;

} // namespace ecourna::app::dados
