#include "ModuloImpedidos.h"

namespace ModuloImpedidos{

const void* ModuloImpedidos::EleitorImpedido::fieldInfos[3] = {
    &identificadorEleitor::value_type::theInfo,
    &impedimentoP1::value_type::theInfo,
    &impedimentoP2::value_type::theInfo
};

int ModuloImpedidos::EleitorImpedido::fieldIds[3] = {
    -1,
    -1,
    0
};

const char* ModuloImpedidos::EleitorImpedido::fieldNames[3] = {
    "identificadorEleitor",
    "impedimentoP1",
    "impedimentoP2"
};

const ModuloImpedidos::EleitorImpedido::InfoType ModuloImpedidos::EleitorImpedido::theInfo = {
    ModuloImpedidos::EleitorImpedido::create,
    0x00000010,
    0,
    false,
    ModuloImpedidos::EleitorImpedido::fieldInfos,
    ModuloImpedidos::EleitorImpedido::fieldIds,
    3, 0, 1,
    NULL,
    &ModuloImpedidos::EleitorImpedido::defaultTag,
    ModuloImpedidos::EleitorImpedido::fieldNames,
    "EleitorImpedido"
};

const ModuloImpedidos::EntidadeImpedidos::eleitores::value_type::InfoType ModuloImpedidos::EntidadeImpedidos::eleitores::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloImpedidos::EleitorImpedido::theInfo,
    0
};

const void* ModuloImpedidos::EntidadeImpedidos::fieldInfos[3] = {
    &cabecalho::value_type::theInfo,
    &identificacao::value_type::theInfo,
    &eleitores::value_type::theInfo
};

int ModuloImpedidos::EntidadeImpedidos::fieldIds[3] = {
    -1,
    -1,
    0
};

const char* ModuloImpedidos::EntidadeImpedidos::fieldNames[3] = {
    "cabecalho",
    "identificacao",
    "eleitores"
};

const ModuloImpedidos::EntidadeImpedidos::InfoType ModuloImpedidos::EntidadeImpedidos::theInfo = {
    ModuloImpedidos::EntidadeImpedidos::create,
    0x00000010,
    0,
    false,
    ModuloImpedidos::EntidadeImpedidos::fieldInfos,
    ModuloImpedidos::EntidadeImpedidos::fieldIds,
    3, 0, 1,
    NULL,
    &ModuloImpedidos::EntidadeImpedidos::defaultTag,
    ModuloImpedidos::EntidadeImpedidos::fieldNames,
    "EntidadeImpedidos"
};

} // namespace ModuloImpedidos
