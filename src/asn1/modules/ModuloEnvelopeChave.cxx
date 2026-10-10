#include "ModuloEnvelopeChave.h"

namespace ModuloEnvelopeChave{

const void* ModuloEnvelopeChave::DescritorChave::fieldInfos[2] = {
    &nomeUsuario::value_type::theInfo,
    &serial::value_type::theInfo
};

int ModuloEnvelopeChave::DescritorChave::fieldIds[2] = {
    -1,
    -1
};

const char* ModuloEnvelopeChave::DescritorChave::fieldNames[2] = {
    "nomeUsuario",
    "serial"
};

const ModuloEnvelopeChave::DescritorChave::InfoType ModuloEnvelopeChave::DescritorChave::theInfo = {
    ModuloEnvelopeChave::DescritorChave::create,
    0x00000010,
    0,
    false,
    ModuloEnvelopeChave::DescritorChave::fieldInfos,
    ModuloEnvelopeChave::DescritorChave::fieldIds,
    2, 0, 0,
    NULL,
    &ModuloEnvelopeChave::DescritorChave::defaultTag,
    ModuloEnvelopeChave::DescritorChave::fieldNames,
    "DescritorChave"
};

const char * ModuloEnvelopeChave::EntidadeChave::tipo::value_type::nameList[] = {
    "chaveSecreta",
    "chavePublica"
  };

const ModuloEnvelopeChave::EntidadeChave::tipo::value_type::InfoType ModuloEnvelopeChave::EntidadeChave::tipo::value_type::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    1, nameList
};

const void* ModuloEnvelopeChave::EntidadeChave::fieldInfos[5] = {
    &tagChaves::value_type::theInfo,
    &descritor::value_type::theInfo,
    &cifrado::value_type::theInfo,
    &tipo::value_type::theInfo,
    &chave::value_type::theInfo
};

int ModuloEnvelopeChave::EntidadeChave::fieldIds[5] = {
    -1,
    -1,
    -1,
    -1,
    -1
};

const char* ModuloEnvelopeChave::EntidadeChave::fieldNames[5] = {
    "tagChaves",
    "descritor",
    "cifrado",
    "tipo",
    "chave"
};

const ModuloEnvelopeChave::EntidadeChave::InfoType ModuloEnvelopeChave::EntidadeChave::theInfo = {
    ModuloEnvelopeChave::EntidadeChave::create,
    0x00000010,
    0,
    false,
    ModuloEnvelopeChave::EntidadeChave::fieldInfos,
    ModuloEnvelopeChave::EntidadeChave::fieldIds,
    5, 0, 0,
    NULL,
    &ModuloEnvelopeChave::EntidadeChave::defaultTag,
    ModuloEnvelopeChave::EntidadeChave::fieldNames,
    "EntidadeChave"
};

} // namespace ModuloEnvelopeChave
