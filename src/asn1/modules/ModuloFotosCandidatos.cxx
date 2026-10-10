#include "ModuloFotosCandidatos.h"

namespace ModuloFotosCandidatos{

const ModuloFotosCandidatos::CodigoCandidato::InfoType ModuloFotosCandidatos::CodigoCandidato::theInfo = {
    ASN1::AbstractString::create,
    0x0000001b,
    &ASN1::GeneralString::theInfo,
    ASN1::FixedConstraint, 0, 18,
    ASN1::GeneralString::theInfo.characterSet, ASN1::GeneralString::theInfo.characterSetSize, ASN1::GeneralString::theInfo.canonicalSetBits,
    ASN1::GeneralString::theInfo.charSetUnalignedBits, ASN1::GeneralString::theInfo.charSetAlignedBits
};

const void* ModuloFotosCandidatos::FotoCandidato::fieldInfos[2] = {
    &codigoCandidato::value_type::theInfo,
    &foto::value_type::theInfo
};

int ModuloFotosCandidatos::FotoCandidato::fieldIds[2] = {
    -1,
    -1
};

const char* ModuloFotosCandidatos::FotoCandidato::fieldNames[2] = {
    "codigoCandidato",
    "foto"
};

const ModuloFotosCandidatos::FotoCandidato::InfoType ModuloFotosCandidatos::FotoCandidato::theInfo = {
    ModuloFotosCandidatos::FotoCandidato::create,
    0x00000010,
    0,
    false,
    ModuloFotosCandidatos::FotoCandidato::fieldInfos,
    ModuloFotosCandidatos::FotoCandidato::fieldIds,
    2, 0, 0,
    NULL,
    &ModuloFotosCandidatos::FotoCandidato::defaultTag,
    ModuloFotosCandidatos::FotoCandidato::fieldNames,
    "FotoCandidato"
};

const ModuloFotosCandidatos::EntidadeFotosCandidatos::fotos::value_type::InfoType ModuloFotosCandidatos::EntidadeFotosCandidatos::fotos::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloFotosCandidatos::FotoCandidato::theInfo,
    0
};

const void* ModuloFotosCandidatos::EntidadeFotosCandidatos::fieldInfos[3] = {
    &cabecalho::value_type::theInfo,
    &abrangencia::value_type::theInfo,
    &fotos::value_type::theInfo
};

int ModuloFotosCandidatos::EntidadeFotosCandidatos::fieldIds[3] = {
    -1,
    -1,
    -1
};

const char* ModuloFotosCandidatos::EntidadeFotosCandidatos::fieldNames[3] = {
    "cabecalho",
    "abrangencia",
    "fotos"
};

const ModuloFotosCandidatos::EntidadeFotosCandidatos::InfoType ModuloFotosCandidatos::EntidadeFotosCandidatos::theInfo = {
    ModuloFotosCandidatos::EntidadeFotosCandidatos::create,
    0x00000010,
    0,
    false,
    ModuloFotosCandidatos::EntidadeFotosCandidatos::fieldInfos,
    ModuloFotosCandidatos::EntidadeFotosCandidatos::fieldIds,
    3, 0, 0,
    NULL,
    &ModuloFotosCandidatos::EntidadeFotosCandidatos::defaultTag,
    ModuloFotosCandidatos::EntidadeFotosCandidatos::fieldNames,
    "EntidadeFotosCandidatos"
};

} // namespace ModuloFotosCandidatos
