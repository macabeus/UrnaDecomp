#include "ecourna/api/security/cepesc/ccipheredout.hpp"

#include "ecourna/api/security/esecurityerror.hpp"

namespace ecourna::api::cepesc {

using security::CSecurityError;

// (original lines 9-21: content not recovered)
#line 22
CCipheredOut::CCipheredOut(const std::vector<uebyte>& chave, const std::vector<uebyte>& conteudo)
    : m_chave(chave), m_conteudo(conteudo), m_infoSalt()
{
    if (chave.empty()) {
        throw CSecurityError(security::ESE_CIPHERED_OUT_CHAVE_VAZIA, "chave vazia.");
    }
    if (conteudo.empty()) {
        throw CSecurityError(security::ESE_CIPHERED_OUT_CONTEUDO_VAZIO, "conteúdo vazio.");
    }
}

// (original lines 33-39: content not recovered)
#line 40
CCipheredOut::CCipheredOut(const std::vector<uebyte>& chave, const std::vector<uebyte>& conteudo, const CInfoSalt& infoSalt)
    : m_chave(chave), m_conteudo(conteudo), m_infoSalt(new CInfoSalt(infoSalt))
{
    if (chave.empty()) {
        throw CSecurityError(security::ESE_CIPHERED_OUT_SALT_CHAVE_VAZIA, "chave vazia.");
    }
    if (conteudo.empty()) {
        throw CSecurityError(security::ESE_CIPHERED_OUT_SALT_CONTEUDO_VAZIO, "conteúdo vazio.");
    }
}

} // namespace ecourna::api::cepesc
