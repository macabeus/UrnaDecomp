#ifndef __MODULOTIPOSECOURNA_H
#define __MODULOTIPOSECOURNA_H

#include "asn1.h"

#include "ModuloTiposEleitorais.h"

namespace ModuloTiposEcoUrna {

//
// ModuloTiposEcoUrna::RegistroIdentificacaoEleitor
//

class RegistroIdentificacaoEleitor : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    RegistroIdentificacaoEleitor(const void* info) : Inherited(info) {}
public:
    RegistroIdentificacaoEleitor() : Inherited(&theInfo) {}
    RegistroIdentificacaoEleitor(const RegistroIdentificacaoEleitor & that) : Inherited(that) {}

    RegistroIdentificacaoEleitor& operator=(const RegistroIdentificacaoEleitor& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_identificacaoPrincipal
    };

    class identificacaoUtilizada
    {
    public:
        typedef ModuloTiposEleitorais::IdentificadorEleitor value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class identificacaoUtilizada

    identificacaoUtilizada::const_reference get_identificacaoUtilizada () const { return *static_cast<identificacaoUtilizada::const_pointer>(fields[0]); }
    identificacaoUtilizada::reference ref_identificacaoUtilizada () { return *static_cast<identificacaoUtilizada::pointer>(fields[0]); }
    identificacaoUtilizada::reference set_identificacaoUtilizada () { return *static_cast<identificacaoUtilizada::pointer>(fields[0]); }

    class identificacaoPrincipal
    {
    public:
        typedef ModuloTiposEleitorais::IdentificadorEleitor value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class identificacaoPrincipal

    identificacaoPrincipal::const_reference get_identificacaoPrincipal () const
    {
      assert(hasOptionalField(e_identificacaoPrincipal));
      return *static_cast<identificacaoPrincipal::const_pointer>(fields[1]);
    }
    identificacaoPrincipal::reference ref_identificacaoPrincipal ()
    {
      assert(hasOptionalField(e_identificacaoPrincipal));
      return *static_cast<identificacaoPrincipal::pointer>(fields[1]);
    }
    identificacaoPrincipal::reference set_identificacaoPrincipal ()
    {
      includeOptionalField( e_identificacaoPrincipal, 1);
      return *static_cast<identificacaoPrincipal::pointer>(fields[1]);
    }
    void omit_identificacaoPrincipal () { removeOptionalField( e_identificacaoPrincipal); }
    bool identificacaoPrincipal_isPresent () const { return hasOptionalField( e_identificacaoPrincipal); }
    void swap(RegistroIdentificacaoEleitor& that) { Inherited::swap(that); }
    RegistroIdentificacaoEleitor * clone() const
    { return static_cast<RegistroIdentificacaoEleitor*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[2];
    static int fieldIds[2];
    static const char* fieldNames[2];
}; // end class RegistroIdentificacaoEleitor

//
// ModuloTiposEcoUrna::IdentificadorGeradorMidia
//

class IdentificadorGeradorMidia : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    IdentificadorGeradorMidia(const void* info) : Inherited(info) {}
public:
    IdentificadorGeradorMidia() : Inherited(&theInfo) {}
    IdentificadorGeradorMidia(const IdentificadorGeradorMidia & that) : Inherited(that) {}

    IdentificadorGeradorMidia& operator=(const IdentificadorGeradorMidia& that)
    { Inherited::operator=(that); return *this; }


    class nome
    {
    public:
        typedef ASN1::GeneralString value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class nome

    nome::const_reference get_nome () const { return *static_cast<nome::const_pointer>(fields[0]); }
    nome::reference ref_nome () { return *static_cast<nome::pointer>(fields[0]); }
    nome::reference set_nome () { return *static_cast<nome::pointer>(fields[0]); }
    nome::reference set_nome (const ASN1_STD string& v)
    { return *static_cast<nome::pointer>(fields[0]) = v; }

