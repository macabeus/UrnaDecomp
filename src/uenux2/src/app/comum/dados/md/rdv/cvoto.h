#pragma once

#include <string>

namespace comum::md {

class CVoto {
public:
    // ModuloRegistroDigitalVoto::TipoVoto
    enum ETipo {
        LEGENDA = 1,
        NOMINAL,
        BRANCO,
        NULO,
        BRANCO_APOS_SUSPENSAO,
        NULO_APOS_SUSPENSAO,
        NULO_POR_REPETICAO,
        NULO_CARGO_SEM_CANDIDATO,
        NULO_APOS_SUSPENSAO_CARGO_SEM_CANDIDATO,
    };

    CVoto(ETipo tipo, const std::string& numero);

    ETipo GetTipo() const { return m_tipo; }
    const std::string& GetNumero() const { return m_numero; }

    bool EhNominal() const;
    bool EhLegenda() const;
    bool EhNulo() const;
    bool EhBranco() const;

    static bool EhNulo(ETipo tipo);

private:
    ETipo m_tipo;
    std::string m_numero;
};

} // namespace comum::md
