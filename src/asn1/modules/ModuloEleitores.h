#ifndef __MODULOELEITORES_H
#define __MODULOELEITORES_H

#include "asn1.h"

#include "ModuloComplementosMunicipios.h"
#include "ModuloTiposEleitorais.h"

namespace ModuloEleitores {

//
// ModuloEleitores::Dedo
//

class Dedo : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    Dedo(const void* info) : Inherited(info) {}
public:
    Dedo() : Inherited(&theInfo) {}
    Dedo(const Dedo & that) : Inherited(that) {}

    Dedo& operator=(const Dedo& that)
    { Inherited::operator=(that); return *this; }


    class tipo
    {
    public:
        typedef ModuloTiposEleitorais::TipoDedo value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class tipo

    tipo::const_reference get_tipo () const { return *static_cast<tipo::const_pointer>(fields[0]); }
    tipo::reference ref_tipo () { return *static_cast<tipo::pointer>(fields[0]); }
    tipo::reference set_tipo () { return *static_cast<tipo::pointer>(fields[0]); }
    tipo::reference set_tipo (ModuloTiposEleitorais::TipoDedo::NamedNumber v)
    { return *static_cast<tipo::pointer>(fields[0]) = v; }

    class qtdMinucias
    {
    public:
        typedef ASN1::INTEGER value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class qtdMinucias

    qtdMinucias::const_reference get_qtdMinucias () const { return *static_cast<qtdMinucias::const_pointer>(fields[1]); }
    qtdMinucias::reference ref_qtdMinucias () { return *static_cast<qtdMinucias::pointer>(fields[1]); }
    qtdMinucias::reference set_qtdMinucias () { return *static_cast<qtdMinucias::pointer>(fields[1]); }
    qtdMinucias::reference set_qtdMinucias (ASN1::INTEGER::int_type v)
    { return *static_cast<qtdMinucias::pointer>(fields[1]) = v; }

    class minucias
    {
    public:
        typedef ASN1::OCTET_STRING value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class minucias

    minucias::const_reference get_minucias () const { return *static_cast<minucias::const_pointer>(fields[2]); }
    minucias::reference ref_minucias () { return *static_cast<minucias::pointer>(fields[2]); }
    minucias::reference set_minucias () { return *static_cast<minucias::pointer>(fields[2]); }
    minucias::reference set_minucias (const ASN1::OCTET_STRING& v)
    { return *static_cast<minucias::pointer>(fields[2]) = v; }
    void swap(Dedo& that) { Inherited::swap(that); }
    Dedo * clone() const
    { return static_cast<Dedo*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[3];
    static int fieldIds[3];
    static const char* fieldNames[3];
}; // end class Dedo

//
// ModuloEleitores::BiometriaEleitorCifrada
//

class BiometriaEleitorCifrada : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    BiometriaEleitorCifrada(const void* info) : Inherited(info) {}
public:
    BiometriaEleitorCifrada() : Inherited(&theInfo) {}
    BiometriaEleitorCifrada(const BiometriaEleitorCifrada & that) : Inherited(that) {}

    BiometriaEleitorCifrada& operator=(const BiometriaEleitorCifrada& that)
    { Inherited::operator=(that); return *this; }


    class composicaoBiometria
    {
    public:
        //
        // ModuloEleitores::BiometriaEleitorCifrada::composicaoBiometria::value_type
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
                foto = 1,
                dedos,
                fotoDedos
            };

            bool is_foto() const { return value == foto; }
            void set_foto() { value = foto; }

            bool is_dedos() const { return value == dedos; }
            void set_dedos() { value = dedos; }

            bool is_fotoDedos() const { return value == fotoDedos; }
            void set_fotoDedos() { value = fotoDedos; }

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
    }; // end class composicaoBiometria

