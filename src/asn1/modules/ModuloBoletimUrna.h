#ifndef __MODULOBOLETIMURNA_H
#define __MODULOBOLETIMURNA_H

#include "asn1.h"

#include "ModuloRegistroDigitalVoto.h"
#include "ModuloTiposEleitorais.h"
#include "ModuloTiposResultadosEcoUrna.h"

namespace ModuloBoletimUrna {

//
// ModuloBoletimUrna::DetalhamentoComparecimento
//

class DetalhamentoComparecimento : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    DetalhamentoComparecimento(const void* info) : Inherited(info) {}
public:
    DetalhamentoComparecimento() : Inherited(&theInfo) {}
    DetalhamentoComparecimento(const DetalhamentoComparecimento & that) : Inherited(that) {}

    DetalhamentoComparecimento& operator=(const DetalhamentoComparecimento& that)
    { Inherited::operator=(that); return *this; }


    class qtdEleitoresCompareceramSemBiometria
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 9999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class qtdEleitoresCompareceramSemBiometria

    qtdEleitoresCompareceramSemBiometria::const_reference get_qtdEleitoresCompareceramSemBiometria () const { return *static_cast<qtdEleitoresCompareceramSemBiometria::const_pointer>(fields[0]); }
    qtdEleitoresCompareceramSemBiometria::reference ref_qtdEleitoresCompareceramSemBiometria () { return *static_cast<qtdEleitoresCompareceramSemBiometria::pointer>(fields[0]); }
    qtdEleitoresCompareceramSemBiometria::reference set_qtdEleitoresCompareceramSemBiometria () { return *static_cast<qtdEleitoresCompareceramSemBiometria::pointer>(fields[0]); }
    qtdEleitoresCompareceramSemBiometria::reference set_qtdEleitoresCompareceramSemBiometria (ASN1::INTEGER::int_type v)
    { return *static_cast<qtdEleitoresCompareceramSemBiometria::pointer>(fields[0]) = v; }

    class qtdEleitoresHabilitadosPorBiometria
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 9999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class qtdEleitoresHabilitadosPorBiometria

    qtdEleitoresHabilitadosPorBiometria::const_reference get_qtdEleitoresHabilitadosPorBiometria () const { return *static_cast<qtdEleitoresHabilitadosPorBiometria::const_pointer>(fields[1]); }
    qtdEleitoresHabilitadosPorBiometria::reference ref_qtdEleitoresHabilitadosPorBiometria () { return *static_cast<qtdEleitoresHabilitadosPorBiometria::pointer>(fields[1]); }
    qtdEleitoresHabilitadosPorBiometria::reference set_qtdEleitoresHabilitadosPorBiometria () { return *static_cast<qtdEleitoresHabilitadosPorBiometria::pointer>(fields[1]); }
    qtdEleitoresHabilitadosPorBiometria::reference set_qtdEleitoresHabilitadosPorBiometria (ASN1::INTEGER::int_type v)
    { return *static_cast<qtdEleitoresHabilitadosPorBiometria::pointer>(fields[1]) = v; }

    class qtdEleitoresHabilitadosPorBiografia
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 9999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class qtdEleitoresHabilitadosPorBiografia

    qtdEleitoresHabilitadosPorBiografia::const_reference get_qtdEleitoresHabilitadosPorBiografia () const { return *static_cast<qtdEleitoresHabilitadosPorBiografia::const_pointer>(fields[2]); }
    qtdEleitoresHabilitadosPorBiografia::reference ref_qtdEleitoresHabilitadosPorBiografia () { return *static_cast<qtdEleitoresHabilitadosPorBiografia::pointer>(fields[2]); }
    qtdEleitoresHabilitadosPorBiografia::reference set_qtdEleitoresHabilitadosPorBiografia () { return *static_cast<qtdEleitoresHabilitadosPorBiografia::pointer>(fields[2]); }
    qtdEleitoresHabilitadosPorBiografia::reference set_qtdEleitoresHabilitadosPorBiografia (ASN1::INTEGER::int_type v)
    { return *static_cast<qtdEleitoresHabilitadosPorBiografia::pointer>(fields[2]) = v; }
    void swap(DetalhamentoComparecimento& that) { Inherited::swap(that); }
    DetalhamentoComparecimento * clone() const
    { return static_cast<DetalhamentoComparecimento*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[3];
    static int fieldIds[3];
    static const char* fieldNames[3];
}; // end class DetalhamentoComparecimento

//
// ModuloBoletimUrna::IdentificacaoVotavel
//

class IdentificacaoVotavel : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    IdentificacaoVotavel(const void* info) : Inherited(info) {}
public:
    IdentificacaoVotavel() : Inherited(&theInfo) {}
    IdentificacaoVotavel(const IdentificacaoVotavel & that) : Inherited(that) {}

    IdentificacaoVotavel& operator=(const IdentificacaoVotavel& that)
    { Inherited::operator=(that); return *this; }


    class partido
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 99> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class partido

    partido::const_reference get_partido () const { return *static_cast<partido::const_pointer>(fields[0]); }
    partido::reference ref_partido () { return *static_cast<partido::pointer>(fields[0]); }
    partido::reference set_partido () { return *static_cast<partido::pointer>(fields[0]); }
    partido::reference set_partido (ASN1::INTEGER::int_type v)
    { return *static_cast<partido::pointer>(fields[0]) = v; }

    class codigo
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 99999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class codigo

