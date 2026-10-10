#ifndef __MODULOENVELOPEGENERICO_H
#define __MODULOENVELOPEGENERICO_H

#include "asn1.h"

#include "ModuloTiposEleitorais.h"
#include "ModuloTiposResultadosEcoUrna.h"

namespace ModuloEnvelopeGenerico {

//
// ModuloEnvelopeGenerico::TipoEnvelope   // named by exact field/RTTI match
//

class TipoEnvelope : public ASN1::ENUMERATED
{
    typedef ASN1::ENUMERATED Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    TipoEnvelope(const void* info) : Inherited(info) {}
public:
    TipoEnvelope() : Inherited(&theInfo) {}
    enum NamedNumber {
        unknownEnumeration_ = -1,
        envelopeBoletimUrna = 1,
        envelopeRegistroDigitalVoto,
        envelopeBoletimUrnaImpresso = 4,
        envelopeImagemBiometria,
        envelopeZeresimaImpressa
    };

    bool is_envelopeBoletimUrna() const { return value == envelopeBoletimUrna; }
    void set_envelopeBoletimUrna() { value = envelopeBoletimUrna; }

    bool is_envelopeRegistroDigitalVoto() const { return value == envelopeRegistroDigitalVoto; }
    void set_envelopeRegistroDigitalVoto() { value = envelopeRegistroDigitalVoto; }

    bool is_envelopeBoletimUrnaImpresso() const { return value == envelopeBoletimUrnaImpresso; }
    void set_envelopeBoletimUrnaImpresso() { value = envelopeBoletimUrnaImpresso; }

    bool is_envelopeImagemBiometria() const { return value == envelopeImagemBiometria; }
    void set_envelopeImagemBiometria() { value = envelopeImagemBiometria; }

    bool is_envelopeZeresimaImpressa() const { return value == envelopeZeresimaImpressa; }
    void set_envelopeZeresimaImpressa() { value = envelopeZeresimaImpressa; }

    TipoEnvelope(const NamedNumber v) : Inherited(&theInfo) { setFromInt(v); }
    TipoEnvelope & operator=(const NamedNumber v) { setFromInt(v);  return *this; }
    operator NamedNumber() const { return NamedNumber(asInt()); }

    bool operator == (NamedNumber rhs) const { return value == rhs; }
    bool operator != (NamedNumber rhs) const { return value != rhs; }
    bool operator <  (NamedNumber rhs) const { return value <  rhs; }
    bool operator >  (NamedNumber rhs) const { return value >  rhs; }
    bool operator <= (NamedNumber rhs) const { return value <= rhs; }
    bool operator >= (NamedNumber rhs) const { return value >= rhs; }
    bool operator == (const TipoEnvelope &rhs) const { return value == rhs.value; }
    bool operator != (const TipoEnvelope &rhs) const { return value != rhs.value; }

    void swap (TipoEnvelope & that) { Inherited::swap(that); }
    TipoEnvelope * clone() const
    { return static_cast<TipoEnvelope*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const char * nameList[];
}; // end class TipoEnvelope

//
// ModuloEnvelopeGenerico::EntidadeEnvelopeGenerico
//

class EntidadeEnvelopeGenerico : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EntidadeEnvelopeGenerico(const void* info) : Inherited(info) {}
public:
    EntidadeEnvelopeGenerico() : Inherited(&theInfo) {}
    EntidadeEnvelopeGenerico(const EntidadeEnvelopeGenerico & that) : Inherited(that) {}

    EntidadeEnvelopeGenerico& operator=(const EntidadeEnvelopeGenerico& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_urna,
      e_seguranca
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

    class urna
    {
    public:
        typedef ModuloTiposResultadosEcoUrna::Urna value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class urna

    urna::const_reference get_urna () const
    {
      assert(hasOptionalField(e_urna));
      return *static_cast<urna::const_pointer>(fields[2]);
    }
    urna::reference ref_urna ()
    {
      assert(hasOptionalField(e_urna));
      return *static_cast<urna::pointer>(fields[2]);
    }
    urna::reference set_urna ()
    {
      includeOptionalField( e_urna, 2);
      return *static_cast<urna::pointer>(fields[2]);
    }
    void omit_urna () { removeOptionalField( e_urna); }
    bool urna_isPresent () const { return hasOptionalField( e_urna); }

    class identificacao
    {
    public:
        typedef ModuloTiposResultadosEcoUrna::Identificacao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class identificacao

    identificacao::const_reference get_identificacao () const { return *static_cast<identificacao::const_pointer>(fields[3]); }
    identificacao::reference ref_identificacao () { return *static_cast<identificacao::pointer>(fields[3]); }
    identificacao::reference set_identificacao () { return *static_cast<identificacao::pointer>(fields[3]); }

    class tipoEnvelope
    {
    public:
        typedef ModuloEnvelopeGenerico::TipoEnvelope value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class tipoEnvelope

    tipoEnvelope::const_reference get_tipoEnvelope () const { return *static_cast<tipoEnvelope::const_pointer>(fields[4]); }
    tipoEnvelope::reference ref_tipoEnvelope () { return *static_cast<tipoEnvelope::pointer>(fields[4]); }
    tipoEnvelope::reference set_tipoEnvelope () { return *static_cast<tipoEnvelope::pointer>(fields[4]); }
    tipoEnvelope::reference set_tipoEnvelope (ModuloEnvelopeGenerico::TipoEnvelope::NamedNumber v)
    { return *static_cast<tipoEnvelope::pointer>(fields[4]) = v; }

    class seguranca
    {
    public:
        typedef ModuloTiposEleitorais::Seguranca value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class seguranca

    seguranca::const_reference get_seguranca () const
    {
      assert(hasOptionalField(e_seguranca));
      return *static_cast<seguranca::const_pointer>(fields[5]);
    }
    seguranca::reference ref_seguranca ()
    {
      assert(hasOptionalField(e_seguranca));
      return *static_cast<seguranca::pointer>(fields[5]);
    }
    seguranca::reference set_seguranca ()
    {
      includeOptionalField( e_seguranca, 5);
      return *static_cast<seguranca::pointer>(fields[5]);
    }
    void omit_seguranca () { removeOptionalField( e_seguranca); }
    bool seguranca_isPresent () const { return hasOptionalField( e_seguranca); }

    class conteudo
    {
    public:
        typedef ASN1::OCTET_STRING value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class conteudo

    conteudo::const_reference get_conteudo () const { return *static_cast<conteudo::const_pointer>(fields[6]); }
    conteudo::reference ref_conteudo () { return *static_cast<conteudo::pointer>(fields[6]); }
    conteudo::reference set_conteudo () { return *static_cast<conteudo::pointer>(fields[6]); }
    conteudo::reference set_conteudo (const ASN1::OCTET_STRING& v)
    { return *static_cast<conteudo::pointer>(fields[6]) = v; }
    void swap(EntidadeEnvelopeGenerico& that) { Inherited::swap(that); }
    EntidadeEnvelopeGenerico * clone() const
    { return static_cast<EntidadeEnvelopeGenerico*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[7];
    static int fieldIds[7];
    static const char* fieldNames[7];
}; // end class EntidadeEnvelopeGenerico

} // namespace ModuloEnvelopeGenerico

#endif // __MODULOENVELOPEGENERICO_H
