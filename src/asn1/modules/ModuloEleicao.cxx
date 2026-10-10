#include "ModuloEleicao.h"

namespace ModuloEleicao{

const void* ModuloEleicao::CalendarioEleicao::fieldInfos[8] = {
    &dataInicioPropaganda::value_type::theInfo,
    &dataFimPropaganda::value_type::theInfo,
    &dataInicioRegistroCandidatura::value_type::theInfo,
    &dataFimRegistroCandidatura::value_type::theInfo,
    &dataInicioConvencaoPartidaria::value_type::theInfo,
    &dataFimConvencaoPartidaria::value_type::theInfo,
    &dataInicioPrestacaoConta::value_type::theInfo,
    &dataFimPrestacaoConta::value_type::theInfo
};

int ModuloEleicao::CalendarioEleicao::fieldIds[8] = {
    -1,
    -1,
    0,
    1,
    2,
    3,
    -1,
    -1
};

unsigned ModuloEleicao::CalendarioEleicao::fieldTags[8] = {
    0x00000000,
    0x00000000,
    0x80000001,
    0x80000002,
    0x80000003,
    0x80000004,
    0x00000000,
    0x00000000
};

const char* ModuloEleicao::CalendarioEleicao::fieldNames[8] = {
    "dataInicioPropaganda",
    "dataFimPropaganda",
    "dataInicioRegistroCandidatura",
    "dataFimRegistroCandidatura",
    "dataInicioConvencaoPartidaria",
    "dataFimConvencaoPartidaria",
    "dataInicioPrestacaoConta",
    "dataFimPrestacaoConta"
};

const ModuloEleicao::CalendarioEleicao::InfoType ModuloEleicao::CalendarioEleicao::theInfo = {
    ModuloEleicao::CalendarioEleicao::create,
    0x00000010,
    0,
    false,
    ModuloEleicao::CalendarioEleicao::fieldInfos,
    ModuloEleicao::CalendarioEleicao::fieldIds,
    8, 0, 4,
    NULL,
    ModuloEleicao::CalendarioEleicao::fieldTags,
    ModuloEleicao::CalendarioEleicao::fieldNames,
    "CalendarioEleicao"
};

const ModuloEleicao::RespostaPergunta::resposta::value_type::InfoType ModuloEleicao::RespostaPergunta::resposta::value_type::theInfo = {
    ASN1::AbstractString::create,
    0x0000001b,
    &ASN1::GeneralString::theInfo,
    ASN1::FixedConstraint, 1, 30,
    ASN1::GeneralString::theInfo.characterSet, ASN1::GeneralString::theInfo.characterSetSize, ASN1::GeneralString::theInfo.canonicalSetBits,
    ASN1::GeneralString::theInfo.charSetUnalignedBits, ASN1::GeneralString::theInfo.charSetAlignedBits
};

const void* ModuloEleicao::RespostaPergunta::fieldInfos[3] = {
    &numero::value_type::theInfo,
    &resposta::value_type::theInfo,
    &textoFonetico::value_type::theInfo
};

int ModuloEleicao::RespostaPergunta::fieldIds[3] = {
    -1,
    -1,
    0
};

unsigned ModuloEleicao::RespostaPergunta::fieldTags[3] = {
    0x00000000,
    0x0000001b,
    0x80000001
};

const char* ModuloEleicao::RespostaPergunta::fieldNames[3] = {
    "numero",
    "resposta",
    "textoFonetico"
};

const ModuloEleicao::RespostaPergunta::InfoType ModuloEleicao::RespostaPergunta::theInfo = {
    ModuloEleicao::RespostaPergunta::create,
    0x00000010,
    0,
    false,
    ModuloEleicao::RespostaPergunta::fieldInfos,
    ModuloEleicao::RespostaPergunta::fieldIds,
    3, 0, 1,
    NULL,
    ModuloEleicao::RespostaPergunta::fieldTags,
    ModuloEleicao::RespostaPergunta::fieldNames,
    "RespostaPergunta"
};

const ModuloEleicao::NomesCargo::nomeNeutro::value_type::InfoType ModuloEleicao::NomesCargo::nomeNeutro::value_type::theInfo = {
    ASN1::AbstractString::create,
    0x0000001b,
    &ASN1::GeneralString::theInfo,
    ASN1::FixedConstraint, 1, 36,
    ASN1::GeneralString::theInfo.characterSet, ASN1::GeneralString::theInfo.characterSetSize, ASN1::GeneralString::theInfo.canonicalSetBits,
    ASN1::GeneralString::theInfo.charSetUnalignedBits, ASN1::GeneralString::theInfo.charSetAlignedBits
};

const ModuloEleicao::NomesCargo::nomeMasculino::value_type::InfoType ModuloEleicao::NomesCargo::nomeMasculino::value_type::theInfo = {
    ASN1::AbstractString::create,
    0x0000001b,
    &ASN1::GeneralString::theInfo,
    ASN1::FixedConstraint, 1, 36,
    ASN1::GeneralString::theInfo.characterSet, ASN1::GeneralString::theInfo.characterSetSize, ASN1::GeneralString::theInfo.canonicalSetBits,
    ASN1::GeneralString::theInfo.charSetUnalignedBits, ASN1::GeneralString::theInfo.charSetAlignedBits
};

const ModuloEleicao::NomesCargo::nomeFeminino::value_type::InfoType ModuloEleicao::NomesCargo::nomeFeminino::value_type::theInfo = {
    ASN1::AbstractString::create,
    0x0000001b,
    &ASN1::GeneralString::theInfo,
    ASN1::FixedConstraint, 1, 36,
    ASN1::GeneralString::theInfo.characterSet, ASN1::GeneralString::theInfo.characterSetSize, ASN1::GeneralString::theInfo.canonicalSetBits,
    ASN1::GeneralString::theInfo.charSetUnalignedBits, ASN1::GeneralString::theInfo.charSetAlignedBits
};

const ModuloEleicao::NomesCargo::nomeAbreviado::value_type::InfoType ModuloEleicao::NomesCargo::nomeAbreviado::value_type::theInfo = {
    ASN1::AbstractString::create,
    0x0000001b,
    &ASN1::GeneralString::theInfo,
    ASN1::FixedConstraint, 1, 10,
    ASN1::GeneralString::theInfo.characterSet, ASN1::GeneralString::theInfo.characterSetSize, ASN1::GeneralString::theInfo.canonicalSetBits,
    ASN1::GeneralString::theInfo.charSetUnalignedBits, ASN1::GeneralString::theInfo.charSetAlignedBits
};

const void* ModuloEleicao::NomesCargo::fieldInfos[4] = {
    &nomeNeutro::value_type::theInfo,
    &nomeMasculino::value_type::theInfo,
    &nomeFeminino::value_type::theInfo,
    &nomeAbreviado::value_type::theInfo
};

int ModuloEleicao::NomesCargo::fieldIds[4] = {
    -1,
    -1,
    -1,
    -1
};

const char* ModuloEleicao::NomesCargo::fieldNames[4] = {
    "nomeNeutro",
    "nomeMasculino",
    "nomeFeminino",
    "nomeAbreviado"
};

const ModuloEleicao::NomesCargo::InfoType ModuloEleicao::NomesCargo::theInfo = {
    ModuloEleicao::NomesCargo::create,
    0x00000010,
    0,
    false,
    ModuloEleicao::NomesCargo::fieldInfos,
    ModuloEleicao::NomesCargo::fieldIds,
    4, 0, 0,
    NULL,
    &ModuloEleicao::NomesCargo::defaultTag,
    ModuloEleicao::NomesCargo::fieldNames,
    "NomesCargo"
};

const ModuloEleicao::DetalhePergunta::nome::value_type::InfoType ModuloEleicao::DetalhePergunta::nome::value_type::theInfo = {
    ASN1::AbstractString::create,
    0x0000001b,
    &ASN1::GeneralString::theInfo,
    ASN1::FixedConstraint, 1, 36,
    ASN1::GeneralString::theInfo.characterSet, ASN1::GeneralString::theInfo.characterSetSize, ASN1::GeneralString::theInfo.canonicalSetBits,
    ASN1::GeneralString::theInfo.charSetUnalignedBits, ASN1::GeneralString::theInfo.charSetAlignedBits
};

const ModuloEleicao::DetalhePergunta::pergunta::value_type::InfoType ModuloEleicao::DetalhePergunta::pergunta::value_type::theInfo = {
    ASN1::AbstractString::create,
    0x0000001b,
    &ASN1::GeneralString::theInfo,
    ASN1::FixedConstraint, 1, 200,
    ASN1::GeneralString::theInfo.characterSet, ASN1::GeneralString::theInfo.characterSetSize, ASN1::GeneralString::theInfo.canonicalSetBits,
    ASN1::GeneralString::theInfo.charSetUnalignedBits, ASN1::GeneralString::theInfo.charSetAlignedBits
};

const ModuloEleicao::DetalhePergunta::respostas::value_type::InfoType ModuloEleicao::DetalhePergunta::respostas::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloEleicao::RespostaPergunta::theInfo,
    0
};

