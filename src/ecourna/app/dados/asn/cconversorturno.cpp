#include "ecourna/app/dados/asn/cconversorturno.hpp"

#include "ecourna/app/dados/asn/easnerror.hpp"

namespace ecourna::app::dados::asn {

// (original lines 7-16: content not recovered)
#line 17
ModuloTiposEleitorais::Turno CConversorTurno::DoConverte(const TDado& dado) const
{
    switch (dado) {
    case 1:
        return TEntidade::turno1;
    case 2:
        return TEntidade::turno2;
    default:
        break;
    }
    throw CAsnError(EAE_TURNO_INVALIDO, "Turno inválido.");
}


app::dados::TTurno CConversorTurno::DoDeconverte(const TEntidade& entidade) const
{
    switch (entidade) {
    case TEntidade::turno1:
        return 1;
    case TEntidade::turno2:
        return 2;
    default:
        break;
    }
    throw CAsnError(EAE_TURNO_ENTIDADE_INVALIDO, "Turno inválido.");
}

} // namespace ecourna::app::dados::asn
