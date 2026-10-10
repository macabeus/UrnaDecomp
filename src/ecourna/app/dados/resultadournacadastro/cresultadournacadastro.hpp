#pragma once
#include <optional>
#include <string>

#include "ecourna/app/dados/ccabecalhoentidade.hpp"
#include "ecourna/app/dados/cfaseid.hpp"
#include "ecourna/app/dados/resultadournacadastro/cdadoscomparecimento.hpp"
#include "ecourna/app/dados/resultadournacadastro/cdadoscomparecimentocifrado.hpp"

namespace ecourna::app::dados {

// The voter-register result of a voting machine: header (@0, 16 B), phase (@16), voting software version (@20),
// file state (@32) and either the attendance data (@36, 72 B, engaged @108) or its encrypted form (@112, 48 B,
// engaged @160). 168 bytes. The header is taken BY VALUE: CConversorResultadoUrnaCadastro::DoDeconverte (fn 9052)
// copies it into an argument temporary and again into the outgoing byval slot at frame offset 0. ESituacaoArquivo
// follows ModuloTiposEcoUrna::SituacaoArquivo; values synthesised.
class CResultadoUrnaCadastro {
public:
    enum ESituacaoArquivo {
        ArquivoFinal,
        ArquivoParcial,
    };

    CResultadoUrnaCadastro(CCabecalhoEntidade cabecalho, CFaseID fase, std::string versaoVotacao,
                           ESituacaoArquivo situacao, std::optional<CDadosComparecimento> dadosComparecimento);
    CResultadoUrnaCadastro(CCabecalhoEntidade cabecalho, CFaseID fase, std::string versaoVotacao,
                           ESituacaoArquivo situacao,
                           std::optional<CDadosComparecimentoCifrado> dadosComparecimentoCifrado);

    const CCabecalhoEntidade& GetCabecalho() const { return m_cabecalho; }
    CFaseID GetFase() const { return m_fase; }
    const std::string& GetVersaoVotacao() const { return m_versaoVotacao; }
    ESituacaoArquivo GetSituacao() const { return m_situacao; }
    bool HasDadosComparecimento() const { return m_dadosComparecimento.has_value(); }
    const CDadosComparecimento& GetDadosComparecimento() const;
    bool HasDadosComparecimentoCifrado() const { return m_dadosComparecimentoCifrado.has_value(); }
    const CDadosComparecimentoCifrado& GetDadosComparecimentoCifrado() const;

private:
    CCabecalhoEntidade m_cabecalho;
    CFaseID m_fase;
    std::string m_versaoVotacao;
    ESituacaoArquivo m_situacao;
    std::optional<CDadosComparecimento> m_dadosComparecimento;
    std::optional<CDadosComparecimentoCifrado> m_dadosComparecimentoCifrado;
};

} // namespace ecourna::app::dados
