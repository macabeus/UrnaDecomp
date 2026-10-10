#include "ModuloTiposEleitorais.h"

namespace ModuloTiposEleitorais{

const ModuloTiposEleitorais::SiglaUF::InfoType ModuloTiposEleitorais::SiglaUF::theInfo = {
    ASN1::AbstractString::create,
    0x0000001b,
    &ASN1::GeneralString::theInfo,
    ASN1::FixedConstraint, 2, 2,
    ASN1::GeneralString::theInfo.characterSet, ASN1::GeneralString::theInfo.characterSetSize, ASN1::GeneralString::theInfo.canonicalSetBits,
    ASN1::GeneralString::theInfo.charSetUnalignedBits, ASN1::GeneralString::theInfo.charSetAlignedBits
};

const ModuloTiposEleitorais::UF::nome::value_type::InfoType ModuloTiposEleitorais::UF::nome::value_type::theInfo = {
    ASN1::AbstractString::create,
    0x0000001b,
    &ASN1::GeneralString::theInfo,
    ASN1::FixedConstraint, 1, 35,
    ASN1::GeneralString::theInfo.characterSet, ASN1::GeneralString::theInfo.characterSetSize, ASN1::GeneralString::theInfo.canonicalSetBits,
    ASN1::GeneralString::theInfo.charSetUnalignedBits, ASN1::GeneralString::theInfo.charSetAlignedBits
};

const void* ModuloTiposEleitorais::UF::fieldInfos[2] = {
    &sigla::value_type::theInfo,
    &nome::value_type::theInfo
};

int ModuloTiposEleitorais::UF::fieldIds[2] = {
    -1,
    -1
};

const char* ModuloTiposEleitorais::UF::fieldNames[2] = {
    "sigla",
    "nome"
};

const ModuloTiposEleitorais::UF::InfoType ModuloTiposEleitorais::UF::theInfo = {
    ModuloTiposEleitorais::UF::create,
    0x00000010,
    0,
    false,
    ModuloTiposEleitorais::UF::fieldInfos,
    ModuloTiposEleitorais::UF::fieldIds,
    2, 0, 0,
    NULL,
    &ModuloTiposEleitorais::UF::defaultTag,
    ModuloTiposEleitorais::UF::fieldNames,
    "UF"
};

const void* ModuloTiposEleitorais::MunicipioZona::fieldInfos[2] = {
    &municipio::value_type::theInfo,
    &zona::value_type::theInfo
};

int ModuloTiposEleitorais::MunicipioZona::fieldIds[2] = {
    -1,
    -1
};

const char* ModuloTiposEleitorais::MunicipioZona::fieldNames[2] = {
    "municipio",
    "zona"
};

const ModuloTiposEleitorais::MunicipioZona::InfoType ModuloTiposEleitorais::MunicipioZona::theInfo = {
    ModuloTiposEleitorais::MunicipioZona::create,
    0x00000010,
    0,
    false,
    ModuloTiposEleitorais::MunicipioZona::fieldInfos,
    ModuloTiposEleitorais::MunicipioZona::fieldIds,
    2, 0, 0,
    NULL,
    &ModuloTiposEleitorais::MunicipioZona::defaultTag,
    ModuloTiposEleitorais::MunicipioZona::fieldNames,
    "MunicipioZona"
};

const char * ModuloTiposEleitorais::TipoIdentificadorEleitor::nameList[] = {
    "<undefined>=0",
    "numeroInscricaoEleitoral",
    "numeroCPF",
    "numeroLivre"
  };

const ModuloTiposEleitorais::TipoIdentificadorEleitor::InfoType ModuloTiposEleitorais::TipoIdentificadorEleitor::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    3, nameList
};

const ModuloTiposEleitorais::IdentificadorEleitor::numeroInscricao::value_type::InfoType ModuloTiposEleitorais::IdentificadorEleitor::numeroInscricao::value_type::theInfo = {
    ASN1::AbstractString::create,
    0x00000012,
    &ASN1::NumericString::theInfo,
    ASN1::FixedConstraint, 12, 12,
    ASN1::NumericString::theInfo.characterSet, ASN1::NumericString::theInfo.characterSetSize, ASN1::NumericString::theInfo.canonicalSetBits,
    ASN1::NumericString::theInfo.charSetUnalignedBits, ASN1::NumericString::theInfo.charSetAlignedBits
};

