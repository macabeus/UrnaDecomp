#pragma once
#include <filesystem>
#include <map>
#include <vector>

#include "ecourna/api/pattern/iobservableprogresswithdescription.hpp"
#include "ecourna/api/pattern/noncopyable.hpp"

namespace ecourna::api::compression {

// CZip: ConvertToCompressLevel maps 0,1,2,3 -> Z_NO_COMPRESSION, Z_BEST_SPEED, Z_DEFAULT_COMPRESSION, Z_BEST_COMPRESSION;
// CLzmaCompress only asks "!= NENHUM" (Copy vs LZMA).
enum class ENivelCompressao {
    NENHUM = 0,
    RAPIDO = 1,
    PADRAO = 2,
    MAXIMO = 3,
};

// RTTI: vmi class, bases NonCopyable (private, offset 0) then IObservableProgressWithDescription (public, offset 0).
// Vtable: ~ICompressor (2 slots), Close, SuportaGlob, DoAdd.
class ICompressor : pattern::NonCopyable, public pattern::IObservableProgressWithDescription {
public:
    ICompressor();
    virtual ~ICompressor();

    ICompressor& Add(const std::map<std::filesystem::path, std::filesystem::path>& arquivos);
    void Add(const std::vector<std::filesystem::path>& arquivos);
    void Add(const std::filesystem::path& arquivo);

    virtual void Close() = 0;
    virtual bool SuportaGlob() const = 0;

protected:
    virtual void DoAdd(const std::filesystem::path& arquivo, const std::filesystem::path& nome) = 0;
};

} // namespace ecourna::api::compression
