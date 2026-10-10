#ifndef __MODULOHASHES_H
#define __MODULOHASHES_H

#include "asn1.h"

#include "ModuloTiposEleitorais.h"
#include "ModuloTiposResultadosEcoUrna.h"

namespace ModuloHashes {

//
// ModuloHashes::HashesArquivos   // SYNTHESIZED name (field ['hashesArquivos', 'arquivos'])
//

class HashesArquivos : public ASN1::SEQUENCE_OF<ModuloTiposEleitorais::ArquivoAssinatura>
{
    typedef ASN1::SEQUENCE_OF<ModuloTiposEleitorais::ArquivoAssinatura> Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    HashesArquivos(const void* info) : Inherited(info) {}
public:
    HashesArquivos(size_type n=0) : Inherited(n) {}
    HashesArquivos(size_type n, const ModuloTiposEleitorais::ArquivoAssinatura& val) : Inherited(n, val) {}
    HashesArquivos(const_iterator first, const_iterator last) : Inherited(first, last) {}
    HashesArquivos(const HashesArquivos& that) : Inherited(that) {}
    HashesArquivos * clone() const
    { return static_cast<HashesArquivos*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
}; // end class HashesArquivos

//
// ModuloHashes::EntidadeHashes
//

class EntidadeHashes : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EntidadeHashes(const void* info) : Inherited(info) {}
public:
    EntidadeHashes() : Inherited(&theInfo) {}
    EntidadeHashes(const EntidadeHashes & that) : Inherited(that) {}

