#ifndef __MODULOELEICAO_H
#define __MODULOELEICAO_H

#include "asn1.h"

#include "ModuloComplementosMunicipios.h"
#include "ModuloTiposEleitorais.h"

namespace ModuloEleicao {

//
// ModuloEleicao::CalendarioEleicao
//

class CalendarioEleicao : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    CalendarioEleicao(const void* info) : Inherited(info) {}
public:
    CalendarioEleicao() : Inherited(&theInfo) {}
    CalendarioEleicao(const CalendarioEleicao & that) : Inherited(that) {}

    CalendarioEleicao& operator=(const CalendarioEleicao& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_dataInicioRegistroCandidatura,
      e_dataFimRegistroCandidatura,
      e_dataInicioConvencaoPartidaria,
      e_dataFimConvencaoPartidaria
    };

    class dataInicioPropaganda
    {
    public:
        typedef ModuloComplementosMunicipios::InicioHV value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class dataInicioPropaganda

    dataInicioPropaganda::const_reference get_dataInicioPropaganda () const { return *static_cast<dataInicioPropaganda::const_pointer>(fields[0]); }
    dataInicioPropaganda::reference ref_dataInicioPropaganda () { return *static_cast<dataInicioPropaganda::pointer>(fields[0]); }
    dataInicioPropaganda::reference set_dataInicioPropaganda () { return *static_cast<dataInicioPropaganda::pointer>(fields[0]); }
    dataInicioPropaganda::reference set_dataInicioPropaganda (const ASN1_STD string& v)
    { return *static_cast<dataInicioPropaganda::pointer>(fields[0]) = v; }

    class dataFimPropaganda
    {
    public:
        typedef ModuloComplementosMunicipios::InicioHV value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class dataFimPropaganda

    dataFimPropaganda::const_reference get_dataFimPropaganda () const { return *static_cast<dataFimPropaganda::const_pointer>(fields[1]); }
    dataFimPropaganda::reference ref_dataFimPropaganda () { return *static_cast<dataFimPropaganda::pointer>(fields[1]); }
    dataFimPropaganda::reference set_dataFimPropaganda () { return *static_cast<dataFimPropaganda::pointer>(fields[1]); }
    dataFimPropaganda::reference set_dataFimPropaganda (const ASN1_STD string& v)
    { return *static_cast<dataFimPropaganda::pointer>(fields[1]) = v; }

    class dataInicioRegistroCandidatura
    {
    public:
        typedef ModuloComplementosMunicipios::InicioHV value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class dataInicioRegistroCandidatura

    dataInicioRegistroCandidatura::const_reference get_dataInicioRegistroCandidatura () const
    {
      assert(hasOptionalField(e_dataInicioRegistroCandidatura));
      return *static_cast<dataInicioRegistroCandidatura::const_pointer>(fields[2]);
    }
    dataInicioRegistroCandidatura::reference ref_dataInicioRegistroCandidatura ()
    {
      assert(hasOptionalField(e_dataInicioRegistroCandidatura));
      return *static_cast<dataInicioRegistroCandidatura::pointer>(fields[2]);
    }
    dataInicioRegistroCandidatura::reference set_dataInicioRegistroCandidatura ()
    {
      includeOptionalField( e_dataInicioRegistroCandidatura, 2);
      return *static_cast<dataInicioRegistroCandidatura::pointer>(fields[2]);
    }
    dataInicioRegistroCandidatura::reference set_dataInicioRegistroCandidatura (const ASN1_STD string& v)
    {
      includeOptionalField( e_dataInicioRegistroCandidatura, 2);
      return *static_cast<dataInicioRegistroCandidatura::pointer>(fields[2]) = v;
    }
    void omit_dataInicioRegistroCandidatura () { removeOptionalField( e_dataInicioRegistroCandidatura); }
    bool dataInicioRegistroCandidatura_isPresent () const { return hasOptionalField( e_dataInicioRegistroCandidatura); }

    class dataFimRegistroCandidatura
    {
    public:
        typedef ModuloComplementosMunicipios::InicioHV value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class dataFimRegistroCandidatura

    dataFimRegistroCandidatura::const_reference get_dataFimRegistroCandidatura () const
    {
      assert(hasOptionalField(e_dataFimRegistroCandidatura));
      return *static_cast<dataFimRegistroCandidatura::const_pointer>(fields[3]);
    }
    dataFimRegistroCandidatura::reference ref_dataFimRegistroCandidatura ()
    {
      assert(hasOptionalField(e_dataFimRegistroCandidatura));
      return *static_cast<dataFimRegistroCandidatura::pointer>(fields[3]);
    }
    dataFimRegistroCandidatura::reference set_dataFimRegistroCandidatura ()
    {
      includeOptionalField( e_dataFimRegistroCandidatura, 3);
      return *static_cast<dataFimRegistroCandidatura::pointer>(fields[3]);
    }
    dataFimRegistroCandidatura::reference set_dataFimRegistroCandidatura (const ASN1_STD string& v)
    {
      includeOptionalField( e_dataFimRegistroCandidatura, 3);
      return *static_cast<dataFimRegistroCandidatura::pointer>(fields[3]) = v;
    }
    void omit_dataFimRegistroCandidatura () { removeOptionalField( e_dataFimRegistroCandidatura); }
    bool dataFimRegistroCandidatura_isPresent () const { return hasOptionalField( e_dataFimRegistroCandidatura); }

    class dataInicioConvencaoPartidaria
    {
    public:
        typedef ModuloComplementosMunicipios::InicioHV value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class dataInicioConvencaoPartidaria

