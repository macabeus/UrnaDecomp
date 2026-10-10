#include "ModuloTiposResultadosEcoUrna.h"

namespace ModuloTiposResultadosEcoUrna{

const void* ModuloTiposResultadosEcoUrna::Identificacao::selectionInfos[2] = {
    &identificacaoSecaoEleitoral::value_type::theInfo,
    &identificacaoContingencia::value_type::theInfo
};

const char* ModuloTiposResultadosEcoUrna::Identificacao::selectionNames[2] = {
    "identificacaoSecaoEleitoral",
    "identificacaoContingencia"
};

const ModuloTiposResultadosEcoUrna::Identificacao::InfoType ModuloTiposResultadosEcoUrna::Identificacao::theInfo = {
    ModuloTiposResultadosEcoUrna::Identificacao::create,
    0x00000000,
    0,
    false,
    ModuloTiposResultadosEcoUrna::Identificacao::selectionInfos,
    2, 2,
    NULL,
    ModuloTiposResultadosEcoUrna::Identificacao::selectionNames
};

const void* ModuloTiposResultadosEcoUrna::CorrespondenciaResultado::fieldInfos[2] = {
    &identificacao::value_type::theInfo,
    &carga::value_type::theInfo
};

int ModuloTiposResultadosEcoUrna::CorrespondenciaResultado::fieldIds[2] = {
    -1,
    -1
};

const char* ModuloTiposResultadosEcoUrna::CorrespondenciaResultado::fieldNames[2] = {
    "identificacao",
    "carga"
};

const ModuloTiposResultadosEcoUrna::CorrespondenciaResultado::InfoType ModuloTiposResultadosEcoUrna::CorrespondenciaResultado::theInfo = {
    ModuloTiposResultadosEcoUrna::CorrespondenciaResultado::create,
    0x00000010,
    0,
    false,
    ModuloTiposResultadosEcoUrna::CorrespondenciaResultado::fieldInfos,
    ModuloTiposResultadosEcoUrna::CorrespondenciaResultado::fieldIds,
    2, 0, 0,
    NULL,
    &ModuloTiposResultadosEcoUrna::CorrespondenciaResultado::defaultTag,
    ModuloTiposResultadosEcoUrna::CorrespondenciaResultado::fieldNames,
    "CorrespondenciaResultado"
};

const char * ModuloTiposResultadosEcoUrna::TipoArquivo::nameList[] = {
    "<undefined>=0",
    "votacaoUE",
    "votacaoRED",
    "saMistaMRParcialCedula",
    "saMistaBUImpressoCedula",
    "saManual",
    "saEletronica"
  };

const ModuloTiposResultadosEcoUrna::TipoArquivo::InfoType ModuloTiposResultadosEcoUrna::TipoArquivo::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    6, nameList
};

const char * ModuloTiposResultadosEcoUrna::TipoUrna::nameList[] = {
    "<undefined>=0",
    "secao",
    "<undefined>=2",
    "contingencia",
    "contingenciaSecao",
    "<undefined>=5",
    "contingenciaEncerrandoSecao"
  };

const ModuloTiposResultadosEcoUrna::TipoUrna::InfoType ModuloTiposResultadosEcoUrna::TipoUrna::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    6, nameList
};

const char * ModuloTiposResultadosEcoUrna::TipoApuracao::nameList[] = {
    "<undefined>=0",
    "totalmenteManual",
    "totalmenteEletronica",
    "mistaBU",
    "mistaMR"
  };

const ModuloTiposResultadosEcoUrna::TipoApuracao::InfoType ModuloTiposResultadosEcoUrna::TipoApuracao::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    4, nameList
};

