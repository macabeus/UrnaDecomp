#ifndef __MODULOTIPOSELEITORAIS_H
#define __MODULOTIPOSELEITORAIS_H

#include "asn1.h"

#include "ModuloTiposPacotes.h"

namespace ModuloTiposEleitorais {

//
// ModuloTiposEleitorais::SiglaUF   // named by exact field/RTTI match
//

class SiglaUF : public ASN1::GeneralString
{
    typedef ASN1::GeneralString Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    SiglaUF(const void* info) : Inherited(info) {}
public:
    SiglaUF() : Inherited(&theInfo) {}
    SiglaUF(const base_string& str, const void* info = &theInfo) : Inherited(str, info) {}
    SiglaUF(const char* str, const void* info = &theInfo) : Inherited(str, info) {}
    SiglaUF(const SiglaUF& that) : Inherited(that) {}
    SiglaUF& operator=(const ASN1_STD string& that)
    { Inherited::operator=(that); return *this; }
    SiglaUF& operator=(const char* that)
    { Inherited::operator=(that); return *this; }
    SiglaUF * clone() const
    { return static_cast<SiglaUF*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
}; // end class SiglaUF

//
// ModuloTiposEleitorais::UF
//

class UF : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    UF(const void* info) : Inherited(info) {}
public:
    UF() : Inherited(&theInfo) {}
    UF(const UF & that) : Inherited(that) {}

    UF& operator=(const UF& that)
    { Inherited::operator=(that); return *this; }


    class sigla
    {
    public:
        typedef ModuloTiposEleitorais::SiglaUF value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class sigla

    sigla::const_reference get_sigla () const { return *static_cast<sigla::const_pointer>(fields[0]); }
    sigla::reference ref_sigla () { return *static_cast<sigla::pointer>(fields[0]); }
    sigla::reference set_sigla () { return *static_cast<sigla::pointer>(fields[0]); }
    sigla::reference set_sigla (const ASN1_STD string& v)
    { return *static_cast<sigla::pointer>(fields[0]) = v; }

    class nome
    {
    public:
        //
        // ModuloTiposEleitorais::UF::nome::value_type
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
    void swap(UF& that) { Inherited::swap(that); }
    UF * clone() const
    { return static_cast<UF*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[2];
    static int fieldIds[2];
    static const char* fieldNames[2];
}; // end class UF

//
// ModuloTiposEleitorais::MunicipioZona
//

class MunicipioZona : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    MunicipioZona(const void* info) : Inherited(info) {}
public:
    MunicipioZona() : Inherited(&theInfo) {}
    MunicipioZona(const MunicipioZona & that) : Inherited(that) {}

    MunicipioZona& operator=(const MunicipioZona& that)
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
    void swap(MunicipioZona& that) { Inherited::swap(that); }
    MunicipioZona * clone() const
    { return static_cast<MunicipioZona*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[2];
    static int fieldIds[2];
    static const char* fieldNames[2];
}; // end class MunicipioZona

//
// ModuloTiposEleitorais::TipoIdentificadorEleitor
//

class TipoIdentificadorEleitor : public ASN1::ENUMERATED
{
    typedef ASN1::ENUMERATED Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    TipoIdentificadorEleitor(const void* info) : Inherited(info) {}
public:
    TipoIdentificadorEleitor() : Inherited(&theInfo) {}
    enum NamedNumber {
        unknownEnumeration_ = -1,
        numeroInscricaoEleitoral = 1,
        numeroCPF,
        numeroLivre
    };

    bool is_numeroInscricaoEleitoral() const { return value == numeroInscricaoEleitoral; }
    void set_numeroInscricaoEleitoral() { value = numeroInscricaoEleitoral; }

    bool is_numeroCPF() const { return value == numeroCPF; }
    void set_numeroCPF() { value = numeroCPF; }

    bool is_numeroLivre() const { return value == numeroLivre; }
    void set_numeroLivre() { value = numeroLivre; }

    TipoIdentificadorEleitor(const NamedNumber v) : Inherited(&theInfo) { setFromInt(v); }
    TipoIdentificadorEleitor & operator=(const NamedNumber v) { setFromInt(v);  return *this; }
    operator NamedNumber() const { return NamedNumber(asInt()); }

    bool operator == (NamedNumber rhs) const { return value == rhs; }
    bool operator != (NamedNumber rhs) const { return value != rhs; }
    bool operator <  (NamedNumber rhs) const { return value <  rhs; }
    bool operator >  (NamedNumber rhs) const { return value >  rhs; }
    bool operator <= (NamedNumber rhs) const { return value <= rhs; }
    bool operator >= (NamedNumber rhs) const { return value >= rhs; }
    bool operator == (const TipoIdentificadorEleitor &rhs) const { return value == rhs.value; }
    bool operator != (const TipoIdentificadorEleitor &rhs) const { return value != rhs.value; }

    void swap (TipoIdentificadorEleitor & that) { Inherited::swap(that); }
    TipoIdentificadorEleitor * clone() const
    { return static_cast<TipoIdentificadorEleitor*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const char * nameList[];
}; // end class TipoIdentificadorEleitor

//
// ModuloTiposEleitorais::IdentificadorEleitor
//

class IdentificadorEleitor : public ASN1::CHOICE
{
    typedef ASN1::CHOICE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    IdentificadorEleitor(const void* info) : Inherited(info) {}
public:
    IdentificadorEleitor() : Inherited(&theInfo) {}
    enum Choice_Ids {
      unknownSelection_ = -2,
      unselected_ = -1,
      numeroInscricao0 = 0,
      numeroCPF1 = 1,
      identificacaoLivre2 = 2
    };

    class numeroInscricao
    {
    public:
        enum Id { eid = 0 };
        //
        // ModuloTiposEleitorais::IdentificadorEleitor::numeroInscricao::value_type
        //

        class value_type : public ASN1::NumericString
        {
            typedef ASN1::NumericString Inherited;
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
    }; // end class numeroInscricao

    numeroInscricao::const_reference get_numeroInscricao () const
    {
        assert(currentSelection() == numeroInscricao::eid);
        return *static_cast<numeroInscricao::const_pointer>(choice.get());
    }
    numeroInscricao::reference ref_numeroInscricao ()
    {
        assert(currentSelection() == numeroInscricao::eid);
        return *static_cast<numeroInscricao::pointer>(choice.get());
    }
    numeroInscricao::reference select_numeroInscricao ()
    {
        return *static_cast<numeroInscricao::pointer>(setSelection(numeroInscricao::eid, ASN1::AbstractData::create(&numeroInscricao::value_type::theInfo)));
    }
    bool numeroInscricao_isSelected () const { return currentSelection() == numeroInscricao::eid; }

    class numeroCPF
    {
    public:
        enum Id { eid = 1 };
        //
        // ModuloTiposEleitorais::IdentificadorEleitor::numeroCPF::value_type
        //

        class value_type : public ASN1::NumericString
        {
            typedef ASN1::NumericString Inherited;
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
    }; // end class numeroCPF

