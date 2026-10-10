#pragma once
#include <optional>

#include "ecourna/app/dados/cregistroidentificacaoeleitor.hpp"

namespace ecourna::app::dados {

// How a voter was enabled by code: whether a poll worker was recognised (@0) and, unless NaoReconhecido, the poll
// worker's identification (@4, optional, engaged byte @24). 28 bytes. NaoReconhecido = 0 (fn 5090 throws for any
// other value); the other values follow ModuloResultadoUrnaCadastro::SituacaoReconhecimentoMesario, synthesised.
class CEstadoHabilitacaoPorCodigo {
public:
    enum ESituacaoReconhecimentoMesario {
        NaoReconhecido,
        ReconhecidoDigitalCadastroUrna,
        ReconhecidoDigitalRegistroMesarios,
    };

    CEstadoHabilitacaoPorCodigo(ESituacaoReconhecimentoMesario situacao);
    CEstadoHabilitacaoPorCodigo(ESituacaoReconhecimentoMesario situacao,
                                const CRegistroIdentificacaoEleitor& identificacaoMesario);

    ESituacaoReconhecimentoMesario GetSituacaoReconhecimentoMesario() const { return m_situacaoReconhecimentoMesario; }
    bool HasIdentificacaoMesario() const { return m_identificacaoMesario.has_value(); }
    const CRegistroIdentificacaoEleitor& GetIdentificacaoMesario() const;

private:
    ESituacaoReconhecimentoMesario m_situacaoReconhecimentoMesario;
    std::optional<CRegistroIdentificacaoEleitor> m_identificacaoMesario;
};

} // namespace ecourna::app::dados