const ModuloTiposEleitorais::IdentificadorEleitor::numeroCPF::value_type::InfoType ModuloTiposEleitorais::IdentificadorEleitor::numeroCPF::value_type::theInfo = {
    ASN1::AbstractString::create,
    0x00000012,
    &ASN1::NumericString::theInfo,
    ASN1::FixedConstraint, 11, 11,
    ASN1::NumericString::theInfo.characterSet, ASN1::NumericString::theInfo.characterSetSize, ASN1::NumericString::theInfo.canonicalSetBits,
    ASN1::NumericString::theInfo.charSetUnalignedBits, ASN1::NumericString::theInfo.charSetAlignedBits
};

const ModuloTiposEleitorais::IdentificadorEleitor::identificacaoLivre::value_type::InfoType ModuloTiposEleitorais::IdentificadorEleitor::identificacaoLivre::value_type::theInfo = {
    ASN1::AbstractString::create,
    0x00000012,
    &ASN1::NumericString::theInfo,
    ASN1::FixedConstraint, 12, 12,
    ASN1::NumericString::theInfo.characterSet, ASN1::NumericString::theInfo.characterSetSize, ASN1::NumericString::theInfo.canonicalSetBits,
    ASN1::NumericString::theInfo.charSetUnalignedBits, ASN1::NumericString::theInfo.charSetAlignedBits
};

const void* ModuloTiposEleitorais::IdentificadorEleitor::selectionInfos[3] = {
    &numeroInscricao::value_type::theInfo,
    &numeroCPF::value_type::theInfo,
    &identificacaoLivre::value_type::theInfo
};

const char* ModuloTiposEleitorais::IdentificadorEleitor::selectionNames[3] = {
    "numeroInscricao",
    "numeroCPF",
    "identificacaoLivre"
};

const ModuloTiposEleitorais::IdentificadorEleitor::InfoType ModuloTiposEleitorais::IdentificadorEleitor::theInfo = {
    ModuloTiposEleitorais::IdentificadorEleitor::create,
    0x00000000,
    0,
    false,
    ModuloTiposEleitorais::IdentificadorEleitor::selectionInfos,
    3, 3,
    NULL,
    ModuloTiposEleitorais::IdentificadorEleitor::selectionNames
};

const char * ModuloTiposEleitorais::FormatoImagem::nameList[] = {
    "<undefined>=0",
    "formatoJPEG",
    "formatoBMP"
  };

const ModuloTiposEleitorais::FormatoImagem::InfoType ModuloTiposEleitorais::FormatoImagem::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    2, nameList
};

const void* ModuloTiposEleitorais::IDEleitoral::selectionInfos[3] = {
    &idProcessoEleitoral::value_type::theInfo,
    &idPleito::value_type::theInfo,
    &idEleicao::value_type::theInfo
};

unsigned ModuloTiposEleitorais::IDEleitoral::selectionTags[3] = {
    0x80000001,
    0x80000002,
    0x80000003
};

const char* ModuloTiposEleitorais::IDEleitoral::selectionNames[3] = {
    "idProcessoEleitoral",
    "idPleito",
    "idEleicao"
};

const ModuloTiposEleitorais::IDEleitoral::InfoType ModuloTiposEleitorais::IDEleitoral::theInfo = {
    ModuloTiposEleitorais::IDEleitoral::create,
    0x00000000,
    0,
    false,
    ModuloTiposEleitorais::IDEleitoral::selectionInfos,
    3, 3,
    ModuloTiposEleitorais::IDEleitoral::selectionTags,
    ModuloTiposEleitorais::IDEleitoral::selectionNames
};

const char * ModuloTiposEleitorais::Fase::nameList[] = {
    "<undefined>=0",
    "simulado",
    "oficial",
    "treinamento"
  };

const ModuloTiposEleitorais::Fase::InfoType ModuloTiposEleitorais::Fase::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    3, nameList
};

const char * ModuloTiposEleitorais::Turno::nameList[] = {
    "semTurno",
    "turno1",
    "turno2"
  };

const ModuloTiposEleitorais::Turno::InfoType ModuloTiposEleitorais::Turno::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    2, nameList
};

const void* ModuloTiposEleitorais::IdentificacaoSecaoEleitoral::fieldInfos[3] = {
    &municipioZona::value_type::theInfo,
    &local::value_type::theInfo,
    &secao::value_type::theInfo
};

