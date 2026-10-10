#include "ModuloEstadoGeralSA.h"

namespace ModuloEstadoGeralSA{

const void* ModuloEstadoGeralSA::DadoSecaoSA::fieldInfos[3] = {
    &municipio::value_type::theInfo,
    &zona::value_type::theInfo,
    &secao::value_type::theInfo
};

int ModuloEstadoGeralSA::DadoSecaoSA::fieldIds[3] = {
    -1,
    -1,
    -1
};

const char* ModuloEstadoGeralSA::DadoSecaoSA::fieldNames[3] = {
    "municipio",
    "zona",
    "secao"
};

const ModuloEstadoGeralSA::DadoSecaoSA::InfoType ModuloEstadoGeralSA::DadoSecaoSA::theInfo = {
    ModuloEstadoGeralSA::DadoSecaoSA::create,
    0x00000010,
    0,
    false,
    ModuloEstadoGeralSA::DadoSecaoSA::fieldInfos,
    ModuloEstadoGeralSA::DadoSecaoSA::fieldIds,
    3, 0, 0,
    NULL,
    &ModuloEstadoGeralSA::DadoSecaoSA::defaultTag,
    ModuloEstadoGeralSA::DadoSecaoSA::fieldNames,
    "DadoSecaoSA"
};

const char * ModuloEstadoGeralSA::EstadoSA::nameList[] = {
    "inicial",
    "arvoreraizpreparada",
    "preparada",
    "imprimirZEsecao",
    "iniciarapuracao",
    "apurarmajoritaria",
    "majoritariacompleta",
    "majoritariaencerrada",
    "apurarproporcional",
    "proporcionalcompleta",
    "proporcionalencerrada",
    "buiniciacargo",
    "buproporcionalpedepartido",
    "buproporcionalpedecandidato",
    "bumajoritariapedecandidato",
    "buregistrabunordv",
    "posicionarmista",
    "gravarbu",
    "imprimirbu",
    "gravaresultado",
    "copiaresultadomr",
    "imprimiradicionalbu",
    "encerrada",
    "ultimoid"
  };

const ModuloEstadoGeralSA::EstadoSA::InfoType ModuloEstadoGeralSA::EstadoSA::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    23, nameList
};

const void* ModuloEstadoGeralSA::EstadoGeralSA::fieldInfos[3] = {
    &estadoSA::value_type::theInfo,
    &atualizacaoBloqueada::value_type::theInfo,
    &dadosSecao::value_type::theInfo
};

int ModuloEstadoGeralSA::EstadoGeralSA::fieldIds[3] = {
    -1,
    -1,
    -1
};

const char* ModuloEstadoGeralSA::EstadoGeralSA::fieldNames[3] = {
    "estadoSA",
    "atualizacaoBloqueada",
    "dadosSecao"
};

const ModuloEstadoGeralSA::EstadoGeralSA::InfoType ModuloEstadoGeralSA::EstadoGeralSA::theInfo = {
    ModuloEstadoGeralSA::EstadoGeralSA::create,
    0x00000010,
    0,
    false,
    ModuloEstadoGeralSA::EstadoGeralSA::fieldInfos,
    ModuloEstadoGeralSA::EstadoGeralSA::fieldIds,
    3, 0, 0,
    NULL,
    &ModuloEstadoGeralSA::EstadoGeralSA::defaultTag,
    ModuloEstadoGeralSA::EstadoGeralSA::fieldNames,
    "EstadoGeralSA"
};

} // namespace ModuloEstadoGeralSA
