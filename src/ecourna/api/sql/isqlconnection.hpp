#pragma once
#include <memory>
#include <string>

#include "ecourna/api/pattern/noncopyable.hpp"
#include "ecourna/api/sql/isqlstatement.hpp"

namespace ecourna::api::sql {

class ISqlConnection : pattern::NonCopyable {
public:
    virtual ~ISqlConnection() = default;

    virtual void BeginTransaction() = 0;
    virtual void CommitTransaction() = 0;
    virtual void RollbackTransaction() = 0;
    virtual TSharedSqlStatement Prepare(const std::string& sql) = 0;
    virtual bool IsClosed() const = 0;
    virtual void Close() = 0;
    virtual bool CheckIntegrity() = 0;
    virtual const std::string& GetCaminho() const = 0;
};

using TSharedSqlConnection = std::shared_ptr<ISqlConnection>;

} // namespace ecourna::api::sql
