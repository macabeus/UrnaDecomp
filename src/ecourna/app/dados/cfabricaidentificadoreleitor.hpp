#pragma once
#include <memory>
#include <string>

#include "ecourna/app/dados/iidentificadoreleitor.hpp"

namespace ecourna::app::dados {

class CFabricaIdentificadorEleitor {
public:
    static std::shared_ptr<IIdentificadorEleitor> Cria(int tipo, const std::string& numero);
};

} // namespace ecourna::app::dados
