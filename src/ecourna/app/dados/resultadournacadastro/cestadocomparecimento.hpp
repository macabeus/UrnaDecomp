#pragma once
#include <optional>

#include "ecourna/app/dados/cregistroidentificacaoeleitor.hpp"
#include "ecourna/app/dados/resultadournacadastro/capresentacaofotoeleitor.hpp"
#include "ecourna/app/dados/resultadournacadastro/chabilitacaobiometrica.hpp"

namespace ecourna::app::dados {

// One voter's attendance: who (@0, 20 B), the attendance (@20), and optionally the photo shown (@24, engaged @32),
// how audio was enabled (@36, engaged @40) and the biometric enabling (@44, engaged @92). 96 bytes.
// ESituacaoComparecimento's values: the converter maps ASN.1 faltou/naoVotou/votou/semCargoParaVotar (1..4) to
// 0/2/3/1 (table at 1135444, fn 9080). ESituacaoHabilitacaoAudio's values are synthesised (0-based).
class CEstadoComparecimento {
public:
    enum ESituacaoComparecimento {
        Faltou = 0,
        SemCargoParaVotar = 1,
        NaoVotou = 2,
        Votou = 3,
    };

    enum ESituacaoHabilitacaoAudio {
        Automatica,
        PeloMesario,
        NaoHabilitado,
    };

    CEstadoComparecimento(const CRegistroIdentificacaoEleitor& identificacaoEleitor,
                          ESituacaoComparecimento situacaoComparecimento);
    CEstadoComparecimento(const CRegistroIdentificacaoEleitor& identificacaoEleitor,
                          ESituacaoComparecimento situacaoComparecimento,
                          const CApresentacaoFotoEleitor& apresentacaoFoto,
                          ESituacaoHabilitacaoAudio situacaoHabilitacaoAudio);
    CEstadoComparecimento(const CRegistroIdentificacaoEleitor& identificacaoEleitor,
                          ESituacaoComparecimento situacaoComparecimento,
                          const CApresentacaoFotoEleitor& apresentacaoFoto,
                          ESituacaoHabilitacaoAudio situacaoHabilitacaoAudio,
                          const CHabilitacaoBiometrica& habilitacaoBiometrica);

    const CRegistroIdentificacaoEleitor& GetIdentificacaoEleitor() const { return m_identificacaoEleitor; }
    ESituacaoComparecimento GetSituacaoComparecimento() const { return m_situacaoComparecimento; }
    bool HasApresentacaoFoto() const { return m_apresentacaoFoto.has_value(); }
    const CApresentacaoFotoEleitor& GetApresentacaoFoto() const;
    bool HasSituacaoHabilitacaoAudio() const { return m_situacaoHabilitacaoAudio.has_value(); }
    ESituacaoHabilitacaoAudio GetSituacaoHabilitacaoAudio() const;
    bool HasHabilitacaoBiometrica() const { return m_habilitacaoBiometrica.has_value(); }
    const CHabilitacaoBiometrica& GetHabilitacaoBiometrica() const;

private:
    CRegistroIdentificacaoEleitor m_identificacaoEleitor;
    ESituacaoComparecimento m_situacaoComparecimento;
    std::optional<CApresentacaoFotoEleitor> m_apresentacaoFoto;
    std::optional<ESituacaoHabilitacaoAudio> m_situacaoHabilitacaoAudio;
    std::optional<CHabilitacaoBiometrica> m_habilitacaoBiometrica;
};

} // namespace ecourna::app::dados
