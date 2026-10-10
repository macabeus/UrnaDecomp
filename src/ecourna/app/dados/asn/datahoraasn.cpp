#include "ecourna/app/dados/asn/datahoraasn.hpp"

#include <format>

#include <boost/date_time/posix_time/posix_time.hpp>

namespace ecourna::app::dados::asn {

std::string ConverteDataHora(const boost::posix_time::ptime& dataHora)
{
    return std::format("{:04}{:02}{:02}T{:02}{:02}{:02}", std::stoi(std::to_string(dataHora.date().year())),
                       dataHora.date().month().as_number(), dataHora.date().day().as_number(),
                       dataHora.time_of_day().hours(), dataHora.time_of_day().minutes(), dataHora.time_of_day().seconds());
}

boost::posix_time::ptime DesconverteDataHora(const std::string& dataHora)
{
    std::string texto = dataHora;
    if (texto.size() >= 9 && texto.back() == 'Z') {
        texto = texto.substr(0, 8) + "T" + texto.substr(8, 6);
    }
    return boost::posix_time::from_iso_string(texto);
}

} // namespace ecourna::app::dados::asn