    numeroCPF::const_reference get_numeroCPF () const
    {
        assert(currentSelection() == numeroCPF::eid);
        return *static_cast<numeroCPF::const_pointer>(choice.get());
    }
    numeroCPF::reference ref_numeroCPF ()
    {
        assert(currentSelection() == numeroCPF::eid);
        return *static_cast<numeroCPF::pointer>(choice.get());
    }
    numeroCPF::reference select_numeroCPF ()
    {
        return *static_cast<numeroCPF::pointer>(setSelection(numeroCPF::eid, ASN1::AbstractData::create(&numeroCPF::value_type::theInfo)));
    }
    bool numeroCPF_isSelected () const { return currentSelection() == numeroCPF::eid; }

    class identificacaoLivre
    {
    public:
        enum Id { eid = 2 };
        //
        // ModuloTiposEleitorais::IdentificadorEleitor::identificacaoLivre::value_type
        //

        class value_type : public ASN1::NumericString
        {
            typedef ASN1::NumericString Inherited;
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
    }; // end class identificacaoLivre

    identificacaoLivre::const_reference get_identificacaoLivre () const
    {
        assert(currentSelection() == identificacaoLivre::eid);
        return *static_cast<identificacaoLivre::const_pointer>(choice.get());
    }
    identificacaoLivre::reference ref_identificacaoLivre ()
    {
        assert(currentSelection() == identificacaoLivre::eid);
        return *static_cast<identificacaoLivre::pointer>(choice.get());
    }
    identificacaoLivre::reference select_identificacaoLivre ()
    {
        return *static_cast<identificacaoLivre::pointer>(setSelection(identificacaoLivre::eid, ASN1::AbstractData::create(&identificacaoLivre::value_type::theInfo)));
    }
    bool identificacaoLivre_isSelected () const { return currentSelection() == identificacaoLivre::eid; }

    IdentificadorEleitor(const IdentificadorEleitor & that) : Inherited(that) {}
    IdentificadorEleitor & operator=(const IdentificadorEleitor & that)
    { Inherited::operator=(that); return *this; }
    void swap(IdentificadorEleitor & that) { Inherited::swap(that); }
    IdentificadorEleitor * clone() const
    { return static_cast<IdentificadorEleitor*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const void* selectionInfos[3];
    static const char* selectionNames[3];
}; // end class IdentificadorEleitor

//
// ModuloTiposEleitorais::FormatoImagem
//

class FormatoImagem : public ASN1::ENUMERATED
{
    typedef ASN1::ENUMERATED Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    FormatoImagem(const void* info) : Inherited(info) {}
public:
    FormatoImagem() : Inherited(&theInfo) {}
    enum NamedNumber {
        unknownEnumeration_ = -1,
        formatoJPEG = 1,
        formatoBMP
    };

    bool is_formatoJPEG() const { return value == formatoJPEG; }
    void set_formatoJPEG() { value = formatoJPEG; }

    bool is_formatoBMP() const { return value == formatoBMP; }
    void set_formatoBMP() { value = formatoBMP; }

    FormatoImagem(const NamedNumber v) : Inherited(&theInfo) { setFromInt(v); }
    FormatoImagem & operator=(const NamedNumber v) { setFromInt(v);  return *this; }
    operator NamedNumber() const { return NamedNumber(asInt()); }

    bool operator == (NamedNumber rhs) const { return value == rhs; }
    bool operator != (NamedNumber rhs) const { return value != rhs; }
    bool operator <  (NamedNumber rhs) const { return value <  rhs; }
    bool operator >  (NamedNumber rhs) const { return value >  rhs; }
    bool operator <= (NamedNumber rhs) const { return value <= rhs; }
    bool operator >= (NamedNumber rhs) const { return value >= rhs; }
    bool operator == (const FormatoImagem &rhs) const { return value == rhs.value; }
    bool operator != (const FormatoImagem &rhs) const { return value != rhs.value; }

    void swap (FormatoImagem & that) { Inherited::swap(that); }
    FormatoImagem * clone() const
    { return static_cast<FormatoImagem*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const char * nameList[];
}; // end class FormatoImagem

//
// ModuloTiposEleitorais::IDEleitoral   // named by exact field/RTTI match
//

class IDEleitoral : public ASN1::CHOICE
{
    typedef ASN1::CHOICE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    IDEleitoral(const void* info) : Inherited(info) {}
public:
    IDEleitoral() : Inherited(&theInfo) {}
    enum Choice_Ids {
      unknownSelection_ = -2,
      unselected_ = -1,
      idProcessoEleitoral0 = 0,
      idPleito1 = 1,
      idEleicao2 = 2
    };

    class idProcessoEleitoral
    {
    public:
        enum Id { eid = 0 };
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 99999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class idProcessoEleitoral

    idProcessoEleitoral::const_reference get_idProcessoEleitoral () const
    {
        assert(currentSelection() == idProcessoEleitoral::eid);
        return *static_cast<idProcessoEleitoral::const_pointer>(choice.get());
    }
    idProcessoEleitoral::reference ref_idProcessoEleitoral ()
    {
        assert(currentSelection() == idProcessoEleitoral::eid);
        return *static_cast<idProcessoEleitoral::pointer>(choice.get());
    }
    idProcessoEleitoral::reference select_idProcessoEleitoral ()
    {
        return *static_cast<idProcessoEleitoral::pointer>(setSelection(idProcessoEleitoral::eid, ASN1::AbstractData::create(&idProcessoEleitoral::value_type::theInfo)));
    }
    bool idProcessoEleitoral_isSelected () const { return currentSelection() == idProcessoEleitoral::eid; }

    class idPleito
    {
    public:
        enum Id { eid = 1 };
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 99999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class idPleito

    idPleito::const_reference get_idPleito () const
    {
        assert(currentSelection() == idPleito::eid);
        return *static_cast<idPleito::const_pointer>(choice.get());
    }
    idPleito::reference ref_idPleito ()
    {
        assert(currentSelection() == idPleito::eid);
        return *static_cast<idPleito::pointer>(choice.get());
    }
    idPleito::reference select_idPleito ()
    {
        return *static_cast<idPleito::pointer>(setSelection(idPleito::eid, ASN1::AbstractData::create(&idPleito::value_type::theInfo)));
    }
    bool idPleito_isSelected () const { return currentSelection() == idPleito::eid; }

    class idEleicao
    {
    public:
        enum Id { eid = 2 };
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 99999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class idEleicao

    idEleicao::const_reference get_idEleicao () const
    {
        assert(currentSelection() == idEleicao::eid);
        return *static_cast<idEleicao::const_pointer>(choice.get());
    }
    idEleicao::reference ref_idEleicao ()
    {
        assert(currentSelection() == idEleicao::eid);
        return *static_cast<idEleicao::pointer>(choice.get());
    }
    idEleicao::reference select_idEleicao ()
    {
        return *static_cast<idEleicao::pointer>(setSelection(idEleicao::eid, ASN1::AbstractData::create(&idEleicao::value_type::theInfo)));
    }
    bool idEleicao_isSelected () const { return currentSelection() == idEleicao::eid; }

