#pragma once
#include <vector>

#include "ecourna/api/uetypes.hpp"

namespace ecourna::api::cepesc {

class CInfoSalt {
public:
    CInfoSalt(const std::vector<uebyte>& salt, const std::vector<uebyte>& info);
    CInfoSalt(const CInfoSalt& outro);

    const std::vector<uebyte>& GetSalt() const { return m_salt; }
    const std::vector<uebyte>& GetInfo() const { return m_info; }

private:
    std::vector<uebyte> m_salt;
    std::vector<uebyte> m_info;
};

} // namespace ecourna::api::cepesc
