#pragma once
#include "ecourna/app/dados/tiposbasicos.hpp"

namespace ecourna::app::dados {

// A municipality and electoral zone pair. Layout from its converter (app/dados/asn/cconversormunicipiozona.cpp,
// fn 9131): the municipality code at +0, the zone at +4; the constructor is out of line (fn 9130 calls it, fn 5108).
class CMunicipioZona {
public:
    CMunicipioZona(const TMunicipioID& municipio, const TZonaID& zona);

    TMunicipioID GetMunicipio() const { return m_municipio; }
    TZonaID GetZona() const { return m_zona; }

private:
    TMunicipioID m_municipio;
    TZonaID m_zona;
};

} // namespace ecourna::app::dados