    composicaoBiometria::const_reference get_composicaoBiometria () const { return *static_cast<composicaoBiometria::const_pointer>(fields[0]); }
    composicaoBiometria::reference ref_composicaoBiometria () { return *static_cast<composicaoBiometria::pointer>(fields[0]); }
    composicaoBiometria::reference set_composicaoBiometria () { return *static_cast<composicaoBiometria::pointer>(fields[0]); }
    composicaoBiometria::reference set_composicaoBiometria (composicaoBiometria::value_type::NamedNumber v)
    { return *static_cast<composicaoBiometria::pointer>(fields[0]) = v; }

    class conteudo
    {
    public:
        typedef ASN1::OCTET_STRING value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class conteudo

    conteudo::const_reference get_conteudo () const { return *static_cast<conteudo::const_pointer>(fields[1]); }
    conteudo::reference ref_conteudo () { return *static_cast<conteudo::pointer>(fields[1]); }
    conteudo::reference set_conteudo () { return *static_cast<conteudo::pointer>(fields[1]); }
    conteudo::reference set_conteudo (const ASN1::OCTET_STRING& v)
    { return *static_cast<conteudo::pointer>(fields[1]) = v; }

    class salt
    {
    public:
        typedef ASN1::OCTET_STRING value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class salt

    salt::const_reference get_salt () const { return *static_cast<salt::const_pointer>(fields[2]); }
    salt::reference ref_salt () { return *static_cast<salt::pointer>(fields[2]); }
    salt::reference set_salt () { return *static_cast<salt::pointer>(fields[2]); }
    salt::reference set_salt (const ASN1::OCTET_STRING& v)
    { return *static_cast<salt::pointer>(fields[2]) = v; }
    void swap(BiometriaEleitorCifrada& that) { Inherited::swap(that); }
    BiometriaEleitorCifrada * clone() const
    { return static_cast<BiometriaEleitorCifrada*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[3];
    static int fieldIds[3];
    static const char* fieldNames[3];
}; // end class BiometriaEleitorCifrada

//
// ModuloEleitores::BiometriaEleitor
//

class BiometriaEleitor : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    BiometriaEleitor(const void* info) : Inherited(info) {}
public:
    BiometriaEleitor() : Inherited(&theInfo) {}
    BiometriaEleitor(const BiometriaEleitor & that) : Inherited(that) {}

    BiometriaEleitor& operator=(const BiometriaEleitor& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_foto,
      e_dedos
    };

    class foto
    {
    public:
        typedef ModuloTiposEleitorais::Foto value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class foto

    foto::const_reference get_foto () const
    {
      assert(hasOptionalField(e_foto));
      return *static_cast<foto::const_pointer>(fields[0]);
    }
    foto::reference ref_foto ()
    {
      assert(hasOptionalField(e_foto));
      return *static_cast<foto::pointer>(fields[0]);
    }
    foto::reference set_foto ()
    {
      includeOptionalField( e_foto, 0);
      return *static_cast<foto::pointer>(fields[0]);
    }
    void omit_foto () { removeOptionalField( e_foto); }
    bool foto_isPresent () const { return hasOptionalField( e_foto); }

    class dedos
    {
    public:
        //
        // ModuloEleitores::BiometriaEleitor::dedos::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ModuloEleitores::Dedo>
        {
            typedef ASN1::SEQUENCE_OF<ModuloEleitores::Dedo> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ModuloEleitores::Dedo& val) : Inherited(n, val) {}
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
    }; // end class dedos

    dedos::const_reference get_dedos () const
    {
      assert(hasOptionalField(e_dedos));
      return *static_cast<dedos::const_pointer>(fields[1]);
    }
    dedos::reference ref_dedos ()
    {
      assert(hasOptionalField(e_dedos));
      return *static_cast<dedos::pointer>(fields[1]);
    }
    dedos::reference set_dedos ()
    {
      includeOptionalField( e_dedos, 1);
      return *static_cast<dedos::pointer>(fields[1]);
    }
    void omit_dedos () { removeOptionalField( e_dedos); }
    bool dedos_isPresent () const { return hasOptionalField( e_dedos); }
    void swap(BiometriaEleitor& that) { Inherited::swap(that); }
    BiometriaEleitor * clone() const
    { return static_cast<BiometriaEleitor*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[2];
    static int fieldIds[2];
    static unsigned fieldTags[2];
    static const char* fieldNames[2];
}; // end class BiometriaEleitor

//
// ModuloEleitores::DomicilioEleitoral
//

class DomicilioEleitoral : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    DomicilioEleitoral(const void* info) : Inherited(info) {}
public:
    DomicilioEleitoral() : Inherited(&theInfo) {}
    DomicilioEleitoral(const DomicilioEleitoral & that) : Inherited(that) {}