    dataInicioConvencaoPartidaria::const_reference get_dataInicioConvencaoPartidaria () const
    {
      assert(hasOptionalField(e_dataInicioConvencaoPartidaria));
      return *static_cast<dataInicioConvencaoPartidaria::const_pointer>(fields[4]);
    }
    dataInicioConvencaoPartidaria::reference ref_dataInicioConvencaoPartidaria ()
    {
      assert(hasOptionalField(e_dataInicioConvencaoPartidaria));
      return *static_cast<dataInicioConvencaoPartidaria::pointer>(fields[4]);
    }
    dataInicioConvencaoPartidaria::reference set_dataInicioConvencaoPartidaria ()
    {
      includeOptionalField( e_dataInicioConvencaoPartidaria, 4);
      return *static_cast<dataInicioConvencaoPartidaria::pointer>(fields[4]);
    }
    dataInicioConvencaoPartidaria::reference set_dataInicioConvencaoPartidaria (const ASN1_STD string& v)
    {
      includeOptionalField( e_dataInicioConvencaoPartidaria, 4);
      return *static_cast<dataInicioConvencaoPartidaria::pointer>(fields[4]) = v;
    }
    void omit_dataInicioConvencaoPartidaria () { removeOptionalField( e_dataInicioConvencaoPartidaria); }
    bool dataInicioConvencaoPartidaria_isPresent () const { return hasOptionalField( e_dataInicioConvencaoPartidaria); }

    class dataFimConvencaoPartidaria
    {
    public:
        typedef ModuloComplementosMunicipios::InicioHV value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class dataFimConvencaoPartidaria

    dataFimConvencaoPartidaria::const_reference get_dataFimConvencaoPartidaria () const
    {
      assert(hasOptionalField(e_dataFimConvencaoPartidaria));
      return *static_cast<dataFimConvencaoPartidaria::const_pointer>(fields[5]);
    }
    dataFimConvencaoPartidaria::reference ref_dataFimConvencaoPartidaria ()
    {
      assert(hasOptionalField(e_dataFimConvencaoPartidaria));
      return *static_cast<dataFimConvencaoPartidaria::pointer>(fields[5]);
    }
    dataFimConvencaoPartidaria::reference set_dataFimConvencaoPartidaria ()
    {
      includeOptionalField( e_dataFimConvencaoPartidaria, 5);
      return *static_cast<dataFimConvencaoPartidaria::pointer>(fields[5]);
    }
    dataFimConvencaoPartidaria::reference set_dataFimConvencaoPartidaria (const ASN1_STD string& v)
    {
      includeOptionalField( e_dataFimConvencaoPartidaria, 5);
      return *static_cast<dataFimConvencaoPartidaria::pointer>(fields[5]) = v;
    }
    void omit_dataFimConvencaoPartidaria () { removeOptionalField( e_dataFimConvencaoPartidaria); }
    bool dataFimConvencaoPartidaria_isPresent () const { return hasOptionalField( e_dataFimConvencaoPartidaria); }

    class dataInicioPrestacaoConta
    {
    public:
        typedef ModuloComplementosMunicipios::InicioHV value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class dataInicioPrestacaoConta

    dataInicioPrestacaoConta::const_reference get_dataInicioPrestacaoConta () const { return *static_cast<dataInicioPrestacaoConta::const_pointer>(fields[6]); }
    dataInicioPrestacaoConta::reference ref_dataInicioPrestacaoConta () { return *static_cast<dataInicioPrestacaoConta::pointer>(fields[6]); }
    dataInicioPrestacaoConta::reference set_dataInicioPrestacaoConta () { return *static_cast<dataInicioPrestacaoConta::pointer>(fields[6]); }
    dataInicioPrestacaoConta::reference set_dataInicioPrestacaoConta (const ASN1_STD string& v)
    { return *static_cast<dataInicioPrestacaoConta::pointer>(fields[6]) = v; }

    class dataFimPrestacaoConta
    {
    public:
        typedef ModuloComplementosMunicipios::InicioHV value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class dataFimPrestacaoConta

    dataFimPrestacaoConta::const_reference get_dataFimPrestacaoConta () const { return *static_cast<dataFimPrestacaoConta::const_pointer>(fields[7]); }
    dataFimPrestacaoConta::reference ref_dataFimPrestacaoConta () { return *static_cast<dataFimPrestacaoConta::pointer>(fields[7]); }
    dataFimPrestacaoConta::reference set_dataFimPrestacaoConta () { return *static_cast<dataFimPrestacaoConta::pointer>(fields[7]); }
    dataFimPrestacaoConta::reference set_dataFimPrestacaoConta (const ASN1_STD string& v)
    { return *static_cast<dataFimPrestacaoConta::pointer>(fields[7]) = v; }
    void swap(CalendarioEleicao& that) { Inherited::swap(that); }
    CalendarioEleicao * clone() const
    { return static_cast<CalendarioEleicao*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[8];
    static int fieldIds[8];
    static unsigned fieldTags[8];
    static const char* fieldNames[8];
}; // end class CalendarioEleicao

//
// ModuloEleicao::RespostaPergunta
//

class RespostaPergunta : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    RespostaPergunta(const void* info) : Inherited(info) {}
public:
    RespostaPergunta() : Inherited(&theInfo) {}
    RespostaPergunta(const RespostaPergunta & that) : Inherited(that) {}

    RespostaPergunta& operator=(const RespostaPergunta& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_textoFonetico
    };

    class numero
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 99999> value_type;
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

    class resposta
    {
    public:
        //
        // ModuloEleicao::RespostaPergunta::resposta::value_type
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
    }; // end class resposta