int ModuloTiposEleitorais::IdentificacaoSecaoEleitoral::fieldIds[3] = {
    -1,
    -1,
    -1
};

const char* ModuloTiposEleitorais::IdentificacaoSecaoEleitoral::fieldNames[3] = {
    "municipioZona",
    "local",
    "secao"
};

const ModuloTiposEleitorais::IdentificacaoSecaoEleitoral::InfoType ModuloTiposEleitorais::IdentificacaoSecaoEleitoral::theInfo = {
    ModuloTiposEleitorais::IdentificacaoSecaoEleitoral::create,
    0x00000010,
    0,
    false,
    ModuloTiposEleitorais::IdentificacaoSecaoEleitoral::fieldInfos,
    ModuloTiposEleitorais::IdentificacaoSecaoEleitoral::fieldIds,
    3, 0, 0,
    NULL,
    &ModuloTiposEleitorais::IdentificacaoSecaoEleitoral::defaultTag,
    ModuloTiposEleitorais::IdentificacaoSecaoEleitoral::fieldNames,
    "IdentificacaoSecaoEleitoral"
};

const void* ModuloTiposEleitorais::IdentificacaoContingencia::fieldInfos[1] = {
    &municipioZona::value_type::theInfo
};

int ModuloTiposEleitorais::IdentificacaoContingencia::fieldIds[1] = {
    -1
};

const char* ModuloTiposEleitorais::IdentificacaoContingencia::fieldNames[1] = {
    "municipioZona"
};

const ModuloTiposEleitorais::IdentificacaoContingencia::InfoType ModuloTiposEleitorais::IdentificacaoContingencia::theInfo = {
    ModuloTiposEleitorais::IdentificacaoContingencia::create,
    0x00000010,
    0,
    false,
    ModuloTiposEleitorais::IdentificacaoContingencia::fieldInfos,
    ModuloTiposEleitorais::IdentificacaoContingencia::fieldIds,
    1, 0, 0,
    NULL,
    &ModuloTiposEleitorais::IdentificacaoContingencia::defaultTag,
    ModuloTiposEleitorais::IdentificacaoContingencia::fieldNames,
    "IdentificacaoContingencia"
};

const char * ModuloTiposEleitorais::Tipo::nameList[] = {
    "<undefined>=0",
    "majoritario",
    "proporcional",
    "consulta"
  };

const ModuloTiposEleitorais::Tipo::InfoType ModuloTiposEleitorais::Tipo::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    3, nameList
};

const char * ModuloTiposEleitorais::TipoAbrangencia::nameList[] = {
    "municipal",
    "estadual",
    "federal"
  };

const ModuloTiposEleitorais::TipoAbrangencia::InfoType ModuloTiposEleitorais::TipoAbrangencia::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    2, nameList
};

const void* ModuloTiposEleitorais::IdentificacaoAbrangencia::fieldInfos[2] = {
    &siglaUF::value_type::theInfo,
    &codigoMunicipio::value_type::theInfo
};

int ModuloTiposEleitorais::IdentificacaoAbrangencia::fieldIds[2] = {
    -1,
    0
};

const char* ModuloTiposEleitorais::IdentificacaoAbrangencia::fieldNames[2] = {
    "siglaUF",
    "codigoMunicipio"
};

const ModuloTiposEleitorais::IdentificacaoAbrangencia::InfoType ModuloTiposEleitorais::IdentificacaoAbrangencia::theInfo = {
    ModuloTiposEleitorais::IdentificacaoAbrangencia::create,
    0x00000010,
    0,
    false,
    ModuloTiposEleitorais::IdentificacaoAbrangencia::fieldInfos,
    ModuloTiposEleitorais::IdentificacaoAbrangencia::fieldIds,
    2, 0, 1,
    NULL,
    &ModuloTiposEleitorais::IdentificacaoAbrangencia::defaultTag,
    ModuloTiposEleitorais::IdentificacaoAbrangencia::fieldNames,
    "IdentificacaoAbrangencia"
};

const void* ModuloTiposEleitorais::Abrangencia::fieldInfos[2] = {
    &tipo::value_type::theInfo,
    &id::value_type::theInfo
};

int ModuloTiposEleitorais::Abrangencia::fieldIds[2] = {
    -1,
    0
};

