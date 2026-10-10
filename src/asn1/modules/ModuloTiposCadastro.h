#ifndef __MODULOTIPOSCADASTRO_H
#define __MODULOTIPOSCADASTRO_H

#include "asn1.h"

namespace ModuloTiposCadastro {

//
// ModuloTiposCadastro::TipoLocalVotacao
//

class TipoLocalVotacao : public ASN1::ENUMERATED
{
    typedef ASN1::ENUMERATED Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    TipoLocalVotacao(const void* info) : Inherited(info) {}
public:
    TipoLocalVotacao() : Inherited(&theInfo) {}
    enum NamedNumber {
        unknownEnumeration_ = -1,
        normal = 1,
        emTransito,
        presoProvisorio,
        temporario
    };

    bool is_normal() const { return value == normal; }
    void set_normal() { value = normal; }

    bool is_emTransito() const { return value == emTransito; }
    void set_emTransito() { value = emTransito; }

    bool is_presoProvisorio() const { return value == presoProvisorio; }
    void set_presoProvisorio() { value = presoProvisorio; }

    bool is_temporario() const { return value == temporario; }
    void set_temporario() { value = temporario; }

    TipoLocalVotacao(const NamedNumber v) : Inherited(&theInfo) { setFromInt(v); }
    TipoLocalVotacao & operator=(const NamedNumber v) { setFromInt(v);  return *this; }
    operator NamedNumber() const { return NamedNumber(asInt()); }

    bool operator == (NamedNumber rhs) const { return value == rhs; }
    bool operator != (NamedNumber rhs) const { return value != rhs; }
    bool operator <  (NamedNumber rhs) const { return value <  rhs; }
    bool operator >  (NamedNumber rhs) const { return value >  rhs; }
    bool operator <= (NamedNumber rhs) const { return value <= rhs; }
    bool operator >= (NamedNumber rhs) const { return value >= rhs; }
    bool operator == (const TipoLocalVotacao &rhs) const { return value == rhs.value; }
    bool operator != (const TipoLocalVotacao &rhs) const { return value != rhs.value; }

    void swap (TipoLocalVotacao & that) { Inherited::swap(that); }
    TipoLocalVotacao * clone() const
    { return static_cast<TipoLocalVotacao*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const char * nameList[];
}; // end class TipoLocalVotacao

//
// ModuloTiposCadastro::Municipio
//

class Municipio : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    Municipio(const void* info) : Inherited(info) {}
public:
    Municipio() : Inherited(&theInfo) {}
    Municipio(const Municipio & that) : Inherited(that) {}

    Municipio& operator=(const Municipio& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_codigoIBGE,
      e_comBiometria
    };

    class codigo
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 99999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class codigo

    codigo::const_reference get_codigo () const { return *static_cast<codigo::const_pointer>(fields[0]); }
    codigo::reference ref_codigo () { return *static_cast<codigo::pointer>(fields[0]); }
    codigo::reference set_codigo () { return *static_cast<codigo::pointer>(fields[0]); }
    codigo::reference set_codigo (ASN1::INTEGER::int_type v)
    { return *static_cast<codigo::pointer>(fields[0]) = v; }

    class nome
    {
    public:
        //
        // ModuloTiposCadastro::Municipio::nome::value_type
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

    nome::const_reference get_nome () const { return *static_cast<nome::const_pointer>(fields[1]); }
    nome::reference ref_nome () { return *static_cast<nome::pointer>(fields[1]); }
    nome::reference set_nome () { return *static_cast<nome::pointer>(fields[1]); }
    nome::reference set_nome (const ASN1_STD string& v)
    { return *static_cast<nome::pointer>(fields[1]) = v; }

    class capital
    {
    public:
        typedef ASN1::BOOLEAN value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class capital

    capital::const_reference get_capital () const { return *static_cast<capital::const_pointer>(fields[2]); }
    capital::reference ref_capital () { return *static_cast<capital::pointer>(fields[2]); }
    capital::reference set_capital () { return *static_cast<capital::pointer>(fields[2]); }
    capital::reference set_capital (bool v)
    { return *static_cast<capital::pointer>(fields[2]) = v; }

    class codigoIBGE
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1100000, 9999999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class codigoIBGE

    codigoIBGE::const_reference get_codigoIBGE () const
    {
      assert(hasOptionalField(e_codigoIBGE));
      return *static_cast<codigoIBGE::const_pointer>(fields[3]);
    }
    codigoIBGE::reference ref_codigoIBGE ()
    {
      assert(hasOptionalField(e_codigoIBGE));
      return *static_cast<codigoIBGE::pointer>(fields[3]);
    }
    codigoIBGE::reference set_codigoIBGE ()
    {
      includeOptionalField( e_codigoIBGE, 3);
      return *static_cast<codigoIBGE::pointer>(fields[3]);
    }
    codigoIBGE::reference set_codigoIBGE (ASN1::INTEGER::int_type v)
    {
      includeOptionalField( e_codigoIBGE, 3);
      return *static_cast<codigoIBGE::pointer>(fields[3]) = v;
    }
    void omit_codigoIBGE () { removeOptionalField( e_codigoIBGE); }
    bool codigoIBGE_isPresent () const { return hasOptionalField( e_codigoIBGE); }

    class comBiometria
    {
    public:
        typedef ASN1::BOOLEAN value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class comBiometria

    comBiometria::const_reference get_comBiometria () const
    {
      assert(hasOptionalField(e_comBiometria));
      return *static_cast<comBiometria::const_pointer>(fields[4]);
    }
    comBiometria::reference ref_comBiometria ()
    {
      assert(hasOptionalField(e_comBiometria));
      return *static_cast<comBiometria::pointer>(fields[4]);
    }
    comBiometria::reference set_comBiometria ()
    {
      includeOptionalField( e_comBiometria, 4);
      return *static_cast<comBiometria::pointer>(fields[4]);
    }
    comBiometria::reference set_comBiometria (bool v)
    {
      includeOptionalField( e_comBiometria, 4);
      return *static_cast<comBiometria::pointer>(fields[4]) = v;
    }
    void omit_comBiometria () { removeOptionalField( e_comBiometria); }
    bool comBiometria_isPresent () const { return hasOptionalField( e_comBiometria); }
    void swap(Municipio& that) { Inherited::swap(that); }
    Municipio * clone() const
    { return static_cast<Municipio*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[5];
    static int fieldIds[5];
    static unsigned fieldTags[5];
    static const char* fieldNames[5];
}; // end class Municipio

} // namespace ModuloTiposCadastro

#endif // __MODULOTIPOSCADASTRO_H
