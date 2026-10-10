#pragma once
#include <memory>
#include <vector>

#include <openssl/evp.h>

#include "ecourna/api/security/irng.hpp"
#include "ecourna/api/security/isymmetriccipher.hpp"

namespace ecourna::api::security {

class CAesCipher {
public:
    CAesCipher(const EOperationModes modo, const ESymmetricCipherKeySize tamanho, std::shared_ptr<IRng> rng);

    void SetKey(const CAesKey& chave);
    void Encrypt(const std::vector<uebyte>& entrada, std::vector<uebyte>& saida);
    void Decrypt(const std::vector<uebyte>& entrada, std::vector<uebyte>& saida);

private:
    const EVP_CIPHER* m_cipher;
    EOperationModes m_modo;
    std::shared_ptr<IRng> m_rng;
    CAesKey m_chave;
    int m_tamanhoBloco;
};

} // namespace ecourna::api::security