const char * ModuloTiposResultadosEcoUrna::ApuracaoMistaMR::motivoApuracao::value_type::nameList[] = {
    "<undefined>=0",
    "naoObteveExitoContingencia",
    "indisponibilidadeUrnaContingencia",
    "indisponibilidadeFlashContingencia",
    "problemaEnergiaEletrica",
    "naoFoiPossivelTrocarUrna",
    "naoFoiSolicitadaTrocaUrna",
    "<undefined>=7",
    "<undefined>=8",
    "<undefined>=9",
    "<undefined>=10",
    "<undefined>=11",
    "<undefined>=12",
    "<undefined>=13",
    "<undefined>=14",
    "<undefined>=15",
    "<undefined>=16",
    "<undefined>=17",
    "<undefined>=18",
    "<undefined>=19",
    "<undefined>=20",
    "<undefined>=21",
    "<undefined>=22",
    "<undefined>=23",
    "<undefined>=24",
    "<undefined>=25",
    "<undefined>=26",
    "<undefined>=27",
    "<undefined>=28",
    "<undefined>=29",
    "<undefined>=30",
    "<undefined>=31",
    "<undefined>=32",
    "<undefined>=33",
    "<undefined>=34",
    "<undefined>=35",
    "<undefined>=36",
    "<undefined>=37",
    "<undefined>=38",
    "<undefined>=39",
    "<undefined>=40",
    "<undefined>=41",
    "<undefined>=42",
    "<undefined>=43",
    "<undefined>=44",
    "<undefined>=45",
    "<undefined>=46",
    "<undefined>=47",
    "<undefined>=48",
    "<undefined>=49",
    "<undefined>=50",
    "<undefined>=51",
    "<undefined>=52",
    "<undefined>=53",
    "<undefined>=54",
    "<undefined>=55",
    "<undefined>=56",
    "<undefined>=57",
    "<undefined>=58",
    "<undefined>=59",
    "<undefined>=60",
    "<undefined>=61",
    "<undefined>=62",
    "<undefined>=63",
    "<undefined>=64",
    "<undefined>=65",
    "<undefined>=66",
    "<undefined>=67",
    "<undefined>=68",
    "<undefined>=69",
    "<undefined>=70",
    "<undefined>=71",
    "<undefined>=72",
    "<undefined>=73",
    "<undefined>=74",
    "<undefined>=75",
    "<undefined>=76",
    "<undefined>=77",
    "<undefined>=78",
    "<undefined>=79",
    "<undefined>=80",
    "<undefined>=81",
    "<undefined>=82",
    "<undefined>=83",
    "<undefined>=84",
    "<undefined>=85",
    "<undefined>=86",
    "<undefined>=87",
    "<undefined>=88",
    "<undefined>=89",
    "<undefined>=90",
    "<undefined>=91",
    "<undefined>=92",
    "<undefined>=93",
    "<undefined>=94",
    "<undefined>=95",
    "<undefined>=96",
    "<undefined>=97",
    "<undefined>=98",
    "outros"
  };

const ModuloTiposResultadosEcoUrna::ApuracaoMistaMR::motivoApuracao::value_type::InfoType ModuloTiposResultadosEcoUrna::ApuracaoMistaMR::motivoApuracao::value_type::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    99, nameList
};

const void* ModuloTiposResultadosEcoUrna::ApuracaoMistaMR::fieldInfos[2] = {
    &tipoApuracao::value_type::theInfo,
    &motivoApuracao::value_type::theInfo
};

int ModuloTiposResultadosEcoUrna::ApuracaoMistaMR::fieldIds[2] = {
    -1,
    -1
};

const char* ModuloTiposResultadosEcoUrna::ApuracaoMistaMR::fieldNames[2] = {
    "tipoApuracao",
    "motivoApuracao"
};

const ModuloTiposResultadosEcoUrna::ApuracaoMistaMR::InfoType ModuloTiposResultadosEcoUrna::ApuracaoMistaMR::theInfo = {
    ModuloTiposResultadosEcoUrna::ApuracaoMistaMR::create,
    0x00000010,
    0,
    false,
    ModuloTiposResultadosEcoUrna::ApuracaoMistaMR::fieldInfos,
    ModuloTiposResultadosEcoUrna::ApuracaoMistaMR::fieldIds,
    2, 0, 0,
    NULL,
    &ModuloTiposResultadosEcoUrna::ApuracaoMistaMR::defaultTag,
    ModuloTiposResultadosEcoUrna::ApuracaoMistaMR::fieldNames,
    "ApuracaoMistaMR"
};

