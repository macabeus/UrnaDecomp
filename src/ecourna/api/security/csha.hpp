#pragma once
#include <memory>
#include <string>
#include <vector>

#include <openssl/evp.h>

#include "ecourna/api/security/ihash.hpp"

namespace ecourna::api::security {

class CSha : public IHash {
public:
    CSha(std::string algoritmo, size_t tamanho);
    virtual ~CSha();

    virtual void Reset();
    virtual void Update(const std::vector<uebyte>& dados);
    virtual const std::vector<uebyte> Finish();
    virtual std::string GetAlgoritmo() const { return m_algoritmo; }

private:
    std::string m_algoritmo;
    size_t m_tamanho;
    std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> m_ctx;
};

class CSha512 : public CSha {
public:
    CSha512();
};

} // namespace ecourna::api::security
