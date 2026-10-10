#pragma once
#include <memory>
#include <vector>

#include "ecourna/api/uetypes.hpp"

namespace ecourna::api::security {

class ISymmetricCipher;
using TSharedSymmetricCipher = std::shared_ptr<ISymmetricCipher>;

class CKeyLoader {
public:
    explicit CKeyLoader(TSharedSymmetricCipher cipher) : m_cipher(std::move(cipher)) {}

    template <typename T>
    std::vector<uebyte> DecipherKeyIfNeeded(const T& entidade);

private:
    TSharedSymmetricCipher m_cipher;
};

} // namespace ecourna::api::security
