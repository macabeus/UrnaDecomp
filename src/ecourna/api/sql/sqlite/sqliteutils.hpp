#pragma once
#include <string>

struct sqlite3;

namespace ecourna::api::sql::sqlite {

// Placeholder home (R-SQL): the target has this as ONE out-of-line function (fn 9411) called from the
// connection, statement and result-set TUs; its real file name is unknown (no throw site names it).
std::string ObterMensagemErro(sqlite3* db);

} // namespace ecourna::api::sql::sqlite