    resposta::const_reference get_resposta () const { return *static_cast<resposta::const_pointer>(fields[1]); }
    resposta::reference ref_resposta () { return *static_cast<resposta::pointer>(fields[1]); }
    resposta::reference set_resposta () { return *static_cast<resposta::pointer>(fields[1]); }
    resposta::reference set_resposta (const ASN1_STD string& v)
    { return *static_cast<resposta::pointer>(fields[1]) = v; }

    class textoFonetico
    {
    public:
        typedef ASN1::GeneralString value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class textoFonetico

    textoFonetico::const_reference get_textoFonetico () const
    {
      assert(hasOptionalField(e_textoFonetico));
      return *static_cast<textoFonetico::const_pointer>(fields[2]);
    }
    textoFonetico::reference ref_textoFonetico ()
    {
      assert(hasOptionalField(e_textoFonetico));
      return *static_cast<textoFonetico::pointer>(fields[2]);
    }
    textoFonetico::reference set_textoFonetico ()
    {
      includeOptionalField( e_textoFonetico, 2);
      return *static_cast<textoFonetico::pointer>(fields[2]);
    }
    textoFonetico::reference set_textoFonetico (const ASN1_STD string& v)
    {
      includeOptionalField( e_textoFonetico, 2);
      return *static_cast<textoFonetico::pointer>(fields[2]) = v;
    }
    void omit_textoFonetico () { removeOptionalField( e_textoFonetico); }
    bool textoFonetico_isPresent () const { return hasOptionalField( e_textoFonetico); }
    void swap(RespostaPergunta& that) { Inherited::swap(that); }
    RespostaPergunta * clone() const
    { return static_cast<RespostaPergunta*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[3];
    static int fieldIds[3];
    static unsigned fieldTags[3];
    static const char* fieldNames[3];
}; // end class RespostaPergunta

//
// ModuloEleicao::NomesCargo
//

class NomesCargo : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    NomesCargo(const void* info) : Inherited(info) {}
public:
    NomesCargo() : Inherited(&theInfo) {}
    NomesCargo(const NomesCargo & that) : Inherited(that) {}

    NomesCargo& operator=(const NomesCargo& that)
    { Inherited::operator=(that); return *this; }


    class nomeNeutro
    {
    public:
        //
        // ModuloEleicao::NomesCargo::nomeNeutro::value_type
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
    }; // end class nomeNeutro

    nomeNeutro::const_reference get_nomeNeutro () const { return *static_cast<nomeNeutro::const_pointer>(fields[0]); }
    nomeNeutro::reference ref_nomeNeutro () { return *static_cast<nomeNeutro::pointer>(fields[0]); }
    nomeNeutro::reference set_nomeNeutro () { return *static_cast<nomeNeutro::pointer>(fields[0]); }
    nomeNeutro::reference set_nomeNeutro (const ASN1_STD string& v)
    { return *static_cast<nomeNeutro::pointer>(fields[0]) = v; }

    class nomeMasculino
    {
    public:
        //
        // ModuloEleicao::NomesCargo::nomeMasculino::value_type
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
    }; // end class nomeMasculino

    nomeMasculino::const_reference get_nomeMasculino () const { return *static_cast<nomeMasculino::const_pointer>(fields[1]); }
    nomeMasculino::reference ref_nomeMasculino () { return *static_cast<nomeMasculino::pointer>(fields[1]); }
    nomeMasculino::reference set_nomeMasculino () { return *static_cast<nomeMasculino::pointer>(fields[1]); }
    nomeMasculino::reference set_nomeMasculino (const ASN1_STD string& v)
    { return *static_cast<nomeMasculino::pointer>(fields[1]) = v; }

    class nomeFeminino
    {
    public:
        //
        // ModuloEleicao::NomesCargo::nomeFeminino::value_type
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
    }; // end class nomeFeminino

    nomeFeminino::const_reference get_nomeFeminino () const { return *static_cast<nomeFeminino::const_pointer>(fields[2]); }
    nomeFeminino::reference ref_nomeFeminino () { return *static_cast<nomeFeminino::pointer>(fields[2]); }
    nomeFeminino::reference set_nomeFeminino () { return *static_cast<nomeFeminino::pointer>(fields[2]); }
    nomeFeminino::reference set_nomeFeminino (const ASN1_STD string& v)
    { return *static_cast<nomeFeminino::pointer>(fields[2]) = v; }

    class nomeAbreviado
    {
    public:
        //
        // ModuloEleicao::NomesCargo::nomeAbreviado::value_type
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
    }; // end class nomeAbreviado

    nomeAbreviado::const_reference get_nomeAbreviado () const { return *static_cast<nomeAbreviado::const_pointer>(fields[3]); }
    nomeAbreviado::reference ref_nomeAbreviado () { return *static_cast<nomeAbreviado::pointer>(fields[3]); }
    nomeAbreviado::reference set_nomeAbreviado () { return *static_cast<nomeAbreviado::pointer>(fields[3]); }
    nomeAbreviado::reference set_nomeAbreviado (const ASN1_STD string& v)
    { return *static_cast<nomeAbreviado::pointer>(fields[3]) = v; }
    void swap(NomesCargo& that) { Inherited::swap(that); }
    NomesCargo * clone() const
    { return static_cast<NomesCargo*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[4];
    static int fieldIds[4];
    static const char* fieldNames[4];
}; // end class NomesCargo

//
// ModuloEleicao::DetalhePergunta
//

class DetalhePergunta : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    DetalhePergunta(const void* info) : Inherited(info) {}
public:
    DetalhePergunta() : Inherited(&theInfo) {}
    DetalhePergunta(const DetalhePergunta & that) : Inherited(that) {}

