#pragma once
#include <sstream>
#include <string>
#include <typeinfo>

#include "asn1/asn1.h"
#include "ecourna/api/asn/eapiasnerror.hpp"

namespace ecourna::api::asn {

// Converts between a generated ASN.1 entity and the application's data class. The public pair validates the
// entity (isValid + isStrictlyValid) and reports the first invalid field through ASN1::trace_invalid; the
// virtual pair does the actual mapping.
template <typename ENTIDADE, typename DADO>
class IConversorASN {
public:
    typedef ENTIDADE TEntidade;
    typedef DADO TDado;

    virtual ~IConversorASN() {}

    // (original lines 22-40: content not recovered)
#line 41
    TEntidade Converte(const TDado& dado) const
    {
        TEntidade entidade = DoConverte(dado);
        if (!entidade.isValid() || !entidade.isStrictlyValid()) {
            std::string prefixo(typeid(ENTIDADE).name());
            prefixo += ": ";
            std::stringstream erro;
            ASN1::trace_invalid(erro, prefixo.c_str(), entidade);
            throw CApiAsnError(EAAE_ENTIDADE_INVALIDA, erro.str());
        }
        return entidade;
    }

    // (original lines 54-58: content not recovered)
#line 59
    TDado Deconverte(const TEntidade& entidade) const
    {
        if (!entidade.isValid() || !entidade.isStrictlyValid()) {
            std::string prefixo(typeid(ENTIDADE).name());
            prefixo += ": ";
            std::stringstream erro;
            ASN1::trace_invalid(erro, prefixo.c_str(), entidade);
            throw CApiAsnError(EAAE_DADO_INVALIDO, erro.str());
        }
        return DoDeconverte(entidade);
    }

protected:
    virtual TEntidade DoConverte(const TDado& dado) const = 0;
    virtual TDado DoDeconverte(const TEntidade& entidade) const = 0;
};

} // namespace ecourna::api::asn
