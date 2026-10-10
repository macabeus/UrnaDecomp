#pragma once
#include <algorithm>
#include <iterator>
#include <vector>

#include "ModuloInformacaoMidia.h"
#include "ecourna/api/asn/iconversorasn.hpp"
#include "ecourna/app/dados/midias/cinformacaomidia.hpp"

namespace ecourna::app::dados::asn {

class CConversorAplicativo;

// The application data of an ASN.1 sequence, one Deconverte per element.
template <typename TSequencia, typename TConversor>
std::vector<typename TConversor::TDado> DeconverteSequencia(const TSequencia& sequencia, const TConversor& conversor)
{
    std::vector<typename TConversor::TDado> dados;
    std::transform(sequencia.begin(), sequencia.end(), std::back_inserter(dados),
                   [&conversor](const typename TConversor::TEntidade& entidade) { return conversor.Deconverte(entidade); });
    return dados;
}

class CConversorInformacaoMidia
    : public api::asn::IConversorASN<ModuloInformacaoMidia::InformacaoMidia, CInformacaoMidia> {
protected:
    TEntidade DoConverte(const TDado& dado) const override;
    TDado DoDeconverte(const TEntidade& entidade) const override;

private:
    ModuloInformacaoMidia::TipoMidia ConverteTipoMidia(TDado::ETipoMidia tipoMidia) const;
    TDado::ETipoMidia DeconverteTipoMidia(ModuloInformacaoMidia::TipoMidia tipoMidia) const;

    static TEntidade::aplicativos::value_type ConverteAplicativos(const TVectorAplicativo& aplicativos,
                                                                  const CConversorAplicativo& conversor);
};

} // namespace ecourna::app::dados::asn
