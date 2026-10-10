#include "ecourna/app/dados/asn/midias/cconversorautenticacao.hpp"

#include "ecourna/app/dados/asn/datahoraasn.hpp"

namespace ecourna::app::dados::asn {

ModuloInformacaoMidia::Autenticacao CConversorAutenticacao::DoConverte(const TDado& dado) const
{
    TEntidade entidade;
    if (dado.PossuiDataHora()) {
        entidade.set_dataInicial(ConverteDataHora(dado.GetDataHoraInicial()));
        entidade.set_dataFinal(ConverteDataHora(dado.GetDataHoraFinal()));
    } else {
        entidade.omit_dataInicial();
        entidade.omit_dataFinal();
    }
    entidade.set_numeroTentativas(dado.GetNumeroTentativas());
    entidade.set_tamanhoSenha(dado.GetTamanhoSenha());
    entidade.ref_hashSenha().insert(entidade.ref_hashSenha().begin(), dado.GetHashSenha().begin(),
                                    dado.GetHashSenha().end());
    return entidade;
}

CAutenticacao CConversorAutenticacao::DoDeconverte(const TEntidade& entidade) const
{
    const uedword numeroTentativas = entidade.get_numeroTentativas().getValue();
    const uedword tamanhoSenha = entidade.get_tamanhoSenha().getValue();
    const std::vector<uebyte> hashSenha(entidade.get_hashSenha().begin(), entidade.get_hashSenha().end());
    if (entidade.dataInicial_isPresent() && entidade.dataFinal_isPresent()) {
        return CAutenticacao(DesconverteDataHora(entidade.get_dataInicial()), DesconverteDataHora(entidade.get_dataFinal()),
                             numeroTentativas, tamanhoSenha, hashSenha);
    }
    return CAutenticacao(numeroTentativas, tamanhoSenha, hashSenha);
}

} // namespace ecourna::app::dados::asn
