#pragma once
#include <string>

#include "ecourna/app/dados/iidentificadoreleitor.hpp"

namespace ecourna::app::dados {

class CNumeroCPF : public IIdentificadorEleitor {
public:
    explicit CNumeroCPF(const std::string& numero);

    std::string Formatado() const override;
};

} // namespace ecourna::app::dados
