#pragma once
#include <cstdio>
#include <filesystem>
#include <string>
#include <vector>

#include "ecourna/api/uetypes.hpp"

namespace ecourna::api::io {

// Flags applied with fcntl after Open (bit 0 -> O_SYNC, bit 1 -> O_NOATIME).
enum FileMode {
    FM_NORMAL = 0,
    FM_SYNC = 1,
    FM_NOATIME = 2,
};

class CFile {
public:
    // Both defined here: the target's CFile ctor (fn 517) is a uenux2-flavoured COMDAT copy and the dtor is inlined
    // into every caller as `if (m_file) Close();`.
    CFile(const std::string& nome, const std::string& modo, FileMode fileMode = FM_NORMAL) : m_file(nullptr), m_nome(), m_escrita(false)
    {
        if (!nome.empty()) {
            Open(nome, modo, fileMode);
        }
    }
    ~CFile()
    {
        if (m_file != nullptr) {
            Close();
        }
    }

    bool Open(const std::string& nome, const std::string& modo, FileMode fileMode);
    void Close();
    bool SetFileMode(FileMode fileMode);
    int Flush() const;
    void Sync() const;
    uedword RawWrite(const void* buffer, uedword tamanho) const;
    uedword WriteLine(const std::string& linha) const;
    uedword RawRead(void* buffer, uedword tamanho) const;
    uedword ReadLine(std::string& linha) const;
    bool Seek(int deslocamento, int origem) const;
    bool Eof() const;
    long Position() const;

    const std::string& GetNome() const { return m_nome; }
    bool IsOpen() const { return m_file != nullptr; }

    static std::string ReadFileContent(const std::string& nome);
    static std::vector<uebyte> ReadFileBinary(const std::filesystem::path& caminho);

private:
    static const int TAMANHO_BUFFER_LINHA = 128;

    FILE* m_file;
    std::string m_nome;
    bool m_escrita;
};

} // namespace ecourna::api::io
