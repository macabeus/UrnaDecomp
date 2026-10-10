#include "ecourna/api/compression/icompressor.hpp"

#include <regex>
#include <string>

#include "ecourna/api/compression/ecompressionerror.hpp"

namespace ecourna::api::compression {

namespace {

bool endsWith(const std::string& valor, const std::string& sufixo)
{
    return valor.size() >= sufixo.size() && valor.compare(valor.size() - sufixo.size(), sufixo.size(), sufixo) == 0;
}

bool isGlob(const std::string& valor)
{
    return valor.find_first_of("*?") != std::string::npos;
}

std::string globToRegex(const std::string& glob)
{
    std::string regex;
    for (const char caractere : glob) {
        switch (caractere) {
        case '*':
            regex.append(".*");
            break;
        case '?':
            regex.append(".");
            break;
        case '.':
            regex.append("\\.");
            break;
        case '\\':
            regex.append("\\\\");
            break;
        default:
            regex.push_back(caractere);
            break;
        }
    }
    return regex;
}






std::vector<std::filesystem::path> globToFileList(const std::filesystem::path& glob)
{
    std::filesystem::path caminho = glob;
    std::filesystem::path diretorio = caminho.parent_path();
    if (diretorio == "") {
        diretorio = ".";
        caminho = diretorio / caminho;
    }
    if (!std::filesystem::is_directory(diretorio)) {
        throw CCompressionError(ECE_GLOB_INVALIDO, "[" + glob.string() + "] é um glob inválido");
    }

    std::vector<std::filesystem::path> arquivos;
    const std::regex regex(globToRegex(caminho.string()));
    auto adiciona = [&regex, &arquivos](const std::filesystem::directory_entry& entrada) {
        if (entrada.is_regular_file()) {
            const std::filesystem::path encontrado = entrada.path();
            if (std::regex_match(encontrado.string(), regex)) {
                arquivos.push_back(encontrado);
            }
        }
    };
    if (endsWith(caminho.string(), "/*")) {
        for (const auto& entrada : std::filesystem::recursive_directory_iterator(diretorio)) {
            adiciona(entrada);
        }
    } else {
        for (const auto& entrada : std::filesystem::directory_iterator(diretorio)) {
            adiciona(entrada);
        }
    }
    return arquivos;
}

} // namespace

ICompressor::ICompressor() {}

ICompressor::~ICompressor() {}

ICompressor& ICompressor::Add(const std::map<std::filesystem::path, std::filesystem::path>& arquivos)
{
    for (const auto& [arquivo, nome] : arquivos) {
        DoAdd(arquivo, nome);
    }
    return *this;
}

void ICompressor::Add(const std::vector<std::filesystem::path>& arquivos)
{
    for (const auto& arquivo : arquivos) {
        DoAdd(arquivo, arquivo.filename());
    }
}

void ICompressor::Add(const std::filesystem::path& arquivo)
{
    if (SuportaGlob() && isGlob(arquivo.string())) {
        for (const auto& encontrado : globToFileList(arquivo)) {
            Add(std::vector<std::filesystem::path>{encontrado});
        }
    } else {
        Add(std::vector<std::filesystem::path>{arquivo});
    }
}

} // namespace ecourna::api::compression
