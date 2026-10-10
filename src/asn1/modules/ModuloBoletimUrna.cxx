#include "ModuloBoletimUrna.h"

namespace ModuloBoletimUrna{

const void* ModuloBoletimUrna::DetalhamentoComparecimento::fieldInfos[3] = {
    &qtdEleitoresCompareceramSemBiometria::value_type::theInfo,
    &qtdEleitoresHabilitadosPorBiometria::value_type::theInfo,
    &qtdEleitoresHabilitadosPorBiografia::value_type::theInfo
};

int ModuloBoletimUrna::DetalhamentoComparecimento::fieldIds[3] = {
    -1,
    -1,
    -1
};

const char* ModuloBoletimUrna::DetalhamentoComparecimento::fieldNames[3] = {
    "qtdEleitoresCompareceramSemBiometria",
    "qtdEleitoresHabilitadosPorBiometria",
    "qtdEleitoresHabilitadosPorBiografia"
};

const ModuloBoletimUrna::DetalhamentoComparecimento::InfoType ModuloBoletimUrna::DetalhamentoComparecimento::theInfo = {
    ModuloBoletimUrna::DetalhamentoComparecimento::create,
    0x00000010,
    0,
    false,
    ModuloBoletimUrna::DetalhamentoComparecimento::fieldInfos,
    ModuloBoletimUrna::DetalhamentoComparecimento::fieldIds,
    3, 0, 0,
    NULL,
    &ModuloBoletimUrna::DetalhamentoComparecimento::defaultTag,
    ModuloBoletimUrna::DetalhamentoComparecimento::fieldNames,
    "DetalhamentoComparecimento"
};

const void* ModuloBoletimUrna::IdentificacaoVotavel::fieldInfos[2] = {
    &partido::value_type::theInfo,
    &codigo::value_type::theInfo
};

int ModuloBoletimUrna::IdentificacaoVotavel::fieldIds[2] = {
    -1,
    -1
};

const char* ModuloBoletimUrna::IdentificacaoVotavel::fieldNames[2] = {
    "partido",
    "codigo"
};

const ModuloBoletimUrna::IdentificacaoVotavel::InfoType ModuloBoletimUrna::IdentificacaoVotavel::theInfo = {
    ModuloBoletimUrna::IdentificacaoVotavel::create,
    0x00000010,
    0,
    false,
    ModuloBoletimUrna::IdentificacaoVotavel::fieldInfos,
    ModuloBoletimUrna::IdentificacaoVotavel::fieldIds,
    2, 0, 0,
    NULL,
    &ModuloBoletimUrna::IdentificacaoVotavel::defaultTag,
    ModuloBoletimUrna::IdentificacaoVotavel::fieldNames,
    "IdentificacaoVotavel"
};

const char * ModuloBoletimUrna::TipoVoto::nameList[] = {
    "<undefined>=0",
    "nominal",
    "branco",
    "nulo",
    "legenda",
    "cargoSemCandidato"
  };

const ModuloBoletimUrna::TipoVoto::InfoType ModuloBoletimUrna::TipoVoto::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    5, nameList
};

const void* ModuloBoletimUrna::DadosSecao::fieldInfos[3] = {
    &dataHoraAbertura::value_type::theInfo,
    &dataHoraEncerramento::value_type::theInfo,
    &dataHoraDesligamentoVotoImpresso::value_type::theInfo
};

int ModuloBoletimUrna::DadosSecao::fieldIds[3] = {
    -1,
    -1,
    0
};

const char* ModuloBoletimUrna::DadosSecao::fieldNames[3] = {
    "dataHoraAbertura",
    "dataHoraEncerramento",
    "dataHoraDesligamentoVotoImpresso"
};

const ModuloBoletimUrna::DadosSecao::InfoType ModuloBoletimUrna::DadosSecao::theInfo = {
    ModuloBoletimUrna::DadosSecao::create,
    0x00000010,
    0,
    false,
    ModuloBoletimUrna::DadosSecao::fieldInfos,
    ModuloBoletimUrna::DadosSecao::fieldIds,
    3, 0, 1,
    NULL,
    &ModuloBoletimUrna::DadosSecao::defaultTag,
    ModuloBoletimUrna::DadosSecao::fieldNames,
    "DadosSecao"
};

