#pragma once
#include <format>
#include <string>

#include "ecourna/api/pattern/epatternerror.hpp"


// A validated plain value: the constructor rejects anything outside [MIN_VAL, MAX_VAL]. Global namespace, as the
// target's std::source_location records spell it (CBaseType<unsigned short, 0, 99, 4>::CBaseType(TYPE)).
template <typename TYPE, TYPE MIN_VAL, TYPE MAX_VAL, int BASIC_TYPE>
class CBaseType {
public:
    typedef TYPE TValor;

    // The name the range error prints. BASIC_TYPE only makes instantiations with equal TYPE/MIN_VAL/MAX_VAL distinct
    // types, so each one has its own NOME: app/dados/tiposbasicos.cpp defines one explicit specialization per typedef
    // (the target reads them at run time - the short names constant-initialised in .data, the long ones built by the
    // static initialiser). No primary definition: an instantiation without a specialization fails to link.
    static const std::string NOME;

    // (original lines 21-35: content not recovered)
#line 36
    CBaseType(TYPE valor) : m_valor(valor)
    {
        if (valor < MIN_VAL || valor > MAX_VAL) {
            throw ecourna::api::pattern::CPatternError(ecourna::api::pattern::EPE_VALOR_INVALIDO,
                std::format("O tipo '{}' deve ter valores no intervalo [{},{}].", NOME, static_cast<int>(MIN_VAL),
                            static_cast<int>(MAX_VAL)));
        }
    }

    TYPE Get() const { return m_valor; }
    operator TYPE() const { return m_valor; }

private:
    TYPE m_valor;
};
