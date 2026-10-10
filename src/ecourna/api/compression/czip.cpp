#include "ecourna/api/compression/czip.hpp"

#include <chrono>
#include <ctime>
#include <format>

#include "ecourna/api/compression/ecompressionerror.hpp"
#include "ecourna/api/io/cfile.hpp"

namespace ecourna::api::compression {

namespace {

const int TAMANHO_BUFFER = 1024;
} // namespace
std::string CZip::ConvertToErrorMessage(int erro)
{
    switch (erro) {
    case ZIP_OK: return "";
    case Z_ERRNO: return "Erro de arquivo.";
    case Z_STREAM_ERROR: return "Erro de leitura/escrita.";
    case Z_DATA_ERROR: return "Erro de dados.";
    case Z_MEM_ERROR: return "Memória insuficiente.";
    case Z_BUF_ERROR: return "Erro de buffer.";
    case Z_VERSION_ERROR: return "Versão incompatível.";
    case ZIP_PARAMERROR: return "Parâmetro incorreto.";
    case ZIP_BADZIPFILE: return "Arquivo inválido.";
    case ZIP_INTERNALERROR: return "Erro interno.";
    case -105: return "Erro de CRC.";
    case -100: return "Arquivo não encontrado.";
    }
    return "Erro desconhecido.";
}

// (original lines 35-132: content not recovered)
#line 133
void CZip::Close()
{
    if (m_zip != nullptr) {
        const int erro = zipClose(m_zip, nullptr);
        m_zip = nullptr;
        if (erro != ZIP_OK) {
            throw CCompressionError(ECE_ZIP_FECHAR, std::format("O arquivo {} não pode ser fechado. {}", m_arquivo, ConvertToErrorMessage(erro)));
        }
    }
}

// (original lines 144-148: content not recovered)
#line 149
int CZip::ConvertToOpen()
{
    switch (m_modo) {
    case EModoAbertura::CRIAR: return APPEND_STATUS_CREATE;
    case EModoAbertura::ADICIONAR: return APPEND_STATUS_ADDINZIP;
    }
    throw CCompressionError(ECE_ZIP_MODO_ABERTURA, "Modo de abertura do zip incorreto.");
}

// (original lines 158-165: content not recovered)
#line 166
void CZip::CreateZipFile()
{
    m_zip = zipOpen64(m_arquivo.c_str(), ConvertToOpen());
    if (m_zip == nullptr) {
        throw CCompressionError(ECE_ZIP_CRIAR, std::format("O arquivo {} não pode ser criado.", m_arquivo));
    }
}

void CZip::SafeClose()
{
    try {
        CZip::Close();
    } catch (...) {
    }
}

// (original lines 182-184: content not recovered)
#line 185
void CZip::AssertZipIsOpened()
{
    if (m_zip == nullptr) {
        throw CCompressionError(ECE_ZIP_FECHADO, std::format("O arquivo {} está fechado.", m_arquivo));
    }
}

// (original lines 192-198: content not recovered)
#line 199
int CZip::ConvertToCompressLevel()
{
    switch (m_nivel) {
    case ENivelCompressao::NENHUM: return Z_NO_COMPRESSION;
    case ENivelCompressao::RAPIDO: return Z_BEST_SPEED;
    case ENivelCompressao::PADRAO: return Z_DEFAULT_COMPRESSION;
    case ENivelCompressao::MAXIMO: return Z_BEST_COMPRESSION;
    }
    throw CCompressionError(ECE_ZIP_MODO_COMPRESSAO, "Modo de compressão incorreto.");
}

void CZip::DoAdd(const std::filesystem::path& arquivo, const std::filesystem::path& nome)
{
    AssertZipIsOpened();

    zip_fileinfo informacoes{};
    const std::time_t dataHora = std::chrono::system_clock::to_time_t(std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        std::chrono::file_clock::to_sys(std::filesystem::last_write_time(arquivo))));
    const std::tm* data = std::localtime(&dataHora);
    informacoes.tmz_date.tm_sec = data->tm_sec;
    informacoes.tmz_date.tm_min = data->tm_min;
    informacoes.tmz_date.tm_hour = data->tm_hour;
    informacoes.tmz_date.tm_mday = data->tm_mday;
    informacoes.tmz_date.tm_mon = data->tm_mon;
    informacoes.tmz_date.tm_year = data->tm_year;
    const std::uintmax_t tamanho = std::filesystem::file_size(arquivo);

    // (original lines 226-239: content not recovered)
#line 240
    int erro = zipOpenNewFileInZip2_64(m_zip, nome.string().c_str(), &informacoes, nullptr, 0, nullptr, 0, nullptr,
                                       m_nivel != ENivelCompressao::NENHUM ? Z_DEFLATED : 0, ConvertToCompressLevel(), 0,
                                       tamanho >= 0xFFFFFFFF);
    if (erro != ZIP_OK) {
        throw CCompressionError(ECE_ZIP_ADICIONAR, std::format("O arquivo {} não pode ser adicionado. {}", arquivo.string(), ConvertToErrorMessage(erro)));
    }

    // (original lines 247-255: content not recovered)
#line 256
    io::CFile entrada(arquivo.string(), "rb");
    ueqword total = 0;
    while (!entrada.Eof()) {
        char* buffer = new char[TAMANHO_BUFFER]();
        const uedword lido = entrada.RawRead(buffer, TAMANHO_BUFFER);
        if (lido != 0) {
            erro = zipWriteInFileInZip(m_zip, buffer, lido);
            if (erro < 0) {
                throw CCompressionError(ECE_ZIP_ESCREVER, std::format("Falha ao adicionar o arquivo {}. {}", arquivo.string(), ConvertToErrorMessage(erro)));
            }
            total += lido;
            AtualizaProgresso(total, tamanho, nome.string());
        }
        delete[] buffer;
    }
    entrada.Close();

    // (original lines 273-278: content not recovered)
#line 279
    erro = zipCloseFileInZip(m_zip);
    if (erro != ZIP_OK) {
        throw CCompressionError(ECE_ZIP_CONCLUIR, std::format("A adição do arquivo {} não pode ser concluída. {}", arquivo.string(), ConvertToErrorMessage(erro)));
    }
}

void CZip::AtualizaProgresso(ueqword atual, ueqword total, const std::string& arquivo)
{
    if ((atual & 0xFFFFF) == 0) {
        NotificarProgresso((atual << 10) / total, 1024, std::format("Compactando arquivo {}...", arquivo));
    }
}

} // namespace ecourna::api::compression

// <stubs>
// f5193 · 238 B · unmatched · placement: link order · name unknown
// ? f5193(i32, i32, i32) { ... }
// </stubs>
