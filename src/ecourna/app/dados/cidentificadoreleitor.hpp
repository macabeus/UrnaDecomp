#pragma once
#include <memory>
#include <string>

#include "ecourna/app/dados/cnumerocpf.hpp"
#include "ecourna/app/dados/cnumeroidentificacaolivre.hpp"
#include "ecourna/app/dados/cnumeroinscricaoeleitoral.hpp"
#include "ecourna/app/dados/iidentificadoreleitor.hpp"

namespace ecourna::app::dados {

// A voter identifier of any kind: the IIdentificadorEleitor its type creates. 8 bytes (one shared_ptr). Layout and the
// inline creation from its converter (app/dados/asn/cconversoridentificadoreleitor.cpp, fns 9117/9116): a type outside
// 1..3 leaves the pointer empty.
class CIdentificadorEleitor {
public:
    CIdentificadorEleitor(ETipoIdentificadorEleitor tipo, const std::string& numero)
        : m_identificador(Cria(tipo, numero))
    {
    }

    // Out of line (cidentificadoreleitor.cpp, fn 5172): CConversorRegistroIdentificacaoEleitor::DoConverte (9114) and
    // vota's CGeraBU (12110) call it.
    CIdentificadorEleitor(TSharedIdentificadorEleitor identificador);

    const TSharedIdentificadorEleitor& Get() const { return m_identificador; }

private:
    static TSharedIdentificadorEleitor Cria(TTipoIdentificadorEleitor tipo, const std::string& numero)
    {
        switch (tipo) {
            case TipoIDInscricaoEleitoral:
                return std::make_shared<CNumeroInscricaoEleitoral>(numero);
            case TipoIDCPF:
                return std::make_shared<CNumeroCPF>(numero);
            case TipoIDNumeroLivre:
                return std::make_shared<CNumeroIdentificacaoLivre>(numero);
        }
        return TSharedIdentificadorEleitor();
    }

    TSharedIdentificadorEleitor m_identificador;
};

} // namespace ecourna::app::dados
