#include "ModuloVersaoArquivos.h"

namespace ModuloVersaoArquivos{

const void* ModuloVersaoArquivos::EntidadeVersaoArquivos::fieldInfos[2] = {
    &versaoTag::value_type::theInfo,
    &arquivos::value_type::theInfo
};

int ModuloVersaoArquivos::EntidadeVersaoArquivos::fieldIds[2] = {
    -1,
    -1
};

const char* ModuloVersaoArquivos::EntidadeVersaoArquivos::fieldNames[2] = {
    "versaoTag",
    "arquivos"
};

const ModuloVersaoArquivos::EntidadeVersaoArquivos::InfoType ModuloVersaoArquivos::EntidadeVersaoArquivos::theInfo = {
    ModuloVersaoArquivos::EntidadeVersaoArquivos::create,
    0x00000010,
    0,
    false,
    ModuloVersaoArquivos::EntidadeVersaoArquivos::fieldInfos,
    ModuloVersaoArquivos::EntidadeVersaoArquivos::fieldIds,
    2, 0, 0,
    NULL,
    &ModuloVersaoArquivos::EntidadeVersaoArquivos::defaultTag,
    ModuloVersaoArquivos::EntidadeVersaoArquivos::fieldNames,
    "EntidadeVersaoArquivos"
};

} // namespace ModuloVersaoArquivos
