#include "ModuloParametrizacaoUrna.h"

namespace ModuloParametrizacaoUrna{

const char * ModuloParametrizacaoUrna::TituloRelatorio::alinhamento::value_type::nameList[] = {
    "<undefined>=0",
    "esquerdo",
    "direito",
    "centro"
  };

const ModuloParametrizacaoUrna::TituloRelatorio::alinhamento::value_type::InfoType ModuloParametrizacaoUrna::TituloRelatorio::alinhamento::value_type::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    3, nameList
};

const char * ModuloParametrizacaoUrna::TituloRelatorio::estilo::value_type::nameList[] = {
    "<undefined>=0",
    "normal",
    "expandido"
  };

const ModuloParametrizacaoUrna::TituloRelatorio::estilo::value_type::InfoType ModuloParametrizacaoUrna::TituloRelatorio::estilo::value_type::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    2, nameList
};

const ModuloParametrizacaoUrna::TituloRelatorio::texto::value_type::InfoType ModuloParametrizacaoUrna::TituloRelatorio::texto::value_type::theInfo = {
    ASN1::AbstractString::create,
    0x0000001b,
    &ASN1::GeneralString::theInfo,
    ASN1::Unconstrained, 0, 4294967295,
    ASN1::GeneralString::theInfo.characterSet, ASN1::GeneralString::theInfo.characterSetSize, ASN1::GeneralString::theInfo.canonicalSetBits,
    ASN1::GeneralString::theInfo.charSetUnalignedBits, ASN1::GeneralString::theInfo.charSetAlignedBits
};

const void* ModuloParametrizacaoUrna::TituloRelatorio::fieldInfos[3] = {
    &alinhamento::value_type::theInfo,
    &estilo::value_type::theInfo,
    &texto::value_type::theInfo
};

int ModuloParametrizacaoUrna::TituloRelatorio::fieldIds[3] = {
    -1,
    -1,
    -1
};

const char* ModuloParametrizacaoUrna::TituloRelatorio::fieldNames[3] = {
    "alinhamento",
    "estilo",
    "texto"
};

const ModuloParametrizacaoUrna::TituloRelatorio::InfoType ModuloParametrizacaoUrna::TituloRelatorio::theInfo = {
    ModuloParametrizacaoUrna::TituloRelatorio::create,
    0x00000010,
    0,
    false,
    ModuloParametrizacaoUrna::TituloRelatorio::fieldInfos,
    ModuloParametrizacaoUrna::TituloRelatorio::fieldIds,
    3, 0, 0,
    NULL,
    &ModuloParametrizacaoUrna::TituloRelatorio::defaultTag,
    ModuloParametrizacaoUrna::TituloRelatorio::fieldNames,
    "TituloRelatorio"
};

const char * ModuloParametrizacaoUrna::FormaSuspenderSemVoto::nameList[] = {
    "<undefined>=0",
    "tornaNaoVotou",
    "tornaTodosBranco",
    "tornaTodosNulo"
  };

const ModuloParametrizacaoUrna::FormaSuspenderSemVoto::InfoType ModuloParametrizacaoUrna::FormaSuspenderSemVoto::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    3, nameList
};

const char * ModuloParametrizacaoUrna::FormaSuspenderComVoto::nameList[] = {
    "<undefined>=0",
    "tornaOutrosBranco",
    "tornaOutrosNulo",
    "tornaNaoVotouParcial"
  };

const ModuloParametrizacaoUrna::FormaSuspenderComVoto::InfoType ModuloParametrizacaoUrna::FormaSuspenderComVoto::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    3, nameList
};

const char * ModuloParametrizacaoUrna::LabelParametrizado::genero::value_type::nameList[] = {
    "<undefined>=0",
    "neutro",
    "masculino",
    "feminino"
  };

const ModuloParametrizacaoUrna::LabelParametrizado::genero::value_type::InfoType ModuloParametrizacaoUrna::LabelParametrizado::genero::value_type::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    3, nameList
};

const ModuloParametrizacaoUrna::LabelParametrizado::longo::value_type::InfoType ModuloParametrizacaoUrna::LabelParametrizado::longo::value_type::theInfo = {
    ASN1::AbstractString::create,
    0x0000001b,
    &ASN1::GeneralString::theInfo,
    ASN1::FixedConstraint, 1, 20,
    ASN1::GeneralString::theInfo.characterSet, ASN1::GeneralString::theInfo.characterSetSize, ASN1::GeneralString::theInfo.canonicalSetBits,
    ASN1::GeneralString::theInfo.charSetUnalignedBits, ASN1::GeneralString::theInfo.charSetAlignedBits
};

const ModuloParametrizacaoUrna::LabelParametrizado::curto::value_type::InfoType ModuloParametrizacaoUrna::LabelParametrizado::curto::value_type::theInfo = {
    ASN1::AbstractString::create,
    0x0000001b,
    &ASN1::GeneralString::theInfo,
    ASN1::FixedConstraint, 1, 11,
    ASN1::GeneralString::theInfo.characterSet, ASN1::GeneralString::theInfo.characterSetSize, ASN1::GeneralString::theInfo.canonicalSetBits,
    ASN1::GeneralString::theInfo.charSetUnalignedBits, ASN1::GeneralString::theInfo.charSetAlignedBits
};

