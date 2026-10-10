#include "app/comum/dados/md/rdv/cvotos.h"

namespace comum::md {

TQtdVoto CVotos::Conta(TCriterio criterio) const
{
    TQtdVoto total = 0;
    for (const CVoto& voto : m_votos) {
        total += criterio(voto);
    }
    return total;
}

TQtdVoto CVotos::Candidato(const std::string& numero) const
{
    return Conta([&numero](const CVoto& voto) { return voto.GetTipo() == CVoto::NOMINAL && voto.GetNumero() == numero; });
}

TQtdVoto CVotos::Legenda(const std::string& numero) const
{
    return Conta([&numero](const CVoto& voto) {
        return voto.GetTipo() == CVoto::LEGENDA && voto.GetNumero().compare(0, numero.size(), numero) == 0;
    });
}

TQtdVoto CVotos::Partido(const std::string& numero) const
{
    return Conta([&numero](const CVoto& voto) {
        return (voto.GetTipo() == CVoto::LEGENDA || voto.GetTipo() == CVoto::NOMINAL) &&
               voto.GetNumero().compare(0, numero.size(), numero) == 0;
    });
}

TQtdVoto CVotos::TotalQue(bool (CVoto::*criterio)() const) const
{
    return Conta([&criterio](const CVoto& voto) { return (voto.*criterio)(); });
}

} // namespace comum::md
