#include "app/comum/dados/md/rdv/cvotoscargos.h"

namespace comum::md {

TQtdVoto CVotosCargos::Candidato(TCargoID cargo, std::string numero) const
{
    const auto it = m_mapa.find(SCargoInfo{cargo});
    return it == m_mapa.end() ? 0 : it->second.Candidato(numero);
}

TQtdVoto CVotosCargos::Legenda(TCargoID cargo, std::string numero) const
{
    const auto it = m_mapa.find(SCargoInfo{cargo});
    return it == m_mapa.end() ? 0 : it->second.Legenda(numero);
}

TQtdVoto CVotosCargos::Partido(TCargoID cargo, std::string numero) const
{
    const auto it = m_mapa.find(SCargoInfo{cargo});
    return it == m_mapa.end() ? 0 : it->second.Partido(numero);
}

TQtdVoto CVotosCargos::Nominais(TCargoID cargo) const
{
    const auto it = m_mapa.find(SCargoInfo{cargo});
    return it == m_mapa.end() ? 0 : it->second.TotalQue(&CVoto::EhNominal);
}

TQtdVoto CVotosCargos::Legendas(TCargoID cargo) const
{
    const auto it = m_mapa.find(SCargoInfo{cargo});
    return it == m_mapa.end() ? 0 : it->second.TotalQue(&CVoto::EhLegenda);
}

TQtdVoto CVotosCargos::Nulos(TCargoID cargo) const
{
    const auto it = m_mapa.find(SCargoInfo{cargo});
    return it == m_mapa.end() ? 0 : it->second.TotalQue(&CVoto::EhNulo);
}

TQtdVoto CVotosCargos::Brancos(TCargoID cargo) const
{
    const auto it = m_mapa.find(SCargoInfo{cargo});
    return it == m_mapa.end() ? 0 : it->second.TotalQue(&CVoto::EhBranco);
}

TQtdVoto CVotosCargos::Cargo(TCargoID cargo) const
{
    const auto it = m_mapa.find(SCargoInfo{cargo});
    return it == m_mapa.end() ? 0 : it->second.Total();
}

} // namespace comum::md

// <stubs>
// f5643 · 665 B · unmatched · name unknown
// ? f5643(i32, i32) { ... }

// f4454 · 5617 B · unmatched · name unknown
// void f4454(i32) { ... }
// </stubs>