    DomicilioEleitoral& operator=(const DomicilioEleitoral& that)
    { Inherited::operator=(that); return *this; }


    class uf
    {
    public:
        typedef ModuloTiposEleitorais::SiglaUF value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class uf

    uf::const_reference get_uf () const { return *static_cast<uf::const_pointer>(fields[0]); }
    uf::reference ref_uf () { return *static_cast<uf::pointer>(fields[0]); }
    uf::reference set_uf () { return *static_cast<uf::pointer>(fields[0]); }
    uf::reference set_uf (const ASN1_STD string& v)
    { return *static_cast<uf::pointer>(fields[0]) = v; }

    class zona
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 9999> value_type;
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
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 9999> value_type;
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

    class dataNaUF
    {
    public:
        typedef ModuloComplementosMunicipios::InicioHV value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class dataNaUF

    dataNaUF::const_reference get_dataNaUF () const { return *static_cast<dataNaUF::const_pointer>(fields[3]); }
    dataNaUF::reference ref_dataNaUF () { return *static_cast<dataNaUF::pointer>(fields[3]); }
    dataNaUF::reference set_dataNaUF () { return *static_cast<dataNaUF::pointer>(fields[3]); }
    dataNaUF::reference set_dataNaUF (const ASN1_STD string& v)
    { return *static_cast<dataNaUF::pointer>(fields[3]) = v; }

    class dataNoMunicipio
    {
    public:
        typedef ModuloComplementosMunicipios::InicioHV value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class dataNoMunicipio

    dataNoMunicipio::const_reference get_dataNoMunicipio () const { return *static_cast<dataNoMunicipio::const_pointer>(fields[4]); }
    dataNoMunicipio::reference ref_dataNoMunicipio () { return *static_cast<dataNoMunicipio::pointer>(fields[4]); }
    dataNoMunicipio::reference set_dataNoMunicipio () { return *static_cast<dataNoMunicipio::pointer>(fields[4]); }
    dataNoMunicipio::reference set_dataNoMunicipio (const ASN1_STD string& v)
    { return *static_cast<dataNoMunicipio::pointer>(fields[4]) = v; }

    class codigoMunicipio
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 99999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class codigoMunicipio

    codigoMunicipio::const_reference get_codigoMunicipio () const { return *static_cast<codigoMunicipio::const_pointer>(fields[5]); }
    codigoMunicipio::reference ref_codigoMunicipio () { return *static_cast<codigoMunicipio::pointer>(fields[5]); }
    codigoMunicipio::reference set_codigoMunicipio () { return *static_cast<codigoMunicipio::pointer>(fields[5]); }
    codigoMunicipio::reference set_codigoMunicipio (ASN1::INTEGER::int_type v)
    { return *static_cast<codigoMunicipio::pointer>(fields[5]) = v; }
    void swap(DomicilioEleitoral& that) { Inherited::swap(that); }
    DomicilioEleitoral * clone() const
    { return static_cast<DomicilioEleitoral*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[6];
    static int fieldIds[6];
    static const char* fieldNames[6];
}; // end class DomicilioEleitoral

//
// ModuloEleitores::EleitorUrna
//

class EleitorUrna : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EleitorUrna(const void* info) : Inherited(info) {}
public:
    EleitorUrna() : Inherited(&theInfo) {}
    EleitorUrna(const EleitorUrna & that) : Inherited(that) {}

