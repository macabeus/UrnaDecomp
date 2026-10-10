#include "ModuloEstadoGeralDefs.h"

namespace ModuloEstadoGeralDefs{

const void* ModuloEstadoGeralDefs::NumViasImpressasRelatorios::fieldInfos[4] = {
    &numViasEstadoUrna::value_type::theInfo,
    &numViasEleitores::value_type::theInfo,
    &numViasVersoesDados::value_type::theInfo,
    &numViasPU::value_type::theInfo
};

int ModuloEstadoGeralDefs::NumViasImpressasRelatorios::fieldIds[4] = {
    -1,
    -1,
    -1,
    -1
};

const char* ModuloEstadoGeralDefs::NumViasImpressasRelatorios::fieldNames[4] = {
    "numViasEstadoUrna",
    "numViasEleitores",
    "numViasVersoesDados",
    "numViasPU"
};

const ModuloEstadoGeralDefs::NumViasImpressasRelatorios::InfoType ModuloEstadoGeralDefs::NumViasImpressasRelatorios::theInfo = {
    ModuloEstadoGeralDefs::NumViasImpressasRelatorios::create,
    0x00000010,
    0,
    false,
    ModuloEstadoGeralDefs::NumViasImpressasRelatorios::fieldInfos,
    ModuloEstadoGeralDefs::NumViasImpressasRelatorios::fieldIds,
    4, 0, 0,
    NULL,
    &ModuloEstadoGeralDefs::NumViasImpressasRelatorios::defaultTag,
    ModuloEstadoGeralDefs::NumViasImpressasRelatorios::fieldNames,
    "NumViasImpressasRelatorios"
};

} // namespace ModuloEstadoGeralDefs
