#pragma once
#include <memory>
#include <string>
#include <vector>

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include "ecourna/api/uetypes.hpp"

namespace ecourna::api::sql {

class ISqlResultSet {
public:
    virtual ~ISqlResultSet() = default;

    virtual int GetInt(int coluna) = 0;
    virtual int GetInt(const std::string& coluna) = 0;
    virtual ueint64 GetInt64(int coluna) = 0;
    virtual ueint64 GetInt64(const std::string& coluna) = 0;
    virtual double GetDouble(int coluna) = 0;
    virtual double GetDouble(const std::string& coluna) = 0;
    virtual std::string GetText(int coluna) = 0;
    virtual std::string GetText(const std::string& coluna) = 0;
    virtual bool IsNull(int coluna) = 0;
    virtual bool IsNull(const std::string& coluna) = 0;
    virtual boost::posix_time::ptime GetDateTime(int coluna) = 0;
    virtual boost::posix_time::ptime GetDateTime(const std::string& coluna) = 0;
    virtual bool GetBool(int coluna) = 0;
    virtual bool GetBool(const std::string& coluna) = 0;
    virtual std::vector<uebyte> GetBlob(int coluna) = 0;
    virtual std::vector<uebyte> GetBlob(const std::string& coluna) = 0;
    virtual bool Next() = 0;
};

using TSharedSqlResultSet = std::shared_ptr<ISqlResultSet>;

} // namespace ecourna::api::sql
