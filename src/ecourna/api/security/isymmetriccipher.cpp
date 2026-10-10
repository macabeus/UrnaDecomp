#include "ecourna/api/security/isymmetriccipher.hpp"

#include <format>

#include "ecourna/api/security/esecurityerror.hpp"

namespace ecourna::api::security {

CAesKey::~CAesKey() = default;

ISymmetricCipher::ISymmetricCipher(const CAesKey& chave)
    : m_chave(chave)
{
    const size_t tamanho = m_chave.GetBytes().size();
    const ESymmetricCipherKeySize bits = m_chave.GetTamanho();
    if (bits != tamanho * 8) {
        const std::string mensagem = std::format("Incompatibilidade entre o tamanho da chave e a quantidade de bits [{}/{}].",
                                                 tamanho, static_cast<ueword>(bits));

        // (original lines 20-25: content not recovered)
#line 26
        throw CSecurityError(ESE_CHAVE_TAMANHO_BITS, mensagem);
    }
    if (bits == 0) {
        throw CSecurityError(ESE_CHAVE_TAMANHO_BITS, "Quantidade de bits nula.");
    }
}

} // namespace ecourna::api::security
