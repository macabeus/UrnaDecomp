#ifndef __MODULOESTADOGERALURNA_H
#define __MODULOESTADOGERALURNA_H

#include "asn1.h"

#include "ModuloTiposCadastro.h"
#include "ModuloTiposEcoUrna.h"
#include "ModuloTiposEleitorais.h"

namespace ModuloEstadoGeralUrna {

//
// ModuloEstadoGeralUrna::DadoSecao
//

class DadoSecao : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    DadoSecao(const void* info) : Inherited(info) {}
public:
    DadoSecao() : Inherited(&theInfo) {}
    DadoSecao(const DadoSecao & that) : Inherited(that) {}

    DadoSecao& operator=(const DadoSecao& that)
    { Inherited::operator=(that); return *this; }


    class municipio
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 99999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class municipio

    municipio::const_reference get_municipio () const { return *static_cast<municipio::const_pointer>(fields[0]); }
    municipio::reference ref_municipio () { return *static_cast<municipio::pointer>(fields[0]); }
    municipio::reference set_municipio () { return *static_cast<municipio::pointer>(fields[0]); }
    municipio::reference set_municipio (ASN1::INTEGER::int_type v)
    { return *static_cast<municipio::pointer>(fields[0]) = v; }

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
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 9999> value_type;
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
    void swap(DadoSecao& that) { Inherited::swap(that); }
    DadoSecao * clone() const
    { return static_cast<DadoSecao*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[3];
    static int fieldIds[3];
    static const char* fieldNames[3];
}; // end class DadoSecao

//
// ModuloEstadoGeralUrna::DadoLocal
//

class DadoLocal : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    DadoLocal(const void* info) : Inherited(info) {}
public:
    DadoLocal() : Inherited(&theInfo) {}
    DadoLocal(const DadoLocal & that) : Inherited(that) {}

    DadoLocal& operator=(const DadoLocal& that)
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

    class secaoCarga
    {
    public:
        typedef ModuloEstadoGeralUrna::DadoSecao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class secaoCarga

    secaoCarga::const_reference get_secaoCarga () const { return *static_cast<secaoCarga::const_pointer>(fields[1]); }
    secaoCarga::reference ref_secaoCarga () { return *static_cast<secaoCarga::pointer>(fields[1]); }
    secaoCarga::reference set_secaoCarga () { return *static_cast<secaoCarga::pointer>(fields[1]); }

    class tipoLocalVotacao
    {
    public:
        typedef ModuloTiposCadastro::TipoLocalVotacao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class tipoLocalVotacao

    tipoLocalVotacao::const_reference get_tipoLocalVotacao () const { return *static_cast<tipoLocalVotacao::const_pointer>(fields[2]); }
    tipoLocalVotacao::reference ref_tipoLocalVotacao () { return *static_cast<tipoLocalVotacao::pointer>(fields[2]); }
    tipoLocalVotacao::reference set_tipoLocalVotacao () { return *static_cast<tipoLocalVotacao::pointer>(fields[2]); }
    tipoLocalVotacao::reference set_tipoLocalVotacao (ModuloTiposCadastro::TipoLocalVotacao::NamedNumber v)
    { return *static_cast<tipoLocalVotacao::pointer>(fields[2]) = v; }
    void swap(DadoLocal& that) { Inherited::swap(that); }
    DadoLocal * clone() const
    { return static_cast<DadoLocal*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[3];
    static int fieldIds[3];
    static const char* fieldNames[3];
}; // end class DadoLocal

//
// ModuloEstadoGeralUrna::DadoCorrespondencia
//

class DadoCorrespondencia : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    DadoCorrespondencia(const void* info) : Inherited(info) {}
public:
    DadoCorrespondencia() : Inherited(&theInfo) {}
    DadoCorrespondencia(const DadoCorrespondencia & that) : Inherited(that) {}

    DadoCorrespondencia& operator=(const DadoCorrespondencia& that)
    { Inherited::operator=(that); return *this; }


