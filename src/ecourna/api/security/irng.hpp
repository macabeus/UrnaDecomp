#pragma once
#include <vector>

#include "ecourna/api/uetypes.hpp"

namespace ecourna::api::security {

class IRng {
public:
    virtual ~IRng() = default;

    virtual uebyte GetByte() = 0;
    virtual int GetInt() = 0;
    virtual int Generate(std::vector<uebyte>& dados) = 0;
    virtual int Generate(std::vector<uebyte>& dados, size_t tamanho) = 0;
};

} // namespace ecourna::api::security
