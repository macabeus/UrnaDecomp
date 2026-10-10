#include "ecourna/api/sql/sqlite/csqlresultset.hpp"

#include <format>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <sqlite3.h>

#include "ecourna/api/sql/esqlerror.hpp"
#include "ecourna/api/sql/sqlite/sqliteutils.hpp"
#include "ecourna/api/util/cstringutils.hpp"

namespace ecourna::api::sql::sqlite {

CSqlResultSet::CSqlResultSet(sqlite3* db, sqlite3_stmt* stmt)
    : m_db(db), m_stmt(stmt), m_temDados(false), m_primeiraLinha(false), m_numeroColunas(0)
{
    m_temDados = StepStatement();
    m_primeiraLinha = true;
}

// (original lines 21-34: content not recovered)
#line 35
int CSqlResultSet::GetInt(int coluna)
{
    VerifyColumnIndex(coluna);
    if (m_primeiraLinha) {
        throw CSqlError(ESE_SEM_RESULTADO_INT, "Chame o método 'Next()' antes de buscar resultados.");
    }

    return sqlite3_column_int(m_stmt, coluna);
}


ueint64 CSqlResultSet::GetInt64(int coluna)
{
    VerifyColumnIndex(coluna);
    if (m_primeiraLinha) {
        throw CSqlError(ESE_SEM_RESULTADO_INT64, "Chame o método 'Next()' antes de buscar resultados.");
    }

    return sqlite3_column_int64(m_stmt, coluna);
}


double CSqlResultSet::GetDouble(int coluna)
{
    VerifyColumnIndex(coluna);
    if (m_primeiraLinha) {
        throw CSqlError(ESE_SEM_RESULTADO_DOUBLE, "Chame o método 'Next()' antes de buscar resultados.");
    }

    return sqlite3_column_double(m_stmt, coluna);
}


std::string CSqlResultSet::GetText(int coluna)
{
    VerifyColumnIndex(coluna);
    if (m_primeiraLinha) {
        throw CSqlError(ESE_SEM_RESULTADO_TEXT, "Chame o método 'Next()' antes de buscar resultados.");
    }

    const unsigned char* texto = sqlite3_column_text(m_stmt, coluna);
    int tamanho = sqlite3_column_bytes(m_stmt, coluna);
    if (tamanho != 0 && texto != nullptr) {
        return std::string(reinterpret_cast<const char*>(texto));
    }

    return std::string();
}

// (original lines 84-85: content not recovered)
#line 86
bool CSqlResultSet::IsNull(int coluna)
{
    VerifyColumnIndex(coluna);
    if (m_primeiraLinha) {
        throw CSqlError(ESE_SEM_RESULTADO_NULL, "Chame o método 'Next()' antes de buscar resultados.");
    }

    const unsigned char* texto = sqlite3_column_text(m_stmt, coluna);
    int tamanho = sqlite3_column_bytes(m_stmt, coluna);
    return texto == nullptr && tamanho == 0;
}

boost::posix_time::ptime CSqlResultSet::GetDateTime(int coluna)
{
    std::string texto = GetText(coluna);
    util::CStringUtils::Replace(texto, ' ', "T");
    util::CStringUtils::Replace(texto, '-', "");
    util::CStringUtils::Replace(texto, ':', "");

    return boost::posix_time::from_iso_string(texto);
}

boost::posix_time::ptime CSqlResultSet::GetDateTime(const std::string& coluna)
{
    return GetDateTime(IndexForColumnName(coluna));
}

bool CSqlResultSet::GetBool(int coluna)
{
    return GetInt(coluna) == 1;
}

bool CSqlResultSet::GetBool(const std::string& coluna)
{
    return GetInt(IndexForColumnName(coluna)) == 1;
}


std::vector<uebyte> CSqlResultSet::GetBlob(int coluna)
{
    VerifyColumnIndex(coluna);
    if (m_primeiraLinha) {
        throw CSqlError(ESE_SEM_RESULTADO_BLOB, "Chame o método 'Next()' antes de buscar resultados.");
    }

    const uebyte* dados = static_cast<const uebyte*>(sqlite3_column_blob(m_stmt, coluna));
    int tamanho = sqlite3_column_bytes(m_stmt, coluna);

    return std::vector<uebyte>(dados, dados + tamanho);
}

// (original lines 137-139: content not recovered)
#line 140
void CSqlResultSet::VerifyColumnIndex(int coluna) const
{
    if (coluna < 0 || coluna >= m_numeroColunas) {
        throw CSqlError(ESE_INDICE_COLUNA, "Índice da coluna fora do intervalo válido.");
    }
}

int CSqlResultSet::IndexForColumnName(const std::string& coluna)
{
    auto iterador = m_colunas.find(util::CStringUtils::ToLower(coluna));
    if (iterador != m_colunas.end()) {
        return iterador->second;
    }
    throw CSqlError(ESE_NOME_COLUNA, std::format("Coluna {} não faz parte do resultado.", coluna));
}

bool CSqlResultSet::Next()
{
    if (m_primeiraLinha) {
        m_primeiraLinha = false;
        return m_temDados;
    }

    return StepStatement();
}

bool CSqlResultSet::StepStatement()
{
    int resultado = sqlite3_step(m_stmt);

    m_numeroColunas = sqlite3_column_count(m_stmt);
    for (int i = 0; i < m_numeroColunas; ++i) {
        const char* nome = sqlite3_column_name(m_stmt, i);
        if (nome != nullptr) {
            m_colunas[util::CStringUtils::ToLower(nome)] = i;
        }
    }

    bool temLinha = resultado == SQLITE_ROW;
    if (temLinha || resultado == SQLITE_OK || resultado == SQLITE_DONE) {
        return temLinha;
    } else {
        const char* sql = sqlite3_sql(m_stmt);
        std::string operacao = sql != nullptr ? std::string(sql) : std::string("INDISPONÍVEL");

        throw CSqlError(ESE_EXECUTAR_PASSO,
                        std::format("Falha ao executar operação no banco de dados.\nMensagem do banco de dados: '{}'\nOperação: '{}'",
                                    ObterMensagemErro(m_db), operacao));
    }
}

int CSqlResultSet::GetInt(const std::string& coluna)
{
    return GetInt(IndexForColumnName(coluna));
}

ueint64 CSqlResultSet::GetInt64(const std::string& coluna)
{
    return GetInt64(IndexForColumnName(coluna));
}

double CSqlResultSet::GetDouble(const std::string& coluna)
{
    return GetDouble(IndexForColumnName(coluna));
}

std::string CSqlResultSet::GetText(const std::string& coluna)
{
    return GetText(IndexForColumnName(coluna));
}

bool CSqlResultSet::IsNull(const std::string& coluna)
{
    return IsNull(IndexForColumnName(coluna));
}

std::vector<uebyte> CSqlResultSet::GetBlob(const std::string& coluna)
{
    return GetBlob(IndexForColumnName(coluna));
}

} // namespace ecourna::api::sql::sqlite

// <stubs>
// f9449 · 135 B · unmatched · placement: link order · name unknown
// ? f9449(i32, i32, i32) { ... }
// </stubs>
