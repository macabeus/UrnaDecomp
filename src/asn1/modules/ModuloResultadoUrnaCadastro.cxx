#include "ModuloResultadoUrnaCadastro.h"

namespace ModuloResultadoUrnaCadastro{

const void* ModuloResultadoUrnaCadastro::DadosCifracao::fieldInfos[3] = {
    &chave::value_type::theInfo,
    &salt::value_type::theInfo,
    &informacaoAdicional::value_type::theInfo
};

int ModuloResultadoUrnaCadastro::DadosCifracao::fieldIds[3] = {
    -1,
    -1,
    -1
};

const char* ModuloResultadoUrnaCadastro::DadosCifracao::fieldNames[3] = {
    "chave",
    "salt",
    "informacaoAdicional"
};

const ModuloResultadoUrnaCadastro::DadosCifracao::InfoType ModuloResultadoUrnaCadastro::DadosCifracao::theInfo = {
    ModuloResultadoUrnaCadastro::DadosCifracao::create,
    0x00000010,
    0,
    false,
    ModuloResultadoUrnaCadastro::DadosCifracao::fieldInfos,
    ModuloResultadoUrnaCadastro::DadosCifracao::fieldIds,
    3, 0, 0,
    NULL,
    &ModuloResultadoUrnaCadastro::DadosCifracao::defaultTag,
    ModuloResultadoUrnaCadastro::DadosCifracao::fieldNames,
    "DadosCifracao"
};

const void* ModuloResultadoUrnaCadastro::DadosComparecimentoCifrado::fieldInfos[2] = {
    &dadosCifracao::value_type::theInfo,
    &conteudo::value_type::theInfo
};

int ModuloResultadoUrnaCadastro::DadosComparecimentoCifrado::fieldIds[2] = {
    -1,
    -1
};

const char* ModuloResultadoUrnaCadastro::DadosComparecimentoCifrado::fieldNames[2] = {
    "dadosCifracao",
    "conteudo"
};

const ModuloResultadoUrnaCadastro::DadosComparecimentoCifrado::InfoType ModuloResultadoUrnaCadastro::DadosComparecimentoCifrado::theInfo = {
    ModuloResultadoUrnaCadastro::DadosComparecimentoCifrado::create,
    0x00000010,
    0,
    false,
    ModuloResultadoUrnaCadastro::DadosComparecimentoCifrado::fieldInfos,
    ModuloResultadoUrnaCadastro::DadosComparecimentoCifrado::fieldIds,
    2, 0, 0,
    NULL,
    &ModuloResultadoUrnaCadastro::DadosComparecimentoCifrado::defaultTag,
    ModuloResultadoUrnaCadastro::DadosComparecimentoCifrado::fieldNames,
    "DadosComparecimentoCifrado"
};

const void* ModuloResultadoUrnaCadastro::IdentificacaoJustificativa::fieldInfos[2] = {
    &identificacaoEleitor::value_type::theInfo,
    &anoNascimentoEleitor::value_type::theInfo
};

int ModuloResultadoUrnaCadastro::IdentificacaoJustificativa::fieldIds[2] = {
    -1,
    -1
};

const char* ModuloResultadoUrnaCadastro::IdentificacaoJustificativa::fieldNames[2] = {
    "identificacaoEleitor",
    "anoNascimentoEleitor"
};

const ModuloResultadoUrnaCadastro::IdentificacaoJustificativa::InfoType ModuloResultadoUrnaCadastro::IdentificacaoJustificativa::theInfo = {
    ModuloResultadoUrnaCadastro::IdentificacaoJustificativa::create,
    0x00000010,
    0,
    false,
    ModuloResultadoUrnaCadastro::IdentificacaoJustificativa::fieldInfos,
    ModuloResultadoUrnaCadastro::IdentificacaoJustificativa::fieldIds,
    2, 0, 0,
    NULL,
    &ModuloResultadoUrnaCadastro::IdentificacaoJustificativa::defaultTag,
    ModuloResultadoUrnaCadastro::IdentificacaoJustificativa::fieldNames,
    "IdentificacaoJustificativa"
};

const char * ModuloResultadoUrnaCadastro::SituacaoComparecimentoEleitor::nameList[] = {
    "<undefined>=0",
    "faltou",
    "naoVotou",
    "votou",
    "semCargoParaVotar"
  };

const ModuloResultadoUrnaCadastro::SituacaoComparecimentoEleitor::InfoType ModuloResultadoUrnaCadastro::SituacaoComparecimentoEleitor::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    4, nameList
};

