#include "ecourna/app/dados/resultadournacadastro/ccomparecimentomesario.hpp"

namespace ecourna::app::dados {

CComparecimentoMesario::CComparecimentoMesario(const CRegistroIdentificacaoEleitor& identificacaoMesario,
                                               const boost::posix_time::ptime& dataHoraColeta,
                                               EEstadoColetaDigital estadoColetaDigital)
    : m_identificacaoMesario(identificacaoMesario), m_dataHoraColeta(dataHoraColeta),
      m_estadoColetaDigital(estadoColetaDigital)
{
}

} // namespace ecourna::app::dados
