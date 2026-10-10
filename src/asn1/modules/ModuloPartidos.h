#ifndef __MODULOPARTIDOS_H
#define __MODULOPARTIDOS_H

#include "asn1.h"

#include "ModuloTiposEleitorais.h"

namespace ModuloPartidos {

//
// ModuloPartidos::Partido
//

class Partido : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    Partido(const void* info) : Inherited(info) {}
public:
    Partido() : Inherited(&theInfo) {}
    Partido(const Partido & that) : Inherited(that) {}

    Partido& operator=(const Partido& that)
    { Inherited::operator=(that); return *this; }


    class numero
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 99> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class numero

    numero::const_reference get_numero () const { return *static_cast<numero::const_pointer>(fields[0]); }
    numero::reference ref_numero () { return *static_cast<numero::pointer>(fields[0]); }
    numero::reference set_numero () { return *static_cast<numero::pointer>(fields[0]); }
    numero::reference set_numero (ASN1::INTEGER::int_type v)
    { return *static_cast<numero::pointer>(fields[0]) = v; }

    class sigla
    {
    public:
        //
        // ModuloPartidos::Partido::sigla::value_type
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
        // ModuloPartidos::Partido::nome::value_type
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
    void swap(Partido& that) { Inherited::swap(that); }
    Partido * clone() const
    { return static_cast<Partido*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[3];
    static int fieldIds[3];
    static const char* fieldNames[3];
}; // end class Partido

//
// ModuloPartidos::EntidadePartidos
//

class EntidadePartidos : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EntidadePartidos(const void* info) : Inherited(info) {}
public:
    EntidadePartidos() : Inherited(&theInfo) {}
    EntidadePartidos(const EntidadePartidos & that) : Inherited(that) {}

    EntidadePartidos& operator=(const EntidadePartidos& that)
    { Inherited::operator=(that); return *this; }


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

    class abrangencia
    {
    public:
        typedef ModuloTiposEleitorais::Abrangencia value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class abrangencia

    abrangencia::const_reference get_abrangencia () const { return *static_cast<abrangencia::const_pointer>(fields[1]); }
    abrangencia::reference ref_abrangencia () { return *static_cast<abrangencia::pointer>(fields[1]); }
    abrangencia::reference set_abrangencia () { return *static_cast<abrangencia::pointer>(fields[1]); }

    class partidos
    {
    public:
        //
        // ModuloPartidos::EntidadePartidos::partidos::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ModuloPartidos::Partido>
        {
            typedef ASN1::SEQUENCE_OF<ModuloPartidos::Partido> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ModuloPartidos::Partido& val) : Inherited(n, val) {}
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

    partidos::const_reference get_partidos () const { return *static_cast<partidos::const_pointer>(fields[2]); }
    partidos::reference ref_partidos () { return *static_cast<partidos::pointer>(fields[2]); }
    partidos::reference set_partidos () { return *static_cast<partidos::pointer>(fields[2]); }
    void swap(EntidadePartidos& that) { Inherited::swap(that); }
    EntidadePartidos * clone() const
    { return static_cast<EntidadePartidos*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[3];
    static int fieldIds[3];
    static const char* fieldNames[3];
}; // end class EntidadePartidos

} // namespace ModuloPartidos

#endif // __MODULOPARTIDOS_H
