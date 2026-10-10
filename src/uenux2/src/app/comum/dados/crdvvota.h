#pragma once

#include "app/comum/dados/crdv.h"
#include "app/comum/dados/md/rdv/cvotoseleicoesvota.h"

namespace comum {

class CRdvVota final : public CRdv {
public:
    static CRdvVota& GetInst();
    static void CreateInst();

    std::set<md::TCargoID> GetCargos() const override;
    const md::CVotosCargos& GetVotos(md::TEleicaoID eleicao) const override;
    md::TQtdVoto Comparecimento(md::TEleicaoID eleicao) const override;
    md::TQtdVoto Candidato(md::TCargoID cargo, uedword numero, uebyte digitos) const override;
    md::TQtdVoto Legenda(md::TCargoID cargo, int numero) const override;
    md::TQtdVoto Partido(md::TCargoID cargo, int numero) const override;
    md::TQtdVoto Nominais(md::TCargoID cargo) const override;
    md::TQtdVoto Legendas(md::TCargoID cargo) const override;
    md::TQtdVoto Nulos(md::TCargoID cargo) const override;
    md::TQtdVoto Brancos(md::TCargoID cargo) const override;
    md::TQtdVoto Cargo(md::TCargoID cargo) const override;
    std::vector<uebyte> Converte() const override;
    void Desconverte(const std::vector<uebyte>& dados) override;
    bool Confere(const std::vector<uebyte>& dados) const override;

private:
    md::CVotosEleicoesVota m_votos;
};

} // namespace comum
