#pragma once
#include <memory>
#include <vector>

#include "ecourna/api/security/esecurityerror.hpp"
#include "ecourna/api/security/isymmetriccipher.hpp"

namespace ecourna::api::security {

template <typename ALGO, typename RANDOM>
class CBlockCipher : public ISymmetricCipher {
public:
    explicit CBlockCipher(const CAesKey& chave)
        : ISymmetricCipher(chave)
        , m_modo(EOM_CBC)
    {
    }

    virtual ~CBlockCipher() = default;

    // (original lines 21-51: content not recovered)
#line 52
    virtual void Encrypt(const std::vector<uebyte>& entrada, std::vector<uebyte>& saida) const
    {
        ALGO algoritmo(m_modo, GetChave().GetTamanho(), std::make_shared<RANDOM>());

        if (entrada.empty()) {
            throw CSecurityError(ESE_BLOCK_CIPHER_SEM_DADOS_CIFRAR, "Sem dados para cifrar.");
        }

        algoritmo.SetKey(GetChave());
        algoritmo.Encrypt(entrada, saida);
    }

    // (original lines 64-65: content not recovered)
#line 66
    virtual void Decrypt(const std::vector<uebyte>& entrada, std::vector<uebyte>& saida) const
    {
        ALGO algoritmo(m_modo, GetChave().GetTamanho(), std::make_shared<RANDOM>());

        if (entrada.empty()) {
            throw CSecurityError(ESE_BLOCK_CIPHER_SEM_DADOS_DECIFRAR, "Sem dados para decifrar.");
        }

        algoritmo.SetKey(GetChave());
        algoritmo.Decrypt(entrada, saida);
    }

private:
    EOperationModes m_modo;
};

} // namespace ecourna::api::security