    codigo::const_reference get_codigo () const { return *static_cast<codigo::const_pointer>(fields[1]); }
    codigo::reference ref_codigo () { return *static_cast<codigo::pointer>(fields[1]); }
    codigo::reference set_codigo () { return *static_cast<codigo::pointer>(fields[1]); }
    codigo::reference set_codigo (ASN1::INTEGER::int_type v)
    { return *static_cast<codigo::pointer>(fields[1]) = v; }
    void swap(IdentificacaoVotavel& that) { Inherited::swap(that); }
    IdentificacaoVotavel * clone() const
    { return static_cast<IdentificacaoVotavel*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[2];
    static int fieldIds[2];
    static const char* fieldNames[2];
}; // end class IdentificacaoVotavel

//
// ModuloBoletimUrna::TipoVoto   // named by exact field/RTTI match
//

class TipoVoto : public ASN1::ENUMERATED
{
    typedef ASN1::ENUMERATED Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    TipoVoto(const void* info) : Inherited(info) {}
public:
    TipoVoto() : Inherited(&theInfo) {}
    enum NamedNumber {
        unknownEnumeration_ = -1,
        nominal = 1,
        branco,
        nulo,
        legenda,
        cargoSemCandidato
    };

    bool is_nominal() const { return value == nominal; }
    void set_nominal() { value = nominal; }

    bool is_branco() const { return value == branco; }
    void set_branco() { value = branco; }

    bool is_nulo() const { return value == nulo; }
    void set_nulo() { value = nulo; }

    bool is_legenda() const { return value == legenda; }
    void set_legenda() { value = legenda; }

    bool is_cargoSemCandidato() const { return value == cargoSemCandidato; }
    void set_cargoSemCandidato() { value = cargoSemCandidato; }

    TipoVoto(const NamedNumber v) : Inherited(&theInfo) { setFromInt(v); }
    TipoVoto & operator=(const NamedNumber v) { setFromInt(v);  return *this; }
    operator NamedNumber() const { return NamedNumber(asInt()); }

    bool operator == (NamedNumber rhs) const { return value == rhs; }
    bool operator != (NamedNumber rhs) const { return value != rhs; }
    bool operator <  (NamedNumber rhs) const { return value <  rhs; }
    bool operator >  (NamedNumber rhs) const { return value >  rhs; }
    bool operator <= (NamedNumber rhs) const { return value <= rhs; }
    bool operator >= (NamedNumber rhs) const { return value >= rhs; }
    bool operator == (const TipoVoto &rhs) const { return value == rhs.value; }
    bool operator != (const TipoVoto &rhs) const { return value != rhs.value; }

    void swap (TipoVoto & that) { Inherited::swap(that); }
    TipoVoto * clone() const
    { return static_cast<TipoVoto*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const char * nameList[];
}; // end class TipoVoto

//
// ModuloBoletimUrna::DadosSecao
//

class DadosSecao : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    DadosSecao(const void* info) : Inherited(info) {}
public:
    DadosSecao() : Inherited(&theInfo) {}
    DadosSecao(const DadosSecao & that) : Inherited(that) {}

    DadosSecao& operator=(const DadosSecao& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_dataHoraDesligamentoVotoImpresso
    };

    class dataHoraAbertura
    {
    public:
        typedef ModuloTiposEleitorais::DataGeracao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class dataHoraAbertura

    dataHoraAbertura::const_reference get_dataHoraAbertura () const { return *static_cast<dataHoraAbertura::const_pointer>(fields[0]); }
    dataHoraAbertura::reference ref_dataHoraAbertura () { return *static_cast<dataHoraAbertura::pointer>(fields[0]); }
    dataHoraAbertura::reference set_dataHoraAbertura () { return *static_cast<dataHoraAbertura::pointer>(fields[0]); }
    dataHoraAbertura::reference set_dataHoraAbertura (const ASN1_STD string& v)
    { return *static_cast<dataHoraAbertura::pointer>(fields[0]) = v; }

    class dataHoraEncerramento
    {
    public:
        typedef ModuloTiposEleitorais::DataGeracao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class dataHoraEncerramento

    dataHoraEncerramento::const_reference get_dataHoraEncerramento () const { return *static_cast<dataHoraEncerramento::const_pointer>(fields[1]); }
    dataHoraEncerramento::reference ref_dataHoraEncerramento () { return *static_cast<dataHoraEncerramento::pointer>(fields[1]); }
    dataHoraEncerramento::reference set_dataHoraEncerramento () { return *static_cast<dataHoraEncerramento::pointer>(fields[1]); }
    dataHoraEncerramento::reference set_dataHoraEncerramento (const ASN1_STD string& v)
    { return *static_cast<dataHoraEncerramento::pointer>(fields[1]) = v; }

    class dataHoraDesligamentoVotoImpresso
    {
    public:
        typedef ModuloTiposEleitorais::DataGeracao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class dataHoraDesligamentoVotoImpresso

    dataHoraDesligamentoVotoImpresso::const_reference get_dataHoraDesligamentoVotoImpresso () const
    {
      assert(hasOptionalField(e_dataHoraDesligamentoVotoImpresso));
      return *static_cast<dataHoraDesligamentoVotoImpresso::const_pointer>(fields[2]);
    }
    dataHoraDesligamentoVotoImpresso::reference ref_dataHoraDesligamentoVotoImpresso ()
    {
      assert(hasOptionalField(e_dataHoraDesligamentoVotoImpresso));
      return *static_cast<dataHoraDesligamentoVotoImpresso::pointer>(fields[2]);
    }
    dataHoraDesligamentoVotoImpresso::reference set_dataHoraDesligamentoVotoImpresso ()
    {
      includeOptionalField( e_dataHoraDesligamentoVotoImpresso, 2);
      return *static_cast<dataHoraDesligamentoVotoImpresso::pointer>(fields[2]);
    }
    dataHoraDesligamentoVotoImpresso::reference set_dataHoraDesligamentoVotoImpresso (const ASN1_STD string& v)
    {
      includeOptionalField( e_dataHoraDesligamentoVotoImpresso, 2);
      return *static_cast<dataHoraDesligamentoVotoImpresso::pointer>(fields[2]) = v;
    }
    void omit_dataHoraDesligamentoVotoImpresso () { removeOptionalField( e_dataHoraDesligamentoVotoImpresso); }
    bool dataHoraDesligamentoVotoImpresso_isPresent () const { return hasOptionalField( e_dataHoraDesligamentoVotoImpresso); }
    void swap(DadosSecao& that) { Inherited::swap(that); }
    DadosSecao * clone() const
    { return static_cast<DadosSecao*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[3];
    static int fieldIds[3];
    static const char* fieldNames[3];
}; // end class DadosSecao

//
// ModuloBoletimUrna::DadosSA
//

class DadosSA : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    DadosSA(const void* info) : Inherited(info) {}
public:
    DadosSA() : Inherited(&theInfo) {}
    DadosSA(const DadosSA & that) : Inherited(that) {}

