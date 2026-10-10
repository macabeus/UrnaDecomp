// Pending members: the CConversorMunicipioZona member src/ecourna/app/dados/asn/cconversormunicipiozona.cpp does not
// hold yet. The class's vtable (emitted with DoConverte in that TU) needs it defined; its body is NOT the target's, it
// traps. Target function: DoDeconverte 9130 (vtable slot 3), which builds a CMunicipioZona through its out-of-line
// constructor (fn 5108) from TMunicipioID and TZonaID temporaries (fns 2851, 5850), none of them reconstructed yet.
// Move it into cconversormunicipiozona.cpp when it is reconstructed; delete this file with it.
#include "ecourna/app/dados/asn/cconversormunicipiozona.hpp"

namespace ecourna::app::dados::asn {

CMunicipioZona CConversorMunicipioZona::DoDeconverte(const ModuloTiposEleitorais::MunicipioZona&) const
{
    __builtin_trap();
}

} // namespace ecourna::app::dados::asn