    class carga
    {
    public:
        typedef ModuloTiposEcoUrna::Carga value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class carga

    carga::const_reference get_carga () const { return *static_cast<carga::const_pointer>(fields[0]); }
    carga::reference ref_carga () { return *static_cast<carga::pointer>(fields[0]); }
    carga::reference set_carga () { return *static_cast<carga::pointer>(fields[0]); }

    class secaoCarga
    {
    public:
        typedef ModuloEstadoGeralUrna::DadoSecao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class secaoCarga

    secaoCarga::const_reference get_secaoCarga () const { return *static_cast<secaoCarga::const_pointer>(fields[1]); }
    secaoCarga::reference ref_secaoCarga () { return *static_cast<secaoCarga::pointer>(fields[1]); }
    secaoCarga::reference set_secaoCarga () { return *static_cast<secaoCarga::pointer>(fields[1]); }

    class assinatura
    {
    public:
        typedef ASN1::OCTET_STRING value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class assinatura

    assinatura::const_reference get_assinatura () const { return *static_cast<assinatura::const_pointer>(fields[2]); }
    assinatura::reference ref_assinatura () { return *static_cast<assinatura::pointer>(fields[2]); }
    assinatura::reference set_assinatura () { return *static_cast<assinatura::pointer>(fields[2]); }
    assinatura::reference set_assinatura (const ASN1::OCTET_STRING& v)
    { return *static_cast<assinatura::pointer>(fields[2]) = v; }
    void swap(DadoCorrespondencia& that) { Inherited::swap(that); }
    DadoCorrespondencia * clone() const
    { return static_cast<DadoCorrespondencia*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[3];
    static int fieldIds[3];
    static const char* fieldNames[3];
}; // end class DadoCorrespondencia

//
// ModuloEstadoGeralUrna::EstadoUrna
//

class EstadoUrna : public ASN1::ENUMERATED
{
    typedef ASN1::ENUMERATED Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EstadoUrna(const void* info) : Inherited(info) {}
public:
    EstadoUrna() : Inherited(&theInfo) {}
    enum NamedNumber {
        unknownEnumeration_ = -1,
        carregando = 1,
        carregada,
        testada
    };

    bool is_carregando() const { return value == carregando; }
    void set_carregando() { value = carregando; }

    bool is_carregada() const { return value == carregada; }
    void set_carregada() { value = carregada; }

    bool is_testada() const { return value == testada; }
    void set_testada() { value = testada; }

    EstadoUrna(const NamedNumber v) : Inherited(&theInfo) { setFromInt(v); }
    EstadoUrna & operator=(const NamedNumber v) { setFromInt(v);  return *this; }
    operator NamedNumber() const { return NamedNumber(asInt()); }

    bool operator == (NamedNumber rhs) const { return value == rhs; }
    bool operator != (NamedNumber rhs) const { return value != rhs; }
    bool operator <  (NamedNumber rhs) const { return value <  rhs; }
    bool operator >  (NamedNumber rhs) const { return value >  rhs; }
    bool operator <= (NamedNumber rhs) const { return value <= rhs; }
    bool operator >= (NamedNumber rhs) const { return value >= rhs; }
    bool operator == (const EstadoUrna &rhs) const { return value == rhs.value; }
    bool operator != (const EstadoUrna &rhs) const { return value != rhs.value; }

    void swap (EstadoUrna & that) { Inherited::swap(that); }
    EstadoUrna * clone() const
    { return static_cast<EstadoUrna*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const char * nameList[];
}; // end class EstadoUrna

//
// ModuloEstadoGeralUrna::TipoUrnaT1   // SYNTHESIZED name (field ['tipoUrnaT1', 'tipoUrnaT2'])
//

class TipoUrnaT1 : public ASN1::ENUMERATED
{
    typedef ASN1::ENUMERATED Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    TipoUrnaT1(const void* info) : Inherited(info) {}
public:
    TipoUrnaT1() : Inherited(&theInfo) {}
    enum NamedNumber {
        unknownEnumeration_ = -1,
        semtipo,
        vota,
        contingencia,
        contingenciavota,
        contingenciavotarecupera
    };

