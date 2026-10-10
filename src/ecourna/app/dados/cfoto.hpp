#pragma once
#include <vector>

#include "ecourna/api/uetypes.hpp"

namespace ecourna::app::dados {

// A photo: its format and the image bytes. 16 bytes. Layout from fn 5111 (the constructor) and the converter
// (app/dados/asn/cconversorfoto.cpp, fns 9227/9226); enumerator names synthesised.
class CFoto {
public:
    enum EFormatoImagem {
        EFI_JPEG = 1,
        EFI_BMP,
    };

    CFoto(EFormatoImagem formato, const std::vector<uebyte>& imagem);

    EFormatoImagem GetFormato() const { return m_formato; }
    const std::vector<uebyte>& GetImagem() const { return m_imagem; }

private:
    EFormatoImagem m_formato;
    std::vector<uebyte> m_imagem;
};

} // namespace ecourna::app::dados
