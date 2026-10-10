#pragma once
#include <memory>
#include <string>
#include <vector>

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include "ecourna/api/sql/isqlresultset.hpp"
#include "ecourna/api/uetypes.hpp"

namespace ecourna::api::sql {

class ISqlStatement {
public:
    virtual ~ISqlStatement() = default;

    virtual void SetInt(int indice, int valor) = 0;
    virtual void SetInt64(int indice, ueint64 valor) = 0;
    virtual void SetDouble(int indice, double valor) = 0;
    virtual void SetText(int indice, const std::string& valor) = 0;
    virtual void SetDateTime(int indice, const boost::posix_time::ptime& valor) = 0;
    virtual void SetBool(int indice, bool valor) = 0;
    virtual void SetBlob(int indice, const std::vector<uebyte>& valor) = 0;
    virtual void SetNull(int indice) = 0;
    virtual TSharedSqlResultSet Execute() = 0;
    virtual void Reset() = 0;
    virtual void Close() = 0;
    virtual bool IsClosed() const = 0;
};

using TSharedSqlStatement = std::shared_ptr<ISqlStatement>;

} // namespace ecourna::api::sql