    bool is_semtipo() const { return value == semtipo; }
    void set_semtipo() { value = semtipo; }

    bool is_vota() const { return value == vota; }
    void set_vota() { value = vota; }

    bool is_contingencia() const { return value == contingencia; }
    void set_contingencia() { value = contingencia; }

    bool is_contingenciavota() const { return value == contingenciavota; }
    void set_contingenciavota() { value = contingenciavota; }

    bool is_contingenciavotarecupera() const { return value == contingenciavotarecupera; }
    void set_contingenciavotarecupera() { value = contingenciavotarecupera; }

    TipoUrnaT1(const NamedNumber v) : Inherited(&theInfo) { setFromInt(v); }
    TipoUrnaT1 & operator=(const NamedNumber v) { setFromInt(v);  return *this; }
    operator NamedNumber() const { return NamedNumber(asInt()); }

    bool operator == (NamedNumber rhs) const { return value == rhs; }
    bool operator != (NamedNumber rhs) const { return value != rhs; }
    bool operator <  (NamedNumber rhs) const { return value <  rhs; }
    bool operator >  (NamedNumber rhs) const { return value >  rhs; }
    bool operator <= (NamedNumber rhs) const { return value <= rhs; }
    bool operator >= (NamedNumber rhs) const { return value >= rhs; }
    bool operator == (const TipoUrnaT1 &rhs) const { return value == rhs.value; }
    bool operator != (const TipoUrnaT1 &rhs) const { return value != rhs.value; }

    void swap (TipoUrnaT1 & that) { Inherited::swap(that); }
    TipoUrnaT1 * clone() const
    { return static_cast<TipoUrnaT1*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const char * nameList[];
}; // end class TipoUrnaT1

//
// ModuloEstadoGeralUrna::TipoAjusteDataHora   // named by exact field/RTTI match
//

class TipoAjusteDataHora : public ASN1::ENUMERATED
{
    typedef ASN1::ENUMERATED Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    TipoAjusteDataHora(const void* info) : Inherited(info) {}
public:
    TipoAjusteDataHora() : Inherited(&theInfo) {}
    enum NamedNumber {
        unknownEnumeration_ = -1,
        semAjusteDataHora = 1,
        alterarDataEleicao,
        alterarDataSistema
    };

    bool is_semAjusteDataHora() const { return value == semAjusteDataHora; }
    void set_semAjusteDataHora() { value = semAjusteDataHora; }

    bool is_alterarDataEleicao() const { return value == alterarDataEleicao; }
    void set_alterarDataEleicao() { value = alterarDataEleicao; }

    bool is_alterarDataSistema() const { return value == alterarDataSistema; }
    void set_alterarDataSistema() { value = alterarDataSistema; }

    TipoAjusteDataHora(const NamedNumber v) : Inherited(&theInfo) { setFromInt(v); }
    TipoAjusteDataHora & operator=(const NamedNumber v) { setFromInt(v);  return *this; }
    operator NamedNumber() const { return NamedNumber(asInt()); }

    bool operator == (NamedNumber rhs) const { return value == rhs; }
    bool operator != (NamedNumber rhs) const { return value != rhs; }
    bool operator <  (NamedNumber rhs) const { return value <  rhs; }
    bool operator >  (NamedNumber rhs) const { return value >  rhs; }
    bool operator <= (NamedNumber rhs) const { return value <= rhs; }
    bool operator >= (NamedNumber rhs) const { return value >= rhs; }
    bool operator == (const TipoAjusteDataHora &rhs) const { return value == rhs.value; }
    bool operator != (const TipoAjusteDataHora &rhs) const { return value != rhs.value; }

