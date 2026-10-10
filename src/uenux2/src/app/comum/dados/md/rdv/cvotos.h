#pragma once

#include <cstddef>
#include <functional>
#include <string>
#include <vector>

#include "app/comum/dados/md/rdv/cvoto.h"
#include "ecourna/api/uetypes.hpp"

namespace comum::md {

using TQtdVoto = ueword;

class CVotos {
public:
    void Insere(const CVoto& voto, size_t posicao);

    TQtdVoto Total() const { return static_cast<TQtdVoto>(m_votos.size()); }
    TQtdVoto Candidato(const std::string& numero) const;
    TQtdVoto Legenda(const std::string& numero) const;
    TQtdVoto Partido(const std::string& numero) const;
    TQtdVoto TotalQue(bool (CVoto::*criterio)() const) const;

private:
    using TCriterio = std::function<bool(const CVoto&)>;

    TQtdVoto Conta(TCriterio criterio) const;

    std::vector<CVoto> m_votos;
};

} // namespace comum::md
