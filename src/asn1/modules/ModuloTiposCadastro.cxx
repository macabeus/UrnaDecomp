#include "ModuloTiposCadastro.h"

namespace ModuloTiposCadastro{

const char * ModuloTiposCadastro::TipoLocalVotacao::nameList[] = {
    "<undefined>=0",
    "normal",
    "emTransito",
    "presoProvisorio",
    "temporario"
  };

const ModuloTiposCadastro::TipoLocalVotacao::InfoType ModuloTiposCadastro::TipoLocalVotacao::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    4, nameList
};

const ModuloTiposCadastro::Municipio::nome::value_type::InfoType ModuloTiposCadastro::Municipio::nome::value_type::theInfo = {
    ASN1::AbstractString::create,
    0x0000001b,
    &ASN1::GeneralString::theInfo,
    ASN1::FixedConstraint, 1, 35,
    ASN1::GeneralString::theInfo.characterSet, ASN1::GeneralString::theInfo.characterSetSize, ASN1::GeneralString::theInfo.canonicalSetBits,
    ASN1::GeneralString::theInfo.charSetUnalignedBits, ASN1::GeneralString::theInfo.charSetAlignedBits
};

const void* ModuloTiposCadastro::Municipio::fieldInfos[5] = {
    &codigo::value_type::theInfo,
    &nome::value_type::theInfo,
    &capital::value_type::theInfo,
    &codigoIBGE::value_type::theInfo,
    &comBiometria::value_type::theInfo
};

int ModuloTiposCadastro::Municipio::fieldIds[5] = {
    -1,
    -1,
    -1,
    0,
    1
};

unsigned ModuloTiposCadastro::Municipio::fieldTags[5] = {
    0x00000000,
    0x0000001b,
    0x00000001,
    0x80000001,
    0x80000002
};

const char* ModuloTiposCadastro::Municipio::fieldNames[5] = {
    "codigo",
    "nome",
    "capital",
    "codigoIBGE",
    "comBiometria"
};

const ModuloTiposCadastro::Municipio::InfoType ModuloTiposCadastro::Municipio::theInfo = {
    ModuloTiposCadastro::Municipio::create,
    0x00000010,
    0,
    false,
    ModuloTiposCadastro::Municipio::fieldInfos,
    ModuloTiposCadastro::Municipio::fieldIds,
    5, 0, 2,
    NULL,
    ModuloTiposCadastro::Municipio::fieldTags,
    ModuloTiposCadastro::Municipio::fieldNames,
    "Municipio"
};

} // namespace ModuloTiposCadastro
