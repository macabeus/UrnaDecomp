#pragma once

namespace ecourna::app::dados {

// The election phase. Layout from its converter (app/dados/asn/cconversorfase.cpp, fns 9195/9193): one 4-byte enum,
// passed and returned by value; FASE_INDEFINIDA is the value the converter rejects explicitly. Names synthesised.
class CFaseID {
public:
    enum EFase {
        FASE_INDEFINIDA = 0,
        FASE_SIMULADO,
        FASE_OFICIAL,
        FASE_TREINAMENTO,
    };

    CFaseID(EFase fase) : m_fase(fase) {}

    EFase Get() const { return m_fase; }

private:
    EFase m_fase;
};

// The name CInformacaoMidia (app/dados/midias) and comum::CGravadorUtil spell the phase by (source-location records).
using TFaseID = CFaseID;

} // namespace ecourna::app::dados
