#include "app/comum/dados/md/rdv/cvoto.h"

namespace comum::md {

bool CVoto::EhNulo(ETipo tipo)
{
    return tipo == NULO || tipo == NULO_APOS_SUSPENSAO || tipo == NULO_POR_REPETICAO;
}

bool CVoto::EhNominal() const
{
    return m_tipo == NOMINAL;
}

bool CVoto::EhLegenda() const
{
    return m_tipo == LEGENDA;
}

bool CVoto::EhNulo() const
{
    return EhNulo(m_tipo);
}

bool CVoto::EhBranco() const
{
    return m_tipo == BRANCO || m_tipo == BRANCO_APOS_SUSPENSAO;
}

} // namespace comum::md

// <stubs>
// f2256 · 1398 B · unmatched · name unknown
// ? f2256(i32, i32, i32) { ... }
// </stubs>
