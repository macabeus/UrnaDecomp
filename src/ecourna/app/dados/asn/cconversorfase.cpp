#include "ecourna/app/dados/asn/cconversorfase.hpp"

#include "ecourna/app/dados/asn/easnerror.hpp"

namespace ecourna::app::dados::asn {

// (original lines 7-16: content not recovered)
#line 17
CConversorFase::TEntidade CConversorFase::DoConverte(const TDado& dado) const
{
    switch (dado.Get()) {
    case CFaseID::FASE_SIMULADO:
        return TEntidade::simulado;
    case CFaseID::FASE_OFICIAL:
        return TEntidade::oficial;
    case CFaseID::FASE_TREINAMENTO:
        return TEntidade::treinamento;
    case CFaseID::FASE_INDEFINIDA:
        throw CAsnError(EAE_FASE_INDEFINIDA, "Fase inválida.");
    }
    throw CAsnError(EAE_FASE_INVALIDA, "Fase inválida.");
}

CConversorFase::TDado CConversorFase::DoDeconverte(const TEntidade& entidade) const
{
    switch (entidade) {
    case TEntidade::simulado:
        return CFaseID::FASE_SIMULADO;
    case TEntidade::oficial:
        return CFaseID::FASE_OFICIAL;
    case TEntidade::treinamento:
        return CFaseID::FASE_TREINAMENTO;
    case TEntidade::unknownEnumeration_:
        throw CAsnError(EAE_FASE_DESCONHECIDA, "Fase inválida.");
    }
    throw CAsnError(EAE_FASE_ENTIDADE_INVALIDA, "Fase inválida.");
}

} // namespace ecourna::app::dados::asn
