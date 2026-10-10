#include "ecourna/api/security/ckey.hpp"

#include "ecourna/api/security/esecurityerror.hpp"

namespace ecourna::api::security {

CKeyData::CKeyData(const std::vector<uebyte>& bytes)
{

    // (original lines 10-29: content not recovered)
#line 30
    if (bytes.empty()) {
        throw CSecurityError(ESE_KEY_DATA_BYTES_VAZIO, "Bytes da chave vazio.");
    }

    m_bytes = bytes;
}

CKey::CKey(const std::string& titular, uedword id, KeyType tipo, const std::vector<uebyte>& bytes, const std::string& descricao)
    : m_titular(titular)
    , m_id(id)
    , m_tipo(tipo)
    , m_bytes(bytes)
    , m_descricao(descricao)
    , m_reservado(0)
{

    // (original lines 46-53: content not recovered)
#line 54
    if (m_titular.empty()) {
        throw CSecurityError(ESE_KEY_TITULAR_VAZIO, "Titular da chave vazio.");
    }
    if (m_id == 0) {
        throw CSecurityError(ESE_KEY_ID_ZERO, "Identificador do par de chaves igual a zero.");
    }
    if (m_bytes.empty()) {
        throw CSecurityError(ESE_KEY_BYTES_VAZIO, "Bytes da chave vazio.");
    }
}

} // namespace ecourna::api::security