const void* ModuloEleicao::DetalhePergunta::fieldInfos[4] = {
    &nome::value_type::theInfo,
    &pergunta::value_type::theInfo,
    &textoFonetico::value_type::theInfo,
    &respostas::value_type::theInfo
};

int ModuloEleicao::DetalhePergunta::fieldIds[4] = {
    -1,
    -1,
    0,
    -1
};

unsigned ModuloEleicao::DetalhePergunta::fieldTags[4] = {
    0x0000001b,
    0x0000001b,
    0x80000001,
    0x00000010
};

const char* ModuloEleicao::DetalhePergunta::fieldNames[4] = {
    "nome",
    "pergunta",
    "textoFonetico",
    "respostas"
};

const ModuloEleicao::DetalhePergunta::InfoType ModuloEleicao::DetalhePergunta::theInfo = {
    ModuloEleicao::DetalhePergunta::create,
    0x00000010,
    0,
    false,
    ModuloEleicao::DetalhePergunta::fieldInfos,
    ModuloEleicao::DetalhePergunta::fieldIds,
    4, 0, 1,
    NULL,
    ModuloEleicao::DetalhePergunta::fieldTags,
    ModuloEleicao::DetalhePergunta::fieldNames,
    "DetalhePergunta"
};

const void* ModuloEleicao::Suplencias::fieldInfos[4] = {
    &codigo::value_type::theInfo,
    &ordem::value_type::theInfo,
    &nomes::value_type::theInfo,
    &temFoto::value_type::theInfo
};

