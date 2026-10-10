#include "ModuloRegistroDigitalVoto.h"

namespace ModuloRegistroDigitalVoto{

const char * ModuloRegistroDigitalVoto::TipoVoto::nameList[] = {
    "<undefined>=0",
    "legenda",
    "nominal",
    "branco",
    "nulo",
    "brancoAposSuspensao",
    "nuloAposSuspensao",
    "nuloPorRepeticao",
    "nuloCargoSemCandidato",
    "nuloAposSuspensaoCargoSemCandidato"
  };

const ModuloRegistroDigitalVoto::TipoVoto::InfoType ModuloRegistroDigitalVoto::TipoVoto::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    9, nameList
};

const ModuloRegistroDigitalVoto::Voto::digitacao::value_type::InfoType ModuloRegistroDigitalVoto::Voto::digitacao::value_type::theInfo = {
    ASN1::AbstractString::create,
    0x00000012,
    &ASN1::NumericString::theInfo,
    ASN1::FixedConstraint, 1, 5,
    ASN1::NumericString::theInfo.characterSet, ASN1::NumericString::theInfo.characterSetSize, ASN1::NumericString::theInfo.canonicalSetBits,
    ASN1::NumericString::theInfo.charSetUnalignedBits, ASN1::NumericString::theInfo.charSetAlignedBits
};

const void* ModuloRegistroDigitalVoto::Voto::fieldInfos[2] = {
    &tipoVoto::value_type::theInfo,
    &digitacao::value_type::theInfo
};

int ModuloRegistroDigitalVoto::Voto::fieldIds[2] = {
    -1,
    0
};

const char* ModuloRegistroDigitalVoto::Voto::fieldNames[2] = {
    "tipoVoto",
    "digitacao"
};

const ModuloRegistroDigitalVoto::Voto::InfoType ModuloRegistroDigitalVoto::Voto::theInfo = {
    ModuloRegistroDigitalVoto::Voto::create,
    0x00000010,
    0,
    false,
    ModuloRegistroDigitalVoto::Voto::fieldInfos,
    ModuloRegistroDigitalVoto::Voto::fieldIds,
    2, 0, 1,
    NULL,
    &ModuloRegistroDigitalVoto::Voto::defaultTag,
    ModuloRegistroDigitalVoto::Voto::fieldNames,
    "Voto"
};

const ModuloRegistroDigitalVoto::VotosCargo::votos::value_type::InfoType ModuloRegistroDigitalVoto::VotosCargo::votos::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloRegistroDigitalVoto::Voto::theInfo,
    0
};

const void* ModuloRegistroDigitalVoto::VotosCargo::fieldInfos[3] = {
    &idCargo::value_type::theInfo,
    &quantidadeEscolhas::value_type::theInfo,
    &votos::value_type::theInfo
};

int ModuloRegistroDigitalVoto::VotosCargo::fieldIds[3] = {
    -1,
    -1,
    -1
};

const char* ModuloRegistroDigitalVoto::VotosCargo::fieldNames[3] = {
    "idCargo",
    "quantidadeEscolhas",
    "votos"
};

const ModuloRegistroDigitalVoto::VotosCargo::InfoType ModuloRegistroDigitalVoto::VotosCargo::theInfo = {
    ModuloRegistroDigitalVoto::VotosCargo::create,
    0x00000010,
    0,
    false,
    ModuloRegistroDigitalVoto::VotosCargo::fieldInfos,
    ModuloRegistroDigitalVoto::VotosCargo::fieldIds,
    3, 0, 0,
    NULL,
    &ModuloRegistroDigitalVoto::VotosCargo::defaultTag,
    ModuloRegistroDigitalVoto::VotosCargo::fieldNames,
    "VotosCargo"
};

const ModuloRegistroDigitalVoto::VotosCargos::InfoType ModuloRegistroDigitalVoto::VotosCargos::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloRegistroDigitalVoto::VotosCargo::theInfo,
    0
};