const char * ModuloResultadoUrnaCadastro::EstadoApresentacaoFotoEleitor::nameList[] = {
    "<undefined>=0",
    "eleitorSemFoto",
    "apresentada",
    "naoApresentadaPorErro"
  };

const ModuloResultadoUrnaCadastro::EstadoApresentacaoFotoEleitor::InfoType ModuloResultadoUrnaCadastro::EstadoApresentacaoFotoEleitor::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    3, nameList
};

const char * ModuloResultadoUrnaCadastro::ResultadoApresentacaoFotoEleitor::nameList[] = {
    "semErro",
    "erroFormato",
    "erroFormacaoImagem",
    "erroResolucao",
    "erroCompressao",
    "erroProfundidadeCores",
    "erroApresentacao",
    "erroDecifrandoBuffer",
    "erroDesconhecidoDeCriptografia"
  };

const ModuloResultadoUrnaCadastro::ResultadoApresentacaoFotoEleitor::InfoType ModuloResultadoUrnaCadastro::ResultadoApresentacaoFotoEleitor::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    8, nameList
};

const char * ModuloResultadoUrnaCadastro::SituacaoHabilitacaoAudio::nameList[] = {
    "<undefined>=0",
    "automatica",
    "peloMesario",
    "naoHabilitado"
  };

const ModuloResultadoUrnaCadastro::SituacaoHabilitacaoAudio::InfoType ModuloResultadoUrnaCadastro::SituacaoHabilitacaoAudio::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    3, nameList
};

const char * ModuloResultadoUrnaCadastro::ErroLeituraBiometria::nameList[] = {
    "semErro",
    "tipoArquivoInvalido",
    "idCriptografiaInvalido",
    "chaveSecretaVazia",
    "chaveSecaoVazia",
    "conteudoVazio",
    "chaveSecaoComTamanhoInvalido",
    "chaveSecretaComTamanhoInvalido",
    "naoFoiPossivelIniciarADecifragem",
    "erroDecifrandoBuffer",
    "erroDesconhecidoDeCriptografia",
    "erroFormacaoTemplate",
    "erroCodificacaoMinuncias"
  };

const ModuloResultadoUrnaCadastro::ErroLeituraBiometria::InfoType ModuloResultadoUrnaCadastro::ErroLeituraBiometria::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    12, nameList
};

const char * ModuloResultadoUrnaCadastro::EstadoColetaDigital::nameList[] = {
    "<undefined>=0",
    "naoColetadaReconhecidaNoCadastro",
    "coletadaMesarioNaoPossuiDigitalNoCadastro",
    "coletadaDigitalNaoReconhecidaNoCadastro",
    "coletadaMesarioNaoPertenceASecao"
  };

const ModuloResultadoUrnaCadastro::EstadoColetaDigital::InfoType ModuloResultadoUrnaCadastro::EstadoColetaDigital::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    4, nameList
};

const char * ModuloResultadoUrnaCadastro::SituacaoReconhecimentoMesario::nameList[] = {
    "<undefined>=0",
    "naoReconhecido",
    "reconhecidoDigitalCadastroUrna",
    "reconhecidoDigitalRegistroMesarios"
  };

const ModuloResultadoUrnaCadastro::SituacaoReconhecimentoMesario::InfoType ModuloResultadoUrnaCadastro::SituacaoReconhecimentoMesario::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    3, nameList
};

const void* ModuloResultadoUrnaCadastro::ApresentacaoFotoEleitor::fieldInfos[2] = {
    &estadoApresentacao::value_type::theInfo,
    &resultadoApresentacao::value_type::theInfo
};

int ModuloResultadoUrnaCadastro::ApresentacaoFotoEleitor::fieldIds[2] = {
    -1,
    -1
};

const char* ModuloResultadoUrnaCadastro::ApresentacaoFotoEleitor::fieldNames[2] = {
    "estadoApresentacao",
    "resultadoApresentacao"
};

const ModuloResultadoUrnaCadastro::ApresentacaoFotoEleitor::InfoType ModuloResultadoUrnaCadastro::ApresentacaoFotoEleitor::theInfo = {
    ModuloResultadoUrnaCadastro::ApresentacaoFotoEleitor::create,
    0x00000010,
    0,
    false,
    ModuloResultadoUrnaCadastro::ApresentacaoFotoEleitor::fieldInfos,
    ModuloResultadoUrnaCadastro::ApresentacaoFotoEleitor::fieldIds,
    2, 0, 0,
    NULL,
    &ModuloResultadoUrnaCadastro::ApresentacaoFotoEleitor::defaultTag,
    ModuloResultadoUrnaCadastro::ApresentacaoFotoEleitor::fieldNames,
    "ApresentacaoFotoEleitor"
};