    IDEleitoral(const IDEleitoral & that) : Inherited(that) {}
    IDEleitoral & operator=(const IDEleitoral & that)
    { Inherited::operator=(that); return *this; }
    void swap(IDEleitoral & that) { Inherited::swap(that); }
    IDEleitoral * clone() const
    { return static_cast<IDEleitoral*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const void* selectionInfos[3];
    static unsigned selectionTags[3];
    static const char* selectionNames[3];
}; // end class IDEleitoral

//
// ModuloTiposEleitorais::Fase
//

class Fase : public ASN1::ENUMERATED
{
    typedef ASN1::ENUMERATED Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    Fase(const void* info) : Inherited(info) {}
public:
    Fase() : Inherited(&theInfo) {}
    enum NamedNumber {
        unknownEnumeration_ = -1,
        simulado = 1,
        oficial,
        treinamento
    };

    bool is_simulado() const { return value == simulado; }
    void set_simulado() { value = simulado; }

    bool is_oficial() const { return value == oficial; }
    void set_oficial() { value = oficial; }

    bool is_treinamento() const { return value == treinamento; }
    void set_treinamento() { value = treinamento; }

    Fase(const NamedNumber v) : Inherited(&theInfo) { setFromInt(v); }
    Fase & operator=(const NamedNumber v) { setFromInt(v);  return *this; }
    operator NamedNumber() const { return NamedNumber(asInt()); }

    bool operator == (NamedNumber rhs) const { return value == rhs; }
    bool operator != (NamedNumber rhs) const { return value != rhs; }
    bool operator <  (NamedNumber rhs) const { return value <  rhs; }
    bool operator >  (NamedNumber rhs) const { return value >  rhs; }
    bool operator <= (NamedNumber rhs) const { return value <= rhs; }
    bool operator >= (NamedNumber rhs) const { return value >= rhs; }
    bool operator == (const Fase &rhs) const { return value == rhs.value; }
    bool operator != (const Fase &rhs) const { return value != rhs.value; }

    void swap (Fase & that) { Inherited::swap(that); }
    Fase * clone() const
    { return static_cast<Fase*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const char * nameList[];
}; // end class Fase

//
// ModuloTiposEleitorais::Turno   // named by exact field/RTTI match
//

class Turno : public ASN1::ENUMERATED
{
    typedef ASN1::ENUMERATED Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    Turno(const void* info) : Inherited(info) {}
public:
    Turno() : Inherited(&theInfo) {}
    enum NamedNumber {
        unknownEnumeration_ = -1,
        semTurno,
        turno1,
        turno2
    };

    bool is_semTurno() const { return value == semTurno; }
    void set_semTurno() { value = semTurno; }

    bool is_turno1() const { return value == turno1; }
    void set_turno1() { value = turno1; }

    bool is_turno2() const { return value == turno2; }
    void set_turno2() { value = turno2; }

    Turno(const NamedNumber v) : Inherited(&theInfo) { setFromInt(v); }
    Turno & operator=(const NamedNumber v) { setFromInt(v);  return *this; }
    operator NamedNumber() const { return NamedNumber(asInt()); }

    bool operator == (NamedNumber rhs) const { return value == rhs; }
    bool operator != (NamedNumber rhs) const { return value != rhs; }
    bool operator <  (NamedNumber rhs) const { return value <  rhs; }
    bool operator >  (NamedNumber rhs) const { return value >  rhs; }
    bool operator <= (NamedNumber rhs) const { return value <= rhs; }
    bool operator >= (NamedNumber rhs) const { return value >= rhs; }
    bool operator == (const Turno &rhs) const { return value == rhs.value; }
    bool operator != (const Turno &rhs) const { return value != rhs.value; }

    void swap (Turno & that) { Inherited::swap(that); }
    Turno * clone() const
    { return static_cast<Turno*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const char * nameList[];
}; // end class Turno

//
// ModuloTiposEleitorais::IdentificacaoSecaoEleitoral
//

class IdentificacaoSecaoEleitoral : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    IdentificacaoSecaoEleitoral(const void* info) : Inherited(info) {}
public:
    IdentificacaoSecaoEleitoral() : Inherited(&theInfo) {}
    IdentificacaoSecaoEleitoral(const IdentificacaoSecaoEleitoral & that) : Inherited(that) {}

    IdentificacaoSecaoEleitoral& operator=(const IdentificacaoSecaoEleitoral& that)
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

    class local
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 9999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class local

    local::const_reference get_local () const { return *static_cast<local::const_pointer>(fields[1]); }
    local::reference ref_local () { return *static_cast<local::pointer>(fields[1]); }
    local::reference set_local () { return *static_cast<local::pointer>(fields[1]); }
    local::reference set_local (ASN1::INTEGER::int_type v)
    { return *static_cast<local::pointer>(fields[1]) = v; }

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
    void swap(IdentificacaoSecaoEleitoral& that) { Inherited::swap(that); }
    IdentificacaoSecaoEleitoral * clone() const
    { return static_cast<IdentificacaoSecaoEleitoral*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[3];
    static int fieldIds[3];
    static const char* fieldNames[3];
}; // end class IdentificacaoSecaoEleitoral

//
// ModuloTiposEleitorais::IdentificacaoContingencia
//

class IdentificacaoContingencia : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    IdentificacaoContingencia(const void* info) : Inherited(info) {}
public:
    IdentificacaoContingencia() : Inherited(&theInfo) {}
    IdentificacaoContingencia(const IdentificacaoContingencia & that) : Inherited(that) {}

    IdentificacaoContingencia& operator=(const IdentificacaoContingencia& that)
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
    void swap(IdentificacaoContingencia& that) { Inherited::swap(that); }
    IdentificacaoContingencia * clone() const
    { return static_cast<IdentificacaoContingencia*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[1];
    static int fieldIds[1];
    static const char* fieldNames[1];
}; // end class IdentificacaoContingencia

//
// ModuloTiposEleitorais::Tipo   // SYNTHESIZED name (field ['tipo', 'tipoCargo'])
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
        majoritario = 1,
        proporcional,
        consulta
    };

    bool is_majoritario() const { return value == majoritario; }
    void set_majoritario() { value = majoritario; }

    bool is_proporcional() const { return value == proporcional; }
    void set_proporcional() { value = proporcional; }

    bool is_consulta() const { return value == consulta; }
    void set_consulta() { value = consulta; }

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
// ModuloTiposEleitorais::TipoAbrangencia   // name from the app code / RTTI by field match
//

class TipoAbrangencia : public ASN1::ENUMERATED
{
    typedef ASN1::ENUMERATED Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    TipoAbrangencia(const void* info) : Inherited(info) {}
public:
    TipoAbrangencia() : Inherited(&theInfo) {}
    enum NamedNumber {
        unknownEnumeration_ = -1,
        municipal,
        estadual,
        federal
    };

    bool is_municipal() const { return value == municipal; }
    void set_municipal() { value = municipal; }

    bool is_estadual() const { return value == estadual; }
    void set_estadual() { value = estadual; }

    bool is_federal() const { return value == federal; }
    void set_federal() { value = federal; }

    TipoAbrangencia(const NamedNumber v) : Inherited(&theInfo) { setFromInt(v); }
    TipoAbrangencia & operator=(const NamedNumber v) { setFromInt(v);  return *this; }
    operator NamedNumber() const { return NamedNumber(asInt()); }

