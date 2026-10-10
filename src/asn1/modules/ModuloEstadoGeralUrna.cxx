#include "ModuloEstadoGeralUrna.h"

namespace ModuloEstadoGeralUrna{

const void* ModuloEstadoGeralUrna::DadoSecao::fieldInfos[3] = {
    &municipio::value_type::theInfo,
    &zona::value_type::theInfo,
    &secao::value_type::theInfo
};

int ModuloEstadoGeralUrna::DadoSecao::fieldIds[3] = {
    -1,
    -1,
    -1
};

const char* ModuloEstadoGeralUrna::DadoSecao::fieldNames[3] = {
    "municipio",
    "zona",
    "secao"
};

const ModuloEstadoGeralUrna::DadoSecao::InfoType ModuloEstadoGeralUrna::DadoSecao::theInfo = {
    ModuloEstadoGeralUrna::DadoSecao::create,
    0x00000010,
    0,
    false,
    ModuloEstadoGeralUrna::DadoSecao::fieldInfos,
    ModuloEstadoGeralUrna::DadoSecao::fieldIds,
    3, 0, 0,
    NULL,
    &ModuloEstadoGeralUrna::DadoSecao::defaultTag,
    ModuloEstadoGeralUrna::DadoSecao::fieldNames,
    "DadoSecao"
};

const void* ModuloEstadoGeralUrna::DadoLocal::fieldInfos[3] = {
    &uf::value_type::theInfo,
    &secaoCarga::value_type::theInfo,
    &tipoLocalVotacao::value_type::theInfo
};

int ModuloEstadoGeralUrna::DadoLocal::fieldIds[3] = {
    -1,
    -1,
    -1
};

const char* ModuloEstadoGeralUrna::DadoLocal::fieldNames[3] = {
    "uf",
    "secaoCarga",
    "tipoLocalVotacao"
};

const ModuloEstadoGeralUrna::DadoLocal::InfoType ModuloEstadoGeralUrna::DadoLocal::theInfo = {
    ModuloEstadoGeralUrna::DadoLocal::create,
    0x00000010,
    0,
    false,
    ModuloEstadoGeralUrna::DadoLocal::fieldInfos,
    ModuloEstadoGeralUrna::DadoLocal::fieldIds,
    3, 0, 0,
    NULL,
    &ModuloEstadoGeralUrna::DadoLocal::defaultTag,
    ModuloEstadoGeralUrna::DadoLocal::fieldNames,
    "DadoLocal"
};

const void* ModuloEstadoGeralUrna::DadoCorrespondencia::fieldInfos[3] = {
    &carga::value_type::theInfo,
    &secaoCarga::value_type::theInfo,
    &assinatura::value_type::theInfo
};

int ModuloEstadoGeralUrna::DadoCorrespondencia::fieldIds[3] = {
    -1,
    -1,
    -1
};

const char* ModuloEstadoGeralUrna::DadoCorrespondencia::fieldNames[3] = {
    "carga",
    "secaoCarga",
    "assinatura"
};

const ModuloEstadoGeralUrna::DadoCorrespondencia::InfoType ModuloEstadoGeralUrna::DadoCorrespondencia::theInfo = {
    ModuloEstadoGeralUrna::DadoCorrespondencia::create,
    0x00000010,
    0,
    false,
    ModuloEstadoGeralUrna::DadoCorrespondencia::fieldInfos,
    ModuloEstadoGeralUrna::DadoCorrespondencia::fieldIds,
    3, 0, 0,
    NULL,
    &ModuloEstadoGeralUrna::DadoCorrespondencia::defaultTag,
    ModuloEstadoGeralUrna::DadoCorrespondencia::fieldNames,
    "DadoCorrespondencia"
};

const char * ModuloEstadoGeralUrna::EstadoUrna::nameList[] = {
    "<undefined>=0",
    "carregando",
    "carregada",
    "testada"
  };

const ModuloEstadoGeralUrna::EstadoUrna::InfoType ModuloEstadoGeralUrna::EstadoUrna::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    3, nameList
};

const char * ModuloEstadoGeralUrna::TipoUrnaT1::nameList[] = {
    "semtipo",
    "vota",
    "contingencia",
    "contingenciavota",
    "contingenciavotarecupera"
  };

const ModuloEstadoGeralUrna::TipoUrnaT1::InfoType ModuloEstadoGeralUrna::TipoUrnaT1::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    4, nameList
};

const char * ModuloEstadoGeralUrna::TipoAjusteDataHora::nameList[] = {
    "<undefined>=0",
    "semAjusteDataHora",
    "alterarDataEleicao",
    "alterarDataSistema"
  };

const ModuloEstadoGeralUrna::TipoAjusteDataHora::InfoType ModuloEstadoGeralUrna::TipoAjusteDataHora::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    3, nameList
};

