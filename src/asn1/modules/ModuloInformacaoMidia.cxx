#include "ModuloInformacaoMidia.h"

namespace ModuloInformacaoMidia{

const char * ModuloInformacaoMidia::TipoMidia::nameList[] = {
    "<undefined>=0",
    "mr",
    "fc",
    "fv"
  };

const ModuloInformacaoMidia::TipoMidia::InfoType ModuloInformacaoMidia::TipoMidia::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    3, nameList
};

const void* ModuloInformacaoMidia::DadosGeracaoMidia::fieldInfos[4] = {
    &serialMidia::value_type::theInfo,
    &usuario::value_type::theInfo,
    &identificadorGeradorMidia::value_type::theInfo,
    &data::value_type::theInfo
};

int ModuloInformacaoMidia::DadosGeracaoMidia::fieldIds[4] = {
    -1,
    -1,
    -1,
    -1
};

const char* ModuloInformacaoMidia::DadosGeracaoMidia::fieldNames[4] = {
    "serialMidia",
    "usuario",
    "identificadorGeradorMidia",
    "data"
};

const ModuloInformacaoMidia::DadosGeracaoMidia::InfoType ModuloInformacaoMidia::DadosGeracaoMidia::theInfo = {
    ModuloInformacaoMidia::DadosGeracaoMidia::create,
    0x00000010,
    0,
    false,
    ModuloInformacaoMidia::DadosGeracaoMidia::fieldInfos,
    ModuloInformacaoMidia::DadosGeracaoMidia::fieldIds,
    4, 0, 0,
    NULL,
    &ModuloInformacaoMidia::DadosGeracaoMidia::defaultTag,
    ModuloInformacaoMidia::DadosGeracaoMidia::fieldNames,
    "DadosGeracaoMidia"
};

const char * ModuloInformacaoMidia::TipoAplicativo::nameList[] = {
    "<undefined>=0",
    "vota",
    "sa",
    "red",
    "vpp",
    "ste",
    "adh",
    "atue"
  };

const ModuloInformacaoMidia::TipoAplicativo::InfoType ModuloInformacaoMidia::TipoAplicativo::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    7, nameList
};

const void* ModuloInformacaoMidia::Autenticacao::fieldInfos[5] = {
    &dataInicial::value_type::theInfo,
    &dataFinal::value_type::theInfo,
    &tamanhoSenha::value_type::theInfo,
    &numeroTentativas::value_type::theInfo,
    &hashSenha::value_type::theInfo
};

int ModuloInformacaoMidia::Autenticacao::fieldIds[5] = {
    0,
    1,
    -1,
    -1,
    -1
};

unsigned ModuloInformacaoMidia::Autenticacao::fieldTags[5] = {
    0xa0000001,
    0xa0000002,
    0x00000002,
    0x00000002,
    0x00000004
};

const char* ModuloInformacaoMidia::Autenticacao::fieldNames[5] = {
    "dataInicial",
    "dataFinal",
    "tamanhoSenha",
    "numeroTentativas",
    "hashSenha"
};

const ModuloInformacaoMidia::Autenticacao::InfoType ModuloInformacaoMidia::Autenticacao::theInfo = {
    ModuloInformacaoMidia::Autenticacao::create,
    0x00000010,
    0,
    false,
    ModuloInformacaoMidia::Autenticacao::fieldInfos,
    ModuloInformacaoMidia::Autenticacao::fieldIds,
    5, 0, 2,
    NULL,
    ModuloInformacaoMidia::Autenticacao::fieldTags,
    ModuloInformacaoMidia::Autenticacao::fieldNames,
    "Autenticacao"
};

const void* ModuloInformacaoMidia::Aplicativo::fieldInfos[2] = {
    &tipoAplicativo::value_type::theInfo,
    &autenticacao::value_type::theInfo
};

int ModuloInformacaoMidia::Aplicativo::fieldIds[2] = {
    -1,
    0
};

const char* ModuloInformacaoMidia::Aplicativo::fieldNames[2] = {
    "tipoAplicativo",
    "autenticacao"
};

const ModuloInformacaoMidia::Aplicativo::InfoType ModuloInformacaoMidia::Aplicativo::theInfo = {
    ModuloInformacaoMidia::Aplicativo::create,
    0x00000010,
    0,
    false,
    ModuloInformacaoMidia::Aplicativo::fieldInfos,
    ModuloInformacaoMidia::Aplicativo::fieldIds,
    2, 0, 1,
    NULL,
    &ModuloInformacaoMidia::Aplicativo::defaultTag,
    ModuloInformacaoMidia::Aplicativo::fieldNames,
    "Aplicativo"
};

const void* ModuloInformacaoMidia::InformacaoMidia::fieldInfos[7] = {
    &tipoMidia::value_type::theInfo,
    &fase::value_type::theInfo,
    &idPE::value_type::theInfo,
    &uf::value_type::theInfo,
    &turno::value_type::theInfo,
    &dadosGeracaoMidia::value_type::theInfo,
    &aplicativos::value_type::theInfo
};

int ModuloInformacaoMidia::InformacaoMidia::fieldIds[7] = {
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    0
};

const char* ModuloInformacaoMidia::InformacaoMidia::fieldNames[7] = {
    "tipoMidia",
    "fase",
    "idPE",
    "uf",
    "turno",
    "dadosGeracaoMidia",
    "aplicativos"
};

const ModuloInformacaoMidia::InformacaoMidia::InfoType ModuloInformacaoMidia::InformacaoMidia::theInfo = {
    ModuloInformacaoMidia::InformacaoMidia::create,
    0x00000010,
    0,
    false,
    ModuloInformacaoMidia::InformacaoMidia::fieldInfos,
    ModuloInformacaoMidia::InformacaoMidia::fieldIds,
    7, 0, 1,
    NULL,
    &ModuloInformacaoMidia::InformacaoMidia::defaultTag,
    ModuloInformacaoMidia::InformacaoMidia::fieldNames,
    "InformacaoMidia"
};

} // namespace ModuloInformacaoMidia