    DetalhePergunta& operator=(const DetalhePergunta& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_textoFonetico
    };

    class nome
    {
    public:
        //
        // ModuloEleicao::DetalhePergunta::nome::value_type
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

    nome::const_reference get_nome () const { return *static_cast<nome::const_pointer>(fields[0]); }
    nome::reference ref_nome () { return *static_cast<nome::pointer>(fields[0]); }
    nome::reference set_nome () { return *static_cast<nome::pointer>(fields[0]); }
    nome::reference set_nome (const ASN1_STD string& v)
    { return *static_cast<nome::pointer>(fields[0]) = v; }

    class pergunta
    {
    public:
        //
        // ModuloEleicao::DetalhePergunta::pergunta::value_type
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
    }; // end class pergunta

    pergunta::const_reference get_pergunta () const { return *static_cast<pergunta::const_pointer>(fields[1]); }
    pergunta::reference ref_pergunta () { return *static_cast<pergunta::pointer>(fields[1]); }
    pergunta::reference set_pergunta () { return *static_cast<pergunta::pointer>(fields[1]); }
    pergunta::reference set_pergunta (const ASN1_STD string& v)
    { return *static_cast<pergunta::pointer>(fields[1]) = v; }

    class textoFonetico
    {
    public:
        typedef ASN1::GeneralString value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class textoFonetico

    textoFonetico::const_reference get_textoFonetico () const
    {
      assert(hasOptionalField(e_textoFonetico));
      return *static_cast<textoFonetico::const_pointer>(fields[2]);
    }
    textoFonetico::reference ref_textoFonetico ()
    {
      assert(hasOptionalField(e_textoFonetico));
      return *static_cast<textoFonetico::pointer>(fields[2]);
    }
    textoFonetico::reference set_textoFonetico ()
    {
      includeOptionalField( e_textoFonetico, 2);
      return *static_cast<textoFonetico::pointer>(fields[2]);
    }
    textoFonetico::reference set_textoFonetico (const ASN1_STD string& v)
    {
      includeOptionalField( e_textoFonetico, 2);
      return *static_cast<textoFonetico::pointer>(fields[2]) = v;
    }
    void omit_textoFonetico () { removeOptionalField( e_textoFonetico); }
    bool textoFonetico_isPresent () const { return hasOptionalField( e_textoFonetico); }

    class respostas
    {
    public:
        //
        // ModuloEleicao::DetalhePergunta::respostas::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ModuloEleicao::RespostaPergunta>
        {
            typedef ASN1::SEQUENCE_OF<ModuloEleicao::RespostaPergunta> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ModuloEleicao::RespostaPergunta& val) : Inherited(n, val) {}
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
    }; // end class respostas

    respostas::const_reference get_respostas () const { return *static_cast<respostas::const_pointer>(fields[3]); }
    respostas::reference ref_respostas () { return *static_cast<respostas::pointer>(fields[3]); }
    respostas::reference set_respostas () { return *static_cast<respostas::pointer>(fields[3]); }
    void swap(DetalhePergunta& that) { Inherited::swap(that); }
    DetalhePergunta * clone() const
    { return static_cast<DetalhePergunta*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[4];
    static int fieldIds[4];
    static unsigned fieldTags[4];
    static const char* fieldNames[4];
}; // end class DetalhePergunta

//
// ModuloEleicao::Suplencias
//

class Suplencias : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    Suplencias(const void* info) : Inherited(info) {}
public:
    Suplencias() : Inherited(&theInfo) {}
    Suplencias(const Suplencias & that) : Inherited(that) {}

    Suplencias& operator=(const Suplencias& that)
    { Inherited::operator=(that); return *this; }


    class codigo
    {
    public:
        typedef ModuloTiposEleitorais::CodigoCargoConsulta value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class codigo

    codigo::const_reference get_codigo () const { return *static_cast<codigo::const_pointer>(fields[0]); }
    codigo::reference ref_codigo () { return *static_cast<codigo::pointer>(fields[0]); }
    codigo::reference set_codigo () { return *static_cast<codigo::pointer>(fields[0]); }

    class ordem
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 9> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class ordem

    ordem::const_reference get_ordem () const { return *static_cast<ordem::const_pointer>(fields[1]); }
    ordem::reference ref_ordem () { return *static_cast<ordem::pointer>(fields[1]); }
    ordem::reference set_ordem () { return *static_cast<ordem::pointer>(fields[1]); }
    ordem::reference set_ordem (ASN1::INTEGER::int_type v)
    { return *static_cast<ordem::pointer>(fields[1]) = v; }

    class nomes
    {
    public:
        typedef ModuloEleicao::NomesCargo value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class nomes

    nomes::const_reference get_nomes () const { return *static_cast<nomes::const_pointer>(fields[2]); }
    nomes::reference ref_nomes () { return *static_cast<nomes::pointer>(fields[2]); }
    nomes::reference set_nomes () { return *static_cast<nomes::pointer>(fields[2]); }

    class temFoto
    {
    public:
        typedef ASN1::BOOLEAN value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class temFoto

    temFoto::const_reference get_temFoto () const { return *static_cast<temFoto::const_pointer>(fields[3]); }
    temFoto::reference ref_temFoto () { return *static_cast<temFoto::pointer>(fields[3]); }
    temFoto::reference set_temFoto () { return *static_cast<temFoto::pointer>(fields[3]); }
    temFoto::reference set_temFoto (bool v)
    { return *static_cast<temFoto::pointer>(fields[3]) = v; }
    void swap(Suplencias& that) { Inherited::swap(that); }
    Suplencias * clone() const
    { return static_cast<Suplencias*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[4];
    static int fieldIds[4];
    static const char* fieldNames[4];
}; // end class Suplencias

//
// ModuloEleicao::DetalheCargo
//

class DetalheCargo : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    DetalheCargo(const void* info) : Inherited(info) {}
public:
    DetalheCargo() : Inherited(&theInfo) {}
    DetalheCargo(const DetalheCargo & that) : Inherited(that) {}

