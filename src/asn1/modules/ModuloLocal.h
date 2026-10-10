#ifndef __MODULOLOCAL_H
#define __MODULOLOCAL_H

#include "asn1.h"

#include "ModuloComplementosMunicipios.h"
#include "ModuloTiposCadastro.h"
#include "ModuloTiposEleitorais.h"

namespace ModuloLocal {

//
// ModuloLocal::IdentificacaoUrnaContingencia
//

class IdentificacaoUrnaContingencia : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    IdentificacaoUrnaContingencia(const void* info) : Inherited(info) {}
public:
    IdentificacaoUrnaContingencia() : Inherited(&theInfo) {}
    IdentificacaoUrnaContingencia(const IdentificacaoUrnaContingencia & that) : Inherited(that) {}

    IdentificacaoUrnaContingencia& operator=(const IdentificacaoUrnaContingencia& that)
    { Inherited::operator=(that); return *this; }


    class municipioZona
    {
    public:
        typedef ModuloTiposEleitorais::MunicipioZona value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class municipioZona

    municipioZona::const_reference get_municipioZona () const { return *static_cast<municipioZona::const_pointer>(fields[0]); }
    municipioZona::reference ref_municipioZona () { return *static_cast<municipioZona::pointer>(fields[0]); }
    municipioZona::reference set_municipioZona () { return *static_cast<municipioZona::pointer>(fields[0]); }
    void swap(IdentificacaoUrnaContingencia& that) { Inherited::swap(that); }
    IdentificacaoUrnaContingencia * clone() const
    { return static_cast<IdentificacaoUrnaContingencia*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[1];
    static int fieldIds[1];
    static const char* fieldNames[1];
}; // end class IdentificacaoUrnaContingencia

//
// ModuloLocal::IdentificacaoAgregada
//

class IdentificacaoAgregada : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    IdentificacaoAgregada(const void* info) : Inherited(info) {}
public:
    IdentificacaoAgregada() : Inherited(&theInfo) {}
    IdentificacaoAgregada(const IdentificacaoAgregada & that) : Inherited(that) {}

    IdentificacaoAgregada& operator=(const IdentificacaoAgregada& that)
    { Inherited::operator=(that); return *this; }


    class numero
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 9999> value_type;
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

    class tipoLocalOrigem
    {
    public:
        typedef ModuloTiposCadastro::TipoLocalVotacao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class tipoLocalOrigem

    tipoLocalOrigem::const_reference get_tipoLocalOrigem () const { return *static_cast<tipoLocalOrigem::const_pointer>(fields[1]); }
    tipoLocalOrigem::reference ref_tipoLocalOrigem () { return *static_cast<tipoLocalOrigem::pointer>(fields[1]); }
    tipoLocalOrigem::reference set_tipoLocalOrigem () { return *static_cast<tipoLocalOrigem::pointer>(fields[1]); }
    tipoLocalOrigem::reference set_tipoLocalOrigem (ModuloTiposCadastro::TipoLocalVotacao::NamedNumber v)
    { return *static_cast<tipoLocalOrigem::pointer>(fields[1]) = v; }
    void swap(IdentificacaoAgregada& that) { Inherited::swap(that); }
    IdentificacaoAgregada * clone() const
    { return static_cast<IdentificacaoAgregada*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[2];
    static int fieldIds[2];
    static const char* fieldNames[2];
}; // end class IdentificacaoAgregada

//
// ModuloLocal::SecaoEleitoral
//

class SecaoEleitoral : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    SecaoEleitoral(const void* info) : Inherited(info) {}
public:
    SecaoEleitoral() : Inherited(&theInfo) {}
    SecaoEleitoral(const SecaoEleitoral & that) : Inherited(that) {}

    SecaoEleitoral& operator=(const SecaoEleitoral& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_agregadas
    };

    class tipo
    {
    public:
        typedef ModuloTiposCadastro::TipoLocalVotacao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class tipo

    tipo::const_reference get_tipo () const { return *static_cast<tipo::const_pointer>(fields[0]); }
    tipo::reference ref_tipo () { return *static_cast<tipo::pointer>(fields[0]); }
    tipo::reference set_tipo () { return *static_cast<tipo::pointer>(fields[0]); }
    tipo::reference set_tipo (ModuloTiposCadastro::TipoLocalVotacao::NamedNumber v)
    { return *static_cast<tipo::pointer>(fields[0]) = v; }

    class secao
    {
    public:
        typedef ModuloTiposEleitorais::IdentificacaoSecaoEleitoral value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class secao

    secao::const_reference get_secao () const { return *static_cast<secao::const_pointer>(fields[1]); }
    secao::reference ref_secao () { return *static_cast<secao::pointer>(fields[1]); }
    secao::reference set_secao () { return *static_cast<secao::pointer>(fields[1]); }

    class agregadas
    {
    public:
        //
        // ModuloLocal::SecaoEleitoral::agregadas::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ModuloLocal::IdentificacaoAgregada>
        {
            typedef ASN1::SEQUENCE_OF<ModuloLocal::IdentificacaoAgregada> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ModuloLocal::IdentificacaoAgregada& val) : Inherited(n, val) {}
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
    }; // end class agregadas

