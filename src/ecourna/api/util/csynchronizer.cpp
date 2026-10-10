#include "ecourna/api/util/csynchronizer.hpp"

#include "ecourna/api/util/eutilerror.hpp"

namespace ecourna::api::util {

CSynchronizer* CSynchronizer::s_instancia = nullptr;

// (original lines 9-55: content not recovered)
#line 56
void CSynchronizer::CreateInst(bool habilitado)
{
    if (s_instancia != nullptr) {
        throw CUtilError(EUE_SYNCHRONIZER_INSTANCIA_JA_CRIADA, "Instância já criada");
    }
    s_instancia = new CSynchronizer(habilitado);
}

} // namespace ecourna::api::util