const ModuloParametrizacaoUrna::LabelParametrizado::longoPlural::value_type::InfoType ModuloParametrizacaoUrna::LabelParametrizado::longoPlural::value_type::theInfo = {
    ASN1::AbstractString::create,
    0x0000001b,
    &ASN1::GeneralString::theInfo,
    ASN1::FixedConstraint, 1, 24,
    ASN1::GeneralString::theInfo.characterSet, ASN1::GeneralString::theInfo.characterSetSize, ASN1::GeneralString::theInfo.canonicalSetBits,
    ASN1::GeneralString::theInfo.charSetUnalignedBits, ASN1::GeneralString::theInfo.charSetAlignedBits
};

const ModuloParametrizacaoUrna::LabelParametrizado::curtoPlural::value_type::InfoType ModuloParametrizacaoUrna::LabelParametrizado::curtoPlural::value_type::theInfo = {
    ASN1::AbstractString::create,
    0x0000001b,
    &ASN1::GeneralString::theInfo,
    ASN1::FixedConstraint, 1, 13,
    ASN1::GeneralString::theInfo.characterSet, ASN1::GeneralString::theInfo.characterSetSize, ASN1::GeneralString::theInfo.canonicalSetBits,
    ASN1::GeneralString::theInfo.charSetUnalignedBits, ASN1::GeneralString::theInfo.charSetAlignedBits
};

const void* ModuloParametrizacaoUrna::LabelParametrizado::fieldInfos[5] = {
    &genero::value_type::theInfo,
    &longo::value_type::theInfo,
    &curto::value_type::theInfo,
    &longoPlural::value_type::theInfo,
    &curtoPlural::value_type::theInfo
};

int ModuloParametrizacaoUrna::LabelParametrizado::fieldIds[5] = {
    -1,
    -1,
    -1,
    -1,
    -1
};

const char* ModuloParametrizacaoUrna::LabelParametrizado::fieldNames[5] = {
    "genero",
    "longo",
    "curto",
    "longoPlural",
    "curtoPlural"
};

const ModuloParametrizacaoUrna::LabelParametrizado::InfoType ModuloParametrizacaoUrna::LabelParametrizado::theInfo = {
    ModuloParametrizacaoUrna::LabelParametrizado::create,
    0x00000010,
    0,
    false,
    ModuloParametrizacaoUrna::LabelParametrizado::fieldInfos,
    ModuloParametrizacaoUrna::LabelParametrizado::fieldIds,
    5, 0, 0,
    NULL,
    &ModuloParametrizacaoUrna::LabelParametrizado::defaultTag,
    ModuloParametrizacaoUrna::LabelParametrizado::fieldNames,
    "LabelParametrizado"
};

const ModuloParametrizacaoUrna::ParametrosUrna::telaEmissaoBU::value_type::InfoType ModuloParametrizacaoUrna::ParametrosUrna::telaEmissaoBU::value_type::theInfo = {
    ASN1::AbstractString::create,
    0x0000001b,
    &ASN1::GeneralString::theInfo,
    ASN1::FixedConstraint, 1, 512,
    ASN1::GeneralString::theInfo.characterSet, ASN1::GeneralString::theInfo.characterSetSize, ASN1::GeneralString::theInfo.canonicalSetBits,
    ASN1::GeneralString::theInfo.charSetUnalignedBits, ASN1::GeneralString::theInfo.charSetAlignedBits
};

const void* ModuloParametrizacaoUrna::ParametrosUrna::fieldInfos[47] = {
    &formaSuspensaoSemVoto::value_type::theInfo,
    &formaSuspensaoComVoto::value_type::theInfo,
    &numZeresimas::value_type::theInfo,
    &numBUVotaObrigatorios::value_type::theInfo,
    &numBUVotaAdicionais::value_type::theInfo,
    &numBUFinalREDObrigat::value_type::theInfo,
    &numBUFinalREDAdic::value_type::theInfo,
    &numBUParcialREDObrigat::value_type::theInfo,
    &numBUParcialREDAdic::value_type::theInfo,
    &numBUFinalSAObrigat::value_type::theInfo,
    &numBUFinalSAAdic::value_type::theInfo,
    &numBUParcialSAObrigat::value_type::theInfo,
    &numTentativasHabilitacao::value_type::theInfo,
    &numTentativasVerificacao::value_type::theInfo,
    &numRelatorioEstado::value_type::theInfo,
    &numRelatorioEleitores::value_type::theInfo,
    &numRelatorioVersoesDados::value_type::theInfo,
    &numRelatorioPU::value_type::theInfo,
    &criptografarBU::value_type::theInfo,
    &criptografarJUFA::value_type::theInfo,
    &numDigitosPartido::value_type::theInfo,
    &tempoDispararSuspensao::value_type::theInfo,
    &tempoDispararSuspensaoTE::value_type::theInfo,
    &tempoConfirmacaoVoto::value_type::theInfo,
    &tempoDesligamentoAutomatico::value_type::theInfo,
    &tempoAvisoDesligamentoAutomatico::value_type::theInfo,
    &cabecalho::value_type::theInfo,
    &rodapeZEVOTA::value_type::theInfo,
    &rodapeBUVOTA::value_type::theInfo,
    &rodapeZESA::value_type::theInfo,
    &rodapeBUSA::value_type::theInfo,
    &rodapeBUParcial::value_type::theInfo,
    &telaEmissaoBU::value_type::theInfo,
    &labelMunicipio::value_type::theInfo,
    &labelZona::value_type::theInfo,
    &labelSecao::value_type::theInfo,
    &labelPartido::value_type::theInfo,
    &imprimirZeradosBU::value_type::theInfo,
    &gravarZeradosBU::value_type::theInfo,
    &aceitarJustificativa::value_type::theInfo,
    &aceitarBrancoNulo::value_type::theInfo,
    &apresentarPartido::value_type::theInfo,
    &permitirHabManualAudio::value_type::theInfo,
    &imprimirQrCodeNoBU::value_type::theInfo,
    &exibirScoreBiometria::value_type::theInfo,
    &registrarMesarios::value_type::theInfo,
    &pedeAnoNascimentoEleitor::value_type::theInfo
};

