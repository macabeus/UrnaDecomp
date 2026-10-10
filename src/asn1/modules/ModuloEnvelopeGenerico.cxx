#include "ModuloEnvelopeGenerico.h"

namespace ModuloEnvelopeGenerico{

const char * ModuloEnvelopeGenerico::TipoEnvelope::nameList[] = {
    "<undefined>=0",
    "envelopeBoletimUrna",
    "envelopeRegistroDigitalVoto",
    "<undefined>=3",
    "envelopeBoletimUrnaImpresso",
    "envelopeImagemBiometria",
    "envelopeZeresimaImpressa"
  };

const ModuloEnvelopeGenerico::TipoEnvelope::InfoType ModuloEnvelopeGenerico::TipoEnvelope::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    6, nameList
};

const void* ModuloEnvelopeGenerico::EntidadeEnvelopeGenerico::fieldInfos[7] = {
    &cabecalho::value_type::theInfo,
    &fase::value_type::theInfo,
    &urna::value_type::theInfo,
    &identificacao::value_type::theInfo,
    &tipoEnvelope::value_type::theInfo,
    &seguranca::value_type::theInfo,
    &conteudo::value_type::theInfo
};

int ModuloEnvelopeGenerico::EntidadeEnvelopeGenerico::fieldIds[7] = {
    -1,
    -1,
    0,
    -1,
    -1,
    1,
    -1
};

const char* ModuloEnvelopeGenerico::EntidadeEnvelopeGenerico::fieldNames[7] = {
    "cabecalho",
    "fase",
    "urna",
    "identificacao",
    "tipoEnvelope",
    "seguranca",
    "conteudo"
};

const ModuloEnvelopeGenerico::EntidadeEnvelopeGenerico::InfoType ModuloEnvelopeGenerico::EntidadeEnvelopeGenerico::theInfo = {
    ModuloEnvelopeGenerico::EntidadeEnvelopeGenerico::create,
    0x00000010,
    0,
    false,
    ModuloEnvelopeGenerico::EntidadeEnvelopeGenerico::fieldInfos,
    ModuloEnvelopeGenerico::EntidadeEnvelopeGenerico::fieldIds,
    7, 0, 2,
    NULL,
    &ModuloEnvelopeGenerico::EntidadeEnvelopeGenerico::defaultTag,
    ModuloEnvelopeGenerico::EntidadeEnvelopeGenerico::fieldNames,
    "EntidadeEnvelopeGenerico"
};

} // namespace ModuloEnvelopeGenerico
