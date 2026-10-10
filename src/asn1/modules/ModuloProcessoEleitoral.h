#ifndef __MODULOPROCESSOELEITORAL_H
#define __MODULOPROCESSOELEITORAL_H

#include "asn1.h"

#include "ModuloComplementosMunicipios.h"
#include "ModuloEleicao.h"
#include "ModuloEleitores.h"
#include "ModuloTiposEleitorais.h"

namespace ModuloProcessoEleitoral {

//
// ModuloProcessoEleitoral::Pleito
//

class Pleito : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    Pleito(const void* info) : Inherited(info) {}
public:
    Pleito() : Inherited(&theInfo) {}
    Pleito(const Pleito & that) : Inherited(that) {}

    Pleito& operator=(const Pleito& that)
    { Inherited::operator=(that); return *this; }


    class id
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 99999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class id

    id::const_reference get_id () const { return *static_cast<id::const_pointer>(fields[0]); }
    id::reference ref_id () { return *static_cast<id::pointer>(fields[0]); }
    id::reference set_id () { return *static_cast<id::pointer>(fields[0]); }
    id::reference set_id (ASN1::INTEGER::int_type v)
    { return *static_cast<id::pointer>(fields[0]) = v; }

    class nome
    {
    public:
        //
        // ModuloProcessoEleitoral::Pleito::nome::value_type
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

    class data
    {
    public:
        typedef ModuloComplementosMunicipios::InicioHV value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class data

    data::const_reference get_data () const { return *static_cast<data::const_pointer>(fields[2]); }
    data::reference ref_data () { return *static_cast<data::pointer>(fields[2]); }
    data::reference set_data () { return *static_cast<data::pointer>(fields[2]); }
    data::reference set_data (const ASN1_STD string& v)
    { return *static_cast<data::pointer>(fields[2]) = v; }
    void swap(Pleito& that) { Inherited::swap(that); }
    Pleito * clone() const
    { return static_cast<Pleito*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[3];
    static int fieldIds[3];
    static const char* fieldNames[3];
}; // end class Pleito

//
// ModuloProcessoEleitoral::OrigemConfiguracao   // named by exact field/RTTI match
//

class OrigemConfiguracao : public ASN1::ENUMERATED
{
    typedef ASN1::ENUMERATED Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    OrigemConfiguracao(const void* info) : Inherited(info) {}
public:
    OrigemConfiguracao() : Inherited(&theInfo) {}
    enum NamedNumber {
        unknownEnumeration_ = -1,
        oficial = 1,
        comunitaria
    };

    bool is_oficial() const { return value == oficial; }
    void set_oficial() { value = oficial; }

    bool is_comunitaria() const { return value == comunitaria; }
    void set_comunitaria() { value = comunitaria; }

    OrigemConfiguracao(const NamedNumber v) : Inherited(&theInfo) { setFromInt(v); }
    OrigemConfiguracao & operator=(const NamedNumber v) { setFromInt(v);  return *this; }
    operator NamedNumber() const { return NamedNumber(asInt()); }

    bool operator == (NamedNumber rhs) const { return value == rhs; }
    bool operator != (NamedNumber rhs) const { return value != rhs; }
    bool operator <  (NamedNumber rhs) const { return value <  rhs; }
    bool operator >  (NamedNumber rhs) const { return value >  rhs; }
    bool operator <= (NamedNumber rhs) const { return value <= rhs; }
    bool operator >= (NamedNumber rhs) const { return value >= rhs; }
    bool operator == (const OrigemConfiguracao &rhs) const { return value == rhs.value; }
    bool operator != (const OrigemConfiguracao &rhs) const { return value != rhs.value; }

