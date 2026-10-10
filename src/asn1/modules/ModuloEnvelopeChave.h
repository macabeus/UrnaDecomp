#ifndef __MODULOENVELOPECHAVE_H
#define __MODULOENVELOPECHAVE_H

#include "asn1.h"

#include "ModuloTiposEleitorais.h"

namespace ModuloEnvelopeChave {

//
// ModuloEnvelopeChave::DescritorChave
//

class DescritorChave : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    DescritorChave(const void* info) : Inherited(info) {}
public:
    DescritorChave() : Inherited(&theInfo) {}
    DescritorChave(const DescritorChave & that) : Inherited(that) {}

    DescritorChave& operator=(const DescritorChave& that)
    { Inherited::operator=(that); return *this; }


    class nomeUsuario
    {
    public:
        typedef ASN1::GeneralString value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class nomeUsuario

    nomeUsuario::const_reference get_nomeUsuario () const { return *static_cast<nomeUsuario::const_pointer>(fields[0]); }
    nomeUsuario::reference ref_nomeUsuario () { return *static_cast<nomeUsuario::pointer>(fields[0]); }
    nomeUsuario::reference set_nomeUsuario () { return *static_cast<nomeUsuario::pointer>(fields[0]); }
    nomeUsuario::reference set_nomeUsuario (const ASN1_STD string& v)
    { return *static_cast<nomeUsuario::pointer>(fields[0]) = v; }

    class serial
    {
    public:
        typedef ASN1::INTEGER value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class serial

    serial::const_reference get_serial () const { return *static_cast<serial::const_pointer>(fields[1]); }
    serial::reference ref_serial () { return *static_cast<serial::pointer>(fields[1]); }
    serial::reference set_serial () { return *static_cast<serial::pointer>(fields[1]); }
    serial::reference set_serial (ASN1::INTEGER::int_type v)
    { return *static_cast<serial::pointer>(fields[1]) = v; }
    void swap(DescritorChave& that) { Inherited::swap(that); }
    DescritorChave * clone() const
    { return static_cast<DescritorChave*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[2];
    static int fieldIds[2];
    static const char* fieldNames[2];
}; // end class DescritorChave

//
// ModuloEnvelopeChave::EntidadeChave
//

class EntidadeChave : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EntidadeChave(const void* info) : Inherited(info) {}
public:
    EntidadeChave() : Inherited(&theInfo) {}
    EntidadeChave(const EntidadeChave & that) : Inherited(that) {}

    EntidadeChave& operator=(const EntidadeChave& that)
    { Inherited::operator=(that); return *this; }


    class tagChaves
    {
    public:
        typedef ModuloTiposEleitorais::Versao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class tagChaves

    tagChaves::const_reference get_tagChaves () const { return *static_cast<tagChaves::const_pointer>(fields[0]); }
    tagChaves::reference ref_tagChaves () { return *static_cast<tagChaves::pointer>(fields[0]); }
    tagChaves::reference set_tagChaves () { return *static_cast<tagChaves::pointer>(fields[0]); }
    tagChaves::reference set_tagChaves (const ASN1_STD string& v)
    { return *static_cast<tagChaves::pointer>(fields[0]) = v; }

    class descritor
    {
    public:
        typedef ModuloEnvelopeChave::DescritorChave value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class descritor

    descritor::const_reference get_descritor () const { return *static_cast<descritor::const_pointer>(fields[1]); }
    descritor::reference ref_descritor () { return *static_cast<descritor::pointer>(fields[1]); }
    descritor::reference set_descritor () { return *static_cast<descritor::pointer>(fields[1]); }

    class cifrado
    {
    public:
        typedef ASN1::BOOLEAN value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class cifrado

    cifrado::const_reference get_cifrado () const { return *static_cast<cifrado::const_pointer>(fields[2]); }
    cifrado::reference ref_cifrado () { return *static_cast<cifrado::pointer>(fields[2]); }
    cifrado::reference set_cifrado () { return *static_cast<cifrado::pointer>(fields[2]); }
    cifrado::reference set_cifrado (bool v)
    { return *static_cast<cifrado::pointer>(fields[2]) = v; }

    class tipo
    {
    public:
        //
        // ModuloEnvelopeChave::EntidadeChave::tipo::value_type
        //

        class value_type : public ASN1::ENUMERATED
        {
            typedef ASN1::ENUMERATED Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type() : Inherited(&theInfo) {}
            enum NamedNumber {
                unknownEnumeration_ = -1,
                chaveSecreta,
                chavePublica
            };

            bool is_chaveSecreta() const { return value == chaveSecreta; }
            void set_chaveSecreta() { value = chaveSecreta; }

            bool is_chavePublica() const { return value == chavePublica; }
            void set_chavePublica() { value = chavePublica; }

            value_type(const NamedNumber v) : Inherited(&theInfo) { setFromInt(v); }
            value_type & operator=(const NamedNumber v) { setFromInt(v);  return *this; }
            operator NamedNumber() const { return NamedNumber(asInt()); }

            bool operator == (NamedNumber rhs) const { return value == rhs; }
            bool operator != (NamedNumber rhs) const { return value != rhs; }
            bool operator <  (NamedNumber rhs) const { return value <  rhs; }
            bool operator >  (NamedNumber rhs) const { return value >  rhs; }
            bool operator <= (NamedNumber rhs) const { return value <= rhs; }
            bool operator >= (NamedNumber rhs) const { return value >= rhs; }
            bool operator == (const value_type &rhs) const { return value == rhs.value; }
            bool operator != (const value_type &rhs) const { return value != rhs.value; }

            void swap (value_type & that) { Inherited::swap(that); }
            value_type * clone() const
            { return static_cast<value_type*> (Inherited::clone()); }
            static bool equal_type(const ASN1::AbstractData& type)
            { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
            static const InfoType theInfo;
        private:
            static const char * nameList[];
        }; // end class value_type

        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class tipo

    tipo::const_reference get_tipo () const { return *static_cast<tipo::const_pointer>(fields[3]); }
    tipo::reference ref_tipo () { return *static_cast<tipo::pointer>(fields[3]); }
    tipo::reference set_tipo () { return *static_cast<tipo::pointer>(fields[3]); }
    tipo::reference set_tipo (tipo::value_type::NamedNumber v)
    { return *static_cast<tipo::pointer>(fields[3]) = v; }

    class chave
    {
    public:
        typedef ASN1::OCTET_STRING value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class chave

    chave::const_reference get_chave () const { return *static_cast<chave::const_pointer>(fields[4]); }
    chave::reference ref_chave () { return *static_cast<chave::pointer>(fields[4]); }
    chave::reference set_chave () { return *static_cast<chave::pointer>(fields[4]); }
    chave::reference set_chave (const ASN1::OCTET_STRING& v)
    { return *static_cast<chave::pointer>(fields[4]) = v; }
    void swap(EntidadeChave& that) { Inherited::swap(that); }
    EntidadeChave * clone() const
    { return static_cast<EntidadeChave*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[5];
    static int fieldIds[5];
    static const char* fieldNames[5];
}; // end class EntidadeChave

} // namespace ModuloEnvelopeChave

#endif // __MODULOENVELOPECHAVE_H
