#include "ecourna/app/dados/ccabecalhoentidade.hpp"

namespace ecourna::app::dados {

CCabecalhoEntidade::CCabecalhoEntidade(const boost::posix_time::ptime& dataGeracao, uedword idEleitoral,
                                       ETipoIDEleitoral tipoIDEleitoral)
    : m_dataGeracao(dataGeracao), m_idEleitoral(idEleitoral), m_tipoIDEleitoral(tipoIDEleitoral)
{
}

} // namespace ecourna::app::dados