    bool operator == (NamedNumber rhs) const { return value == rhs; }
    bool operator != (NamedNumber rhs) const { return value != rhs; }
    bool operator <  (NamedNumber rhs) const { return value <  rhs; }
    bool operator >  (NamedNumber rhs) const { return value >  rhs; }
    bool operator <= (NamedNumber rhs) const { return value <= rhs; }
    bool operator >= (NamedNumber rhs) const { return value >= rhs; }
    bool operator == (const TipoAbrangencia &rhs) const { return value == rhs.value; }
    bool operator != (const TipoAbrangencia &rhs) const { return value != rhs.value; }

    void swap (TipoAbrangencia & that) { Inherited::swap(that); }
    TipoAbrangencia * clone() const
    { return static_cast<TipoAbrangencia*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const char * nameList[];
}; // end class TipoAbrangencia

//
// ModuloTiposEleitorais::IdentificacaoAbrangencia
//

class IdentificacaoAbrangencia : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    IdentificacaoAbrangencia(const void* info) : Inherited(info) {}
public:
    IdentificacaoAbrangencia() : Inherited(&theInfo) {}
    IdentificacaoAbrangencia(const IdentificacaoAbrangencia & that) : Inherited(that) {}

    IdentificacaoAbrangencia& operator=(const IdentificacaoAbrangencia& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_codigoMunicipio
    };

    class siglaUF
    {
    public:
        typedef ModuloTiposEleitorais::SiglaUF value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class siglaUF

    siglaUF::const_reference get_siglaUF () const { return *static_cast<siglaUF::const_pointer>(fields[0]); }
    siglaUF::reference ref_siglaUF () { return *static_cast<siglaUF::pointer>(fields[0]); }
    siglaUF::reference set_siglaUF () { return *static_cast<siglaUF::pointer>(fields[0]); }
    siglaUF::reference set_siglaUF (const ASN1_STD string& v)
    { return *static_cast<siglaUF::pointer>(fields[0]) = v; }

    class codigoMunicipio
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 99999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class codigoMunicipio

    codigoMunicipio::const_reference get_codigoMunicipio () const
    {
      assert(hasOptionalField(e_codigoMunicipio));
      return *static_cast<codigoMunicipio::const_pointer>(fields[1]);
    }
    codigoMunicipio::reference ref_codigoMunicipio ()
    {
      assert(hasOptionalField(e_codigoMunicipio));
      return *static_cast<codigoMunicipio::pointer>(fields[1]);
    }
    codigoMunicipio::reference set_codigoMunicipio ()
    {
      includeOptionalField( e_codigoMunicipio, 1);
      return *static_cast<codigoMunicipio::pointer>(fields[1]);
    }
    codigoMunicipio::reference set_codigoMunicipio (ASN1::INTEGER::int_type v)
    {
      includeOptionalField( e_codigoMunicipio, 1);
      return *static_cast<codigoMunicipio::pointer>(fields[1]) = v;
    }
    void omit_codigoMunicipio () { removeOptionalField( e_codigoMunicipio); }
    bool codigoMunicipio_isPresent () const { return hasOptionalField( e_codigoMunicipio); }
    void swap(IdentificacaoAbrangencia& that) { Inherited::swap(that); }
    IdentificacaoAbrangencia * clone() const
    { return static_cast<IdentificacaoAbrangencia*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[2];
    static int fieldIds[2];
    static const char* fieldNames[2];
}; // end class IdentificacaoAbrangencia

//
// ModuloTiposEleitorais::Abrangencia
//

class Abrangencia : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    Abrangencia(const void* info) : Inherited(info) {}
public:
    Abrangencia() : Inherited(&theInfo) {}
    Abrangencia(const Abrangencia & that) : Inherited(that) {}

    Abrangencia& operator=(const Abrangencia& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_id
    };

    class tipo
    {
    public:
        typedef ModuloTiposEleitorais::TipoAbrangencia value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class tipo

    tipo::const_reference get_tipo () const { return *static_cast<tipo::const_pointer>(fields[0]); }
    tipo::reference ref_tipo () { return *static_cast<tipo::pointer>(fields[0]); }
    tipo::reference set_tipo () { return *static_cast<tipo::pointer>(fields[0]); }
    tipo::reference set_tipo (ModuloTiposEleitorais::TipoAbrangencia::NamedNumber v)
    { return *static_cast<tipo::pointer>(fields[0]) = v; }

    class id
    {
    public:
        typedef ModuloTiposEleitorais::IdentificacaoAbrangencia value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class id

    id::const_reference get_id () const
    {
      assert(hasOptionalField(e_id));
      return *static_cast<id::const_pointer>(fields[1]);
    }
    id::reference ref_id ()
    {
      assert(hasOptionalField(e_id));
      return *static_cast<id::pointer>(fields[1]);
    }
    id::reference set_id ()
    {
      includeOptionalField( e_id, 1);
      return *static_cast<id::pointer>(fields[1]);
    }
    void omit_id () { removeOptionalField( e_id); }
    bool id_isPresent () const { return hasOptionalField( e_id); }
    void swap(Abrangencia& that) { Inherited::swap(that); }
    Abrangencia * clone() const
    { return static_cast<Abrangencia*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[2];
    static int fieldIds[2];
    static const char* fieldNames[2];
}; // end class Abrangencia

//
// ModuloTiposEleitorais::TipoDedo
//

class TipoDedo : public ASN1::ENUMERATED
{
    typedef ASN1::ENUMERATED Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    TipoDedo(const void* info) : Inherited(info) {}
public:
    TipoDedo() : Inherited(&theInfo) {}
    enum NamedNumber {
        unknownEnumeration_ = -1,
        naoIdentificado,
        polegarDireito,
        indicadorDireito,
        medioDireito,
        anelarDireito,
        minimoDireito,
        polegarEsquerdo,
        indicadorEsquerdo,
        medioEsquerdo,
        anelarEsquerdo,
        minimoEsquerdo
    };

    bool is_naoIdentificado() const { return value == naoIdentificado; }
    void set_naoIdentificado() { value = naoIdentificado; }

    bool is_polegarDireito() const { return value == polegarDireito; }
    void set_polegarDireito() { value = polegarDireito; }

    bool is_indicadorDireito() const { return value == indicadorDireito; }
    void set_indicadorDireito() { value = indicadorDireito; }

    bool is_medioDireito() const { return value == medioDireito; }
    void set_medioDireito() { value = medioDireito; }

    bool is_anelarDireito() const { return value == anelarDireito; }
    void set_anelarDireito() { value = anelarDireito; }

    bool is_minimoDireito() const { return value == minimoDireito; }
    void set_minimoDireito() { value = minimoDireito; }

    bool is_polegarEsquerdo() const { return value == polegarEsquerdo; }
    void set_polegarEsquerdo() { value = polegarEsquerdo; }

    bool is_indicadorEsquerdo() const { return value == indicadorEsquerdo; }
    void set_indicadorEsquerdo() { value = indicadorEsquerdo; }

    bool is_medioEsquerdo() const { return value == medioEsquerdo; }
    void set_medioEsquerdo() { value = medioEsquerdo; }

    bool is_anelarEsquerdo() const { return value == anelarEsquerdo; }
    void set_anelarEsquerdo() { value = anelarEsquerdo; }

    bool is_minimoEsquerdo() const { return value == minimoEsquerdo; }
    void set_minimoEsquerdo() { value = minimoEsquerdo; }

