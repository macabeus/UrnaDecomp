// Root set for the APPFLAGS probe: calls every reconstructed unit the way the target does (same number of call
// sites, virtual dispatch through shared_ptr), so binaryen's inlining and merging decisions match the real link.
#include <cstdio>
#include <memory>
#include <string>
#include <vector>

#include "ecourna/api/security/base64.hpp"
#include "ecourna/api/util/cstringutils.hpp"
#include "ecourna/app/dados/cfabricaidentificadoreleitor.hpp"
#include "ecourna/app/dados/cnumerocpf.hpp"
#include "ecourna/app/dados/cnumeroidentificacaolivre.hpp"
#include "ecourna/app/dados/cnumeroinscricaoeleitoral.hpp"

namespace teste {

using ecourna::api::util::CStringUtils;

// other classes of the target share IIdentificadorEleitor's destructor shape ({vptr, int, std::string}); two
// stand-ins let binaryen merge the destructors the way the real link does
struct CComNome {
    virtual ~CComNome() = default;
    virtual int Valor() const { return m_valor; }
    int m_valor = 1;
    std::string m_nome;
};
struct CComTitulo {
    virtual ~CComTitulo() = default;
    virtual int Valor() const { return m_valor * 2; }
    int m_valor = 2;
    std::string m_titulo;
};
__attribute__((noinline)) int UsaNomes(const std::string& s)
{
    std::unique_ptr<CComNome> a(new CComNome{});
    a->m_nome = s;
    std::unique_ptr<CComTitulo> b(new CComTitulo{});
    b->m_titulo = s + s;
    return a->Valor() + b->Valor() + static_cast<int>(a->m_nome.size() + b->m_titulo.size());
}

// the target's caller (function 3703) reads the vector from offset 168 of an object
struct SComResumo {
    char preenchimento[168];
    std::vector<uebyte> dados;
};

__attribute__((noinline)) std::string ResumoBase64(const SComResumo& obj)
{
    return ecourna::api::security::EncodeBase64(obj.dados).substr(0, 8);
}

std::shared_ptr<ecourna::app::dados::IIdentificadorEleitor> CriaIdentificador(int tipo, const std::string& numero)
{
    switch (tipo) {
    case 1:
        return std::make_shared<ecourna::app::dados::CNumeroInscricaoEleitoral>(numero);
    case 2:
        return std::make_shared<ecourna::app::dados::CNumeroCPF>(numero);
    default:
        return std::make_shared<ecourna::app::dados::CNumeroIdentificacaoLivre>(numero);
    }
}

__attribute__((noinline)) unsigned long long UsaConversoes1(const std::string& s)
{
    return CStringUtils::ToByte(s) + CStringUtils::ToInt16(s) + CStringUtils::ToWord(s) + CStringUtils::ToInt32(s) +
           CStringUtils::ToQWord(s) + CStringUtils::ToDWord(s) + CStringUtils::HexToInt(s) +
           CStringUtils::HexStringToBytes(s).size();
}

__attribute__((noinline)) unsigned long long UsaConversoes2(const std::string& s)
{
    return CStringUtils::ToByte(s) * 3 + CStringUtils::ToInt16(s) * 5 + CStringUtils::ToWord(s) * 7 +
           CStringUtils::ToInt32(s) * 11 + CStringUtils::ToQWord(s) * 13 + CStringUtils::ToDWord(s) * 17 +
           CStringUtils::HexToInt(s) * 19 + CStringUtils::HexStringToBytes(s).size() * 23;
}

__attribute__((noinline)) unsigned long long UsaConversoes3(const std::string& s)
{
    return CStringUtils::ToQWord(s) ^ CStringUtils::HexToQWord(s) ^ CStringUtils::ToDWord(s);
}

// PreencheAEsquerda has 15 callers in the target: keep it out of line here too
__attribute__((noinline)) std::string UsaPreenche1(const std::string& s) { return CStringUtils::PreencheAEsquerda(s, '0', 11); }
__attribute__((noinline)) std::string UsaPreenche2(const std::string& s) { return CStringUtils::PreencheAEsquerda(s, ' ', 20) + "x"; }
__attribute__((noinline)) std::string UsaPreenche3(const std::string& s) { return "y" + CStringUtils::PreencheAEsquerda(s, '*', 5); }

__attribute__((noinline)) std::string UsaBase64A(const SComResumo& v) { return ResumoBase64(v) + "A"; }
__attribute__((noinline)) std::string UsaBase64B(const SComResumo& v) { return "B" + ResumoBase64(v); }
__attribute__((noinline)) std::string UsaBase64C(const SComResumo& v) { return ResumoBase64(v); }

__attribute__((noinline)) std::string UsaIdentificador(int tipo, const std::string& numero)
{
    auto id = CriaIdentificador(tipo, numero);
    auto outro = ecourna::app::dados::CFabricaIdentificadorEleitor::Cria(tipo, numero + "1");
    return id->Formatado() + id->GetNumero() + outro->Formatado();
}

} // namespace teste

int main(int argc, char** argv)
{
    std::string s = argc > 1 ? argv[1] : "123";
    teste::SComResumo v{};
    v.dados.assign(s.begin(), s.end());
    unsigned long long acc = teste::UsaNomes(s) + teste::UsaConversoes1(s) + teste::UsaConversoes2(s) + teste::UsaConversoes3(s);
    std::string out = teste::UsaBase64A(v) + teste::UsaBase64B(v) + teste::UsaBase64C(v);
    out += teste::UsaPreenche1(s) + teste::UsaPreenche2(s) + teste::UsaPreenche3(s);
    out += teste::UsaIdentificador(argc, s) + teste::UsaIdentificador(argc + 1, s) + teste::UsaIdentificador(argc + 2, s);
    std::printf("%llu %s\n", acc, out.c_str());
    return 0;
}
