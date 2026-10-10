#ifndef __MODULOCANDIDATOS_H
#define __MODULOCANDIDATOS_H

#include "asn1.h"

#include "ModuloTiposEleitorais.h"
#include "ModuloTiposResultadosEcoUrna.h"

namespace ModuloCandidatos {

//
// ModuloCandidatos::CandidatosPorCargos
//

class CandidatosPorCargos : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    CandidatosPorCargos(const void* info) : Inherited(info) {}
public:
    CandidatosPorCargos() : Inherited(&theInfo) {}
    CandidatosPorCargos(const CandidatosPorCargos & that) : Inherited(that) {}

    CandidatosPorCargos& operator=(const CandidatosPorCargos& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_partidos
    };

    class cargo
    {
    public:
        typedef ModuloTiposEleitorais::CodigoCargoConsulta value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class cargo

    cargo::const_reference get_cargo () const { return *static_cast<cargo::const_pointer>(fields[0]); }
    cargo::reference ref_cargo () { return *static_cast<cargo::pointer>(fields[0]); }
    cargo::reference set_cargo () { return *static_cast<cargo::pointer>(fields[0]); }

    class quantidadeVagas
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 99> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class quantidadeVagas

    quantidadeVagas::const_reference get_quantidadeVagas () const { return *static_cast<quantidadeVagas::const_pointer>(fields[1]); }
    quantidadeVagas::reference ref_quantidadeVagas () { return *static_cast<quantidadeVagas::pointer>(fields[1]); }
    quantidadeVagas::reference set_quantidadeVagas () { return *static_cast<quantidadeVagas::pointer>(fields[1]); }
    quantidadeVagas::reference set_quantidadeVagas (ASN1::INTEGER::int_type v)
    { return *static_cast<quantidadeVagas::pointer>(fields[1]) = v; }

    class partidos
    {
    public:
        //
        // ModuloCandidatos::CandidatosPorCargos::partidos::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ModuloTiposResultadosEcoUrna::CandidatoPorPartido>
        {
            typedef ASN1::SEQUENCE_OF<ModuloTiposResultadosEcoUrna::CandidatoPorPartido> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ModuloTiposResultadosEcoUrna::CandidatoPorPartido& val) : Inherited(n, val) {}
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

    partidos::const_reference get_partidos () const
    {
      assert(hasOptionalField(e_partidos));
      return *static_cast<partidos::const_pointer>(fields[2]);
    }
    partidos::reference ref_partidos ()
    {
      assert(hasOptionalField(e_partidos));
      return *static_cast<partidos::pointer>(fields[2]);
    }
    partidos::reference set_partidos ()
    {
      includeOptionalField( e_partidos, 2);
      return *static_cast<partidos::pointer>(fields[2]);
    }
    void omit_partidos () { removeOptionalField( e_partidos); }
    bool partidos_isPresent () const { return hasOptionalField( e_partidos); }
    void swap(CandidatosPorCargos& that) { Inherited::swap(that); }
    CandidatosPorCargos * clone() const
    { return static_cast<CandidatosPorCargos*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[3];
    static int fieldIds[3];
    static const char* fieldNames[3];
}; // end class CandidatosPorCargos

//
// ModuloCandidatos::EntidadeCandidatos
//

class EntidadeCandidatos : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EntidadeCandidatos(const void* info) : Inherited(info) {}
public:
    EntidadeCandidatos() : Inherited(&theInfo) {}
    EntidadeCandidatos(const EntidadeCandidatos & that) : Inherited(that) {}

    EntidadeCandidatos& operator=(const EntidadeCandidatos& that)
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

    class cargos
    {
    public:
        //
        // ModuloCandidatos::EntidadeCandidatos::cargos::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ModuloCandidatos::CandidatosPorCargos>
        {
            typedef ASN1::SEQUENCE_OF<ModuloCandidatos::CandidatosPorCargos> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ModuloCandidatos::CandidatosPorCargos& val) : Inherited(n, val) {}
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

    cargos::const_reference get_cargos () const { return *static_cast<cargos::const_pointer>(fields[2]); }
    cargos::reference ref_cargos () { return *static_cast<cargos::pointer>(fields[2]); }
    cargos::reference set_cargos () { return *static_cast<cargos::pointer>(fields[2]); }
    void swap(EntidadeCandidatos& that) { Inherited::swap(that); }
    EntidadeCandidatos * clone() const
    { return static_cast<EntidadeCandidatos*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[3];
    static int fieldIds[3];
    static const char* fieldNames[3];
}; // end class EntidadeCandidatos

} // namespace ModuloCandidatos

#endif // __MODULOCANDIDATOS_H
