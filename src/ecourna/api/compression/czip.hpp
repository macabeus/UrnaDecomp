#pragma once
#include <filesystem>
#include <string>

#include <minizip/zip.h>

#include "ecourna/api/compression/icompressor.hpp"
#include "ecourna/api/uetypes.hpp"

namespace ecourna::api::compression {

// A zip archive written through zlib's minizip (zipOpen64/zipOpenNewFileInZip2_64/zipWriteInFileInZip/zipClose).
// Layout (40 bytes): ICompressor(16) + m_arquivo(12) + m_zip(4) + m_nivel(4) + m_modo(4).
class CZip : public ICompressor {
public:
    enum class EModoAbertura {
        CRIAR = 0,
        ADICIONAR = 1,
    };

    CZip(const std::filesystem::path& arquivo, ENivelCompressao nivel)
        : m_arquivo(arquivo.string()), m_zip(nullptr), m_nivel(nivel), m_modo(EModoAbertura::CRIAR)
    {
        CreateZipFile();
    }
    ~CZip() override { SafeClose(); }

    void Close() override;
    bool SuportaGlob() const override { return true; }
    void SafeClose();

protected:
    void DoAdd(const std::filesystem::path& arquivo, const std::filesystem::path& nome) override;

private:
    int ConvertToOpen();
    void CreateZipFile();
    void AssertZipIsOpened();
    int ConvertToCompressLevel();
    static std::string ConvertToErrorMessage(int erro);
    void AtualizaProgresso(ueqword atual, ueqword total, const std::string& arquivo);

    std::string m_arquivo;
    zipFile m_zip;
    ENivelCompressao m_nivel;
    EModoAbertura m_modo;
};

} // namespace ecourna::api::compression
