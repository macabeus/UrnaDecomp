#include "ecourna/app/dados/cfoto.hpp"

namespace ecourna::app::dados {

CFoto::CFoto(EFormatoImagem formato, const std::vector<uebyte>& imagem) : m_formato(formato), m_imagem(imagem)
{
}

} // namespace ecourna::app::dados
