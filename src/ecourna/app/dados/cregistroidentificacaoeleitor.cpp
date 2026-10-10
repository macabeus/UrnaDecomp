#include "ecourna/app/dados/cregistroidentificacaoeleitor.hpp"

#include "ecourna/app/dados/edadoserror.hpp"

namespace ecourna::app::dados {

// (original lines 7-18: content not recovered)
#line 19
CRegistroIdentificacaoEleitor::CRegistroIdentificacaoEleitor(TSharedIdentificadorEleitor identificadorHabilitacao)
    : m_identificadorHabilitacao(identificadorHabilitacao)
{
    if (!m_identificadorHabilitacao) {
        throw CDadosError(EDE_REGISTRO_SEM_HABILITACAO, "O identificador de habilitação do eleitor precisa estar definido.");
    }
}

// (original lines 27-31: content not recovered)
#line 32
CRegistroIdentificacaoEleitor::CRegistroIdentificacaoEleitor(TSharedIdentificadorEleitor identificadorHabilitacao,
                                                             TSharedIdentificadorEleitor identificadorPrincipal)
    : m_identificadorHabilitacao(identificadorHabilitacao), m_identificadorPrincipal(identificadorPrincipal)
{
    if (!m_identificadorHabilitacao) {
        throw CDadosError(EDE_REGISTRO_SEM_HABILITACAO_COM_PRINCIPAL,
                          "O identificador de habilitação do eleitor precisa estar definido.");
    }
}


TSharedIdentificadorEleitor CRegistroIdentificacaoEleitor::GetIdentificadorHabilitacao() const
{
    if (!m_identificadorHabilitacao) {
        throw CDadosError(EDE_IDENTIFICADOR_HABILITACAO_NAO_DEFINIDO, "O identificador de habilitação do eleitor não está definido.");
    }
    return m_identificadorHabilitacao;
}

// (original lines 51-52: content not recovered)
#line 53
TSharedIdentificadorEleitor CRegistroIdentificacaoEleitor::GetIdentificadorPrincipal() const
{
    if (!m_identificadorPrincipal) {
        throw CDadosError(EDE_IDENTIFICADOR_PRINCIPAL_NAO_DEFINIDO, "O identificador principal do eleitor não está definido.");
    }
    return *m_identificadorPrincipal;
}

} // namespace ecourna::app::dados
