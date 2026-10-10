#pragma once
#include <vector>

#include "ecourna/app/dados/cregistroidentificacaoeleitor.hpp"
#include "ecourna/app/dados/tiposbasicos.hpp"

namespace ecourna::app::dados {

// A justified absence: the voter (@0, 20 B) and the voter's birth year (@20). 24 bytes (the vector element the
// converters destroy in 24-byte steps). Header-only: no function of the class exists in the target.
class CIdentificacaoJustificativa {
public:
    CIdentificacaoJustificativa(const CRegistroIdentificacaoEleitor& identificacaoEleitor, TAno anoNascimentoEleitor)
        : m_identificacaoEleitor(identificacaoEleitor), m_anoNascimentoEleitor(anoNascimentoEleitor)
    {
    }

    const CRegistroIdentificacaoEleitor& GetIdentificacaoEleitor() const { return m_identificacaoEleitor; }
    TAno GetAnoNascimentoEleitor() const { return m_anoNascimentoEleitor; }

private:
    CRegistroIdentificacaoEleitor m_identificacaoEleitor;
    TAno m_anoNascimentoEleitor;
};

using TVectorIdentificacaoJustificativa = std::vector<CIdentificacaoJustificativa>;

} // namespace ecourna::app::dados
