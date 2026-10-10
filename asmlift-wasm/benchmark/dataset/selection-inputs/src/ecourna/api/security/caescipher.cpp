#include "ecourna/api/security/caescipher.hpp"

#include <map>
#include <utility>

#include <openssl/err.h>

#include "ecourna/api/security/esecurityerror.hpp"

namespace ecourna::api::security {

namespace {

const int TAMANHO_IV = 16;

using TCipherFn = const EVP_CIPHER* (*)();

const std::map<std::pair<EOperationModes, ESymmetricCipherKeySize>, TCipherFn> CIFRAS = {
    {{EOM_CBC, ESCKS_128}, EVP_aes_128_cbc},
    {{EOM_CBC, ESCKS_192}, EVP_aes_192_cbc},
    {{EOM_CBC, ESCKS_256}, EVP_aes_256_cbc},
    {{EOM_ECB, ESCKS_128}, EVP_aes_128_ecb},
    {{EOM_ECB, ESCKS_192}, EVP_aes_192_ecb},
    {{EOM_ECB, ESCKS_256}, EVP_aes_256_ecb},
    {{EOM_CTR, ESCKS_128}, EVP_aes_128_ctr},
    {{EOM_CTR, ESCKS_192}, EVP_aes_192_ctr},
    {{EOM_CTR, ESCKS_256}, EVP_aes_256_ctr},
};

using TContexto = std::unique_ptr<EVP_CIPHER_CTX, decltype(&EVP_CIPHER_CTX_free)>;

} // namespace

CAesCipher::CAesCipher(const EOperationModes modo, const ESymmetricCipherKeySize tamanho, std::shared_ptr<IRng> rng)
    : m_cipher(nullptr)
    , m_modo(modo)
    , m_rng(rng)
{

    const auto cifra = CIFRAS.find(std::make_pair(modo, tamanho));












    if (cifra == CIFRAS.end()) {
        throw CSecurityError(ESE_AES_CHAVE_TAMANHO, "Chave AES de tamanho inválido.");
    }

    m_cipher = cifra->second();
    m_tamanhoBloco = EVP_CIPHER_get_block_size(m_cipher);
}

void CAesCipher::SetKey(const CAesKey& chave)
{






    if (EVP_CIPHER_get_key_length(m_cipher) != static_cast<int>(chave.GetBytes().size())) {
        throw CSecurityError(ESE_AES_CHAVE_TAMANHO_SETKEY, "Chave AES de tamanho inválido.");
    }

    m_chave = chave;
}
void CAesCipher::Encrypt(const std::vector<uebyte>& entrada, std::vector<uebyte>& saida)
{
    TContexto ctx(EVP_CIPHER_CTX_new(), EVP_CIPHER_CTX_free);
    if (ctx == nullptr) {
        throw CSecurityError(ESE_AES_CONTEXTO_ENCRYPT, "Erro ao alocar contexto de cifra OpenSSL.");
    }

    int tamanho = 0;
    int tamanhoFinal = 0;
    std::unique_ptr<uebyte[]> buffer(new uebyte[m_tamanhoBloco + entrada.size()]());
    std::vector<uebyte> iv(TAMANHO_IV);
    if (m_chave.GetIV().empty()) {
        m_rng->Generate(iv, TAMANHO_IV);
    }
    else {
        iv.assign(m_chave.GetIV().begin(), m_chave.GetIV().end());
    }



    if (!EVP_EncryptInit_ex(ctx.get(), m_cipher, nullptr, m_chave.GetBytes().data(), iv.data())) {
        throw CSecurityError(ESE_AES_ENCRYPT_INIT, "EncryptInit_ex falhou.");
    }
    if (!EVP_EncryptUpdate(ctx.get(), buffer.get(), &tamanho, entrada.data(), entrada.size())) {
        if (const unsigned long erro = ERR_get_error(); erro != 0) {
            throw CSecurityError(ESE_AES_ENCRYPT_UPDATE, ERR_error_string(erro, nullptr));
        }
    }
    if (!EVP_EncryptFinal_ex(ctx.get(), buffer.get() + tamanho, &tamanhoFinal)) {
        if (const unsigned long erro = ERR_get_error(); erro != 0) {
            throw CSecurityError(ESE_AES_ENCRYPT_FINAL, ERR_error_string(erro, nullptr));
        }
    }

    saida.assign(buffer.get(), buffer.get() + tamanho + tamanhoFinal);
    if (m_chave.GetIV().empty()) {
        saida.insert(saida.end(), iv.begin(), iv.end());
    }
}

void CAesCipher::Decrypt(const std::vector<uebyte>& entrada, std::vector<uebyte>& saida)
{
    TContexto ctx(EVP_CIPHER_CTX_new(), EVP_CIPHER_CTX_free);
    if (ctx == nullptr) {
        throw CSecurityError(ESE_AES_CONTEXTO_DECRYPT, "Erro ao alocar contexto de cifra OpenSSL.");
    }

    int tamanho = 0;
    int tamanhoFinal = 0;
    std::unique_ptr<uebyte[]> buffer(new uebyte[m_tamanhoBloco + entrada.size()]());
    std::vector<uebyte> iv(TAMANHO_IV);
    std::vector<uebyte> dados(entrada.size());
    if (m_chave.GetIV().empty()) {
        iv.assign(entrada.end() - TAMANHO_IV, entrada.end());
        dados.assign(entrada.begin(), entrada.end() - TAMANHO_IV);
    }
    else {
        iv.assign(m_chave.GetIV().begin(), m_chave.GetIV().end());
        dados.assign(entrada.begin(), entrada.end());
    }





    if (!EVP_DecryptInit_ex(ctx.get(), m_cipher, nullptr, m_chave.GetBytes().data(), iv.data())) {
        throw CSecurityError(ESE_AES_DECRYPT_INIT, "DecryptInit_ex falhou.");
    }
    if (!EVP_DecryptUpdate(ctx.get(), buffer.get(), &tamanho, dados.data(), dados.size())) {
        if (const unsigned long erro = ERR_get_error(); erro != 0) {
            throw CSecurityError(ESE_AES_DECRYPT_UPDATE, ERR_error_string(erro, nullptr));
        }
    }
    if (!EVP_DecryptFinal_ex(ctx.get(), buffer.get() + tamanho, &tamanhoFinal)) {
        if (const unsigned long erro = ERR_get_error(); erro != 0) {
            throw CSecurityError(ESE_AES_DECRYPT_FINAL, ERR_error_string(erro, nullptr));
        }
    }

    saida.assign(buffer.get(), buffer.get() + tamanho + tamanhoFinal);
}

} // namespace ecourna::api::security