    DadosSA& operator=(const DadosSA& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_numeroInternoUrnaOrigem
    };

    class juntaApuradora
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 9999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class juntaApuradora

    juntaApuradora::const_reference get_juntaApuradora () const { return *static_cast<juntaApuradora::const_pointer>(fields[0]); }
    juntaApuradora::reference ref_juntaApuradora () { return *static_cast<juntaApuradora::pointer>(fields[0]); }
    juntaApuradora::reference set_juntaApuradora () { return *static_cast<juntaApuradora::pointer>(fields[0]); }
    juntaApuradora::reference set_juntaApuradora (ASN1::INTEGER::int_type v)
    { return *static_cast<juntaApuradora::pointer>(fields[0]) = v; }

    class turmaApuradora
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 9999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class turmaApuradora

    turmaApuradora::const_reference get_turmaApuradora () const { return *static_cast<turmaApuradora::const_pointer>(fields[1]); }
    turmaApuradora::reference ref_turmaApuradora () { return *static_cast<turmaApuradora::pointer>(fields[1]); }
    turmaApuradora::reference set_turmaApuradora () { return *static_cast<turmaApuradora::pointer>(fields[1]); }
    turmaApuradora::reference set_turmaApuradora (ASN1::INTEGER::int_type v)
    { return *static_cast<turmaApuradora::pointer>(fields[1]) = v; }

    class numeroInternoUrnaOrigem
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 99999999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class numeroInternoUrnaOrigem

    numeroInternoUrnaOrigem::const_reference get_numeroInternoUrnaOrigem () const
    {
      assert(hasOptionalField(e_numeroInternoUrnaOrigem));
      return *static_cast<numeroInternoUrnaOrigem::const_pointer>(fields[2]);
    }
    numeroInternoUrnaOrigem::reference ref_numeroInternoUrnaOrigem ()
    {
      assert(hasOptionalField(e_numeroInternoUrnaOrigem));
      return *static_cast<numeroInternoUrnaOrigem::pointer>(fields[2]);
    }
    numeroInternoUrnaOrigem::reference set_numeroInternoUrnaOrigem ()
    {
      includeOptionalField( e_numeroInternoUrnaOrigem, 2);
      return *static_cast<numeroInternoUrnaOrigem::pointer>(fields[2]);
    }
    numeroInternoUrnaOrigem::reference set_numeroInternoUrnaOrigem (ASN1::INTEGER::int_type v)
    {
      includeOptionalField( e_numeroInternoUrnaOrigem, 2);
      return *static_cast<numeroInternoUrnaOrigem::pointer>(fields[2]) = v;
    }
    void omit_numeroInternoUrnaOrigem () { removeOptionalField( e_numeroInternoUrnaOrigem); }
    bool numeroInternoUrnaOrigem_isPresent () const { return hasOptionalField( e_numeroInternoUrnaOrigem); }
    void swap(DadosSA& that) { Inherited::swap(that); }
    DadosSA * clone() const
    { return static_cast<DadosSA*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[3];
    static int fieldIds[3];
    static const char* fieldNames[3];
}; // end class DadosSA

//
// ModuloBoletimUrna::HistoricoVotoImpresso
//

class HistoricoVotoImpresso : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    HistoricoVotoImpresso(const void* info) : Inherited(info) {}
public:
    HistoricoVotoImpresso() : Inherited(&theInfo) {}
    HistoricoVotoImpresso(const HistoricoVotoImpresso & that) : Inherited(that) {}

    HistoricoVotoImpresso& operator=(const HistoricoVotoImpresso& that)
    { Inherited::operator=(that); return *this; }


    class idImpressoraVotos
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 99999999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class idImpressoraVotos

    idImpressoraVotos::const_reference get_idImpressoraVotos () const { return *static_cast<idImpressoraVotos::const_pointer>(fields[0]); }
    idImpressoraVotos::reference ref_idImpressoraVotos () { return *static_cast<idImpressoraVotos::pointer>(fields[0]); }
    idImpressoraVotos::reference set_idImpressoraVotos () { return *static_cast<idImpressoraVotos::pointer>(fields[0]); }
    idImpressoraVotos::reference set_idImpressoraVotos (ASN1::INTEGER::int_type v)
    { return *static_cast<idImpressoraVotos::pointer>(fields[0]) = v; }

    class idRepositorioVotos
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 99999999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class idRepositorioVotos

    idRepositorioVotos::const_reference get_idRepositorioVotos () const { return *static_cast<idRepositorioVotos::const_pointer>(fields[1]); }
    idRepositorioVotos::reference ref_idRepositorioVotos () { return *static_cast<idRepositorioVotos::pointer>(fields[1]); }
    idRepositorioVotos::reference set_idRepositorioVotos () { return *static_cast<idRepositorioVotos::pointer>(fields[1]); }
    idRepositorioVotos::reference set_idRepositorioVotos (ASN1::INTEGER::int_type v)
    { return *static_cast<idRepositorioVotos::pointer>(fields[1]) = v; }

