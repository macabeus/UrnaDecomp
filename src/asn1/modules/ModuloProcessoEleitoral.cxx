#include "ModuloProcessoEleitoral.h"

namespace ModuloProcessoEleitoral{

const ModuloProcessoEleitoral::Pleito::nome::value_type::InfoType ModuloProcessoEleitoral::Pleito::nome::value_type::theInfo = {
    ASN1::AbstractString::create,
    0x0000001b,
    &ASN1::GeneralString::theInfo,
    ASN1::FixedConstraint, 1, 38,
    ASN1::GeneralString::theInfo.characterSet, ASN1::GeneralString::theInfo.characterSetSize, ASN1::GeneralString::theInfo.canonicalSetBits,
    ASN1::GeneralString::theInfo.charSetUnalignedBits, ASN1::GeneralString::theInfo.charSetAlignedBits
};

const void* ModuloProcessoEleitoral::Pleito::fieldInfos[3] = {
    &id::value_type::theInfo,
    &nome::value_type::theInfo,
    &data::value_type::theInfo
};

int ModuloProcessoEleitoral::Pleito::fieldIds[3] = {
    -1,
    -1,
    -1
};

const char* ModuloProcessoEleitoral::Pleito::fieldNames[3] = {
    "id",
    "nome",
    "data"
};

const ModuloProcessoEleitoral::Pleito::InfoType ModuloProcessoEleitoral::Pleito::theInfo = {
    ModuloProcessoEleitoral::Pleito::create,
    0x00000010,
    0,
    false,
    ModuloProcessoEleitoral::Pleito::fieldInfos,
    ModuloProcessoEleitoral::Pleito::fieldIds,
    3, 0, 0,
    NULL,
    &ModuloProcessoEleitoral::Pleito::defaultTag,
    ModuloProcessoEleitoral::Pleito::fieldNames,
    "Pleito"
};

const char * ModuloProcessoEleitoral::OrigemConfiguracao::nameList[] = {
    "<undefined>=0",
    "oficial",
    "comunitaria"
  };

const ModuloProcessoEleitoral::OrigemConfiguracao::InfoType ModuloProcessoEleitoral::OrigemConfiguracao::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    2, nameList
};

const ModuloProcessoEleitoral::EntidadeProcessoEleitoral::nome::value_type::InfoType ModuloProcessoEleitoral::EntidadeProcessoEleitoral::nome::value_type::theInfo = {
    ASN1::AbstractString::create,
    0x0000001b,
    &ASN1::GeneralString::theInfo,
    ASN1::FixedConstraint, 1, 38,
    ASN1::GeneralString::theInfo.characterSet, ASN1::GeneralString::theInfo.characterSetSize, ASN1::GeneralString::theInfo.canonicalSetBits,
    ASN1::GeneralString::theInfo.charSetUnalignedBits, ASN1::GeneralString::theInfo.charSetAlignedBits
};

const ModuloProcessoEleitoral::EntidadeProcessoEleitoral::tiposTTEPermitidos::value_type::InfoType ModuloProcessoEleitoral::EntidadeProcessoEleitoral::tiposTTEPermitidos::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloEleitores::EleitorSequencia::tipoTransferenciaTemporaria::value_type::theInfo,
    0
};

const ModuloProcessoEleitoral::EntidadeProcessoEleitoral::tiposIdentificadoresPermitidos::value_type::InfoType ModuloProcessoEleitoral::EntidadeProcessoEleitoral::tiposIdentificadoresPermitidos::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloTiposEleitorais::TipoIdentificadorEleitor::theInfo,
    0
};

const ModuloProcessoEleitoral::EntidadeProcessoEleitoral::ufs::value_type::InfoType ModuloProcessoEleitoral::EntidadeProcessoEleitoral::ufs::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloTiposEleitorais::SiglaUF::theInfo,
    0
};

const void* ModuloProcessoEleitoral::EntidadeProcessoEleitoral::fieldInfos[14] = {
    &cabecalho::value_type::theInfo,
    &nome::value_type::theInfo,
    &dataEleitorado::value_type::theInfo,
    &pleito1::value_type::theInfo,
    &pleito2::value_type::theInfo,
    &tipoEleicaoPrincipal::value_type::theInfo,
    &abrangenciaEleicaoPrincipal::value_type::theInfo,
    &permiteMRJ::value_type::theInfo,
    &utilizaBiometria::value_type::theInfo,
    &tiposTTEPermitidos::value_type::theInfo,
    &tipoIdentificadorPrincipalEleitor::value_type::theInfo,
    &tiposIdentificadoresPermitidos::value_type::theInfo,
    &origemConfiguracao::value_type::theInfo,
    &ufs::value_type::theInfo
};

int ModuloProcessoEleitoral::EntidadeProcessoEleitoral::fieldIds[14] = {
    -1,
    -1,
    0,
    -1,
    1,
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

const char* ModuloProcessoEleitoral::EntidadeProcessoEleitoral::fieldNames[14] = {
    "cabecalho",
    "nome",
    "dataEleitorado",
    "pleito1",
    "pleito2",
    "tipoEleicaoPrincipal",
    "abrangenciaEleicaoPrincipal",
    "permiteMRJ",
    "utilizaBiometria",
    "tiposTTEPermitidos",
    "tipoIdentificadorPrincipalEleitor",
    "tiposIdentificadoresPermitidos",
    "origemConfiguracao",
    "ufs"
};

const ModuloProcessoEleitoral::EntidadeProcessoEleitoral::InfoType ModuloProcessoEleitoral::EntidadeProcessoEleitoral::theInfo = {
    ModuloProcessoEleitoral::EntidadeProcessoEleitoral::create,
    0x00000010,
    0,
    false,
    ModuloProcessoEleitoral::EntidadeProcessoEleitoral::fieldInfos,
    ModuloProcessoEleitoral::EntidadeProcessoEleitoral::fieldIds,
    14, 0, 2,
    NULL,
    &ModuloProcessoEleitoral::EntidadeProcessoEleitoral::defaultTag,
    ModuloProcessoEleitoral::EntidadeProcessoEleitoral::fieldNames,
    "EntidadeProcessoEleitoral"
};

} // namespace ModuloProcessoEleitoral
