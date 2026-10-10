#include "ecourna/api/security/cepesc/cplaintext.hpp"

#include <utility>

#include "ecourna/api/security/esecurityerror.hpp"

namespace ecourna::api::cepesc {

using security::CSecurityError;

// (original lines 11-74: content not recovered)
#line 75
CPlainText::CPlainText(const uebyte tipoArquivo, const uebyte idCriptografia, const ueword identificadorUE, const ueword versao,
                       std::vector<uebyte> tabelaCriptografiaUE, std::vector<uebyte> numeroAleatorioUE, std::vector<uebyte> chave,
                       std::vector<uebyte> conteudo, std::shared_ptr<CInfoSalt> infoSalt)
    : m_tipoArquivo(tipoArquivo),
      m_idCriptografia(idCriptografia),
      m_identificadorUE(identificadorUE),
      m_versao(versao),
      m_tabelaCriptografiaUE(std::move(tabelaCriptografiaUE)),
      m_numeroAleatorioUE(std::move(numeroAleatorioUE)),
      m_chave(std::move(chave)),
      m_conteudo(std::move(conteudo)),
      m_infoSalt(std::move(infoSalt))
{
    if (tipoArquivo > 2) {
        throw CSecurityError(security::ESE_PLAIN_TEXT_TIPO_ARQUIVO, "tipo do arquivo inválido.");
    }
    if (idCriptografia > 3) {
        throw CSecurityError(security::ESE_PLAIN_TEXT_ID_CRIPTOGRAFIA, "id da criptografia inválido.");
    }
    if (m_tabelaCriptografiaUE.empty()) {
        throw CSecurityError(security::ESE_PLAIN_TEXT_TABELA_VAZIA, "tabela de criptografia da UE vazia.");
    }
    if (m_numeroAleatorioUE.empty()) {
        throw CSecurityError(security::ESE_PLAIN_TEXT_NUMERO_ALEATORIO_VAZIO, "número aleatório da UE vazio.");
    }
    if (m_chave.empty()) {
        throw CSecurityError(security::ESE_PLAIN_TEXT_CHAVE_VAZIA, "chave vazia.");
    }
    if (m_conteudo.empty()) {
        throw CSecurityError(security::ESE_PLAIN_TEXT_CONTEUDO_VAZIO, "conteúdo vazio.");
    }
}

CPlainText::CPlainText(const uebyte tipoArquivo, const uebyte idCriptografia, const ueword identificadorUE, const ueword versao,
                       std::vector<uebyte> tabelaCriptografiaUE, std::vector<uebyte> numeroAleatorioUE, std::vector<uebyte> chave,
                       std::vector<uebyte> conteudo, const CInfoSalt& infoSalt)
    : CPlainText(tipoArquivo, idCriptografia, identificadorUE, versao, std::move(tabelaCriptografiaUE), std::move(numeroAleatorioUE),
                 std::move(chave), std::move(conteudo), std::make_shared<CInfoSalt>(infoSalt))
{
}

} // namespace ecourna::api::cepesc