const void* ModuloBoletimUrna::DadosSA::fieldInfos[3] = {
    &juntaApuradora::value_type::theInfo,
    &turmaApuradora::value_type::theInfo,
    &numeroInternoUrnaOrigem::value_type::theInfo
};

int ModuloBoletimUrna::DadosSA::fieldIds[3] = {
    -1,
    -1,
    0
};

const char* ModuloBoletimUrna::DadosSA::fieldNames[3] = {
    "juntaApuradora",
    "turmaApuradora",
    "numeroInternoUrnaOrigem"
};

const ModuloBoletimUrna::DadosSA::InfoType ModuloBoletimUrna::DadosSA::theInfo = {
    ModuloBoletimUrna::DadosSA::create,
    0x00000010,
    0,
    false,
    ModuloBoletimUrna::DadosSA::fieldInfos,
    ModuloBoletimUrna::DadosSA::fieldIds,
    3, 0, 1,
    NULL,
    &ModuloBoletimUrna::DadosSA::defaultTag,
    ModuloBoletimUrna::DadosSA::fieldNames,
    "DadosSA"
};

const void* ModuloBoletimUrna::HistoricoVotoImpresso::fieldInfos[3] = {
    &idImpressoraVotos::value_type::theInfo,
    &idRepositorioVotos::value_type::theInfo,
    &dataHoraLigamento::value_type::theInfo
};

int ModuloBoletimUrna::HistoricoVotoImpresso::fieldIds[3] = {
    -1,
    -1,
    -1
};

const char* ModuloBoletimUrna::HistoricoVotoImpresso::fieldNames[3] = {
    "idImpressoraVotos",
    "idRepositorioVotos",
    "dataHoraLigamento"
};

const ModuloBoletimUrna::HistoricoVotoImpresso::InfoType ModuloBoletimUrna::HistoricoVotoImpresso::theInfo = {
    ModuloBoletimUrna::HistoricoVotoImpresso::create,
    0x00000010,
    0,
    false,
    ModuloBoletimUrna::HistoricoVotoImpresso::fieldInfos,
    ModuloBoletimUrna::HistoricoVotoImpresso::fieldIds,
    3, 0, 0,
    NULL,
    &ModuloBoletimUrna::HistoricoVotoImpresso::defaultTag,
    ModuloBoletimUrna::HistoricoVotoImpresso::fieldNames,
    "HistoricoVotoImpresso"
};

const void* ModuloBoletimUrna::TotalVotosVotavel::fieldInfos[5] = {
    &tipoVoto::value_type::theInfo,
    &quantidadeVotos::value_type::theInfo,
    &identificacaoVotavel::value_type::theInfo,
    &ordemGeracaoHash::value_type::theInfo,
    &hash::value_type::theInfo
};

int ModuloBoletimUrna::TotalVotosVotavel::fieldIds[5] = {
    -1,
    -1,
    0,
    -1,
    -1
};

unsigned ModuloBoletimUrna::TotalVotosVotavel::fieldTags[5] = {
    0x80000001,
    0x80000002,
    0x80000003,
    0x00000000,
    0x00000004
};

const char* ModuloBoletimUrna::TotalVotosVotavel::fieldNames[5] = {
    "tipoVoto",
    "quantidadeVotos",
    "identificacaoVotavel",
    "ordemGeracaoHash",
    "hash"
};

const ModuloBoletimUrna::TotalVotosVotavel::InfoType ModuloBoletimUrna::TotalVotosVotavel::theInfo = {
    ModuloBoletimUrna::TotalVotosVotavel::create,
    0x00000010,
    0,
    false,
    ModuloBoletimUrna::TotalVotosVotavel::fieldInfos,
    ModuloBoletimUrna::TotalVotosVotavel::fieldIds,
    5, 0, 1,
    NULL,
    ModuloBoletimUrna::TotalVotosVotavel::fieldTags,
    ModuloBoletimUrna::TotalVotosVotavel::fieldNames,
    "TotalVotosVotavel"
};

const ModuloBoletimUrna::TotalVotosCargo::votosVotaveis::value_type::InfoType ModuloBoletimUrna::TotalVotosCargo::votosVotaveis::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloBoletimUrna::TotalVotosVotavel::theInfo,
    0
};

