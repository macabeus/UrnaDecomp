#include "ModuloHashes.h"

namespace ModuloHashes{

const ModuloHashes::HashesArquivos::InfoType ModuloHashes::HashesArquivos::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloTiposEleitorais::ArquivoAssinatura::theInfo,
    0
};

const void* ModuloHashes::EntidadeHashes::fieldInfos[6] = {
    &cabecalho::value_type::theInfo,
    &fase::value_type::theInfo,
    &siglaUF::value_type::theInfo,
    &identificacaoUrna::value_type::theInfo,
    &versaoUENUX::value_type::theInfo,
    &hashesArquivos::value_type::theInfo
};

int ModuloHashes::EntidadeHashes::fieldIds[6] = {
    -1,
    -1,
    -1,
    0,
    -1,
    -1
};

const char* ModuloHashes::EntidadeHashes::fieldNames[6] = {
    "cabecalho",
    "fase",
    "siglaUF",
    "identificacaoUrna",
    "versaoUENUX",
    "hashesArquivos"
};

const ModuloHashes::EntidadeHashes::InfoType ModuloHashes::EntidadeHashes::theInfo = {
    ModuloHashes::EntidadeHashes::create,
    0x00000010,
    0,
    false,
    ModuloHashes::EntidadeHashes::fieldInfos,
    ModuloHashes::EntidadeHashes::fieldIds,
    6, 0, 1,
    NULL,
    &ModuloHashes::EntidadeHashes::defaultTag,
    ModuloHashes::EntidadeHashes::fieldNames,
    "EntidadeHashes"
};

const char * ModuloHashes::ImpedimentoP1::nameList[] = {
    "<undefined>=0",
    "semImpedimento",
    "votaNaSecaoOriginal",
    "solicitouVotoEmTransito",
    "presoProvisorio",
    "suspenso",
    "cancelado",
    "naoLiberado",
    "semIdadeMinima",
    "militarEmServico",
    "tteAcessibilidade",
    "tteEleitorConvocado",
    "tteJusticaEleitoral",
    "tteOficio",
    "tteIndigenasQuilombolas",
    "tteSituacaoRua"
  };

const ModuloHashes::ImpedimentoP1::InfoType ModuloHashes::ImpedimentoP1::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    15, nameList
};

} // namespace ModuloHashes