const char * ModuloTiposResultadosEcoUrna::ApuracaoMistaBUAE::motivoApuracao::value_type::nameList[] = {
    "<undefined>=0",
    "urnaDataHoraIncorreta",
    "urnaComDefeito",
    "urnaOutrasecao",
    "urnaPreparadaIncorretamente",
    "urnaEncerradaComEleitoresNaFila",
    "<undefined>=6",
    "<undefined>=7",
    "<undefined>=8",
    "<undefined>=9",
    "<undefined>=10",
    "<undefined>=11",
    "<undefined>=12",
    "<undefined>=13",
    "<undefined>=14",
    "<undefined>=15",
    "<undefined>=16",
    "<undefined>=17",
    "<undefined>=18",
    "<undefined>=19",
    "<undefined>=20",
    "<undefined>=21",
    "<undefined>=22",
    "<undefined>=23",
    "<undefined>=24",
    "<undefined>=25",
    "<undefined>=26",
    "<undefined>=27",
    "<undefined>=28",
    "<undefined>=29",
    "<undefined>=30",
    "<undefined>=31",
    "<undefined>=32",
    "<undefined>=33",
    "<undefined>=34",
    "<undefined>=35",
    "<undefined>=36",
    "<undefined>=37",
    "<undefined>=38",
    "<undefined>=39",
    "<undefined>=40",
    "<undefined>=41",
    "<undefined>=42",
    "<undefined>=43",
    "<undefined>=44",
    "<undefined>=45",
    "<undefined>=46",
    "<undefined>=47",
    "<undefined>=48",
    "<undefined>=49",
    "<undefined>=50",
    "<undefined>=51",
    "<undefined>=52",
    "<undefined>=53",
    "<undefined>=54",
    "<undefined>=55",
    "<undefined>=56",
    "<undefined>=57",
    "<undefined>=58",
    "<undefined>=59",
    "<undefined>=60",
    "<undefined>=61",
    "<undefined>=62",
    "<undefined>=63",
    "<undefined>=64",
    "<undefined>=65",
    "<undefined>=66",
    "<undefined>=67",
    "<undefined>=68",
    "<undefined>=69",
    "<undefined>=70",
    "<undefined>=71",
    "<undefined>=72",
    "<undefined>=73",
    "<undefined>=74",
    "<undefined>=75",
    "<undefined>=76",
    "<undefined>=77",
    "<undefined>=78",
    "<undefined>=79",
    "<undefined>=80",
    "<undefined>=81",
    "<undefined>=82",
    "<undefined>=83",
    "<undefined>=84",
    "<undefined>=85",
    "<undefined>=86",
    "<undefined>=87",
    "<undefined>=88",
    "<undefined>=89",
    "<undefined>=90",
    "<undefined>=91",
    "<undefined>=92",
    "<undefined>=93",
    "<undefined>=94",
    "<undefined>=95",
    "<undefined>=96",
    "<undefined>=97",
    "<undefined>=98",
    "outros"
  };

const ModuloTiposResultadosEcoUrna::ApuracaoMistaBUAE::motivoApuracao::value_type::InfoType ModuloTiposResultadosEcoUrna::ApuracaoMistaBUAE::motivoApuracao::value_type::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    99, nameList
};

const void* ModuloTiposResultadosEcoUrna::ApuracaoMistaBUAE::fieldInfos[2] = {
    &tipoapuracao::value_type::theInfo,
    &motivoApuracao::value_type::theInfo
};

int ModuloTiposResultadosEcoUrna::ApuracaoMistaBUAE::fieldIds[2] = {
    -1,
    -1
};

const char* ModuloTiposResultadosEcoUrna::ApuracaoMistaBUAE::fieldNames[2] = {
    "tipoapuracao",
    "motivoApuracao"
};

const ModuloTiposResultadosEcoUrna::ApuracaoMistaBUAE::InfoType ModuloTiposResultadosEcoUrna::ApuracaoMistaBUAE::theInfo = {
    ModuloTiposResultadosEcoUrna::ApuracaoMistaBUAE::create,
    0x00000010,
    0,
    false,
    ModuloTiposResultadosEcoUrna::ApuracaoMistaBUAE::fieldInfos,
    ModuloTiposResultadosEcoUrna::ApuracaoMistaBUAE::fieldIds,
    2, 0, 0,
    NULL,
    &ModuloTiposResultadosEcoUrna::ApuracaoMistaBUAE::defaultTag,
    ModuloTiposResultadosEcoUrna::ApuracaoMistaBUAE::fieldNames,
    "ApuracaoMistaBUAE"
};

