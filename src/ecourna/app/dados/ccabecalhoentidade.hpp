#pragma once
#include <boost/date_time/posix_time/ptime.hpp>

#include "ecourna/api/uetypes.hpp"

namespace ecourna::app::dados {

// The header every electoral entity carries: when it was generated and which electoral id (process, election or
// poll) it belongs to. 16 bytes, 8-aligned. Layout from fn 5112 (the constructor) and the converter
// (app/dados/asn/cconversorcabecalhoentidade.cpp, fns 9219/9218); the id kinds are the IDEleitoral CHOICE ids, the
// enumerator names synthesised.
class CCabecalhoEntidade {
public:
    enum ETipoIDEleitoral {
        TIPO_ID_PROCESSO_ELEITORAL,
        TIPO_ID_PLEITO,
        TIPO_ID_ELEICAO,
        TIPO_ID_INDEFINIDO,
    };

    CCabecalhoEntidade(const boost::posix_time::ptime& dataGeracao, uedword idEleitoral, ETipoIDEleitoral tipoIDEleitoral);

    const boost::posix_time::ptime& GetDataGeracao() const { return m_dataGeracao; }
    uedword GetIDEleitoral() const { return m_idEleitoral; }
    ETipoIDEleitoral GetTipoIDEleitoral() const { return m_tipoIDEleitoral; }

private:
    boost::posix_time::ptime m_dataGeracao;
    uedword m_idEleitoral;
    ETipoIDEleitoral m_tipoIDEleitoral;
};

} // namespace ecourna::app::dados