    EleitorUrna& operator=(const EleitorUrna& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_nomeSocial,
      e_domicilio,
      e_biometria
    };

    class identificacaoEleitor
    {
    public:
        //
        // ModuloEleitores::EleitorUrna::identificacaoEleitor::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ModuloTiposEleitorais::IdentificadorEleitor>
        {
            typedef ASN1::SEQUENCE_OF<ModuloTiposEleitorais::IdentificadorEleitor> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ModuloTiposEleitorais::IdentificadorEleitor& val) : Inherited(n, val) {}
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
    }; // end class identificacaoEleitor

    identificacaoEleitor::const_reference get_identificacaoEleitor () const { return *static_cast<identificacaoEleitor::const_pointer>(fields[0]); }
    identificacaoEleitor::reference ref_identificacaoEleitor () { return *static_cast<identificacaoEleitor::pointer>(fields[0]); }
    identificacaoEleitor::reference set_identificacaoEleitor () { return *static_cast<identificacaoEleitor::pointer>(fields[0]); }

    class nome
    {
    public:
        //
        // ModuloEleitores::EleitorUrna::nome::value_type
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

    class nomeSocial
    {
    public:
        //
        // ModuloEleitores::EleitorUrna::nomeSocial::value_type
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
    }; // end class nomeSocial

    nomeSocial::const_reference get_nomeSocial () const
    {
      assert(hasOptionalField(e_nomeSocial));
      return *static_cast<nomeSocial::const_pointer>(fields[2]);
    }
    nomeSocial::reference ref_nomeSocial ()
    {
      assert(hasOptionalField(e_nomeSocial));
      return *static_cast<nomeSocial::pointer>(fields[2]);
    }
    nomeSocial::reference set_nomeSocial ()
    {
      includeOptionalField( e_nomeSocial, 2);
      return *static_cast<nomeSocial::pointer>(fields[2]);
    }
    nomeSocial::reference set_nomeSocial (const ASN1_STD string& v)
    {
      includeOptionalField( e_nomeSocial, 2);
      return *static_cast<nomeSocial::pointer>(fields[2]) = v;
    }
    void omit_nomeSocial () { removeOptionalField( e_nomeSocial); }
    bool nomeSocial_isPresent () const { return hasOptionalField( e_nomeSocial); }

    class nomeMae
    {
    public:
        //
        // ModuloEleitores::EleitorUrna::nomeMae::value_type
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
    }; // end class nomeMae

    nomeMae::const_reference get_nomeMae () const { return *static_cast<nomeMae::const_pointer>(fields[3]); }
    nomeMae::reference ref_nomeMae () { return *static_cast<nomeMae::pointer>(fields[3]); }
    nomeMae::reference set_nomeMae () { return *static_cast<nomeMae::pointer>(fields[3]); }
    nomeMae::reference set_nomeMae (const ASN1_STD string& v)
    { return *static_cast<nomeMae::pointer>(fields[3]) = v; }

    class dataNascimento
    {
    public:
        typedef ModuloComplementosMunicipios::InicioHV value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class dataNascimento

    dataNascimento::const_reference get_dataNascimento () const { return *static_cast<dataNascimento::const_pointer>(fields[4]); }
    dataNascimento::reference ref_dataNascimento () { return *static_cast<dataNascimento::pointer>(fields[4]); }
    dataNascimento::reference set_dataNascimento () { return *static_cast<dataNascimento::pointer>(fields[4]); }
    dataNascimento::reference set_dataNascimento (const ASN1_STD string& v)
    { return *static_cast<dataNascimento::pointer>(fields[4]) = v; }

    class situacao
    {
    public:
        //
        // ModuloEleitores::EleitorUrna::situacao::value_type
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
                regular,
                cancelado = 4,
                suspenso = 6,
                envolvidoEmCoincidenciaNaoLiberado,
                envolvidoEmCoincidenciaLiberado = 9
            };

            bool is_regular() const { return value == regular; }
            void set_regular() { value = regular; }