const char* ModuloTiposEleitorais::Abrangencia::fieldNames[2] = {
    "tipo",
    "id"
};

const ModuloTiposEleitorais::Abrangencia::InfoType ModuloTiposEleitorais::Abrangencia::theInfo = {
    ModuloTiposEleitorais::Abrangencia::create,
    0x00000010,
    0,
    false,
    ModuloTiposEleitorais::Abrangencia::fieldInfos,
    ModuloTiposEleitorais::Abrangencia::fieldIds,
    2, 0, 1,
    NULL,
    &ModuloTiposEleitorais::Abrangencia::defaultTag,
    ModuloTiposEleitorais::Abrangencia::fieldNames,
    "Abrangencia"
};

const char * ModuloTiposEleitorais::TipoDedo::nameList[] = {
    "naoIdentificado",
    "polegarDireito",
    "indicadorDireito",
    "medioDireito",
    "anelarDireito",
    "minimoDireito",
    "polegarEsquerdo",
    "indicadorEsquerdo",
    "medioEsquerdo",
    "anelarEsquerdo",
    "minimoEsquerdo"
  };

const ModuloTiposEleitorais::TipoDedo::InfoType ModuloTiposEleitorais::TipoDedo::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    10, nameList
};

const void* ModuloTiposEleitorais::ArquivoAssinatura::fieldInfos[2] = {
    &nomeArquivo::value_type::theInfo,
    &assinatura::value_type::theInfo
};

int ModuloTiposEleitorais::ArquivoAssinatura::fieldIds[2] = {
    -1,
    -1
};

const char* ModuloTiposEleitorais::ArquivoAssinatura::fieldNames[2] = {
    "nomeArquivo",
    "assinatura"
};

const ModuloTiposEleitorais::ArquivoAssinatura::InfoType ModuloTiposEleitorais::ArquivoAssinatura::theInfo = {
    ModuloTiposEleitorais::ArquivoAssinatura::create,
    0x00000010,
    0,
    false,
    ModuloTiposEleitorais::ArquivoAssinatura::fieldInfos,
    ModuloTiposEleitorais::ArquivoAssinatura::fieldIds,
    2, 0, 0,
    NULL,
    &ModuloTiposEleitorais::ArquivoAssinatura::defaultTag,
    ModuloTiposEleitorais::ArquivoAssinatura::fieldNames,
    "ArquivoAssinatura"
};

const void* ModuloTiposEleitorais::Seguranca::fieldInfos[4] = {
    &idTipoArquivo::value_type::theInfo,
    &idCriptografia::value_type::theInfo,
    &idArquivoCD::value_type::theInfo,
    &idArquivoChave::value_type::theInfo
};

int ModuloTiposEleitorais::Seguranca::fieldIds[4] = {
    -1,
    -1,
    -1,
    -1
};

const char* ModuloTiposEleitorais::Seguranca::fieldNames[4] = {
    "idTipoArquivo",
    "idCriptografia",
    "idArquivoCD",
    "idArquivoChave"
};

const ModuloTiposEleitorais::Seguranca::InfoType ModuloTiposEleitorais::Seguranca::theInfo = {
    ModuloTiposEleitorais::Seguranca::create,
    0x00000010,
    0,
    false,
    ModuloTiposEleitorais::Seguranca::fieldInfos,
    ModuloTiposEleitorais::Seguranca::fieldIds,
    4, 0, 0,
    NULL,
    &ModuloTiposEleitorais::Seguranca::defaultTag,
    ModuloTiposEleitorais::Seguranca::fieldNames,
    "Seguranca"
};

const char * ModuloTiposEleitorais::CodigoCargoConsulta::cargoConstitucional::value_type::nameList[] = {
    "<undefined>=0",
    "presidente",
    "vicePresidente",
    "governador",
    "viceGovernador",
    "senador",
    "deputadoFederal",
    "deputadoEstadual",
    "deputadoDistrital",
    "primeiroSuplenteSenador",
    "segundoSuplenteSenador",
    "prefeito",
    "vicePrefeito",
    "vereador"
  };

const ModuloTiposEleitorais::CodigoCargoConsulta::cargoConstitucional::value_type::InfoType ModuloTiposEleitorais::CodigoCargoConsulta::cargoConstitucional::value_type::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    13, nameList
};

