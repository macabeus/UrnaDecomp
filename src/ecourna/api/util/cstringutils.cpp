#include "ecourna/api/util/cstringutils.hpp"

#include <algorithm>
#include <cctype>
#include <cerrno>
#include <cstdlib>
#include <limits>
#include <source_location>

#include "ecourna/api/util/eutilerror.hpp"

namespace ecourna::api::util {

namespace {

ueqword ConverteSemSinal(const std::string& valor, int base, const char* digitosValidos, ueqword maximo,
                         std::source_location local)
{
    std::string texto(valor);
    CStringUtils::Trim(texto);

    if (texto.find_first_not_of(digitosValidos) != std::string::npos) {
        throw CUtilError(EUE_VALOR_INVALIDO, "valor invalido [" + valor + "]", local);
    }

    errno = 0;
    char* fim;
    const ueqword resultado = std::strtoull(texto.c_str(), &fim, base);
    if (*fim != '\0') {
        throw CUtilError(EUE_VALOR_NAO_NUMERICO, "valor invalido [" + valor + "]", local);
    }
    if (resultado > maximo || errno == ERANGE) {
        throw CUtilError(EUE_VALOR_FORA_DA_FAIXA, "valor invalido [" + valor + "]", local);
    }
    return resultado;
}

ueint64 ConverteComSinal(const std::string& valor, const char* digitosValidos, ueint64 minimo, ueint64 maximo,
                         std::source_location local)
{
    std::string texto(valor);
    CStringUtils::Trim(texto);

    if (texto.find_first_not_of(digitosValidos) != std::string::npos) {
        throw CUtilError(EUE_VALOR_INVALIDO, "valor invalido [" + valor + "]", local);
    }

    errno = 0;
    char* fim;
    const ueint64 resultado = std::strtoll(texto.c_str(), &fim, 10);
    if (*fim != '\0') {
        throw CUtilError(EUE_VALOR_NAO_NUMERICO, "valor invalido [" + valor + "]", local);
    }
    if (resultado < minimo || resultado > maximo || errno == ERANGE) {
        throw CUtilError(EUE_VALOR_FORA_DA_FAIXA, "valor invalido [" + valor + "]", local);
    }
    return resultado;
}

template <typename T>
T ConverteDecimalSemSinal(const std::string& valor, std::source_location local = std::source_location::current())
{
    char digitosValidos[] = "+-0123456789";
    return static_cast<T>(ConverteSemSinal(valor, 10, digitosValidos, std::numeric_limits<T>::max(), local));
}

template <typename T>
T ConverteDecimalComSinal(const std::string& valor, std::source_location local = std::source_location::current())
{
    char digitosValidos[] = "+-0123456789";
    return static_cast<T>(ConverteComSinal(valor, digitosValidos, std::numeric_limits<T>::min(),
                                           std::numeric_limits<T>::max(), local));
}

template <typename T>
T ConverteHexadecimal(const std::string& valor, std::source_location local = std::source_location::current())
{
    char digitosValidos[] = "xXabcdefABCDEF0123456789";
    return static_cast<T>(ConverteSemSinal(valor, 16, digitosValidos, std::numeric_limits<T>::max(), local));
}

bool NaoEhBranco(unsigned char c)
{
    return c > ' ';
}

int HexCharToInt(char c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    return -1;
}

} // namespace

void CStringUtils::Trim(std::string& valor)
{
    if (!valor.empty()) {
        const std::string::reverse_iterator ultimo = std::find_if(valor.rbegin(), valor.rend(), NaoEhBranco);
        valor.erase(valor.size() - (ultimo - valor.rbegin()));
    }
    valor.erase(0, std::find_if(valor.begin(), valor.end(), NaoEhBranco) - valor.begin());
}

std::string CStringUtils::PreencheAEsquerda(const std::string& valor, char preenchimento, size_t tamanho)
{
    std::string preenchido(valor);
    if (preenchido.size() < tamanho) {
        preenchido.insert(0, tamanho - preenchido.size(), preenchimento);
    }
    return preenchido;
}

// Blank lines below keep each std::source_location call site (the To* conversions and the throw) on
// the line the target records (ledger/source-locations.tsv).

// (original lines 124-711: content not recovered)
#line 712
uebyte CStringUtils::ToByte(const std::string& valor)
{
    return ConverteDecimalSemSinal<uebyte>(valor);
}

ueint16 CStringUtils::ToInt16(const std::string& valor)
{
    return ConverteDecimalComSinal<ueint16>(valor);
}

ueword CStringUtils::ToWord(const std::string& valor)
{
    return ConverteDecimalSemSinal<ueword>(valor);
}

ueint32 CStringUtils::ToInt32(const std::string& valor)
{
    return ConverteDecimalComSinal<ueint32>(valor);
}

ueqword CStringUtils::ToQWord(const std::string& valor)
{
    return ConverteDecimalSemSinal<ueqword>(valor);
}

uedword CStringUtils::ToDWord(const std::string& valor)
{
    return ConverteDecimalSemSinal<uedword>(valor);
}

uedword CStringUtils::HexToInt(const std::string& valor)
{
    return ConverteHexadecimal<uedword>(valor);
}

ueint64 CStringUtils::ToInt64(const std::string& valor)
{
    return ConverteDecimalComSinal<ueint64>(valor);
}

ueqword CStringUtils::HexToQWord(const std::string& valor)
{
    return ConverteHexadecimal<ueqword>(valor);
}

std::vector<uebyte> CStringUtils::HexStringToBytes(const std::string& valor)
{
    std::string texto(valor);
    Trim(texto);

    if (texto.empty()) {
        return std::vector<uebyte>();
    }

    // (original lines 766-805: content not recovered)
#line 806
    for (const char c : texto) {
        if (!std::isxdigit(c)) {
            throw CUtilError(EUE_HEX_INVALIDO, "valor invalido [" + texto + "]");
        }
    }

    std::vector<uebyte> bytes;
    bytes.reserve(texto.size() / 2);
    for (int i = 0; i < static_cast<int>(texto.size()) - 1; i += 2) {
        const int alto = HexCharToInt(texto[i]);
        const int baixo = HexCharToInt(texto[i + 1]);
        if (alto >= 0 && baixo >= 0) {
            bytes.push_back(static_cast<uebyte>((alto << 4) + baixo));
        }
    }
    return bytes;
}

std::string CStringUtils::BytesToHexString(const std::vector<uebyte>& valor)
{
    static const char DIGITOS[] = "0123456789ABCDEF";
    std::string texto;
    texto.reserve(valor.size() * 2);
    for (uebyte byte : valor) {
        texto.push_back(DIGITOS[byte >> 4]);
        texto.push_back(DIGITOS[byte & 0x0F]);
    }
    return texto;
}

} // namespace ecourna::api::util
