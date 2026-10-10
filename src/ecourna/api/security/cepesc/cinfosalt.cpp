#include "ecourna/api/security/cepesc/cinfosalt.hpp"

#include "ecourna/api/security/esecurityerror.hpp"

namespace ecourna::api::cepesc {

using security::CSecurityError;

// (original lines 9-23: content not recovered)
#line 24
CInfoSalt::CInfoSalt(const std::vector<uebyte>& salt, const std::vector<uebyte>& info) : m_salt(salt), m_info(info)
{
    if (salt.empty()) {
        throw CSecurityError(security::ESE_SALT_VAZIO, "salt vazio.");
    }

    if (salt.size() < 16) {
        throw CSecurityError(security::ESE_SALT_TAMANHO_MINIMO, "tamanho minimo do salt deve ser 16 bytes.");
    }

    if (info.empty()) {
        throw CSecurityError(security::ESE_INFO_VAZIO, "conteúdo de info vazio.");
    }
}

CInfoSalt::CInfoSalt(const CInfoSalt& outro) : m_salt(outro.m_salt), m_info(outro.m_info)
{
}

} // namespace ecourna::api::cepesc