const void* ModuloBoletimUrna::TotalVotosCargo::fieldInfos[3] = {
    &codigoCargo::value_type::theInfo,
    &ordemImpressao::value_type::theInfo,
    &votosVotaveis::value_type::theInfo
};

int ModuloBoletimUrna::TotalVotosCargo::fieldIds[3] = {
    -1,
    -1,
    -1
};

const char* ModuloBoletimUrna::TotalVotosCargo::fieldNames[3] = {
    "codigoCargo",
    "ordemImpressao",
    "votosVotaveis"
};

const ModuloBoletimUrna::TotalVotosCargo::InfoType ModuloBoletimUrna::TotalVotosCargo::theInfo = {
    ModuloBoletimUrna::TotalVotosCargo::create,
    0x00000010,
    0,
    false,
    ModuloBoletimUrna::TotalVotosCargo::fieldInfos,
    ModuloBoletimUrna::TotalVotosCargo::fieldIds,
    3, 0, 0,
    NULL,
    &ModuloBoletimUrna::TotalVotosCargo::defaultTag,
    ModuloBoletimUrna::TotalVotosCargo::fieldNames,
    "TotalVotosCargo"
};

const ModuloBoletimUrna::ResultadoVotacao::totaisVotosCargo::value_type::InfoType ModuloBoletimUrna::ResultadoVotacao::totaisVotosCargo::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloBoletimUrna::TotalVotosCargo::theInfo,
    0
};

const void* ModuloBoletimUrna::ResultadoVotacao::fieldInfos[3] = {
    &tipoCargo::value_type::theInfo,
    &qtdComparecimento::value_type::theInfo,
    &totaisVotosCargo::value_type::theInfo
};

int ModuloBoletimUrna::ResultadoVotacao::fieldIds[3] = {
    -1,
    -1,
    -1
};

const char* ModuloBoletimUrna::ResultadoVotacao::fieldNames[3] = {
    "tipoCargo",
    "qtdComparecimento",
    "totaisVotosCargo"
};

const ModuloBoletimUrna::ResultadoVotacao::InfoType ModuloBoletimUrna::ResultadoVotacao::theInfo = {
    ModuloBoletimUrna::ResultadoVotacao::create,
    0x00000010,
    0,
    false,
    ModuloBoletimUrna::ResultadoVotacao::fieldInfos,
    ModuloBoletimUrna::ResultadoVotacao::fieldIds,
    3, 0, 0,
    NULL,
    &ModuloBoletimUrna::ResultadoVotacao::defaultTag,
    ModuloBoletimUrna::ResultadoVotacao::fieldNames,
    "ResultadoVotacao"
};

const ModuloBoletimUrna::ResultadoVotacaoPorEleicao::resultadosVotacao::value_type::InfoType ModuloBoletimUrna::ResultadoVotacaoPorEleicao::resultadosVotacao::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloBoletimUrna::ResultadoVotacao::theInfo,
    0
};

const void* ModuloBoletimUrna::ResultadoVotacaoPorEleicao::fieldInfos[7] = {
    &idEleicao::value_type::theInfo,
    &qtdEleitoresAptos::value_type::theInfo,
    &qtdEleitoresAptosSecao::value_type::theInfo,
    &qtdEleitoresAptosTTE::value_type::theInfo,
    &resultadosVotacao::value_type::theInfo,
    &ultimoHashVotosVotavel::value_type::theInfo,
    &assinaturaUltimoHashVotosVotavel::value_type::theInfo
};

int ModuloBoletimUrna::ResultadoVotacaoPorEleicao::fieldIds[7] = {
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1
};

const char* ModuloBoletimUrna::ResultadoVotacaoPorEleicao::fieldNames[7] = {
    "idEleicao",
    "qtdEleitoresAptos",
    "qtdEleitoresAptosSecao",
    "qtdEleitoresAptosTTE",
    "resultadosVotacao",
    "ultimoHashVotosVotavel",
    "assinaturaUltimoHashVotosVotavel"
};