    void swap (TipoAjusteDataHora & that) { Inherited::swap(that); }
    TipoAjusteDataHora * clone() const
    { return static_cast<TipoAjusteDataHora*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const char * nameList[];
}; // end class TipoAjusteDataHora

//
// ModuloEstadoGeralUrna::DadoCarga
//

class DadoCarga : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    DadoCarga(const void* info) : Inherited(info) {}
public:
    DadoCarga() : Inherited(&theInfo) {}
    DadoCarga(const DadoCarga & that) : Inherited(that) {}

    DadoCarga& operator=(const DadoCarga& that)
    { Inherited::operator=(that); return *this; }


    class turno
    {
    public:
        typedef ModuloTiposEleitorais::Turno value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class turno

    turno::const_reference get_turno () const { return *static_cast<turno::const_pointer>(fields[0]); }
    turno::reference ref_turno () { return *static_cast<turno::pointer>(fields[0]); }
    turno::reference set_turno () { return *static_cast<turno::pointer>(fields[0]); }
    turno::reference set_turno (ModuloTiposEleitorais::Turno::NamedNumber v)
    { return *static_cast<turno::pointer>(fields[0]) = v; }

    class tipoUrnaT1
    {
    public:
        typedef ModuloEstadoGeralUrna::TipoUrnaT1 value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class tipoUrnaT1

    tipoUrnaT1::const_reference get_tipoUrnaT1 () const { return *static_cast<tipoUrnaT1::const_pointer>(fields[1]); }
    tipoUrnaT1::reference ref_tipoUrnaT1 () { return *static_cast<tipoUrnaT1::pointer>(fields[1]); }
    tipoUrnaT1::reference set_tipoUrnaT1 () { return *static_cast<tipoUrnaT1::pointer>(fields[1]); }
    tipoUrnaT1::reference set_tipoUrnaT1 (ModuloEstadoGeralUrna::TipoUrnaT1::NamedNumber v)
    { return *static_cast<tipoUrnaT1::pointer>(fields[1]) = v; }

    class tipoUrnaT2
    {
    public:
        typedef ModuloEstadoGeralUrna::TipoUrnaT1 value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class tipoUrnaT2

    tipoUrnaT2::const_reference get_tipoUrnaT2 () const { return *static_cast<tipoUrnaT2::const_pointer>(fields[2]); }
    tipoUrnaT2::reference ref_tipoUrnaT2 () { return *static_cast<tipoUrnaT2::pointer>(fields[2]); }
    tipoUrnaT2::reference set_tipoUrnaT2 () { return *static_cast<tipoUrnaT2::pointer>(fields[2]); }
    tipoUrnaT2::reference set_tipoUrnaT2 (ModuloEstadoGeralUrna::TipoUrnaT1::NamedNumber v)
    { return *static_cast<tipoUrnaT2::pointer>(fields[2]) = v; }

    class modelo
    {
    public:
        //
        // ModuloEstadoGeralUrna::DadoCarga::modelo::value_type
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
                tpm20 = 2,
                ue2013 = 13,
                ue2015 = 15,
                ue2020 = 20,
                ue2022 = 22
            };

            bool is_tpm20() const { return value == tpm20; }
            void set_tpm20() { value = tpm20; }

            bool is_ue2013() const { return value == ue2013; }
            void set_ue2013() { value = ue2013; }

            bool is_ue2015() const { return value == ue2015; }
            void set_ue2015() { value = ue2015; }

            bool is_ue2020() const { return value == ue2020; }
            void set_ue2020() { value = ue2020; }

            bool is_ue2022() const { return value == ue2022; }
            void set_ue2022() { value = ue2022; }

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
    }; // end class modelo

    modelo::const_reference get_modelo () const { return *static_cast<modelo::const_pointer>(fields[3]); }
    modelo::reference ref_modelo () { return *static_cast<modelo::pointer>(fields[3]); }
    modelo::reference set_modelo () { return *static_cast<modelo::pointer>(fields[3]); }
    modelo::reference set_modelo (modelo::value_type::NamedNumber v)
    { return *static_cast<modelo::pointer>(fields[3]) = v; }

    class fase
    {
    public:
        typedef ModuloTiposEleitorais::Fase value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class fase