const void* ModuloRegistroDigitalVoto::EleicaoVota::fieldInfos[2] = {
    &idEleicao::value_type::theInfo,
    &votosCargos::value_type::theInfo
};

int ModuloRegistroDigitalVoto::EleicaoVota::fieldIds[2] = {
    -1,
    -1
};

const char* ModuloRegistroDigitalVoto::EleicaoVota::fieldNames[2] = {
    "idEleicao",
    "votosCargos"
};

const ModuloRegistroDigitalVoto::EleicaoVota::InfoType ModuloRegistroDigitalVoto::EleicaoVota::theInfo = {
    ModuloRegistroDigitalVoto::EleicaoVota::create,
    0x00000010,
    0,
    false,
    ModuloRegistroDigitalVoto::EleicaoVota::fieldInfos,
    ModuloRegistroDigitalVoto::EleicaoVota::fieldIds,
    2, 0, 0,
    NULL,
    &ModuloRegistroDigitalVoto::EleicaoVota::defaultTag,
    ModuloRegistroDigitalVoto::EleicaoVota::fieldNames,
    "EleicaoVota"
};

const char * ModuloRegistroDigitalVoto::EleicaoSA::tipoCedulaSA::value_type::nameList[] = {
    "<undefined>=0",
    "majoritario",
    "proporcional"
  };

const ModuloRegistroDigitalVoto::EleicaoSA::tipoCedulaSA::value_type::InfoType ModuloRegistroDigitalVoto::EleicaoSA::tipoCedulaSA::value_type::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    2, nameList
};

const char * ModuloRegistroDigitalVoto::EleicaoSA::origemVotosSA::value_type::nameList[] = {
    "<undefined>=0",
    "cedula",
    "rdv",
    "bu"
  };

const ModuloRegistroDigitalVoto::EleicaoSA::origemVotosSA::value_type::InfoType ModuloRegistroDigitalVoto::EleicaoSA::origemVotosSA::value_type::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    3, nameList
};

const void* ModuloRegistroDigitalVoto::EleicaoSA::fieldInfos[4] = {
    &idEleicao::value_type::theInfo,
    &tipoCedulaSA::value_type::theInfo,
    &origemVotosSA::value_type::theInfo,
    &votosCargos::value_type::theInfo
};

int ModuloRegistroDigitalVoto::EleicaoSA::fieldIds[4] = {
    -1,
    -1,
    -1,
    -1
};

const char* ModuloRegistroDigitalVoto::EleicaoSA::fieldNames[4] = {
    "idEleicao",
    "tipoCedulaSA",
    "origemVotosSA",
    "votosCargos"
};

const ModuloRegistroDigitalVoto::EleicaoSA::InfoType ModuloRegistroDigitalVoto::EleicaoSA::theInfo = {
    ModuloRegistroDigitalVoto::EleicaoSA::create,
    0x00000010,
    0,
    false,
    ModuloRegistroDigitalVoto::EleicaoSA::fieldInfos,
    ModuloRegistroDigitalVoto::EleicaoSA::fieldIds,
    4, 0, 0,
    NULL,
    &ModuloRegistroDigitalVoto::EleicaoSA::defaultTag,
    ModuloRegistroDigitalVoto::EleicaoSA::fieldNames,
    "EleicaoSA"
};

const ModuloRegistroDigitalVoto::Eleicoes::eleicoesVota::value_type::InfoType ModuloRegistroDigitalVoto::Eleicoes::eleicoesVota::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x80000001,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloRegistroDigitalVoto::EleicaoVota::theInfo,
    "value_type"
};

const ModuloRegistroDigitalVoto::Eleicoes::eleicoesSA::value_type::InfoType ModuloRegistroDigitalVoto::Eleicoes::eleicoesSA::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x80000002,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloRegistroDigitalVoto::EleicaoSA::theInfo,
    "value_type"
};

const void* ModuloRegistroDigitalVoto::Eleicoes::selectionInfos[2] = {
    &eleicoesVota::value_type::theInfo,
    &eleicoesSA::value_type::theInfo
};

