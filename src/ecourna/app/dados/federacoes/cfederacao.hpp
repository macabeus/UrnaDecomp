#pragma once
#include <string>
#include <vector>

#include "ecourna/api/uetypes.hpp"
#include "ecourna/app/dados/tiposbasicos.hpp"

namespace ecourna::app::dados {

using TVectorNumeroPartido = std::vector<uebyte>;

// A party federation: its id, acronym, name and the numbers of its parties (never empty). 40 bytes.
class CFederacao {
public:
    CFederacao(TFederacaoID id, const std::string& sigla, const std::string& nome, const TVectorNumeroPartido& partidos);

    TFederacaoID GetId() const { return m_id; }
    const std::string& GetSigla() const { return m_sigla; }
    const std::string& GetNome() const { return m_nome; }
    const TVectorNumeroPartido& GetPartidos() const { return m_partidos; }

private:
    TFederacaoID m_id;
    std::string m_sigla;
    std::string m_nome;
    TVectorNumeroPartido m_partidos;
};

} // namespace ecourna::app::dados