    DetalheCargo& operator=(const DetalheCargo& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_suplencias
    };

    class qtdeSuplentes
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 2> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class qtdeSuplentes

    qtdeSuplentes::const_reference get_qtdeSuplentes () const { return *static_cast<qtdeSuplentes::const_pointer>(fields[0]); }
    qtdeSuplentes::reference ref_qtdeSuplentes () { return *static_cast<qtdeSuplentes::pointer>(fields[0]); }
    qtdeSuplentes::reference set_qtdeSuplentes () { return *static_cast<qtdeSuplentes::pointer>(fields[0]); }
    qtdeSuplentes::reference set_qtdeSuplentes (ASN1::INTEGER::int_type v)
    { return *static_cast<qtdeSuplentes::pointer>(fields[0]) = v; }

    class nomes
    {
    public:
        typedef ModuloEleicao::NomesCargo value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class nomes

    nomes::const_reference get_nomes () const { return *static_cast<nomes::const_pointer>(fields[1]); }
    nomes::reference ref_nomes () { return *static_cast<nomes::pointer>(fields[1]); }
    nomes::reference set_nomes () { return *static_cast<nomes::pointer>(fields[1]); }

    class temFoto
    {
    public:
        typedef ASN1::BOOLEAN value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class temFoto

    temFoto::const_reference get_temFoto () const { return *static_cast<temFoto::const_pointer>(fields[2]); }
    temFoto::reference ref_temFoto () { return *static_cast<temFoto::pointer>(fields[2]); }
    temFoto::reference set_temFoto () { return *static_cast<temFoto::pointer>(fields[2]); }
    temFoto::reference set_temFoto (bool v)
    { return *static_cast<temFoto::pointer>(fields[2]) = v; }

    class suplencias
    {
    public:
        //
        // ModuloEleicao::DetalheCargo::suplencias::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ModuloEleicao::Suplencias>
        {
            typedef ASN1::SEQUENCE_OF<ModuloEleicao::Suplencias> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ModuloEleicao::Suplencias& val) : Inherited(n, val) {}
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
    }; // end class suplencias

    suplencias::const_reference get_suplencias () const
    {
      assert(hasOptionalField(e_suplencias));
      return *static_cast<suplencias::const_pointer>(fields[3]);
    }
    suplencias::reference ref_suplencias ()
    {
      assert(hasOptionalField(e_suplencias));
      return *static_cast<suplencias::pointer>(fields[3]);
    }
    suplencias::reference set_suplencias ()
    {
      includeOptionalField( e_suplencias, 3);
      return *static_cast<suplencias::pointer>(fields[3]);
    }
    void omit_suplencias () { removeOptionalField( e_suplencias); }
    bool suplencias_isPresent () const { return hasOptionalField( e_suplencias); }
    void swap(DetalheCargo& that) { Inherited::swap(that); }
    DetalheCargo * clone() const
    { return static_cast<DetalheCargo*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[4];
    static int fieldIds[4];
    static const char* fieldNames[4];
}; // end class DetalheCargo

//
// ModuloEleicao::CargoPergunta
//

class CargoPergunta : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    CargoPergunta(const void* info) : Inherited(info) {}
public:
    CargoPergunta() : Inherited(&theInfo) {}
    CargoPergunta(const CargoPergunta & that) : Inherited(that) {}

    CargoPergunta& operator=(const CargoPergunta& that)
    { Inherited::operator=(that); return *this; }


    class codigo
    {
    public:
        typedef ModuloTiposEleitorais::CodigoCargoConsulta value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class codigo

    codigo::const_reference get_codigo () const { return *static_cast<codigo::const_pointer>(fields[0]); }
    codigo::reference ref_codigo () { return *static_cast<codigo::pointer>(fields[0]); }
    codigo::reference set_codigo () { return *static_cast<codigo::pointer>(fields[0]); }

    class tipo
    {
    public:
        typedef ModuloTiposEleitorais::Tipo value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class tipo

    tipo::const_reference get_tipo () const { return *static_cast<tipo::const_pointer>(fields[1]); }
    tipo::reference ref_tipo () { return *static_cast<tipo::pointer>(fields[1]); }
    tipo::reference set_tipo () { return *static_cast<tipo::pointer>(fields[1]); }
    tipo::reference set_tipo (ModuloTiposEleitorais::Tipo::NamedNumber v)
    { return *static_cast<tipo::pointer>(fields[1]) = v; }

    class numeroDigitos
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 5> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class numeroDigitos

    numeroDigitos::const_reference get_numeroDigitos () const { return *static_cast<numeroDigitos::const_pointer>(fields[2]); }
    numeroDigitos::reference ref_numeroDigitos () { return *static_cast<numeroDigitos::pointer>(fields[2]); }
    numeroDigitos::reference set_numeroDigitos () { return *static_cast<numeroDigitos::pointer>(fields[2]); }
    numeroDigitos::reference set_numeroDigitos (ASN1::INTEGER::int_type v)
    { return *static_cast<numeroDigitos::pointer>(fields[2]) = v; }

    class qtdeEscolhas
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 50> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class qtdeEscolhas

