// Root callers for the reconstructed ecourna units (src/ecourna, docs/reference/app-flags.md): the same call-site counts as
// the target so binaryen inlines and merges them the same way. Compiled as uenux2 code (UENUX2_CXXFLAGS).
#include <memory>
#include <string>
#include <vector>

#include "ecourna/api/security/base64.hpp"
#include "ecourna/api/util/cstringutils.hpp"
#include "ecourna/app/dados/cfabricaidentificadoreleitor.hpp"
#include "ecourna/app/dados/cnumerocpf.hpp"
#include "ecourna/app/dados/cnumeroidentificacaolivre.hpp"
#include "ecourna/app/dados/cnumeroinscricaoeleitoral.hpp"

namespace {

using ecourna::api::util::CStringUtils;

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

} // namespace

int urna_roots_ecourna(int a)
{
    std::string s(static_cast<size_t>(a), '1');
    SComResumo v{};
    v.dados.assign(s.begin(), s.end());
    unsigned long long acc = UsaConversoes1(s) + UsaConversoes2(s) + UsaConversoes3(s);
    std::string out = UsaBase64A(v) + UsaBase64B(v) + UsaBase64C(v);
    out += UsaPreenche1(s) + UsaPreenche2(s) + UsaPreenche3(s);
    out += UsaIdentificador(a, s) + UsaIdentificador(a + 1, s) + UsaIdentificador(a + 2, s);
    return static_cast<int>(acc) + static_cast<int>(out.size());
}
