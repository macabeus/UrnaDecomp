#ifndef __MODULOIMPEDIDOS_H
#define __MODULOIMPEDIDOS_H

#include "asn1.h"

#include "ModuloHashes.h"
#include "ModuloTiposEleitorais.h"

namespace ModuloImpedidos {

//
// ModuloImpedidos::EleitorImpedido
//

class EleitorImpedido : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EleitorImpedido(const void* info) : Inherited(info) {}
public:
    EleitorImpedido() : Inherited(&theInfo) {}
    EleitorImpedido(const EleitorImpedido & that) : Inherited(that) {}

    EleitorImpedido& operator=(const EleitorImpedido& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_impedimentoP2
    };

    class identificadorEleitor
    {
    public:
        typedef ModuloTiposEleitorais::IdentificadorEleitor value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class identificadorEleitor

    identificadorEleitor::const_reference get_identificadorEleitor () const { return *static_cast<identificadorEleitor::const_pointer>(fields[0]); }
    identificadorEleitor::reference ref_identificadorEleitor () { return *static_cast<identificadorEleitor::pointer>(fields[0]); }
    identificadorEleitor::reference set_identificadorEleitor () { return *static_cast<identificadorEleitor::pointer>(fields[0]); }

    class impedimentoP1
    {
    public:
        typedef ModuloHashes::ImpedimentoP1 value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class impedimentoP1

    impedimentoP1::const_reference get_impedimentoP1 () const { return *static_cast<impedimentoP1::const_pointer>(fields[1]); }
    impedimentoP1::reference ref_impedimentoP1 () { return *static_cast<impedimentoP1::pointer>(fields[1]); }
    impedimentoP1::reference set_impedimentoP1 () { return *static_cast<impedimentoP1::pointer>(fields[1]); }
    impedimentoP1::reference set_impedimentoP1 (ModuloHashes::ImpedimentoP1::NamedNumber v)
    { return *static_cast<impedimentoP1::pointer>(fields[1]) = v; }

    class impedimentoP2
    {
    public:
        typedef ModuloHashes::ImpedimentoP1 value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class impedimentoP2

    impedimentoP2::const_reference get_impedimentoP2 () const
    {
      assert(hasOptionalField(e_impedimentoP2));
      return *static_cast<impedimentoP2::const_pointer>(fields[2]);
    }
    impedimentoP2::reference ref_impedimentoP2 ()
    {
      assert(hasOptionalField(e_impedimentoP2));
      return *static_cast<impedimentoP2::pointer>(fields[2]);
    }
    impedimentoP2::reference set_impedimentoP2 ()
    {
      includeOptionalField( e_impedimentoP2, 2);
      return *static_cast<impedimentoP2::pointer>(fields[2]);
    }
    impedimentoP2::reference set_impedimentoP2 (ModuloHashes::ImpedimentoP1::NamedNumber v)
    {
      includeOptionalField( e_impedimentoP2, 2);
      return *static_cast<impedimentoP2::pointer>(fields[2]) = v;
    }
    void omit_impedimentoP2 () { removeOptionalField( e_impedimentoP2); }
    bool impedimentoP2_isPresent () const { return hasOptionalField( e_impedimentoP2); }
    void swap(EleitorImpedido& that) { Inherited::swap(that); }
    EleitorImpedido * clone() const
    { return static_cast<EleitorImpedido*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[3];
    static int fieldIds[3];
    static const char* fieldNames[3];
}; // end class EleitorImpedido

//
// ModuloImpedidos::EntidadeImpedidos
//

class EntidadeImpedidos : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EntidadeImpedidos(const void* info) : Inherited(info) {}
public:
    EntidadeImpedidos() : Inherited(&theInfo) {}
    EntidadeImpedidos(const EntidadeImpedidos & that) : Inherited(that) {}

    EntidadeImpedidos& operator=(const EntidadeImpedidos& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_eleitores
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

    class identificacao
    {
    public:
        typedef ModuloTiposEleitorais::IdentificacaoSecaoEleitoral value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class identificacao

    identificacao::const_reference get_identificacao () const { return *static_cast<identificacao::const_pointer>(fields[1]); }
    identificacao::reference ref_identificacao () { return *static_cast<identificacao::pointer>(fields[1]); }
    identificacao::reference set_identificacao () { return *static_cast<identificacao::pointer>(fields[1]); }

    class eleitores
    {
    public:
        //
        // ModuloImpedidos::EntidadeImpedidos::eleitores::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ModuloImpedidos::EleitorImpedido>
        {
            typedef ASN1::SEQUENCE_OF<ModuloImpedidos::EleitorImpedido> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ModuloImpedidos::EleitorImpedido& val) : Inherited(n, val) {}
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
    }; // end class eleitores

    eleitores::const_reference get_eleitores () const
    {
      assert(hasOptionalField(e_eleitores));
      return *static_cast<eleitores::const_pointer>(fields[2]);
    }
    eleitores::reference ref_eleitores ()
    {
      assert(hasOptionalField(e_eleitores));
      return *static_cast<eleitores::pointer>(fields[2]);
    }
    eleitores::reference set_eleitores ()
    {
      includeOptionalField( e_eleitores, 2);
      return *static_cast<eleitores::pointer>(fields[2]);
    }
    void omit_eleitores () { removeOptionalField( e_eleitores); }
    bool eleitores_isPresent () const { return hasOptionalField( e_eleitores); }
    void swap(EntidadeImpedidos& that) { Inherited::swap(that); }
    EntidadeImpedidos * clone() const
    { return static_cast<EntidadeImpedidos*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[3];
    static int fieldIds[3];
    static const char* fieldNames[3];
}; // end class EntidadeImpedidos

} // namespace ModuloImpedidos

#endif // __MODULOIMPEDIDOS_H
