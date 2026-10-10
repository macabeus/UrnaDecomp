#pragma once
// STAND-IN for the TSE wrapper over the 7-Zip 21.02 writer (not an ecourna file: the real header belongs to the LZMA
// library package and its name is unknown). What the target shows (fns 8376, 3414, 4937 and the inlined ctor in
// 9586): a 568-byte context object kept in a static std::unique_ptr by CLzmaCompress::DoAdd, whose ctor creates the
// CCodecs (CCodecs::Load inlined), and an entry point taking (ctx, files, archive, names, bool compress,
// std::function<void(unsigned long long, const std::string&)> progress) that returns true on success and leaves the
// HRESULT at +532 and the message at +536. The 7-Zip lane owns the real thing; this file only lets the ecourna TUs
// compile and link. Names are ours.
#include <functional>
#include <string>
#include <vector>

namespace lzma {

struct SEntrada {
    unsigned long long tamanho;
    unsigned long long processado;
    std::string nome;
    unsigned reservado;
};

struct CContexto {
    unsigned char opcoes[528];
    void* codecs;
    unsigned codigo;
    std::string mensagem;
    std::vector<SEntrada> entradas;
    unsigned char modo;
    unsigned char flag;
    unsigned char reservado[6];

    CContexto();
    ~CContexto();
};

bool Compacta(CContexto& contexto, const std::vector<std::string>& arquivos, const std::string& pacote,
              const std::vector<std::string>& nomes, bool comprimir,
              std::function<void(unsigned long long, const std::string&)> progresso);

} // namespace lzma