    class dataHoraLigamento
    {
    public:
        typedef ModuloTiposEleitorais::DataGeracao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class dataHoraLigamento

    dataHoraLigamento::const_reference get_dataHoraLigamento () const { return *static_cast<dataHoraLigamento::const_pointer>(fields[2]); }
    dataHoraLigamento::reference ref_dataHoraLigamento () { return *static_cast<dataHoraLigamento::pointer>(fields[2]); }
    dataHoraLigamento::reference set_dataHoraLigamento () { return *static_cast<dataHoraLigamento::pointer>(fields[2]); }
    dataHoraLigamento::reference set_dataHoraLigamento (const ASN1_STD string& v)
    { return *static_cast<dataHoraLigamento::pointer>(fields[2]) = v; }
    void swap(HistoricoVotoImpresso& that) { Inherited::swap(that); }
    HistoricoVotoImpresso * clone() const
    { return static_cast<HistoricoVotoImpresso*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[3];
    static int fieldIds[3];
    static const char* fieldNames[3];
}; // end class HistoricoVotoImpresso

//
// ModuloBoletimUrna::TotalVotosVotavel
//

class TotalVotosVotavel : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    TotalVotosVotavel(const void* info) : Inherited(info) {}
public:
    TotalVotosVotavel() : Inherited(&theInfo) {}
    TotalVotosVotavel(const TotalVotosVotavel & that) : Inherited(that) {}

    TotalVotosVotavel& operator=(const TotalVotosVotavel& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_identificacaoVotavel
    };

    class tipoVoto
    {
    public:
        typedef ModuloBoletimUrna::TipoVoto value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class tipoVoto

    tipoVoto::const_reference get_tipoVoto () const { return *static_cast<tipoVoto::const_pointer>(fields[0]); }
    tipoVoto::reference ref_tipoVoto () { return *static_cast<tipoVoto::pointer>(fields[0]); }
    tipoVoto::reference set_tipoVoto () { return *static_cast<tipoVoto::pointer>(fields[0]); }
    tipoVoto::reference set_tipoVoto (ModuloBoletimUrna::TipoVoto::NamedNumber v)
    { return *static_cast<tipoVoto::pointer>(fields[0]) = v; }

    class quantidadeVotos
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 9999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class quantidadeVotos

    quantidadeVotos::const_reference get_quantidadeVotos () const { return *static_cast<quantidadeVotos::const_pointer>(fields[1]); }
    quantidadeVotos::reference ref_quantidadeVotos () { return *static_cast<quantidadeVotos::pointer>(fields[1]); }
    quantidadeVotos::reference set_quantidadeVotos () { return *static_cast<quantidadeVotos::pointer>(fields[1]); }
    quantidadeVotos::reference set_quantidadeVotos (ASN1::INTEGER::int_type v)
    { return *static_cast<quantidadeVotos::pointer>(fields[1]) = v; }

    class identificacaoVotavel
    {
    public:
        typedef ModuloBoletimUrna::IdentificacaoVotavel value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class identificacaoVotavel

    identificacaoVotavel::const_reference get_identificacaoVotavel () const
    {
      assert(hasOptionalField(e_identificacaoVotavel));
      return *static_cast<identificacaoVotavel::const_pointer>(fields[2]);
    }
    identificacaoVotavel::reference ref_identificacaoVotavel ()
    {
      assert(hasOptionalField(e_identificacaoVotavel));
      return *static_cast<identificacaoVotavel::pointer>(fields[2]);
    }
    identificacaoVotavel::reference set_identificacaoVotavel ()
    {
      includeOptionalField( e_identificacaoVotavel, 2);
      return *static_cast<identificacaoVotavel::pointer>(fields[2]);
    }
    void omit_identificacaoVotavel () { removeOptionalField( e_identificacaoVotavel); }
    bool identificacaoVotavel_isPresent () const { return hasOptionalField( e_identificacaoVotavel); }

    class ordemGeracaoHash
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 9999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class ordemGeracaoHash

    ordemGeracaoHash::const_reference get_ordemGeracaoHash () const { return *static_cast<ordemGeracaoHash::const_pointer>(fields[3]); }
    ordemGeracaoHash::reference ref_ordemGeracaoHash () { return *static_cast<ordemGeracaoHash::pointer>(fields[3]); }
    ordemGeracaoHash::reference set_ordemGeracaoHash () { return *static_cast<ordemGeracaoHash::pointer>(fields[3]); }
    ordemGeracaoHash::reference set_ordemGeracaoHash (ASN1::INTEGER::int_type v)
    { return *static_cast<ordemGeracaoHash::pointer>(fields[3]) = v; }

    class hash
    {
    public:
        typedef ASN1::OCTET_STRING value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class hash

    hash::const_reference get_hash () const { return *static_cast<hash::const_pointer>(fields[4]); }
    hash::reference ref_hash () { return *static_cast<hash::pointer>(fields[4]); }
    hash::reference set_hash () { return *static_cast<hash::pointer>(fields[4]); }
    hash::reference set_hash (const ASN1::OCTET_STRING& v)
    { return *static_cast<hash::pointer>(fields[4]) = v; }
    void swap(TotalVotosVotavel& that) { Inherited::swap(that); }
    TotalVotosVotavel * clone() const
    { return static_cast<TotalVotosVotavel*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[5];
    static int fieldIds[5];
    static unsigned fieldTags[5];
    static const char* fieldNames[5];
}; // end class TotalVotosVotavel

//
// ModuloBoletimUrna::TotalVotosCargo
//

class TotalVotosCargo : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    TotalVotosCargo(const void* info) : Inherited(info) {}
public:
    TotalVotosCargo() : Inherited(&theInfo) {}
    TotalVotosCargo(const TotalVotosCargo & that) : Inherited(that) {}