    qtdeEscolhas::const_reference get_qtdeEscolhas () const { return *static_cast<qtdeEscolhas::const_pointer>(fields[3]); }
    qtdeEscolhas::reference ref_qtdeEscolhas () { return *static_cast<qtdeEscolhas::pointer>(fields[3]); }
    qtdeEscolhas::reference set_qtdeEscolhas () { return *static_cast<qtdeEscolhas::pointer>(fields[3]); }
    qtdeEscolhas::reference set_qtdeEscolhas (ASN1::INTEGER::int_type v)
    { return *static_cast<qtdeEscolhas::pointer>(fields[3]) = v; }

    class podeRepetir
    {
    public:
        typedef ASN1::BOOLEAN value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class podeRepetir

    podeRepetir::const_reference get_podeRepetir () const { return *static_cast<podeRepetir::const_pointer>(fields[4]); }
    podeRepetir::reference ref_podeRepetir () { return *static_cast<podeRepetir::pointer>(fields[4]); }
    podeRepetir::reference set_podeRepetir () { return *static_cast<podeRepetir::pointer>(fields[4]); }
    podeRepetir::reference set_podeRepetir (bool v)
    { return *static_cast<podeRepetir::pointer>(fields[4]) = v; }

    class ordemAquisicao
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 99> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class ordemAquisicao

    ordemAquisicao::const_reference get_ordemAquisicao () const { return *static_cast<ordemAquisicao::const_pointer>(fields[5]); }
    ordemAquisicao::reference ref_ordemAquisicao () { return *static_cast<ordemAquisicao::pointer>(fields[5]); }
    ordemAquisicao::reference set_ordemAquisicao () { return *static_cast<ordemAquisicao::pointer>(fields[5]); }
    ordemAquisicao::reference set_ordemAquisicao (ASN1::INTEGER::int_type v)
    { return *static_cast<ordemAquisicao::pointer>(fields[5]) = v; }

    class ordemImpressao
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 99> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class ordemImpressao

    ordemImpressao::const_reference get_ordemImpressao () const { return *static_cast<ordemImpressao::const_pointer>(fields[6]); }
    ordemImpressao::reference ref_ordemImpressao () { return *static_cast<ordemImpressao::pointer>(fields[6]); }
    ordemImpressao::reference set_ordemImpressao () { return *static_cast<ordemImpressao::pointer>(fields[6]); }
    ordemImpressao::reference set_ordemImpressao (ASN1::INTEGER::int_type v)
    { return *static_cast<ordemImpressao::pointer>(fields[6]) = v; }

    class ordemApuracao
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 99> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class ordemApuracao

    ordemApuracao::const_reference get_ordemApuracao () const { return *static_cast<ordemApuracao::const_pointer>(fields[7]); }
    ordemApuracao::reference ref_ordemApuracao () { return *static_cast<ordemApuracao::pointer>(fields[7]); }
    ordemApuracao::reference set_ordemApuracao () { return *static_cast<ordemApuracao::pointer>(fields[7]); }
    ordemApuracao::reference set_ordemApuracao (ASN1::INTEGER::int_type v)
    { return *static_cast<ordemApuracao::pointer>(fields[7]) = v; }

    class paginaImpressaoVoto
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 5> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class paginaImpressaoVoto

    paginaImpressaoVoto::const_reference get_paginaImpressaoVoto () const { return *static_cast<paginaImpressaoVoto::const_pointer>(fields[8]); }
    paginaImpressaoVoto::reference ref_paginaImpressaoVoto () { return *static_cast<paginaImpressaoVoto::pointer>(fields[8]); }
    paginaImpressaoVoto::reference set_paginaImpressaoVoto () { return *static_cast<paginaImpressaoVoto::pointer>(fields[8]); }
    paginaImpressaoVoto::reference set_paginaImpressaoVoto (ASN1::INTEGER::int_type v)
    { return *static_cast<paginaImpressaoVoto::pointer>(fields[8]) = v; }

    class detalhe
    {
    public:
        //
        // ModuloEleicao::CargoPergunta::detalhe::value_type
        //

        class value_type : public ASN1::CHOICE
        {
            typedef ASN1::CHOICE Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type() : Inherited(&theInfo) {}
            enum Choice_Ids {
              unknownSelection_ = -2,
              unselected_ = -1,
              cargo0 = 0,
              pergunta1 = 1
            };

            class cargo
            {
            public:
                enum Id { eid = 0 };
                typedef ModuloEleicao::DetalheCargo value_type;
                typedef value_type&        reference;
                typedef const value_type&  const_reference;
                typedef value_type*        pointer;
                typedef const value_type*  const_pointer;
            }; // end class cargo

            cargo::const_reference get_cargo () const
            {
                assert(currentSelection() == cargo::eid);
                return *static_cast<cargo::const_pointer>(choice.get());
            }
            cargo::reference ref_cargo ()
            {
                assert(currentSelection() == cargo::eid);
                return *static_cast<cargo::pointer>(choice.get());
            }
            cargo::reference select_cargo ()
            {
                return *static_cast<cargo::pointer>(setSelection(cargo::eid, ASN1::AbstractData::create(&cargo::value_type::theInfo)));
            }
            bool cargo_isSelected () const { return currentSelection() == cargo::eid; }

            class pergunta
            {
            public:
                enum Id { eid = 1 };
                typedef ModuloEleicao::DetalhePergunta value_type;
                typedef value_type&        reference;
                typedef const value_type&  const_reference;
                typedef value_type*        pointer;
                typedef const value_type*  const_pointer;
            }; // end class pergunta

