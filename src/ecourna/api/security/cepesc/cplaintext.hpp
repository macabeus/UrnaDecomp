#pragma once
#include <memory>
#include <vector>

#include "ecourna/api/security/cepesc/cinfosalt.hpp"
#include "ecourna/api/uetypes.hpp"

namespace ecourna::api::cepesc {

class CPlainText {
public:
    CPlainText(const uebyte tipoArquivo, const uebyte idCriptografia, const ueword identificadorUE, const ueword versao,
               std::vector<uebyte> tabelaCriptografiaUE, std::vector<uebyte> numeroAleatorioUE, std::vector<uebyte> chave,
               std::vector<uebyte> conteudo, std::shared_ptr<CInfoSalt> infoSalt);
    CPlainText(const uebyte tipoArquivo, const uebyte idCriptografia, const ueword identificadorUE, const ueword versao,
               std::vector<uebyte> tabelaCriptografiaUE, std::vector<uebyte> numeroAleatorioUE, std::vector<uebyte> chave,
               std::vector<uebyte> conteudo, const CInfoSalt& infoSalt);

    uebyte GetTipoArquivo() const { return m_tipoArquivo; }
    uebyte GetIdCriptografia() const { return m_idCriptografia; }
    ueword GetIdentificadorUE() const { return m_identificadorUE; }
    ueword GetVersao() const { return m_versao; }
    const std::vector<uebyte>& GetTabelaCriptografiaUE() const { return m_tabelaCriptografiaUE; }
    const std::vector<uebyte>& GetNumeroAleatorioUE() const { return m_numeroAleatorioUE; }
    const std::vector<uebyte>& GetChave() const { return m_chave; }
    const std::vector<uebyte>& GetConteudo() const { return m_conteudo; }
    const std::shared_ptr<CInfoSalt>& GetInfoSalt() const { return m_infoSalt; }

private:
    uebyte m_tipoArquivo;
    uebyte m_idCriptografia;
    ueword m_identificadorUE;
    ueword m_versao;
    std::vector<uebyte> m_tabelaCriptografiaUE;
    std::vector<uebyte> m_numeroAleatorioUE;
    std::vector<uebyte> m_chave;
    std::vector<uebyte> m_conteudo;
    std::shared_ptr<CInfoSalt> m_infoSalt;
};

} // namespace ecourna::api::cepesc
