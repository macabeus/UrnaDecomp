#pragma once
#include <vector>

#include "ecourna/api/security/imacalgorithm.hpp"

namespace ecourna::api::security {

class CSiphashMac final : public IMacAlgorithm {
public:
    CSiphashMac() = default;
    virtual ~CSiphashMac() = default;

protected:
    virtual void DoMac(const std::vector<uebyte>& dados, std::vector<uebyte>& mac, const std::vector<uebyte>& chave) const;
    virtual bool DoVerify(const std::vector<uebyte>& dados, const std::vector<uebyte>& mac, const std::vector<uebyte>& chave) const;
};

} // namespace ecourna::api::security
