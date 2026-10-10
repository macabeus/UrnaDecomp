#pragma once
#include "ecourna/api/exception/cbaseerror.hpp"

namespace ecourna::api::sql {

enum ESqlError {
    ESE_ABRIR_BANCO = 1725,
    ESE_INICIALIZAR_BANCO = 1726,
    ESE_PREPARAR_COMANDO = 1727,
    ESE_FECHAR_BANCO = 1728,
    ESE_SEM_RESULTADO_INT = 1729,
    ESE_SEM_RESULTADO_INT64 = 1730,
    ESE_SEM_RESULTADO_DOUBLE = 1731,
    ESE_SEM_RESULTADO_TEXT = 1732,
    ESE_SEM_RESULTADO_NULL = 1733,
    ESE_SEM_RESULTADO_BLOB = 1734,
    ESE_INDICE_COLUNA = 1735,
    ESE_NOME_COLUNA = 1736,
    ESE_EXECUTAR_PASSO = 1737,
    ESE_SET_INT = 1738,
    ESE_SET_INT64 = 1739,
    ESE_SET_DOUBLE = 1740,
    ESE_SET_TEXT = 1741,
    ESE_SET_BLOB = 1742,
    ESE_SET_NULL = 1743,
    ESE_RESET = 1744,
    ESE_FECHAR_COMANDO = 1745,
    ESE_INDICE_PARAMETRO = 1746,
};

using CSqlError = exception::CBaseError<ESqlError, exception::SErrorLimits{1725, 1775}>;

} // namespace ecourna::api::sql
