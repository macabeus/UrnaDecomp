#pragma once
#include <string>

namespace ecourna::app::dados {

// A parametrised label: its grammatical gender and four spellings. 52 bytes. Layout from its converter
// (app/dados/asn/parametrizacaourna/cconversorlabelparametrizado.cpp, fns 9169/9167, which inline the constructor);
// enumerator names synthesised.
class CLabelParametrizado {
public:
    enum EGeneroLabel {
        EGL_NEUTRO,
        EGL_MASCULINO,
        EGL_FEMININO,
    };

    CLabelParametrizado(EGeneroLabel genero, const std::string& longo, const std::string& curto,
                        const std::string& longoPlural, const std::string& curtoPlural)
        : m_genero(genero), m_longo(longo), m_curto(curto), m_longoPlural(longoPlural), m_curtoPlural(curtoPlural)
    {
    }

    EGeneroLabel GetGenero() const { return m_genero; }
    const std::string& GetLongo() const { return m_longo; }
    const std::string& GetCurto() const { return m_curto; }
    const std::string& GetLongoPlural() const { return m_longoPlural; }
    const std::string& GetCurtoPlural() const { return m_curtoPlural; }

private:
    EGeneroLabel m_genero;
    std::string m_longo;
    std::string m_curto;
    std::string m_longoPlural;
    std::string m_curtoPlural;
};

} // namespace ecourna::app::dados
