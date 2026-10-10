#ifndef __MODULOCOMPLEMENTOSMUNICIPIOS_H
#define __MODULOCOMPLEMENTOSMUNICIPIOS_H

#include "asn1.h"

#include "ModuloTiposEleitorais.h"

namespace ModuloComplementosMunicipios {

//
// ModuloComplementosMunicipios::InicioHV   // SYNTHESIZED name (field ['inicioHV', 'fimHV', 'dataInicioPropaganda', 'dataFimPropaganda', 'dataInicioRegistroCandidatura', 'dataFimRegistroCandidatura', 'dataInicioConvencaoPartidaria', 'dataFimConvencaoPartidaria', 'dataInicioPrestacaoConta', 'dataFimPrestacaoConta', 'dataSegundoTurno', 'dataNascimento', 'dataNaUF', 'dataNoMunicipio', 'dataEleitorado', 'data'])
//

class InicioHV : public ASN1::NumericString
{
    typedef ASN1::NumericString Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    InicioHV(const void* info) : Inherited(info) {}
public:
    InicioHV() : Inherited(&theInfo) {}
    InicioHV(const base_string& str, const void* info = &theInfo) : Inherited(str, info) {}
    InicioHV(const char* str, const void* info = &theInfo) : Inherited(str, info) {}
    InicioHV(const InicioHV& that) : Inherited(that) {}
    InicioHV& operator=(const ASN1_STD string& that)
    { Inherited::operator=(that); return *this; }
    InicioHV& operator=(const char* that)
    { Inherited::operator=(that); return *this; }
    InicioHV * clone() const
    { return static_cast<InicioHV*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
}; // end class InicioHV

//
// ModuloComplementosMunicipios::HorarioVerao
//

class HorarioVerao : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    HorarioVerao(const void* info) : Inherited(info) {}
public:
    HorarioVerao() : Inherited(&theInfo) {}
    HorarioVerao(const HorarioVerao & that) : Inherited(that) {}

    HorarioVerao& operator=(const HorarioVerao& that)
    { Inherited::operator=(that); return *this; }


    class inicioHV
    {
    public:
        typedef ModuloComplementosMunicipios::InicioHV value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class inicioHV

    inicioHV::const_reference get_inicioHV () const { return *static_cast<inicioHV::const_pointer>(fields[0]); }
    inicioHV::reference ref_inicioHV () { return *static_cast<inicioHV::pointer>(fields[0]); }
    inicioHV::reference set_inicioHV () { return *static_cast<inicioHV::pointer>(fields[0]); }
    inicioHV::reference set_inicioHV (const ASN1_STD string& v)
    { return *static_cast<inicioHV::pointer>(fields[0]) = v; }

    class fimHV
    {
    public:
        typedef ModuloComplementosMunicipios::InicioHV value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class fimHV

    fimHV::const_reference get_fimHV () const { return *static_cast<fimHV::const_pointer>(fields[1]); }
    fimHV::reference ref_fimHV () { return *static_cast<fimHV::pointer>(fields[1]); }
    fimHV::reference set_fimHV () { return *static_cast<fimHV::pointer>(fields[1]); }
    fimHV::reference set_fimHV (const ASN1_STD string& v)
    { return *static_cast<fimHV::pointer>(fields[1]) = v; }

    class diferencaHV
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, -180, 180> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class diferencaHV

    diferencaHV::const_reference get_diferencaHV () const { return *static_cast<diferencaHV::const_pointer>(fields[2]); }
    diferencaHV::reference ref_diferencaHV () { return *static_cast<diferencaHV::pointer>(fields[2]); }
    diferencaHV::reference set_diferencaHV () { return *static_cast<diferencaHV::pointer>(fields[2]); }
    diferencaHV::reference set_diferencaHV (ASN1::INTEGER::int_type v)
    { return *static_cast<diferencaHV::pointer>(fields[2]) = v; }
    void swap(HorarioVerao& that) { Inherited::swap(that); }
    HorarioVerao * clone() const
    { return static_cast<HorarioVerao*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[3];
    static int fieldIds[3];
    static const char* fieldNames[3];
}; // end class HorarioVerao

//
// ModuloComplementosMunicipios::ComplementoMunicipio
//

class ComplementoMunicipio : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    ComplementoMunicipio(const void* info) : Inherited(info) {}
public:
    ComplementoMunicipio() : Inherited(&theInfo) {}
    ComplementoMunicipio(const ComplementoMunicipio & that) : Inherited(that) {}

