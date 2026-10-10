# ecourna::api::sql::sqlite DAO (csqlconnection / csqlresultset / csqlstatement) + the shared sqlite error helper.
# (sqlite3.h and boost come from the include dirs src/mk/ecourna-flags.mk adds)
ECOURNA_SRCS += api/sql/sqlite/csqlconnection.cpp
ECOURNA_SRCS += api/sql/sqlite/csqlresultset.cpp
ECOURNA_SRCS += api/sql/sqlite/csqlstatement.cpp
ECOURNA_SRCS += api/sql/sqlite/sqliteutils.cpp
# placeholder definitions of CStringUtils::ToLower/Replace (see the file header); path is relative to src/ecourna
ECOURNA_SRCS += ../units/12-ecourna-api-sql-sqlite-externs.cpp
