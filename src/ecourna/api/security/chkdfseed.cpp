#include "ecourna/api/security/chkdfseed.hpp"

#include <openssl/evp.h>
#include <openssl/kdf.h>

#include "ecourna/api/security/esecurityerror.hpp"

namespace ecourna::api::security {

namespace {

const size_t TAMANHO_SEMENTE = 128;
const char* const MENSAGEM_FALHA = "Falha ao gerar as sementes criptográficas HKDF.";

} // namespace

std::vector<uebyte> CHKDFSeed::GetSeed()
{
    std::vector<uebyte> semente(TAMANHO_SEMENTE);

    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_HKDF, nullptr);

    // (original lines 23-45: content not recovered)
#line 46
    if (ctx == nullptr) {
        throw CSecurityError(ESE_HKDF_FALHA_CONTEXTO, MENSAGEM_FALHA);
    }
    if (EVP_PKEY_derive_init(ctx) <= 0) {
        throw CSecurityError(ESE_HKDF_FALHA_INICIAR, MENSAGEM_FALHA);
    }
    if (EVP_PKEY_CTX_set_hkdf_md(ctx, EVP_sha512()) <= 0) {
        throw CSecurityError(ESE_HKDF_FALHA_MD, MENSAGEM_FALHA);
    }
    if (EVP_PKEY_CTX_set1_hkdf_salt(ctx, m_salt.data(), m_salt.size()) <= 0) {
        throw CSecurityError(ESE_HKDF_FALHA_SALT, MENSAGEM_FALHA);
    }
    if (EVP_PKEY_CTX_set1_hkdf_key(ctx, m_chave.data(), m_chave.size()) <= 0) {
        throw CSecurityError(ESE_HKDF_FALHA_CHAVE, MENSAGEM_FALHA);
    }
    if (EVP_PKEY_CTX_add1_hkdf_info(ctx, m_info.data(), m_info.size()) <= 0) {
        throw CSecurityError(ESE_HKDF_FALHA_INFO, MENSAGEM_FALHA);
    }

    size_t tamanho = semente.size();
    if (EVP_PKEY_derive(ctx, semente.data(), &tamanho) <= 0) {
        throw CSecurityError(ESE_HKDF_FALHA_DERIVAR, MENSAGEM_FALHA);
    }

    EVP_PKEY_CTX_free(ctx);

    return semente;
}

} // namespace ecourna::api::security