const void* ModuloResultadoUrnaCadastro::ComparecimentoMesario::fieldInfos[3] = {
    &identificacaoMesario::value_type::theInfo,
    &dataHoraColeta::value_type::theInfo,
    &estadoColetaDigital::value_type::theInfo
};

int ModuloResultadoUrnaCadastro::ComparecimentoMesario::fieldIds[3] = {
    -1,
    -1,
    0
};

const char* ModuloResultadoUrnaCadastro::ComparecimentoMesario::fieldNames[3] = {
    "identificacaoMesario",
    "dataHoraColeta",
    "estadoColetaDigital"
};

const ModuloResultadoUrnaCadastro::ComparecimentoMesario::InfoType ModuloResultadoUrnaCadastro::ComparecimentoMesario::theInfo = {
    ModuloResultadoUrnaCadastro::ComparecimentoMesario::create,
    0x00000010,
    0,
    false,
    ModuloResultadoUrnaCadastro::ComparecimentoMesario::fieldInfos,
    ModuloResultadoUrnaCadastro::ComparecimentoMesario::fieldIds,
    3, 0, 1,
    NULL,
    &ModuloResultadoUrnaCadastro::ComparecimentoMesario::defaultTag,
    ModuloResultadoUrnaCadastro::ComparecimentoMesario::fieldNames,
    "ComparecimentoMesario"
};

const void* ModuloResultadoUrnaCadastro::EstadoHabilitacaoPorCodigo::fieldInfos[2] = {
    &situacaoReconhecimentoMesario::value_type::theInfo,
    &identificacaoMesario::value_type::theInfo
};

int ModuloResultadoUrnaCadastro::EstadoHabilitacaoPorCodigo::fieldIds[2] = {
    -1,
    0
};

const char* ModuloResultadoUrnaCadastro::EstadoHabilitacaoPorCodigo::fieldNames[2] = {
    "situacaoReconhecimentoMesario",
    "identificacaoMesario"
};

const ModuloResultadoUrnaCadastro::EstadoHabilitacaoPorCodigo::InfoType ModuloResultadoUrnaCadastro::EstadoHabilitacaoPorCodigo::theInfo = {
    ModuloResultadoUrnaCadastro::EstadoHabilitacaoPorCodigo::create,
    0x00000010,
    0,
    false,
    ModuloResultadoUrnaCadastro::EstadoHabilitacaoPorCodigo::fieldInfos,
    ModuloResultadoUrnaCadastro::EstadoHabilitacaoPorCodigo::fieldIds,
    2, 0, 1,
    NULL,
    &ModuloResultadoUrnaCadastro::EstadoHabilitacaoPorCodigo::defaultTag,
    ModuloResultadoUrnaCadastro::EstadoHabilitacaoPorCodigo::fieldNames,
    "EstadoHabilitacaoPorCodigo"
};

const void* ModuloResultadoUrnaCadastro::HabilitacaoBiometrica::fieldInfos[5] = {
    &tentativasHabilitacaoBiometrica::value_type::theInfo,
    &dedoHabilitado::value_type::theInfo,
    &erroLeituraBiometria::value_type::theInfo,
    &habilitacaoPorCodigo::value_type::theInfo,
    &ultimoScore::value_type::theInfo
};

int ModuloResultadoUrnaCadastro::HabilitacaoBiometrica::fieldIds[5] = {
    -1,
    -1,
    -1,
    0,
    1
};

unsigned ModuloResultadoUrnaCadastro::HabilitacaoBiometrica::fieldTags[5] = {
    0x00000002,
    0x00000000,
    0x00000000,
    0x80000001,
    0x80000002
};

const char* ModuloResultadoUrnaCadastro::HabilitacaoBiometrica::fieldNames[5] = {
    "tentativasHabilitacaoBiometrica",
    "dedoHabilitado",
    "erroLeituraBiometria",
    "habilitacaoPorCodigo",
    "ultimoScore"
};

const ModuloResultadoUrnaCadastro::HabilitacaoBiometrica::InfoType ModuloResultadoUrnaCadastro::HabilitacaoBiometrica::theInfo = {
    ModuloResultadoUrnaCadastro::HabilitacaoBiometrica::create,
    0x00000010,
    0,
    false,
    ModuloResultadoUrnaCadastro::HabilitacaoBiometrica::fieldInfos,
    ModuloResultadoUrnaCadastro::HabilitacaoBiometrica::fieldIds,
    5, 0, 2,
    NULL,
    ModuloResultadoUrnaCadastro::HabilitacaoBiometrica::fieldTags,
    ModuloResultadoUrnaCadastro::HabilitacaoBiometrica::fieldNames,
    "HabilitacaoBiometrica"
};