const char * ModuloTiposResultadosEcoUrna::ApuracaoTotalmenteManualDigitacaoAE::motivoApuracao::value_type::nameList[] = {
    "<undefined>=0",
    "urnaComDefeito",
    "urnaIndisponivelInicio",
    "urnaOutraSecao",
    "<undefined>=4",
    "<undefined>=5",
    "<undefined>=6",
    "<undefined>=7",
    "<undefined>=8",
    "<undefined>=9",
    "<undefined>=10",
    "<undefined>=11",
    "<undefined>=12",
    "<undefined>=13",
    "<undefined>=14",
    "<undefined>=15",
    "<undefined>=16",
    "<undefined>=17",
    "<undefined>=18",
    "<undefined>=19",
    "<undefined>=20",
    "<undefined>=21",
    "<undefined>=22",
    "<undefined>=23",
    "<undefined>=24",
    "<undefined>=25",
    "<undefined>=26",
    "<undefined>=27",
    "<undefined>=28",
    "<undefined>=29",
    "<undefined>=30",
    "<undefined>=31",
    "<undefined>=32",
    "<undefined>=33",
    "<undefined>=34",
    "<undefined>=35",
    "<undefined>=36",
    "<undefined>=37",
    "<undefined>=38",
    "<undefined>=39",
    "<undefined>=40",
    "<undefined>=41",
    "<undefined>=42",
    "<undefined>=43",
    "<undefined>=44",
    "<undefined>=45",
    "<undefined>=46",
    "<undefined>=47",
    "<undefined>=48",
    "<undefined>=49",
    "<undefined>=50",
    "<undefined>=51",
    "<undefined>=52",
    "<undefined>=53",
    "<undefined>=54",
    "<undefined>=55",
    "<undefined>=56",
    "<undefined>=57",
    "<undefined>=58",
    "<undefined>=59",
    "<undefined>=60",
    "<undefined>=61",
    "<undefined>=62",
    "<undefined>=63",
    "<undefined>=64",
    "<undefined>=65",
    "<undefined>=66",
    "<undefined>=67",
    "<undefined>=68",
    "<undefined>=69",
    "<undefined>=70",
    "<undefined>=71",
    "<undefined>=72",
    "<undefined>=73",
    "<undefined>=74",
    "<undefined>=75",
    "<undefined>=76",
    "<undefined>=77",
    "<undefined>=78",
    "<undefined>=79",
    "<undefined>=80",
    "<undefined>=81",
    "<undefined>=82",
    "<undefined>=83",
    "<undefined>=84",
    "<undefined>=85",
    "<undefined>=86",
    "<undefined>=87",
    "<undefined>=88",
    "<undefined>=89",
    "<undefined>=90",
    "<undefined>=91",
    "<undefined>=92",
    "<undefined>=93",
    "<undefined>=94",
    "<undefined>=95",
    "<undefined>=96",
    "<undefined>=97",
    "<undefined>=98",
    "outros"
  };

const ModuloTiposResultadosEcoUrna::ApuracaoTotalmenteManualDigitacaoAE::motivoApuracao::value_type::InfoType ModuloTiposResultadosEcoUrna::ApuracaoTotalmenteManualDigitacaoAE::motivoApuracao::value_type::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    99, nameList
};

const void* ModuloTiposResultadosEcoUrna::ApuracaoTotalmenteManualDigitacaoAE::fieldInfos[2] = {
    &tipoapuracao::value_type::theInfo,
    &motivoApuracao::value_type::theInfo
};

int ModuloTiposResultadosEcoUrna::ApuracaoTotalmenteManualDigitacaoAE::fieldIds[2] = {
    -1,
    -1
};

const char* ModuloTiposResultadosEcoUrna::ApuracaoTotalmenteManualDigitacaoAE::fieldNames[2] = {
    "tipoapuracao",
    "motivoApuracao"
};

const ModuloTiposResultadosEcoUrna::ApuracaoTotalmenteManualDigitacaoAE::InfoType ModuloTiposResultadosEcoUrna::ApuracaoTotalmenteManualDigitacaoAE::theInfo = {
    ModuloTiposResultadosEcoUrna::ApuracaoTotalmenteManualDigitacaoAE::create,
    0x00000010,
    0,
    false,
    ModuloTiposResultadosEcoUrna::ApuracaoTotalmenteManualDigitacaoAE::fieldInfos,
    ModuloTiposResultadosEcoUrna::ApuracaoTotalmenteManualDigitacaoAE::fieldIds,
    2, 0, 0,
    NULL,
    &ModuloTiposResultadosEcoUrna::ApuracaoTotalmenteManualDigitacaoAE::defaultTag,
    ModuloTiposResultadosEcoUrna::ApuracaoTotalmenteManualDigitacaoAE::fieldNames,
    "ApuracaoTotalmenteManualDigitacaoAE"
};

