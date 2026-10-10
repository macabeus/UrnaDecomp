#pragma once
#include <string>

#include "ecourna/app/dados/iidentificadoreleitor.hpp"

namespace ecourna::app::dados {

class CNumeroIdentificacaoLivre : public IIdentificadorEleitor {
public:
    explicit CNumeroIdentificacaoLivre(const std::string& numero);

    std::string Formatado() const override;
};

} // namespace ecourna::app::dados
