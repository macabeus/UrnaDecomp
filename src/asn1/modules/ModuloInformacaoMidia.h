#ifndef __MODULOINFORMACAOMIDIA_H
#define __MODULOINFORMACAOMIDIA_H

#include "asn1.h"

#include "ModuloTiposEcoUrna.h"
#include "ModuloTiposEleitorais.h"

namespace ModuloInformacaoMidia {

//
// ModuloInformacaoMidia::TipoMidia
//

class TipoMidia : public ASN1::ENUMERATED
{
    typedef ASN1::ENUMERATED Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    TipoMidia(const void* info) : Inherited(info) {}
public:
    TipoMidia() : Inherited(&theInfo) {}
    enum NamedNumber {
        unknownEnumeration_ = -1,
        mr = 1,
        fc,
        fv
    };

    bool is_mr() const { return value == mr; }
    void set_mr() { value = mr; }

    bool is_fc() const { return value == fc; }
    void set_fc() { value = fc; }

    bool is_fv() const { return value == fv; }
    void set_fv() { value = fv; }

    TipoMidia(const NamedNumber v) : Inherited(&theInfo) { setFromInt(v); }
    TipoMidia & operator=(const NamedNumber v) { setFromInt(v);  return *this; }
    operator NamedNumber() const { return NamedNumber(asInt()); }

    bool operator == (NamedNumber rhs) const { return value == rhs; }
    bool operator != (NamedNumber rhs) const { return value != rhs; }
    bool operator <  (NamedNumber rhs) const { return value <  rhs; }
    bool operator >  (NamedNumber rhs) const { return value >  rhs; }
    bool operator <= (NamedNumber rhs) const { return value <= rhs; }
    bool operator >= (NamedNumber rhs) const { return value >= rhs; }
    bool operator == (const TipoMidia &rhs) const { return value == rhs.value; }
    bool operator != (const TipoMidia &rhs) const { return value != rhs.value; }

    void swap (TipoMidia & that) { Inherited::swap(that); }
    TipoMidia * clone() const
    { return static_cast<TipoMidia*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const char * nameList[];
}; // end class TipoMidia

//
// ModuloInformacaoMidia::DadosGeracaoMidia
//

class DadosGeracaoMidia : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    DadosGeracaoMidia(const void* info) : Inherited(info) {}
public:
    DadosGeracaoMidia() : Inherited(&theInfo) {}
    DadosGeracaoMidia(const DadosGeracaoMidia & that) : Inherited(that) {}

    DadosGeracaoMidia& operator=(const DadosGeracaoMidia& that)
    { Inherited::operator=(that); return *this; }


    class serialMidia
    {
    public:
        typedef ASN1::GeneralString value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class serialMidia

    serialMidia::const_reference get_serialMidia () const { return *static_cast<serialMidia::const_pointer>(fields[0]); }
    serialMidia::reference ref_serialMidia () { return *static_cast<serialMidia::pointer>(fields[0]); }
    serialMidia::reference set_serialMidia () { return *static_cast<serialMidia::pointer>(fields[0]); }
    serialMidia::reference set_serialMidia (const ASN1_STD string& v)
    { return *static_cast<serialMidia::pointer>(fields[0]) = v; }

    class usuario
    {
    public:
        typedef ASN1::GeneralString value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class usuario

    usuario::const_reference get_usuario () const { return *static_cast<usuario::const_pointer>(fields[1]); }
    usuario::reference ref_usuario () { return *static_cast<usuario::pointer>(fields[1]); }
    usuario::reference set_usuario () { return *static_cast<usuario::pointer>(fields[1]); }
    usuario::reference set_usuario (const ASN1_STD string& v)
    { return *static_cast<usuario::pointer>(fields[1]) = v; }

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
    identificadorGeradorMidia::reference set_identificadorGeradorMidia (identificadorGeradorMidia::const_reference v)
    { return *static_cast<identificadorGeradorMidia::pointer>(fields[2]) = v; }

    class data
    {
    public:
        typedef ModuloTiposEleitorais::DataGeracao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class data

    data::const_reference get_data () const { return *static_cast<data::const_pointer>(fields[3]); }
    data::reference ref_data () { return *static_cast<data::pointer>(fields[3]); }
    data::reference set_data () { return *static_cast<data::pointer>(fields[3]); }
    data::reference set_data (data::const_reference v)
    { return *static_cast<data::pointer>(fields[3]) = v; }
    void swap(DadosGeracaoMidia& that) { Inherited::swap(that); }
    DadosGeracaoMidia * clone() const
    { return static_cast<DadosGeracaoMidia*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[4];
    static int fieldIds[4];
    static const char* fieldNames[4];
}; // end class DadosGeracaoMidia

//
// ModuloInformacaoMidia::TipoAplicativo
//

class TipoAplicativo : public ASN1::ENUMERATED
{
    typedef ASN1::ENUMERATED Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    TipoAplicativo(const void* info) : Inherited(info) {}
public:
    TipoAplicativo() : Inherited(&theInfo) {}
    enum NamedNumber {
        unknownEnumeration_ = -1,
        vota = 1,
        sa,
        red,
        vpp,
        ste,
        adh,
        atue
    };