unsigned ModuloRegistroDigitalVoto::Eleicoes::selectionTags[2] = {
    0x80000001,
    0x80000002
};

const char* ModuloRegistroDigitalVoto::Eleicoes::selectionNames[2] = {
    "eleicoesVota",
    "eleicoesSA"
};

const ModuloRegistroDigitalVoto::Eleicoes::InfoType ModuloRegistroDigitalVoto::Eleicoes::theInfo = {
    ModuloRegistroDigitalVoto::Eleicoes::create,
    0x00000000,
    0,
    false,
    ModuloRegistroDigitalVoto::Eleicoes::selectionInfos,
    2, 2,
    ModuloRegistroDigitalVoto::Eleicoes::selectionTags,
    ModuloRegistroDigitalVoto::Eleicoes::selectionNames
};

const ModuloRegistroDigitalVoto::HistoricoCodigosCarga::InfoType ModuloRegistroDigitalVoto::HistoricoCodigosCarga::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ASN1::GeneralString::theInfo,
    0
};

const void* ModuloRegistroDigitalVoto::EntidadeRegistroDigitalVoto::fieldInfos[5] = {
    &pleito::value_type::theInfo,
    &fase::value_type::theInfo,
    &identificacao::value_type::theInfo,
    &historicoCodigosCarga::value_type::theInfo,
    &eleicoes::value_type::theInfo
};

int ModuloRegistroDigitalVoto::EntidadeRegistroDigitalVoto::fieldIds[5] = {
    -1,
    -1,
    -1,
    -1,
    -1
};

const char* ModuloRegistroDigitalVoto::EntidadeRegistroDigitalVoto::fieldNames[5] = {
    "pleito",
    "fase",
    "identificacao",
    "historicoCodigosCarga",
    "eleicoes"
};

const ModuloRegistroDigitalVoto::EntidadeRegistroDigitalVoto::InfoType ModuloRegistroDigitalVoto::EntidadeRegistroDigitalVoto::theInfo = {
    ModuloRegistroDigitalVoto::EntidadeRegistroDigitalVoto::create,
    0x00000010,
    0,
    false,
    ModuloRegistroDigitalVoto::EntidadeRegistroDigitalVoto::fieldInfos,
    ModuloRegistroDigitalVoto::EntidadeRegistroDigitalVoto::fieldIds,
    5, 0, 0,
    NULL,
    &ModuloRegistroDigitalVoto::EntidadeRegistroDigitalVoto::defaultTag,
    ModuloRegistroDigitalVoto::EntidadeRegistroDigitalVoto::fieldNames,
    "EntidadeRegistroDigitalVoto"
};

const void* ModuloRegistroDigitalVoto::EntidadeResultadoRDV::fieldInfos[3] = {
    &cabecalho::value_type::theInfo,
    &urna::value_type::theInfo,
    &rdv::value_type::theInfo
};

int ModuloRegistroDigitalVoto::EntidadeResultadoRDV::fieldIds[3] = {
    -1,
    -1,
    -1
};

const char* ModuloRegistroDigitalVoto::EntidadeResultadoRDV::fieldNames[3] = {
    "cabecalho",
    "urna",
    "rdv"
};

const ModuloRegistroDigitalVoto::EntidadeResultadoRDV::InfoType ModuloRegistroDigitalVoto::EntidadeResultadoRDV::theInfo = {
    ModuloRegistroDigitalVoto::EntidadeResultadoRDV::create,
    0x00000010,
    0,
    false,
    ModuloRegistroDigitalVoto::EntidadeResultadoRDV::fieldInfos,
    ModuloRegistroDigitalVoto::EntidadeResultadoRDV::fieldIds,
    3, 0, 0,
    NULL,
    &ModuloRegistroDigitalVoto::EntidadeResultadoRDV::defaultTag,
    ModuloRegistroDigitalVoto::EntidadeResultadoRDV::fieldNames,
    "EntidadeResultadoRDV"
};

} // namespace ModuloRegistroDigitalVoto
