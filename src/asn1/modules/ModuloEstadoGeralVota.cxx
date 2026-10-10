#include "ModuloEstadoGeralVota.h"

namespace ModuloEstadoGeralVota{

const char * ModuloEstadoGeralVota::EstadoVota::nameList[] = {
    "inicial",
    "gerabasedinamica",
    "aguardahorazeresima",
    "gerarze",
    "zeresimagerada",
    "zeresimaimpressa",
    "registromesarioinicial",
    "votar",
    "fimaquisicaovotos",
    "registromesariofinal",
    "gerarbu",
    "gerarrelatorios",
    "imprimirbu",
    "gravarresultados",
    "copiaresultadosmr",
    "encerrada",
    "exibealertadesligamento",
    "ultimoid"
  };

const ModuloEstadoGeralVota::EstadoVota::InfoType ModuloEstadoGeralVota::EstadoVota::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    17, nameList
};

const char * ModuloEstadoGeralVota::EstadoEncerramento::nameList[] = {
    "inicial",
    "imprimirobrigatoriabu",
    "retirarmr",
    "fimdostrabalhos",
    "ultimoid"
  };

const ModuloEstadoGeralVota::EstadoEncerramento::InfoType ModuloEstadoGeralVota::EstadoEncerramento::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    4, nameList
};

const void* ModuloEstadoGeralVota::EstadoGeralVota::fieldInfos[13] = {
    &estadoVota::value_type::theInfo,
    &estadoEncerramento::value_type::theInfo,
    &qtdBU::value_type::theInfo,
    &comparecimento::value_type::theInfo,
    &qtdJustificativa::value_type::theInfo,
    &treinamentoEleitor::value_type::theInfo,
    &numViasImpressasRelatorios::value_type::theInfo,
    &tecladoTestadoPosConversao::value_type::theInfo,
    &urnaIdGerouZeresima::value_type::theInfo,
    &dhIniAquisicao::value_type::theInfo,
    &dhFimAquisicao::value_type::theInfo,
    &dhUltimoVoto::value_type::theInfo,
    &dhEmissao::value_type::theInfo
};

int ModuloEstadoGeralVota::EstadoGeralVota::fieldIds[13] = {
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    0,
    1,
    2,
    3,
    4
};

unsigned ModuloEstadoGeralVota::EstadoGeralVota::fieldTags[13] = {
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000001,
    0x00000000,
    0x00000001,
    0xa0000001,
    0xa0000002,
    0xa0000003,
    0xa0000004,
    0xa0000005
};

const char* ModuloEstadoGeralVota::EstadoGeralVota::fieldNames[13] = {
    "estadoVota",
    "estadoEncerramento",
    "qtdBU",
    "comparecimento",
    "qtdJustificativa",
    "treinamentoEleitor",
    "numViasImpressasRelatorios",
    "tecladoTestadoPosConversao",
    "urnaIdGerouZeresima",
    "dhIniAquisicao",
    "dhFimAquisicao",
    "dhUltimoVoto",
    "dhEmissao"
};

const ModuloEstadoGeralVota::EstadoGeralVota::InfoType ModuloEstadoGeralVota::EstadoGeralVota::theInfo = {
    ModuloEstadoGeralVota::EstadoGeralVota::create,
    0x00000010,
    0,
    false,
    ModuloEstadoGeralVota::EstadoGeralVota::fieldInfos,
    ModuloEstadoGeralVota::EstadoGeralVota::fieldIds,
    13, 0, 5,
    NULL,
    ModuloEstadoGeralVota::EstadoGeralVota::fieldTags,
    ModuloEstadoGeralVota::EstadoGeralVota::fieldNames,
    "EstadoGeralVota"
};

} // namespace ModuloEstadoGeralVota
