#ifndef __MODULODADOSDISPONIVEISCARGA_H
#define __MODULODADOSDISPONIVEISCARGA_H

#include "asn1.h"

#include "ModuloComplementosMunicipios.h"
#include "ModuloLocal.h"
#include "ModuloTiposCadastro.h"
#include "ModuloTiposEleitorais.h"

namespace ModuloDadosDisponiveisCarga {

//
// ModuloDadosDisponiveisCarga::MunicipioDisponivel
//

class MunicipioDisponivel : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    MunicipioDisponivel(const void* info) : Inherited(info) {}
public:
    MunicipioDisponivel() : Inherited(&theInfo) {}
    MunicipioDisponivel(const MunicipioDisponivel & that) : Inherited(that) {}

    MunicipioDisponivel& operator=(const MunicipioDisponivel& that)
    { Inherited::operator=(that); return *this; }


    class municipio
    {
    public:
        typedef ModuloTiposCadastro::Municipio value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class municipio

    municipio::const_reference get_municipio () const { return *static_cast<municipio::const_pointer>(fields[0]); }
    municipio::reference ref_municipio () { return *static_cast<municipio::pointer>(fields[0]); }
    municipio::reference set_municipio () { return *static_cast<municipio::pointer>(fields[0]); }

    class complementoMunicipio
    {
    public:
        typedef ModuloComplementosMunicipios::ComplementoMunicipio value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class complementoMunicipio

    complementoMunicipio::const_reference get_complementoMunicipio () const { return *static_cast<complementoMunicipio::const_pointer>(fields[1]); }
    complementoMunicipio::reference ref_complementoMunicipio () { return *static_cast<complementoMunicipio::pointer>(fields[1]); }
    complementoMunicipio::reference set_complementoMunicipio () { return *static_cast<complementoMunicipio::pointer>(fields[1]); }

    class secoes
    {
    public:
        //
        // ModuloDadosDisponiveisCarga::MunicipioDisponivel::secoes::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ModuloLocal::SecaoEleitoral>
        {
            typedef ASN1::SEQUENCE_OF<ModuloLocal::SecaoEleitoral> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ModuloLocal::SecaoEleitoral& val) : Inherited(n, val) {}
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
    }; // end class secoes

    secoes::const_reference get_secoes () const { return *static_cast<secoes::const_pointer>(fields[2]); }
    secoes::reference ref_secoes () { return *static_cast<secoes::pointer>(fields[2]); }
    secoes::reference set_secoes () { return *static_cast<secoes::pointer>(fields[2]); }
    void swap(MunicipioDisponivel& that) { Inherited::swap(that); }
    MunicipioDisponivel * clone() const
    { return static_cast<MunicipioDisponivel*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[3];
    static int fieldIds[3];
    static const char* fieldNames[3];
}; // end class MunicipioDisponivel

//
// ModuloDadosDisponiveisCarga::DadosDisponiveisCarga
//

class DadosDisponiveisCarga : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    DadosDisponiveisCarga(const void* info) : Inherited(info) {}
public:
    DadosDisponiveisCarga() : Inherited(&theInfo) {}
    DadosDisponiveisCarga(const DadosDisponiveisCarga & that) : Inherited(that) {}

    DadosDisponiveisCarga& operator=(const DadosDisponiveisCarga& that)
    { Inherited::operator=(that); return *this; }


    class fase
    {
    public:
        typedef ModuloTiposEleitorais::Fase value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class fase

    fase::const_reference get_fase () const { return *static_cast<fase::const_pointer>(fields[0]); }
    fase::reference ref_fase () { return *static_cast<fase::pointer>(fields[0]); }
    fase::reference set_fase () { return *static_cast<fase::pointer>(fields[0]); }
    fase::reference set_fase (ModuloTiposEleitorais::Fase::NamedNumber v)
    { return *static_cast<fase::pointer>(fields[0]) = v; }

    class idPE
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 99999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class idPE

    idPE::const_reference get_idPE () const { return *static_cast<idPE::const_pointer>(fields[1]); }
    idPE::reference ref_idPE () { return *static_cast<idPE::pointer>(fields[1]); }
    idPE::reference set_idPE () { return *static_cast<idPE::pointer>(fields[1]); }
    idPE::reference set_idPE (ASN1::INTEGER::int_type v)
    { return *static_cast<idPE::pointer>(fields[1]) = v; }

    class idPleito
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 99999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class idPleito

