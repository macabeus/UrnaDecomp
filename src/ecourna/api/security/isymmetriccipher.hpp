#pragma once
#include <vector>

#include "ecourna/api/pattern/noncopyable.hpp"
#include "ecourna/api/uetypes.hpp"

namespace ecourna::api::security {

enum EOperationModes {
    EOM_ECB = 0,
    EOM_CBC = 1,
    EOM_CFB = 2,
    EOM_OFB = 3,
    EOM_CTR = 4,
};

enum ESymmetricCipherKeySize {
    ESCKS_128 = 128,
    ESCKS_192 = 192,
    ESCKS_256 = 256,
};

class CAesKey {
public:
    CAesKey() = default;
    CAesKey(const std::vector<uebyte>& bytes, ESymmetricCipherKeySize tamanho, const std::vector<uebyte>& iv = std::vector<uebyte>())
        : m_bytes(bytes), m_tamanho(tamanho), m_iv(iv)
    {
    }
    ~CAesKey();

    const std::vector<uebyte>& GetBytes() const { return m_bytes; }
    ESymmetricCipherKeySize GetTamanho() const { return m_tamanho; }
    ueword GetBits() const { return static_cast<ueword>(m_tamanho); }
    const std::vector<uebyte>& GetIV() const { return m_iv; }

private:
    std::vector<uebyte> m_bytes;
    ESymmetricCipherKeySize m_tamanho = ESCKS_256;
    std::vector<uebyte> m_iv;
};

class ISymmetricCipher : private pattern::NonCopyable {
public:
    explicit ISymmetricCipher(const CAesKey& chave);
    virtual ~ISymmetricCipher() = default;

    virtual void Encrypt(const std::vector<uebyte>& entrada, std::vector<uebyte>& saida) const = 0;
    virtual void Decrypt(const std::vector<uebyte>& entrada, std::vector<uebyte>& saida) const = 0;

protected:
    const CAesKey& GetChave() const { return m_chave; }

private:
    CAesKey m_chave;
};

} // namespace ecourna::api::security