int ModuloEleicao::Suplencias::fieldIds[4] = {
    -1,
    -1,
    -1,
    -1
};

const char* ModuloEleicao::Suplencias::fieldNames[4] = {
    "codigo",
    "ordem",
    "nomes",
    "temFoto"
};

const ModuloEleicao::Suplencias::InfoType ModuloEleicao::Suplencias::theInfo = {
    ModuloEleicao::Suplencias::create,
    0x00000010,
    0,
    false,
    ModuloEleicao::Suplencias::fieldInfos,
    ModuloEleicao::Suplencias::fieldIds,
    4, 0, 0,
    NULL,
    &ModuloEleicao::Suplencias::defaultTag,
    ModuloEleicao::Suplencias::fieldNames,
    "Suplencias"
};

const ModuloEleicao::DetalheCargo::suplencias::value_type::InfoType ModuloEleicao::DetalheCargo::suplencias::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloEleicao::Suplencias::theInfo,
    0
};

const void* ModuloEleicao::DetalheCargo::fieldInfos[4] = {
    &qtdeSuplentes::value_type::theInfo,
    &nomes::value_type::theInfo,
    &temFoto::value_type::theInfo,
    &suplencias::value_type::theInfo
};

int ModuloEleicao::DetalheCargo::fieldIds[4] = {
    -1,
    -1,
    -1,
    0
};

const char* ModuloEleicao::DetalheCargo::fieldNames[4] = {
    "qtdeSuplentes",
    "nomes",
    "temFoto",
    "suplencias"
};

const ModuloEleicao::DetalheCargo::InfoType ModuloEleicao::DetalheCargo::theInfo = {
    ModuloEleicao::DetalheCargo::create,
    0x00000010,
    0,
    false,
    ModuloEleicao::DetalheCargo::fieldInfos,
    ModuloEleicao::DetalheCargo::fieldIds,
    4, 0, 1,
    NULL,
    &ModuloEleicao::DetalheCargo::defaultTag,
    ModuloEleicao::DetalheCargo::fieldNames,
    "DetalheCargo"
};

const void* ModuloEleicao::CargoPergunta::detalhe::value_type::selectionInfos[2] = {
    &cargo::value_type::theInfo,
    &pergunta::value_type::theInfo
};

const char* ModuloEleicao::CargoPergunta::detalhe::value_type::selectionNames[2] = {
    "cargo",
    "pergunta"
};

const ModuloEleicao::CargoPergunta::detalhe::value_type::InfoType ModuloEleicao::CargoPergunta::detalhe::value_type::theInfo = {
    ModuloEleicao::CargoPergunta::detalhe::value_type::create,
    0x00000000,
    0,
    false,
    ModuloEleicao::CargoPergunta::detalhe::value_type::selectionInfos,
    2, 2,
    NULL,
    ModuloEleicao::CargoPergunta::detalhe::value_type::selectionNames
};

