// STAND-IN definitions for lzmasdkstub.hpp (see that header). Replace by the 7-Zip lane's wrapper.
#include "ecourna/api/compression/lzmasdkstub.hpp"

namespace lzma {

CContexto::CContexto() : opcoes{}, codecs(nullptr), codigo(0), modo(0), flag(0), reservado{} {}

CContexto::~CContexto() {}

bool Compacta(CContexto& contexto, const std::vector<std::string>& arquivos, const std::string& pacote,
              const std::vector<std::string>& nomes, bool comprimir,
              std::function<void(unsigned long long, const std::string&)> progresso)
{
    (void)arquivos;
    (void)pacote;
    (void)nomes;
    (void)comprimir;
    (void)progresso;
    contexto.codigo = 0;
    return true;
}

} // namespace lzma