    TipoDedo(const NamedNumber v) : Inherited(&theInfo) { setFromInt(v); }
    TipoDedo & operator=(const NamedNumber v) { setFromInt(v);  return *this; }
    operator NamedNumber() const { return NamedNumber(asInt()); }

    bool operator == (NamedNumber rhs) const { return value == rhs; }
    bool operator != (NamedNumber rhs) const { return value != rhs; }
    bool operator <  (NamedNumber rhs) const { return value <  rhs; }
    bool operator >  (NamedNumber rhs) const { return value >  rhs; }
    bool operator <= (NamedNumber rhs) const { return value <= rhs; }
    bool operator >= (NamedNumber rhs) const { return value >= rhs; }
    bool operator == (const TipoDedo &rhs) const { return value == rhs.value; }
    bool operator != (const TipoDedo &rhs) const { return value != rhs.value; }

    void swap (TipoDedo & that) { Inherited::swap(that); }
    TipoDedo * clone() const
    { return static_cast<TipoDedo*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const char * nameList[];
}; // end class TipoDedo

//
// ModuloTiposEleitorais::ArquivoAssinatura
//

class ArquivoAssinatura : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    ArquivoAssinatura(const void* info) : Inherited(info) {}
public:
    ArquivoAssinatura() : Inherited(&theInfo) {}
    ArquivoAssinatura(const ArquivoAssinatura & that) : Inherited(that) {}

    ArquivoAssinatura& operator=(const ArquivoAssinatura& that)
    { Inherited::operator=(that); return *this; }


    class nomeArquivo
    {
    public:
        typedef ASN1::GeneralString value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class nomeArquivo

    nomeArquivo::const_reference get_nomeArquivo () const { return *static_cast<nomeArquivo::const_pointer>(fields[0]); }
    nomeArquivo::reference ref_nomeArquivo () { return *static_cast<nomeArquivo::pointer>(fields[0]); }
    nomeArquivo::reference set_nomeArquivo () { return *static_cast<nomeArquivo::pointer>(fields[0]); }
    nomeArquivo::reference set_nomeArquivo (const ASN1_STD string& v)
    { return *static_cast<nomeArquivo::pointer>(fields[0]) = v; }

    class assinatura
    {
    public:
        typedef ASN1::GeneralString value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class assinatura

    assinatura::const_reference get_assinatura () const { return *static_cast<assinatura::const_pointer>(fields[1]); }
    assinatura::reference ref_assinatura () { return *static_cast<assinatura::pointer>(fields[1]); }
    assinatura::reference set_assinatura () { return *static_cast<assinatura::pointer>(fields[1]); }
    assinatura::reference set_assinatura (const ASN1_STD string& v)
    { return *static_cast<assinatura::pointer>(fields[1]) = v; }
    void swap(ArquivoAssinatura& that) { Inherited::swap(that); }
    ArquivoAssinatura * clone() const
    { return static_cast<ArquivoAssinatura*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[2];
    static int fieldIds[2];
    static const char* fieldNames[2];
}; // end class ArquivoAssinatura

//
// ModuloTiposEleitorais::Seguranca
//

class Seguranca : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    Seguranca(const void* info) : Inherited(info) {}
public:
    Seguranca() : Inherited(&theInfo) {}
    Seguranca(const Seguranca & that) : Inherited(that) {}

    Seguranca& operator=(const Seguranca& that)
    { Inherited::operator=(that); return *this; }


    class idTipoArquivo
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 2> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class idTipoArquivo

    idTipoArquivo::const_reference get_idTipoArquivo () const { return *static_cast<idTipoArquivo::const_pointer>(fields[0]); }
    idTipoArquivo::reference ref_idTipoArquivo () { return *static_cast<idTipoArquivo::pointer>(fields[0]); }
    idTipoArquivo::reference set_idTipoArquivo () { return *static_cast<idTipoArquivo::pointer>(fields[0]); }
    idTipoArquivo::reference set_idTipoArquivo (ASN1::INTEGER::int_type v)
    { return *static_cast<idTipoArquivo::pointer>(fields[0]) = v; }

    class idCriptografia
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 3> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class idCriptografia

    idCriptografia::const_reference get_idCriptografia () const { return *static_cast<idCriptografia::const_pointer>(fields[1]); }
    idCriptografia::reference ref_idCriptografia () { return *static_cast<idCriptografia::pointer>(fields[1]); }
    idCriptografia::reference set_idCriptografia () { return *static_cast<idCriptografia::pointer>(fields[1]); }
    idCriptografia::reference set_idCriptografia (ASN1::INTEGER::int_type v)
    { return *static_cast<idCriptografia::pointer>(fields[1]) = v; }

    class idArquivoCD
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 255> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class idArquivoCD

    idArquivoCD::const_reference get_idArquivoCD () const { return *static_cast<idArquivoCD::const_pointer>(fields[2]); }
    idArquivoCD::reference ref_idArquivoCD () { return *static_cast<idArquivoCD::pointer>(fields[2]); }
    idArquivoCD::reference set_idArquivoCD () { return *static_cast<idArquivoCD::pointer>(fields[2]); }
    idArquivoCD::reference set_idArquivoCD (ASN1::INTEGER::int_type v)
    { return *static_cast<idArquivoCD::pointer>(fields[2]) = v; }

    class idArquivoChave
    {
    public:
        typedef ASN1::OCTET_STRING value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class idArquivoChave

    idArquivoChave::const_reference get_idArquivoChave () const { return *static_cast<idArquivoChave::const_pointer>(fields[3]); }
    idArquivoChave::reference ref_idArquivoChave () { return *static_cast<idArquivoChave::pointer>(fields[3]); }
    idArquivoChave::reference set_idArquivoChave () { return *static_cast<idArquivoChave::pointer>(fields[3]); }
    idArquivoChave::reference set_idArquivoChave (const ASN1::OCTET_STRING& v)
    { return *static_cast<idArquivoChave::pointer>(fields[3]) = v; }
    void swap(Seguranca& that) { Inherited::swap(that); }
    Seguranca * clone() const
    { return static_cast<Seguranca*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[4];
    static int fieldIds[4];
    static const char* fieldNames[4];
}; // end class Seguranca

//
// ModuloTiposEleitorais::CodigoCargoConsulta
//

class CodigoCargoConsulta : public ASN1::CHOICE
{
    typedef ASN1::CHOICE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    CodigoCargoConsulta(const void* info) : Inherited(info) {}
public:
    CodigoCargoConsulta() : Inherited(&theInfo) {}
    enum Choice_Ids {
      unknownSelection_ = -2,
      unselected_ = -1,
      cargoConstitucional0 = 0,
      numeroCargoConsultaLivre1 = 1
    };

    class cargoConstitucional
    {
    public:
        enum Id { eid = 0 };
        //
        // ModuloTiposEleitorais::CodigoCargoConsulta::cargoConstitucional::value_type
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
                presidente = 1,
                vicePresidente,
                governador,
                viceGovernador,
                senador,
                deputadoFederal,
                deputadoEstadual,
                deputadoDistrital,
                primeiroSuplenteSenador,
                segundoSuplenteSenador,
                prefeito,
                vicePrefeito,
                vereador
            };

