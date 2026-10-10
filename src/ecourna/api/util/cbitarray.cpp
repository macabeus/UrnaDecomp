#include "ecourna/api/util/cbitarray.hpp"

#include "ecourna/api/util/eutilerror.hpp"

namespace ecourna::api::util {

// (original lines 7-40: content not recovered)
#line 41
ueword CBitArray::GetValor(const uebyte bits)
{
    if (!m_escrita) {
        throw CUtilError(EUE_BITARRAY_SEM_ESCRITA, "sem permissao de escrita no array de bits");
    }
    ueword valor = GetValor(m_posicao, bits);
    m_posicao += bits;
    return valor;
}

// (original lines 51-52: content not recovered)
#line 53
ueword CBitArray::GetValor(const ueqword posicao, const uebyte bits) const
{
    if (m_bits == nullptr) {
        throw CUtilError(EUE_BITARRAY_NAO_ALOCADO, "array de bits nao alocado corretamente");
    }


    ueword valor = 0;
    if (bits > 0) {
        ueqword restantes = (posicao <= m_bits->size()) ? m_bits->size() - posicao : 0;
        if (bits - 1 >= restantes) {
            throw CUtilError(EUE_BITARRAY_POSICAO_INVALIDA, "busca no vetor em posicao invalida");
        }
        for (uebyte i = 0; i < bits; ++i) {
            if (m_bits->test(posicao + i)) {
                valor |= 1 << (bits - 1 - i);
            }
        }
    }
    return valor;
}

} // namespace ecourna::api::util

// <stubs>
// f5702 · 2565 B · unmatched · placement: link order · name unknown
// void f5702(i32, i32) { ... }
// </stubs>
