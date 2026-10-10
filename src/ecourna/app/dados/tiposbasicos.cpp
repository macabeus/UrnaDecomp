#include "ecourna/app/dados/tiposbasicos.hpp"

#include <format>

#include "ecourna/app/dados/edadoserror.hpp"

// The names CBaseType<...>::CBaseType prints. Target layout: the names longer than 10 characters live in .bss from
// 1911992 (12 bytes apart, built by this TU's static initialiser inside __wasm_call_ctors), the short ones are
// constant-initialised SSO strings in .data from 1122284, both in this order.
template <> const std::string ecourna::app::dados::TProcessoEleitoralID::NOME = "ProcessoEleitoralID";
template <> const std::string ecourna::app::dados::TMunicipioID::NOME = "MunicipioID";
template <> const std::string ecourna::app::dados::TOrdemSuplencia::NOME = "OrdemSuplencia";
template <> const std::string ecourna::app::dados::TQtdCandidato::NOME = "QtdCandidato";
template <> const std::string ecourna::app::dados::TOrigemConfiguracao::NOME = "OrigemConfiguracao";
template <> const std::string ecourna::app::dados::TAmbienteExecucao::NOME = "AmbienteExecucao";
template <> const std::string ecourna::app::dados::TTipoAbrangencia::NOME = "TipoAbrangencia";
template <> const std::string ecourna::app::dados::TTipoEleicao::NOME = "TipoEleicao";
template <> const std::string ecourna::app::dados::TSituacaoPleito::NOME = "SituacaoPleito";
template <> const std::string ecourna::app::dados::TTipoImpedimento::NOME = "TipoImpedimento";
template <> const std::string ecourna::app::dados::TTipoLocalVotacao::NOME = "TipoLocalVotacao";
template <> const std::string ecourna::app::dados::TTipoTransferenciaTemporaria::NOME = "TipoTransferenciaTemporaria";
template <> const std::string ecourna::app::dados::TErroLeituraBiometria::NOME = "ErroLeituraBiometria";
template <> const std::string ecourna::app::dados::TOrdemEleicao::NOME = "OrdemEleicao";
template <> const std::string ecourna::app::dados::TNumeroVotavel::NOME = "NumeroVotavel";
template <> const std::string ecourna::app::dados::TRepositorioVotosID::NOME = "RepositorioVotosID";
template <> const std::string ecourna::app::dados::TScoreHabilitacao::NOME = "ScoreHabilitacao";
template <> const std::string ecourna::app::dados::TFederacaoID::NOME = "FederacaoID";
template <> const std::string ecourna::app::dados::TTipoIdentificadorEleitor::NOME = "TipoIdentificadorEleitor";
template <> const std::string ecourna::app::dados::TCargoID::NOME = "CargoID";
template <> const std::string ecourna::app::dados::TPartidoID::NOME = "PartidoID";
template <> const std::string ecourna::app::dados::TZonaID::NOME = "ZonaID";
template <> const std::string ecourna::app::dados::TLocalID::NOME = "LocalID";
template <> const std::string ecourna::app::dados::TSecaoID::NOME = "SecaoID";
template <> const std::string ecourna::app::dados::TQtdEleitor::NOME = "QtdEleitor";
template <> const std::string ecourna::app::dados::TAno::NOME = "Ano";

namespace ecourna::app::dados {

// The rest of the original file (its throw below is on line 1149) is not recovered: no other function, record or
// datum of this object is identified in the target.

// (original lines 42-1145: content not recovered)
#line 1146
CErroLeituraBiometria::CErroLeituraBiometria(EErroLeituraBiometria erro) : TErroLeituraBiometria(erro)
{
    if (erro == ErroBioUltimo) {
        throw CDadosError(EDE_ERRO_LEITURA_BIOMETRIA_INVALIDO,
                          std::format("O tipo '{}' deve ter valores no intervalo [{},{}].", NOME, ErroBioPrimeiro,
                                      ErroBioUltimo - 1));
    }
}

// Stand-in: the target links CBaseType<ETipoIdentificadorEleitor, ...>::CBaseType (fn 2666) from THIS object (its
// record 1122400 follows the CErroLeituraBiometria ones), so something here calls it out of line after the constructor
// above. Which function did is unknown; this one is dead in the target and garbage-collected by the link.
TTipoIdentificadorEleitor CriaTipoIdentificadorEleitor(int tipo)
{
    return TTipoIdentificadorEleitor(static_cast<ETipoIdentificadorEleitor>(tipo));
}

} // namespace ecourna::app::dados