const char * ModuloTiposResultadosEcoUrna::ApuracaoEletronica::motivoApuracao::value_type::nameList[] = {
    "<undefined>=0",
    "naoFoiPossivelReuperarResultado",
    "urnaNaoChegouMidiaDefeituosa",
    "urnaNaoChegouMidiaExtraviada",
    "<undefined>=4",
    "<undefined>=5",
    "<undefined>=6",
    "<undefined>=7",
    "<undefined>=8",
    "<undefined>=9",
    "<undefined>=10",
    "<undefined>=11",
    "<undefined>=12",
    "<undefined>=13",
    "<undefined>=14",
    "<undefined>=15",
    "<undefined>=16",
    "<undefined>=17",
    "<undefined>=18",
    "<undefined>=19",
    "<undefined>=20",
    "<undefined>=21",
    "<undefined>=22",
    "<undefined>=23",
    "<undefined>=24",
    "<undefined>=25",
    "<undefined>=26",
    "<undefined>=27",
    "<undefined>=28",
    "<undefined>=29",
    "<undefined>=30",
    "<undefined>=31",
    "<undefined>=32",
    "<undefined>=33",
    "<undefined>=34",
    "<undefined>=35",
    "<undefined>=36",
    "<undefined>=37",
    "<undefined>=38",
    "<undefined>=39",
    "<undefined>=40",
    "<undefined>=41",
    "<undefined>=42",
    "<undefined>=43",
    "<undefined>=44",
    "<undefined>=45",
    "<undefined>=46",
    "<undefined>=47",
    "<undefined>=48",
    "<undefined>=49",
    "<undefined>=50",
    "<undefined>=51",
    "<undefined>=52",
    "<undefined>=53",
    "<undefined>=54",
    "<undefined>=55",
    "<undefined>=56",
    "<undefined>=57",
    "<undefined>=58",
    "<undefined>=59",
    "<undefined>=60",
    "<undefined>=61",
    "<undefined>=62",
    "<undefined>=63",
    "<undefined>=64",
    "<undefined>=65",
    "<undefined>=66",
    "<undefined>=67",
    "<undefined>=68",
    "<undefined>=69",
    "<undefined>=70",
    "<undefined>=71",
    "<undefined>=72",
    "<undefined>=73",
    "<undefined>=74",
    "<undefined>=75",
    "<undefined>=76",
    "<undefined>=77",
    "<undefined>=78",
    "<undefined>=79",
    "<undefined>=80",
    "<undefined>=81",
    "<undefined>=82",
    "<undefined>=83",
    "<undefined>=84",
    "<undefined>=85",
    "<undefined>=86",
    "<undefined>=87",
    "<undefined>=88",
    "<undefined>=89",
    "<undefined>=90",
    "<undefined>=91",
    "<undefined>=92",
    "<undefined>=93",
    "<undefined>=94",
    "<undefined>=95",
    "<undefined>=96",
    "<undefined>=97",
    "<undefined>=98",
    "outros"
  };

const ModuloTiposResultadosEcoUrna::ApuracaoEletronica::motivoApuracao::value_type::InfoType ModuloTiposResultadosEcoUrna::ApuracaoEletronica::motivoApuracao::value_type::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    99, nameList
};

const void* ModuloTiposResultadosEcoUrna::ApuracaoEletronica::fieldInfos[2] = {
    &tipoapuracao::value_type::theInfo,
    &motivoApuracao::value_type::theInfo
};

int ModuloTiposResultadosEcoUrna::ApuracaoEletronica::fieldIds[2] = {
    -1,
    -1
};

const char* ModuloTiposResultadosEcoUrna::ApuracaoEletronica::fieldNames[2] = {
    "tipoapuracao",
    "motivoApuracao"
};

const ModuloTiposResultadosEcoUrna::ApuracaoEletronica::InfoType ModuloTiposResultadosEcoUrna::ApuracaoEletronica::theInfo = {
    ModuloTiposResultadosEcoUrna::ApuracaoEletronica::create,
    0x00000010,
    0,
    false,
    ModuloTiposResultadosEcoUrna::ApuracaoEletronica::fieldInfos,
    ModuloTiposResultadosEcoUrna::ApuracaoEletronica::fieldIds,
    2, 0, 0,
    NULL,
    &ModuloTiposResultadosEcoUrna::ApuracaoEletronica::defaultTag,
    ModuloTiposResultadosEcoUrna::ApuracaoEletronica::fieldNames,
    "ApuracaoEletronica"
};