    EntidadeHashes& operator=(const EntidadeHashes& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_identificacaoUrna
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

    class siglaUF
    {
    public:
        typedef ModuloTiposEleitorais::SiglaUF value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class siglaUF

    siglaUF::const_reference get_siglaUF () const { return *static_cast<siglaUF::const_pointer>(fields[2]); }
    siglaUF::reference ref_siglaUF () { return *static_cast<siglaUF::pointer>(fields[2]); }
    siglaUF::reference set_siglaUF () { return *static_cast<siglaUF::pointer>(fields[2]); }
    siglaUF::reference set_siglaUF (const ASN1_STD string& v)
    { return *static_cast<siglaUF::pointer>(fields[2]) = v; }

    class identificacaoUrna
    {
    public:
        typedef ModuloTiposResultadosEcoUrna::Identificacao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class identificacaoUrna

    identificacaoUrna::const_reference get_identificacaoUrna () const
    {
      assert(hasOptionalField(e_identificacaoUrna));
      return *static_cast<identificacaoUrna::const_pointer>(fields[3]);
    }
    identificacaoUrna::reference ref_identificacaoUrna ()
    {
      assert(hasOptionalField(e_identificacaoUrna));
      return *static_cast<identificacaoUrna::pointer>(fields[3]);
    }
    identificacaoUrna::reference set_identificacaoUrna ()
    {
      includeOptionalField( e_identificacaoUrna, 3);
      return *static_cast<identificacaoUrna::pointer>(fields[3]);
    }
    void omit_identificacaoUrna () { removeOptionalField( e_identificacaoUrna); }
    bool identificacaoUrna_isPresent () const { return hasOptionalField( e_identificacaoUrna); }

    class versaoUENUX
    {
    public:
        typedef ASN1::GeneralString value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class versaoUENUX

    versaoUENUX::const_reference get_versaoUENUX () const { return *static_cast<versaoUENUX::const_pointer>(fields[4]); }
    versaoUENUX::reference ref_versaoUENUX () { return *static_cast<versaoUENUX::pointer>(fields[4]); }
    versaoUENUX::reference set_versaoUENUX () { return *static_cast<versaoUENUX::pointer>(fields[4]); }
    versaoUENUX::reference set_versaoUENUX (const ASN1_STD string& v)
    { return *static_cast<versaoUENUX::pointer>(fields[4]) = v; }

    class hashesArquivos
    {
    public:
        typedef ModuloHashes::HashesArquivos value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class hashesArquivos

    hashesArquivos::const_reference get_hashesArquivos () const { return *static_cast<hashesArquivos::const_pointer>(fields[5]); }
    hashesArquivos::reference ref_hashesArquivos () { return *static_cast<hashesArquivos::pointer>(fields[5]); }
    hashesArquivos::reference set_hashesArquivos () { return *static_cast<hashesArquivos::pointer>(fields[5]); }
    void swap(EntidadeHashes& that) { Inherited::swap(that); }
    EntidadeHashes * clone() const
    { return static_cast<EntidadeHashes*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[6];
    static int fieldIds[6];
    static const char* fieldNames[6];
}; // end class EntidadeHashes

//
// ModuloHashes::ImpedimentoP1   // SYNTHESIZED name (field ['impedimentoP1', 'impedimentoP2'])
//

class ImpedimentoP1 : public ASN1::ENUMERATED
{
    typedef ASN1::ENUMERATED Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    ImpedimentoP1(const void* info) : Inherited(info) {}
public:
    ImpedimentoP1() : Inherited(&theInfo) {}
    enum NamedNumber {
        unknownEnumeration_ = -1,
        semImpedimento = 1,
        votaNaSecaoOriginal,
        solicitouVotoEmTransito,
        presoProvisorio,
        suspenso,
        cancelado,
        naoLiberado,
        semIdadeMinima,
        militarEmServico,
        tteAcessibilidade,
        tteEleitorConvocado,
        tteJusticaEleitoral,
        tteOficio,
        tteIndigenasQuilombolas,
        tteSituacaoRua
    };

    bool is_semImpedimento() const { return value == semImpedimento; }
    void set_semImpedimento() { value = semImpedimento; }

    bool is_votaNaSecaoOriginal() const { return value == votaNaSecaoOriginal; }
    void set_votaNaSecaoOriginal() { value = votaNaSecaoOriginal; }

    bool is_solicitouVotoEmTransito() const { return value == solicitouVotoEmTransito; }
    void set_solicitouVotoEmTransito() { value = solicitouVotoEmTransito; }

    bool is_presoProvisorio() const { return value == presoProvisorio; }
    void set_presoProvisorio() { value = presoProvisorio; }

    bool is_suspenso() const { return value == suspenso; }
    void set_suspenso() { value = suspenso; }

    bool is_cancelado() const { return value == cancelado; }
    void set_cancelado() { value = cancelado; }

    bool is_naoLiberado() const { return value == naoLiberado; }
    void set_naoLiberado() { value = naoLiberado; }

    bool is_semIdadeMinima() const { return value == semIdadeMinima; }
    void set_semIdadeMinima() { value = semIdadeMinima; }

    bool is_militarEmServico() const { return value == militarEmServico; }
    void set_militarEmServico() { value = militarEmServico; }

    bool is_tteAcessibilidade() const { return value == tteAcessibilidade; }
    void set_tteAcessibilidade() { value = tteAcessibilidade; }

    bool is_tteEleitorConvocado() const { return value == tteEleitorConvocado; }
    void set_tteEleitorConvocado() { value = tteEleitorConvocado; }

    bool is_tteJusticaEleitoral() const { return value == tteJusticaEleitoral; }
    void set_tteJusticaEleitoral() { value = tteJusticaEleitoral; }

    bool is_tteOficio() const { return value == tteOficio; }
    void set_tteOficio() { value = tteOficio; }

    bool is_tteIndigenasQuilombolas() const { return value == tteIndigenasQuilombolas; }
    void set_tteIndigenasQuilombolas() { value = tteIndigenasQuilombolas; }

    bool is_tteSituacaoRua() const { return value == tteSituacaoRua; }
    void set_tteSituacaoRua() { value = tteSituacaoRua; }

    ImpedimentoP1(const NamedNumber v) : Inherited(&theInfo) { setFromInt(v); }
    ImpedimentoP1 & operator=(const NamedNumber v) { setFromInt(v);  return *this; }
    operator NamedNumber() const { return NamedNumber(asInt()); }

    bool operator == (NamedNumber rhs) const { return value == rhs; }
    bool operator != (NamedNumber rhs) const { return value != rhs; }
    bool operator <  (NamedNumber rhs) const { return value <  rhs; }
    bool operator >  (NamedNumber rhs) const { return value >  rhs; }
    bool operator <= (NamedNumber rhs) const { return value <= rhs; }
    bool operator >= (NamedNumber rhs) const { return value >= rhs; }
    bool operator == (const ImpedimentoP1 &rhs) const { return value == rhs.value; }
    bool operator != (const ImpedimentoP1 &rhs) const { return value != rhs.value; }

    void swap (ImpedimentoP1 & that) { Inherited::swap(that); }
    ImpedimentoP1 * clone() const
    { return static_cast<ImpedimentoP1*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const char * nameList[];
}; // end class ImpedimentoP1

} // namespace ModuloHashes

#endif // __MODULOHASHES_H
