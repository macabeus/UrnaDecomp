#include "ModuloPartidos.h"

namespace ModuloPartidos{

const ModuloPartidos::Partido::sigla::value_type::InfoType ModuloPartidos::Partido::sigla::value_type::theInfo = {
    ASN1::AbstractString::create,
    0x0000001b,
    &ASN1::GeneralString::theInfo,
    ASN1::FixedConstraint, 1, 24,
    ASN1::GeneralString::theInfo.characterSet, ASN1::GeneralString::theInfo.characterSetSize, ASN1::GeneralString::theInfo.canonicalSetBits,
    ASN1::GeneralString::theInfo.charSetUnalignedBits, ASN1::GeneralString::theInfo.charSetAlignedBits
};

const ModuloPartidos::Partido::nome::value_type::InfoType ModuloPartidos::Partido::nome::value_type::theInfo = {
    ASN1::AbstractString::create,
    0x0000001b,
    &ASN1::GeneralString::theInfo,
    ASN1::FixedConstraint, 1, 55,
    ASN1::GeneralString::theInfo.characterSet, ASN1::GeneralString::theInfo.characterSetSize, ASN1::GeneralString::theInfo.canonicalSetBits,
    ASN1::GeneralString::theInfo.charSetUnalignedBits, ASN1::GeneralString::theInfo.charSetAlignedBits
};

const void* ModuloPartidos::Partido::fieldInfos[3] = {
    &numero::value_type::theInfo,
    &sigla::value_type::theInfo,
    &nome::value_type::theInfo
};

int ModuloPartidos::Partido::fieldIds[3] = {
    -1,
    -1,
    -1
};

const char* ModuloPartidos::Partido::fieldNames[3] = {
    "numero",
    "sigla",
    "nome"
};

const ModuloPartidos::Partido::InfoType ModuloPartidos::Partido::theInfo = {
    ModuloPartidos::Partido::create,
    0x00000010,
    0,
    false,
    ModuloPartidos::Partido::fieldInfos,
    ModuloPartidos::Partido::fieldIds,
    3, 0, 0,
    NULL,
    &ModuloPartidos::Partido::defaultTag,
    ModuloPartidos::Partido::fieldNames,
    "Partido"
};

const ModuloPartidos::EntidadePartidos::partidos::value_type::InfoType ModuloPartidos::EntidadePartidos::partidos::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloPartidos::Partido::theInfo,
    0
};

const void* ModuloPartidos::EntidadePartidos::fieldInfos[3] = {
    &cabecalho::value_type::theInfo,
    &abrangencia::value_type::theInfo,
    &partidos::value_type::theInfo
};

int ModuloPartidos::EntidadePartidos::fieldIds[3] = {
    -1,
    -1,
    -1
};

const char* ModuloPartidos::EntidadePartidos::fieldNames[3] = {
    "cabecalho",
    "abrangencia",
    "partidos"
};

const ModuloPartidos::EntidadePartidos::InfoType ModuloPartidos::EntidadePartidos::theInfo = {
    ModuloPartidos::EntidadePartidos::create,
    0x00000010,
    0,
    false,
    ModuloPartidos::EntidadePartidos::fieldInfos,
    ModuloPartidos::EntidadePartidos::fieldIds,
    3, 0, 0,
    NULL,
    &ModuloPartidos::EntidadePartidos::defaultTag,
    ModuloPartidos::EntidadePartidos::fieldNames,
    "EntidadePartidos"
};

} // namespace ModuloPartidos