const void* ModuloTiposResultadosEcoUrna::Urna::motivoUtilizacaoSA::value_type::selectionInfos[4] = {
    &apuracaoMistaMR::value_type::theInfo,
    &apuracaoMistaBUAE::value_type::theInfo,
    &apuracaoTotalmenteManual::value_type::theInfo,
    &apuracaoEletronica::value_type::theInfo
};

const char* ModuloTiposResultadosEcoUrna::Urna::motivoUtilizacaoSA::value_type::selectionNames[4] = {
    "apuracaoMistaMR",
    "apuracaoMistaBUAE",
    "apuracaoTotalmenteManual",
    "apuracaoEletronica"
};

const ModuloTiposResultadosEcoUrna::Urna::motivoUtilizacaoSA::value_type::InfoType ModuloTiposResultadosEcoUrna::Urna::motivoUtilizacaoSA::value_type::theInfo = {
    ModuloTiposResultadosEcoUrna::Urna::motivoUtilizacaoSA::value_type::create,
    0x00000000,
    0,
    false,
    ModuloTiposResultadosEcoUrna::Urna::motivoUtilizacaoSA::value_type::selectionInfos,
    4, 4,
    NULL,
    ModuloTiposResultadosEcoUrna::Urna::motivoUtilizacaoSA::value_type::selectionNames
};

const void* ModuloTiposResultadosEcoUrna::Urna::fieldInfos[6] = {
    &tipoUrna::value_type::theInfo,
    &versaoVotacao::value_type::theInfo,
    &correspondenciaResultado::value_type::theInfo,
    &tipoArquivo::value_type::theInfo,
    &numeroSerieFV::value_type::theInfo,
    &motivoUtilizacaoSA::value_type::theInfo
};

int ModuloTiposResultadosEcoUrna::Urna::fieldIds[6] = {
    -1,
    -1,
    -1,
    -1,
    -1,
    0
};

const char* ModuloTiposResultadosEcoUrna::Urna::fieldNames[6] = {
    "tipoUrna",
    "versaoVotacao",
    "correspondenciaResultado",
    "tipoArquivo",
    "numeroSerieFV",
    "motivoUtilizacaoSA"
};

const ModuloTiposResultadosEcoUrna::Urna::InfoType ModuloTiposResultadosEcoUrna::Urna::theInfo = {
    ModuloTiposResultadosEcoUrna::Urna::create,
    0x00000010,
    0,
    false,
    ModuloTiposResultadosEcoUrna::Urna::fieldInfos,
    ModuloTiposResultadosEcoUrna::Urna::fieldIds,
    6, 0, 1,
    NULL,
    &ModuloTiposResultadosEcoUrna::Urna::defaultTag,
    ModuloTiposResultadosEcoUrna::Urna::fieldNames,
    "Urna"
};

const ModuloTiposResultadosEcoUrna::DadosCandidato::nome::value_type::InfoType ModuloTiposResultadosEcoUrna::DadosCandidato::nome::value_type::theInfo = {
    ASN1::AbstractString::create,
    0x0000001b,
    &ASN1::GeneralString::theInfo,
    ASN1::FixedConstraint, 1, 70,
    ASN1::GeneralString::theInfo.characterSet, ASN1::GeneralString::theInfo.characterSetSize, ASN1::GeneralString::theInfo.canonicalSetBits,
    ASN1::GeneralString::theInfo.charSetUnalignedBits, ASN1::GeneralString::theInfo.charSetAlignedBits
};

const ModuloTiposResultadosEcoUrna::DadosCandidato::nomeUrna::value_type::InfoType ModuloTiposResultadosEcoUrna::DadosCandidato::nomeUrna::value_type::theInfo = {
    ASN1::AbstractString::create,
    0x0000001b,
    &ASN1::GeneralString::theInfo,
    ASN1::FixedConstraint, 1, 30,
    ASN1::GeneralString::theInfo.characterSet, ASN1::GeneralString::theInfo.characterSetSize, ASN1::GeneralString::theInfo.canonicalSetBits,
    ASN1::GeneralString::theInfo.charSetUnalignedBits, ASN1::GeneralString::theInfo.charSetAlignedBits
};