    TotalVotosCargo& operator=(const TotalVotosCargo& that)
    { Inherited::operator=(that); return *this; }


    class codigoCargo
    {
    public:
        typedef ModuloTiposEleitorais::CodigoCargoConsulta value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class codigoCargo

    codigoCargo::const_reference get_codigoCargo () const { return *static_cast<codigoCargo::const_pointer>(fields[0]); }
    codigoCargo::reference ref_codigoCargo () { return *static_cast<codigoCargo::pointer>(fields[0]); }
    codigoCargo::reference set_codigoCargo () { return *static_cast<codigoCargo::pointer>(fields[0]); }

    class ordemImpressao
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 99> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class ordemImpressao

    ordemImpressao::const_reference get_ordemImpressao () const { return *static_cast<ordemImpressao::const_pointer>(fields[1]); }
    ordemImpressao::reference ref_ordemImpressao () { return *static_cast<ordemImpressao::pointer>(fields[1]); }
    ordemImpressao::reference set_ordemImpressao () { return *static_cast<ordemImpressao::pointer>(fields[1]); }
    ordemImpressao::reference set_ordemImpressao (ASN1::INTEGER::int_type v)
    { return *static_cast<ordemImpressao::pointer>(fields[1]) = v; }

    class votosVotaveis
    {
    public:
        //
        // ModuloBoletimUrna::TotalVotosCargo::votosVotaveis::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ModuloBoletimUrna::TotalVotosVotavel>
        {
            typedef ASN1::SEQUENCE_OF<ModuloBoletimUrna::TotalVotosVotavel> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ModuloBoletimUrna::TotalVotosVotavel& val) : Inherited(n, val) {}
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
    }; // end class votosVotaveis

    votosVotaveis::const_reference get_votosVotaveis () const { return *static_cast<votosVotaveis::const_pointer>(fields[2]); }
    votosVotaveis::reference ref_votosVotaveis () { return *static_cast<votosVotaveis::pointer>(fields[2]); }
    votosVotaveis::reference set_votosVotaveis () { return *static_cast<votosVotaveis::pointer>(fields[2]); }
    void swap(TotalVotosCargo& that) { Inherited::swap(that); }
    TotalVotosCargo * clone() const
    { return static_cast<TotalVotosCargo*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[3];
    static int fieldIds[3];
    static const char* fieldNames[3];
}; // end class TotalVotosCargo

//
// ModuloBoletimUrna::ResultadoVotacao
//

class ResultadoVotacao : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    ResultadoVotacao(const void* info) : Inherited(info) {}
public:
    ResultadoVotacao() : Inherited(&theInfo) {}
    ResultadoVotacao(const ResultadoVotacao & that) : Inherited(that) {}

    ResultadoVotacao& operator=(const ResultadoVotacao& that)
    { Inherited::operator=(that); return *this; }


    class tipoCargo
    {
    public:
        typedef ModuloTiposEleitorais::Tipo value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class tipoCargo

    tipoCargo::const_reference get_tipoCargo () const { return *static_cast<tipoCargo::const_pointer>(fields[0]); }
    tipoCargo::reference ref_tipoCargo () { return *static_cast<tipoCargo::pointer>(fields[0]); }
    tipoCargo::reference set_tipoCargo () { return *static_cast<tipoCargo::pointer>(fields[0]); }
    tipoCargo::reference set_tipoCargo (ModuloTiposEleitorais::Tipo::NamedNumber v)
    { return *static_cast<tipoCargo::pointer>(fields[0]) = v; }

    class qtdComparecimento
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 9999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class qtdComparecimento

    qtdComparecimento::const_reference get_qtdComparecimento () const { return *static_cast<qtdComparecimento::const_pointer>(fields[1]); }
    qtdComparecimento::reference ref_qtdComparecimento () { return *static_cast<qtdComparecimento::pointer>(fields[1]); }
    qtdComparecimento::reference set_qtdComparecimento () { return *static_cast<qtdComparecimento::pointer>(fields[1]); }
    qtdComparecimento::reference set_qtdComparecimento (ASN1::INTEGER::int_type v)
    { return *static_cast<qtdComparecimento::pointer>(fields[1]) = v; }

    class totaisVotosCargo
    {
    public:
        //
        // ModuloBoletimUrna::ResultadoVotacao::totaisVotosCargo::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ModuloBoletimUrna::TotalVotosCargo>
        {
            typedef ASN1::SEQUENCE_OF<ModuloBoletimUrna::TotalVotosCargo> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ModuloBoletimUrna::TotalVotosCargo& val) : Inherited(n, val) {}
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
    }; // end class totaisVotosCargo

    totaisVotosCargo::const_reference get_totaisVotosCargo () const { return *static_cast<totaisVotosCargo::const_pointer>(fields[2]); }
    totaisVotosCargo::reference ref_totaisVotosCargo () { return *static_cast<totaisVotosCargo::pointer>(fields[2]); }
    totaisVotosCargo::reference set_totaisVotosCargo () { return *static_cast<totaisVotosCargo::pointer>(fields[2]); }
    void swap(ResultadoVotacao& that) { Inherited::swap(that); }
    ResultadoVotacao * clone() const
    { return static_cast<ResultadoVotacao*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[3];
    static int fieldIds[3];
    static const char* fieldNames[3];
}; // end class ResultadoVotacao

//
// ModuloBoletimUrna::ResultadoVotacaoPorEleicao
//

class ResultadoVotacaoPorEleicao : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    ResultadoVotacaoPorEleicao(const void* info) : Inherited(info) {}
public:
    ResultadoVotacaoPorEleicao() : Inherited(&theInfo) {}
    ResultadoVotacaoPorEleicao(const ResultadoVotacaoPorEleicao & that) : Inherited(that) {}

