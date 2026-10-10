#pragma once
#include <optional>

#include "ecourna/app/dados/midias/cautenticacao.hpp"

namespace ecourna::app::dados {

// An application the media carries and, optionally, how it authenticates. 72 bytes (CInformacaoMidia's vector
// stride). The enumerator values are ModuloInformacaoMidia::TipoAplicativo's (the converter maps them 1:1);
// enumerator names synthesised.
class CAplicativo {
public:
    enum ETipoAplicativo {
        TA_VOTA = 1,
        TA_SA,
        TA_RED,
        TA_VPP,
        TA_STE,
        TA_ADH,
        TA_ATUE,
    };

    explicit CAplicativo(ETipoAplicativo tipo) : m_tipo(tipo) {}
    CAplicativo(ETipoAplicativo tipo, const CAutenticacao& autenticacao);

    ETipoAplicativo GetTipo() const { return m_tipo; }
    bool PossuiAutenticacao() const { return m_autenticacao.has_value(); }
    const CAutenticacao& GetAutenticacao() const;

private:
    ETipoAplicativo m_tipo;
    std::optional<CAutenticacao> m_autenticacao;
};

} // namespace ecourna::app::dados