    bool is_vota() const { return value == vota; }
    void set_vota() { value = vota; }

    bool is_sa() const { return value == sa; }
    void set_sa() { value = sa; }

    bool is_red() const { return value == red; }
    void set_red() { value = red; }

    bool is_vpp() const { return value == vpp; }
    void set_vpp() { value = vpp; }

    bool is_ste() const { return value == ste; }
    void set_ste() { value = ste; }

    bool is_adh() const { return value == adh; }
    void set_adh() { value = adh; }

    bool is_atue() const { return value == atue; }
    void set_atue() { value = atue; }

    TipoAplicativo(const NamedNumber v) : Inherited(&theInfo) { setFromInt(v); }
    TipoAplicativo & operator=(const NamedNumber v) { setFromInt(v);  return *this; }
    operator NamedNumber() const { return NamedNumber(asInt()); }

    bool operator == (NamedNumber rhs) const { return value == rhs; }
    bool operator != (NamedNumber rhs) const { return value != rhs; }
    bool operator <  (NamedNumber rhs) const { return value <  rhs; }
    bool operator >  (NamedNumber rhs) const { return value >  rhs; }
    bool operator <= (NamedNumber rhs) const { return value <= rhs; }
    bool operator >= (NamedNumber rhs) const { return value >= rhs; }
    bool operator == (const TipoAplicativo &rhs) const { return value == rhs.value; }
    bool operator != (const TipoAplicativo &rhs) const { return value != rhs.value; }

    void swap (TipoAplicativo & that) { Inherited::swap(that); }
    TipoAplicativo * clone() const
    { return static_cast<TipoAplicativo*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const char * nameList[];
}; // end class TipoAplicativo

//
// ModuloInformacaoMidia::Autenticacao
//

class Autenticacao : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    Autenticacao(const void* info) : Inherited(info) {}
public:
    Autenticacao() : Inherited(&theInfo) {}
    Autenticacao(const Autenticacao & that) : Inherited(that) {}

    Autenticacao& operator=(const Autenticacao& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_dataInicial,
      e_dataFinal
    };

    class dataInicial
    {
    public:
        typedef ModuloTiposEleitorais::DataGeracao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class dataInicial

    dataInicial::const_reference get_dataInicial () const
    {
      assert(hasOptionalField(e_dataInicial));
      return *static_cast<dataInicial::const_pointer>(fields[0]);
    }
    dataInicial::reference ref_dataInicial ()
    {
      assert(hasOptionalField(e_dataInicial));
      return *static_cast<dataInicial::pointer>(fields[0]);
    }
    dataInicial::reference set_dataInicial ()
    {
      includeOptionalField( e_dataInicial, 0);
      return *static_cast<dataInicial::pointer>(fields[0]);
    }
    dataInicial::reference set_dataInicial (dataInicial::const_reference v)
    {
      includeOptionalField( e_dataInicial, 0);
      return *static_cast<dataInicial::pointer>(fields[0]) = v;
    }
    void omit_dataInicial () { removeOptionalField( e_dataInicial); }
    bool dataInicial_isPresent () const { return hasOptionalField( e_dataInicial); }

    class dataFinal
    {
    public:
        typedef ModuloTiposEleitorais::DataGeracao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class dataFinal

    dataFinal::const_reference get_dataFinal () const
    {
      assert(hasOptionalField(e_dataFinal));
      return *static_cast<dataFinal::const_pointer>(fields[1]);
    }
    dataFinal::reference ref_dataFinal ()
    {
      assert(hasOptionalField(e_dataFinal));
      return *static_cast<dataFinal::pointer>(fields[1]);
    }
    dataFinal::reference set_dataFinal ()
    {
      includeOptionalField( e_dataFinal, 1);
      return *static_cast<dataFinal::pointer>(fields[1]);
    }
    dataFinal::reference set_dataFinal (dataFinal::const_reference v)
    {
      includeOptionalField( e_dataFinal, 1);
      return *static_cast<dataFinal::pointer>(fields[1]) = v;
    }
    void omit_dataFinal () { removeOptionalField( e_dataFinal); }
    bool dataFinal_isPresent () const { return hasOptionalField( e_dataFinal); }

    class tamanhoSenha
    {
    public:
        typedef ASN1::INTEGER value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class tamanhoSenha

