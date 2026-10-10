#ifndef __MODULOESTADOGERALSA_H
#define __MODULOESTADOGERALSA_H

#include "asn1.h"

namespace ModuloEstadoGeralSA {

//
// ModuloEstadoGeralSA::DadoSecaoSA
//

class DadoSecaoSA : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    DadoSecaoSA(const void* info) : Inherited(info) {}
public:
    DadoSecaoSA() : Inherited(&theInfo) {}
    DadoSecaoSA(const DadoSecaoSA & that) : Inherited(that) {}

    DadoSecaoSA& operator=(const DadoSecaoSA& that)
    { Inherited::operator=(that); return *this; }


    class municipio
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 99999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class municipio

    municipio::const_reference get_municipio () const { return *static_cast<municipio::const_pointer>(fields[0]); }
    municipio::reference ref_municipio () { return *static_cast<municipio::pointer>(fields[0]); }
    municipio::reference set_municipio () { return *static_cast<municipio::pointer>(fields[0]); }
    municipio::reference set_municipio (ASN1::INTEGER::int_type v)
    { return *static_cast<municipio::pointer>(fields[0]) = v; }

    class zona
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 9999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class zona

    zona::const_reference get_zona () const { return *static_cast<zona::const_pointer>(fields[1]); }
    zona::reference ref_zona () { return *static_cast<zona::pointer>(fields[1]); }
    zona::reference set_zona () { return *static_cast<zona::pointer>(fields[1]); }
    zona::reference set_zona (ASN1::INTEGER::int_type v)
    { return *static_cast<zona::pointer>(fields[1]) = v; }

    class secao
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 9999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class secao

    secao::const_reference get_secao () const { return *static_cast<secao::const_pointer>(fields[2]); }
    secao::reference ref_secao () { return *static_cast<secao::pointer>(fields[2]); }
    secao::reference set_secao () { return *static_cast<secao::pointer>(fields[2]); }
    secao::reference set_secao (ASN1::INTEGER::int_type v)
    { return *static_cast<secao::pointer>(fields[2]) = v; }
    void swap(DadoSecaoSA& that) { Inherited::swap(that); }
    DadoSecaoSA * clone() const
    { return static_cast<DadoSecaoSA*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[3];
    static int fieldIds[3];
    static const char* fieldNames[3];
}; // end class DadoSecaoSA

//
// ModuloEstadoGeralSA::EstadoSA
//

class EstadoSA : public ASN1::ENUMERATED
{
    typedef ASN1::ENUMERATED Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EstadoSA(const void* info) : Inherited(info) {}
public:
    EstadoSA() : Inherited(&theInfo) {}
    enum NamedNumber {
        unknownEnumeration_ = -1,
        inicial,
        arvoreraizpreparada,
        preparada,
        imprimirZEsecao,
        iniciarapuracao,
        apurarmajoritaria,
        majoritariacompleta,
        majoritariaencerrada,
        apurarproporcional,
        proporcionalcompleta,
        proporcionalencerrada,
        buiniciacargo,
        buproporcionalpedepartido,
        buproporcionalpedecandidato,
        bumajoritariapedecandidato,
        buregistrabunordv,
        posicionarmista,
        gravarbu,
        imprimirbu,
        gravaresultado,
        copiaresultadomr,
        imprimiradicionalbu,
        encerrada,
        ultimoid
    };

    bool is_inicial() const { return value == inicial; }
    void set_inicial() { value = inicial; }

    bool is_arvoreraizpreparada() const { return value == arvoreraizpreparada; }
    void set_arvoreraizpreparada() { value = arvoreraizpreparada; }

    bool is_preparada() const { return value == preparada; }
    void set_preparada() { value = preparada; }

    bool is_imprimirZEsecao() const { return value == imprimirZEsecao; }
    void set_imprimirZEsecao() { value = imprimirZEsecao; }

    bool is_iniciarapuracao() const { return value == iniciarapuracao; }
    void set_iniciarapuracao() { value = iniciarapuracao; }

    bool is_apurarmajoritaria() const { return value == apurarmajoritaria; }
    void set_apurarmajoritaria() { value = apurarmajoritaria; }

    bool is_majoritariacompleta() const { return value == majoritariacompleta; }
    void set_majoritariacompleta() { value = majoritariacompleta; }

    bool is_majoritariaencerrada() const { return value == majoritariaencerrada; }
    void set_majoritariaencerrada() { value = majoritariaencerrada; }

    bool is_apurarproporcional() const { return value == apurarproporcional; }
    void set_apurarproporcional() { value = apurarproporcional; }

    bool is_proporcionalcompleta() const { return value == proporcionalcompleta; }
    void set_proporcionalcompleta() { value = proporcionalcompleta; }

    bool is_proporcionalencerrada() const { return value == proporcionalencerrada; }
    void set_proporcionalencerrada() { value = proporcionalencerrada; }

    bool is_buiniciacargo() const { return value == buiniciacargo; }
    void set_buiniciacargo() { value = buiniciacargo; }

    bool is_buproporcionalpedepartido() const { return value == buproporcionalpedepartido; }
    void set_buproporcionalpedepartido() { value = buproporcionalpedepartido; }

