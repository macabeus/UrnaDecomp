#pragma once
#include <string>
#include <vector>

#include "ecourna/api/uetypes.hpp"

namespace ecourna::api::util {

class CStringUtils {
public:
    static void Trim(std::string& valor);
    static std::string PreencheAEsquerda(const std::string& valor, char preenchimento, size_t tamanho);

    static uebyte ToByte(const std::string& valor);
    static ueint16 ToInt16(const std::string& valor);
    static ueword ToWord(const std::string& valor);
    static ueint32 ToInt32(const std::string& valor);
    static ueqword ToQWord(const std::string& valor);
    static uedword ToDWord(const std::string& valor);
    static uedword HexToInt(const std::string& valor);
    static ueint64 ToInt64(const std::string& valor);
    static ueqword HexToQWord(const std::string& valor);
    static std::vector<uebyte> HexStringToBytes(const std::string& valor);
    // fn 1243: two upper-case hex digits per byte (callers 9257, 5604, 5634, 5636, 10273, 11587, 11626, 11629)
    static std::string BytesToHexString(const std::vector<uebyte>& valor);

    // Declared by round R-SQL (api/sql/sqlite callers): the target has ToLower as an in-place body (fn 5158,
    // Latin-1 aware) plus a copying wrapper (fn 1879), and Replace(char -> string) as fn 9398. Their definitions
    // are placeholders in src/units/12-ecourna-api-sql-sqlite-externs.cpp until the cstringutils unit reconstructs them.
    static void ToLower(std::string& valor);
    static std::string ToLower(const std::string& valor);
    static void Replace(std::string& valor, char de, const std::string& para);
};

} // namespace ecourna::api::util
