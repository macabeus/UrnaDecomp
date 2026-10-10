#include "ecourna/api/compression/clzmacompress.hpp"

#include <format>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

#include "ecourna/api/compression/ecompressionerror.hpp"
#include "ecourna/api/compression/lzmasdkstub.hpp"

namespace ecourna::api::compression {

namespace {

std::unique_ptr<lzma::CContexto> s_contexto;
std::mutex s_mutex;

} // namespace

void CLzmaCompress::AtualizaProgresso(unsigned long long percentual, const std::string& arquivo)
{
    NotificarProgresso(percentual, 100, std::format("Compactando arquivo {}...", arquivo));
}

void CLzmaCompress::DoAdd(const std::filesystem::path& arquivo, const std::filesystem::path& nome)
{
    {
        std::lock_guard<std::mutex> trava(s_mutex);
        if (!s_contexto) {
            s_contexto = std::make_unique<lzma::CContexto>();
        }
    }

    // (original lines 35-108: content not recovered)
#line 109
    std::vector<std::string> arquivos{arquivo.string()};
    const bool manterNome = arquivo.filename() == nome && arquivo.is_absolute();
    std::vector<std::string> nomes = manterNome ? std::vector<std::string>() : std::vector<std::string>{nome.string()};
    if (!lzma::Compacta(*s_contexto, arquivos, m_pacote, nomes, m_nivel != ENivelCompressao::NENHUM,
                        std::bind(&CLzmaCompress::AtualizaProgresso, this, std::placeholders::_1, std::placeholders::_2))) {
        throw CCompressionError(ECE_LZMA_INCLUIR_ARQUIVO, std::format("(0x{:X} - {}) Não foi possível incluir o arquivo {} no pacote {}.", s_contexto->codigo, s_contexto->mensagem, arquivo.string(), m_pacote));
    }
}

} // namespace ecourna::api::compression
