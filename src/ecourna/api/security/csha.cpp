#include "ecourna/api/security/csha.hpp"

#include <utility>

#include "ecourna/api/security/esecurityerror.hpp"

namespace ecourna::api::security {

CSha::CSha(std::string algoritmo, size_t tamanho)
    : m_algoritmo(std::move(algoritmo))
    , m_tamanho(tamanho)
    , m_ctx(EVP_MD_CTX_new(), EVP_MD_CTX_free)
{
    Reset();
}

CSha::~CSha() = default;

void CSha::Reset()
{
    if (!EVP_DigestInit_ex(m_ctx.get(), EVP_get_digestbyname(m_algoritmo.c_str()), nullptr)) {

        throw CSecurityError(ESE_SHA_FALHA_INICIAR, "Falha ao iniciar o contexto.");
    }
}

void CSha::Update(const std::vector<uebyte>& dados)
{
    if (dados.empty()) {
        throw CSecurityError(ESE_SHA_DADOS_VAZIOS, "Não é possível calcular o hash de dados vazios.");
    }


    if (!EVP_DigestUpdate(m_ctx.get(), dados.data(), dados.size())) {
        throw CSecurityError(ESE_SHA_FALHA_ATUALIZAR, "Falha ao atualizar o hash.");
    }
}

const std::vector<uebyte> CSha::Finish()
{
    std::vector<uebyte> hash(m_tamanho);


    if (!EVP_DigestFinal_ex(m_ctx.get(), hash.data(), nullptr)) {
        throw CSecurityError(ESE_SHA_FALHA_FINALIZAR, "Falha ao finalizar o hash.");
    }

    return hash;
}

CSha512::CSha512()
    : CSha("SHA2-512", 64)
{
}

} // namespace ecourna::api::security
