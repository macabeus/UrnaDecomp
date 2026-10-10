#include "ecourna/api/security/csiphashmac.hpp"

#include <cstring>
#include <format>
#include <memory>

#include <openssl/core_names.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/params.h>

#include "ecourna/api/security/esecurityerror.hpp"

namespace ecourna::api::security {

namespace {

const size_t TAMANHO_MAC = 8;
const size_t TAMANHO_CHAVE = 16;

} // namespace

void CSiphashMac::DoMac(const std::vector<uebyte>& dados, std::vector<uebyte>& mac, const std::vector<uebyte>& chave) const
{
    mac = std::vector<uebyte>(TAMANHO_MAC);

    std::unique_ptr<EVP_MAC, decltype(&EVP_MAC_free)> algoritmo(EVP_MAC_fetch(nullptr, "siphash", nullptr), EVP_MAC_free);

    unsigned int tamanho = TAMANHO_MAC;
    unsigned int cRounds = 4;
    unsigned int dRounds = 6;
    std::unique_ptr<OSSL_PARAM[]> params(new OSSL_PARAM[4]());
    params[0] = OSSL_PARAM_construct_uint(OSSL_MAC_PARAM_SIZE, &tamanho);
    params[1] = OSSL_PARAM_construct_uint(OSSL_MAC_PARAM_C_ROUNDS, &cRounds);
    params[2] = OSSL_PARAM_construct_uint(OSSL_MAC_PARAM_D_ROUNDS, &dRounds);
    params[3] = OSSL_PARAM_construct_end();

    std::unique_ptr<EVP_MAC_CTX, decltype(&EVP_MAC_CTX_free)> ctx(EVP_MAC_CTX_new(algoritmo.get()), EVP_MAC_CTX_free);

    // (original lines 40-50: content not recovered)
#line 51
    if (EVP_MAC_init(ctx.get(), chave.data(), chave.size(), params.get()) != 1) {
        throw CSecurityError(ESE_MAC_FALHA_INICIAR, std::format("Falha ao autenticar. - {}", ERR_get_error()));
    }

    // (original lines 55-57: content not recovered)
#line 58
    if (EVP_MAC_update(ctx.get(), dados.data(), dados.size()) != 1) {
        throw CSecurityError(ESE_MAC_FALHA_ATUALIZAR, std::format("Falha ao autenticar. - {}", ERR_get_error()));
    }

    // (original lines 62-64: content not recovered)
#line 65
    if (EVP_MAC_final(ctx.get(), mac.data(), nullptr, mac.size()) != 1) {
        throw CSecurityError(ESE_MAC_FALHA_FINALIZAR, std::format("Falha ao autenticar. - {}", ERR_get_error()));
    }
}

bool CSiphashMac::DoVerify(const std::vector<uebyte>& dados, const std::vector<uebyte>& mac, const std::vector<uebyte>& chave) const
{
    if (chave.size() != TAMANHO_CHAVE) {

        // (original lines 74-78: content not recovered)
#line 79
        throw CSecurityError(ESE_MAC_TAMANHO_CHAVE, "O tamanho da chave deve ser igual 16 bytes.");
    }

    try {
        std::vector<uebyte> calculado(TAMANHO_MAC);
        DoMac(dados, calculado, chave);
        return std::memcmp(mac.data(), calculado.data(), TAMANHO_MAC) == 0;
    }
    catch (const std::exception&) {
        throw CSecurityError(ESE_MAC_TAMANHO_CHAVE, "Falha ao autenticar.");
    }
}

} // namespace ecourna::api::security
