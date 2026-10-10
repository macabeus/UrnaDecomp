#include "ecourna/api/io/cfile.hpp"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <format>
#include <unistd.h>

#include "ecourna/api/io/cioerror.hpp"
#include "ecourna/api/util/csynchronizer.hpp"

using namespace std::string_literals;

namespace ecourna::api::io {

// (original lines 16-55: content not recovered)
#line 56
bool CFile::Open(const std::string& nome, const std::string& modo, FileMode fileMode)
{
    FILE* arquivo = fopen(nome.c_str(), modo.c_str());
    if (arquivo == nullptr) {
        throw CIoError(EIO_ABRIR, EFileOperation::Open, "nao foi possivel abrir " + nome, errno);
    }

    if (m_file != nullptr) {
        Close();
    }
    m_nome = nome;
    m_file = arquivo;
    if (modo.find_first_of("+aw") != std::string::npos) {
        m_escrita = true;
    }
    if (!SetFileMode(fileMode)) {
        m_file = nullptr;
        fclose(arquivo);
        throw CIoError(EIO_ALTERAR_MODO, EFileOperation::Mode, "nao foi possivel alterar modo " + nome, errno);
    }
    return true;
}

// (original lines 79-97: content not recovered)
#line 98
void CFile::Close()
{
    if (m_file == nullptr) {
        return;
    }
    if (m_escrita == true) {
        Sync();
    }
    if (fclose(m_file) == 0) {
        m_nome.clear();
        m_file = nullptr;
        return;
    }
    throw CIoError(EIO_FECHAR, EFileOperation::Close, "nao foi possivel fechar " + m_nome, errno);
}

bool CFile::SetFileMode(FileMode fileMode)
{
    if (m_file == nullptr) {
        throw CIoError(EIO_MODO_NAO_ABERTO, EFileOperation::None, "arquivo nao estava aberto");
    }
    int descritor = fileno(m_file);
    int flags = fcntl(descritor, F_GETFD);
    if (descritor < 0 || flags == -1) {
        return false;
    }
    int novos = 0;
    if (fileMode & FM_SYNC) {
        novos |= O_SYNC;
    }
    if (fileMode & FM_NOATIME) {
        novos |= O_NOATIME;
    }
    flags = (flags & ~(O_SYNC | O_NOATIME)) | novos;
    return fcntl(descritor, F_SETFD, flags) != -1;
}

// (original lines 135-141: content not recovered)
#line 142
int CFile::Flush() const
{
    if (m_file == nullptr) {
        throw CIoError(EIO_FLUSH_NAO_ABERTO, EFileOperation::None, "arquivo nao estava aberto");
    }
    return fflush(m_file);
}

// (original lines 150-156: content not recovered)
#line 157
void CFile::Sync() const
{
    if (m_file == nullptr) {
        throw CIoError(EIO_SYNC_NAO_ABERTO, EFileOperation::None, "arquivo nao estava aberto " + m_nome);
    }

    if (fflush(m_file) != 0) {
        std::string mensagem = "nao sincronizou " + m_nome + " - erro = " + strerror(errno);
        throw CIoError(EIO_SYNC_FFLUSH, EFileOperation::Sync, mensagem, errno);
    }
    int descritor = fileno(m_file);
    if (descritor == -1) {
        std::string mensagem = "nao recuperou o descritor de "s + m_nome + ", erro = "s + strerror(errno);
        throw CIoError(EIO_SYNC_DESCRITOR, EFileOperation::Read, mensagem, errno);
    }

    if (util::CSynchronizer::GetInst().Sync(descritor) != 0) {
        std::string mensagem = "nao sincronizou " + m_nome + " - erro = " + strerror(errno);
        throw CIoError(EIO_SYNC_FSYNC, EFileOperation::Sync, mensagem, errno);
    }
}

// (original lines 179-180: content not recovered)
#line 181
uedword CFile::RawWrite(const void* buffer, uedword tamanho) const
{
    if (m_file == nullptr) {
        throw CIoError(EIO_ESCRITA_NAO_ABERTO, EFileOperation::None, "arquivo nao estava aberto " + m_nome);
    }
    if (buffer == nullptr) {
        throw CIoError(EIO_ESCRITA_BUFFER, EFileOperation::None, "buffer invalido");
    }
    if (tamanho == 0) {
        throw CIoError(EIO_ESCRITA_TAMANHO, EFileOperation::None, "tamanho do buffer deve ser maior que zero");
    }
    uedword escritos = fwrite(buffer, 1, tamanho, m_file);
    if (escritos == 0 || fflush(m_file) != 0) {
        std::string detalhe = std::format("size = {}", tamanho);
        throw CIoError(EIO_ESCRITA_FALHOU, EFileOperation::Write, m_nome + " " + detalhe, errno);
    }
    return escritos;
}

// (original lines 200-207: content not recovered)
#line 208
uedword CFile::WriteLine(const std::string& linha) const
{
    if (m_file == nullptr) {
        throw CIoError(EIO_ESCRITA_LINHA_NAO_ABERTO, EFileOperation::None, "arquivo nao estava aberto " + m_nome);
    }
    if (RawWrite(linha.c_str(), linha.size()) != linha.size()) {
        throw CIoError(EIO_ESCRITA_LINHA_FALHOU, EFileOperation::Write, m_nome, errno);
    }
    return linha.size();
}

uedword CFile::RawRead(void* buffer, uedword tamanho) const
{
    if (m_file == nullptr) {
        throw CIoError(EIO_LEITURA_NAO_ABERTO, EFileOperation::None, "arquivo nao estava aberto");
    }
    if (buffer == nullptr) {
        throw CIoError(EIO_LEITURA_BUFFER, EFileOperation::None, "buffer invalido");
    }
    if (tamanho == 0) {
        throw CIoError(EIO_LEITURA_TAMANHO, EFileOperation::None, "tamanho do buffer nao pode ser zero");
    }
    uedword lidos = fread(buffer, 1, tamanho, m_file);
    if (lidos == 0) {
        if (ferror(m_file) != 0) {
            std::string detalhe = std::format("size = {}", tamanho);
            throw CIoError(EIO_LEITURA_ERRO, EFileOperation::Read, m_nome + " " + detalhe, errno);
        }

    }
    if (lidos == 0 && feof(m_file) == 0) {
        std::string detalhe = std::format("size = {}", tamanho);
        throw CIoError(EIO_LEITURA_INCOMPLETA, EFileOperation::Read, m_nome + " " + detalhe, errno);
    }
    return lidos;
}

// The target keeps no other throw site between RawRead (line 240) and ReadLine (line 336); whatever members the
// original defines here have no source_location record and were inlined or dropped, so they are not reconstructed.

// (original lines 248-332: content not recovered)
#line 333
uedword CFile::ReadLine(std::string& linha) const
{
    if (m_file == nullptr) {
        throw CIoError(EIO_LEITURA_LINHA_NAO_ABERTO, EFileOperation::None, "arquivo nao estava aberto");
    }
    char buffer[TAMANHO_BUFFER_LINHA];
    std::string resultado;
    buffer[0] = buffer[TAMANHO_BUFFER_LINHA - 2] = '\0';
    while (fgets(buffer, TAMANHO_BUFFER_LINHA, m_file) != nullptr) {
        if (buffer[TAMANHO_BUFFER_LINHA - 2] == '\0' || buffer[TAMANHO_BUFFER_LINHA - 2] == '\n') break;
        resultado += buffer;
        buffer[TAMANHO_BUFFER_LINHA - 2] = '\0';
    }
    resultado += buffer;
    if (!resultado.empty()) {
        if (resultado.back() != '\n') resultado += "\n";
        linha.swap(resultado);
        return linha.size();
    }
    if (feof(m_file) != 0) {
        linha.clear();
        return 0;
    } else {
        throw CIoError(EIO_LEITURA_LINHA_FALHOU, EFileOperation::Read, m_nome, errno);
    }
}

bool CFile::Seek(int deslocamento, int origem) const
{
    if (m_file == nullptr) {
        throw CIoError(EIO_SEEK_NAO_ABERTO, EFileOperation::None, "arquivo nao estava aberto");
    }
    if (fseek(m_file, deslocamento, origem) != 0) {
        throw CIoError(EIO_SEEK_FALHOU, EFileOperation::Seek, m_nome, errno);
    }
    return true;
}

bool CFile::Eof() const
{
    if (m_file == nullptr) {
        throw CIoError(EIO_EOF_NAO_ABERTO, EFileOperation::None, "arquivo nao estava aberto");
    }
    return feof(m_file) != 0;
}

// (original lines 379-381: content not recovered)
#line 382
long CFile::Position() const
{
    if (m_file == nullptr) {
        throw CIoError(EIO_POSICAO_NAO_ABERTO, EFileOperation::None, "arquivo nao estava aberto");
    }
    long posicao = ftell(m_file);
    if (posicao < 0) {
        throw CIoError(EIO_POSICAO_FALHOU, EFileOperation::Tell, m_nome, errno);
    }
    return posicao;
}

// (original lines 394-415: content not recovered)
#line 416
std::string CFile::ReadFileContent(const std::string& nome)
{
    FILE* arquivo = fopen(nome.c_str(), "rb");
    if (arquivo == nullptr) {
        throw CIoError(EIO_CONTEUDO_ABRIR, EFileOperation::Open, "nao foi possivel abrir " + nome, errno);
    }
    std::string conteudo;
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), arquivo) != nullptr) {
        conteudo += buffer;
    }
    fclose(arquivo);
    return conteudo;
}

std::vector<uebyte> CFile::ReadFileBinary(const std::filesystem::path& caminho)
{
    FILE* arquivo = fopen(caminho.string().c_str(), "rb");
    if (arquivo == nullptr) {
        throw CIoError(EIO_BINARIO_ABRIR, EFileOperation::Open, "nao foi possível abrir " + caminho.string(), errno);
    }

    fseek(arquivo, 0, SEEK_END);
    long tamanho = ftell(arquivo);

    std::vector<uebyte> conteudo(tamanho);
    fseek(arquivo, 0, SEEK_SET);
    uedword lidos = fread(conteudo.data(), 1, conteudo.size(), arquivo);
    fclose(arquivo);

    if (lidos != conteudo.size()) {
        throw CIoError(EIO_BINARIO_LER, EFileOperation::Read, "não foi possível ler conteúdo de " + caminho.string());
    }
    return conteudo;
}

} // namespace ecourna::api::io