const void* ModuloEleicao::CargoPergunta::fieldInfos[10] = {
    &codigo::value_type::theInfo,
    &tipo::value_type::theInfo,
    &numeroDigitos::value_type::theInfo,
    &qtdeEscolhas::value_type::theInfo,
    &podeRepetir::value_type::theInfo,
    &ordemAquisicao::value_type::theInfo,
    &ordemImpressao::value_type::theInfo,
    &ordemApuracao::value_type::theInfo,
    &paginaImpressaoVoto::value_type::theInfo,
    &detalhe::value_type::theInfo
};

int ModuloEleicao::CargoPergunta::fieldIds[10] = {
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1
};

const char* ModuloEleicao::CargoPergunta::fieldNames[10] = {
    "codigo",
    "tipo",
    "numeroDigitos",
    "qtdeEscolhas",
    "podeRepetir",
    "ordemAquisicao",
    "ordemImpressao",
    "ordemApuracao",
    "paginaImpressaoVoto",
    "detalhe"
};

const ModuloEleicao::CargoPergunta::InfoType ModuloEleicao::CargoPergunta::theInfo = {
    ModuloEleicao::CargoPergunta::create,
    0x00000010,
    0,
    false,
    ModuloEleicao::CargoPergunta::fieldInfos,
    ModuloEleicao::CargoPergunta::fieldIds,
    10, 0, 0,
    NULL,
    &ModuloEleicao::CargoPergunta::defaultTag,
    ModuloEleicao::CargoPergunta::fieldNames,
    "CargoPergunta"
};

const char * ModuloEleicao::Tipo::nameList[] = {
    "ordinaria",
    "extraordinaria",
    "consultaPopular"
  };

const ModuloEleicao::Tipo::InfoType ModuloEleicao::Tipo::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    2, nameList
};

const ModuloEleicao::EntidadeEleicao::nome::value_type::InfoType ModuloEleicao::EntidadeEleicao::nome::value_type::theInfo = {
    ASN1::AbstractString::create,
    0x0000001b,
    &ASN1::GeneralString::theInfo,
    ASN1::FixedConstraint, 1, 38,
    ASN1::GeneralString::theInfo.characterSet, ASN1::GeneralString::theInfo.characterSetSize, ASN1::GeneralString::theInfo.canonicalSetBits,
    ASN1::GeneralString::theInfo.charSetUnalignedBits, ASN1::GeneralString::theInfo.charSetAlignedBits
};

const ModuloEleicao::EntidadeEleicao::cargos::value_type::InfoType ModuloEleicao::EntidadeEleicao::cargos::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloEleicao::CargoPergunta::theInfo,
    0
};

const ModuloEleicao::EntidadeEleicao::municipios::value_type::InfoType ModuloEleicao::EntidadeEleicao::municipios::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 99999>::theInfo,
    0
};

const void* ModuloEleicao::EntidadeEleicao::fieldInfos[11] = {
    &cabecalho::value_type::theInfo,
    &idPleito::value_type::theInfo,
    &idEleicaoPleito2::value_type::theInfo,
    &nome::value_type::theInfo,
    &tipo::value_type::theInfo,
    &anoReferencia::value_type::theInfo,
    &abrangencia::value_type::theInfo,
    &calendario::value_type::theInfo,
    &cargos::value_type::theInfo,
    &municipios::value_type::theInfo,
    &candidaturaDivulgavel::value_type::theInfo
};

int ModuloEleicao::EntidadeEleicao::fieldIds[11] = {
    -1,
    -1,
    0,
    -1,
    -1,
    -1,
    -1,
    1,
    -1,
    -1,
    -1
};

unsigned ModuloEleicao::EntidadeEleicao::fieldTags[11] = {
    0x00000000,
    0x00000000,
    0x80000001,
    0x0000001b,
    0x00000000,
    0x00000002,
    0x00000000,
    0x80000002,
    0x00000010,
    0x00000010,
    0x00000001
};

const char* ModuloEleicao::EntidadeEleicao::fieldNames[11] = {
    "cabecalho",
    "idPleito",
    "idEleicaoPleito2",
    "nome",
    "tipo",
    "anoReferencia",
    "abrangencia",
    "calendario",
    "cargos",
    "municipios",
    "candidaturaDivulgavel"
};

const ModuloEleicao::EntidadeEleicao::InfoType ModuloEleicao::EntidadeEleicao::theInfo = {
    ModuloEleicao::EntidadeEleicao::create,
    0x00000010,
    0,
    false,
    ModuloEleicao::EntidadeEleicao::fieldInfos,
    ModuloEleicao::EntidadeEleicao::fieldIds,
    11, 0, 2,
    NULL,
    ModuloEleicao::EntidadeEleicao::fieldTags,
    ModuloEleicao::EntidadeEleicao::fieldNames,
    "EntidadeEleicao"
};

} // namespace ModuloEleicao
