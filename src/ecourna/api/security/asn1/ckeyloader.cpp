#include "ecourna/api/security/asn1/ckeyloader.hpp"

#include "ecourna/api/security/esecurityerror.hpp"
#include "ecourna/api/security/isymmetriccipher.hpp"
#include "ModuloEnvelopeChave.h"

namespace ecourna::api::security {

template <typename T>
std::vector<uebyte> CKeyLoader::DecipherKeyIfNeeded(const T& entidade)
{
    const bool cifrado = entidade.get_cifrado();
    const ASN1::OCTET_STRING& octetos = entidade.get_chave();
    const std::vector<uebyte> chave(octetos.begin(), octetos.end());
    if (!cifrado) {
        return chave;
    }

    // (original lines 19-76: content not recovered)
#line 77
    if (!m_cipher) {
        throw CSecurityError(ESE_CIFRA_SIMETRICA_NECESSARIA, "Um TSharedSymmetricCipher é necessário para decifrar chaves.");
    }

    std::vector<uebyte> decifrada;
    m_cipher->Decrypt(chave, decifrada);
    return decifrada;
}

template std::vector<uebyte> CKeyLoader::DecipherKeyIfNeeded<ModuloEnvelopeChave::EntidadeChave>(const ModuloEnvelopeChave::EntidadeChave&);

} // namespace ecourna::api::security