    ResultadoVotacaoPorEleicao& operator=(const ResultadoVotacaoPorEleicao& that)
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

    class qtdEleitoresAptos
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 9999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class qtdEleitoresAptos

    qtdEleitoresAptos::const_reference get_qtdEleitoresAptos () const { return *static_cast<qtdEleitoresAptos::const_pointer>(fields[1]); }
    qtdEleitoresAptos::reference ref_qtdEleitoresAptos () { return *static_cast<qtdEleitoresAptos::pointer>(fields[1]); }
    qtdEleitoresAptos::reference set_qtdEleitoresAptos () { return *static_cast<qtdEleitoresAptos::pointer>(fields[1]); }
    qtdEleitoresAptos::reference set_qtdEleitoresAptos (ASN1::INTEGER::int_type v)
    { return *static_cast<qtdEleitoresAptos::pointer>(fields[1]) = v; }

    class qtdEleitoresAptosSecao
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 9999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class qtdEleitoresAptosSecao

    qtdEleitoresAptosSecao::const_reference get_qtdEleitoresAptosSecao () const { return *static_cast<qtdEleitoresAptosSecao::const_pointer>(fields[2]); }
    qtdEleitoresAptosSecao::reference ref_qtdEleitoresAptosSecao () { return *static_cast<qtdEleitoresAptosSecao::pointer>(fields[2]); }
    qtdEleitoresAptosSecao::reference set_qtdEleitoresAptosSecao () { return *static_cast<qtdEleitoresAptosSecao::pointer>(fields[2]); }
    qtdEleitoresAptosSecao::reference set_qtdEleitoresAptosSecao (ASN1::INTEGER::int_type v)
    { return *static_cast<qtdEleitoresAptosSecao::pointer>(fields[2]) = v; }

    class qtdEleitoresAptosTTE
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 9999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class qtdEleitoresAptosTTE

    qtdEleitoresAptosTTE::const_reference get_qtdEleitoresAptosTTE () const { return *static_cast<qtdEleitoresAptosTTE::const_pointer>(fields[3]); }
    qtdEleitoresAptosTTE::reference ref_qtdEleitoresAptosTTE () { return *static_cast<qtdEleitoresAptosTTE::pointer>(fields[3]); }
    qtdEleitoresAptosTTE::reference set_qtdEleitoresAptosTTE () { return *static_cast<qtdEleitoresAptosTTE::pointer>(fields[3]); }
    qtdEleitoresAptosTTE::reference set_qtdEleitoresAptosTTE (ASN1::INTEGER::int_type v)
    { return *static_cast<qtdEleitoresAptosTTE::pointer>(fields[3]) = v; }

    class resultadosVotacao
    {
    public:
        //
        // ModuloBoletimUrna::ResultadoVotacaoPorEleicao::resultadosVotacao::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ModuloBoletimUrna::ResultadoVotacao>
        {
            typedef ASN1::SEQUENCE_OF<ModuloBoletimUrna::ResultadoVotacao> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ModuloBoletimUrna::ResultadoVotacao& val) : Inherited(n, val) {}
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
    }; // end class resultadosVotacao

    resultadosVotacao::const_reference get_resultadosVotacao () const { return *static_cast<resultadosVotacao::const_pointer>(fields[4]); }
    resultadosVotacao::reference ref_resultadosVotacao () { return *static_cast<resultadosVotacao::pointer>(fields[4]); }
    resultadosVotacao::reference set_resultadosVotacao () { return *static_cast<resultadosVotacao::pointer>(fields[4]); }

    class ultimoHashVotosVotavel
    {
    public:
        typedef ASN1::OCTET_STRING value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class ultimoHashVotosVotavel

    ultimoHashVotosVotavel::const_reference get_ultimoHashVotosVotavel () const { return *static_cast<ultimoHashVotosVotavel::const_pointer>(fields[5]); }
    ultimoHashVotosVotavel::reference ref_ultimoHashVotosVotavel () { return *static_cast<ultimoHashVotosVotavel::pointer>(fields[5]); }
    ultimoHashVotosVotavel::reference set_ultimoHashVotosVotavel () { return *static_cast<ultimoHashVotosVotavel::pointer>(fields[5]); }
    ultimoHashVotosVotavel::reference set_ultimoHashVotosVotavel (const ASN1::OCTET_STRING& v)
    { return *static_cast<ultimoHashVotosVotavel::pointer>(fields[5]) = v; }

    class assinaturaUltimoHashVotosVotavel
    {
    public:
        typedef ASN1::OCTET_STRING value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class assinaturaUltimoHashVotosVotavel

    assinaturaUltimoHashVotosVotavel::const_reference get_assinaturaUltimoHashVotosVotavel () const { return *static_cast<assinaturaUltimoHashVotosVotavel::const_pointer>(fields[6]); }
    assinaturaUltimoHashVotosVotavel::reference ref_assinaturaUltimoHashVotosVotavel () { return *static_cast<assinaturaUltimoHashVotosVotavel::pointer>(fields[6]); }
    assinaturaUltimoHashVotosVotavel::reference set_assinaturaUltimoHashVotosVotavel () { return *static_cast<assinaturaUltimoHashVotosVotavel::pointer>(fields[6]); }
    assinaturaUltimoHashVotosVotavel::reference set_assinaturaUltimoHashVotosVotavel (const ASN1::OCTET_STRING& v)
    { return *static_cast<assinaturaUltimoHashVotosVotavel::pointer>(fields[6]) = v; }
    void swap(ResultadoVotacaoPorEleicao& that) { Inherited::swap(that); }
    ResultadoVotacaoPorEleicao * clone() const
    { return static_cast<ResultadoVotacaoPorEleicao*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[7];
    static int fieldIds[7];
    static const char* fieldNames[7];
}; // end class ResultadoVotacaoPorEleicao

//
// ModuloBoletimUrna::EntidadeBoletimUrna
//

class EntidadeBoletimUrna : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EntidadeBoletimUrna(const void* info) : Inherited(info) {}
public:
    EntidadeBoletimUrna() : Inherited(&theInfo) {}
    EntidadeBoletimUrna(const EntidadeBoletimUrna & that) : Inherited(that) {}

