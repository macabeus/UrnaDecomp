#pragma once
#include <vector>

#include "ecourna/api/uetypes.hpp"
#include "ecourna/app/dados/resultadournacadastro/cdadoscifracao.hpp"

namespace ecourna::app::dados {

// The encrypted attendance data: how it was encrypted (@0, 36 B) and the ciphertext (@36). 48 bytes.
class CDadosComparecimentoCifrado {
public:
    CDadosComparecimentoCifrado(CDadosCifracao dadosCifracao, std::vector<uebyte> conteudo);

    const CDadosCifracao& GetDadosCifracao() const { return m_dadosCifracao; }
    const std::vector<uebyte>& GetConteudo() const { return m_conteudo; }

private:
    CDadosCifracao m_dadosCifracao;
    std::vector<uebyte> m_conteudo;
};

} // namespace ecourna::app::dados
