#pragma once
#include <string>

#include <boost/date_time/posix_time/ptime.hpp>

#include "ecourna/app/dados/cidentificadorgeradormidia.hpp"
#include "ecourna/app/dados/cserialmidia.hpp"

namespace ecourna::app::dados {

// When, by whom and on which media a media was generated. 72 bytes; layout from the constructor (fn 9034), the
// copy constructor (fn 9032) and the converter (app/dados/asn/midias/cconversordadosgeracaomidia.cpp, fns 9203/9202).
class CDadosGeracaoMidia {
public:
    CDadosGeracaoMidia(const CSerialMidia& serialMidia, std::string usuario,
                       const CIdentificadorGeradorMidia& identificadorGeradorMidia,
                       const boost::posix_time::ptime& data);

    const CSerialMidia& GetSerialMidia() const { return m_serialMidia; }
    const std::string& GetUsuario() const { return m_usuario; }
    const CIdentificadorGeradorMidia& GetIdentificadorGeradorMidia() const { return m_identificadorGeradorMidia; }
    const boost::posix_time::ptime& GetData() const { return m_data; }

private:
    CSerialMidia m_serialMidia;
    std::string m_usuario;
    CIdentificadorGeradorMidia m_identificadorGeradorMidia;
    boost::posix_time::ptime m_data;
};

} // namespace ecourna::app::dados
