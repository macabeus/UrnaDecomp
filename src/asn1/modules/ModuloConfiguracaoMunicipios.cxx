#include "ModuloConfiguracaoMunicipios.h"

namespace ModuloConfiguracaoMunicipios{

const void* ModuloConfiguracaoMunicipios::HorariosUrna::fieldInfos[4] = {
    &emissaoZeresima::value_type::theInfo,
    &inicioVotacao::value_type::theInfo,
    &encerramentoVotacao::value_type::theInfo,
    &terminoVotacao::value_type::theInfo
};

int ModuloConfiguracaoMunicipios::HorariosUrna::fieldIds[4] = {
    -1,
    -1,
    -1,
    -1
};

const char* ModuloConfiguracaoMunicipios::HorariosUrna::fieldNames[4] = {
    "emissaoZeresima",
    "inicioVotacao",
    "encerramentoVotacao",
    "terminoVotacao"
};

const ModuloConfiguracaoMunicipios::HorariosUrna::InfoType ModuloConfiguracaoMunicipios::HorariosUrna::theInfo = {
    ModuloConfiguracaoMunicipios::HorariosUrna::create,
    0x00000010,
    0,
    false,
    ModuloConfiguracaoMunicipios::HorariosUrna::fieldInfos,
    ModuloConfiguracaoMunicipios::HorariosUrna::fieldIds,
    4, 0, 0,
    NULL,
    &ModuloConfiguracaoMunicipios::HorariosUrna::defaultTag,
    ModuloConfiguracaoMunicipios::HorariosUrna::fieldNames,
    "HorariosUrna"
};

const void* ModuloConfiguracaoMunicipios::ConfiguracaoMunicipio::fieldInfos[2] = {
    &codigoMunicipio::value_type::theInfo,
    &horariosUrna::value_type::theInfo
};

int ModuloConfiguracaoMunicipios::ConfiguracaoMunicipio::fieldIds[2] = {
    -1,
    -1
};

const char* ModuloConfiguracaoMunicipios::ConfiguracaoMunicipio::fieldNames[2] = {
    "codigoMunicipio",
    "horariosUrna"
};

const ModuloConfiguracaoMunicipios::ConfiguracaoMunicipio::InfoType ModuloConfiguracaoMunicipios::ConfiguracaoMunicipio::theInfo = {
    ModuloConfiguracaoMunicipios::ConfiguracaoMunicipio::create,
    0x00000010,
    0,
    false,
    ModuloConfiguracaoMunicipios::ConfiguracaoMunicipio::fieldInfos,
    ModuloConfiguracaoMunicipios::ConfiguracaoMunicipio::fieldIds,
    2, 0, 0,
    NULL,
    &ModuloConfiguracaoMunicipios::ConfiguracaoMunicipio::defaultTag,
    ModuloConfiguracaoMunicipios::ConfiguracaoMunicipio::fieldNames,
    "ConfiguracaoMunicipio"
};

const ModuloConfiguracaoMunicipios::EntidadeConfiguracaoMunicipios::configuracoes::value_type::InfoType ModuloConfiguracaoMunicipios::EntidadeConfiguracaoMunicipios::configuracoes::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloConfiguracaoMunicipios::ConfiguracaoMunicipio::theInfo,
    0
};

const void* ModuloConfiguracaoMunicipios::EntidadeConfiguracaoMunicipios::fieldInfos[2] = {
    &cabecalho::value_type::theInfo,
    &configuracoes::value_type::theInfo
};

int ModuloConfiguracaoMunicipios::EntidadeConfiguracaoMunicipios::fieldIds[2] = {
    -1,
    -1
};

const char* ModuloConfiguracaoMunicipios::EntidadeConfiguracaoMunicipios::fieldNames[2] = {
    "cabecalho",
    "configuracoes"
};

const ModuloConfiguracaoMunicipios::EntidadeConfiguracaoMunicipios::InfoType ModuloConfiguracaoMunicipios::EntidadeConfiguracaoMunicipios::theInfo = {
    ModuloConfiguracaoMunicipios::EntidadeConfiguracaoMunicipios::create,
    0x00000010,
    0,
    false,
    ModuloConfiguracaoMunicipios::EntidadeConfiguracaoMunicipios::fieldInfos,
    ModuloConfiguracaoMunicipios::EntidadeConfiguracaoMunicipios::fieldIds,
    2, 0, 0,
    NULL,
    &ModuloConfiguracaoMunicipios::EntidadeConfiguracaoMunicipios::defaultTag,
    ModuloConfiguracaoMunicipios::EntidadeConfiguracaoMunicipios::fieldNames,
    "EntidadeConfiguracaoMunicipios"
};

} // namespace ModuloConfiguracaoMunicipios
