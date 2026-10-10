#include "ModuloEleitores.h"

namespace ModuloEleitores{

const void* ModuloEleitores::Dedo::fieldInfos[3] = {
    &tipo::value_type::theInfo,
    &qtdMinucias::value_type::theInfo,
    &minucias::value_type::theInfo
};

int ModuloEleitores::Dedo::fieldIds[3] = {
    -1,
    -1,
    -1
};

const char* ModuloEleitores::Dedo::fieldNames[3] = {
    "tipo",
    "qtdMinucias",
    "minucias"
};

const ModuloEleitores::Dedo::InfoType ModuloEleitores::Dedo::theInfo = {
    ModuloEleitores::Dedo::create,
    0x00000010,
    0,
    false,
    ModuloEleitores::Dedo::fieldInfos,
    ModuloEleitores::Dedo::fieldIds,
    3, 0, 0,
    NULL,
    &ModuloEleitores::Dedo::defaultTag,
    ModuloEleitores::Dedo::fieldNames,
    "Dedo"
};

const char * ModuloEleitores::BiometriaEleitorCifrada::composicaoBiometria::value_type::nameList[] = {
    "<undefined>=0",
    "foto",
    "dedos",
    "fotoDedos"
  };

const ModuloEleitores::BiometriaEleitorCifrada::composicaoBiometria::value_type::InfoType ModuloEleitores::BiometriaEleitorCifrada::composicaoBiometria::value_type::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    3, nameList
};

const void* ModuloEleitores::BiometriaEleitorCifrada::fieldInfos[3] = {
    &composicaoBiometria::value_type::theInfo,
    &conteudo::value_type::theInfo,
    &salt::value_type::theInfo
};

int ModuloEleitores::BiometriaEleitorCifrada::fieldIds[3] = {
    -1,
    -1,
    -1
};

const char* ModuloEleitores::BiometriaEleitorCifrada::fieldNames[3] = {
    "composicaoBiometria",
    "conteudo",
    "salt"
};

const ModuloEleitores::BiometriaEleitorCifrada::InfoType ModuloEleitores::BiometriaEleitorCifrada::theInfo = {
    ModuloEleitores::BiometriaEleitorCifrada::create,
    0x00000010,
    0,
    false,
    ModuloEleitores::BiometriaEleitorCifrada::fieldInfos,
    ModuloEleitores::BiometriaEleitorCifrada::fieldIds,
    3, 0, 0,
    NULL,
    &ModuloEleitores::BiometriaEleitorCifrada::defaultTag,
    ModuloEleitores::BiometriaEleitorCifrada::fieldNames,
    "BiometriaEleitorCifrada"
};

const ModuloEleitores::BiometriaEleitor::dedos::value_type::InfoType ModuloEleitores::BiometriaEleitor::dedos::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x80000002,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloEleitores::Dedo::theInfo,
    "value_type"
};

const void* ModuloEleitores::BiometriaEleitor::fieldInfos[2] = {
    &foto::value_type::theInfo,
    &dedos::value_type::theInfo
};

int ModuloEleitores::BiometriaEleitor::fieldIds[2] = {
    0,
    1
};

unsigned ModuloEleitores::BiometriaEleitor::fieldTags[2] = {
    0x80000001,
    0x80000002
};

const char* ModuloEleitores::BiometriaEleitor::fieldNames[2] = {
    "foto",
    "dedos"
};

const ModuloEleitores::BiometriaEleitor::InfoType ModuloEleitores::BiometriaEleitor::theInfo = {
    ModuloEleitores::BiometriaEleitor::create,
    0x00000010,
    0,
    false,
    ModuloEleitores::BiometriaEleitor::fieldInfos,
    ModuloEleitores::BiometriaEleitor::fieldIds,
    2, 0, 2,
    NULL,
    ModuloEleitores::BiometriaEleitor::fieldTags,
    ModuloEleitores::BiometriaEleitor::fieldNames,
    "BiometriaEleitor"
};

const void* ModuloEleitores::DomicilioEleitoral::fieldInfos[6] = {
    &uf::value_type::theInfo,
    &zona::value_type::theInfo,
    &secao::value_type::theInfo,
    &dataNaUF::value_type::theInfo,
    &dataNoMunicipio::value_type::theInfo,
    &codigoMunicipio::value_type::theInfo
};

int ModuloEleitores::DomicilioEleitoral::fieldIds[6] = {
    -1,
    -1,
    -1,
    -1,
    -1,
    -1
};

