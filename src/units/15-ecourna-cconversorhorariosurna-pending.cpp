// Pending members: the ecourna::app::dados::asn::CConversorHorariosUrna members
// src/ecourna/app/dados/asn/cconversorhorariosurna.cpp does not hold yet. DoConverte is the class's key function, so
// the vtable is emitted with it here; its body is NOT the target's, it traps. Target function: DoConverte 9133
// (vtable slot 2). Move it into cconversorhorariosurna.cpp when it is reconstructed; delete this file with it.
#include "ecourna/app/dados/asn/cconversorhorariosurna.hpp"

namespace ecourna::app::dados::asn {

ModuloConfiguracaoMunicipios::HorariosUrna CConversorHorariosUrna::DoConverte(const CHorariosUrna&) const
{
    __builtin_trap();
}

} // namespace ecourna::app::dados::asn
