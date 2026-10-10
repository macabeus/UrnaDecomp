// Placeholder TU: definitions the api/sql/sqlite units call but which belong to other units (CStringUtils).
// Reconstructed from target fns 5158 (ToLower in place), 1879 (ToLower copy) and 9398 (Replace); compiled with the
// ecourna flags through src/units/12-ecourna-api-sql-sqlite.mk. Fold into api/util/cstringutils.cpp when that unit takes them over.
#include "ecourna/api/util/cstringutils.hpp"

#include <cctype>
#include <set>

namespace ecourna::api::util {

void CStringUtils::ToLower(std::string& valor)
{
    for (char& caractere : valor) {
        static const std::set<int> acentuadas = {'Ç', 'Á', 'É', 'Í', 'Ó', 'Ú', 'À', 'È', 'Ì', 'Ò', 'Ù', 'Â', 'Ê', 'Î',
                                                 'Ô', 'Û', 'Ã', 'Õ', 'Ñ'};
        if (acentuadas.find(caractere) != acentuadas.end()) {
            caractere |= 32;
        } else if (isalpha(caractere)) {
            caractere = tolower(caractere);
        }
    }
}

std::string CStringUtils::ToLower(const std::string& valor)
{
    std::string resultado = valor;
    ToLower(resultado);
    return resultado;
}

void CStringUtils::Replace(std::string& valor, char de, const std::string& para)
{
    size_t posicao = valor.find(de);
    while (posicao != std::string::npos) {
        valor.replace(posicao, 1, para);
        posicao += para.size();
        posicao = valor.find(de, posicao);
    }
}

} // namespace ecourna::api::util
