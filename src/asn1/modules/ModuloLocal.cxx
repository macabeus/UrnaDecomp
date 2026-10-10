#include "ModuloLocal.h"

namespace ModuloLocal{

const void* ModuloLocal::IdentificacaoUrnaContingencia::fieldInfos[1] = {
    &municipioZona::value_type::theInfo
};

int ModuloLocal::IdentificacaoUrnaContingencia::fieldIds[1] = {
    -1
};

const char* ModuloLocal::IdentificacaoUrnaContingencia::fieldNames[1] = {
    "municipioZona"
};

const ModuloLocal::IdentificacaoUrnaContingencia::InfoType ModuloLocal::IdentificacaoUrnaContingencia::theInfo = {
    ModuloLocal::IdentificacaoUrnaContingencia::create,
    0x00000010,
    0,
    false,
    ModuloLocal::IdentificacaoUrnaContingencia::fieldInfos,
    ModuloLocal::IdentificacaoUrnaContingencia::fieldIds,
    1, 0, 0,
    NULL,
    &ModuloLocal::IdentificacaoUrnaContingencia::defaultTag,
    ModuloLocal::IdentificacaoUrnaContingencia::fieldNames,
    "IdentificacaoUrnaContingencia"
};

const void* ModuloLocal::IdentificacaoAgregada::fieldInfos[2] = {
    &numero::value_type::theInfo,
    &tipoLocalOrigem::value_type::theInfo
};

int ModuloLocal::IdentificacaoAgregada::fieldIds[2] = {
    -1,
    -1
};

const char* ModuloLocal::IdentificacaoAgregada::fieldNames[2] = {
    "numero",
    "tipoLocalOrigem"
};

const ModuloLocal::IdentificacaoAgregada::InfoType ModuloLocal::IdentificacaoAgregada::theInfo = {
    ModuloLocal::IdentificacaoAgregada::create,
    0x00000010,
    0,
    false,
    ModuloLocal::IdentificacaoAgregada::fieldInfos,
    ModuloLocal::IdentificacaoAgregada::fieldIds,
    2, 0, 0,
    NULL,
    &ModuloLocal::IdentificacaoAgregada::defaultTag,
    ModuloLocal::IdentificacaoAgregada::fieldNames,
    "IdentificacaoAgregada"
};

const ModuloLocal::SecaoEleitoral::agregadas::value_type::InfoType ModuloLocal::SecaoEleitoral::agregadas::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloLocal::IdentificacaoAgregada::theInfo,
    0
};

const void* ModuloLocal::SecaoEleitoral::fieldInfos[3] = {
    &tipo::value_type::theInfo,
    &secao::value_type::theInfo,
    &agregadas::value_type::theInfo
};

int ModuloLocal::SecaoEleitoral::fieldIds[3] = {
    -1,
    -1,
    0
};

const char* ModuloLocal::SecaoEleitoral::fieldNames[3] = {
    "tipo",
    "secao",
    "agregadas"
};

const ModuloLocal::SecaoEleitoral::InfoType ModuloLocal::SecaoEleitoral::theInfo = {
    ModuloLocal::SecaoEleitoral::create,
    0x00000010,
    0,
    false,
    ModuloLocal::SecaoEleitoral::fieldInfos,
    ModuloLocal::SecaoEleitoral::fieldIds,
    3, 0, 1,
    NULL,
    &ModuloLocal::SecaoEleitoral::defaultTag,
    ModuloLocal::SecaoEleitoral::fieldNames,
    "SecaoEleitoral"
};

const ModuloLocal::Local::pais::value_type::InfoType ModuloLocal::Local::pais::value_type::theInfo = {
    ASN1::AbstractString::create,
    0x0000001b,
    &ASN1::GeneralString::theInfo,
    ASN1::FixedConstraint, 1, 30,
    ASN1::GeneralString::theInfo.characterSet, ASN1::GeneralString::theInfo.characterSetSize, ASN1::GeneralString::theInfo.canonicalSetBits,
    ASN1::GeneralString::theInfo.charSetUnalignedBits, ASN1::GeneralString::theInfo.charSetAlignedBits
};

const void* ModuloLocal::Local::id::value_type::selectionInfos[2] = {
    &secao::value_type::theInfo,
    &contingencia::value_type::theInfo
};

unsigned ModuloLocal::Local::id::value_type::selectionTags[2] = {
    0xa0000001,
    0xa0000002
};

const char* ModuloLocal::Local::id::value_type::selectionNames[2] = {
    "secao",
    "contingencia"
};

const ModuloLocal::Local::id::value_type::InfoType ModuloLocal::Local::id::value_type::theInfo = {
    ModuloLocal::Local::id::value_type::create,
    0x00000000,
    0,
    false,
    ModuloLocal::Local::id::value_type::selectionInfos,
    2, 2,
    ModuloLocal::Local::id::value_type::selectionTags,
    ModuloLocal::Local::id::value_type::selectionNames
};

const void* ModuloLocal::Local::fieldInfos[6] = {
    &idPE::value_type::theInfo,
    &pais::value_type::theInfo,
    &uf::value_type::theInfo,
    &municipio::value_type::theInfo,
    &complementoMunicipio::value_type::theInfo,
    &id::value_type::theInfo
};

int ModuloLocal::Local::fieldIds[6] = {
    -1,
    -1,
    -1,
    -1,
    -1,
    -1
};

const char* ModuloLocal::Local::fieldNames[6] = {
    "idPE",
    "pais",
    "uf",
    "municipio",
    "complementoMunicipio",
    "id"
};

const ModuloLocal::Local::InfoType ModuloLocal::Local::theInfo = {
    ModuloLocal::Local::create,
    0x00000010,
    0,
    false,
    ModuloLocal::Local::fieldInfos,
    ModuloLocal::Local::fieldIds,
    6, 0, 0,
    NULL,
    &ModuloLocal::Local::defaultTag,
    ModuloLocal::Local::fieldNames,
    "Local"
};

} // namespace ModuloLocal
