#pragma once
#include <optional>
#include <vector>

#include <boost/date_time/posix_time/ptime.hpp>

#include "ecourna/api/uetypes.hpp"

namespace ecourna::app::dados {

// How an application on the media authenticates: an optional validity period, the password size, the number of
// attempts and the password hash. 56 bytes; layout from the constructors (fns 9037/9040) and the converter
// (app/dados/asn/midias/cconversorautenticacao.cpp, fns 9201/9200). Member names synthesised.
class CAutenticacao {
public:
    CAutenticacao(uedword numeroTentativas, uedword tamanhoSenha, const std::vector<uebyte>& hashSenha);
    CAutenticacao(const boost::posix_time::ptime& dataHoraInicial, const boost::posix_time::ptime& dataHoraFinal,
                  uedword numeroTentativas, uedword tamanhoSenha, const std::vector<uebyte>& hashSenha);

    bool PossuiDataHora() const { return m_dataHoraInicial.has_value() && m_dataHoraFinal.has_value(); }
    const boost::posix_time::ptime& GetDataHoraInicial() const;
    const boost::posix_time::ptime& GetDataHoraFinal() const;
    uedword GetNumeroTentativas() const { return m_numeroTentativas; }
    uedword GetTamanhoSenha() const { return m_tamanhoSenha; }
    const std::vector<uebyte>& GetHashSenha() const { return m_hashSenha; }

private:
    std::optional<boost::posix_time::ptime> m_dataHoraInicial;
    std::optional<boost::posix_time::ptime> m_dataHoraFinal;
    uedword m_numeroTentativas;
    uedword m_tamanhoSenha;
    std::vector<uebyte> m_hashSenha;
};

} // namespace ecourna::app::dados