    class serialCertificadoTPM
    {
    public:
        typedef ASN1::GeneralString value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class serialCertificadoTPM

    serialCertificadoTPM::const_reference get_serialCertificadoTPM () const { return *static_cast<serialCertificadoTPM::const_pointer>(fields[1]); }
    serialCertificadoTPM::reference ref_serialCertificadoTPM () { return *static_cast<serialCertificadoTPM::pointer>(fields[1]); }
    serialCertificadoTPM::reference set_serialCertificadoTPM () { return *static_cast<serialCertificadoTPM::pointer>(fields[1]); }
    serialCertificadoTPM::reference set_serialCertificadoTPM (const ASN1_STD string& v)
    { return *static_cast<serialCertificadoTPM::pointer>(fields[1]) = v; }

    class serialInstalacao
    {
    public:
        typedef ASN1::GeneralString value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class serialInstalacao

    serialInstalacao::const_reference get_serialInstalacao () const { return *static_cast<serialInstalacao::const_pointer>(fields[2]); }
    serialInstalacao::reference ref_serialInstalacao () { return *static_cast<serialInstalacao::pointer>(fields[2]); }
    serialInstalacao::reference set_serialInstalacao () { return *static_cast<serialInstalacao::pointer>(fields[2]); }
    serialInstalacao::reference set_serialInstalacao (const ASN1_STD string& v)
    { return *static_cast<serialInstalacao::pointer>(fields[2]) = v; }
    void swap(IdentificadorGeradorMidia& that) { Inherited::swap(that); }
    IdentificadorGeradorMidia * clone() const
    { return static_cast<IdentificadorGeradorMidia*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[3];
    static int fieldIds[3];
    static const char* fieldNames[3];
}; // end class IdentificadorGeradorMidia

//
// ModuloTiposEcoUrna::Carga
//

class Carga : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    Carga(const void* info) : Inherited(info) {}
public:
    Carga() : Inherited(&theInfo) {}
    Carga(const Carga & that) : Inherited(that) {}

    Carga& operator=(const Carga& that)
    { Inherited::operator=(that); return *this; }


    class numeroInternoUrna
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 99999999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class numeroInternoUrna

    numeroInternoUrna::const_reference get_numeroInternoUrna () const { return *static_cast<numeroInternoUrna::const_pointer>(fields[0]); }
    numeroInternoUrna::reference ref_numeroInternoUrna () { return *static_cast<numeroInternoUrna::pointer>(fields[0]); }
    numeroInternoUrna::reference set_numeroInternoUrna () { return *static_cast<numeroInternoUrna::pointer>(fields[0]); }
    numeroInternoUrna::reference set_numeroInternoUrna (ASN1::INTEGER::int_type v)
    { return *static_cast<numeroInternoUrna::pointer>(fields[0]) = v; }

    class numeroSerieFC
    {
    public:
        typedef ASN1::Constrained_OCTET_STRING<ASN1::SizeConstraint<ASN1::FixedConstraint, 4, 4> > value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class numeroSerieFC

    numeroSerieFC::const_reference get_numeroSerieFC () const { return *static_cast<numeroSerieFC::const_pointer>(fields[1]); }
    numeroSerieFC::reference ref_numeroSerieFC () { return *static_cast<numeroSerieFC::pointer>(fields[1]); }
    numeroSerieFC::reference set_numeroSerieFC () { return *static_cast<numeroSerieFC::pointer>(fields[1]); }
    numeroSerieFC::reference set_numeroSerieFC (const ASN1::OCTET_STRING& v)
    { return *static_cast<numeroSerieFC::pointer>(fields[1]) = v; }

    class identificadorGeradorMidia
    {
    public:
        typedef ModuloTiposEcoUrna::IdentificadorGeradorMidia value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class identificadorGeradorMidia

