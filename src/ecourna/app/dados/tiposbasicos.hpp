#pragma once
#include <format>
#include <string>

#include "ecourna/api/pattern/cbasetype.hpp"

namespace ecourna::app::dados {

// The limits CBaseType<ETipoIdentificadorEleitor, TipoIDPrimeiro, TipoIDNumeroLivre, 40> spells in the target.
enum ETipoIdentificadorEleitor {
    TipoIDPrimeiro = 1,
    TipoIDInscricaoEleitoral = TipoIDPrimeiro,
    TipoIDCPF = 2,
    TipoIDNumeroLivre = 3,
};

// ErroBioUltimo (13) is accepted by the range check and rejected by CErroLeituraBiometria: the valid errors are 0..12.
// The names between follow ModuloResultadoUrnaCadastro::ErroLeituraBiometria (synthesised; the converters map them 1:1).
enum EErroLeituraBiometria {
    ErroBioPrimeiro = 0,
    ErroBioSemErro = ErroBioPrimeiro,
    ErroBioTipoArquivoInvalido,
    ErroBioIdCriptografiaInvalido,
    ErroBioChaveSecretaVazia,
    ErroBioChaveSecaoVazia,
    ErroBioConteudoVazio,
    ErroBioChaveSecaoComTamanhoInvalido,
    ErroBioChaveSecretaComTamanhoInvalido,
    ErroBioNaoFoiPossivelIniciarADecifragem,
    ErroBioErroDecifrandoBuffer,
    ErroBioErroDesconhecidoDeCriptografia,
    ErroBioErroFormacaoTemplate,
    ErroBioErroCodificacaoMinuncias,
    ErroBioUltimo = 13,
};

// One typedef per NOME the target defines, in tiposbasicos.cpp's definition order. The template arguments are the
// target's where a CBaseType constructor record spells them (BASIC_TYPE 0 3 4 6 7 8 9 15 34 36 38 39 40); the others
// have no constructor in the target, so their arguments are unobservable and synthesised here.
using TProcessoEleitoralID = CBaseType<unsigned int, 0, 99999, 0>;
using TMunicipioID = CBaseType<unsigned int, 0, 99999, 6>;
using TOrdemSuplencia = CBaseType<unsigned short, 0, 99, 10>;            // synthesised
using TQtdCandidato = CBaseType<unsigned short, 0, 9999, 12>;           // synthesised
using TOrigemConfiguracao = CBaseType<unsigned short, 0, 99, 13>;       // synthesised
using TAmbienteExecucao = CBaseType<unsigned short, 0, 99, 14>;         // synthesised
using TTipoAbrangencia = CBaseType<unsigned short, 0, 99, 16>;          // synthesised
using TTipoEleicao = CBaseType<unsigned short, 0, 99, 17>;              // synthesised
using TSituacaoPleito = CBaseType<unsigned short, 0, 99, 18>;           // synthesised
using TTipoImpedimento = CBaseType<unsigned short, 0, 99, 19>;          // synthesised
using TTipoLocalVotacao = CBaseType<unsigned short, 0, 99, 20>;         // synthesised
using TTipoTransferenciaTemporaria = CBaseType<unsigned short, 0, 99, 21>; // synthesised
using TErroLeituraBiometria = CBaseType<EErroLeituraBiometria, ErroBioPrimeiro, ErroBioUltimo, 34>;
using TOrdemEleicao = CBaseType<unsigned short, 0, 99, 35>;             // synthesised
using TNumeroVotavel = CBaseType<unsigned int, 0, 99999, 37>;           // synthesised
using TRepositorioVotosID = CBaseType<unsigned int, 0, 99999, 41>;      // synthesised
using TScoreHabilitacao = CBaseType<unsigned short, 0, 999, 38>;
using TFederacaoID = CBaseType<unsigned short, 100, 999, 39>;
using TTipoIdentificadorEleitor = CBaseType<ETipoIdentificadorEleitor, TipoIDPrimeiro, TipoIDNumeroLivre, 40>;
using TCargoID = CBaseType<unsigned short, 1, 99, 3>;
using TPartidoID = CBaseType<unsigned short, 0, 99, 4>;
using TZonaID = CBaseType<unsigned short, 0, 9999, 7>;
using TLocalID = CBaseType<unsigned int, 0, 9999, 8>;
using TSecaoID = CBaseType<unsigned short, 0, 9999, 9>;
using TQtdEleitor = CBaseType<unsigned short, 0, 9999, 15>;
using TAno = CBaseType<unsigned short, 0, 9999, 36>;
// No NOME: every constructor call the converters make is constant and inlined (R-ASNCORE, app/dados/asn/cconversorturno.cpp).
using TTurno = CBaseType<unsigned short, 1, 2, 11>;

class CErroLeituraBiometria : public TErroLeituraBiometria {
public:
    CErroLeituraBiometria(EErroLeituraBiometria erro);
};

} // namespace ecourna::app::dados

// The explicit specializations tiposbasicos.cpp defines.
template <> const std::string ecourna::app::dados::TProcessoEleitoralID::NOME;
template <> const std::string ecourna::app::dados::TMunicipioID::NOME;
template <> const std::string ecourna::app::dados::TOrdemSuplencia::NOME;
template <> const std::string ecourna::app::dados::TQtdCandidato::NOME;
template <> const std::string ecourna::app::dados::TOrigemConfiguracao::NOME;
template <> const std::string ecourna::app::dados::TAmbienteExecucao::NOME;
template <> const std::string ecourna::app::dados::TTipoAbrangencia::NOME;
template <> const std::string ecourna::app::dados::TTipoEleicao::NOME;
template <> const std::string ecourna::app::dados::TSituacaoPleito::NOME;
template <> const std::string ecourna::app::dados::TTipoImpedimento::NOME;
template <> const std::string ecourna::app::dados::TTipoLocalVotacao::NOME;
template <> const std::string ecourna::app::dados::TTipoTransferenciaTemporaria::NOME;
template <> const std::string ecourna::app::dados::TErroLeituraBiometria::NOME;
template <> const std::string ecourna::app::dados::TOrdemEleicao::NOME;
template <> const std::string ecourna::app::dados::TNumeroVotavel::NOME;
template <> const std::string ecourna::app::dados::TRepositorioVotosID::NOME;
template <> const std::string ecourna::app::dados::TScoreHabilitacao::NOME;
template <> const std::string ecourna::app::dados::TFederacaoID::NOME;
template <> const std::string ecourna::app::dados::TTipoIdentificadorEleitor::NOME;
template <> const std::string ecourna::app::dados::TCargoID::NOME;
template <> const std::string ecourna::app::dados::TPartidoID::NOME;
template <> const std::string ecourna::app::dados::TZonaID::NOME;
template <> const std::string ecourna::app::dados::TLocalID::NOME;
template <> const std::string ecourna::app::dados::TSecaoID::NOME;
template <> const std::string ecourna::app::dados::TQtdEleitor::NOME;
template <> const std::string ecourna::app::dados::TAno::NOME;

// An enum formats as its underlying unsigned value (the target's handle formatter fn 536 tests `>= 128` unsigned for
// the char presentation, i.e. formatter<unsigned int>).
template <>
struct std::formatter<ecourna::app::dados::EErroLeituraBiometria> : std::formatter<unsigned int> {
    template <typename FormatContext>
    auto format(ecourna::app::dados::EErroLeituraBiometria valor, FormatContext& contexto) const
    {
        return std::formatter<unsigned int>::format(static_cast<unsigned int>(valor), contexto);
    }
};
