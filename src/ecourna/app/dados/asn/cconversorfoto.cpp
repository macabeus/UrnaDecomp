#include "ecourna/app/dados/asn/cconversorfoto.hpp"

#include "ecourna/app/dados/asn/easnerror.hpp"

namespace ecourna::app::dados::asn {

// (original lines 7-21: content not recovered)
#line 22
ModuloTiposEleitorais::FormatoImagem::NamedNumber CConversorFoto::ConverteFormatoImagem(CFoto::EFormatoImagem formato) const
{
    switch (formato) {
    case CFoto::EFI_JPEG:
        return ModuloTiposEleitorais::FormatoImagem::formatoJPEG;
    case CFoto::EFI_BMP:
        return ModuloTiposEleitorais::FormatoImagem::formatoBMP;
    }
    throw CAsnError(EAE_FORMATO_FOTO_INVALIDO, "Código de formato da foto inválido.");
}


CFoto::EFormatoImagem CConversorFoto::DeconverteFormatoImagem(ModuloTiposEleitorais::FormatoImagem::NamedNumber formato) const
{
    switch (formato) {
    case ModuloTiposEleitorais::FormatoImagem::formatoJPEG:
        return CFoto::EFI_JPEG;
    case ModuloTiposEleitorais::FormatoImagem::formatoBMP:
        return CFoto::EFI_BMP;
    default:
        break;
    }
    throw CAsnError(EAE_FORMATO_FOTO_ENTIDADE_INVALIDO, "Código de formato da foto inválido.");
}

CConversorFoto::TEntidade CConversorFoto::DoConverte(const TDado& dado) const
{
    const ModuloTiposEleitorais::FormatoImagem formato = ConverteFormatoImagem(dado.GetFormato());
    TEntidade entidade;
    entidade.set_formato(formato);
    entidade.ref_imagem().insert(entidade.ref_imagem().begin(), dado.GetImagem().begin(), dado.GetImagem().end());
    return entidade;
}

CConversorFoto::TDado CConversorFoto::DoDeconverte(const TEntidade& entidade) const
{
    return CFoto(DeconverteFormatoImagem(entidade.get_formato()),
                 std::vector<uebyte>(entidade.get_imagem().begin(), entidade.get_imagem().end()));
}

} // namespace ecourna::app::dados::asn
