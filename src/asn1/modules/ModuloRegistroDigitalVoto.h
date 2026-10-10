#ifndef __MODULOREGISTRODIGITALVOTO_H
#define __MODULOREGISTRODIGITALVOTO_H

#include "asn1.h"

#include "ModuloTiposEleitorais.h"
#include "ModuloTiposResultadosEcoUrna.h"

namespace ModuloRegistroDigitalVoto {

//
// ModuloRegistroDigitalVoto::TipoVoto
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
        legenda = 1,
        nominal,
        branco,
        nulo,
        brancoAposSuspensao,
        nuloAposSuspensao,
        nuloPorRepeticao,
        nuloCargoSemCandidato,
        nuloAposSuspensaoCargoSemCandidato
    };

    bool is_legenda() const { return value == legenda; }
    void set_legenda() { value = legenda; }

    bool is_nominal() const { return value == nominal; }
    void set_nominal() { value = nominal; }

    bool is_branco() const { return value == branco; }
    void set_branco() { value = branco; }

    bool is_nulo() const { return value == nulo; }
    void set_nulo() { value = nulo; }

    bool is_brancoAposSuspensao() const { return value == brancoAposSuspensao; }
    void set_brancoAposSuspensao() { value = brancoAposSuspensao; }

    bool is_nuloAposSuspensao() const { return value == nuloAposSuspensao; }
    void set_nuloAposSuspensao() { value = nuloAposSuspensao; }

    bool is_nuloPorRepeticao() const { return value == nuloPorRepeticao; }
    void set_nuloPorRepeticao() { value = nuloPorRepeticao; }

    bool is_nuloCargoSemCandidato() const { return value == nuloCargoSemCandidato; }
    void set_nuloCargoSemCandidato() { value = nuloCargoSemCandidato; }

    bool is_nuloAposSuspensaoCargoSemCandidato() const { return value == nuloAposSuspensaoCargoSemCandidato; }
    void set_nuloAposSuspensaoCargoSemCandidato() { value = nuloAposSuspensaoCargoSemCandidato; }

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
// ModuloRegistroDigitalVoto::Voto
//