const void* ModuloTiposEleitorais::CodigoCargoConsulta::selectionInfos[2] = {
    &cargoConstitucional::value_type::theInfo,
    &numeroCargoConsultaLivre::value_type::theInfo
};

unsigned ModuloTiposEleitorais::CodigoCargoConsulta::selectionTags[2] = {
    0x80000001,
    0x80000002
};

const char* ModuloTiposEleitorais::CodigoCargoConsulta::selectionNames[2] = {
    "cargoConstitucional",
    "numeroCargoConsultaLivre"
};

const ModuloTiposEleitorais::CodigoCargoConsulta::InfoType ModuloTiposEleitorais::CodigoCargoConsulta::theInfo = {
    ModuloTiposEleitorais::CodigoCargoConsulta::create,
    0x00000000,
    0,
    false,
    ModuloTiposEleitorais::CodigoCargoConsulta::selectionInfos,
    2, 2,
    ModuloTiposEleitorais::CodigoCargoConsulta::selectionTags,
    ModuloTiposEleitorais::CodigoCargoConsulta::selectionNames
};

const void* ModuloTiposEleitorais::Foto::fieldInfos[2] = {
    &formato::value_type::theInfo,
    &imagem::value_type::theInfo
};

int ModuloTiposEleitorais::Foto::fieldIds[2] = {
    -1,
    -1
};

const char* ModuloTiposEleitorais::Foto::fieldNames[2] = {
    "formato",
    "imagem"
};

const ModuloTiposEleitorais::Foto::InfoType ModuloTiposEleitorais::Foto::theInfo = {
    ModuloTiposEleitorais::Foto::create,
    0x00000010,
    0,
    false,
    ModuloTiposEleitorais::Foto::fieldInfos,
    ModuloTiposEleitorais::Foto::fieldIds,
    2, 0, 0,
    NULL,
    &ModuloTiposEleitorais::Foto::defaultTag,
    ModuloTiposEleitorais::Foto::fieldNames,
    "Foto"
};

const ModuloTiposEleitorais::DataGeracao::InfoType ModuloTiposEleitorais::DataGeracao::theInfo = {
    ASN1::AbstractString::create,
    0x0000001b,
    &ASN1::GeneralString::theInfo,
    ASN1::FixedConstraint, 15, 15,
    ASN1::GeneralString::theInfo.characterSet, ASN1::GeneralString::theInfo.characterSetSize, ASN1::GeneralString::theInfo.canonicalSetBits,
    ASN1::GeneralString::theInfo.charSetUnalignedBits, ASN1::GeneralString::theInfo.charSetAlignedBits
};

const ModuloTiposEleitorais::DataHoraJE::InfoType ModuloTiposEleitorais::DataHoraJE::theInfo = {
    ASN1::AbstractString::create,
    0x0000001b,
    &ASN1::GeneralString::theInfo,
    ASN1::FixedConstraint, 15, 15,
    ASN1::GeneralString::theInfo.characterSet, ASN1::GeneralString::theInfo.characterSetSize, ASN1::GeneralString::theInfo.canonicalSetBits,
    ASN1::GeneralString::theInfo.charSetUnalignedBits, ASN1::GeneralString::theInfo.charSetAlignedBits
};

const void* ModuloTiposEleitorais::CabecalhoEntidade::fieldInfos[2] = {
    &dataGeracao::value_type::theInfo,
    &idEleitoral::value_type::theInfo
};

int ModuloTiposEleitorais::CabecalhoEntidade::fieldIds[2] = {
    -1,
    -1
};

const char* ModuloTiposEleitorais::CabecalhoEntidade::fieldNames[2] = {
    "dataGeracao",
    "idEleitoral"
};

const ModuloTiposEleitorais::CabecalhoEntidade::InfoType ModuloTiposEleitorais::CabecalhoEntidade::theInfo = {
    ModuloTiposEleitorais::CabecalhoEntidade::create,
    0x00000010,
    0,
    false,
    ModuloTiposEleitorais::CabecalhoEntidade::fieldInfos,
    ModuloTiposEleitorais::CabecalhoEntidade::fieldIds,
    2, 0, 0,
    NULL,
    &ModuloTiposEleitorais::CabecalhoEntidade::defaultTag,
    ModuloTiposEleitorais::CabecalhoEntidade::fieldNames,
    "CabecalhoEntidade"
};

