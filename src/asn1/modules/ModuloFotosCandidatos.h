#ifndef __MODULOFOTOSCANDIDATOS_H
#define __MODULOFOTOSCANDIDATOS_H

#include "asn1.h"

#include "ModuloTiposEleitorais.h"

namespace ModuloFotosCandidatos {

//
// ModuloFotosCandidatos::CodigoCandidato   // SYNTHESIZED name (field ['codigoCandidato', 'codigo', 'candidatoSubstituido', 'drap'])
//

class CodigoCandidato : public ASN1::GeneralString
{
    typedef ASN1::GeneralString Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    CodigoCandidato(const void* info) : Inherited(info) {}
public:
    CodigoCandidato() : Inherited(&theInfo) {}
    CodigoCandidato(const base_string& str, const void* info = &theInfo) : Inherited(str, info) {}
    CodigoCandidato(const char* str, const void* info = &theInfo) : Inherited(str, info) {}
    CodigoCandidato(const CodigoCandidato& that) : Inherited(that) {}
    CodigoCandidato& operator=(const ASN1_STD string& that)
    { Inherited::operator=(that); return *this; }
    CodigoCandidato& operator=(const char* that)
    { Inherited::operator=(that); return *this; }
    CodigoCandidato * clone() const
    { return static_cast<CodigoCandidato*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
}; // end class CodigoCandidato

//
// ModuloFotosCandidatos::FotoCandidato
//

class FotoCandidato : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    FotoCandidato(const void* info) : Inherited(info) {}
public:
    FotoCandidato() : Inherited(&theInfo) {}
    FotoCandidato(const FotoCandidato & that) : Inherited(that) {}

    FotoCandidato& operator=(const FotoCandidato& that)
    { Inherited::operator=(that); return *this; }


    class codigoCandidato
    {
    public:
        typedef ModuloFotosCandidatos::CodigoCandidato value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class codigoCandidato

    codigoCandidato::const_reference get_codigoCandidato () const { return *static_cast<codigoCandidato::const_pointer>(fields[0]); }
    codigoCandidato::reference ref_codigoCandidato () { return *static_cast<codigoCandidato::pointer>(fields[0]); }
    codigoCandidato::reference set_codigoCandidato () { return *static_cast<codigoCandidato::pointer>(fields[0]); }
    codigoCandidato::reference set_codigoCandidato (const ASN1_STD string& v)
    { return *static_cast<codigoCandidato::pointer>(fields[0]) = v; }

    class foto
    {
    public:
        typedef ModuloTiposEleitorais::Foto value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class foto

    foto::const_reference get_foto () const { return *static_cast<foto::const_pointer>(fields[1]); }
    foto::reference ref_foto () { return *static_cast<foto::pointer>(fields[1]); }
    foto::reference set_foto () { return *static_cast<foto::pointer>(fields[1]); }
    void swap(FotoCandidato& that) { Inherited::swap(that); }
    FotoCandidato * clone() const
    { return static_cast<FotoCandidato*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[2];
    static int fieldIds[2];
    static const char* fieldNames[2];
}; // end class FotoCandidato

//
// ModuloFotosCandidatos::EntidadeFotosCandidatos
//

class EntidadeFotosCandidatos : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EntidadeFotosCandidatos(const void* info) : Inherited(info) {}
public:
    EntidadeFotosCandidatos() : Inherited(&theInfo) {}
    EntidadeFotosCandidatos(const EntidadeFotosCandidatos & that) : Inherited(that) {}

    EntidadeFotosCandidatos& operator=(const EntidadeFotosCandidatos& that)
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

    class fotos
    {
    public:
        //
        // ModuloFotosCandidatos::EntidadeFotosCandidatos::fotos::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ModuloFotosCandidatos::FotoCandidato>
        {
            typedef ASN1::SEQUENCE_OF<ModuloFotosCandidatos::FotoCandidato> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ModuloFotosCandidatos::FotoCandidato& val) : Inherited(n, val) {}
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
    }; // end class fotos

    fotos::const_reference get_fotos () const { return *static_cast<fotos::const_pointer>(fields[2]); }
    fotos::reference ref_fotos () { return *static_cast<fotos::pointer>(fields[2]); }
    fotos::reference set_fotos () { return *static_cast<fotos::pointer>(fields[2]); }
    void swap(EntidadeFotosCandidatos& that) { Inherited::swap(that); }
    EntidadeFotosCandidatos * clone() const
    { return static_cast<EntidadeFotosCandidatos*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[3];
    static int fieldIds[3];
    static const char* fieldNames[3];
}; // end class EntidadeFotosCandidatos

} // namespace ModuloFotosCandidatos

#endif // __MODULOFOTOSCANDIDATOS_H