    agregadas::const_reference get_agregadas () const
    {
      assert(hasOptionalField(e_agregadas));
      return *static_cast<agregadas::const_pointer>(fields[2]);
    }
    agregadas::reference ref_agregadas ()
    {
      assert(hasOptionalField(e_agregadas));
      return *static_cast<agregadas::pointer>(fields[2]);
    }
    agregadas::reference set_agregadas ()
    {
      includeOptionalField( e_agregadas, 2);
      return *static_cast<agregadas::pointer>(fields[2]);
    }
    void omit_agregadas () { removeOptionalField( e_agregadas); }
    bool agregadas_isPresent () const { return hasOptionalField( e_agregadas); }
    void swap(SecaoEleitoral& that) { Inherited::swap(that); }
    SecaoEleitoral * clone() const
    { return static_cast<SecaoEleitoral*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[3];
    static int fieldIds[3];
    static const char* fieldNames[3];
}; // end class SecaoEleitoral

//
// ModuloLocal::Local
//

class Local : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    Local(const void* info) : Inherited(info) {}
public:
    Local() : Inherited(&theInfo) {}
    Local(const Local & that) : Inherited(that) {}

    Local& operator=(const Local& that)
    { Inherited::operator=(that); return *this; }


    class idPE
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 99999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class idPE

    idPE::const_reference get_idPE () const { return *static_cast<idPE::const_pointer>(fields[0]); }
    idPE::reference ref_idPE () { return *static_cast<idPE::pointer>(fields[0]); }
    idPE::reference set_idPE () { return *static_cast<idPE::pointer>(fields[0]); }
    idPE::reference set_idPE (ASN1::INTEGER::int_type v)
    { return *static_cast<idPE::pointer>(fields[0]) = v; }

    class pais
    {
    public:
        //
        // ModuloLocal::Local::pais::value_type
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
    }; // end class pais

    pais::const_reference get_pais () const { return *static_cast<pais::const_pointer>(fields[1]); }
    pais::reference ref_pais () { return *static_cast<pais::pointer>(fields[1]); }
    pais::reference set_pais () { return *static_cast<pais::pointer>(fields[1]); }
    pais::reference set_pais (const ASN1_STD string& v)
    { return *static_cast<pais::pointer>(fields[1]) = v; }

    class uf
    {
    public:
        typedef ModuloTiposEleitorais::UF value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class uf

    uf::const_reference get_uf () const { return *static_cast<uf::const_pointer>(fields[2]); }
    uf::reference ref_uf () { return *static_cast<uf::pointer>(fields[2]); }
    uf::reference set_uf () { return *static_cast<uf::pointer>(fields[2]); }

    class municipio
    {
    public:
        typedef ModuloTiposCadastro::Municipio value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class municipio

    municipio::const_reference get_municipio () const { return *static_cast<municipio::const_pointer>(fields[3]); }
    municipio::reference ref_municipio () { return *static_cast<municipio::pointer>(fields[3]); }
    municipio::reference set_municipio () { return *static_cast<municipio::pointer>(fields[3]); }

    class complementoMunicipio
    {
    public:
        typedef ModuloComplementosMunicipios::ComplementoMunicipio value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class complementoMunicipio

    complementoMunicipio::const_reference get_complementoMunicipio () const { return *static_cast<complementoMunicipio::const_pointer>(fields[4]); }
    complementoMunicipio::reference ref_complementoMunicipio () { return *static_cast<complementoMunicipio::pointer>(fields[4]); }
    complementoMunicipio::reference set_complementoMunicipio () { return *static_cast<complementoMunicipio::pointer>(fields[4]); }

    class id
    {
    public:
        //
        // ModuloLocal::Local::id::value_type
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
              secao0 = 0,
              contingencia1 = 1
            };

            class secao
            {
            public:
                enum Id { eid = 0 };
                typedef ModuloLocal::SecaoEleitoral value_type;
                typedef value_type&        reference;
                typedef const value_type&  const_reference;
                typedef value_type*        pointer;
                typedef const value_type*  const_pointer;
            }; // end class secao

            secao::const_reference get_secao () const
            {
                assert(currentSelection() == secao::eid);
                return *static_cast<secao::const_pointer>(choice.get());
            }
            secao::reference ref_secao ()
            {
                assert(currentSelection() == secao::eid);
                return *static_cast<secao::pointer>(choice.get());
            }
            secao::reference select_secao ()
            {
                return *static_cast<secao::pointer>(setSelection(secao::eid, ASN1::AbstractData::create(&secao::value_type::theInfo)));
            }
            bool secao_isSelected () const { return currentSelection() == secao::eid; }

            class contingencia
            {
            public:
                enum Id { eid = 1 };
                typedef ModuloLocal::IdentificacaoUrnaContingencia value_type;
                typedef value_type&        reference;
                typedef const value_type&  const_reference;
                typedef value_type*        pointer;
                typedef const value_type*  const_pointer;
            }; // end class contingencia

            contingencia::const_reference get_contingencia () const
            {
                assert(currentSelection() == contingencia::eid);
                return *static_cast<contingencia::const_pointer>(choice.get());
            }
            contingencia::reference ref_contingencia ()
            {
                assert(currentSelection() == contingencia::eid);
                return *static_cast<contingencia::pointer>(choice.get());
            }
            contingencia::reference select_contingencia ()
            {
                return *static_cast<contingencia::pointer>(setSelection(contingencia::eid, ASN1::AbstractData::create(&contingencia::value_type::theInfo)));
            }
            bool contingencia_isSelected () const { return currentSelection() == contingencia::eid; }

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
            static unsigned selectionTags[2];
            static const char* selectionNames[2];
        }; // end class value_type

        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class id

    id::const_reference get_id () const { return *static_cast<id::const_pointer>(fields[5]); }
    id::reference ref_id () { return *static_cast<id::pointer>(fields[5]); }
    id::reference set_id () { return *static_cast<id::pointer>(fields[5]); }
    void swap(Local& that) { Inherited::swap(that); }
    Local * clone() const
    { return static_cast<Local*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[6];
    static int fieldIds[6];
    static const char* fieldNames[6];
}; // end class Local

} // namespace ModuloLocal

#endif // __MODULOLOCAL_H