    tamanhoSenha::const_reference get_tamanhoSenha () const { return *static_cast<tamanhoSenha::const_pointer>(fields[2]); }
    tamanhoSenha::reference ref_tamanhoSenha () { return *static_cast<tamanhoSenha::pointer>(fields[2]); }
    tamanhoSenha::reference set_tamanhoSenha () { return *static_cast<tamanhoSenha::pointer>(fields[2]); }
    tamanhoSenha::reference set_tamanhoSenha (ASN1::INTEGER::int_type v)
    { return *static_cast<tamanhoSenha::pointer>(fields[2]) = v; }

    class numeroTentativas
    {
    public:
        typedef ASN1::INTEGER value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class numeroTentativas

    numeroTentativas::const_reference get_numeroTentativas () const { return *static_cast<numeroTentativas::const_pointer>(fields[3]); }
    numeroTentativas::reference ref_numeroTentativas () { return *static_cast<numeroTentativas::pointer>(fields[3]); }
    numeroTentativas::reference set_numeroTentativas () { return *static_cast<numeroTentativas::pointer>(fields[3]); }
    numeroTentativas::reference set_numeroTentativas (ASN1::INTEGER::int_type v)
    { return *static_cast<numeroTentativas::pointer>(fields[3]) = v; }

    class hashSenha
    {
    public:
        typedef ASN1::OCTET_STRING value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class hashSenha

    hashSenha::const_reference get_hashSenha () const { return *static_cast<hashSenha::const_pointer>(fields[4]); }
    hashSenha::reference ref_hashSenha () { return *static_cast<hashSenha::pointer>(fields[4]); }
    hashSenha::reference set_hashSenha () { return *static_cast<hashSenha::pointer>(fields[4]); }
    hashSenha::reference set_hashSenha (const ASN1::OCTET_STRING& v)
    { return *static_cast<hashSenha::pointer>(fields[4]) = v; }
    void swap(Autenticacao& that) { Inherited::swap(that); }
    Autenticacao * clone() const
    { return static_cast<Autenticacao*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[5];
    static int fieldIds[5];
    static unsigned fieldTags[5];
    static const char* fieldNames[5];
}; // end class Autenticacao

//
// ModuloInformacaoMidia::Aplicativo
//

class Aplicativo : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    Aplicativo(const void* info) : Inherited(info) {}
public:
    Aplicativo() : Inherited(&theInfo) {}
    Aplicativo(const Aplicativo & that) : Inherited(that) {}

    Aplicativo& operator=(const Aplicativo& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_autenticacao
    };

    class tipoAplicativo
    {
    public:
        typedef ModuloInformacaoMidia::TipoAplicativo value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class tipoAplicativo

    tipoAplicativo::const_reference get_tipoAplicativo () const { return *static_cast<tipoAplicativo::const_pointer>(fields[0]); }
    tipoAplicativo::reference ref_tipoAplicativo () { return *static_cast<tipoAplicativo::pointer>(fields[0]); }
    tipoAplicativo::reference set_tipoAplicativo () { return *static_cast<tipoAplicativo::pointer>(fields[0]); }
    tipoAplicativo::reference set_tipoAplicativo (ModuloInformacaoMidia::TipoAplicativo::NamedNumber v)
    { return *static_cast<tipoAplicativo::pointer>(fields[0]) = v; }

    class autenticacao
    {
    public:
        typedef ModuloInformacaoMidia::Autenticacao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class autenticacao

    autenticacao::const_reference get_autenticacao () const
    {
      assert(hasOptionalField(e_autenticacao));
      return *static_cast<autenticacao::const_pointer>(fields[1]);
    }
    autenticacao::reference ref_autenticacao ()
    {
      assert(hasOptionalField(e_autenticacao));
      return *static_cast<autenticacao::pointer>(fields[1]);
    }
    autenticacao::reference set_autenticacao ()
    {
      includeOptionalField( e_autenticacao, 1);
      return *static_cast<autenticacao::pointer>(fields[1]);
    }
    autenticacao::reference set_autenticacao (autenticacao::const_reference v)
    {
      includeOptionalField( e_autenticacao, 1);
      return *static_cast<autenticacao::pointer>(fields[1]) = v;
    }
    void omit_autenticacao () { removeOptionalField( e_autenticacao); }
    bool autenticacao_isPresent () const { return hasOptionalField( e_autenticacao); }
    void swap(Aplicativo& that) { Inherited::swap(that); }
    Aplicativo * clone() const
    { return static_cast<Aplicativo*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[2];
    static int fieldIds[2];
    static const char* fieldNames[2];
}; // end class Aplicativo

//
// ModuloInformacaoMidia::InformacaoMidia
//

class InformacaoMidia : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    InformacaoMidia(const void* info) : Inherited(info) {}
public:
    InformacaoMidia() : Inherited(&theInfo) {}
    InformacaoMidia(const InformacaoMidia & that) : Inherited(that) {}

