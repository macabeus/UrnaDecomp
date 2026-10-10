#pragma once
#include <string>

#include "ecourna/api/sql/isqlconnection.hpp"

struct sqlite3;

namespace ecourna::api::sql::sqlite {

class CSqlConnection final : public ISqlConnection {
public:
    explicit CSqlConnection(const std::string& caminho);
    ~CSqlConnection() override
    {
        if (!m_fechada) {
            try {
                Close();
            } catch (...) {
            }
        }
    }

    void BeginTransaction() override;
    void CommitTransaction() override;
    void RollbackTransaction() override;
    TSharedSqlStatement Prepare(const std::string& sql) override;
    bool IsClosed() const override;
    void Close() override;
    bool CheckIntegrity() override;
    const std::string& GetCaminho() const override;

private:
    std::string m_caminho;
    sqlite3* m_db;
    bool m_fechada;
};

} // namespace ecourna::api::sql::sqlite