const char* ModuloEleitores::DomicilioEleitoral::fieldNames[6] = {
    "uf",
    "zona",
    "secao",
    "dataNaUF",
    "dataNoMunicipio",
    "codigoMunicipio"
};

const ModuloEleitores::DomicilioEleitoral::InfoType ModuloEleitores::DomicilioEleitoral::theInfo = {
    ModuloEleitores::DomicilioEleitoral::create,
    0x00000010,
    0,
    false,
    ModuloEleitores::DomicilioEleitoral::fieldInfos,
    ModuloEleitores::DomicilioEleitoral::fieldIds,
    6, 0, 0,
    NULL,
    &ModuloEleitores::DomicilioEleitoral::defaultTag,
    ModuloEleitores::DomicilioEleitoral::fieldNames,
    "DomicilioEleitoral"
};

const ModuloEleitores::EleitorUrna::identificacaoEleitor::value_type::InfoType ModuloEleitores::EleitorUrna::identificacaoEleitor::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloTiposEleitorais::IdentificadorEleitor::theInfo,
    0
};

const ModuloEleitores::EleitorUrna::nome::value_type::InfoType ModuloEleitores::EleitorUrna::nome::value_type::theInfo = {
    ASN1::AbstractString::create,
    0x0000001b,
    &ASN1::GeneralString::theInfo,
    ASN1::FixedConstraint, 1, 70,
    ASN1::GeneralString::theInfo.characterSet, ASN1::GeneralString::theInfo.characterSetSize, ASN1::GeneralString::theInfo.canonicalSetBits,
    ASN1::GeneralString::theInfo.charSetUnalignedBits, ASN1::GeneralString::theInfo.charSetAlignedBits
};

const ModuloEleitores::EleitorUrna::nomeSocial::value_type::InfoType ModuloEleitores::EleitorUrna::nomeSocial::value_type::theInfo = {
    ASN1::AbstractString::create,
    0x80000001,
    &ASN1::GeneralString::theInfo,
    ASN1::FixedConstraint, 1, 70,
    ASN1::GeneralString::theInfo.characterSet, ASN1::GeneralString::theInfo.characterSetSize, ASN1::GeneralString::theInfo.canonicalSetBits,
    ASN1::GeneralString::theInfo.charSetUnalignedBits, ASN1::GeneralString::theInfo.charSetAlignedBits
};

const ModuloEleitores::EleitorUrna::nomeMae::value_type::InfoType ModuloEleitores::EleitorUrna::nomeMae::value_type::theInfo = {
    ASN1::AbstractString::create,
    0x0000001b,
    &ASN1::GeneralString::theInfo,
    ASN1::FixedConstraint, 1, 70,
    ASN1::GeneralString::theInfo.characterSet, ASN1::GeneralString::theInfo.characterSetSize, ASN1::GeneralString::theInfo.canonicalSetBits,
    ASN1::GeneralString::theInfo.charSetUnalignedBits, ASN1::GeneralString::theInfo.charSetAlignedBits
};

const char * ModuloEleitores::EleitorUrna::situacao::value_type::nameList[] = {
    "regular",
    "<undefined>=1",
    "<undefined>=2",
    "<undefined>=3",
    "cancelado",
    "<undefined>=5",
    "suspenso",
    "envolvidoEmCoincidenciaNaoLiberado",
    "<undefined>=8",
    "envolvidoEmCoincidenciaLiberado"
  };

const ModuloEleitores::EleitorUrna::situacao::value_type::InfoType ModuloEleitores::EleitorUrna::situacao::value_type::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    9, nameList
};

const char * ModuloEleitores::EleitorUrna::necessidade::value_type::nameList[] = {
    "<undefined>=0",
    "semNecessidade",
    "necessitaAudio"
  };

const ModuloEleitores::EleitorUrna::necessidade::value_type::InfoType ModuloEleitores::EleitorUrna::necessidade::value_type::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    2, nameList
};

const void* ModuloEleitores::EleitorUrna::fieldInfos[9] = {
    &identificacaoEleitor::value_type::theInfo,
    &nome::value_type::theInfo,
    &nomeSocial::value_type::theInfo,
    &nomeMae::value_type::theInfo,
    &dataNascimento::value_type::theInfo,
    &situacao::value_type::theInfo,
    &domicilio::value_type::theInfo,
    &necessidade::value_type::theInfo,
    &biometria::value_type::theInfo
};

int ModuloEleitores::EleitorUrna::fieldIds[9] = {
    -1,
    -1,
    0,
    -1,
    -1,
    -1,
    1,
    -1,
    2
};

