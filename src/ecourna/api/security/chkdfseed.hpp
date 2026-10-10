#pragma once
#include <vector>

#include "ecourna/api/uetypes.hpp"

namespace ecourna::api::security {

class CHKDFSeed {
public:
    CHKDFSeed(const std::vector<uebyte>& salt, const std::vector<uebyte>& chave, const std::vector<uebyte>& info)
        : m_salt(salt), m_chave(chave), m_info(info)
    {
    }
    virtual ~CHKDFSeed() = default;

    std::vector<uebyte> GetSeed();

private:
    std::vector<uebyte> m_salt;
    std::vector<uebyte> m_chave;
    std::vector<uebyte> m_info;
};

} // namespace ecourna::api::security
