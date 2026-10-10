#pragma once
#include <string>
#include <vector>

#include "ecourna/api/pattern/noncopyable.hpp"
#include "ecourna/api/uetypes.hpp"

namespace ecourna::api::security {

class IHash : private pattern::NonCopyable {
public:
    virtual ~IHash() = default;

    virtual void Reset() = 0;
    virtual void Update(const std::vector<uebyte>& dados) = 0;
    virtual const std::vector<uebyte> Finish() = 0;
    virtual std::string GetAlgoritmo() const = 0;
};

} // namespace ecourna::api::security
