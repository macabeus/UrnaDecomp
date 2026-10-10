#pragma once
#include "ecourna/app/dados/tiposbasicos.hpp"

namespace ecourna::app::dados {

// LAYOUT-ONLY STAND-IN (R-RUC-DATA): what CComparecimentoSecao needs - 16 bytes, 4-byte aligned, trivially copyable
// (fn 3485 copies it as two align=2 i64 pairs). Members follow ModuloTiposEleitorais::IdentificacaoSecaoEleitoral
// (municipioZona, local, secao); their order and types are not recovered. The lane that reconstructs the class (its
// converter converts CComparecimentoSecao, fn 9101) replaces this file.
class CIdentificacaoSecaoEleitoral {
public:
    CIdentificacaoSecaoEleitoral(TMunicipioID municipio, TZonaID zona, TLocalID local, TSecaoID secao)
        : m_municipio(municipio), m_zona(zona), m_local(local), m_secao(secao)
    {
    }

    TMunicipioID GetMunicipio() const { return m_municipio; }
    TZonaID GetZona() const { return m_zona; }
    TLocalID GetLocal() const { return m_local; }
    TSecaoID GetSecao() const { return m_secao; }

private:
    TMunicipioID m_municipio;
    TZonaID m_zona;
    TLocalID m_local;
    TSecaoID m_secao;
};

} // namespace ecourna::app::dados
