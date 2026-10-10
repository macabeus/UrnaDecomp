#ifndef __MODULOFEDERACOES_H
#define __MODULOFEDERACOES_H

#include "asn1.h"

#include "ModuloTiposEleitorais.h"

namespace ModuloFederacoes {

//
// ModuloFederacoes::Federacao
//

class Federacao : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    Federacao(const void* info) : Inherited(info) {}
public:
    Federacao() : Inherited(&theInfo) {}
    Federacao(const Federacao & that) : Inherited(that) {}

    Federacao& operator=(const Federacao& that)
    { Inherited::operator=(that); return *this; }


    class identificador
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 100, 999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class identificador

    identificador::const_reference get_identificador () const { return *static_cast<identificador::const_pointer>(fields[0]); }
    identificador::reference ref_identificador () { return *static_cast<identificador::pointer>(fields[0]); }
    identificador::reference set_identificador () { return *static_cast<identificador::pointer>(fields[0]); }
    identificador::reference set_identificador (ASN1::INTEGER::int_type v)
    { return *static_cast<identificador::pointer>(fields[0]) = v; }

    class sigla
    {
    public:
        //
        // ModuloFederacoes::Federacao::sigla::value_type
        //

        class value_type : public ASN1::GeneralString
        {
            typedef ASN1::GeneralString Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type() : Inherited(&theInfo) {}
            value_type(const base_string& str, const void* info = &theInfo) : Inherited(str, info) {}
            value_type(const char* str, const void* info = &theInfo) : Inherited(str, info) {}
            value_type(const value_type& that) : Inherited(that) {}
            value_type& operator=(const ASN1_STD string& that)
            { Inherited::operator=(that); return *this; }
            value_type& operator=(const char* that)
            { Inherited::operator=(that); return *this; }
            value_type * clone() const
            { return static_cast<value_type*> (Inherited::clone()); }
            static bool equal_type(const ASN1::AbstractData& type)
            { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
            static const InfoType theInfo;
        }; // end class value_type

        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class sigla

    sigla::const_reference get_sigla () const { return *static_cast<sigla::const_pointer>(fields[1]); }
    sigla::reference ref_sigla () { return *static_cast<sigla::pointer>(fields[1]); }
    sigla::reference set_sigla () { return *static_cast<sigla::pointer>(fields[1]); }
    sigla::reference set_sigla (const ASN1_STD string& v)
    { return *static_cast<sigla::pointer>(fields[1]) = v; }

    class nome
    {
    public:
        //
        // ModuloFederacoes::Federacao::nome::value_type
        //

        class value_type : public ASN1::GeneralString
        {
            typedef ASN1::GeneralString Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type() : Inherited(&theInfo) {}
            value_type(const base_string& str, const void* info = &theInfo) : Inherited(str, info) {}
            value_type(const char* str, const void* info = &theInfo) : Inherited(str, info) {}
            value_type(const value_type& that) : Inherited(that) {}
            value_type& operator=(const ASN1_STD string& that)
            { Inherited::operator=(that); return *this; }
            value_type& operator=(const char* that)
            { Inherited::operator=(that); return *this; }
            value_type * clone() const
            { return static_cast<value_type*> (Inherited::clone()); }
            static bool equal_type(const ASN1::AbstractData& type)
            { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
            static const InfoType theInfo;
        }; // end class value_type

        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class nome

    nome::const_reference get_nome () const { return *static_cast<nome::const_pointer>(fields[2]); }
    nome::reference ref_nome () { return *static_cast<nome::pointer>(fields[2]); }
    nome::reference set_nome () { return *static_cast<nome::pointer>(fields[2]); }
    nome::reference set_nome (const ASN1_STD string& v)
    { return *static_cast<nome::pointer>(fields[2]) = v; }

    class partidos
    {
    public:
        //
        // ModuloFederacoes::Federacao::partidos::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 99>>
        {
            typedef ASN1::SEQUENCE_OF<ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 99>> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 99>& val) : Inherited(n, val) {}
            value_type(const_iterator first, const_iterator last) : Inherited(first, last) {}
            value_type(const value_type& that) : Inherited(that) {}
            value_type * clone() const
            { return static_cast<value_type*> (Inherited::clone()); }
            static bool equal_type(const ASN1::AbstractData& type)
            { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
            static const InfoType theInfo;
        }; // end class value_type

        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class partidos

    partidos::const_reference get_partidos () const { return *static_cast<partidos::const_pointer>(fields[3]); }
    partidos::reference ref_partidos () { return *static_cast<partidos::pointer>(fields[3]); }
    partidos::reference set_partidos () { return *static_cast<partidos::pointer>(fields[3]); }
    void swap(Federacao& that) { Inherited::swap(that); }
    Federacao * clone() const
    { return static_cast<Federacao*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[4];
    static int fieldIds[4];
    static const char* fieldNames[4];
}; // end class Federacao

//
// ModuloFederacoes::EntidadeFederacoes
//

class EntidadeFederacoes : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EntidadeFederacoes(const void* info) : Inherited(info) {}
public:
    EntidadeFederacoes() : Inherited(&theInfo) {}
    EntidadeFederacoes(const EntidadeFederacoes & that) : Inherited(that) {}

    EntidadeFederacoes& operator=(const EntidadeFederacoes& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_federacoes
    };

    class cabecalho
    {
    public:
        typedef ModuloTiposEleitorais::CabecalhoEntidade value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class cabecalho

    cabecalho::const_reference get_cabecalho () const { return *static_cast<cabecalho::const_pointer>(fields[0]); }
    cabecalho::reference ref_cabecalho () { return *static_cast<cabecalho::pointer>(fields[0]); }
    cabecalho::reference set_cabecalho () { return *static_cast<cabecalho::pointer>(fields[0]); }

    class federacoes
    {
    public:
        //
        // ModuloFederacoes::EntidadeFederacoes::federacoes::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ModuloFederacoes::Federacao>
        {
            typedef ASN1::SEQUENCE_OF<ModuloFederacoes::Federacao> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ModuloFederacoes::Federacao& val) : Inherited(n, val) {}
            value_type(const_iterator first, const_iterator last) : Inherited(first, last) {}
            value_type(const value_type& that) : Inherited(that) {}
            value_type * clone() const
            { return static_cast<value_type*> (Inherited::clone()); }
            static bool equal_type(const ASN1::AbstractData& type)
            { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
            static const InfoType theInfo;
        }; // end class value_type

        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class federacoes

    federacoes::const_reference get_federacoes () const
    {
      assert(hasOptionalField(e_federacoes));
      return *static_cast<federacoes::const_pointer>(fields[1]);
    }
    federacoes::reference ref_federacoes ()
    {
      assert(hasOptionalField(e_federacoes));
      return *static_cast<federacoes::pointer>(fields[1]);
    }
    federacoes::reference set_federacoes ()
    {
      includeOptionalField( e_federacoes, 1);
      return *static_cast<federacoes::pointer>(fields[1]);
    }
    void omit_federacoes () { removeOptionalField( e_federacoes); }
    bool federacoes_isPresent () const { return hasOptionalField( e_federacoes); }
    void swap(EntidadeFederacoes& that) { Inherited::swap(that); }
    EntidadeFederacoes * clone() const
    { return static_cast<EntidadeFederacoes*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[2];
    static int fieldIds[2];
    static const char* fieldNames[2];
}; // end class EntidadeFederacoes

} // namespace ModuloFederacoes

#endif // __MODULOFEDERACOES_H