const char * ModuloEstadoGeralUrna::DadoCarga::modelo::value_type::nameList[] = {
    "<undefined>=0",
    "<undefined>=1",
    "tpm20",
    "<undefined>=3",
    "<undefined>=4",
    "<undefined>=5",
    "<undefined>=6",
    "<undefined>=7",
    "<undefined>=8",
    "<undefined>=9",
    "<undefined>=10",
    "<undefined>=11",
    "<undefined>=12",
    "ue2013",
    "<undefined>=14",
    "ue2015",
    "<undefined>=16",
    "<undefined>=17",
    "<undefined>=18",
    "<undefined>=19",
    "ue2020",
    "<undefined>=21",
    "ue2022"
  };

const ModuloEstadoGeralUrna::DadoCarga::modelo::value_type::InfoType ModuloEstadoGeralUrna::DadoCarga::modelo::value_type::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    22, nameList
};

const void* ModuloEstadoGeralUrna::DadoCarga::fieldInfos[5] = {
    &turno::value_type::theInfo,
    &tipoUrnaT1::value_type::theInfo,
    &tipoUrnaT2::value_type::theInfo,
    &modelo::value_type::theInfo,
    &fase::value_type::theInfo
};

int ModuloEstadoGeralUrna::DadoCarga::fieldIds[5] = {
    -1,
    -1,
    -1,
    -1,
    -1
};

const char* ModuloEstadoGeralUrna::DadoCarga::fieldNames[5] = {
    "turno",
    "tipoUrnaT1",
    "tipoUrnaT2",
    "modelo",
    "fase"
};

const ModuloEstadoGeralUrna::DadoCarga::InfoType ModuloEstadoGeralUrna::DadoCarga::theInfo = {
    ModuloEstadoGeralUrna::DadoCarga::create,
    0x00000010,
    0,
    false,
    ModuloEstadoGeralUrna::DadoCarga::fieldInfos,
    ModuloEstadoGeralUrna::DadoCarga::fieldIds,
    5, 0, 0,
    NULL,
    &ModuloEstadoGeralUrna::DadoCarga::defaultTag,
    ModuloEstadoGeralUrna::DadoCarga::fieldNames,
    "DadoCarga"
};

const void* ModuloEstadoGeralUrna::AjusteDataHora::fieldInfos[2] = {
    &tipoAjusteDataHora::value_type::theInfo,
    &valorAjusteDataHora::value_type::theInfo
};

int ModuloEstadoGeralUrna::AjusteDataHora::fieldIds[2] = {
    -1,
    -1
};

const char* ModuloEstadoGeralUrna::AjusteDataHora::fieldNames[2] = {
    "tipoAjusteDataHora",
    "valorAjusteDataHora"
};

const ModuloEstadoGeralUrna::AjusteDataHora::InfoType ModuloEstadoGeralUrna::AjusteDataHora::theInfo = {
    ModuloEstadoGeralUrna::AjusteDataHora::create,
    0x00000010,
    0,
    false,
    ModuloEstadoGeralUrna::AjusteDataHora::fieldInfos,
    ModuloEstadoGeralUrna::AjusteDataHora::fieldIds,
    2, 0, 0,
    NULL,
    &ModuloEstadoGeralUrna::AjusteDataHora::defaultTag,
    ModuloEstadoGeralUrna::AjusteDataHora::fieldNames,
    "AjusteDataHora"
};

const void* ModuloEstadoGeralUrna::EstadoGeralUrna::fieldInfos[8] = {
    &idPE::value_type::theInfo,
    &estadoUrna::value_type::theInfo,
    &dadoCarga::value_type::theInfo,
    &dadoLocal::value_type::theInfo,
    &ajusteDataHora::value_type::theInfo,
    &correspondencia::value_type::theInfo,
    &versao::value_type::theInfo,
    &hashVersoesPacotes::value_type::theInfo
};

int ModuloEstadoGeralUrna::EstadoGeralUrna::fieldIds[8] = {
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1
};

const char* ModuloEstadoGeralUrna::EstadoGeralUrna::fieldNames[8] = {
    "idPE",
    "estadoUrna",
    "dadoCarga",
    "dadoLocal",
    "ajusteDataHora",
    "correspondencia",
    "versao",
    "hashVersoesPacotes"
};

const ModuloEstadoGeralUrna::EstadoGeralUrna::InfoType ModuloEstadoGeralUrna::EstadoGeralUrna::theInfo = {
    ModuloEstadoGeralUrna::EstadoGeralUrna::create,
    0x00000010,
    0,
    false,
    ModuloEstadoGeralUrna::EstadoGeralUrna::fieldInfos,
    ModuloEstadoGeralUrna::EstadoGeralUrna::fieldIds,
    8, 0, 0,
    NULL,
    &ModuloEstadoGeralUrna::EstadoGeralUrna::defaultTag,
    ModuloEstadoGeralUrna::EstadoGeralUrna::fieldNames,
    "EstadoGeralUrna"
};

} // namespace ModuloEstadoGeralUrna
