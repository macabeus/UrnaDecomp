#include "ecourna/app/dados/asn/midias/cconversordadosgeracaomidia.hpp"

#include "ecourna/app/dados/asn/cconversoridentificadorgeradormidia.hpp"
#include "ecourna/app/dados/asn/datahoraasn.hpp"

namespace ecourna::app::dados::asn {

ModuloInformacaoMidia::DadosGeracaoMidia CConversorDadosGeracaoMidia::DoConverte(const TDado& dado) const
{
    CConversorIdentificadorGeradorMidia conversorIdentificador;
    TEntidade entidade;
    entidade.set_serialMidia() = dado.GetSerialMidia().ToString();
    entidade.set_usuario() = dado.GetUsuario();
    entidade.set_identificadorGeradorMidia() = conversorIdentificador.Converte(dado.GetIdentificadorGeradorMidia());
    entidade.set_data(ConverteDataHora(dado.GetData()));
    return entidade;
}

CDadosGeracaoMidia CConversorDadosGeracaoMidia::DoDeconverte(const TEntidade& entidade) const
{
    CConversorIdentificadorGeradorMidia conversorIdentificador;
    return CDadosGeracaoMidia(CSerialMidia(entidade.get_serialMidia()), entidade.get_usuario(),
                              conversorIdentificador.Deconverte(entidade.get_identificadorGeradorMidia()),
                              DesconverteDataHora(entidade.get_data()));
}

} // namespace ecourna::app::dados::asn
