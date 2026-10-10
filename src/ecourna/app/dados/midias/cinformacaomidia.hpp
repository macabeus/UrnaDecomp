#pragma once
#include <string>
#include <vector>

#include "ecourna/app/dados/cfaseid.hpp"
#include "ecourna/app/dados/midias/caplicativo.hpp"
#include "ecourna/app/dados/midias/cdadosgeracaomidia.hpp"
#include "ecourna/app/dados/tiposbasicos.hpp"

namespace ecourna::app::dados {

using TVectorAplicativo = std::vector<CAplicativo>;

// What a media is (result, load or vote media), for which election, and how it was generated; a result media also
// lists the applications it initialises. 120 bytes; layout from the two constructors (fns 9033/9031) and the converter
// (app/dados/asn/midias/cconversorinformacaomidia.cpp, fns 9190/9179). Enumerator names synthesised: the values are
// ModuloInformacaoMidia::TipoMidia's minus one (the converter maps mr/fc/fv to 0/1/2).
class CInformacaoMidia {
public:
    enum ETipoMidia {
        TM_MR = 0,
        TM_FC,
        TM_FV,
    };

    // A result media (MR), with the applications it initialises.
    CInformacaoMidia(TFaseID fase, TProcessoEleitoralID processoEleitoral, const std::string& uf, TTurno turno,
                     const CDadosGeracaoMidia& dadosGeracaoMidia, const TVectorAplicativo& aplicativos);
    // Any other media.
    CInformacaoMidia(ETipoMidia tipoMidia, TFaseID fase, TProcessoEleitoralID processoEleitoral, const std::string& uf,
                     TTurno turno, const CDadosGeracaoMidia& dadosGeracaoMidia);

    ETipoMidia GetTipoMidia() const { return m_tipoMidia; }
    TFaseID GetFase() const { return m_fase; }
    TProcessoEleitoralID GetProcessoEleitoral() const { return m_processoEleitoral; }
    const std::string& GetUF() const { return m_uf; }
    TTurno GetTurno() const { return m_turno; }
    const CDadosGeracaoMidia& GetDadosGeracaoMidia() const { return m_dadosGeracaoMidia; }
    const TVectorAplicativo& GetAplicativos() const;

private:
    ETipoMidia m_tipoMidia;
    TFaseID m_fase;
    TProcessoEleitoralID m_processoEleitoral;
    std::string m_uf;
    TTurno m_turno;
    CDadosGeracaoMidia m_dadosGeracaoMidia;
    TVectorAplicativo m_aplicativos;
};

} // namespace ecourna::app::dados
