#pragma once
#include <string>

namespace ecourna::app::dados {

// A report title line: alignment, style and text. 20 bytes. Layout from its converter
// (app/dados/asn/parametrizacaourna/cconversortitulorelatorio.cpp, fns 9149/9145, which inline the constructor);
// enumerator names synthesised.
class CTituloRelatorio {
public:
    enum EAlinhamentoTitulo {
        EAT_ESQUERDO,
        EAT_DIREITO,
        EAT_CENTRO,
    };

    enum EEstiloTitulo {
        EET_NORMAL,
        EET_EXPANDIDO,
    };

    CTituloRelatorio(EAlinhamentoTitulo alinhamento, EEstiloTitulo estilo, const std::string& texto)
        : m_alinhamento(alinhamento), m_estilo(estilo), m_texto(texto)
    {
    }

    EAlinhamentoTitulo GetAlinhamento() const { return m_alinhamento; }
    EEstiloTitulo GetEstilo() const { return m_estilo; }
    const std::string& GetTexto() const { return m_texto; }

private:
    EAlinhamentoTitulo m_alinhamento;
    EEstiloTitulo m_estilo;
    std::string m_texto;
};

} // namespace ecourna::app::dados