            pergunta::const_reference get_pergunta () const
            {
                assert(currentSelection() == pergunta::eid);
                return *static_cast<pergunta::const_pointer>(choice.get());
            }
            pergunta::reference ref_pergunta ()
            {
                assert(currentSelection() == pergunta::eid);
                return *static_cast<pergunta::pointer>(choice.get());
            }
            pergunta::reference select_pergunta ()
            {
                return *static_cast<pergunta::pointer>(setSelection(pergunta::eid, ASN1::AbstractData::create(&pergunta::value_type::theInfo)));
            }
            bool pergunta_isSelected () const { return currentSelection() == pergunta::eid; }

            value_type(const value_type & that) : Inherited(that) {}
            value_type & operator=(const value_type & that)
            { Inherited::operator=(that); return *this; }
            void swap(value_type & that) { Inherited::swap(that); }
            value_type * clone() const
            { return static_cast<value_type*> (Inherited::clone()); }
            static bool equal_type(const ASN1::AbstractData& type)
            { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
            static const InfoType theInfo;
        private:
            static const void* selectionInfos[2];
            static const char* selectionNames[2];
        }; // end class value_type

        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class detalhe

    detalhe::const_reference get_detalhe () const { return *static_cast<detalhe::const_pointer>(fields[9]); }
    detalhe::reference ref_detalhe () { return *static_cast<detalhe::pointer>(fields[9]); }
    detalhe::reference set_detalhe () { return *static_cast<detalhe::pointer>(fields[9]); }
    void swap(CargoPergunta& that) { Inherited::swap(that); }
    CargoPergunta * clone() const
    { return static_cast<CargoPergunta*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[10];
    static int fieldIds[10];
    static const char* fieldNames[10];
}; // end class CargoPergunta

//
// ModuloEleicao::Tipo   // SYNTHESIZED name (field ['tipo', 'tipoEleicaoPrincipal'])
//

class Tipo : public ASN1::ENUMERATED
{
    typedef ASN1::ENUMERATED Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    Tipo(const void* info) : Inherited(info) {}
public:
    Tipo() : Inherited(&theInfo) {}
    enum NamedNumber {
        unknownEnumeration_ = -1,
        ordinaria,
        extraordinaria,
        consultaPopular
    };

    bool is_ordinaria() const { return value == ordinaria; }
    void set_ordinaria() { value = ordinaria; }

    bool is_extraordinaria() const { return value == extraordinaria; }
    void set_extraordinaria() { value = extraordinaria; }

    bool is_consultaPopular() const { return value == consultaPopular; }
    void set_consultaPopular() { value = consultaPopular; }

    Tipo(const NamedNumber v) : Inherited(&theInfo) { setFromInt(v); }
    Tipo & operator=(const NamedNumber v) { setFromInt(v);  return *this; }
    operator NamedNumber() const { return NamedNumber(asInt()); }

    bool operator == (NamedNumber rhs) const { return value == rhs; }
    bool operator != (NamedNumber rhs) const { return value != rhs; }
    bool operator <  (NamedNumber rhs) const { return value <  rhs; }
    bool operator >  (NamedNumber rhs) const { return value >  rhs; }
    bool operator <= (NamedNumber rhs) const { return value <= rhs; }
    bool operator >= (NamedNumber rhs) const { return value >= rhs; }
    bool operator == (const Tipo &rhs) const { return value == rhs.value; }
    bool operator != (const Tipo &rhs) const { return value != rhs.value; }

    void swap (Tipo & that) { Inherited::swap(that); }
    Tipo * clone() const
    { return static_cast<Tipo*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const char * nameList[];
}; // end class Tipo

//
// ModuloEleicao::EntidadeEleicao
//

class EntidadeEleicao : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EntidadeEleicao(const void* info) : Inherited(info) {}
public:
    EntidadeEleicao() : Inherited(&theInfo) {}
    EntidadeEleicao(const EntidadeEleicao & that) : Inherited(that) {}