class Voto : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    Voto(const void* info) : Inherited(info) {}
public:
    Voto() : Inherited(&theInfo) {}
    Voto(const Voto & that) : Inherited(that) {}

    Voto& operator=(const Voto& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_digitacao
    };

    class tipoVoto
    {
    public:
        typedef ModuloRegistroDigitalVoto::TipoVoto value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class tipoVoto

    tipoVoto::const_reference get_tipoVoto () const { return *static_cast<tipoVoto::const_pointer>(fields[0]); }
    tipoVoto::reference ref_tipoVoto () { return *static_cast<tipoVoto::pointer>(fields[0]); }
    tipoVoto::reference set_tipoVoto () { return *static_cast<tipoVoto::pointer>(fields[0]); }
    tipoVoto::reference set_tipoVoto (ModuloRegistroDigitalVoto::TipoVoto::NamedNumber v)
    { return *static_cast<tipoVoto::pointer>(fields[0]) = v; }

    class digitacao
    {
    public:
        //
        // ModuloRegistroDigitalVoto::Voto::digitacao::value_type
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
    }; // end class digitacao

    digitacao::const_reference get_digitacao () const
    {
      assert(hasOptionalField(e_digitacao));
      return *static_cast<digitacao::const_pointer>(fields[1]);
    }
    digitacao::reference ref_digitacao ()
    {
      assert(hasOptionalField(e_digitacao));
      return *static_cast<digitacao::pointer>(fields[1]);
    }
    digitacao::reference set_digitacao ()
    {
      includeOptionalField( e_digitacao, 1);
      return *static_cast<digitacao::pointer>(fields[1]);
    }
    digitacao::reference set_digitacao (const ASN1_STD string& v)
    {
      includeOptionalField( e_digitacao, 1);
      return *static_cast<digitacao::pointer>(fields[1]) = v;
    }
    void omit_digitacao () { removeOptionalField( e_digitacao); }
    bool digitacao_isPresent () const { return hasOptionalField( e_digitacao); }
    void swap(Voto& that) { Inherited::swap(that); }
    Voto * clone() const
    { return static_cast<Voto*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[2];
    static int fieldIds[2];
    static const char* fieldNames[2];
}; // end class Voto

//
// ModuloRegistroDigitalVoto::VotosCargo
//

class VotosCargo : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    VotosCargo(const void* info) : Inherited(info) {}
public:
    VotosCargo() : Inherited(&theInfo) {}
    VotosCargo(const VotosCargo & that) : Inherited(that) {}

    VotosCargo& operator=(const VotosCargo& that)
    { Inherited::operator=(that); return *this; }


    class idCargo
    {
    public:
        typedef ModuloTiposEleitorais::CodigoCargoConsulta value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class idCargo

    idCargo::const_reference get_idCargo () const { return *static_cast<idCargo::const_pointer>(fields[0]); }
    idCargo::reference ref_idCargo () { return *static_cast<idCargo::pointer>(fields[0]); }
    idCargo::reference set_idCargo () { return *static_cast<idCargo::pointer>(fields[0]); }

    class quantidadeEscolhas
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 50> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class quantidadeEscolhas

    quantidadeEscolhas::const_reference get_quantidadeEscolhas () const { return *static_cast<quantidadeEscolhas::const_pointer>(fields[1]); }
    quantidadeEscolhas::reference ref_quantidadeEscolhas () { return *static_cast<quantidadeEscolhas::pointer>(fields[1]); }
    quantidadeEscolhas::reference set_quantidadeEscolhas () { return *static_cast<quantidadeEscolhas::pointer>(fields[1]); }
    quantidadeEscolhas::reference set_quantidadeEscolhas (ASN1::INTEGER::int_type v)
    { return *static_cast<quantidadeEscolhas::pointer>(fields[1]) = v; }

    class votos
    {
    public:
        //
        // ModuloRegistroDigitalVoto::VotosCargo::votos::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ModuloRegistroDigitalVoto::Voto>
        {
            typedef ASN1::SEQUENCE_OF<ModuloRegistroDigitalVoto::Voto> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ModuloRegistroDigitalVoto::Voto& val) : Inherited(n, val) {}
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
    }; // end class votos

    votos::const_reference get_votos () const { return *static_cast<votos::const_pointer>(fields[2]); }
    votos::reference ref_votos () { return *static_cast<votos::pointer>(fields[2]); }
    votos::reference set_votos () { return *static_cast<votos::pointer>(fields[2]); }
    void swap(VotosCargo& that) { Inherited::swap(that); }
    VotosCargo * clone() const
    { return static_cast<VotosCargo*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[3];
    static int fieldIds[3];
    static const char* fieldNames[3];
}; // end class VotosCargo

//
// ModuloRegistroDigitalVoto::VotosCargos   // SYNTHESIZED name (field ['votosCargos', 'votosCargos'])
//

class VotosCargos : public ASN1::SEQUENCE_OF<ModuloRegistroDigitalVoto::VotosCargo>
{
    typedef ASN1::SEQUENCE_OF<ModuloRegistroDigitalVoto::VotosCargo> Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    VotosCargos(const void* info) : Inherited(info) {}
public:
    VotosCargos(size_type n=0) : Inherited(n) {}
    VotosCargos(size_type n, const ModuloRegistroDigitalVoto::VotosCargo& val) : Inherited(n, val) {}
    VotosCargos(const_iterator first, const_iterator last) : Inherited(first, last) {}
    VotosCargos(const VotosCargos& that) : Inherited(that) {}
    VotosCargos * clone() const
    { return static_cast<VotosCargos*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
}; // end class VotosCargos

//
// ModuloRegistroDigitalVoto::EleicaoVota
//

class EleicaoVota : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EleicaoVota(const void* info) : Inherited(info) {}
public:
    EleicaoVota() : Inherited(&theInfo) {}
    EleicaoVota(const EleicaoVota & that) : Inherited(that) {}

    EleicaoVota& operator=(const EleicaoVota& that)
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

    class votosCargos
    {
    public:
        typedef ModuloRegistroDigitalVoto::VotosCargos value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class votosCargos

    votosCargos::const_reference get_votosCargos () const { return *static_cast<votosCargos::const_pointer>(fields[1]); }
    votosCargos::reference ref_votosCargos () { return *static_cast<votosCargos::pointer>(fields[1]); }
    votosCargos::reference set_votosCargos () { return *static_cast<votosCargos::pointer>(fields[1]); }
    void swap(EleicaoVota& that) { Inherited::swap(that); }
    EleicaoVota * clone() const
    { return static_cast<EleicaoVota*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[2];
    static int fieldIds[2];
    static const char* fieldNames[2];
}; // end class EleicaoVota

//
// ModuloRegistroDigitalVoto::EleicaoSA
//

class EleicaoSA : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EleicaoSA(const void* info) : Inherited(info) {}
public:
    EleicaoSA() : Inherited(&theInfo) {}
    EleicaoSA(const EleicaoSA & that) : Inherited(that) {}

    EleicaoSA& operator=(const EleicaoSA& that)
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

    class tipoCedulaSA
    {
    public:
        //
        // ModuloRegistroDigitalVoto::EleicaoSA::tipoCedulaSA::value_type
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
                majoritario = 1,
                proporcional
            };

            bool is_majoritario() const { return value == majoritario; }
            void set_majoritario() { value = majoritario; }

            bool is_proporcional() const { return value == proporcional; }
            void set_proporcional() { value = proporcional; }

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
    }; // end class tipoCedulaSA

    tipoCedulaSA::const_reference get_tipoCedulaSA () const { return *static_cast<tipoCedulaSA::const_pointer>(fields[1]); }
    tipoCedulaSA::reference ref_tipoCedulaSA () { return *static_cast<tipoCedulaSA::pointer>(fields[1]); }
    tipoCedulaSA::reference set_tipoCedulaSA () { return *static_cast<tipoCedulaSA::pointer>(fields[1]); }
    tipoCedulaSA::reference set_tipoCedulaSA (tipoCedulaSA::value_type::NamedNumber v)
    { return *static_cast<tipoCedulaSA::pointer>(fields[1]) = v; }

    class origemVotosSA
    {
    public:
        //
        // ModuloRegistroDigitalVoto::EleicaoSA::origemVotosSA::value_type
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
                cedula = 1,
                rdv,
                bu
            };

            bool is_cedula() const { return value == cedula; }
            void set_cedula() { value = cedula; }

            bool is_rdv() const { return value == rdv; }
            void set_rdv() { value = rdv; }

            bool is_bu() const { return value == bu; }
            void set_bu() { value = bu; }

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
    }; // end class origemVotosSA

    origemVotosSA::const_reference get_origemVotosSA () const { return *static_cast<origemVotosSA::const_pointer>(fields[2]); }
    origemVotosSA::reference ref_origemVotosSA () { return *static_cast<origemVotosSA::pointer>(fields[2]); }
    origemVotosSA::reference set_origemVotosSA () { return *static_cast<origemVotosSA::pointer>(fields[2]); }
    origemVotosSA::reference set_origemVotosSA (origemVotosSA::value_type::NamedNumber v)
    { return *static_cast<origemVotosSA::pointer>(fields[2]) = v; }

    class votosCargos
    {
    public:
        typedef ModuloRegistroDigitalVoto::VotosCargos value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class votosCargos

    votosCargos::const_reference get_votosCargos () const { return *static_cast<votosCargos::const_pointer>(fields[3]); }
    votosCargos::reference ref_votosCargos () { return *static_cast<votosCargos::pointer>(fields[3]); }
    votosCargos::reference set_votosCargos () { return *static_cast<votosCargos::pointer>(fields[3]); }
    void swap(EleicaoSA& that) { Inherited::swap(that); }
    EleicaoSA * clone() const
    { return static_cast<EleicaoSA*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[4];
    static int fieldIds[4];
    static const char* fieldNames[4];
}; // end class EleicaoSA

//
// ModuloRegistroDigitalVoto::Eleicoes
//

class Eleicoes : public ASN1::CHOICE
{
    typedef ASN1::CHOICE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    Eleicoes(const void* info) : Inherited(info) {}
public:
    Eleicoes() : Inherited(&theInfo) {}
    enum Choice_Ids {
      unknownSelection_ = -2,
      unselected_ = -1,
      eleicoesVota0 = 0,
      eleicoesSA1 = 1
    };

    class eleicoesVota
    {
    public:
        enum Id { eid = 0 };
        //
        // ModuloRegistroDigitalVoto::Eleicoes::eleicoesVota::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ModuloRegistroDigitalVoto::EleicaoVota>
        {
            typedef ASN1::SEQUENCE_OF<ModuloRegistroDigitalVoto::EleicaoVota> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ModuloRegistroDigitalVoto::EleicaoVota& val) : Inherited(n, val) {}
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
    }; // end class eleicoesVota

    eleicoesVota::const_reference get_eleicoesVota () const
    {
        assert(currentSelection() == eleicoesVota::eid);
        return *static_cast<eleicoesVota::const_pointer>(choice.get());
    }
    eleicoesVota::reference ref_eleicoesVota ()
    {
        assert(currentSelection() == eleicoesVota::eid);
        return *static_cast<eleicoesVota::pointer>(choice.get());
    }
    eleicoesVota::reference select_eleicoesVota ()
    {
        return *static_cast<eleicoesVota::pointer>(setSelection(eleicoesVota::eid, ASN1::AbstractData::create(&eleicoesVota::value_type::theInfo)));
    }
    bool eleicoesVota_isSelected () const { return currentSelection() == eleicoesVota::eid; }

    class eleicoesSA
    {
    public:
        enum Id { eid = 1 };
        //
        // ModuloRegistroDigitalVoto::Eleicoes::eleicoesSA::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ModuloRegistroDigitalVoto::EleicaoSA>
        {
            typedef ASN1::SEQUENCE_OF<ModuloRegistroDigitalVoto::EleicaoSA> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ModuloRegistroDigitalVoto::EleicaoSA& val) : Inherited(n, val) {}
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
    }; // end class eleicoesSA

    eleicoesSA::const_reference get_eleicoesSA () const
    {
        assert(currentSelection() == eleicoesSA::eid);
        return *static_cast<eleicoesSA::const_pointer>(choice.get());
    }
    eleicoesSA::reference ref_eleicoesSA ()
    {
        assert(currentSelection() == eleicoesSA::eid);
        return *static_cast<eleicoesSA::pointer>(choice.get());
    }
    eleicoesSA::reference select_eleicoesSA ()
    {
        return *static_cast<eleicoesSA::pointer>(setSelection(eleicoesSA::eid, ASN1::AbstractData::create(&eleicoesSA::value_type::theInfo)));
    }
    bool eleicoesSA_isSelected () const { return currentSelection() == eleicoesSA::eid; }

    Eleicoes(const Eleicoes & that) : Inherited(that) {}
    Eleicoes & operator=(const Eleicoes & that)
    { Inherited::operator=(that); return *this; }
    void swap(Eleicoes & that) { Inherited::swap(that); }
    Eleicoes * clone() const
    { return static_cast<Eleicoes*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const void* selectionInfos[2];
    static unsigned selectionTags[2];
    static const char* selectionNames[2];
}; // end class Eleicoes

//
// ModuloRegistroDigitalVoto::HistoricoCodigosCarga   // SYNTHESIZED name (field ['historicoCodigosCarga', 'historicoCodigosCarga'])
//

class HistoricoCodigosCarga : public ASN1::SEQUENCE_OF<ASN1::GeneralString>
{
    typedef ASN1::SEQUENCE_OF<ASN1::GeneralString> Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    HistoricoCodigosCarga(const void* info) : Inherited(info) {}
public:
    HistoricoCodigosCarga(size_type n=0) : Inherited(n) {}
    HistoricoCodigosCarga(size_type n, const ASN1::GeneralString& val) : Inherited(n, val) {}
    HistoricoCodigosCarga(const_iterator first, const_iterator last) : Inherited(first, last) {}
    HistoricoCodigosCarga(const HistoricoCodigosCarga& that) : Inherited(that) {}
    HistoricoCodigosCarga * clone() const
    { return static_cast<HistoricoCodigosCarga*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
}; // end class HistoricoCodigosCarga

//
// ModuloRegistroDigitalVoto::EntidadeRegistroDigitalVoto
//

class EntidadeRegistroDigitalVoto : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EntidadeRegistroDigitalVoto(const void* info) : Inherited(info) {}
public:
    EntidadeRegistroDigitalVoto() : Inherited(&theInfo) {}
    EntidadeRegistroDigitalVoto(const EntidadeRegistroDigitalVoto & that) : Inherited(that) {}

    EntidadeRegistroDigitalVoto& operator=(const EntidadeRegistroDigitalVoto& that)
    { Inherited::operator=(that); return *this; }


    class pleito
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 99999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class pleito

    pleito::const_reference get_pleito () const { return *static_cast<pleito::const_pointer>(fields[0]); }
    pleito::reference ref_pleito () { return *static_cast<pleito::pointer>(fields[0]); }
    pleito::reference set_pleito () { return *static_cast<pleito::pointer>(fields[0]); }
    pleito::reference set_pleito (ASN1::INTEGER::int_type v)
    { return *static_cast<pleito::pointer>(fields[0]) = v; }

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

    class identificacao
    {
    public:
        typedef ModuloTiposEleitorais::IdentificacaoSecaoEleitoral value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class identificacao

    identificacao::const_reference get_identificacao () const { return *static_cast<identificacao::const_pointer>(fields[2]); }
    identificacao::reference ref_identificacao () { return *static_cast<identificacao::pointer>(fields[2]); }
    identificacao::reference set_identificacao () { return *static_cast<identificacao::pointer>(fields[2]); }

    class historicoCodigosCarga
    {
    public:
        typedef ModuloRegistroDigitalVoto::HistoricoCodigosCarga value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class historicoCodigosCarga

    historicoCodigosCarga::const_reference get_historicoCodigosCarga () const { return *static_cast<historicoCodigosCarga::const_pointer>(fields[3]); }
    historicoCodigosCarga::reference ref_historicoCodigosCarga () { return *static_cast<historicoCodigosCarga::pointer>(fields[3]); }
    historicoCodigosCarga::reference set_historicoCodigosCarga () { return *static_cast<historicoCodigosCarga::pointer>(fields[3]); }

    class eleicoes
    {
    public:
        typedef ModuloRegistroDigitalVoto::Eleicoes value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class eleicoes

    eleicoes::const_reference get_eleicoes () const { return *static_cast<eleicoes::const_pointer>(fields[4]); }
    eleicoes::reference ref_eleicoes () { return *static_cast<eleicoes::pointer>(fields[4]); }
    eleicoes::reference set_eleicoes () { return *static_cast<eleicoes::pointer>(fields[4]); }
    void swap(EntidadeRegistroDigitalVoto& that) { Inherited::swap(that); }
    EntidadeRegistroDigitalVoto * clone() const
    { return static_cast<EntidadeRegistroDigitalVoto*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[5];
    static int fieldIds[5];
    static const char* fieldNames[5];
}; // end class EntidadeRegistroDigitalVoto

//
// ModuloRegistroDigitalVoto::EntidadeResultadoRDV
//

class EntidadeResultadoRDV : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EntidadeResultadoRDV(const void* info) : Inherited(info) {}
public:
    EntidadeResultadoRDV() : Inherited(&theInfo) {}
    EntidadeResultadoRDV(const EntidadeResultadoRDV & that) : Inherited(that) {}

    EntidadeResultadoRDV& operator=(const EntidadeResultadoRDV& that)
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

    class urna
    {
    public:
        typedef ModuloTiposResultadosEcoUrna::Urna value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class urna

    urna::const_reference get_urna () const { return *static_cast<urna::const_pointer>(fields[1]); }
    urna::reference ref_urna () { return *static_cast<urna::pointer>(fields[1]); }
    urna::reference set_urna () { return *static_cast<urna::pointer>(fields[1]); }

    class rdv
    {
    public:
        typedef ModuloRegistroDigitalVoto::EntidadeRegistroDigitalVoto value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class rdv

    rdv::const_reference get_rdv () const { return *static_cast<rdv::const_pointer>(fields[2]); }
    rdv::reference ref_rdv () { return *static_cast<rdv::pointer>(fields[2]); }
    rdv::reference set_rdv () { return *static_cast<rdv::pointer>(fields[2]); }
    void swap(EntidadeResultadoRDV& that) { Inherited::swap(that); }
    EntidadeResultadoRDV * clone() const
    { return static_cast<EntidadeResultadoRDV*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[3];
    static int fieldIds[3];
    static const char* fieldNames[3];
}; // end class EntidadeResultadoRDV

} // namespace ModuloRegistroDigitalVoto

#endif // __MODULOREGISTRODIGITALVOTO_H
