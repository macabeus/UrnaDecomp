#include "ecourna/api/sql/sqlite/sqliteutils.hpp"

#include <sqlite3.h>

namespace ecourna::api::sql::sqlite {

std::string ObterMensagemErro(sqlite3* db)
{
    const char* mensagem = sqlite3_errmsg(db);
    if (mensagem != nullptr) {
        return std::string(mensagem);
    }
    return std::string("Erro desconhecido do SQLite.");
}

} // namespace ecourna::api::sql::sqlite