    EntidadeEleicao& operator=(const EntidadeEleicao& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_idEleicaoPleito2,
      e_calendario
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

    class idPleito
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 99999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class idPleito

    idPleito::const_reference get_idPleito () const { return *static_cast<idPleito::const_pointer>(fields[1]); }
    idPleito::reference ref_idPleito () { return *static_cast<idPleito::pointer>(fields[1]); }
    idPleito::reference set_idPleito () { return *static_cast<idPleito::pointer>(fields[1]); }
    idPleito::reference set_idPleito (ASN1::INTEGER::int_type v)
    { return *static_cast<idPleito::pointer>(fields[1]) = v; }

    class idEleicaoPleito2
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 99999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class idEleicaoPleito2

    idEleicaoPleito2::const_reference get_idEleicaoPleito2 () const
    {
      assert(hasOptionalField(e_idEleicaoPleito2));
      return *static_cast<idEleicaoPleito2::const_pointer>(fields[2]);
    }
    idEleicaoPleito2::reference ref_idEleicaoPleito2 ()
    {
      assert(hasOptionalField(e_idEleicaoPleito2));
      return *static_cast<idEleicaoPleito2::pointer>(fields[2]);
    }
    idEleicaoPleito2::reference set_idEleicaoPleito2 ()
    {
      includeOptionalField( e_idEleicaoPleito2, 2);
      return *static_cast<idEleicaoPleito2::pointer>(fields[2]);
    }
    idEleicaoPleito2::reference set_idEleicaoPleito2 (ASN1::INTEGER::int_type v)
    {
      includeOptionalField( e_idEleicaoPleito2, 2);
      return *static_cast<idEleicaoPleito2::pointer>(fields[2]) = v;
    }
    void omit_idEleicaoPleito2 () { removeOptionalField( e_idEleicaoPleito2); }
    bool idEleicaoPleito2_isPresent () const { return hasOptionalField( e_idEleicaoPleito2); }

    class nome
    {
    public:
        //
        // ModuloEleicao::EntidadeEleicao::nome::value_type
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

    nome::const_reference get_nome () const { return *static_cast<nome::const_pointer>(fields[3]); }
    nome::reference ref_nome () { return *static_cast<nome::pointer>(fields[3]); }
    nome::reference set_nome () { return *static_cast<nome::pointer>(fields[3]); }
    nome::reference set_nome (const ASN1_STD string& v)
    { return *static_cast<nome::pointer>(fields[3]) = v; }

    class tipo
    {
    public:
        typedef ModuloEleicao::Tipo value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class tipo

    tipo::const_reference get_tipo () const { return *static_cast<tipo::const_pointer>(fields[4]); }
    tipo::reference ref_tipo () { return *static_cast<tipo::pointer>(fields[4]); }
    tipo::reference set_tipo () { return *static_cast<tipo::pointer>(fields[4]); }
    tipo::reference set_tipo (ModuloEleicao::Tipo::NamedNumber v)
    { return *static_cast<tipo::pointer>(fields[4]) = v; }

    class anoReferencia
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 2012, 9999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class anoReferencia

    anoReferencia::const_reference get_anoReferencia () const { return *static_cast<anoReferencia::const_pointer>(fields[5]); }
    anoReferencia::reference ref_anoReferencia () { return *static_cast<anoReferencia::pointer>(fields[5]); }
    anoReferencia::reference set_anoReferencia () { return *static_cast<anoReferencia::pointer>(fields[5]); }
    anoReferencia::reference set_anoReferencia (ASN1::INTEGER::int_type v)
    { return *static_cast<anoReferencia::pointer>(fields[5]) = v; }

    class abrangencia
    {
    public:
        typedef ModuloTiposEleitorais::TipoAbrangencia value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class abrangencia

    abrangencia::const_reference get_abrangencia () const { return *static_cast<abrangencia::const_pointer>(fields[6]); }
    abrangencia::reference ref_abrangencia () { return *static_cast<abrangencia::pointer>(fields[6]); }
    abrangencia::reference set_abrangencia () { return *static_cast<abrangencia::pointer>(fields[6]); }
    abrangencia::reference set_abrangencia (ModuloTiposEleitorais::TipoAbrangencia::NamedNumber v)
    { return *static_cast<abrangencia::pointer>(fields[6]) = v; }

    class calendario
    {
    public:
        typedef ModuloEleicao::CalendarioEleicao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class calendario

    calendario::const_reference get_calendario () const
    {
      assert(hasOptionalField(e_calendario));
      return *static_cast<calendario::const_pointer>(fields[7]);
    }
    calendario::reference ref_calendario ()
    {
      assert(hasOptionalField(e_calendario));
      return *static_cast<calendario::pointer>(fields[7]);
    }
    calendario::reference set_calendario ()
    {
      includeOptionalField( e_calendario, 7);
      return *static_cast<calendario::pointer>(fields[7]);
    }
    void omit_calendario () { removeOptionalField( e_calendario); }
    bool calendario_isPresent () const { return hasOptionalField( e_calendario); }

    class cargos
    {
    public:
        //
        // ModuloEleicao::EntidadeEleicao::cargos::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ModuloEleicao::CargoPergunta>
        {
            typedef ASN1::SEQUENCE_OF<ModuloEleicao::CargoPergunta> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ModuloEleicao::CargoPergunta& val) : Inherited(n, val) {}
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
    }; // end class cargos

    cargos::const_reference get_cargos () const { return *static_cast<cargos::const_pointer>(fields[8]); }
    cargos::reference ref_cargos () { return *static_cast<cargos::pointer>(fields[8]); }
    cargos::reference set_cargos () { return *static_cast<cargos::pointer>(fields[8]); }

    class municipios
    {
    public:
        //
        // ModuloEleicao::EntidadeEleicao::municipios::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 99999>>
        {
            typedef ASN1::SEQUENCE_OF<ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 99999>> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 99999>& val) : Inherited(n, val) {}
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
    }; // end class municipios

    municipios::const_reference get_municipios () const { return *static_cast<municipios::const_pointer>(fields[9]); }
    municipios::reference ref_municipios () { return *static_cast<municipios::pointer>(fields[9]); }
    municipios::reference set_municipios () { return *static_cast<municipios::pointer>(fields[9]); }

    class candidaturaDivulgavel
    {
    public:
        typedef ASN1::BOOLEAN value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class candidaturaDivulgavel

    candidaturaDivulgavel::const_reference get_candidaturaDivulgavel () const { return *static_cast<candidaturaDivulgavel::const_pointer>(fields[10]); }
    candidaturaDivulgavel::reference ref_candidaturaDivulgavel () { return *static_cast<candidaturaDivulgavel::pointer>(fields[10]); }
    candidaturaDivulgavel::reference set_candidaturaDivulgavel () { return *static_cast<candidaturaDivulgavel::pointer>(fields[10]); }
    candidaturaDivulgavel::reference set_candidaturaDivulgavel (bool v)
    { return *static_cast<candidaturaDivulgavel::pointer>(fields[10]) = v; }
    void swap(EntidadeEleicao& that) { Inherited::swap(that); }
    EntidadeEleicao * clone() const
    { return static_cast<EntidadeEleicao*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[11];
    static int fieldIds[11];
    static unsigned fieldTags[11];
    static const char* fieldNames[11];
}; // end class EntidadeEleicao

} // namespace ModuloEleicao

#endif // __MODULOELEICAO_H
