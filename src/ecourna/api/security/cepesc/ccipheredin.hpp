#pragma once
#include <memory>
#include <vector>

#include "ecourna/api/security/cepesc/cinfosalt.hpp"
#include "ecourna/api/uetypes.hpp"

namespace ecourna::api::cepesc {

class CCipheredIn {
public:
    CCipheredIn(uebyte tipoArquivo, uebyte idCriptografia, uebyte versao, const std::vector<uebyte>& chaveSessao,
                const std::vector<uebyte>& chaveSecreta, const std::vector<uebyte>& conteudo, const CInfoSalt& infoSalt);

    uebyte GetTipoArquivo() const { return m_tipoArquivo; }
    uebyte GetIdCriptografia() const { return m_idCriptografia; }
    uebyte GetVersao() const { return m_versao; }
    const std::vector<uebyte>& GetChaveSessao() const { return m_chaveSessao; }
    const std::vector<uebyte>& GetChaveSecreta() const { return m_chaveSecreta; }
    const std::vector<uebyte>& GetConteudo() const { return m_conteudo; }
    const std::shared_ptr<CInfoSalt>& GetInfoSalt() const { return m_infoSalt; }

private:
    uebyte m_tipoArquivo;
    uebyte m_idCriptografia;
    uebyte m_versao;
    std::vector<uebyte> m_chaveSessao;
    std::vector<uebyte> m_chaveSecreta;
    std::vector<uebyte> m_conteudo;
    std::shared_ptr<CInfoSalt> m_infoSalt;
};

} // namespace ecourna::api::cepesc
