#include "ModuloEstadoGeralGap.h"

namespace ModuloEstadoGeralGap{

const char * ModuloEstadoGeralGap::UrnaAplicativo::nameList[] = {
    "apvotatreinaeleitor1",
    "apvotatreinaeleitor2",
    "apvotatreinamesario1",
    "apvotatreinamesario2",
    "apvotasimula1",
    "apvotasimula2",
    "apvotaoficial1",
    "apvotaoficial2",
    "apapuracaotreina1",
    "apapuracaotreina2",
    "apapuracaosimula1",
    "apapuracaosimula2",
    "apapuracaooficial1",
    "apapuracaooficial2",
    "apsemaplicativo",
    "apultimoid"
  };

const ModuloEstadoGeralGap::UrnaAplicativo::InfoType ModuloEstadoGeralGap::UrnaAplicativo::theInfo = {
    ASN1::ENUMERATED::create,
    0x0000000a,
    0,
    0,
    15, nameList
};

const ModuloEstadoGeralGap::EstadoGeralGap::correspondencias::value_type::InfoType ModuloEstadoGeralGap::EstadoGeralGap::correspondencias::value_type::theInfo = {
    ASN1::SEQUENCE_OF_Base::create,
    0x00000010,
    0,
    ASN1::FixedConstraint, 0, 10,
    &ModuloEstadoGeralUrna::DadoCorrespondencia::theInfo,
    0
};

const void* ModuloEstadoGeralGap::EstadoGeralGap::fieldInfos[10] = {
    &correspondencias::value_type::theInfo,
    &appId::value_type::theInfo,
    &appAnteriorId::value_type::theInfo,
    &executadoRED::value_type::theInfo,
    &identificadoATUE::value_type::theInfo,
    &audio::value_type::theInfo,
    &data2T::value_type::theInfo,
    &numViasImpressasRelatorios1T::value_type::theInfo,
    &numViasImpressasRelatorios2T::value_type::theInfo,
    &dataSegundoTurno::value_type::theInfo
};

int ModuloEstadoGeralGap::EstadoGeralGap::fieldIds[10] = {
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    0
};

const char* ModuloEstadoGeralGap::EstadoGeralGap::fieldNames[10] = {
    "correspondencias",
    "appId",
    "appAnteriorId",
    "executadoRED",
    "identificadoATUE",
    "audio",
    "data2T",
    "numViasImpressasRelatorios1T",
    "numViasImpressasRelatorios2T",
    "dataSegundoTurno"
};

const ModuloEstadoGeralGap::EstadoGeralGap::InfoType ModuloEstadoGeralGap::EstadoGeralGap::theInfo = {
    ModuloEstadoGeralGap::EstadoGeralGap::create,
    0x00000010,
    0,
    false,
    ModuloEstadoGeralGap::EstadoGeralGap::fieldInfos,
    ModuloEstadoGeralGap::EstadoGeralGap::fieldIds,
    10, 0, 1,
    NULL,
    &ModuloEstadoGeralGap::EstadoGeralGap::defaultTag,
    ModuloEstadoGeralGap::EstadoGeralGap::fieldNames,
    "EstadoGeralGap"
};

} // namespace ModuloEstadoGeralGap