    fase::const_reference get_fase () const { return *static_cast<fase::const_pointer>(fields[4]); }
    fase::reference ref_fase () { return *static_cast<fase::pointer>(fields[4]); }
    fase::reference set_fase () { return *static_cast<fase::pointer>(fields[4]); }
    fase::reference set_fase (ModuloTiposEleitorais::Fase::NamedNumber v)
    { return *static_cast<fase::pointer>(fields[4]) = v; }
    void swap(DadoCarga& that) { Inherited::swap(that); }
    DadoCarga * clone() const
    { return static_cast<DadoCarga*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[5];
    static int fieldIds[5];
    static const char* fieldNames[5];
}; // end class DadoCarga

//
// ModuloEstadoGeralUrna::AjusteDataHora
//

class AjusteDataHora : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    AjusteDataHora(const void* info) : Inherited(info) {}
public:
    AjusteDataHora() : Inherited(&theInfo) {}
    AjusteDataHora(const AjusteDataHora & that) : Inherited(that) {}

    AjusteDataHora& operator=(const AjusteDataHora& that)
    { Inherited::operator=(that); return *this; }


    class tipoAjusteDataHora
    {
    public:
        typedef ModuloEstadoGeralUrna::TipoAjusteDataHora value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class tipoAjusteDataHora

    tipoAjusteDataHora::const_reference get_tipoAjusteDataHora () const { return *static_cast<tipoAjusteDataHora::const_pointer>(fields[0]); }
    tipoAjusteDataHora::reference ref_tipoAjusteDataHora () { return *static_cast<tipoAjusteDataHora::pointer>(fields[0]); }
    tipoAjusteDataHora::reference set_tipoAjusteDataHora () { return *static_cast<tipoAjusteDataHora::pointer>(fields[0]); }
    tipoAjusteDataHora::reference set_tipoAjusteDataHora (ModuloEstadoGeralUrna::TipoAjusteDataHora::NamedNumber v)
    { return *static_cast<tipoAjusteDataHora::pointer>(fields[0]) = v; }

    class valorAjusteDataHora
    {
    public:
        typedef ASN1::INTEGER value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class valorAjusteDataHora

    valorAjusteDataHora::const_reference get_valorAjusteDataHora () const { return *static_cast<valorAjusteDataHora::const_pointer>(fields[1]); }
    valorAjusteDataHora::reference ref_valorAjusteDataHora () { return *static_cast<valorAjusteDataHora::pointer>(fields[1]); }
    valorAjusteDataHora::reference set_valorAjusteDataHora () { return *static_cast<valorAjusteDataHora::pointer>(fields[1]); }
    valorAjusteDataHora::reference set_valorAjusteDataHora (ASN1::INTEGER::int_type v)
    { return *static_cast<valorAjusteDataHora::pointer>(fields[1]) = v; }
    void swap(AjusteDataHora& that) { Inherited::swap(that); }
    AjusteDataHora * clone() const
    { return static_cast<AjusteDataHora*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[2];
    static int fieldIds[2];
    static const char* fieldNames[2];
}; // end class AjusteDataHora

//
// ModuloEstadoGeralUrna::EstadoGeralUrna
//

class EstadoGeralUrna : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EstadoGeralUrna(const void* info) : Inherited(info) {}
public:
    EstadoGeralUrna() : Inherited(&theInfo) {}
    EstadoGeralUrna(const EstadoGeralUrna & that) : Inherited(that) {}

    EstadoGeralUrna& operator=(const EstadoGeralUrna& that)
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

    class estadoUrna
    {
    public:
        typedef ModuloEstadoGeralUrna::EstadoUrna value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class estadoUrna

    estadoUrna::const_reference get_estadoUrna () const { return *static_cast<estadoUrna::const_pointer>(fields[1]); }
    estadoUrna::reference ref_estadoUrna () { return *static_cast<estadoUrna::pointer>(fields[1]); }
    estadoUrna::reference set_estadoUrna () { return *static_cast<estadoUrna::pointer>(fields[1]); }
    estadoUrna::reference set_estadoUrna (ModuloEstadoGeralUrna::EstadoUrna::NamedNumber v)
    { return *static_cast<estadoUrna::pointer>(fields[1]) = v; }

