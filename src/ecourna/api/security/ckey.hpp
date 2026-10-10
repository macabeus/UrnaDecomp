#pragma once
#include <string>
#include <vector>

#include "ecourna/api/uetypes.hpp"

namespace ecourna::api::security {

enum KeyType {
    KT_SIMETRICA = 0,
    KT_PUBLICA,
    KT_PRIVADA,
};

class CKeyData {
public:
    explicit CKeyData(const std::vector<uebyte>& bytes);

    const std::vector<uebyte>& GetBytes() const { return m_bytes; }

private:
    std::vector<uebyte> m_bytes;
};

class CKey {
public:
    CKey(const std::string& titular, uedword id, KeyType tipo, const std::vector<uebyte>& bytes, const std::string& descricao);

    const std::string& GetTitular() const { return m_titular; }
    uedword GetId() const { return m_id; }
    KeyType GetTipo() const { return m_tipo; }
    const std::vector<uebyte>& GetBytes() const { return m_bytes; }
    const std::string& GetDescricao() const { return m_descricao; }

private:
    std::string m_titular;
    uedword m_id;
    KeyType m_tipo;
    std::vector<uebyte> m_bytes;
    std::string m_descricao;
    uedword m_reservado;
};

} // namespace ecourna::api::security