int ModuloParametrizacaoUrna::ParametrosUrna::fieldIds[47] = {
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
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

const char* ModuloParametrizacaoUrna::ParametrosUrna::fieldNames[47] = {
    "formaSuspensaoSemVoto",
    "formaSuspensaoComVoto",
    "numZeresimas",
    "numBUVotaObrigatorios",
    "numBUVotaAdicionais",
    "numBUFinalREDObrigat",
    "numBUFinalREDAdic",
    "numBUParcialREDObrigat",
    "numBUParcialREDAdic",
    "numBUFinalSAObrigat",
    "numBUFinalSAAdic",
    "numBUParcialSAObrigat",
    "numTentativasHabilitacao",
    "numTentativasVerificacao",
    "numRelatorioEstado",
    "numRelatorioEleitores",
    "numRelatorioVersoesDados",
    "numRelatorioPU",
    "criptografarBU",
    "criptografarJUFA",
    "numDigitosPartido",
    "tempoDispararSuspensao",
    "tempoDispararSuspensaoTE",
    "tempoConfirmacaoVoto",
    "tempoDesligamentoAutomatico",
    "tempoAvisoDesligamentoAutomatico",
    "cabecalho",
    "rodapeZEVOTA",
    "rodapeBUVOTA",
    "rodapeZESA",
    "rodapeBUSA",
    "rodapeBUParcial",
    "telaEmissaoBU",
    "labelMunicipio",
    "labelZona",
    "labelSecao",
    "labelPartido",
    "imprimirZeradosBU",
    "gravarZeradosBU",
    "aceitarJustificativa",
    "aceitarBrancoNulo",
    "apresentarPartido",
    "permitirHabManualAudio",
    "imprimirQrCodeNoBU",
    "exibirScoreBiometria",
    "registrarMesarios",
    "pedeAnoNascimentoEleitor"
};

const ModuloParametrizacaoUrna::ParametrosUrna::InfoType ModuloParametrizacaoUrna::ParametrosUrna::theInfo = {
    ModuloParametrizacaoUrna::ParametrosUrna::create,
    0x00000010,
    0,
    false,
    ModuloParametrizacaoUrna::ParametrosUrna::fieldInfos,
    ModuloParametrizacaoUrna::ParametrosUrna::fieldIds,
    47, 0, 0,
    NULL,
    &ModuloParametrizacaoUrna::ParametrosUrna::defaultTag,
    ModuloParametrizacaoUrna::ParametrosUrna::fieldNames,
    "ParametrosUrna"
};

const void* ModuloParametrizacaoUrna::EntidadeParametrizacaoUrna::fieldInfos[2] = {
    &cabecalho::value_type::theInfo,
    &parametros::value_type::theInfo
};

int ModuloParametrizacaoUrna::EntidadeParametrizacaoUrna::fieldIds[2] = {
    -1,
    -1
};

const char* ModuloParametrizacaoUrna::EntidadeParametrizacaoUrna::fieldNames[2] = {
    "cabecalho",
    "parametros"
};

const ModuloParametrizacaoUrna::EntidadeParametrizacaoUrna::InfoType ModuloParametrizacaoUrna::EntidadeParametrizacaoUrna::theInfo = {
    ModuloParametrizacaoUrna::EntidadeParametrizacaoUrna::create,
    0x00000010,
    0,
    false,
    ModuloParametrizacaoUrna::EntidadeParametrizacaoUrna::fieldInfos,
    ModuloParametrizacaoUrna::EntidadeParametrizacaoUrna::fieldIds,
    2, 0, 0,
    NULL,
    &ModuloParametrizacaoUrna::EntidadeParametrizacaoUrna::defaultTag,
    ModuloParametrizacaoUrna::EntidadeParametrizacaoUrna::fieldNames,
    "EntidadeParametrizacaoUrna"
};

} // namespace ModuloParametrizacaoUrna