    InformacaoMidia& operator=(const InformacaoMidia& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_aplicativos
    };

    class tipoMidia
    {
    public:
        typedef ModuloInformacaoMidia::TipoMidia value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class tipoMidia

    tipoMidia::const_reference get_tipoMidia () const { return *static_cast<tipoMidia::const_pointer>(fields[0]); }
    tipoMidia::reference ref_tipoMidia () { return *static_cast<tipoMidia::pointer>(fields[0]); }
    tipoMidia::reference set_tipoMidia () { return *static_cast<tipoMidia::pointer>(fields[0]); }
    tipoMidia::reference set_tipoMidia (ModuloInformacaoMidia::TipoMidia::NamedNumber v)
    { return *static_cast<tipoMidia::pointer>(fields[0]) = v; }

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

    class idPE
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 99999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class idPE

    idPE::const_reference get_idPE () const { return *static_cast<idPE::const_pointer>(fields[2]); }
    idPE::reference ref_idPE () { return *static_cast<idPE::pointer>(fields[2]); }
    idPE::reference set_idPE () { return *static_cast<idPE::pointer>(fields[2]); }
    idPE::reference set_idPE (idPE::const_reference v)
    { return *static_cast<idPE::pointer>(fields[2]) = v; }

    class uf
    {
    public:
        typedef ModuloTiposEleitorais::SiglaUF value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class uf

    uf::const_reference get_uf () const { return *static_cast<uf::const_pointer>(fields[3]); }
    uf::reference ref_uf () { return *static_cast<uf::pointer>(fields[3]); }
    uf::reference set_uf () { return *static_cast<uf::pointer>(fields[3]); }
    uf::reference set_uf (uf::const_reference v)
    { return *static_cast<uf::pointer>(fields[3]) = v; }

    class turno
    {
    public:
        typedef ModuloTiposEleitorais::Turno value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class turno

    turno::const_reference get_turno () const { return *static_cast<turno::const_pointer>(fields[4]); }
    turno::reference ref_turno () { return *static_cast<turno::pointer>(fields[4]); }
    turno::reference set_turno () { return *static_cast<turno::pointer>(fields[4]); }
    turno::reference set_turno (ModuloTiposEleitorais::Turno::NamedNumber v)
    { return *static_cast<turno::pointer>(fields[4]) = v; }

    class dadosGeracaoMidia
    {
    public:
        typedef ModuloInformacaoMidia::DadosGeracaoMidia value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class dadosGeracaoMidia

    dadosGeracaoMidia::const_reference get_dadosGeracaoMidia () const { return *static_cast<dadosGeracaoMidia::const_pointer>(fields[5]); }
    dadosGeracaoMidia::reference ref_dadosGeracaoMidia () { return *static_cast<dadosGeracaoMidia::pointer>(fields[5]); }
    dadosGeracaoMidia::reference set_dadosGeracaoMidia () { return *static_cast<dadosGeracaoMidia::pointer>(fields[5]); }
    dadosGeracaoMidia::reference set_dadosGeracaoMidia (dadosGeracaoMidia::const_reference v)
    { return *static_cast<dadosGeracaoMidia::pointer>(fields[5]) = v; }

    class aplicativos
    {
    public:
        // the target has no class of its own here: SEQUENCE_OF<Aplicativo>'s template theInfo and vtable
        // (vptr 1127020, typeinfo 1127048) are what fns 9179/9183 use
        typedef ASN1::SEQUENCE_OF<ModuloInformacaoMidia::Aplicativo> value_type;

        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class aplicativos

    aplicativos::const_reference get_aplicativos () const
    {
      assert(hasOptionalField(e_aplicativos));
      return *static_cast<aplicativos::const_pointer>(fields[6]);
    }
    aplicativos::reference ref_aplicativos ()
    {
      assert(hasOptionalField(e_aplicativos));
      return *static_cast<aplicativos::pointer>(fields[6]);
    }
    aplicativos::reference set_aplicativos ()
    {
      includeOptionalField( e_aplicativos, 6);
      return *static_cast<aplicativos::pointer>(fields[6]);
    }
    aplicativos::reference set_aplicativos (aplicativos::const_reference v)
    {
      includeOptionalField( e_aplicativos, 6);
      return *static_cast<aplicativos::pointer>(fields[6]) = v;
    }
    void omit_aplicativos () { removeOptionalField( e_aplicativos); }
    bool aplicativos_isPresent () const { return hasOptionalField( e_aplicativos); }
    void swap(InformacaoMidia& that) { Inherited::swap(that); }
    InformacaoMidia * clone() const
    { return static_cast<InformacaoMidia*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[7];
    static int fieldIds[7];
    static const char* fieldNames[7];
}; // end class InformacaoMidia

} // namespace ModuloInformacaoMidia

#endif // __MODULOINFORMACAOMIDIA_H
