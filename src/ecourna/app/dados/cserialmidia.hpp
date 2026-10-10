#pragma once
#include <string>
#include <vector>

#include "ecourna/api/uetypes.hpp"

namespace ecourna::app::dados {

// A media serial number: 8 hexadecimal digits, kept as bytes.
class CSerialMidia {
public:
    explicit CSerialMidia(const std::string& serial);

    // The serial as hexadecimal text (fn 9257, used by app/dados/asn/midias/cconversordadosgeracaomidia.cpp).
    std::string ToString() const;

private:
    static bool ValidaSerial(const std::string& serial);

    std::vector<uebyte> m_serial;
};

} // namespace ecourna::app::dados