const void* ModuloTiposEleitorais::IDPacote::fieldInfos[5] = {
    &idPacoteEleitoral::value_type::theInfo,
    &fase::value_type::theInfo,
    &siglaUF::value_type::theInfo,
    &codigoMunicipio::value_type::theInfo,
    &numeroZona::value_type::theInfo
};

int ModuloTiposEleitorais::IDPacote::fieldIds[5] = {
    -1,
    -1,
    0,
    1,
    2
};

unsigned ModuloTiposEleitorais::IDPacote::fieldTags[5] = {
    0x00000000,
    0x00000000,
    0x80000001,
    0x80000002,
    0x80000003
};

const char* ModuloTiposEleitorais::IDPacote::fieldNames[5] = {
    "idPacoteEleitoral",
    "fase",
    "siglaUF",
    "codigoMunicipio",
    "numeroZona"
};

const ModuloTiposEleitorais::IDPacote::InfoType ModuloTiposEleitorais::IDPacote::theInfo = {
    ModuloTiposEleitorais::IDPacote::create,
    0x00000010,
    0,
    false,
    ModuloTiposEleitorais::IDPacote::fieldInfos,
    ModuloTiposEleitorais::IDPacote::fieldIds,
    5, 0, 3,
    NULL,
    ModuloTiposEleitorais::IDPacote::fieldTags,
    ModuloTiposEleitorais::IDPacote::fieldNames,
    "IDPacote"
};

const ModuloTiposEleitorais::Versao::InfoType ModuloTiposEleitorais::Versao::theInfo = {
    ASN1::AbstractString::create,
    0x00000012,
    &ASN1::NumericString::theInfo,
    ASN1::FixedConstraint, 12, 12,
    ASN1::NumericString::theInfo.characterSet, ASN1::NumericString::theInfo.characterSetSize, ASN1::NumericString::theInfo.canonicalSetBits,
    ASN1::NumericString::theInfo.charSetUnalignedBits, ASN1::NumericString::theInfo.charSetAlignedBits
};

const char * ModuloTiposEleitorais::CabecalhoPacote::origem::value_type::nameList[] = {
    "<undefined>=0",
    "configurador",
    "intercad",
    "candidaturas",
    "<undefined>=4",
    "<undefined>=5",
    "gedai",
    "urnaEletronica",
    "simulador",
    "parametrizador",
    "geradorDeBases",
    "sistot",
    "seweb",
    "simon",
    "padaUE"
  };

const ModuloTiposEleitorais::CabecalhoPacote::origem::value_type::InfoType ModuloTiposEleitorais::CabecalhoPacote::origem::value_type::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    14, nameList
};

const void* ModuloTiposEleitorais::CabecalhoPacote::fieldInfos[6] = {
    &tipoPacote::value_type::theInfo,
    &idPacote::value_type::theInfo,
    &nomepacote::value_type::theInfo,
    &versao::value_type::theInfo,
    &abrangencia::value_type::theInfo,
    &origem::value_type::theInfo
};

int ModuloTiposEleitorais::CabecalhoPacote::fieldIds[6] = {
    -1,
    -1,
    -1,
    -1,
    0,
    -1
};

unsigned ModuloTiposEleitorais::CabecalhoPacote::fieldTags[6] = {
    0x00000000,
    0x00000000,
    0x0000001b,
    0x00000000,
    0x80000001,
    0x00000000
};

const char* ModuloTiposEleitorais::CabecalhoPacote::fieldNames[6] = {
    "tipoPacote",
    "idPacote",
    "nomepacote",
    "versao",
    "abrangencia",
    "origem"
};

const ModuloTiposEleitorais::CabecalhoPacote::InfoType ModuloTiposEleitorais::CabecalhoPacote::theInfo = {
    ModuloTiposEleitorais::CabecalhoPacote::create,
    0x00000010,
    0,
    false,
    ModuloTiposEleitorais::CabecalhoPacote::fieldInfos,
    ModuloTiposEleitorais::CabecalhoPacote::fieldIds,
    6, 0, 1,
    NULL,
    ModuloTiposEleitorais::CabecalhoPacote::fieldTags,
    ModuloTiposEleitorais::CabecalhoPacote::fieldNames,
    "CabecalhoPacote"
};

} // namespace ModuloTiposEleitorais