unsigned ModuloEleitores::EleitorUrna::fieldTags[9] = {
    0x00000010,
    0x0000001b,
    0x80000001,
    0x0000001b,
    0x00000000,
    0x00000000,
    0x80000002,
    0x00000000,
    0x00000000
};

const char* ModuloEleitores::EleitorUrna::fieldNames[9] = {
    "identificacaoEleitor",
    "nome",
    "nomeSocial",
    "nomeMae",
    "dataNascimento",
    "situacao",
    "domicilio",
    "necessidade",
    "biometria"
};

const ModuloEleitores::EleitorUrna::InfoType ModuloEleitores::EleitorUrna::theInfo = {
    ModuloEleitores::EleitorUrna::create,
    0x00000010,
    0,
    false,
    ModuloEleitores::EleitorUrna::fieldInfos,
    ModuloEleitores::EleitorUrna::fieldIds,
    9, 0, 3,
    NULL,
    ModuloEleitores::EleitorUrna::fieldTags,
    ModuloEleitores::EleitorUrna::fieldNames,
    "EleitorUrna"
};

const char * ModuloEleitores::EleitorSequencia::tipoTransferenciaTemporaria::value_type::nameList[] = {
    "<undefined>=0",
    "votoEmTransito",
    "presoProvisorio",
    "servidorEmServico",
    "acessibilidade",
    "eleitorConvocado",
    "justicaEleitoral",
    "oficio",
    "indigenaQuilombola",
    "situacaoRua"
  };

const ModuloEleitores::EleitorSequencia::tipoTransferenciaTemporaria::value_type::InfoType ModuloEleitores::EleitorSequencia::tipoTransferenciaTemporaria::value_type::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    9, nameList
};

const void* ModuloEleitores::EleitorSequencia::fieldInfos[3] = {
    &sequencial::value_type::theInfo,
    &eleitor::value_type::theInfo,
    &tipoTransferenciaTemporaria::value_type::theInfo
};

int ModuloEleitores::EleitorSequencia::fieldIds[3] = {
    -1,
    -1,
    0
};

unsigned ModuloEleitores::EleitorSequencia::fieldTags[3] = {
    0x00000000,
    0x00000000,
    0x80000001
};

const char* ModuloEleitores::EleitorSequencia::fieldNames[3] = {
    "sequencial",
    "eleitor",
    "tipoTransferenciaTemporaria"
};

const ModuloEleitores::EleitorSequencia::InfoType ModuloEleitores::EleitorSequencia::theInfo = {
    ModuloEleitores::EleitorSequencia::create,
    0x00000010,
    0,
    false,
    ModuloEleitores::EleitorSequencia::fieldInfos,
    ModuloEleitores::EleitorSequencia::fieldIds,
    3, 0, 1,
    NULL,
    ModuloEleitores::EleitorSequencia::fieldTags,
    ModuloEleitores::EleitorSequencia::fieldNames,
    "EleitorSequencia"
};

const ModuloEleitores::EntidadeEleitores::eleitores::value_type::InfoType ModuloEleitores::EntidadeEleitores::eleitores::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloEleitores::EleitorSequencia::theInfo,
    0
};

const void* ModuloEleitores::EntidadeEleitores::fieldInfos[4] = {
    &cabecalho::value_type::theInfo,
    &identificacao::value_type::theInfo,
    &seguranca::value_type::theInfo,
    &eleitores::value_type::theInfo
};

int ModuloEleitores::EntidadeEleitores::fieldIds[4] = {
    -1,
    -1,
    0,
    1
};

unsigned ModuloEleitores::EntidadeEleitores::fieldTags[4] = {
    0x00000000,
    0x00000000,
    0x80000001,
    0x00000010
};

const char* ModuloEleitores::EntidadeEleitores::fieldNames[4] = {
    "cabecalho",
    "identificacao",
    "seguranca",
    "eleitores"
};

const ModuloEleitores::EntidadeEleitores::InfoType ModuloEleitores::EntidadeEleitores::theInfo = {
    ModuloEleitores::EntidadeEleitores::create,
    0x00000010,
    0,
    false,
    ModuloEleitores::EntidadeEleitores::fieldInfos,
    ModuloEleitores::EntidadeEleitores::fieldIds,
    4, 0, 2,
    NULL,
    ModuloEleitores::EntidadeEleitores::fieldTags,
    ModuloEleitores::EntidadeEleitores::fieldNames,
    "EntidadeEleitores"
};

} // namespace ModuloEleitores
