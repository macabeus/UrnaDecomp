#include "ecourna/app/dados/midias/cinformacaomidia.hpp"

#include "ecourna/app/dados/midias/edadosmidiaserror.hpp"

namespace ecourna::app::dados {

CInformacaoMidia::CInformacaoMidia(TFaseID fase, TProcessoEleitoralID processoEleitoral, const std::string& uf,
                                   TTurno turno, const CDadosGeracaoMidia& dadosGeracaoMidia,
                                   const TVectorAplicativo& aplicativos)
    : m_tipoMidia(TM_MR)
    , m_fase(fase)
    , m_processoEleitoral(processoEleitoral)
    , m_uf(uf)
    , m_turno(turno)
    , m_dadosGeracaoMidia(dadosGeracaoMidia)
    , m_aplicativos(aplicativos)
{
}

// (original lines 20-39: content not recovered)
#line 40
CInformacaoMidia::CInformacaoMidia(ETipoMidia tipoMidia, TFaseID fase, TProcessoEleitoralID processoEleitoral,
                                   const std::string& uf, TTurno turno, const CDadosGeracaoMidia& dadosGeracaoMidia)
    : m_tipoMidia(tipoMidia)
    , m_fase(fase)
    , m_processoEleitoral(processoEleitoral)
    , m_uf(uf)
    , m_turno(turno)
    , m_dadosGeracaoMidia(dadosGeracaoMidia)
{
    if (m_tipoMidia == TM_MR) {


        throw CDadosMidiasError(EDME_TIPO_MIDIA_INVALIDO, "Tipo de mídia inválido para este construtor: MR.");
    }
}

// (original lines 56-57: content not recovered)
#line 58
const TVectorAplicativo& CInformacaoMidia::GetAplicativos() const
{
    if (m_tipoMidia != TM_MR) {
        throw CDadosMidiasError(EDME_APLICATIVOS_SOMENTE_MIDIA_RESULTADO,
                                "Apenas mídias de resultado possuem informação de inicialização de aplicativos.");
    }
    return m_aplicativos;
}

} // namespace ecourna::app::dados
