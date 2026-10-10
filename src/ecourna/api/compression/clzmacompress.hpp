#pragma once
#include <filesystem>
#include <string>

#include "ecourna/api/compression/icompressor.hpp"

namespace ecourna::api::compression {

// A 7z package written through the 7-Zip 21.02 writer behind the TSE wrapper API (see lzmasdkstub.hpp).
// Layout (36 bytes): ICompressor(16) + m_pacote(12) + m_nivel(4). Close() is empty: DoAdd writes the archive at once.
class CLzmaCompress final : public ICompressor {
public:
    CLzmaCompress(const std::filesystem::path& pacote, ENivelCompressao nivel) : m_pacote(pacote.string()), m_nivel(nivel) {}
    ~CLzmaCompress() override = default;

    void Close() override {}
    bool SuportaGlob() const override { return false; }

protected:
    void DoAdd(const std::filesystem::path& arquivo, const std::filesystem::path& nome) override;

private:
    void AtualizaProgresso(unsigned long long percentual, const std::string& arquivo);

    std::string m_pacote;
    ENivelCompressao m_nivel;
};

} // namespace ecourna::api::compression
