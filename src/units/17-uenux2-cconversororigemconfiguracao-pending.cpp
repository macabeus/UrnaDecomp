// Pending members: the CConversorOrigemConfiguracao member
// src/uenux2/src/app/comum/dados/asn/processoeleitoral/cconversororigemconfiguracao.cpp does not hold yet. The class's
// vtable (emitted with DoConverte in that TU) needs it defined; its body is NOT the target's, it traps. Target function:
// DoDesconverte 11366 (vtable slot 3), whose record puts a throw on line 38 col 11 of that file; it calls a helper
// (fn 6030) with that record that is not reconstructed yet.
// Move it into cconversororigemconfiguracao.cpp when it is reconstructed; delete this file with it.
#include "app/comum/dados/asn/processoeleitoral/cconversororigemconfiguracao.h"

namespace comum::asn {

md::EOrigemConfiguracao CConversorOrigemConfiguracao::DoDesconverte(const ModuloProcessoEleitoral::OrigemConfiguracao&) const
{
    __builtin_trap();
}

} // namespace comum::asn