    idPleito::const_reference get_idPleito () const { return *static_cast<idPleito::const_pointer>(fields[2]); }
    idPleito::reference ref_idPleito () { return *static_cast<idPleito::pointer>(fields[2]); }
    idPleito::reference set_idPleito () { return *static_cast<idPleito::pointer>(fields[2]); }
    idPleito::reference set_idPleito (ASN1::INTEGER::int_type v)
    { return *static_cast<idPleito::pointer>(fields[2]) = v; }

    class serialFlashCarga
    {
    public:
        typedef ASN1::GeneralString value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class serialFlashCarga

    serialFlashCarga::const_reference get_serialFlashCarga () const { return *static_cast<serialFlashCarga::const_pointer>(fields[3]); }
    serialFlashCarga::reference ref_serialFlashCarga () { return *static_cast<serialFlashCarga::pointer>(fields[3]); }
    serialFlashCarga::reference set_serialFlashCarga () { return *static_cast<serialFlashCarga::pointer>(fields[3]); }
    serialFlashCarga::reference set_serialFlashCarga (const ASN1_STD string& v)
    { return *static_cast<serialFlashCarga::pointer>(fields[3]) = v; }

    class pais
    {
    public:
        //
        // ModuloDadosDisponiveisCarga::DadosDisponiveisCarga::pais::value_type
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

    pais::const_reference get_pais () const { return *static_cast<pais::const_pointer>(fields[4]); }
    pais::reference ref_pais () { return *static_cast<pais::pointer>(fields[4]); }
    pais::reference set_pais () { return *static_cast<pais::pointer>(fields[4]); }
    pais::reference set_pais (const ASN1_STD string& v)
    { return *static_cast<pais::pointer>(fields[4]) = v; }

    class uf
    {
    public:
        typedef ModuloTiposEleitorais::UF value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class uf

    uf::const_reference get_uf () const { return *static_cast<uf::const_pointer>(fields[5]); }
    uf::reference ref_uf () { return *static_cast<uf::pointer>(fields[5]); }
    uf::reference set_uf () { return *static_cast<uf::pointer>(fields[5]); }

    class zona
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 9999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class zona

    zona::const_reference get_zona () const { return *static_cast<zona::const_pointer>(fields[6]); }
    zona::reference ref_zona () { return *static_cast<zona::pointer>(fields[6]); }
    zona::reference set_zona () { return *static_cast<zona::pointer>(fields[6]); }
    zona::reference set_zona (ASN1::INTEGER::int_type v)
    { return *static_cast<zona::pointer>(fields[6]) = v; }

    class municipios
    {
    public:
        //
        // ModuloDadosDisponiveisCarga::DadosDisponiveisCarga::municipios::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ModuloDadosDisponiveisCarga::MunicipioDisponivel>
        {
            typedef ASN1::SEQUENCE_OF<ModuloDadosDisponiveisCarga::MunicipioDisponivel> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ModuloDadosDisponiveisCarga::MunicipioDisponivel& val) : Inherited(n, val) {}
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

    municipios::const_reference get_municipios () const { return *static_cast<municipios::const_pointer>(fields[7]); }
    municipios::reference ref_municipios () { return *static_cast<municipios::pointer>(fields[7]); }
    municipios::reference set_municipios () { return *static_cast<municipios::pointer>(fields[7]); }

    class pacotesImportados
    {
    public:
        //
        // ModuloDadosDisponiveisCarga::DadosDisponiveisCarga::pacotesImportados::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ModuloTiposEleitorais::CabecalhoPacote>
        {
            typedef ASN1::SEQUENCE_OF<ModuloTiposEleitorais::CabecalhoPacote> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ModuloTiposEleitorais::CabecalhoPacote& val) : Inherited(n, val) {}
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
    }; // end class pacotesImportados

    pacotesImportados::const_reference get_pacotesImportados () const { return *static_cast<pacotesImportados::const_pointer>(fields[8]); }
    pacotesImportados::reference ref_pacotesImportados () { return *static_cast<pacotesImportados::pointer>(fields[8]); }
    pacotesImportados::reference set_pacotesImportados () { return *static_cast<pacotesImportados::pointer>(fields[8]); }
    void swap(DadosDisponiveisCarga& that) { Inherited::swap(that); }
    DadosDisponiveisCarga * clone() const
    { return static_cast<DadosDisponiveisCarga*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[9];
    static int fieldIds[9];
    static const char* fieldNames[9];
}; // end class DadosDisponiveisCarga

} // namespace ModuloDadosDisponiveisCarga

#endif // __MODULODADOSDISPONIVEISCARGA_H
