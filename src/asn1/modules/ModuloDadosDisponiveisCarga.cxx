#include "ModuloDadosDisponiveisCarga.h"

namespace ModuloDadosDisponiveisCarga{

const ModuloDadosDisponiveisCarga::MunicipioDisponivel::secoes::value_type::InfoType ModuloDadosDisponiveisCarga::MunicipioDisponivel::secoes::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloLocal::SecaoEleitoral::theInfo,
    0
};

const void* ModuloDadosDisponiveisCarga::MunicipioDisponivel::fieldInfos[3] = {
    &municipio::value_type::theInfo,
    &complementoMunicipio::value_type::theInfo,
    &secoes::value_type::theInfo
};

int ModuloDadosDisponiveisCarga::MunicipioDisponivel::fieldIds[3] = {
    -1,
    -1,
    -1
};

const char* ModuloDadosDisponiveisCarga::MunicipioDisponivel::fieldNames[3] = {
    "municipio",
    "complementoMunicipio",
    "secoes"
};

const ModuloDadosDisponiveisCarga::MunicipioDisponivel::InfoType ModuloDadosDisponiveisCarga::MunicipioDisponivel::theInfo = {
    ModuloDadosDisponiveisCarga::MunicipioDisponivel::create,
    0x00000010,
    0,
    false,
    ModuloDadosDisponiveisCarga::MunicipioDisponivel::fieldInfos,
    ModuloDadosDisponiveisCarga::MunicipioDisponivel::fieldIds,
    3, 0, 0,
    NULL,
    &ModuloDadosDisponiveisCarga::MunicipioDisponivel::defaultTag,
    ModuloDadosDisponiveisCarga::MunicipioDisponivel::fieldNames,
    "MunicipioDisponivel"
};

const ModuloDadosDisponiveisCarga::DadosDisponiveisCarga::pais::value_type::InfoType ModuloDadosDisponiveisCarga::DadosDisponiveisCarga::pais::value_type::theInfo = {
    ASN1::AbstractString::create,
    0x0000001b,
    &ASN1::GeneralString::theInfo,
    ASN1::FixedConstraint, 1, 30,
    ASN1::GeneralString::theInfo.characterSet, ASN1::GeneralString::theInfo.characterSetSize, ASN1::GeneralString::theInfo.canonicalSetBits,
    ASN1::GeneralString::theInfo.charSetUnalignedBits, ASN1::GeneralString::theInfo.charSetAlignedBits
};

const ModuloDadosDisponiveisCarga::DadosDisponiveisCarga::municipios::value_type::InfoType ModuloDadosDisponiveisCarga::DadosDisponiveisCarga::municipios::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloDadosDisponiveisCarga::MunicipioDisponivel::theInfo,
    0
};

const ModuloDadosDisponiveisCarga::DadosDisponiveisCarga::pacotesImportados::value_type::InfoType ModuloDadosDisponiveisCarga::DadosDisponiveisCarga::pacotesImportados::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloTiposEleitorais::CabecalhoPacote::theInfo,
    0
};

const void* ModuloDadosDisponiveisCarga::DadosDisponiveisCarga::fieldInfos[9] = {
    &fase::value_type::theInfo,
    &idPE::value_type::theInfo,
    &idPleito::value_type::theInfo,
    &serialFlashCarga::value_type::theInfo,
    &pais::value_type::theInfo,
    &uf::value_type::theInfo,
    &zona::value_type::theInfo,
    &municipios::value_type::theInfo,
    &pacotesImportados::value_type::theInfo
};

int ModuloDadosDisponiveisCarga::DadosDisponiveisCarga::fieldIds[9] = {
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1
};

const char* ModuloDadosDisponiveisCarga::DadosDisponiveisCarga::fieldNames[9] = {
    "fase",
    "idPE",
    "idPleito",
    "serialFlashCarga",
    "pais",
    "uf",
    "zona",
    "municipios",
    "pacotesImportados"
};

const ModuloDadosDisponiveisCarga::DadosDisponiveisCarga::InfoType ModuloDadosDisponiveisCarga::DadosDisponiveisCarga::theInfo = {
    ModuloDadosDisponiveisCarga::DadosDisponiveisCarga::create,
    0x00000010,
    0,
    false,
    ModuloDadosDisponiveisCarga::DadosDisponiveisCarga::fieldInfos,
    ModuloDadosDisponiveisCarga::DadosDisponiveisCarga::fieldIds,
    9, 0, 0,
    NULL,
    &ModuloDadosDisponiveisCarga::DadosDisponiveisCarga::defaultTag,
    ModuloDadosDisponiveisCarga::DadosDisponiveisCarga::fieldNames,
    "DadosDisponiveisCarga"
};

} // namespace ModuloDadosDisponiveisCarga
