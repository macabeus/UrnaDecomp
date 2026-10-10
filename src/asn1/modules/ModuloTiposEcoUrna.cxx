#include "ModuloTiposEcoUrna.h"

namespace ModuloTiposEcoUrna{

const void* ModuloTiposEcoUrna::RegistroIdentificacaoEleitor::fieldInfos[2] = {
    &identificacaoUtilizada::value_type::theInfo,
    &identificacaoPrincipal::value_type::theInfo
};

int ModuloTiposEcoUrna::RegistroIdentificacaoEleitor::fieldIds[2] = {
    -1,
    0
};

const char* ModuloTiposEcoUrna::RegistroIdentificacaoEleitor::fieldNames[2] = {
    "identificacaoUtilizada",
    "identificacaoPrincipal"
};

const ModuloTiposEcoUrna::RegistroIdentificacaoEleitor::InfoType ModuloTiposEcoUrna::RegistroIdentificacaoEleitor::theInfo = {
    ModuloTiposEcoUrna::RegistroIdentificacaoEleitor::create,
    0x00000010,
    0,
    false,
    ModuloTiposEcoUrna::RegistroIdentificacaoEleitor::fieldInfos,
    ModuloTiposEcoUrna::RegistroIdentificacaoEleitor::fieldIds,
    2, 0, 1,
    NULL,
    &ModuloTiposEcoUrna::RegistroIdentificacaoEleitor::defaultTag,
    ModuloTiposEcoUrna::RegistroIdentificacaoEleitor::fieldNames,
    "RegistroIdentificacaoEleitor"
};

const void* ModuloTiposEcoUrna::IdentificadorGeradorMidia::fieldInfos[3] = {
    &nome::value_type::theInfo,
    &serialCertificadoTPM::value_type::theInfo,
    &serialInstalacao::value_type::theInfo
};

int ModuloTiposEcoUrna::IdentificadorGeradorMidia::fieldIds[3] = {
    -1,
    -1,
    -1
};

const char* ModuloTiposEcoUrna::IdentificadorGeradorMidia::fieldNames[3] = {
    "nome",
    "serialCertificadoTPM",
    "serialInstalacao"
};

const ModuloTiposEcoUrna::IdentificadorGeradorMidia::InfoType ModuloTiposEcoUrna::IdentificadorGeradorMidia::theInfo = {
    ModuloTiposEcoUrna::IdentificadorGeradorMidia::create,
    0x00000010,
    0,
    false,
    ModuloTiposEcoUrna::IdentificadorGeradorMidia::fieldInfos,
    ModuloTiposEcoUrna::IdentificadorGeradorMidia::fieldIds,
    3, 0, 0,
    NULL,
    &ModuloTiposEcoUrna::IdentificadorGeradorMidia::defaultTag,
    ModuloTiposEcoUrna::IdentificadorGeradorMidia::fieldNames,
    "IdentificadorGeradorMidia"
};

const void* ModuloTiposEcoUrna::Carga::fieldInfos[5] = {
    &numeroInternoUrna::value_type::theInfo,
    &numeroSerieFC::value_type::theInfo,
    &identificadorGeradorMidia::value_type::theInfo,
    &dataHoraCarga::value_type::theInfo,
    &codigoCarga::value_type::theInfo
};

int ModuloTiposEcoUrna::Carga::fieldIds[5] = {
    -1,
    -1,
    -1,
    -1,
    -1
};

const char* ModuloTiposEcoUrna::Carga::fieldNames[5] = {
    "numeroInternoUrna",
    "numeroSerieFC",
    "identificadorGeradorMidia",
    "dataHoraCarga",
    "codigoCarga"
};

const ModuloTiposEcoUrna::Carga::InfoType ModuloTiposEcoUrna::Carga::theInfo = {
    ModuloTiposEcoUrna::Carga::create,
    0x00000010,
    0,
    false,
    ModuloTiposEcoUrna::Carga::fieldInfos,
    ModuloTiposEcoUrna::Carga::fieldIds,
    5, 0, 0,
    NULL,
    &ModuloTiposEcoUrna::Carga::defaultTag,
    ModuloTiposEcoUrna::Carga::fieldNames,
    "Carga"
};

const char * ModuloTiposEcoUrna::SituacaoArquivo::nameList[] = {
    "<undefined>=0",
    "arquivoFinal",
    "arquivoParcial"
  };

const ModuloTiposEcoUrna::SituacaoArquivo::InfoType ModuloTiposEcoUrna::SituacaoArquivo::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    2, nameList
};

} // namespace ModuloTiposEcoUrna
