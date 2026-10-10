#pragma once

#include <map>
#include <string>

#include "app/comum/dados/md/rdv/cvotos.h"
#include "ecourna/api/uetypes.hpp"

namespace comum::md {

using TCargoID = uebyte;
using TEleicaoID = unsigned int;

class CVotosCargos {
public:
    struct SCargoInfo {
        TCargoID cargo;
        bool operator<(const SCargoInfo& outro) const { return cargo < outro.cargo; }
    };
    using TMapa = std::map<SCargoInfo, CVotos>;

    explicit CVotosCargos(TMapa mapa);

    TQtdVoto Candidato(TCargoID cargo, std::string numero) const;
    TQtdVoto Legenda(TCargoID cargo, std::string numero) const;
    TQtdVoto Partido(TCargoID cargo, std::string numero) const;
    TQtdVoto Nominais(TCargoID cargo) const;
    TQtdVoto Legendas(TCargoID cargo) const;
    TQtdVoto Nulos(TCargoID cargo) const;
    TQtdVoto Brancos(TCargoID cargo) const;
    TQtdVoto Cargo(TCargoID cargo) const;

private:
    TMapa m_mapa;
};

} // namespace comum::md
