// api::CBmpConversor (/home/rubio/tse/uenux2/src/api/gui/cbmpconversor.cpp), two static members whose throws are
// records of the target, so their positions in this file are fixed:
//   3664  VerticalFlip(unsigned char*, const int, const int)   records at lines 23 and 27, column 15
//   3663  InvertColors(unsigned char*, const int, const int)   record at line 60, column 15

#include "api/gui/cbmpconversor.h"

#include <cstring>
#include <vector>

#include "api/gui/eueguierror.h"

namespace api {

/**
 * Espelha uma imagem verticalmente, no lugar: troca a primeira linha com a
 * última, a segunda com a penúltima, e assim por diante.
 * @throw CUeGuiError se @p data for nulo ou as dimensões forem inválidas
 */
void CBmpConversor::VerticalFlip(unsigned char* data, const int width, const int height)
{
    if (!data) {
        throw CUeGuiError(EUE_GUI_VERTICALFLIP_DADO_NULO, "Dado de entrada nulo");
    }

    if (width * height < 0) {
        throw CUeGuiError(EUE_GUI_VERTICALFLIP_DIMENSOES_INVALIDAS, "Dimensões inválidas");
    }

    std::vector<unsigned char> linha(width);

    for (int y = 0; y < height / 2; ++y) {
        unsigned char* topo = data + y * width;
        unsigned char* base = data + (height - (y + 1)) * width;

        std::memcpy(linha.data(), topo, width);
        std::memcpy(topo, base, width);
        std::memcpy(base, linha.data(), width);
    }
}

// (original lines 42-46: content not recovered)
#line 47
/**
 * Inverte as cores de uma imagem, no lugar: cada um dos @p width * @p height
 * bytes de @p data passa a ser o seu complemento.
 *
 * @param data pixels da imagem, um byte por pixel
 * @param width largura da imagem em pixels
 * @param height altura da imagem em pixels
 *
 * @throw CUeGuiError se @p data for nulo
 */
void CBmpConversor::InvertColors(unsigned char* data, const int width, const int height)
{
    if (!data) {
        throw CUeGuiError(EUE_GUI_INVERTCOLORS_DADO_NULO, "Dado de entrada nulo");
    }

    for (int i = 0; i < width * height; ++i) {
        data[i] = ~data[i];
    }
}

} // namespace api
