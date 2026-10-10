#include "api/util/cdirreader.h"

#include "api/util/eueutilerror.h"

namespace api {

// (original lines 7-26: content not recovered)
#line 27
// <stubs>
// f1914 · 127 B · unmatched · name unknown
// ? f1914(i32, i32) { ... }
// </stubs>

// (original lines 32-39: content not recovered)
#line 40
bool CDirReader::NextEntry()
{
    if (m_dir == nullptr) {
        throw CUeUtilError(EUE_UTIL_DIRETORIO_NAO_ABERTO, "O diretório não está aberto");
    }

    m_entrada = readdir(m_dir);
    return m_entrada != nullptr;
}

} // namespace api