            bool is_cancelado() const { return value == cancelado; }
            void set_cancelado() { value = cancelado; }

            bool is_suspenso() const { return value == suspenso; }
            void set_suspenso() { value = suspenso; }

            bool is_envolvidoEmCoincidenciaNaoLiberado() const { return value == envolvidoEmCoincidenciaNaoLiberado; }
            void set_envolvidoEmCoincidenciaNaoLiberado() { value = envolvidoEmCoincidenciaNaoLiberado; }

            bool is_envolvidoEmCoincidenciaLiberado() const { return value == envolvidoEmCoincidenciaLiberado; }
            void set_envolvidoEmCoincidenciaLiberado() { value = envolvidoEmCoincidenciaLiberado; }

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
    }; // end class situacao

    situacao::const_reference get_situacao () const { return *static_cast<situacao::const_pointer>(fields[5]); }
    situacao::reference ref_situacao () { return *static_cast<situacao::pointer>(fields[5]); }
    situacao::reference set_situacao () { return *static_cast<situacao::pointer>(fields[5]); }
    situacao::reference set_situacao (situacao::value_type::NamedNumber v)
    { return *static_cast<situacao::pointer>(fields[5]) = v; }

    class domicilio
    {
    public:
        typedef ModuloEleitores::DomicilioEleitoral value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class domicilio

    domicilio::const_reference get_domicilio () const
    {
      assert(hasOptionalField(e_domicilio));
      return *static_cast<domicilio::const_pointer>(fields[6]);
    }
    domicilio::reference ref_domicilio ()
    {
      assert(hasOptionalField(e_domicilio));
      return *static_cast<domicilio::pointer>(fields[6]);
    }
    domicilio::reference set_domicilio ()
    {
      includeOptionalField( e_domicilio, 6);
      return *static_cast<domicilio::pointer>(fields[6]);
    }
    void omit_domicilio () { removeOptionalField( e_domicilio); }
    bool domicilio_isPresent () const { return hasOptionalField( e_domicilio); }

    class necessidade
    {
    public:
        //
        // ModuloEleitores::EleitorUrna::necessidade::value_type
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
                semNecessidade = 1,
                necessitaAudio
            };

            bool is_semNecessidade() const { return value == semNecessidade; }
            void set_semNecessidade() { value = semNecessidade; }

            bool is_necessitaAudio() const { return value == necessitaAudio; }
            void set_necessitaAudio() { value = necessitaAudio; }

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
    }; // end class necessidade

    necessidade::const_reference get_necessidade () const { return *static_cast<necessidade::const_pointer>(fields[7]); }
    necessidade::reference ref_necessidade () { return *static_cast<necessidade::pointer>(fields[7]); }
    necessidade::reference set_necessidade () { return *static_cast<necessidade::pointer>(fields[7]); }
    necessidade::reference set_necessidade (necessidade::value_type::NamedNumber v)
    { return *static_cast<necessidade::pointer>(fields[7]) = v; }

    class biometria
    {
    public:
        typedef ModuloEleitores::BiometriaEleitorCifrada value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class biometria

    biometria::const_reference get_biometria () const
    {
      assert(hasOptionalField(e_biometria));
      return *static_cast<biometria::const_pointer>(fields[8]);
    }
    biometria::reference ref_biometria ()
    {
      assert(hasOptionalField(e_biometria));
      return *static_cast<biometria::pointer>(fields[8]);
    }
    biometria::reference set_biometria ()
    {
      includeOptionalField( e_biometria, 8);
      return *static_cast<biometria::pointer>(fields[8]);
    }
    void omit_biometria () { removeOptionalField( e_biometria); }
    bool biometria_isPresent () const { return hasOptionalField( e_biometria); }
    void swap(EleitorUrna& that) { Inherited::swap(that); }
    EleitorUrna * clone() const
    { return static_cast<EleitorUrna*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[9];
    static int fieldIds[9];
    static unsigned fieldTags[9];
    static const char* fieldNames[9];
}; // end class EleitorUrna

//
// ModuloEleitores::EleitorSequencia
//

class EleitorSequencia : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EleitorSequencia(const void* info) : Inherited(info) {}
public:
    EleitorSequencia() : Inherited(&theInfo) {}
    EleitorSequencia(const EleitorSequencia & that) : Inherited(that) {}