    class dadoCarga
    {
    public:
        typedef ModuloEstadoGeralUrna::DadoCarga value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class dadoCarga

    dadoCarga::const_reference get_dadoCarga () const { return *static_cast<dadoCarga::const_pointer>(fields[2]); }
    dadoCarga::reference ref_dadoCarga () { return *static_cast<dadoCarga::pointer>(fields[2]); }
    dadoCarga::reference set_dadoCarga () { return *static_cast<dadoCarga::pointer>(fields[2]); }

    class dadoLocal
    {
    public:
        typedef ModuloEstadoGeralUrna::DadoLocal value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class dadoLocal

    dadoLocal::const_reference get_dadoLocal () const { return *static_cast<dadoLocal::const_pointer>(fields[3]); }
    dadoLocal::reference ref_dadoLocal () { return *static_cast<dadoLocal::pointer>(fields[3]); }
    dadoLocal::reference set_dadoLocal () { return *static_cast<dadoLocal::pointer>(fields[3]); }

    class ajusteDataHora
    {
    public:
        typedef ModuloEstadoGeralUrna::AjusteDataHora value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class ajusteDataHora

    ajusteDataHora::const_reference get_ajusteDataHora () const { return *static_cast<ajusteDataHora::const_pointer>(fields[4]); }
    ajusteDataHora::reference ref_ajusteDataHora () { return *static_cast<ajusteDataHora::pointer>(fields[4]); }
    ajusteDataHora::reference set_ajusteDataHora () { return *static_cast<ajusteDataHora::pointer>(fields[4]); }

    class correspondencia
    {
    public:
        typedef ModuloEstadoGeralUrna::DadoCorrespondencia value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class correspondencia

    correspondencia::const_reference get_correspondencia () const { return *static_cast<correspondencia::const_pointer>(fields[5]); }
    correspondencia::reference ref_correspondencia () { return *static_cast<correspondencia::pointer>(fields[5]); }
    correspondencia::reference set_correspondencia () { return *static_cast<correspondencia::pointer>(fields[5]); }

    class versao
    {
    public:
        typedef ASN1::GeneralString value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class versao

    versao::const_reference get_versao () const { return *static_cast<versao::const_pointer>(fields[6]); }
    versao::reference ref_versao () { return *static_cast<versao::pointer>(fields[6]); }
    versao::reference set_versao () { return *static_cast<versao::pointer>(fields[6]); }
    versao::reference set_versao (const ASN1_STD string& v)
    { return *static_cast<versao::pointer>(fields[6]) = v; }

    class hashVersoesPacotes
    {
    public:
        typedef ASN1::OCTET_STRING value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class hashVersoesPacotes

    hashVersoesPacotes::const_reference get_hashVersoesPacotes () const { return *static_cast<hashVersoesPacotes::const_pointer>(fields[7]); }
    hashVersoesPacotes::reference ref_hashVersoesPacotes () { return *static_cast<hashVersoesPacotes::pointer>(fields[7]); }
    hashVersoesPacotes::reference set_hashVersoesPacotes () { return *static_cast<hashVersoesPacotes::pointer>(fields[7]); }
    hashVersoesPacotes::reference set_hashVersoesPacotes (const ASN1::OCTET_STRING& v)
    { return *static_cast<hashVersoesPacotes::pointer>(fields[7]) = v; }
    void swap(EstadoGeralUrna& that) { Inherited::swap(that); }
    EstadoGeralUrna * clone() const
    { return static_cast<EstadoGeralUrna*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[8];
    static int fieldIds[8];
    static const char* fieldNames[8];
}; // end class EstadoGeralUrna

} // namespace ModuloEstadoGeralUrna

#endif // __MODULOESTADOGERALURNA_H
