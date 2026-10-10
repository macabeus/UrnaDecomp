#pragma once
#include <string>
#include <vector>

#include "ecourna/api/sql/isqlstatement.hpp"

struct sqlite3;
struct sqlite3_stmt;

namespace ecourna::api::sql::sqlite {

class CSqlStatement final : public ISqlStatement {
public:
    CSqlStatement(sqlite3* db, sqlite3_stmt* stmt);
    ~CSqlStatement() override
    {
        if (!m_fechado) {
            try {
                Close();
            } catch (...) {
            }
        }
    }

    void SetInt(int indice, int valor) override;
    void SetInt64(int indice, ueint64 valor) override;
    void SetDouble(int indice, double valor) override;
    void SetText(int indice, const std::string& valor) override;
    void SetDateTime(int indice, const boost::posix_time::ptime& valor) override;
    void SetBool(int indice, bool valor) override;
    void SetBlob(int indice, const std::vector<uebyte>& valor) override;
    void SetNull(int indice) override;
    TSharedSqlResultSet Execute() override;
    void Reset() override;
    void Close() override;
    bool IsClosed() const override;

private:
    void VerifyParameterIndex(int indice) const;

    sqlite3* m_db;
    sqlite3_stmt* m_stmt;
    bool m_fechado;
    int m_numeroParametros;
};

} // namespace ecourna::api::sql::sqlite
