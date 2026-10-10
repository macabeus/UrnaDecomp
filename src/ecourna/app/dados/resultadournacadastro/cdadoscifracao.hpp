#pragma once
#include <vector>

#include "ecourna/api/uetypes.hpp"

namespace ecourna::app::dados {

// The encryption parameters of the encrypted attendance data: key, salt and additional information (both >= 16
// bytes). 36 bytes: three byte vectors.
class CDadosCifracao {
public:
    CDadosCifracao(std::vector<uebyte> chave, std::vector<uebyte> salt, std::vector<uebyte> informacaoAdicional);

    const std::vector<uebyte>& GetChave() const { return m_chave; }
    const std::vector<uebyte>& GetSalt() const { return m_salt; }
    const std::vector<uebyte>& GetInformacaoAdicional() const { return m_informacaoAdicional; }

private:
    std::vector<uebyte> m_chave;
    std::vector<uebyte> m_salt;
    std::vector<uebyte> m_informacaoAdicional;
};

} // namespace ecourna::app::dados
