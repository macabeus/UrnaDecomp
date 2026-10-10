#pragma once
#include <vector>

#include "ecourna/api/pattern/noncopyable.hpp"
#include "ecourna/api/uetypes.hpp"

namespace ecourna::api::security {

class IMacAlgorithm : private pattern::NonCopyable {
public:
    virtual ~IMacAlgorithm() = default;

    void Mac(const std::vector<uebyte>& dados, std::vector<uebyte>& mac, const std::vector<uebyte>& chave) const;

protected:
    virtual void DoMac(const std::vector<uebyte>& dados, std::vector<uebyte>& mac, const std::vector<uebyte>& chave) const = 0;
    virtual bool DoVerify(const std::vector<uebyte>& dados, const std::vector<uebyte>& mac, const std::vector<uebyte>& chave) const = 0;
};

} // namespace ecourna::api::security