            bool is_presidente() const { return value == presidente; }
            void set_presidente() { value = presidente; }

            bool is_vicePresidente() const { return value == vicePresidente; }
            void set_vicePresidente() { value = vicePresidente; }

            bool is_governador() const { return value == governador; }
            void set_governador() { value = governador; }

            bool is_viceGovernador() const { return value == viceGovernador; }
            void set_viceGovernador() { value = viceGovernador; }

            bool is_senador() const { return value == senador; }
            void set_senador() { value = senador; }

            bool is_deputadoFederal() const { return value == deputadoFederal; }
            void set_deputadoFederal() { value = deputadoFederal; }

            bool is_deputadoEstadual() const { return value == deputadoEstadual; }
            void set_deputadoEstadual() { value = deputadoEstadual; }

            bool is_deputadoDistrital() const { return value == deputadoDistrital; }
            void set_deputadoDistrital() { value = deputadoDistrital; }

            bool is_primeiroSuplenteSenador() const { return value == primeiroSuplenteSenador; }
            void set_primeiroSuplenteSenador() { value = primeiroSuplenteSenador; }

            bool is_segundoSuplenteSenador() const { return value == segundoSuplenteSenador; }
            void set_segundoSuplenteSenador() { value = segundoSuplenteSenador; }

            bool is_prefeito() const { return value == prefeito; }
            void set_prefeito() { value = prefeito; }

            bool is_vicePrefeito() const { return value == vicePrefeito; }
            void set_vicePrefeito() { value = vicePrefeito; }

            bool is_vereador() const { return value == vereador; }
            void set_vereador() { value = vereador; }

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
    }; // end class cargoConstitucional

    cargoConstitucional::const_reference get_cargoConstitucional () const
    {
        assert(currentSelection() == cargoConstitucional::eid);
        return *static_cast<cargoConstitucional::const_pointer>(choice.get());
    }
    cargoConstitucional::reference ref_cargoConstitucional ()
    {
        assert(currentSelection() == cargoConstitucional::eid);
        return *static_cast<cargoConstitucional::pointer>(choice.get());
    }
    cargoConstitucional::reference select_cargoConstitucional ()
    {
        return *static_cast<cargoConstitucional::pointer>(setSelection(cargoConstitucional::eid, ASN1::AbstractData::create(&cargoConstitucional::value_type::theInfo)));
    }
    bool cargoConstitucional_isSelected () const { return currentSelection() == cargoConstitucional::eid; }

    class numeroCargoConsultaLivre
    {
    public:
        enum Id { eid = 1 };
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 25, 99> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class numeroCargoConsultaLivre

    numeroCargoConsultaLivre::const_reference get_numeroCargoConsultaLivre () const
    {
        assert(currentSelection() == numeroCargoConsultaLivre::eid);
        return *static_cast<numeroCargoConsultaLivre::const_pointer>(choice.get());
    }
    numeroCargoConsultaLivre::reference ref_numeroCargoConsultaLivre ()
    {
        assert(currentSelection() == numeroCargoConsultaLivre::eid);
        return *static_cast<numeroCargoConsultaLivre::pointer>(choice.get());
    }
    numeroCargoConsultaLivre::reference select_numeroCargoConsultaLivre ()
    {
        return *static_cast<numeroCargoConsultaLivre::pointer>(setSelection(numeroCargoConsultaLivre::eid, ASN1::AbstractData::create(&numeroCargoConsultaLivre::value_type::theInfo)));
    }
    bool numeroCargoConsultaLivre_isSelected () const { return currentSelection() == numeroCargoConsultaLivre::eid; }

    CodigoCargoConsulta(const CodigoCargoConsulta & that) : Inherited(that) {}
    CodigoCargoConsulta & operator=(const CodigoCargoConsulta & that)
    { Inherited::operator=(that); return *this; }
    void swap(CodigoCargoConsulta & that) { Inherited::swap(that); }
    CodigoCargoConsulta * clone() const
    { return static_cast<CodigoCargoConsulta*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const void* selectionInfos[2];
    static unsigned selectionTags[2];
    static const char* selectionNames[2];
}; // end class CodigoCargoConsulta

//
// ModuloTiposEleitorais::Foto
//

class Foto : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    Foto(const void* info) : Inherited(info) {}
public:
    Foto() : Inherited(&theInfo) {}
    Foto(const Foto & that) : Inherited(that) {}

    Foto& operator=(const Foto& that)
    { Inherited::operator=(that); return *this; }


    class formato
    {
    public:
        typedef ModuloTiposEleitorais::FormatoImagem value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class formato

    formato::const_reference get_formato () const { return *static_cast<formato::const_pointer>(fields[0]); }
    formato::reference ref_formato () { return *static_cast<formato::pointer>(fields[0]); }
    formato::reference set_formato () { return *static_cast<formato::pointer>(fields[0]); }
    formato::reference set_formato (ModuloTiposEleitorais::FormatoImagem::NamedNumber v)
    { return *static_cast<formato::pointer>(fields[0]) = v; }

    class imagem
    {
    public:
        typedef ASN1::OCTET_STRING value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class imagem

    imagem::const_reference get_imagem () const { return *static_cast<imagem::const_pointer>(fields[1]); }
    imagem::reference ref_imagem () { return *static_cast<imagem::pointer>(fields[1]); }
    imagem::reference set_imagem () { return *static_cast<imagem::pointer>(fields[1]); }
    imagem::reference set_imagem (const ASN1::OCTET_STRING& v)
    { return *static_cast<imagem::pointer>(fields[1]) = v; }
    void swap(Foto& that) { Inherited::swap(that); }
    Foto * clone() const
    { return static_cast<Foto*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[2];
    static int fieldIds[2];
    static const char* fieldNames[2];
}; // end class Foto

//
// ModuloTiposEleitorais::DataGeracao   // SYNTHESIZED name (field ['dataGeracao', 'dataHoraCarga', 'dataHoraAbertura', 'dataHoraEncerramento', 'dataHoraDesligamentoVotoImpresso', 'dataHoraLigamento', 'dataHoraEmissao', 'data', 'dataInicial', 'dataFinal', 'dhIniAquisicao', 'dhFimAquisicao', 'dhUltimoVoto', 'dhEmissao', 'emissaoZeresima', 'inicioVotacao', 'encerramentoVotacao', 'terminoVotacao', 'dataHoraColeta'])
//

class DataGeracao : public ASN1::GeneralString
{
    typedef ASN1::GeneralString Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    DataGeracao(const void* info) : Inherited(info) {}
public:
    DataGeracao() : Inherited(&theInfo) {}
    DataGeracao(const base_string& str, const void* info = &theInfo) : Inherited(str, info) {}
    DataGeracao(const char* str, const void* info = &theInfo) : Inherited(str, info) {}
    DataGeracao(const DataGeracao& that) : Inherited(that) {}
    DataGeracao& operator=(const ASN1_STD string& that)
    { Inherited::operator=(that); return *this; }
    DataGeracao& operator=(const char* that)
    { Inherited::operator=(that); return *this; }
    DataGeracao * clone() const
    { return static_cast<DataGeracao*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
}; // end class DataGeracao

//
// ModuloTiposEleitorais::DataHoraJE   // name from the target's RTTI (vtable 1556172); the converters build it from a
// std::string through a temporary (no operator=(string)): CConversorComparecimentoMesario::DoConverte, fn 9110
//

class DataHoraJE : public ASN1::GeneralString
{
    typedef ASN1::GeneralString Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    DataHoraJE(const void* info) : Inherited(info) {}
public:
    DataHoraJE() : Inherited(&theInfo) {}
    DataHoraJE(const base_string& str, const void* info = &theInfo) : Inherited(str, info) {}
    DataHoraJE(const char* str, const void* info = &theInfo) : Inherited(str, info) {}
    DataHoraJE(const DataHoraJE& that) : Inherited(that) {}
    DataHoraJE * clone() const
    { return static_cast<DataHoraJE*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
}; // end class DataHoraJE

//
// ModuloTiposEleitorais::CabecalhoEntidade
//

class CabecalhoEntidade : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    CabecalhoEntidade(const void* info) : Inherited(info) {}
public:
    CabecalhoEntidade() : Inherited(&theInfo) {}
    CabecalhoEntidade(const CabecalhoEntidade & that) : Inherited(that) {}