const ModuloTiposResultadosEcoUrna::DadosCandidato::nomeSocial::value_type::InfoType ModuloTiposResultadosEcoUrna::DadosCandidato::nomeSocial::value_type::theInfo = {
    ASN1::AbstractString::create,
    0x0000001b,
    &ASN1::GeneralString::theInfo,
    ASN1::FixedConstraint, 1, 70,
    ASN1::GeneralString::theInfo.characterSet, ASN1::GeneralString::theInfo.characterSetSize, ASN1::GeneralString::theInfo.canonicalSetBits,
    ASN1::GeneralString::theInfo.charSetUnalignedBits, ASN1::GeneralString::theInfo.charSetAlignedBits
};

const ModuloTiposResultadosEcoUrna::DadosCandidato::dataNascimento::value_type::InfoType ModuloTiposResultadosEcoUrna::DadosCandidato::dataNascimento::value_type::theInfo = {
    ASN1::AbstractString::create,
    0x00000012,
    &ASN1::NumericString::theInfo,
    ASN1::FixedConstraint, 8, 8,
    ASN1::NumericString::theInfo.characterSet, ASN1::NumericString::theInfo.characterSetSize, ASN1::NumericString::theInfo.canonicalSetBits,
    ASN1::NumericString::theInfo.charSetUnalignedBits, ASN1::NumericString::theInfo.charSetAlignedBits
};

const char * ModuloTiposResultadosEcoUrna::DadosCandidato::sexo::value_type::nameList[] = {
    "sexoNaoInformado",
    "<undefined>=1",
    "sexoMasculino",
    "<undefined>=3",
    "sexoFeminino"
  };

const ModuloTiposResultadosEcoUrna::DadosCandidato::sexo::value_type::InfoType ModuloTiposResultadosEcoUrna::DadosCandidato::sexo::value_type::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    4, nameList
};

const char * ModuloTiposResultadosEcoUrna::DadosCandidato::situacao::value_type::nameList[] = {
    "<undefined>=0",
    "cadastrado",
    "deferido",
    "inapto",
    "indeferidoComRecurso",
    "cancelado",
    "renuncia",
    "falecido",
    "aguardandoJulgamento",
    "<undefined>=9",
    "<undefined>=10",
    "impugnado",
    "apto",
    "pedidoNaoConhecido",
    "indeferido",
    "comNoticiaInelegibilidade",
    "deferidoComRecurso",
    "candidatoPendenteJulgamento",
    "<undefined>=18",
    "canceladoComRecurso",
    "pedidoNaoConhecidoComRecurso"
  };

const ModuloTiposResultadosEcoUrna::DadosCandidato::situacao::value_type::InfoType ModuloTiposResultadosEcoUrna::DadosCandidato::situacao::value_type::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    20, nameList
};

const char * ModuloTiposResultadosEcoUrna::DadosCandidato::situacaoCassacao::value_type::nameList[] = {
    "<undefined>=0",
    "cassado",
    "cassadoComRecurso",
    "regular",
    "levantado"
  };

const ModuloTiposResultadosEcoUrna::DadosCandidato::situacaoCassacao::value_type::InfoType ModuloTiposResultadosEcoUrna::DadosCandidato::situacaoCassacao::value_type::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    4, nameList
};

const void* ModuloTiposResultadosEcoUrna::DadosCandidato::fieldInfos[13] = {
    &codigo::value_type::theInfo,
    &candidatoSubstituido::value_type::theInfo,
    &nome::value_type::theInfo,
    &nomeUrna::value_type::theInfo,
    &nomeSocial::value_type::theInfo,
    &nomeFonetico::value_type::theInfo,
    &dataNascimento::value_type::theInfo,
    &sexo::value_type::theInfo,
    &situacao::value_type::theInfo,
    &situacaoCassacao::value_type::theInfo,
    &partido::value_type::theInfo,
    &ordemSuplencia::value_type::theInfo,
    &reeleicao::value_type::theInfo
};

int ModuloTiposResultadosEcoUrna::DadosCandidato::fieldIds[13] = {
    -1,
    0,
    -1,
    -1,
    1,
    2,
    -1,
    -1,
    -1,
    3,
    -1,
    4,
    -1
};

unsigned ModuloTiposResultadosEcoUrna::DadosCandidato::fieldTags[13] = {
    0x80000001,
    0x80000002,
    0x0000001b,
    0x0000001b,
    0x0000001b,
    0x80000003,
    0x00000012,
    0x00000000,
    0x00000000,
    0x80000004,
    0x00000000,
    0x80000005,
    0x00000001
};

