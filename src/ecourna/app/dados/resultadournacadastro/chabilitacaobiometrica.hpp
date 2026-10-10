#pragma once
#include <optional>

#include "ecourna/api/uetypes.hpp"
#include "ecourna/app/dados/cdedo.hpp"
#include "ecourna/app/dados/resultadournacadastro/cestadohabilitacaoporcodigo.hpp"
#include "ecourna/app/dados/tiposbasicos.hpp"

namespace ecourna::app::dados {

// The biometric enabling of a voter: attempts (@0), last score (@4), the finger enabled (@8), the reading error (@12)
// and, optionally, the enabling by code that followed (@16, engaged byte @44). 48 bytes. A score of 0 is "no score"
// for the converter (fn 9071 includes ultimoScore only when it is non-zero).
class CHabilitacaoBiometrica {
public:
    CHabilitacaoBiometrica(uedword tentativas, TScoreHabilitacao ultimoScore, CDedo::ETipoDedo dedoHabilitado,
                           TErroLeituraBiometria erroLeituraBiometria);
    CHabilitacaoBiometrica(uedword tentativas, TScoreHabilitacao ultimoScore, CDedo::ETipoDedo dedoHabilitado,
                           TErroLeituraBiometria erroLeituraBiometria,
                           const CEstadoHabilitacaoPorCodigo& estadoHabilitacaoPorCodigo);

    uedword GetTentativas() const { return m_tentativas; }
    TScoreHabilitacao GetUltimoScore() const { return m_ultimoScore; }
    CDedo::ETipoDedo GetDedoHabilitado() const { return m_dedoHabilitado; }
    TErroLeituraBiometria GetErroLeituraBiometria() const { return m_erroLeituraBiometria; }
    bool HasEstadoHabilitacaoPorCodigo() const { return m_estadoHabilitacaoPorCodigo.has_value(); }
    const CEstadoHabilitacaoPorCodigo& GetEstadoHabilitacaoPorCodigo() const;

private:
    uedword m_tentativas;
    TScoreHabilitacao m_ultimoScore;
    CDedo::ETipoDedo m_dedoHabilitado;
    TErroLeituraBiometria m_erroLeituraBiometria;
    std::optional<CEstadoHabilitacaoPorCodigo> m_estadoHabilitacaoPorCodigo;
};

} // namespace ecourna::app::dados