    EleitorSequencia& operator=(const EleitorSequencia& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_tipoTransferenciaTemporaria
    };

    class sequencial
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 9999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class sequencial

    sequencial::const_reference get_sequencial () const { return *static_cast<sequencial::const_pointer>(fields[0]); }
    sequencial::reference ref_sequencial () { return *static_cast<sequencial::pointer>(fields[0]); }
    sequencial::reference set_sequencial () { return *static_cast<sequencial::pointer>(fields[0]); }
    sequencial::reference set_sequencial (ASN1::INTEGER::int_type v)
    { return *static_cast<sequencial::pointer>(fields[0]) = v; }

    class eleitor
    {
    public:
        typedef ModuloEleitores::EleitorUrna value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class eleitor

    eleitor::const_reference get_eleitor () const { return *static_cast<eleitor::const_pointer>(fields[1]); }
    eleitor::reference ref_eleitor () { return *static_cast<eleitor::pointer>(fields[1]); }
    eleitor::reference set_eleitor () { return *static_cast<eleitor::pointer>(fields[1]); }

    class tipoTransferenciaTemporaria
    {
    public:
        //
        // ModuloEleitores::EleitorSequencia::tipoTransferenciaTemporaria::value_type
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
                votoEmTransito = 1,
                presoProvisorio,
                servidorEmServico,
                acessibilidade,
                eleitorConvocado,
                justicaEleitoral,
                oficio,
                indigenaQuilombola,
                situacaoRua
            };

            bool is_votoEmTransito() const { return value == votoEmTransito; }
            void set_votoEmTransito() { value = votoEmTransito; }

            bool is_presoProvisorio() const { return value == presoProvisorio; }
            void set_presoProvisorio() { value = presoProvisorio; }

            bool is_servidorEmServico() const { return value == servidorEmServico; }
            void set_servidorEmServico() { value = servidorEmServico; }

            bool is_acessibilidade() const { return value == acessibilidade; }
            void set_acessibilidade() { value = acessibilidade; }

            bool is_eleitorConvocado() const { return value == eleitorConvocado; }
            void set_eleitorConvocado() { value = eleitorConvocado; }

            bool is_justicaEleitoral() const { return value == justicaEleitoral; }
            void set_justicaEleitoral() { value = justicaEleitoral; }

            bool is_oficio() const { return value == oficio; }
            void set_oficio() { value = oficio; }

            bool is_indigenaQuilombola() const { return value == indigenaQuilombola; }
            void set_indigenaQuilombola() { value = indigenaQuilombola; }

            bool is_situacaoRua() const { return value == situacaoRua; }
            void set_situacaoRua() { value = situacaoRua; }

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
    }; // end class tipoTransferenciaTemporaria