const char* ModuloTiposResultadosEcoUrna::DadosCandidato::fieldNames[13] = {
    "codigo",
    "candidatoSubstituido",
    "nome",
    "nomeUrna",
    "nomeSocial",
    "nomeFonetico",
    "dataNascimento",
    "sexo",
    "situacao",
    "situacaoCassacao",
    "partido",
    "ordemSuplencia",
    "reeleicao"
};

const ModuloTiposResultadosEcoUrna::DadosCandidato::InfoType ModuloTiposResultadosEcoUrna::DadosCandidato::theInfo = {
    ModuloTiposResultadosEcoUrna::DadosCandidato::create,
    0x00000010,
    0,
    false,
    ModuloTiposResultadosEcoUrna::DadosCandidato::fieldInfos,
    ModuloTiposResultadosEcoUrna::DadosCandidato::fieldIds,
    13, 0, 5,
    NULL,
    ModuloTiposResultadosEcoUrna::DadosCandidato::fieldTags,
    ModuloTiposResultadosEcoUrna::DadosCandidato::fieldNames,
    "DadosCandidato"
};

const ModuloTiposResultadosEcoUrna::Candidatura::suplentesVices::value_type::InfoType ModuloTiposResultadosEcoUrna::Candidatura::suplentesVices::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloTiposResultadosEcoUrna::DadosCandidato::theInfo,
    0
};

const void* ModuloTiposResultadosEcoUrna::Candidatura::fieldInfos[4] = {
    &numero::value_type::theInfo,
    &titular::value_type::theInfo,
    &drap::value_type::theInfo,
    &suplentesVices::value_type::theInfo
};

int ModuloTiposResultadosEcoUrna::Candidatura::fieldIds[4] = {
    -1,
    -1,
    -1,
    0
};

const char* ModuloTiposResultadosEcoUrna::Candidatura::fieldNames[4] = {
    "numero",
    "titular",
    "drap",
    "suplentesVices"
};

const ModuloTiposResultadosEcoUrna::Candidatura::InfoType ModuloTiposResultadosEcoUrna::Candidatura::theInfo = {
    ModuloTiposResultadosEcoUrna::Candidatura::create,
    0x00000010,
    0,
    false,
    ModuloTiposResultadosEcoUrna::Candidatura::fieldInfos,
    ModuloTiposResultadosEcoUrna::Candidatura::fieldIds,
    4, 0, 1,
    NULL,
    &ModuloTiposResultadosEcoUrna::Candidatura::defaultTag,
    ModuloTiposResultadosEcoUrna::Candidatura::fieldNames,
    "Candidatura"
};

const ModuloTiposResultadosEcoUrna::CandidatosAptos::InfoType ModuloTiposResultadosEcoUrna::CandidatosAptos::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloTiposResultadosEcoUrna::Candidatura::theInfo,
    0
};

const void* ModuloTiposResultadosEcoUrna::CandidatoPorPartido::fieldInfos[3] = {
    &partido::value_type::theInfo,
    &candidatosAptos::value_type::theInfo,
    &candidatosInaptos::value_type::theInfo
};

int ModuloTiposResultadosEcoUrna::CandidatoPorPartido::fieldIds[3] = {
    -1,
    -1,
    -1
};

const char* ModuloTiposResultadosEcoUrna::CandidatoPorPartido::fieldNames[3] = {
    "partido",
    "candidatosAptos",
    "candidatosInaptos"
};

const ModuloTiposResultadosEcoUrna::CandidatoPorPartido::InfoType ModuloTiposResultadosEcoUrna::CandidatoPorPartido::theInfo = {
    ModuloTiposResultadosEcoUrna::CandidatoPorPartido::create,
    0x00000010,
    0,
    false,
    ModuloTiposResultadosEcoUrna::CandidatoPorPartido::fieldInfos,
    ModuloTiposResultadosEcoUrna::CandidatoPorPartido::fieldIds,
    3, 0, 0,
    NULL,
    &ModuloTiposResultadosEcoUrna::CandidatoPorPartido::defaultTag,
    ModuloTiposResultadosEcoUrna::CandidatoPorPartido::fieldNames,
    "CandidatoPorPartido"
};

} // namespace ModuloTiposResultadosEcoUrna