    CabecalhoEntidade& operator=(const CabecalhoEntidade& that)
    { Inherited::operator=(that); return *this; }


    class dataGeracao
    {
    public:
        typedef ModuloTiposEleitorais::DataGeracao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class dataGeracao

    dataGeracao::const_reference get_dataGeracao () const { return *static_cast<dataGeracao::const_pointer>(fields[0]); }
    dataGeracao::reference ref_dataGeracao () { return *static_cast<dataGeracao::pointer>(fields[0]); }
    dataGeracao::reference set_dataGeracao () { return *static_cast<dataGeracao::pointer>(fields[0]); }
    dataGeracao::reference set_dataGeracao (const ASN1_STD string& v)
    { return *static_cast<dataGeracao::pointer>(fields[0]) = v; }

    class idEleitoral
    {
    public:
        typedef ModuloTiposEleitorais::IDEleitoral value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class idEleitoral

    idEleitoral::const_reference get_idEleitoral () const { return *static_cast<idEleitoral::const_pointer>(fields[1]); }
    idEleitoral::reference ref_idEleitoral () { return *static_cast<idEleitoral::pointer>(fields[1]); }
    idEleitoral::reference set_idEleitoral () { return *static_cast<idEleitoral::pointer>(fields[1]); }
    void swap(CabecalhoEntidade& that) { Inherited::swap(that); }
    CabecalhoEntidade * clone() const
    { return static_cast<CabecalhoEntidade*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[2];
    static int fieldIds[2];
    static const char* fieldNames[2];
}; // end class CabecalhoEntidade

//
// ModuloTiposEleitorais::IDPacote
//

class IDPacote : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    IDPacote(const void* info) : Inherited(info) {}
public:
    IDPacote() : Inherited(&theInfo) {}
    IDPacote(const IDPacote & that) : Inherited(that) {}

    IDPacote& operator=(const IDPacote& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_siglaUF,
      e_codigoMunicipio,
      e_numeroZona
    };

    class idPacoteEleitoral
    {
    public:
        typedef ModuloTiposEleitorais::IDEleitoral value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class idPacoteEleitoral

    idPacoteEleitoral::const_reference get_idPacoteEleitoral () const { return *static_cast<idPacoteEleitoral::const_pointer>(fields[0]); }
    idPacoteEleitoral::reference ref_idPacoteEleitoral () { return *static_cast<idPacoteEleitoral::pointer>(fields[0]); }
    idPacoteEleitoral::reference set_idPacoteEleitoral () { return *static_cast<idPacoteEleitoral::pointer>(fields[0]); }

    class fase
    {
    public:
        typedef ModuloTiposEleitorais::Fase value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class fase

    fase::const_reference get_fase () const { return *static_cast<fase::const_pointer>(fields[1]); }
    fase::reference ref_fase () { return *static_cast<fase::pointer>(fields[1]); }
    fase::reference set_fase () { return *static_cast<fase::pointer>(fields[1]); }
    fase::reference set_fase (ModuloTiposEleitorais::Fase::NamedNumber v)
    { return *static_cast<fase::pointer>(fields[1]) = v; }

    class siglaUF
    {
    public:
        typedef ModuloTiposEleitorais::SiglaUF value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class siglaUF

    siglaUF::const_reference get_siglaUF () const
    {
      assert(hasOptionalField(e_siglaUF));
      return *static_cast<siglaUF::const_pointer>(fields[2]);
    }
    siglaUF::reference ref_siglaUF ()
    {
      assert(hasOptionalField(e_siglaUF));
      return *static_cast<siglaUF::pointer>(fields[2]);
    }
    siglaUF::reference set_siglaUF ()
    {
      includeOptionalField( e_siglaUF, 2);
      return *static_cast<siglaUF::pointer>(fields[2]);
    }
    siglaUF::reference set_siglaUF (const ASN1_STD string& v)
    {
      includeOptionalField( e_siglaUF, 2);
      return *static_cast<siglaUF::pointer>(fields[2]) = v;
    }
    void omit_siglaUF () { removeOptionalField( e_siglaUF); }
    bool siglaUF_isPresent () const { return hasOptionalField( e_siglaUF); }

    class codigoMunicipio
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 99999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class codigoMunicipio

    codigoMunicipio::const_reference get_codigoMunicipio () const
    {
      assert(hasOptionalField(e_codigoMunicipio));
      return *static_cast<codigoMunicipio::const_pointer>(fields[3]);
    }
    codigoMunicipio::reference ref_codigoMunicipio ()
    {
      assert(hasOptionalField(e_codigoMunicipio));
      return *static_cast<codigoMunicipio::pointer>(fields[3]);
    }
    codigoMunicipio::reference set_codigoMunicipio ()
    {
      includeOptionalField( e_codigoMunicipio, 3);
      return *static_cast<codigoMunicipio::pointer>(fields[3]);
    }
    codigoMunicipio::reference set_codigoMunicipio (ASN1::INTEGER::int_type v)
    {
      includeOptionalField( e_codigoMunicipio, 3);
      return *static_cast<codigoMunicipio::pointer>(fields[3]) = v;
    }
    void omit_codigoMunicipio () { removeOptionalField( e_codigoMunicipio); }
    bool codigoMunicipio_isPresent () const { return hasOptionalField( e_codigoMunicipio); }

    class numeroZona
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 9999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class numeroZona

