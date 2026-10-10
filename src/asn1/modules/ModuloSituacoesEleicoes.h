#ifndef __MODULOSITUACOESELEICOES_H
#define __MODULOSITUACOESELEICOES_H

#include "asn1.h"

#include "ModuloTiposEleitorais.h"

namespace ModuloSituacoesEleicoes {

//
// ModuloSituacoesEleicoes::SituacaoEleicao
//

class SituacaoEleicao : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    SituacaoEleicao(const void* info) : Inherited(info) {}
public:
    SituacaoEleicao() : Inherited(&theInfo) {}
    SituacaoEleicao(const SituacaoEleicao & that) : Inherited(that) {}

    SituacaoEleicao& operator=(const SituacaoEleicao& that)
    { Inherited::operator=(that); return *this; }


    class idEleicao
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 99999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class idEleicao

    idEleicao::const_reference get_idEleicao () const { return *static_cast<idEleicao::const_pointer>(fields[0]); }
    idEleicao::reference ref_idEleicao () { return *static_cast<idEleicao::pointer>(fields[0]); }
    idEleicao::reference set_idEleicao () { return *static_cast<idEleicao::pointer>(fields[0]); }
    idEleicao::reference set_idEleicao (ASN1::INTEGER::int_type v)
    { return *static_cast<idEleicao::pointer>(fields[0]) = v; }

    class tipo
    {
    public:
        //
        // ModuloSituacoesEleicoes::SituacaoEleicao::tipo::value_type
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
                ativa,
                suspensa,
                cancelada,
                excluida
            };

            bool is_ativa() const { return value == ativa; }
            void set_ativa() { value = ativa; }

            bool is_suspensa() const { return value == suspensa; }
            void set_suspensa() { value = suspensa; }

            bool is_cancelada() const { return value == cancelada; }
            void set_cancelada() { value = cancelada; }

            bool is_excluida() const { return value == excluida; }
            void set_excluida() { value = excluida; }

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
    }; // end class tipo

    tipo::const_reference get_tipo () const { return *static_cast<tipo::const_pointer>(fields[1]); }
    tipo::reference ref_tipo () { return *static_cast<tipo::pointer>(fields[1]); }
    tipo::reference set_tipo () { return *static_cast<tipo::pointer>(fields[1]); }
    tipo::reference set_tipo (tipo::value_type::NamedNumber v)
    { return *static_cast<tipo::pointer>(fields[1]) = v; }

    class ordemAquisicao
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 99> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class ordemAquisicao

    ordemAquisicao::const_reference get_ordemAquisicao () const { return *static_cast<ordemAquisicao::const_pointer>(fields[2]); }
    ordemAquisicao::reference ref_ordemAquisicao () { return *static_cast<ordemAquisicao::pointer>(fields[2]); }
    ordemAquisicao::reference set_ordemAquisicao () { return *static_cast<ordemAquisicao::pointer>(fields[2]); }
    ordemAquisicao::reference set_ordemAquisicao (ASN1::INTEGER::int_type v)
    { return *static_cast<ordemAquisicao::pointer>(fields[2]) = v; }

    class ordemImpressao
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 99> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class ordemImpressao

    ordemImpressao::const_reference get_ordemImpressao () const { return *static_cast<ordemImpressao::const_pointer>(fields[3]); }
    ordemImpressao::reference ref_ordemImpressao () { return *static_cast<ordemImpressao::pointer>(fields[3]); }
    ordemImpressao::reference set_ordemImpressao () { return *static_cast<ordemImpressao::pointer>(fields[3]); }
    ordemImpressao::reference set_ordemImpressao (ASN1::INTEGER::int_type v)
    { return *static_cast<ordemImpressao::pointer>(fields[3]) = v; }

    class ordemApuracao
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 99> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class ordemApuracao

    ordemApuracao::const_reference get_ordemApuracao () const { return *static_cast<ordemApuracao::const_pointer>(fields[4]); }
    ordemApuracao::reference ref_ordemApuracao () { return *static_cast<ordemApuracao::pointer>(fields[4]); }
    ordemApuracao::reference set_ordemApuracao () { return *static_cast<ordemApuracao::pointer>(fields[4]); }
    ordemApuracao::reference set_ordemApuracao (ASN1::INTEGER::int_type v)
    { return *static_cast<ordemApuracao::pointer>(fields[4]) = v; }
    void swap(SituacaoEleicao& that) { Inherited::swap(that); }
    SituacaoEleicao * clone() const
    { return static_cast<SituacaoEleicao*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[5];
    static int fieldIds[5];
    static const char* fieldNames[5];
}; // end class SituacaoEleicao

//
// ModuloSituacoesEleicoes::EntidadeSituacoesEleicoes
//

class EntidadeSituacoesEleicoes : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EntidadeSituacoesEleicoes(const void* info) : Inherited(info) {}
public:
    EntidadeSituacoesEleicoes() : Inherited(&theInfo) {}
    EntidadeSituacoesEleicoes(const EntidadeSituacoesEleicoes & that) : Inherited(that) {}

    EntidadeSituacoesEleicoes& operator=(const EntidadeSituacoesEleicoes& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_situacoesEleicoes
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

    class situacoesEleicoes
    {
    public:
        //
        // ModuloSituacoesEleicoes::EntidadeSituacoesEleicoes::situacoesEleicoes::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ModuloSituacoesEleicoes::SituacaoEleicao>
        {
            typedef ASN1::SEQUENCE_OF<ModuloSituacoesEleicoes::SituacaoEleicao> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ModuloSituacoesEleicoes::SituacaoEleicao& val) : Inherited(n, val) {}
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
    }; // end class situacoesEleicoes

    situacoesEleicoes::const_reference get_situacoesEleicoes () const
    {
      assert(hasOptionalField(e_situacoesEleicoes));
      return *static_cast<situacoesEleicoes::const_pointer>(fields[1]);
    }
    situacoesEleicoes::reference ref_situacoesEleicoes ()
    {
      assert(hasOptionalField(e_situacoesEleicoes));
      return *static_cast<situacoesEleicoes::pointer>(fields[1]);
    }
    situacoesEleicoes::reference set_situacoesEleicoes ()
    {
      includeOptionalField( e_situacoesEleicoes, 1);
      return *static_cast<situacoesEleicoes::pointer>(fields[1]);
    }
    void omit_situacoesEleicoes () { removeOptionalField( e_situacoesEleicoes); }
    bool situacoesEleicoes_isPresent () const { return hasOptionalField( e_situacoesEleicoes); }
    void swap(EntidadeSituacoesEleicoes& that) { Inherited::swap(that); }
    EntidadeSituacoesEleicoes * clone() const
    { return static_cast<EntidadeSituacoesEleicoes*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[2];
    static int fieldIds[2];
    static const char* fieldNames[2];
}; // end class EntidadeSituacoesEleicoes

} // namespace ModuloSituacoesEleicoes

#endif // __MODULOSITUACOESELEICOES_H