const void* ModuloResultadoUrnaCadastro::EstadoComparecimento::fieldInfos[5] = {
    &identificacaoEleitor::value_type::theInfo,
    &situacaoComparecimentoEleitor::value_type::theInfo,
    &apresentacaoFoto::value_type::theInfo,
    &situacaoHabilitacaoAudio::value_type::theInfo,
    &habilitacaoBiometrica::value_type::theInfo
};

int ModuloResultadoUrnaCadastro::EstadoComparecimento::fieldIds[5] = {
    -1,
    -1,
    0,
    1,
    2
};

unsigned ModuloResultadoUrnaCadastro::EstadoComparecimento::fieldTags[5] = {
    0x00000000,
    0x00000000,
    0x80000001,
    0x80000002,
    0x80000003
};

const char* ModuloResultadoUrnaCadastro::EstadoComparecimento::fieldNames[5] = {
    "identificacaoEleitor",
    "situacaoComparecimentoEleitor",
    "apresentacaoFoto",
    "situacaoHabilitacaoAudio",
    "habilitacaoBiometrica"
};

const ModuloResultadoUrnaCadastro::EstadoComparecimento::InfoType ModuloResultadoUrnaCadastro::EstadoComparecimento::theInfo = {
    ModuloResultadoUrnaCadastro::EstadoComparecimento::create,
    0x00000010,
    0,
    false,
    ModuloResultadoUrnaCadastro::EstadoComparecimento::fieldInfos,
    ModuloResultadoUrnaCadastro::EstadoComparecimento::fieldIds,
    5, 0, 3,
    NULL,
    ModuloResultadoUrnaCadastro::EstadoComparecimento::fieldTags,
    ModuloResultadoUrnaCadastro::EstadoComparecimento::fieldNames,
    "EstadoComparecimento"
};

const ModuloResultadoUrnaCadastro::ComparecimentoSecao::eleitores::value_type::InfoType ModuloResultadoUrnaCadastro::ComparecimentoSecao::eleitores::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloResultadoUrnaCadastro::EstadoComparecimento::theInfo,
    0
};

const void* ModuloResultadoUrnaCadastro::ComparecimentoSecao::fieldInfos[2] = {
    &identificacao::value_type::theInfo,
    &eleitores::value_type::theInfo
};

int ModuloResultadoUrnaCadastro::ComparecimentoSecao::fieldIds[2] = {
    -1,
    -1
};

const char* ModuloResultadoUrnaCadastro::ComparecimentoSecao::fieldNames[2] = {
    "identificacao",
    "eleitores"
};

const ModuloResultadoUrnaCadastro::ComparecimentoSecao::InfoType ModuloResultadoUrnaCadastro::ComparecimentoSecao::theInfo = {
    ModuloResultadoUrnaCadastro::ComparecimentoSecao::create,
    0x00000010,
    0,
    false,
    ModuloResultadoUrnaCadastro::ComparecimentoSecao::fieldInfos,
    ModuloResultadoUrnaCadastro::ComparecimentoSecao::fieldIds,
    2, 0, 0,
    NULL,
    &ModuloResultadoUrnaCadastro::ComparecimentoSecao::defaultTag,
    ModuloResultadoUrnaCadastro::ComparecimentoSecao::fieldNames,
    "ComparecimentoSecao"
};

const ModuloResultadoUrnaCadastro::DadosComparecimento::justificativas::value_type::InfoType ModuloResultadoUrnaCadastro::DadosComparecimento::justificativas::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloResultadoUrnaCadastro::IdentificacaoJustificativa::theInfo,
    0
};

const ModuloResultadoUrnaCadastro::DadosComparecimento::mesariosAbertura::value_type::InfoType ModuloResultadoUrnaCadastro::DadosComparecimento::mesariosAbertura::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x80000001,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloResultadoUrnaCadastro::ComparecimentoMesario::theInfo,
    "value_type"
};

const ModuloResultadoUrnaCadastro::DadosComparecimento::mesariosEncerramento::value_type::InfoType ModuloResultadoUrnaCadastro::DadosComparecimento::mesariosEncerramento::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x80000002,
    0,
    ASN1::Unconstrained, 0, UINT_MAX,
    &ModuloResultadoUrnaCadastro::ComparecimentoMesario::theInfo,
    "value_type"
};

