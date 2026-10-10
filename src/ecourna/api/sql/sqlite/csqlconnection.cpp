#include "ecourna/api/sql/sqlite/csqlconnection.hpp"

#include <format>

#include <sqlite3.h>

#include "ecourna/api/sql/esqlerror.hpp"
#include "ecourna/api/sql/sqlite/csqlstatement.hpp"
#include "ecourna/api/sql/sqlite/sqliteutils.hpp"

namespace ecourna::api::sql::sqlite {

// (original lines 13-23: content not recovered)
#line 24
CSqlConnection::CSqlConnection(const std::string& caminho) : m_caminho(caminho), m_db(nullptr), m_fechada(true)
{
    if (sqlite3_open(caminho.c_str(), &m_db) != SQLITE_OK) {
        std::string mensagem = ObterMensagemErro(m_db);
        sqlite3_close_v2(m_db);
        throw CSqlError(ESE_ABRIR_BANCO, mensagem);
    }

    try {
        m_fechada = false;

        TSharedSqlStatement comando = Prepare("PRAGMA foreign_keys = ON");
        comando->Execute();
        comando->Close();
    } catch (...) {
        std::string mensagem = ObterMensagemErro(m_db);
        sqlite3_close_v2(m_db);
        m_fechada = true;


        throw CSqlError(ESE_INICIALIZAR_BANCO, mensagem);
    }
}

void CSqlConnection::BeginTransaction()
{
    TSharedSqlStatement comando = Prepare("begin transaction");
    comando->Execute();
    comando->Close();
}

void CSqlConnection::CommitTransaction()
{
    TSharedSqlStatement comando = Prepare("commit transaction");
    comando->Execute();
    comando->Close();
}

void CSqlConnection::RollbackTransaction()
{
    TSharedSqlStatement comando = Prepare("rollback transaction");
    comando->Execute();
    comando->Close();
}

TSharedSqlStatement CSqlConnection::Prepare(const std::string& sql)
{
    sqlite3_stmt* comando;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &comando, nullptr) != SQLITE_OK) {

        // (original lines 74-85: content not recovered)
#line 86
        throw CSqlError(ESE_PREPARAR_COMANDO,
                        std::format("Falha ao executar operação no banco de dados.\nMensagem do banco de dados: '{}'\nOperação: '{}'",
                                    ObterMensagemErro(m_db), sql));
    }

    return TSharedSqlStatement(new CSqlStatement(m_db, comando));
}

bool CSqlConnection::IsClosed() const
{
    return m_fechada;
}


void CSqlConnection::Close()
{
    if (sqlite3_close_v2(m_db) != SQLITE_OK) {
        throw CSqlError(ESE_FECHAR_BANCO, ObterMensagemErro(m_db));
    }

    m_fechada = true;
}

bool CSqlConnection::CheckIntegrity()
{
    try {
        TSharedSqlStatement comando = Prepare("PRAGMA integrity_check(1)");
        comando->Execute();
        comando->Close();

        return true;
    } catch (...) {
        return false;
    }
}

const std::string& CSqlConnection::GetCaminho() const
{
    return m_caminho;
}

} // namespace ecourna::api::sql::sqlite
