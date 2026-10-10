#pragma once

#include <functional>
#include <map>
#include <source_location>
#include <string>

#include "app/comum/dados/euerdverror.h"
#include "app/comum/dados/md/rdv/cvotoscargos.h"

namespace comum::md {

class CVotosEleicoesVota {
public:
    using TMapa = std::map<TEleicaoID, CVotosCargos>;
    using TMapaCargoEleicao = std::map<TCargoID, TEleicaoID>;

    explicit CVotosEleicoesVota(TMapa mapa);

    const CVotosCargos& GetVotos(TEleicaoID eleicao) const;
    TQtdVoto Comparecimento(TEleicaoID eleicao) const;
    TQtdVoto Candidato(TCargoID cargo, const std::string& numero) const;
    TQtdVoto Legenda(TCargoID cargo, const std::string& numero) const;
    TQtdVoto Partido(TCargoID cargo, const std::string& numero) const;
    TQtdVoto Nominais(TCargoID cargo) const;
    TQtdVoto Legendas(TCargoID cargo) const;
    TQtdVoto Nulos(TCargoID cargo) const;
    TQtdVoto Brancos(TCargoID cargo) const;
    TQtdVoto Cargo(TCargoID cargo) const;

private:
    using TTotalCargo = std::function<TQtdVoto(const CVotosCargos&, TCargoID)>;
    using TTotalNumero = std::function<TQtdVoto(const CVotosCargos&, TCargoID, const std::string&)>;

    const CVotosCargos& Votos(api::EUeRdvError erro, TEleicaoID eleicao,
                              std::source_location local = std::source_location::current()) const;
    TQtdVoto Total(TCargoID cargo, const TTotalCargo& total, api::EUeRdvError erro,
                   std::source_location local = std::source_location::current()) const;
    TQtdVoto Total(TCargoID cargo, const std::string& numero, const TTotalNumero& total, api::EUeRdvError erro,
                   std::source_location local = std::source_location::current()) const;

    TMapa m_votos;
    TMapaCargoEleicao m_eleicoes;
};

} // namespace comum::md