const void* ModuloResultadoUrnaCadastro::DadosComparecimento::fieldInfos[4] = {
    &justificativas::value_type::theInfo,
    &identificacaoComparecimento::value_type::theInfo,
    &mesariosAbertura::value_type::theInfo,
    &mesariosEncerramento::value_type::theInfo
};

int ModuloResultadoUrnaCadastro::DadosComparecimento::fieldIds[4] = {
    -1,
    -1,
    0,
    1
};

unsigned ModuloResultadoUrnaCadastro::DadosComparecimento::fieldTags[4] = {
    0x00000010,
    0x00000000,
    0x80000001,
    0x80000002
};

const char* ModuloResultadoUrnaCadastro::DadosComparecimento::fieldNames[4] = {
    "justificativas",
    "identificacaoComparecimento",
    "mesariosAbertura",
    "mesariosEncerramento"
};

const ModuloResultadoUrnaCadastro::DadosComparecimento::InfoType ModuloResultadoUrnaCadastro::DadosComparecimento::theInfo = {
    ModuloResultadoUrnaCadastro::DadosComparecimento::create,
    0x00000010,
    0,
    false,
    ModuloResultadoUrnaCadastro::DadosComparecimento::fieldInfos,
    ModuloResultadoUrnaCadastro::DadosComparecimento::fieldIds,
    4, 0, 2,
    NULL,
    ModuloResultadoUrnaCadastro::DadosComparecimento::fieldTags,
    ModuloResultadoUrnaCadastro::DadosComparecimento::fieldNames,
    "DadosComparecimento"
};

const void* ModuloResultadoUrnaCadastro::EntidadeResultadoUrnaCadastro::infoDadosComparecimento::value_type::selectionInfos[2] = {
    &dadosComparecimento::value_type::theInfo,
    &dadosComparecimentoCifrado::value_type::theInfo
};

const char* ModuloResultadoUrnaCadastro::EntidadeResultadoUrnaCadastro::infoDadosComparecimento::value_type::selectionNames[2] = {
    "dadosComparecimento",
    "dadosComparecimentoCifrado"
};

const ModuloResultadoUrnaCadastro::EntidadeResultadoUrnaCadastro::infoDadosComparecimento::value_type::InfoType ModuloResultadoUrnaCadastro::EntidadeResultadoUrnaCadastro::infoDadosComparecimento::value_type::theInfo = {
    ModuloResultadoUrnaCadastro::EntidadeResultadoUrnaCadastro::infoDadosComparecimento::value_type::create,
    0x00000000,
    0,
    false,
    ModuloResultadoUrnaCadastro::EntidadeResultadoUrnaCadastro::infoDadosComparecimento::value_type::selectionInfos,
    2, 2,
    NULL,
    ModuloResultadoUrnaCadastro::EntidadeResultadoUrnaCadastro::infoDadosComparecimento::value_type::selectionNames
};

const void* ModuloResultadoUrnaCadastro::EntidadeResultadoUrnaCadastro::fieldInfos[5] = {
    &cabecalho::value_type::theInfo,
    &fase::value_type::theInfo,
    &versaoVotacao::value_type::theInfo,
    &situacao::value_type::theInfo,
    &infoDadosComparecimento::value_type::theInfo
};

int ModuloResultadoUrnaCadastro::EntidadeResultadoUrnaCadastro::fieldIds[5] = {
    -1,
    -1,
    -1,
    -1,
    -1
};

const char* ModuloResultadoUrnaCadastro::EntidadeResultadoUrnaCadastro::fieldNames[5] = {
    "cabecalho",
    "fase",
    "versaoVotacao",
    "situacao",
    "infoDadosComparecimento"
};

const ModuloResultadoUrnaCadastro::EntidadeResultadoUrnaCadastro::InfoType ModuloResultadoUrnaCadastro::EntidadeResultadoUrnaCadastro::theInfo = {
    ModuloResultadoUrnaCadastro::EntidadeResultadoUrnaCadastro::create,
    0x00000010,
    0,
    false,
    ModuloResultadoUrnaCadastro::EntidadeResultadoUrnaCadastro::fieldInfos,
    ModuloResultadoUrnaCadastro::EntidadeResultadoUrnaCadastro::fieldIds,
    5, 0, 0,
    NULL,
    &ModuloResultadoUrnaCadastro::EntidadeResultadoUrnaCadastro::defaultTag,
    ModuloResultadoUrnaCadastro::EntidadeResultadoUrnaCadastro::fieldNames,
    "EntidadeResultadoUrnaCadastro"
};

} // namespace ModuloResultadoUrnaCadastro