    tipoTransferenciaTemporaria::const_reference get_tipoTransferenciaTemporaria () const
    {
      assert(hasOptionalField(e_tipoTransferenciaTemporaria));
      return *static_cast<tipoTransferenciaTemporaria::const_pointer>(fields[2]);
    }
    tipoTransferenciaTemporaria::reference ref_tipoTransferenciaTemporaria ()
    {
      assert(hasOptionalField(e_tipoTransferenciaTemporaria));
      return *static_cast<tipoTransferenciaTemporaria::pointer>(fields[2]);
    }
    tipoTransferenciaTemporaria::reference set_tipoTransferenciaTemporaria ()
    {
      includeOptionalField( e_tipoTransferenciaTemporaria, 2);
      return *static_cast<tipoTransferenciaTemporaria::pointer>(fields[2]);
    }
    tipoTransferenciaTemporaria::reference set_tipoTransferenciaTemporaria (tipoTransferenciaTemporaria::value_type::NamedNumber v)
    {
      includeOptionalField( e_tipoTransferenciaTemporaria, 2);
      return *static_cast<tipoTransferenciaTemporaria::pointer>(fields[2]) = v;
    }
    void omit_tipoTransferenciaTemporaria () { removeOptionalField( e_tipoTransferenciaTemporaria); }
    bool tipoTransferenciaTemporaria_isPresent () const { return hasOptionalField( e_tipoTransferenciaTemporaria); }
    void swap(EleitorSequencia& that) { Inherited::swap(that); }
    EleitorSequencia * clone() const
    { return static_cast<EleitorSequencia*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[3];
    static int fieldIds[3];
    static unsigned fieldTags[3];
    static const char* fieldNames[3];
}; // end class EleitorSequencia

//
// ModuloEleitores::EntidadeEleitores
//

class EntidadeEleitores : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EntidadeEleitores(const void* info) : Inherited(info) {}
public:
    EntidadeEleitores() : Inherited(&theInfo) {}
    EntidadeEleitores(const EntidadeEleitores & that) : Inherited(that) {}

    EntidadeEleitores& operator=(const EntidadeEleitores& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_seguranca,
      e_eleitores
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

    class identificacao
    {
    public:
        typedef ModuloTiposEleitorais::IdentificacaoSecaoEleitoral value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class identificacao

    identificacao::const_reference get_identificacao () const { return *static_cast<identificacao::const_pointer>(fields[1]); }
    identificacao::reference ref_identificacao () { return *static_cast<identificacao::pointer>(fields[1]); }
    identificacao::reference set_identificacao () { return *static_cast<identificacao::pointer>(fields[1]); }

    class seguranca
    {
    public:
        typedef ModuloTiposEleitorais::Seguranca value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class seguranca

    seguranca::const_reference get_seguranca () const
    {
      assert(hasOptionalField(e_seguranca));
      return *static_cast<seguranca::const_pointer>(fields[2]);
    }
    seguranca::reference ref_seguranca ()
    {
      assert(hasOptionalField(e_seguranca));
      return *static_cast<seguranca::pointer>(fields[2]);
    }
    seguranca::reference set_seguranca ()
    {
      includeOptionalField( e_seguranca, 2);
      return *static_cast<seguranca::pointer>(fields[2]);
    }
    void omit_seguranca () { removeOptionalField( e_seguranca); }
    bool seguranca_isPresent () const { return hasOptionalField( e_seguranca); }

    class eleitores
    {
    public:
        //
        // ModuloEleitores::EntidadeEleitores::eleitores::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ModuloEleitores::EleitorSequencia>
        {
            typedef ASN1::SEQUENCE_OF<ModuloEleitores::EleitorSequencia> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ModuloEleitores::EleitorSequencia& val) : Inherited(n, val) {}
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
    }; // end class eleitores

    eleitores::const_reference get_eleitores () const
    {
      assert(hasOptionalField(e_eleitores));
      return *static_cast<eleitores::const_pointer>(fields[3]);
    }
    eleitores::reference ref_eleitores ()
    {
      assert(hasOptionalField(e_eleitores));
      return *static_cast<eleitores::pointer>(fields[3]);
    }
    eleitores::reference set_eleitores ()
    {
      includeOptionalField( e_eleitores, 3);
      return *static_cast<eleitores::pointer>(fields[3]);
    }
    void omit_eleitores () { removeOptionalField( e_eleitores); }
    bool eleitores_isPresent () const { return hasOptionalField( e_eleitores); }
    void swap(EntidadeEleitores& that) { Inherited::swap(that); }
    EntidadeEleitores * clone() const
    { return static_cast<EntidadeEleitores*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[4];
    static int fieldIds[4];
    static unsigned fieldTags[4];
    static const char* fieldNames[4];
}; // end class EntidadeEleitores

} // namespace ModuloEleitores

#endif // __MODULOELEITORES_H
