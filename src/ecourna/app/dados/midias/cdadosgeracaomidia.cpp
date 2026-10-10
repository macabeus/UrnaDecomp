#include "ecourna/app/dados/midias/cdadosgeracaomidia.hpp"

namespace ecourna::app::dados {

CDadosGeracaoMidia::CDadosGeracaoMidia(const CSerialMidia& serialMidia, std::string usuario,
                                       const CIdentificadorGeradorMidia& identificadorGeradorMidia,
                                       const boost::posix_time::ptime& data)
    : m_serialMidia(serialMidia)
    , m_usuario(usuario)
    , m_identificadorGeradorMidia(identificadorGeradorMidia)
    , m_data(data)
{
}

} // namespace ecourna::app::dados
