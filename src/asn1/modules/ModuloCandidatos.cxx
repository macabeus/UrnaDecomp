#include "ModuloCandidatos.h"

namespace ModuloCandidatos{

const ModuloCandidatos::CandidatosPorCargos::partidos::value_type::InfoType ModuloCandidatos::CandidatosPorCargos::partidos::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloTiposResultadosEcoUrna::CandidatoPorPartido::theInfo,
    0
};

const void* ModuloCandidatos::CandidatosPorCargos::fieldInfos[3] = {
    &cargo::value_type::theInfo,
    &quantidadeVagas::value_type::theInfo,
    &partidos::value_type::theInfo
};

int ModuloCandidatos::CandidatosPorCargos::fieldIds[3] = {
    -1,
    -1,
    0
};

const char* ModuloCandidatos::CandidatosPorCargos::fieldNames[3] = {
    "cargo",
    "quantidadeVagas",
    "partidos"
};

const ModuloCandidatos::CandidatosPorCargos::InfoType ModuloCandidatos::CandidatosPorCargos::theInfo = {
    ModuloCandidatos::CandidatosPorCargos::create,
    0x00000010,
    0,
    false,
    ModuloCandidatos::CandidatosPorCargos::fieldInfos,
    ModuloCandidatos::CandidatosPorCargos::fieldIds,
    3, 0, 1,
    NULL,
    &ModuloCandidatos::CandidatosPorCargos::defaultTag,
    ModuloCandidatos::CandidatosPorCargos::fieldNames,
    "CandidatosPorCargos"
};

const ModuloCandidatos::EntidadeCandidatos::cargos::value_type::InfoType ModuloCandidatos::EntidadeCandidatos::cargos::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloCandidatos::CandidatosPorCargos::theInfo,
    0
};

const void* ModuloCandidatos::EntidadeCandidatos::fieldInfos[3] = {
    &cabecalho::value_type::theInfo,
    &abrangencia::value_type::theInfo,
    &cargos::value_type::theInfo
};

int ModuloCandidatos::EntidadeCandidatos::fieldIds[3] = {
    -1,
    -1,
    -1
};

const char* ModuloCandidatos::EntidadeCandidatos::fieldNames[3] = {
    "cabecalho",
    "abrangencia",
    "cargos"
};

const ModuloCandidatos::EntidadeCandidatos::InfoType ModuloCandidatos::EntidadeCandidatos::theInfo = {
    ModuloCandidatos::EntidadeCandidatos::create,
    0x00000010,
    0,
    false,
    ModuloCandidatos::EntidadeCandidatos::fieldInfos,
    ModuloCandidatos::EntidadeCandidatos::fieldIds,
    3, 0, 0,
    NULL,
    &ModuloCandidatos::EntidadeCandidatos::defaultTag,
    ModuloCandidatos::EntidadeCandidatos::fieldNames,
    "EntidadeCandidatos"
};

} // namespace ModuloCandidatos
