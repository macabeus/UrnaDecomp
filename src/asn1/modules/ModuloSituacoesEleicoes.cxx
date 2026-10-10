#include "ModuloSituacoesEleicoes.h"

namespace ModuloSituacoesEleicoes{

const char * ModuloSituacoesEleicoes::SituacaoEleicao::tipo::value_type::nameList[] = {
    "ativa",
    "suspensa",
    "cancelada",
    "excluida"
  };

const ModuloSituacoesEleicoes::SituacaoEleicao::tipo::value_type::InfoType ModuloSituacoesEleicoes::SituacaoEleicao::tipo::value_type::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    3, nameList
};

const void* ModuloSituacoesEleicoes::SituacaoEleicao::fieldInfos[5] = {
    &idEleicao::value_type::theInfo,
    &tipo::value_type::theInfo,
    &ordemAquisicao::value_type::theInfo,
    &ordemImpressao::value_type::theInfo,
    &ordemApuracao::value_type::theInfo
};

int ModuloSituacoesEleicoes::SituacaoEleicao::fieldIds[5] = {
    -1,
    -1,
    -1,
    -1,
    -1
};

const char* ModuloSituacoesEleicoes::SituacaoEleicao::fieldNames[5] = {
    "idEleicao",
    "tipo",
    "ordemAquisicao",
    "ordemImpressao",
    "ordemApuracao"
};

const ModuloSituacoesEleicoes::SituacaoEleicao::InfoType ModuloSituacoesEleicoes::SituacaoEleicao::theInfo = {
    ModuloSituacoesEleicoes::SituacaoEleicao::create,
    0x00000010,
    0,
    false,
    ModuloSituacoesEleicoes::SituacaoEleicao::fieldInfos,
    ModuloSituacoesEleicoes::SituacaoEleicao::fieldIds,
    5, 0, 0,
    NULL,
    &ModuloSituacoesEleicoes::SituacaoEleicao::defaultTag,
    ModuloSituacoesEleicoes::SituacaoEleicao::fieldNames,
    "SituacaoEleicao"
};

const ModuloSituacoesEleicoes::EntidadeSituacoesEleicoes::situacoesEleicoes::value_type::InfoType ModuloSituacoesEleicoes::EntidadeSituacoesEleicoes::situacoesEleicoes::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloSituacoesEleicoes::SituacaoEleicao::theInfo,
    0
};

const void* ModuloSituacoesEleicoes::EntidadeSituacoesEleicoes::fieldInfos[2] = {
    &cabecalho::value_type::theInfo,
    &situacoesEleicoes::value_type::theInfo
};

int ModuloSituacoesEleicoes::EntidadeSituacoesEleicoes::fieldIds[2] = {
    -1,
    0
};

const char* ModuloSituacoesEleicoes::EntidadeSituacoesEleicoes::fieldNames[2] = {
    "cabecalho",
    "situacoesEleicoes"
};

const ModuloSituacoesEleicoes::EntidadeSituacoesEleicoes::InfoType ModuloSituacoesEleicoes::EntidadeSituacoesEleicoes::theInfo = {
    ModuloSituacoesEleicoes::EntidadeSituacoesEleicoes::create,
    0x00000010,
    0,
    false,
    ModuloSituacoesEleicoes::EntidadeSituacoesEleicoes::fieldInfos,
    ModuloSituacoesEleicoes::EntidadeSituacoesEleicoes::fieldIds,
    2, 0, 1,
    NULL,
    &ModuloSituacoesEleicoes::EntidadeSituacoesEleicoes::defaultTag,
    ModuloSituacoesEleicoes::EntidadeSituacoesEleicoes::fieldNames,
    "EntidadeSituacoesEleicoes"
};

} // namespace ModuloSituacoesEleicoes
