#pragma once
#include <map>
#include <string>
#include <vector>

#include "ecourna/api/sql/isqlresultset.hpp"

struct sqlite3;
struct sqlite3_stmt;

namespace ecourna::api::sql::sqlite {

class CSqlResultSet final : public ISqlResultSet {
public:
    CSqlResultSet(sqlite3* db, sqlite3_stmt* stmt);
    ~CSqlResultSet() override = default;

    int GetInt(int coluna) override;
    int GetInt(const std::string& coluna) override;
    ueint64 GetInt64(int coluna) override;
    ueint64 GetInt64(const std::string& coluna) override;
    double GetDouble(int coluna) override;
    double GetDouble(const std::string& coluna) override;
    std::string GetText(int coluna) override;
    std::string GetText(const std::string& coluna) override;
    bool IsNull(int coluna) override;
    bool IsNull(const std::string& coluna) override;
    boost::posix_time::ptime GetDateTime(int coluna) override;
    boost::posix_time::ptime GetDateTime(const std::string& coluna) override;
    bool GetBool(int coluna) override;
    bool GetBool(const std::string& coluna) override;
    std::vector<uebyte> GetBlob(int coluna) override;
    std::vector<uebyte> GetBlob(const std::string& coluna) override;
    bool Next() override;

private:
    void VerifyColumnIndex(int coluna) const;
    int IndexForColumnName(const std::string& coluna);
    bool StepStatement();

    sqlite3* m_db;
    sqlite3_stmt* m_stmt;
    bool m_temDados;
    bool m_primeiraLinha;
    int m_numeroColunas;
    std::map<std::string, int> m_colunas;
};

} // namespace ecourna::api::sql::sqlite
