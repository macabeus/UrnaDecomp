#pragma once
#include <string>
#include <vector>

#include "ecourna/api/uetypes.hpp"

namespace ecourna::api::security {

std::string EncodeBase64(const std::vector<uebyte>& dados);

} // namespace ecourna::api::security
