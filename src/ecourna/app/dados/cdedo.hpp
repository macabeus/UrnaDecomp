#pragma once

namespace ecourna::app::dados {

// A finger. Only the nested enum is observable (CHabilitacaoBiometrica's finger, 4 bytes; its converter's
// ConverteTipoDedo/DeconverteTipoDedo take and return CDedo::ETipoDedo, and DeconverteTipoDedo rejects >= 11).
// Names follow ModuloTiposEleitorais::TipoDedo; values synthesised (0-based, 11 fingers).
class CDedo {
public:
    enum ETipoDedo {
        NaoIdentificado,
        PolegarDireito,
        IndicadorDireito,
        MedioDireito,
        AnelarDireito,
        MinimoDireito,
        PolegarEsquerdo,
        IndicadorEsquerdo,
        MedioEsquerdo,
        AnelarEsquerdo,
        MinimoEsquerdo,
    };
};

} // namespace ecourna::app::dados
