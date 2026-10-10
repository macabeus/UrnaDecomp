#include "app/comum/dados/crdvvota.h"

#include <string>

#include "ecourna/api/util/cstringutils.hpp"

namespace comum {

using ecourna::api::util::CStringUtils;

md::TQtdVoto CRdvVota::Candidato(md::TCargoID cargo, uedword numero, uebyte digitos) const
{
    const std::string numeroCandidato = CStringUtils::PreencheAEsquerda(std::to_string(numero), '0', digitos);
    return m_votos.Candidato(cargo, numeroCandidato);
}

} // namespace comum

// <stubs>
// f11487 · 707 B · unmatched · name unknown
// void f11487(i32, i32) { ... }
// </stubs>