    ComplementoMunicipio& operator=(const ComplementoMunicipio& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_horarioVerao
    };

    class codigoMunicipio
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 99999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class codigoMunicipio

    codigoMunicipio::const_reference get_codigoMunicipio () const { return *static_cast<codigoMunicipio::const_pointer>(fields[0]); }
    codigoMunicipio::reference ref_codigoMunicipio () { return *static_cast<codigoMunicipio::pointer>(fields[0]); }
    codigoMunicipio::reference set_codigoMunicipio () { return *static_cast<codigoMunicipio::pointer>(fields[0]); }
    codigoMunicipio::reference set_codigoMunicipio (ASN1::INTEGER::int_type v)
    { return *static_cast<codigoMunicipio::pointer>(fields[0]) = v; }

    class fuso
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, -720, 720> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class fuso

    fuso::const_reference get_fuso () const { return *static_cast<fuso::const_pointer>(fields[1]); }
    fuso::reference ref_fuso () { return *static_cast<fuso::pointer>(fields[1]); }
    fuso::reference set_fuso () { return *static_cast<fuso::pointer>(fields[1]); }
    fuso::reference set_fuso (ASN1::INTEGER::int_type v)
    { return *static_cast<fuso::pointer>(fields[1]) = v; }

    class desligaColetaBiometria
    {
    public:
        typedef ASN1::BOOLEAN value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class desligaColetaBiometria

    desligaColetaBiometria::const_reference get_desligaColetaBiometria () const { return *static_cast<desligaColetaBiometria::const_pointer>(fields[2]); }
    desligaColetaBiometria::reference ref_desligaColetaBiometria () { return *static_cast<desligaColetaBiometria::pointer>(fields[2]); }
    desligaColetaBiometria::reference set_desligaColetaBiometria () { return *static_cast<desligaColetaBiometria::pointer>(fields[2]); }
    desligaColetaBiometria::reference set_desligaColetaBiometria (bool v)
    { return *static_cast<desligaColetaBiometria::pointer>(fields[2]) = v; }

    class horarioVerao
    {
    public:
        typedef ModuloComplementosMunicipios::HorarioVerao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class horarioVerao

    horarioVerao::const_reference get_horarioVerao () const
    {
      assert(hasOptionalField(e_horarioVerao));
      return *static_cast<horarioVerao::const_pointer>(fields[3]);
    }
    horarioVerao::reference ref_horarioVerao ()
    {
      assert(hasOptionalField(e_horarioVerao));
      return *static_cast<horarioVerao::pointer>(fields[3]);
    }
    horarioVerao::reference set_horarioVerao ()
    {
      includeOptionalField( e_horarioVerao, 3);
      return *static_cast<horarioVerao::pointer>(fields[3]);
    }
    void omit_horarioVerao () { removeOptionalField( e_horarioVerao); }
    bool horarioVerao_isPresent () const { return hasOptionalField( e_horarioVerao); }
    void swap(ComplementoMunicipio& that) { Inherited::swap(that); }
    ComplementoMunicipio * clone() const
    { return static_cast<ComplementoMunicipio*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[4];
    static int fieldIds[4];
    static const char* fieldNames[4];
}; // end class ComplementoMunicipio

//
// ModuloComplementosMunicipios::ComplementosMunicipiosUF
//

class ComplementosMunicipiosUF : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    ComplementosMunicipiosUF(const void* info) : Inherited(info) {}
public:
    ComplementosMunicipiosUF() : Inherited(&theInfo) {}
    ComplementosMunicipiosUF(const ComplementosMunicipiosUF & that) : Inherited(that) {}

