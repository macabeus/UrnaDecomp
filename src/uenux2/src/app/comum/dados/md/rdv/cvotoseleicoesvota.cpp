#include "app/comum/dados/md/rdv/cvotoseleicoesvota.h"

#include <string>

namespace comum::md {

namespace {

TEleicaoID GetEleicao(api::EUeRdvError erro, TCargoID cargo, const CVotosEleicoesVota::TMapaCargoEleicao& eleicoes,
                      std::source_location local)
{
    const auto it = eleicoes.find(cargo);
    if (it == eleicoes.end()) {
        throw api::CUeRdvError(erro, "Cargo " + std::to_string(cargo) + " nao encontrado", local);
    }
    return it->second;
}

} // namespace

const CVotosCargos& CVotosEleicoesVota::Votos(api::EUeRdvError erro, TEleicaoID eleicao, std::source_location local) const
{
    const auto it = m_votos.find(eleicao);
    if (it == m_votos.end()) {
        throw api::CUeRdvError(erro, "Eleicao (" + std::to_string(eleicao) + ") nao encontrada", local);
    }
    return it->second;
}

TQtdVoto CVotosEleicoesVota::Total(TCargoID cargo, const TTotalCargo& total, api::EUeRdvError erro,
                                   std::source_location local) const
{
    const TEleicaoID eleicao = GetEleicao(erro, cargo, m_eleicoes, local);
    return total(m_votos.find(eleicao)->second, cargo);
}

TQtdVoto CVotosEleicoesVota::Total(TCargoID cargo, const std::string& numero, const TTotalNumero& total,
                                   api::EUeRdvError erro, std::source_location local) const
{
    const TEleicaoID eleicao = GetEleicao(erro, cargo, m_eleicoes, local);
    return total(m_votos.find(eleicao)->second, cargo, numero);
}

// (original lines 44-110: content not recovered)
#line 111
const CVotosCargos& CVotosEleicoesVota::GetVotos(TEleicaoID eleicao) const
{
    return Votos(api::EUE_RDV_VOTOS_SEM_ELEICAO, eleicao);
}

// (original lines 116-123: content not recovered)
#line 124
TQtdVoto CVotosEleicoesVota::Candidato(TCargoID cargo, const std::string& numero) const
{
    return Total(
        cargo, numero,
        [](const CVotosCargos& votos, TCargoID cargoID, const std::string& numeroCandidato) {
            return votos.Candidato(cargoID, numeroCandidato);
        },
        api::EUE_RDV_CANDIDATO_SEM_ELEICAO);
}

// (original lines 134-137: content not recovered)
#line 138
TQtdVoto CVotosEleicoesVota::Legenda(TCargoID cargo, const std::string& numero) const
{
    return Total(
        cargo, numero,
        [](const CVotosCargos& votos, TCargoID cargoID, const std::string& numeroLegenda) {
            return votos.Legenda(cargoID, numeroLegenda);
        },
        api::EUE_RDV_LEGENDA_SEM_ELEICAO);
}

// (original lines 148-151: content not recovered)
#line 152
TQtdVoto CVotosEleicoesVota::Partido(TCargoID cargo, const std::string& numero) const
{
    return Total(
        cargo, numero,
        [](const CVotosCargos& votos, TCargoID cargoID, const std::string& numeroPartido) {
            return votos.Partido(cargoID, numeroPartido);
        },
        api::EUE_RDV_PARTIDO_SEM_ELEICAO);
}

// (original lines 162-163: content not recovered)
#line 164
TQtdVoto CVotosEleicoesVota::Nominais(TCargoID cargo) const
{
    return Total(cargo, [](const CVotosCargos& votos, TCargoID cargoID) { return votos.Nominais(cargoID); },
                 api::EUE_RDV_NOMINAIS_SEM_ELEICAO);
}

TQtdVoto CVotosEleicoesVota::Legendas(TCargoID cargo) const
{
    return Total(cargo, [](const CVotosCargos& votos, TCargoID cargoID) { return votos.Legendas(cargoID); },
                 api::EUE_RDV_LEGENDAS_SEM_ELEICAO);
}

TQtdVoto CVotosEleicoesVota::Nulos(TCargoID cargo) const
{
    return Total(cargo, [](const CVotosCargos& votos, TCargoID cargoID) { return votos.Nulos(cargoID); },
                 api::EUE_RDV_NULOS_SEM_ELEICAO);
}

TQtdVoto CVotosEleicoesVota::Brancos(TCargoID cargo) const
{
    return Total(cargo, [](const CVotosCargos& votos, TCargoID cargoID) { return votos.Brancos(cargoID); },
                 api::EUE_RDV_BRANCOS_SEM_ELEICAO);
}

TQtdVoto CVotosEleicoesVota::Cargo(TCargoID cargo) const
{
    return Total(cargo, [](const CVotosCargos& votos, TCargoID cargoID) { return votos.Cargo(cargoID); },
                 api::EUE_RDV_CARGO_SEM_ELEICAO);
}

} // namespace comum::md