const ModuloBoletimUrna::ResultadoVotacaoPorEleicao::InfoType ModuloBoletimUrna::ResultadoVotacaoPorEleicao::theInfo = {
    ModuloBoletimUrna::ResultadoVotacaoPorEleicao::create,
    0x00000010,
    0,
    false,
    ModuloBoletimUrna::ResultadoVotacaoPorEleicao::fieldInfos,
    ModuloBoletimUrna::ResultadoVotacaoPorEleicao::fieldIds,
    7, 0, 0,
    NULL,
    &ModuloBoletimUrna::ResultadoVotacaoPorEleicao::defaultTag,
    ModuloBoletimUrna::ResultadoVotacaoPorEleicao::fieldNames,
    "ResultadoVotacaoPorEleicao"
};

const void* ModuloBoletimUrna::EntidadeBoletimUrna::dadosSecaoSA::value_type::selectionInfos[2] = {
    &dadosSecao::value_type::theInfo,
    &dadosSA::value_type::theInfo
};

const char* ModuloBoletimUrna::EntidadeBoletimUrna::dadosSecaoSA::value_type::selectionNames[2] = {
    "dadosSecao",
    "dadosSA"
};

const ModuloBoletimUrna::EntidadeBoletimUrna::dadosSecaoSA::value_type::InfoType ModuloBoletimUrna::EntidadeBoletimUrna::dadosSecaoSA::value_type::theInfo = {
    ModuloBoletimUrna::EntidadeBoletimUrna::dadosSecaoSA::value_type::create,
    0x00000000,
    0,
    false,
    ModuloBoletimUrna::EntidadeBoletimUrna::dadosSecaoSA::value_type::selectionInfos,
    2, 2,
    NULL,
    ModuloBoletimUrna::EntidadeBoletimUrna::dadosSecaoSA::value_type::selectionNames
};

const ModuloBoletimUrna::EntidadeBoletimUrna::resultadosVotacaoPorEleicao::value_type::InfoType ModuloBoletimUrna::EntidadeBoletimUrna::resultadosVotacaoPorEleicao::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloBoletimUrna::ResultadoVotacaoPorEleicao::theInfo,
    0
};

const ModuloBoletimUrna::EntidadeBoletimUrna::historicoVotoImpresso::value_type::InfoType ModuloBoletimUrna::EntidadeBoletimUrna::historicoVotoImpresso::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloBoletimUrna::HistoricoVotoImpresso::theInfo,
    0
};

const void* ModuloBoletimUrna::EntidadeBoletimUrna::fieldInfos[11] = {
    &cabecalho::value_type::theInfo,
    &fase::value_type::theInfo,
    &urna::value_type::theInfo,
    &identificacaoSecao::value_type::theInfo,
    &dataHoraEmissao::value_type::theInfo,
    &dadosSecaoSA::value_type::theInfo,
    &qtdEleitoresCompareceram::value_type::theInfo,
    &detalhamentoComparecimento::value_type::theInfo,
    &resultadosVotacaoPorEleicao::value_type::theInfo,
    &historicoCodigosCarga::value_type::theInfo,
    &historicoVotoImpresso::value_type::theInfo
};

int ModuloBoletimUrna::EntidadeBoletimUrna::fieldIds[11] = {
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    0,
    -1,
    -1,
    1
};

unsigned ModuloBoletimUrna::EntidadeBoletimUrna::fieldTags[11] = {
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x80000001,
    0x00000010,
    0x00000010,
    0x00000010
};

const char* ModuloBoletimUrna::EntidadeBoletimUrna::fieldNames[11] = {
    "cabecalho",
    "fase",
    "urna",
    "identificacaoSecao",
    "dataHoraEmissao",
    "dadosSecaoSA",
    "qtdEleitoresCompareceram",
    "detalhamentoComparecimento",
    "resultadosVotacaoPorEleicao",
    "historicoCodigosCarga",
    "historicoVotoImpresso"
};

const ModuloBoletimUrna::EntidadeBoletimUrna::InfoType ModuloBoletimUrna::EntidadeBoletimUrna::theInfo = {
    ModuloBoletimUrna::EntidadeBoletimUrna::create,
    0x00000010,
    0,
    false,
    ModuloBoletimUrna::EntidadeBoletimUrna::fieldInfos,
    ModuloBoletimUrna::EntidadeBoletimUrna::fieldIds,
    11, 0, 2,
    NULL,
    ModuloBoletimUrna::EntidadeBoletimUrna::fieldTags,
    ModuloBoletimUrna::EntidadeBoletimUrna::fieldNames,
    "EntidadeBoletimUrna"
};

} // namespace ModuloBoletimUrna
