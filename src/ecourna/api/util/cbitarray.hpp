#pragma once
#include <memory>
#include <vector>

#include <boost/dynamic_bitset.hpp>

#include "ecourna/api/uetypes.hpp"

namespace ecourna::api::util {

// A bit stream read MSB-first: GetValor(n) returns the next n bits as a number and advances the cursor.
class CBitArray {
public:
    CBitArray() : m_bits(std::make_shared<boost::dynamic_bitset<uebyte>>()), m_bytes(), m_escrita(true), m_leitura(false), m_posicao(0) {}
    virtual ~CBitArray() = default;

    ueword GetValor(const uebyte bits);
    ueword GetValor(const ueqword posicao, const uebyte bits) const;

private:
    std::shared_ptr<boost::dynamic_bitset<uebyte>> m_bits;
    std::vector<uebyte> m_bytes;
    bool m_escrita;
    bool m_leitura;
    ueqword m_posicao;
};

} // namespace ecourna::api::util