    identificadorGeradorMidia::const_reference get_identificadorGeradorMidia () const { return *static_cast<identificadorGeradorMidia::const_pointer>(fields[2]); }
    identificadorGeradorMidia::reference ref_identificadorGeradorMidia () { return *static_cast<identificadorGeradorMidia::pointer>(fields[2]); }
    identificadorGeradorMidia::reference set_identificadorGeradorMidia () { return *static_cast<identificadorGeradorMidia::pointer>(fields[2]); }

    class dataHoraCarga
    {
    public:
        typedef ModuloTiposEleitorais::DataGeracao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class dataHoraCarga

    dataHoraCarga::const_reference get_dataHoraCarga () const { return *static_cast<dataHoraCarga::const_pointer>(fields[3]); }
    dataHoraCarga::reference ref_dataHoraCarga () { return *static_cast<dataHoraCarga::pointer>(fields[3]); }
    dataHoraCarga::reference set_dataHoraCarga () { return *static_cast<dataHoraCarga::pointer>(fields[3]); }
    dataHoraCarga::reference set_dataHoraCarga (const ASN1_STD string& v)
    { return *static_cast<dataHoraCarga::pointer>(fields[3]) = v; }

    class codigoCarga
    {
    public:
        typedef ASN1::GeneralString value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class codigoCarga

    codigoCarga::const_reference get_codigoCarga () const { return *static_cast<codigoCarga::const_pointer>(fields[4]); }
    codigoCarga::reference ref_codigoCarga () { return *static_cast<codigoCarga::pointer>(fields[4]); }
    codigoCarga::reference set_codigoCarga () { return *static_cast<codigoCarga::pointer>(fields[4]); }
    codigoCarga::reference set_codigoCarga (const ASN1_STD string& v)
    { return *static_cast<codigoCarga::pointer>(fields[4]) = v; }
    void swap(Carga& that) { Inherited::swap(that); }
    Carga * clone() const
    { return static_cast<Carga*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[5];
    static int fieldIds[5];
    static const char* fieldNames[5];
}; // end class Carga

//
// ModuloTiposEcoUrna::SituacaoArquivo
//

class SituacaoArquivo : public ASN1::ENUMERATED
{
    typedef ASN1::ENUMERATED Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    SituacaoArquivo(const void* info) : Inherited(info) {}
public:
    SituacaoArquivo() : Inherited(&theInfo) {}
    enum NamedNumber {
        unknownEnumeration_ = -1,
        arquivoFinal = 1,
        arquivoParcial
    };

    bool is_arquivoFinal() const { return value == arquivoFinal; }
    void set_arquivoFinal() { value = arquivoFinal; }

    bool is_arquivoParcial() const { return value == arquivoParcial; }
    void set_arquivoParcial() { value = arquivoParcial; }

    SituacaoArquivo(const NamedNumber v) : Inherited(&theInfo) { setFromInt(v); }
    SituacaoArquivo & operator=(const NamedNumber v) { setFromInt(v);  return *this; }
    operator NamedNumber() const { return NamedNumber(asInt()); }

    bool operator == (NamedNumber rhs) const { return value == rhs; }
    bool operator != (NamedNumber rhs) const { return value != rhs; }
    bool operator <  (NamedNumber rhs) const { return value <  rhs; }
    bool operator >  (NamedNumber rhs) const { return value >  rhs; }
    bool operator <= (NamedNumber rhs) const { return value <= rhs; }
    bool operator >= (NamedNumber rhs) const { return value >= rhs; }
    bool operator == (const SituacaoArquivo &rhs) const { return value == rhs.value; }
    bool operator != (const SituacaoArquivo &rhs) const { return value != rhs.value; }

    void swap (SituacaoArquivo & that) { Inherited::swap(that); }
    SituacaoArquivo * clone() const
    { return static_cast<SituacaoArquivo*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const char * nameList[];
}; // end class SituacaoArquivo

} // namespace ModuloTiposEcoUrna

#endif // __MODULOTIPOSECOURNA_H
