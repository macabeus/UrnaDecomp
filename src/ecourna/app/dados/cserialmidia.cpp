#include "ecourna/app/dados/cserialmidia.hpp"

#include <format>
#include <regex>

#include "ecourna/api/util/cstringutils.hpp"
#include "ecourna/app/dados/edadoserror.hpp"

namespace ecourna::app::dados {

bool CSerialMidia::ValidaSerial(const std::string& serial)
{
    std::regex formato("([A-Fa-f0-9]{8})");
    return std::regex_match(serial, formato);
}

// (original lines 17-33: content not recovered)
#line 34
CSerialMidia::CSerialMidia(const std::string& serial)
{
    if (!ValidaSerial(serial)) {
        throw CDadosError(EDE_SERIAL_MIDIA_INVALIDO, std::format("Número de série '{}' inválido.", serial));
    }
    m_serial = api::util::CStringUtils::HexStringToBytes(serial);
}

std::string CSerialMidia::ToString() const
{
    return api::util::CStringUtils::BytesToHexString(m_serial);
}

} // namespace ecourna::app::dados
