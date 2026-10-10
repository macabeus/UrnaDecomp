#include "ecourna/app/dados/midias/caplicativo.hpp"

#include "ecourna/app/dados/midias/edadosmidiaserror.hpp"

namespace ecourna::app::dados {

CAplicativo::CAplicativo(ETipoAplicativo tipo, const CAutenticacao& autenticacao)
    : m_tipo(tipo)
    , m_autenticacao(autenticacao)
{
}

// (original lines 13-24: content not recovered)
#line 25
const CAutenticacao& CAplicativo::GetAutenticacao() const
{
    if (!m_autenticacao.has_value()) {
        throw CDadosMidiasError(EDME_AUTENTICACAO_NAO_DEFINIDA, "Autenticação não definida para este aplicativo.");
    }
    return *m_autenticacao;
}

} // namespace ecourna::app::dados