    void swap (OrigemConfiguracao & that) { Inherited::swap(that); }
    OrigemConfiguracao * clone() const
    { return static_cast<OrigemConfiguracao*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const char * nameList[];
}; // end class OrigemConfiguracao

//
// ModuloProcessoEleitoral::EntidadeProcessoEleitoral
//

class EntidadeProcessoEleitoral : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EntidadeProcessoEleitoral(const void* info) : Inherited(info) {}
public:
    EntidadeProcessoEleitoral() : Inherited(&theInfo) {}
    EntidadeProcessoEleitoral(const EntidadeProcessoEleitoral & that) : Inherited(that) {}

    EntidadeProcessoEleitoral& operator=(const EntidadeProcessoEleitoral& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_dataEleitorado,
      e_pleito2
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

    class nome
    {
    public:
        //
        // ModuloProcessoEleitoral::EntidadeProcessoEleitoral::nome::value_type
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

    class dataEleitorado
    {
    public:
        typedef ModuloComplementosMunicipios::InicioHV value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class dataEleitorado

    dataEleitorado::const_reference get_dataEleitorado () const
    {
      assert(hasOptionalField(e_dataEleitorado));
      return *static_cast<dataEleitorado::const_pointer>(fields[2]);
    }
    dataEleitorado::reference ref_dataEleitorado ()
    {
      assert(hasOptionalField(e_dataEleitorado));
      return *static_cast<dataEleitorado::pointer>(fields[2]);
    }
    dataEleitorado::reference set_dataEleitorado ()
    {
      includeOptionalField( e_dataEleitorado, 2);
      return *static_cast<dataEleitorado::pointer>(fields[2]);
    }
    dataEleitorado::reference set_dataEleitorado (const ASN1_STD string& v)
    {
      includeOptionalField( e_dataEleitorado, 2);
      return *static_cast<dataEleitorado::pointer>(fields[2]) = v;
    }
    void omit_dataEleitorado () { removeOptionalField( e_dataEleitorado); }
    bool dataEleitorado_isPresent () const { return hasOptionalField( e_dataEleitorado); }

    class pleito1
    {
    public:
        typedef ModuloProcessoEleitoral::Pleito value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class pleito1

    pleito1::const_reference get_pleito1 () const { return *static_cast<pleito1::const_pointer>(fields[3]); }
    pleito1::reference ref_pleito1 () { return *static_cast<pleito1::pointer>(fields[3]); }
    pleito1::reference set_pleito1 () { return *static_cast<pleito1::pointer>(fields[3]); }

    class pleito2
    {
    public:
        typedef ModuloProcessoEleitoral::Pleito value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class pleito2

    pleito2::const_reference get_pleito2 () const
    {
      assert(hasOptionalField(e_pleito2));
      return *static_cast<pleito2::const_pointer>(fields[4]);
    }
    pleito2::reference ref_pleito2 ()
    {
      assert(hasOptionalField(e_pleito2));
      return *static_cast<pleito2::pointer>(fields[4]);
    }
    pleito2::reference set_pleito2 ()
    {
      includeOptionalField( e_pleito2, 4);
      return *static_cast<pleito2::pointer>(fields[4]);
    }
    void omit_pleito2 () { removeOptionalField( e_pleito2); }
    bool pleito2_isPresent () const { return hasOptionalField( e_pleito2); }

    class tipoEleicaoPrincipal
    {
    public:
        typedef ModuloEleicao::Tipo value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class tipoEleicaoPrincipal

    tipoEleicaoPrincipal::const_reference get_tipoEleicaoPrincipal () const { return *static_cast<tipoEleicaoPrincipal::const_pointer>(fields[5]); }
    tipoEleicaoPrincipal::reference ref_tipoEleicaoPrincipal () { return *static_cast<tipoEleicaoPrincipal::pointer>(fields[5]); }
    tipoEleicaoPrincipal::reference set_tipoEleicaoPrincipal () { return *static_cast<tipoEleicaoPrincipal::pointer>(fields[5]); }
    tipoEleicaoPrincipal::reference set_tipoEleicaoPrincipal (ModuloEleicao::Tipo::NamedNumber v)
    { return *static_cast<tipoEleicaoPrincipal::pointer>(fields[5]) = v; }

    class abrangenciaEleicaoPrincipal
    {
    public:
        typedef ModuloTiposEleitorais::TipoAbrangencia value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class abrangenciaEleicaoPrincipal

    abrangenciaEleicaoPrincipal::const_reference get_abrangenciaEleicaoPrincipal () const { return *static_cast<abrangenciaEleicaoPrincipal::const_pointer>(fields[6]); }
    abrangenciaEleicaoPrincipal::reference ref_abrangenciaEleicaoPrincipal () { return *static_cast<abrangenciaEleicaoPrincipal::pointer>(fields[6]); }
    abrangenciaEleicaoPrincipal::reference set_abrangenciaEleicaoPrincipal () { return *static_cast<abrangenciaEleicaoPrincipal::pointer>(fields[6]); }
    abrangenciaEleicaoPrincipal::reference set_abrangenciaEleicaoPrincipal (ModuloTiposEleitorais::TipoAbrangencia::NamedNumber v)
    { return *static_cast<abrangenciaEleicaoPrincipal::pointer>(fields[6]) = v; }

    class permiteMRJ
    {
    public:
        typedef ASN1::BOOLEAN value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class permiteMRJ

    permiteMRJ::const_reference get_permiteMRJ () const { return *static_cast<permiteMRJ::const_pointer>(fields[7]); }
    permiteMRJ::reference ref_permiteMRJ () { return *static_cast<permiteMRJ::pointer>(fields[7]); }
    permiteMRJ::reference set_permiteMRJ () { return *static_cast<permiteMRJ::pointer>(fields[7]); }
    permiteMRJ::reference set_permiteMRJ (bool v)
    { return *static_cast<permiteMRJ::pointer>(fields[7]) = v; }

    class utilizaBiometria
    {
    public:
        typedef ASN1::BOOLEAN value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class utilizaBiometria

    utilizaBiometria::const_reference get_utilizaBiometria () const { return *static_cast<utilizaBiometria::const_pointer>(fields[8]); }
    utilizaBiometria::reference ref_utilizaBiometria () { return *static_cast<utilizaBiometria::pointer>(fields[8]); }
    utilizaBiometria::reference set_utilizaBiometria () { return *static_cast<utilizaBiometria::pointer>(fields[8]); }
    utilizaBiometria::reference set_utilizaBiometria (bool v)
    { return *static_cast<utilizaBiometria::pointer>(fields[8]) = v; }

    class tiposTTEPermitidos
    {
    public:
        //
        // ModuloProcessoEleitoral::EntidadeProcessoEleitoral::tiposTTEPermitidos::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ModuloEleitores::EleitorSequencia::tipoTransferenciaTemporaria::value_type>
        {
            typedef ASN1::SEQUENCE_OF<ModuloEleitores::EleitorSequencia::tipoTransferenciaTemporaria::value_type> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ModuloEleitores::EleitorSequencia::tipoTransferenciaTemporaria::value_type& val) : Inherited(n, val) {}
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
    }; // end class tiposTTEPermitidos

    tiposTTEPermitidos::const_reference get_tiposTTEPermitidos () const { return *static_cast<tiposTTEPermitidos::const_pointer>(fields[9]); }
    tiposTTEPermitidos::reference ref_tiposTTEPermitidos () { return *static_cast<tiposTTEPermitidos::pointer>(fields[9]); }
    tiposTTEPermitidos::reference set_tiposTTEPermitidos () { return *static_cast<tiposTTEPermitidos::pointer>(fields[9]); }

    class tipoIdentificadorPrincipalEleitor
    {
    public:
        typedef ModuloTiposEleitorais::TipoIdentificadorEleitor value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class tipoIdentificadorPrincipalEleitor

    tipoIdentificadorPrincipalEleitor::const_reference get_tipoIdentificadorPrincipalEleitor () const { return *static_cast<tipoIdentificadorPrincipalEleitor::const_pointer>(fields[10]); }
    tipoIdentificadorPrincipalEleitor::reference ref_tipoIdentificadorPrincipalEleitor () { return *static_cast<tipoIdentificadorPrincipalEleitor::pointer>(fields[10]); }
    tipoIdentificadorPrincipalEleitor::reference set_tipoIdentificadorPrincipalEleitor () { return *static_cast<tipoIdentificadorPrincipalEleitor::pointer>(fields[10]); }
    tipoIdentificadorPrincipalEleitor::reference set_tipoIdentificadorPrincipalEleitor (ModuloTiposEleitorais::TipoIdentificadorEleitor::NamedNumber v)
    { return *static_cast<tipoIdentificadorPrincipalEleitor::pointer>(fields[10]) = v; }

    class tiposIdentificadoresPermitidos
    {
    public:
        //
        // ModuloProcessoEleitoral::EntidadeProcessoEleitoral::tiposIdentificadoresPermitidos::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ModuloTiposEleitorais::TipoIdentificadorEleitor>
        {
            typedef ASN1::SEQUENCE_OF<ModuloTiposEleitorais::TipoIdentificadorEleitor> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ModuloTiposEleitorais::TipoIdentificadorEleitor& val) : Inherited(n, val) {}
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
    }; // end class tiposIdentificadoresPermitidos

    tiposIdentificadoresPermitidos::const_reference get_tiposIdentificadoresPermitidos () const { return *static_cast<tiposIdentificadoresPermitidos::const_pointer>(fields[11]); }
    tiposIdentificadoresPermitidos::reference ref_tiposIdentificadoresPermitidos () { return *static_cast<tiposIdentificadoresPermitidos::pointer>(fields[11]); }
    tiposIdentificadoresPermitidos::reference set_tiposIdentificadoresPermitidos () { return *static_cast<tiposIdentificadoresPermitidos::pointer>(fields[11]); }

    class origemConfiguracao
    {
    public:
        typedef ModuloProcessoEleitoral::OrigemConfiguracao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class origemConfiguracao

    origemConfiguracao::const_reference get_origemConfiguracao () const { return *static_cast<origemConfiguracao::const_pointer>(fields[12]); }
    origemConfiguracao::reference ref_origemConfiguracao () { return *static_cast<origemConfiguracao::pointer>(fields[12]); }
    origemConfiguracao::reference set_origemConfiguracao () { return *static_cast<origemConfiguracao::pointer>(fields[12]); }
    origemConfiguracao::reference set_origemConfiguracao (ModuloProcessoEleitoral::OrigemConfiguracao::NamedNumber v)
    { return *static_cast<origemConfiguracao::pointer>(fields[12]) = v; }

    class ufs
    {
    public:
        //
        // ModuloProcessoEleitoral::EntidadeProcessoEleitoral::ufs::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ModuloTiposEleitorais::SiglaUF>
        {
            typedef ASN1::SEQUENCE_OF<ModuloTiposEleitorais::SiglaUF> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ModuloTiposEleitorais::SiglaUF& val) : Inherited(n, val) {}
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
    }; // end class ufs

    ufs::const_reference get_ufs () const { return *static_cast<ufs::const_pointer>(fields[13]); }
    ufs::reference ref_ufs () { return *static_cast<ufs::pointer>(fields[13]); }
    ufs::reference set_ufs () { return *static_cast<ufs::pointer>(fields[13]); }
    void swap(EntidadeProcessoEleitoral& that) { Inherited::swap(that); }
    EntidadeProcessoEleitoral * clone() const
    { return static_cast<EntidadeProcessoEleitoral*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[14];
    static int fieldIds[14];
    static const char* fieldNames[14];
}; // end class EntidadeProcessoEleitoral

} // namespace ModuloProcessoEleitoral

#endif // __MODULOPROCESSOELEITORAL_H