    bool is_buproporcionalpedecandidato() const { return value == buproporcionalpedecandidato; }
    void set_buproporcionalpedecandidato() { value = buproporcionalpedecandidato; }

    bool is_bumajoritariapedecandidato() const { return value == bumajoritariapedecandidato; }
    void set_bumajoritariapedecandidato() { value = bumajoritariapedecandidato; }

    bool is_buregistrabunordv() const { return value == buregistrabunordv; }
    void set_buregistrabunordv() { value = buregistrabunordv; }

    bool is_posicionarmista() const { return value == posicionarmista; }
    void set_posicionarmista() { value = posicionarmista; }

    bool is_gravarbu() const { return value == gravarbu; }
    void set_gravarbu() { value = gravarbu; }

    bool is_imprimirbu() const { return value == imprimirbu; }
    void set_imprimirbu() { value = imprimirbu; }

    bool is_gravaresultado() const { return value == gravaresultado; }
    void set_gravaresultado() { value = gravaresultado; }

    bool is_copiaresultadomr() const { return value == copiaresultadomr; }
    void set_copiaresultadomr() { value = copiaresultadomr; }

    bool is_imprimiradicionalbu() const { return value == imprimiradicionalbu; }
    void set_imprimiradicionalbu() { value = imprimiradicionalbu; }

    bool is_encerrada() const { return value == encerrada; }
    void set_encerrada() { value = encerrada; }

    bool is_ultimoid() const { return value == ultimoid; }
    void set_ultimoid() { value = ultimoid; }

    EstadoSA(const NamedNumber v) : Inherited(&theInfo) { setFromInt(v); }
    EstadoSA & operator=(const NamedNumber v) { setFromInt(v);  return *this; }
    operator NamedNumber() const { return NamedNumber(asInt()); }

    bool operator == (NamedNumber rhs) const { return value == rhs; }
    bool operator != (NamedNumber rhs) const { return value != rhs; }
    bool operator <  (NamedNumber rhs) const { return value <  rhs; }
    bool operator >  (NamedNumber rhs) const { return value >  rhs; }
    bool operator <= (NamedNumber rhs) const { return value <= rhs; }
    bool operator >= (NamedNumber rhs) const { return value >= rhs; }
    bool operator == (const EstadoSA &rhs) const { return value == rhs.value; }
    bool operator != (const EstadoSA &rhs) const { return value != rhs.value; }

    void swap (EstadoSA & that) { Inherited::swap(that); }
    EstadoSA * clone() const
    { return static_cast<EstadoSA*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const char * nameList[];
}; // end class EstadoSA

//
// ModuloEstadoGeralSA::EstadoGeralSA
//

class EstadoGeralSA : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EstadoGeralSA(const void* info) : Inherited(info) {}
public:
    EstadoGeralSA() : Inherited(&theInfo) {}
    EstadoGeralSA(const EstadoGeralSA & that) : Inherited(that) {}

    EstadoGeralSA& operator=(const EstadoGeralSA& that)
    { Inherited::operator=(that); return *this; }


    class estadoSA
    {
    public:
        typedef ModuloEstadoGeralSA::EstadoSA value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class estadoSA

    estadoSA::const_reference get_estadoSA () const { return *static_cast<estadoSA::const_pointer>(fields[0]); }
    estadoSA::reference ref_estadoSA () { return *static_cast<estadoSA::pointer>(fields[0]); }
    estadoSA::reference set_estadoSA () { return *static_cast<estadoSA::pointer>(fields[0]); }
    estadoSA::reference set_estadoSA (ModuloEstadoGeralSA::EstadoSA::NamedNumber v)
    { return *static_cast<estadoSA::pointer>(fields[0]) = v; }

    class atualizacaoBloqueada
    {
    public:
        typedef ASN1::BOOLEAN value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class atualizacaoBloqueada

    atualizacaoBloqueada::const_reference get_atualizacaoBloqueada () const { return *static_cast<atualizacaoBloqueada::const_pointer>(fields[1]); }
    atualizacaoBloqueada::reference ref_atualizacaoBloqueada () { return *static_cast<atualizacaoBloqueada::pointer>(fields[1]); }
    atualizacaoBloqueada::reference set_atualizacaoBloqueada () { return *static_cast<atualizacaoBloqueada::pointer>(fields[1]); }
    atualizacaoBloqueada::reference set_atualizacaoBloqueada (bool v)
    { return *static_cast<atualizacaoBloqueada::pointer>(fields[1]) = v; }

    class dadosSecao
    {
    public:
        typedef ModuloEstadoGeralSA::DadoSecaoSA value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class dadosSecao

    dadosSecao::const_reference get_dadosSecao () const { return *static_cast<dadosSecao::const_pointer>(fields[2]); }
    dadosSecao::reference ref_dadosSecao () { return *static_cast<dadosSecao::pointer>(fields[2]); }
    dadosSecao::reference set_dadosSecao () { return *static_cast<dadosSecao::pointer>(fields[2]); }
    void swap(EstadoGeralSA& that) { Inherited::swap(that); }
    EstadoGeralSA * clone() const
    { return static_cast<EstadoGeralSA*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[3];
    static int fieldIds[3];
    static const char* fieldNames[3];
}; // end class EstadoGeralSA

} // namespace ModuloEstadoGeralSA

#endif // __MODULOESTADOGERALSA_H