    ComplementosMunicipiosUF& operator=(const ComplementosMunicipiosUF& that)
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

    class fusoTRE
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, -720, 720> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class fusoTRE

    fusoTRE::const_reference get_fusoTRE () const { return *static_cast<fusoTRE::const_pointer>(fields[1]); }
    fusoTRE::reference ref_fusoTRE () { return *static_cast<fusoTRE::pointer>(fields[1]); }
    fusoTRE::reference set_fusoTRE () { return *static_cast<fusoTRE::pointer>(fields[1]); }
    fusoTRE::reference set_fusoTRE (ASN1::INTEGER::int_type v)
    { return *static_cast<fusoTRE::pointer>(fields[1]) = v; }

    class complementosMunicipios
    {
    public:
        //
        // ModuloComplementosMunicipios::ComplementosMunicipiosUF::complementosMunicipios::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ModuloComplementosMunicipios::ComplementoMunicipio>
        {
            typedef ASN1::SEQUENCE_OF<ModuloComplementosMunicipios::ComplementoMunicipio> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ModuloComplementosMunicipios::ComplementoMunicipio& val) : Inherited(n, val) {}
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
    }; // end class complementosMunicipios

    complementosMunicipios::const_reference get_complementosMunicipios () const { return *static_cast<complementosMunicipios::const_pointer>(fields[2]); }
    complementosMunicipios::reference ref_complementosMunicipios () { return *static_cast<complementosMunicipios::pointer>(fields[2]); }
    complementosMunicipios::reference set_complementosMunicipios () { return *static_cast<complementosMunicipios::pointer>(fields[2]); }
    void swap(ComplementosMunicipiosUF& that) { Inherited::swap(that); }
    ComplementosMunicipiosUF * clone() const
    { return static_cast<ComplementosMunicipiosUF*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[3];
    static int fieldIds[3];
    static const char* fieldNames[3];
}; // end class ComplementosMunicipiosUF

//
// ModuloComplementosMunicipios::EntidadeComplementosMunicipios
//

class EntidadeComplementosMunicipios : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EntidadeComplementosMunicipios(const void* info) : Inherited(info) {}
public:
    EntidadeComplementosMunicipios() : Inherited(&theInfo) {}
    EntidadeComplementosMunicipios(const EntidadeComplementosMunicipios & that) : Inherited(that) {}

    EntidadeComplementosMunicipios& operator=(const EntidadeComplementosMunicipios& that)
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

    class municipiosUf
    {
    public:
        //
        // ModuloComplementosMunicipios::EntidadeComplementosMunicipios::municipiosUf::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ModuloComplementosMunicipios::ComplementosMunicipiosUF>
        {
            typedef ASN1::SEQUENCE_OF<ModuloComplementosMunicipios::ComplementosMunicipiosUF> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ModuloComplementosMunicipios::ComplementosMunicipiosUF& val) : Inherited(n, val) {}
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
    }; // end class municipiosUf

    municipiosUf::const_reference get_municipiosUf () const { return *static_cast<municipiosUf::const_pointer>(fields[1]); }
    municipiosUf::reference ref_municipiosUf () { return *static_cast<municipiosUf::pointer>(fields[1]); }
    municipiosUf::reference set_municipiosUf () { return *static_cast<municipiosUf::pointer>(fields[1]); }
    void swap(EntidadeComplementosMunicipios& that) { Inherited::swap(that); }
    EntidadeComplementosMunicipios * clone() const
    { return static_cast<EntidadeComplementosMunicipios*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[2];
    static int fieldIds[2];
    static const char* fieldNames[2];
}; // end class EntidadeComplementosMunicipios

} // namespace ModuloComplementosMunicipios

#endif // __MODULOCOMPLEMENTOSMUNICIPIOS_H
