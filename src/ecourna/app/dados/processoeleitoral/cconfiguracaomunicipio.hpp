#pragma once
#include "ecourna/api/uetypes.hpp"
#include "ecourna/app/dados/tiposbasicos.hpp"

namespace ecourna::app::dados {

// LAYOUT-ONLY STAND-IN (R-DADOS): what CConfiguracaoMunicipios needs - 40 bytes, 8-byte aligned, trivially copyable,
// the municipality code first (fn 9028 compares it with the map key; std::map node value at +24, key at +16). The
// other members are not recovered; the lane that reconstructs the class replaces this file.
class CConfiguracaoMunicipio {
public:
    TMunicipioID GetCodigoMunicipio() const { return m_codigoMunicipio; }

private:
    TMunicipioID m_codigoMunicipio;
    ueint64 m_naoRecuperado[4];
};

} // namespace ecourna::app::dados
