#pragma once
#include <string>

#include <boost/date_time/posix_time/ptime.hpp>

namespace ecourna::app::dados::asn {

// The date-time spelling of the ASN.1 entities: "AAAAMMDDThhmmss" (std::format, fn 9220 = binaryen thunk to the
// merged body 6155) and back (fn 1877, which also takes a GeneralizedTime "AAAAMMDDhhmmssZ"). Names and file
// synthesised: no record names them; the formatter has one caller (9219), the parser several (9218, 9132, ...).
std::string ConverteDataHora(const boost::posix_time::ptime& dataHora);
boost::posix_time::ptime DesconverteDataHora(const std::string& dataHora);

} // namespace ecourna::app::dados::asn
