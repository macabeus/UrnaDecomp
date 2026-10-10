#pragma once
#include <map>

#include "ecourna/app/dados/ccabecalhoentidade.hpp"
#include "ecourna/app/dados/processoeleitoral/cconfiguracaomunicipio.hpp"
#include "ecourna/app/dados/tiposbasicos.hpp"

namespace ecourna::app::dados {

using TMapConfiguracaoMunicipio = std::map<TMunicipioID, CConfiguracaoMunicipio>;

// The municipalities' configurations of an election process, keyed by municipality code (never empty).
class CConfiguracaoMunicipios {
public:
    CConfiguracaoMunicipios(const CCabecalhoEntidade& cabecalho, const TMapConfiguracaoMunicipio& configuracoes);

    const CCabecalhoEntidade& GetCabecalho() const { return m_cabecalho; }
    const TMapConfiguracaoMunicipio& GetConfiguracoes() const { return m_configuracoes; }

private:
    CCabecalhoEntidade m_cabecalho;
    TMapConfiguracaoMunicipio m_configuracoes;
};

} // namespace ecourna::app::dados
