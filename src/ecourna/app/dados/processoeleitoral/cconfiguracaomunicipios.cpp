#include "ecourna/app/dados/processoeleitoral/cconfiguracaomunicipios.hpp"

#include <algorithm>

#include "ecourna/app/dados/processoeleitoral/edadosprocessoeleitoralerror.hpp"

namespace ecourna::app::dados {

// (original lines 9-19: content not recovered)
#line 20
CConfiguracaoMunicipios::CConfiguracaoMunicipios(const CCabecalhoEntidade& cabecalho,
                                                 const TMapConfiguracaoMunicipio& configuracoes)
    : m_cabecalho(cabecalho), m_configuracoes(configuracoes)
{
    if (m_configuracoes.empty()) {
        throw CDadosProcessoEleitoralError(EDPE_CONFIGURACOES_MUNICIPIOS_VAZIA,
                                           "A lista de configurações de municípios não pode ser vazia.");
    }

    if (!std::all_of(m_configuracoes.begin(), m_configuracoes.end(),
                     [](const auto& item) { return item.first == item.second.GetCodigoMunicipio(); })) {
        throw CDadosProcessoEleitoralError(
            EDPE_INDICE_CONFIGURACAO_MUNICIPIO,
            "Cada índice no mapa deve ser igual ao código do município da configuração correspondente.");
    }
}

} // namespace ecourna::app::dados
