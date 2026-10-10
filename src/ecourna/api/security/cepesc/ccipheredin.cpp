#include "ecourna/api/security/cepesc/ccipheredin.hpp"

#include "ecourna/api/security/esecurityerror.hpp"

namespace ecourna::api::cepesc {

using security::CSecurityError;

// (original lines 9-61: content not recovered)
#line 62
CCipheredIn::CCipheredIn(uebyte tipoArquivo, uebyte idCriptografia, uebyte versao, const std::vector<uebyte>& chaveSessao,
                         const std::vector<uebyte>& chaveSecreta, const std::vector<uebyte>& conteudo, const CInfoSalt& infoSalt)
    : m_tipoArquivo(tipoArquivo),
      m_idCriptografia(idCriptografia),
      m_versao(versao),
      m_chaveSessao(chaveSessao),
      m_chaveSecreta(chaveSecreta),
      m_conteudo(conteudo),
      m_infoSalt(new CInfoSalt(infoSalt))
{
    if (tipoArquivo > 2) {
        throw CSecurityError(security::ESE_CIPHERED_IN_TIPO_ARQUIVO, "tipo do arquivo inválido.");
    }
    if (idCriptografia > 3) {
        throw CSecurityError(security::ESE_CIPHERED_IN_ID_CRIPTOGRAFIA, "id da criptografia inválido.");
    }
    if (chaveSessao.empty()) {
        throw CSecurityError(security::ESE_CIPHERED_IN_CHAVE_SESSAO_VAZIA, "chave de sessão vazia.");
    }
    if (chaveSecreta.empty()) {
        throw CSecurityError(security::ESE_CIPHERED_IN_CHAVE_SECRETA_VAZIA, "chave secreta vazia.");
    }
    if (conteudo.empty()) {
        throw CSecurityError(security::ESE_CIPHERED_IN_CONTEUDO_VAZIO, "conteúdo vazio.");
    }
}

} // namespace ecourna::api::cepesc
