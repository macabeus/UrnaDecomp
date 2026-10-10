#include "ModuloFederacoes.h"

namespace ModuloFederacoes{

const ModuloFederacoes::Federacao::sigla::value_type::InfoType ModuloFederacoes::Federacao::sigla::value_type::theInfo = {
    ASN1::AbstractString::create,
    0x0000001b,
    &ASN1::GeneralString::theInfo,
    ASN1::FixedConstraint, 1, 55,
    ASN1::GeneralString::theInfo.characterSet, ASN1::GeneralString::theInfo.characterSetSize, ASN1::GeneralString::theInfo.canonicalSetBits,
    ASN1::GeneralString::theInfo.charSetUnalignedBits, ASN1::GeneralString::theInfo.charSetAlignedBits
};

const ModuloFederacoes::Federacao::nome::value_type::InfoType ModuloFederacoes::Federacao::nome::value_type::theInfo = {
    ASN1::AbstractString::create,
    0x0000001b,
    &ASN1::GeneralString::theInfo,
    ASN1::FixedConstraint, 1, 84,
    ASN1::GeneralString::theInfo.characterSet, ASN1::GeneralString::theInfo.characterSetSize, ASN1::GeneralString::theInfo.canonicalSetBits,
    ASN1::GeneralString::theInfo.charSetUnalignedBits, ASN1::GeneralString::theInfo.charSetAlignedBits
};

const ModuloFederacoes::Federacao::partidos::value_type::InfoType ModuloFederacoes::Federacao::partidos::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 99>::theInfo,
    0
};

const void* ModuloFederacoes::Federacao::fieldInfos[4] = {
    &identificador::value_type::theInfo,
    &sigla::value_type::theInfo,
    &nome::value_type::theInfo,
    &partidos::value_type::theInfo
};

int ModuloFederacoes::Federacao::fieldIds[4] = {
    -1,
    -1,
    -1,
    -1
};

const char* ModuloFederacoes::Federacao::fieldNames[4] = {
    "identificador",
    "sigla",
    "nome",
    "partidos"
};

const ModuloFederacoes::Federacao::InfoType ModuloFederacoes::Federacao::theInfo = {
    ModuloFederacoes::Federacao::create,
    0x00000010,
    0,
    false,
    ModuloFederacoes::Federacao::fieldInfos,
    ModuloFederacoes::Federacao::fieldIds,
    4, 0, 0,
    NULL,
    &ModuloFederacoes::Federacao::defaultTag,
    ModuloFederacoes::Federacao::fieldNames,
    "Federacao"
};

const ModuloFederacoes::EntidadeFederacoes::federacoes::value_type::InfoType ModuloFederacoes::EntidadeFederacoes::federacoes::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloFederacoes::Federacao::theInfo,
    0
};

const void* ModuloFederacoes::EntidadeFederacoes::fieldInfos[2] = {
    &cabecalho::value_type::theInfo,
    &federacoes::value_type::theInfo
};

int ModuloFederacoes::EntidadeFederacoes::fieldIds[2] = {
    -1,
    0
};

const char* ModuloFederacoes::EntidadeFederacoes::fieldNames[2] = {
    "cabecalho",
    "federacoes"
};

const ModuloFederacoes::EntidadeFederacoes::InfoType ModuloFederacoes::EntidadeFederacoes::theInfo = {
    ModuloFederacoes::EntidadeFederacoes::create,
    0x00000010,
    0,
    false,
    ModuloFederacoes::EntidadeFederacoes::fieldInfos,
    ModuloFederacoes::EntidadeFederacoes::fieldIds,
    2, 0, 1,
    NULL,
    &ModuloFederacoes::EntidadeFederacoes::defaultTag,
    ModuloFederacoes::EntidadeFederacoes::fieldNames,
    "EntidadeFederacoes"
};

} // namespace ModuloFederacoes
