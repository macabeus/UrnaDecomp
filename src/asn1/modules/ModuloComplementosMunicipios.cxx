#include "ModuloComplementosMunicipios.h"

namespace ModuloComplementosMunicipios{

const ModuloComplementosMunicipios::InicioHV::InfoType ModuloComplementosMunicipios::InicioHV::theInfo = {
    ASN1::AbstractString::create,
    0x00000012,
    &ASN1::NumericString::theInfo,
    ASN1::FixedConstraint, 8, 8,
    ASN1::NumericString::theInfo.characterSet, ASN1::NumericString::theInfo.characterSetSize, ASN1::NumericString::theInfo.canonicalSetBits,
    ASN1::NumericString::theInfo.charSetUnalignedBits, ASN1::NumericString::theInfo.charSetAlignedBits
};

const void* ModuloComplementosMunicipios::HorarioVerao::fieldInfos[3] = {
    &inicioHV::value_type::theInfo,
    &fimHV::value_type::theInfo,
    &diferencaHV::value_type::theInfo
};

int ModuloComplementosMunicipios::HorarioVerao::fieldIds[3] = {
    -1,
    -1,
    -1
};

const char* ModuloComplementosMunicipios::HorarioVerao::fieldNames[3] = {
    "inicioHV",
    "fimHV",
    "diferencaHV"
};

const ModuloComplementosMunicipios::HorarioVerao::InfoType ModuloComplementosMunicipios::HorarioVerao::theInfo = {
    ModuloComplementosMunicipios::HorarioVerao::create,
    0x00000010,
    0,
    false,
    ModuloComplementosMunicipios::HorarioVerao::fieldInfos,
    ModuloComplementosMunicipios::HorarioVerao::fieldIds,
    3, 0, 0,
    NULL,
    &ModuloComplementosMunicipios::HorarioVerao::defaultTag,
    ModuloComplementosMunicipios::HorarioVerao::fieldNames,
    "HorarioVerao"
};

const void* ModuloComplementosMunicipios::ComplementoMunicipio::fieldInfos[4] = {
    &codigoMunicipio::value_type::theInfo,
    &fuso::value_type::theInfo,
    &desligaColetaBiometria::value_type::theInfo,
    &horarioVerao::value_type::theInfo
};

int ModuloComplementosMunicipios::ComplementoMunicipio::fieldIds[4] = {
    -1,
    -1,
    -1,
    0
};

const char* ModuloComplementosMunicipios::ComplementoMunicipio::fieldNames[4] = {
    "codigoMunicipio",
    "fuso",
    "desligaColetaBiometria",
    "horarioVerao"
};

const ModuloComplementosMunicipios::ComplementoMunicipio::InfoType ModuloComplementosMunicipios::ComplementoMunicipio::theInfo = {
    ModuloComplementosMunicipios::ComplementoMunicipio::create,
    0x00000010,
    0,
    false,
    ModuloComplementosMunicipios::ComplementoMunicipio::fieldInfos,
    ModuloComplementosMunicipios::ComplementoMunicipio::fieldIds,
    4, 0, 1,
    NULL,
    &ModuloComplementosMunicipios::ComplementoMunicipio::defaultTag,
    ModuloComplementosMunicipios::ComplementoMunicipio::fieldNames,
    "ComplementoMunicipio"
};

const ModuloComplementosMunicipios::ComplementosMunicipiosUF::complementosMunicipios::value_type::InfoType ModuloComplementosMunicipios::ComplementosMunicipiosUF::complementosMunicipios::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloComplementosMunicipios::ComplementoMunicipio::theInfo,
    0
};

const void* ModuloComplementosMunicipios::ComplementosMunicipiosUF::fieldInfos[3] = {
    &uf::value_type::theInfo,
    &fusoTRE::value_type::theInfo,
    &complementosMunicipios::value_type::theInfo
};

int ModuloComplementosMunicipios::ComplementosMunicipiosUF::fieldIds[3] = {
    -1,
    -1,
    -1
};

const char* ModuloComplementosMunicipios::ComplementosMunicipiosUF::fieldNames[3] = {
    "uf",
    "fusoTRE",
    "complementosMunicipios"
};

const ModuloComplementosMunicipios::ComplementosMunicipiosUF::InfoType ModuloComplementosMunicipios::ComplementosMunicipiosUF::theInfo = {
    ModuloComplementosMunicipios::ComplementosMunicipiosUF::create,
    0x00000010,
    0,
    false,
    ModuloComplementosMunicipios::ComplementosMunicipiosUF::fieldInfos,
    ModuloComplementosMunicipios::ComplementosMunicipiosUF::fieldIds,
    3, 0, 0,
    NULL,
    &ModuloComplementosMunicipios::ComplementosMunicipiosUF::defaultTag,
    ModuloComplementosMunicipios::ComplementosMunicipiosUF::fieldNames,
    "ComplementosMunicipiosUF"
};

const ModuloComplementosMunicipios::EntidadeComplementosMunicipios::municipiosUf::value_type::InfoType ModuloComplementosMunicipios::EntidadeComplementosMunicipios::municipiosUf::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloComplementosMunicipios::ComplementosMunicipiosUF::theInfo,
    0
};

const void* ModuloComplementosMunicipios::EntidadeComplementosMunicipios::fieldInfos[2] = {
    &cabecalho::value_type::theInfo,
    &municipiosUf::value_type::theInfo
};

int ModuloComplementosMunicipios::EntidadeComplementosMunicipios::fieldIds[2] = {
    -1,
    -1
};

const char* ModuloComplementosMunicipios::EntidadeComplementosMunicipios::fieldNames[2] = {
    "cabecalho",
    "municipiosUf"
};

const ModuloComplementosMunicipios::EntidadeComplementosMunicipios::InfoType ModuloComplementosMunicipios::EntidadeComplementosMunicipios::theInfo = {
    ModuloComplementosMunicipios::EntidadeComplementosMunicipios::create,
    0x00000010,
    0,
    false,
    ModuloComplementosMunicipios::EntidadeComplementosMunicipios::fieldInfos,
    ModuloComplementosMunicipios::EntidadeComplementosMunicipios::fieldIds,
    2, 0, 0,
    NULL,
    &ModuloComplementosMunicipios::EntidadeComplementosMunicipios::defaultTag,
    ModuloComplementosMunicipios::EntidadeComplementosMunicipios::fieldNames,
    "EntidadeComplementosMunicipios"
};

} // namespace ModuloComplementosMunicipios