    numeroZona::const_reference get_numeroZona () const
    {
      assert(hasOptionalField(e_numeroZona));
      return *static_cast<numeroZona::const_pointer>(fields[4]);
    }
    numeroZona::reference ref_numeroZona ()
    {
      assert(hasOptionalField(e_numeroZona));
      return *static_cast<numeroZona::pointer>(fields[4]);
    }
    numeroZona::reference set_numeroZona ()
    {
      includeOptionalField( e_numeroZona, 4);
      return *static_cast<numeroZona::pointer>(fields[4]);
    }
    numeroZona::reference set_numeroZona (ASN1::INTEGER::int_type v)
    {
      includeOptionalField( e_numeroZona, 4);
      return *static_cast<numeroZona::pointer>(fields[4]) = v;
    }
    void omit_numeroZona () { removeOptionalField( e_numeroZona); }
    bool numeroZona_isPresent () const { return hasOptionalField( e_numeroZona); }
    void swap(IDPacote& that) { Inherited::swap(that); }
    IDPacote * clone() const
    { return static_cast<IDPacote*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[5];
    static int fieldIds[5];
    static unsigned fieldTags[5];
    static const char* fieldNames[5];
}; // end class IDPacote

//
// ModuloTiposEleitorais::Versao   // SYNTHESIZED name (field ['versao', 'tagChaves'])
//

class Versao : public ASN1::NumericString
{
    typedef ASN1::NumericString Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    Versao(const void* info) : Inherited(info) {}
public:
    Versao() : Inherited(&theInfo) {}
    Versao(const base_string& str, const void* info = &theInfo) : Inherited(str, info) {}
    Versao(const char* str, const void* info = &theInfo) : Inherited(str, info) {}
    Versao(const Versao& that) : Inherited(that) {}
    Versao& operator=(const ASN1_STD string& that)
    { Inherited::operator=(that); return *this; }
    Versao& operator=(const char* that)
    { Inherited::operator=(that); return *this; }
    Versao * clone() const
    { return static_cast<Versao*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
}; // end class Versao

//
// ModuloTiposEleitorais::CabecalhoPacote
//

class CabecalhoPacote : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    CabecalhoPacote(const void* info) : Inherited(info) {}
public:
    CabecalhoPacote() : Inherited(&theInfo) {}
    CabecalhoPacote(const CabecalhoPacote & that) : Inherited(that) {}

    CabecalhoPacote& operator=(const CabecalhoPacote& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_abrangencia
    };

    class tipoPacote
    {
    public:
        typedef ModuloTiposPacotes::TipoPacote value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class tipoPacote

    tipoPacote::const_reference get_tipoPacote () const { return *static_cast<tipoPacote::const_pointer>(fields[0]); }
    tipoPacote::reference ref_tipoPacote () { return *static_cast<tipoPacote::pointer>(fields[0]); }
    tipoPacote::reference set_tipoPacote () { return *static_cast<tipoPacote::pointer>(fields[0]); }
    tipoPacote::reference set_tipoPacote (ModuloTiposPacotes::TipoPacote::NamedNumber v)
    { return *static_cast<tipoPacote::pointer>(fields[0]) = v; }

    class idPacote
    {
    public:
        typedef ModuloTiposEleitorais::IDPacote value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class idPacote

    idPacote::const_reference get_idPacote () const { return *static_cast<idPacote::const_pointer>(fields[1]); }
    idPacote::reference ref_idPacote () { return *static_cast<idPacote::pointer>(fields[1]); }
    idPacote::reference set_idPacote () { return *static_cast<idPacote::pointer>(fields[1]); }

    class nomepacote
    {
    public:
        typedef ASN1::GeneralString value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class nomepacote

    nomepacote::const_reference get_nomepacote () const { return *static_cast<nomepacote::const_pointer>(fields[2]); }
    nomepacote::reference ref_nomepacote () { return *static_cast<nomepacote::pointer>(fields[2]); }
    nomepacote::reference set_nomepacote () { return *static_cast<nomepacote::pointer>(fields[2]); }
    nomepacote::reference set_nomepacote (const ASN1_STD string& v)
    { return *static_cast<nomepacote::pointer>(fields[2]) = v; }

    class versao
    {
    public:
        typedef ModuloTiposEleitorais::Versao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class versao

    versao::const_reference get_versao () const { return *static_cast<versao::const_pointer>(fields[3]); }
    versao::reference ref_versao () { return *static_cast<versao::pointer>(fields[3]); }
    versao::reference set_versao () { return *static_cast<versao::pointer>(fields[3]); }
    versao::reference set_versao (const ASN1_STD string& v)
    { return *static_cast<versao::pointer>(fields[3]) = v; }

    class abrangencia
    {
    public:
        typedef ModuloTiposEleitorais::Abrangencia value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class abrangencia

    abrangencia::const_reference get_abrangencia () const
    {
      assert(hasOptionalField(e_abrangencia));
      return *static_cast<abrangencia::const_pointer>(fields[4]);
    }
    abrangencia::reference ref_abrangencia ()
    {
      assert(hasOptionalField(e_abrangencia));
      return *static_cast<abrangencia::pointer>(fields[4]);
    }
    abrangencia::reference set_abrangencia ()
    {
      includeOptionalField( e_abrangencia, 4);
      return *static_cast<abrangencia::pointer>(fields[4]);
    }
    void omit_abrangencia () { removeOptionalField( e_abrangencia); }
    bool abrangencia_isPresent () const { return hasOptionalField( e_abrangencia); }

    class origem
    {
    public:
        //
        // ModuloTiposEleitorais::CabecalhoPacote::origem::value_type
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
                configurador = 1,
                intercad,
                candidaturas,
                gedai = 6,
                urnaEletronica,
                simulador,
                parametrizador,
                geradorDeBases,
                sistot,
                seweb,
                simon,
                padaUE
            };

            bool is_configurador() const { return value == configurador; }
            void set_configurador() { value = configurador; }

            bool is_intercad() const { return value == intercad; }
            void set_intercad() { value = intercad; }

            bool is_candidaturas() const { return value == candidaturas; }
            void set_candidaturas() { value = candidaturas; }

            bool is_gedai() const { return value == gedai; }
            void set_gedai() { value = gedai; }

            bool is_urnaEletronica() const { return value == urnaEletronica; }
            void set_urnaEletronica() { value = urnaEletronica; }

            bool is_simulador() const { return value == simulador; }
            void set_simulador() { value = simulador; }

            bool is_parametrizador() const { return value == parametrizador; }
            void set_parametrizador() { value = parametrizador; }

            bool is_geradorDeBases() const { return value == geradorDeBases; }
            void set_geradorDeBases() { value = geradorDeBases; }

            bool is_sistot() const { return value == sistot; }
            void set_sistot() { value = sistot; }

            bool is_seweb() const { return value == seweb; }
            void set_seweb() { value = seweb; }

            bool is_simon() const { return value == simon; }
            void set_simon() { value = simon; }

            bool is_padaUE() const { return value == padaUE; }
            void set_padaUE() { value = padaUE; }

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
    }; // end class origem

    origem::const_reference get_origem () const { return *static_cast<origem::const_pointer>(fields[5]); }
    origem::reference ref_origem () { return *static_cast<origem::pointer>(fields[5]); }
    origem::reference set_origem () { return *static_cast<origem::pointer>(fields[5]); }
    origem::reference set_origem (origem::value_type::NamedNumber v)
    { return *static_cast<origem::pointer>(fields[5]) = v; }
    void swap(CabecalhoPacote& that) { Inherited::swap(that); }
    CabecalhoPacote * clone() const
    { return static_cast<CabecalhoPacote*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[6];
    static int fieldIds[6];
    static unsigned fieldTags[6];
    static const char* fieldNames[6];
}; // end class CabecalhoPacote

} // namespace ModuloTiposEleitorais

#endif // __MODULOTIPOSELEITORAIS_H
