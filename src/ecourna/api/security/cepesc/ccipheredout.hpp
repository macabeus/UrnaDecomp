#pragma once
#include <memory>
#include <vector>

#include "ecourna/api/security/cepesc/cinfosalt.hpp"
#include "ecourna/api/uetypes.hpp"

namespace ecourna::api::cepesc {

class CCipheredOut {
public:
    CCipheredOut(const std::vector<uebyte>& chave, const std::vector<uebyte>& conteudo);
    CCipheredOut(const std::vector<uebyte>& chave, const std::vector<uebyte>& conteudo, const CInfoSalt& infoSalt);

    const std::vector<uebyte>& GetChave() const { return m_chave; }
    const std::vector<uebyte>& GetConteudo() const { return m_conteudo; }
    const std::shared_ptr<CInfoSalt>& GetInfoSalt() const { return m_infoSalt; }

private:
    std::vector<uebyte> m_chave;
    std::vector<uebyte> m_conteudo;
    std::shared_ptr<CInfoSalt> m_infoSalt;
};

} // namespace ecourna::api::cepesc