    EntidadeBoletimUrna& operator=(const EntidadeBoletimUrna& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_detalhamentoComparecimento,
      e_historicoVotoImpresso
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

    urna::const_reference get_urna () const { return *static_cast<urna::const_pointer>(fields[2]); }
    urna::reference ref_urna () { return *static_cast<urna::pointer>(fields[2]); }
    urna::reference set_urna () { return *static_cast<urna::pointer>(fields[2]); }

    class identificacaoSecao
    {
    public:
        typedef ModuloTiposEleitorais::IdentificacaoSecaoEleitoral value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class identificacaoSecao

    identificacaoSecao::const_reference get_identificacaoSecao () const { return *static_cast<identificacaoSecao::const_pointer>(fields[3]); }
    identificacaoSecao::reference ref_identificacaoSecao () { return *static_cast<identificacaoSecao::pointer>(fields[3]); }
    identificacaoSecao::reference set_identificacaoSecao () { return *static_cast<identificacaoSecao::pointer>(fields[3]); }

    class dataHoraEmissao
    {
    public:
        typedef ModuloTiposEleitorais::DataGeracao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class dataHoraEmissao

    dataHoraEmissao::const_reference get_dataHoraEmissao () const { return *static_cast<dataHoraEmissao::const_pointer>(fields[4]); }
    dataHoraEmissao::reference ref_dataHoraEmissao () { return *static_cast<dataHoraEmissao::pointer>(fields[4]); }
    dataHoraEmissao::reference set_dataHoraEmissao () { return *static_cast<dataHoraEmissao::pointer>(fields[4]); }
    dataHoraEmissao::reference set_dataHoraEmissao (const ASN1_STD string& v)
    { return *static_cast<dataHoraEmissao::pointer>(fields[4]) = v; }

    class dadosSecaoSA
    {
    public:
        //
        // ModuloBoletimUrna::EntidadeBoletimUrna::dadosSecaoSA::value_type
        //

        class value_type : public ASN1::CHOICE
        {
            typedef ASN1::CHOICE Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type() : Inherited(&theInfo) {}
            enum Choice_Ids {
              unknownSelection_ = -2,
              unselected_ = -1,
              dadosSecao0 = 0,
              dadosSA1 = 1
            };

            class dadosSecao
            {
            public:
                enum Id { eid = 0 };
                typedef ModuloBoletimUrna::DadosSecao value_type;
                typedef value_type&        reference;
                typedef const value_type&  const_reference;
                typedef value_type*        pointer;
                typedef const value_type*  const_pointer;
            }; // end class dadosSecao

            dadosSecao::const_reference get_dadosSecao () const
            {
                assert(currentSelection() == dadosSecao::eid);
                return *static_cast<dadosSecao::const_pointer>(choice.get());
            }
            dadosSecao::reference ref_dadosSecao ()
            {
                assert(currentSelection() == dadosSecao::eid);
                return *static_cast<dadosSecao::pointer>(choice.get());
            }
            dadosSecao::reference select_dadosSecao ()
            {
                return *static_cast<dadosSecao::pointer>(setSelection(dadosSecao::eid, ASN1::AbstractData::create(&dadosSecao::value_type::theInfo)));
            }
            bool dadosSecao_isSelected () const { return currentSelection() == dadosSecao::eid; }

            class dadosSA
            {
            public:
                enum Id { eid = 1 };
                typedef ModuloBoletimUrna::DadosSA value_type;
                typedef value_type&        reference;
                typedef const value_type&  const_reference;
                typedef value_type*        pointer;
                typedef const value_type*  const_pointer;
            }; // end class dadosSA

            dadosSA::const_reference get_dadosSA () const
            {
                assert(currentSelection() == dadosSA::eid);
                return *static_cast<dadosSA::const_pointer>(choice.get());
            }
            dadosSA::reference ref_dadosSA ()
            {
                assert(currentSelection() == dadosSA::eid);
                return *static_cast<dadosSA::pointer>(choice.get());
            }
            dadosSA::reference select_dadosSA ()
            {
                return *static_cast<dadosSA::pointer>(setSelection(dadosSA::eid, ASN1::AbstractData::create(&dadosSA::value_type::theInfo)));
            }
            bool dadosSA_isSelected () const { return currentSelection() == dadosSA::eid; }

            value_type(const value_type & that) : Inherited(that) {}
            value_type & operator=(const value_type & that)
            { Inherited::operator=(that); return *this; }
            void swap(value_type & that) { Inherited::swap(that); }
            value_type * clone() const
            { return static_cast<value_type*> (Inherited::clone()); }
            static bool equal_type(const ASN1::AbstractData& type)
            { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
            static const InfoType theInfo;
        private:
            static const void* selectionInfos[2];
            static const char* selectionNames[2];
        }; // end class value_type

        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class dadosSecaoSA

    dadosSecaoSA::const_reference get_dadosSecaoSA () const { return *static_cast<dadosSecaoSA::const_pointer>(fields[5]); }
    dadosSecaoSA::reference ref_dadosSecaoSA () { return *static_cast<dadosSecaoSA::pointer>(fields[5]); }
    dadosSecaoSA::reference set_dadosSecaoSA () { return *static_cast<dadosSecaoSA::pointer>(fields[5]); }

    class qtdEleitoresCompareceram
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 9999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class qtdEleitoresCompareceram

    qtdEleitoresCompareceram::const_reference get_qtdEleitoresCompareceram () const { return *static_cast<qtdEleitoresCompareceram::const_pointer>(fields[6]); }
    qtdEleitoresCompareceram::reference ref_qtdEleitoresCompareceram () { return *static_cast<qtdEleitoresCompareceram::pointer>(fields[6]); }
    qtdEleitoresCompareceram::reference set_qtdEleitoresCompareceram () { return *static_cast<qtdEleitoresCompareceram::pointer>(fields[6]); }
    qtdEleitoresCompareceram::reference set_qtdEleitoresCompareceram (ASN1::INTEGER::int_type v)
    { return *static_cast<qtdEleitoresCompareceram::pointer>(fields[6]) = v; }

    class detalhamentoComparecimento
    {
    public:
        typedef ModuloBoletimUrna::DetalhamentoComparecimento value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class detalhamentoComparecimento

    detalhamentoComparecimento::const_reference get_detalhamentoComparecimento () const
    {
      assert(hasOptionalField(e_detalhamentoComparecimento));
      return *static_cast<detalhamentoComparecimento::const_pointer>(fields[7]);
    }
    detalhamentoComparecimento::reference ref_detalhamentoComparecimento ()
    {
      assert(hasOptionalField(e_detalhamentoComparecimento));
      return *static_cast<detalhamentoComparecimento::pointer>(fields[7]);
    }
    detalhamentoComparecimento::reference set_detalhamentoComparecimento ()
    {
      includeOptionalField( e_detalhamentoComparecimento, 7);
      return *static_cast<detalhamentoComparecimento::pointer>(fields[7]);
    }
    void omit_detalhamentoComparecimento () { removeOptionalField( e_detalhamentoComparecimento); }
    bool detalhamentoComparecimento_isPresent () const { return hasOptionalField( e_detalhamentoComparecimento); }

    class resultadosVotacaoPorEleicao
    {
    public:
        //
        // ModuloBoletimUrna::EntidadeBoletimUrna::resultadosVotacaoPorEleicao::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ModuloBoletimUrna::ResultadoVotacaoPorEleicao>
        {
            typedef ASN1::SEQUENCE_OF<ModuloBoletimUrna::ResultadoVotacaoPorEleicao> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ModuloBoletimUrna::ResultadoVotacaoPorEleicao& val) : Inherited(n, val) {}
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
    }; // end class resultadosVotacaoPorEleicao

    resultadosVotacaoPorEleicao::const_reference get_resultadosVotacaoPorEleicao () const { return *static_cast<resultadosVotacaoPorEleicao::const_pointer>(fields[8]); }
    resultadosVotacaoPorEleicao::reference ref_resultadosVotacaoPorEleicao () { return *static_cast<resultadosVotacaoPorEleicao::pointer>(fields[8]); }
    resultadosVotacaoPorEleicao::reference set_resultadosVotacaoPorEleicao () { return *static_cast<resultadosVotacaoPorEleicao::pointer>(fields[8]); }

    class historicoCodigosCarga
    {
    public:
        typedef ModuloRegistroDigitalVoto::HistoricoCodigosCarga value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class historicoCodigosCarga

    historicoCodigosCarga::const_reference get_historicoCodigosCarga () const { return *static_cast<historicoCodigosCarga::const_pointer>(fields[9]); }
    historicoCodigosCarga::reference ref_historicoCodigosCarga () { return *static_cast<historicoCodigosCarga::pointer>(fields[9]); }
    historicoCodigosCarga::reference set_historicoCodigosCarga () { return *static_cast<historicoCodigosCarga::pointer>(fields[9]); }

    class historicoVotoImpresso
    {
    public:
        //
        // ModuloBoletimUrna::EntidadeBoletimUrna::historicoVotoImpresso::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ModuloBoletimUrna::HistoricoVotoImpresso>
        {
            typedef ASN1::SEQUENCE_OF<ModuloBoletimUrna::HistoricoVotoImpresso> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ModuloBoletimUrna::HistoricoVotoImpresso& val) : Inherited(n, val) {}
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
    }; // end class historicoVotoImpresso

    historicoVotoImpresso::const_reference get_historicoVotoImpresso () const
    {
      assert(hasOptionalField(e_historicoVotoImpresso));
      return *static_cast<historicoVotoImpresso::const_pointer>(fields[10]);
    }
    historicoVotoImpresso::reference ref_historicoVotoImpresso ()
    {
      assert(hasOptionalField(e_historicoVotoImpresso));
      return *static_cast<historicoVotoImpresso::pointer>(fields[10]);
    }
    historicoVotoImpresso::reference set_historicoVotoImpresso ()
    {
      includeOptionalField( e_historicoVotoImpresso, 10);
      return *static_cast<historicoVotoImpresso::pointer>(fields[10]);
    }
    void omit_historicoVotoImpresso () { removeOptionalField( e_historicoVotoImpresso); }
    bool historicoVotoImpresso_isPresent () const { return hasOptionalField( e_historicoVotoImpresso); }
    void swap(EntidadeBoletimUrna& that) { Inherited::swap(that); }
    EntidadeBoletimUrna * clone() const
    { return static_cast<EntidadeBoletimUrna*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[11];
    static int fieldIds[11];
    static unsigned fieldTags[11];
    static const char* fieldNames[11];
}; // end class EntidadeBoletimUrna

} // namespace ModuloBoletimUrna

#endif // __MODULOBOLETIMURNA_H
