#ifndef __MODULOPARAMETRIZACAOURNA_H
#define __MODULOPARAMETRIZACAOURNA_H

#include "asn1.h"

#include "ModuloTiposEleitorais.h"

namespace ModuloParametrizacaoUrna {

//
// ModuloParametrizacaoUrna::TituloRelatorio
//

class TituloRelatorio : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    TituloRelatorio(const void* info) : Inherited(info) {}
public:
    TituloRelatorio() : Inherited(&theInfo) {}
    TituloRelatorio(const TituloRelatorio & that) : Inherited(that) {}

    TituloRelatorio& operator=(const TituloRelatorio& that)
    { Inherited::operator=(that); return *this; }


    class alinhamento
    {
    public:
        //
        // ModuloParametrizacaoUrna::TituloRelatorio::alinhamento::value_type
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
                esquerdo = 1,
                direito,
                centro
            };

            bool is_esquerdo() const { return value == esquerdo; }
            void set_esquerdo() { value = esquerdo; }

            bool is_direito() const { return value == direito; }
            void set_direito() { value = direito; }

            bool is_centro() const { return value == centro; }
            void set_centro() { value = centro; }

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
    }; // end class alinhamento

    alinhamento::const_reference get_alinhamento () const { return *static_cast<alinhamento::const_pointer>(fields[0]); }
    alinhamento::reference ref_alinhamento () { return *static_cast<alinhamento::pointer>(fields[0]); }
    alinhamento::reference set_alinhamento () { return *static_cast<alinhamento::pointer>(fields[0]); }
    alinhamento::reference set_alinhamento (alinhamento::value_type::NamedNumber v)
    { return *static_cast<alinhamento::pointer>(fields[0]) = v; }

    class estilo
    {
    public:
        //
        // ModuloParametrizacaoUrna::TituloRelatorio::estilo::value_type
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
                normal = 1,
                expandido
            };

            bool is_normal() const { return value == normal; }
            void set_normal() { value = normal; }

            bool is_expandido() const { return value == expandido; }
            void set_expandido() { value = expandido; }

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
    }; // end class estilo

    estilo::const_reference get_estilo () const { return *static_cast<estilo::const_pointer>(fields[1]); }
    estilo::reference ref_estilo () { return *static_cast<estilo::pointer>(fields[1]); }
    estilo::reference set_estilo () { return *static_cast<estilo::pointer>(fields[1]); }
    estilo::reference set_estilo (estilo::value_type::NamedNumber v)
    { return *static_cast<estilo::pointer>(fields[1]) = v; }

    class texto
    {
    public:
        //
        // ModuloParametrizacaoUrna::TituloRelatorio::texto::value_type
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
    }; // end class texto

    texto::const_reference get_texto () const { return *static_cast<texto::const_pointer>(fields[2]); }
    texto::reference ref_texto () { return *static_cast<texto::pointer>(fields[2]); }
    texto::reference set_texto () { return *static_cast<texto::pointer>(fields[2]); }
    texto::reference set_texto (const ASN1_STD string& v)
    { return *static_cast<texto::pointer>(fields[2]) = v; }
    void swap(TituloRelatorio& that) { Inherited::swap(that); }
    TituloRelatorio * clone() const
    { return static_cast<TituloRelatorio*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[3];
    static int fieldIds[3];
    static const char* fieldNames[3];
}; // end class TituloRelatorio
// the module's own name for the inline ENUMERATED (app source spells it: ledger/source-locations.tsv)
typedef TituloRelatorio::alinhamento::value_type AlinhamentoTitulo;
typedef TituloRelatorio::estilo::value_type EstiloTitulo;

//
// ModuloParametrizacaoUrna::Cabecalho   // SYNTHESIZED name (field ['cabecalho', 'rodapeZEVOTA', 'rodapeBUVOTA', 'rodapeZESA', 'rodapeBUSA', 'rodapeBUParcial'])
//

// Not a class: the six fields' fieldInfos (and the object 9161 builds, vptr 1128628) use the SEQUENCE_OF<TituloRelatorio>
// template statics directly - theInfo 1128280 has typeName NULL (R-ASNCORE). The name is only this typedef's.
typedef ASN1::SEQUENCE_OF<ModuloParametrizacaoUrna::TituloRelatorio> Cabecalho;

//
// ModuloParametrizacaoUrna::FormaSuspenderSemVoto
//

class FormaSuspenderSemVoto : public ASN1::ENUMERATED
{
    typedef ASN1::ENUMERATED Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    FormaSuspenderSemVoto(const void* info) : Inherited(info) {}
public:
    FormaSuspenderSemVoto() : Inherited(&theInfo) {}
    enum NamedNumber {
        unknownEnumeration_ = -1,
        tornaNaoVotou = 1,
        tornaTodosBranco,
        tornaTodosNulo
    };

    bool is_tornaNaoVotou() const { return value == tornaNaoVotou; }
    void set_tornaNaoVotou() { value = tornaNaoVotou; }

    bool is_tornaTodosBranco() const { return value == tornaTodosBranco; }
    void set_tornaTodosBranco() { value = tornaTodosBranco; }

    bool is_tornaTodosNulo() const { return value == tornaTodosNulo; }
    void set_tornaTodosNulo() { value = tornaTodosNulo; }

    FormaSuspenderSemVoto(const NamedNumber v) : Inherited(&theInfo) { setFromInt(v); }
    FormaSuspenderSemVoto & operator=(const NamedNumber v) { setFromInt(v);  return *this; }
    operator NamedNumber() const { return NamedNumber(asInt()); }

    bool operator == (NamedNumber rhs) const { return value == rhs; }
    bool operator != (NamedNumber rhs) const { return value != rhs; }
    bool operator <  (NamedNumber rhs) const { return value <  rhs; }
    bool operator >  (NamedNumber rhs) const { return value >  rhs; }
    bool operator <= (NamedNumber rhs) const { return value <= rhs; }
    bool operator >= (NamedNumber rhs) const { return value >= rhs; }
    bool operator == (const FormaSuspenderSemVoto &rhs) const { return value == rhs.value; }
    bool operator != (const FormaSuspenderSemVoto &rhs) const { return value != rhs.value; }

    void swap (FormaSuspenderSemVoto & that) { Inherited::swap(that); }
    FormaSuspenderSemVoto * clone() const
    { return static_cast<FormaSuspenderSemVoto*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const char * nameList[];
}; // end class FormaSuspenderSemVoto

//
// ModuloParametrizacaoUrna::FormaSuspenderComVoto
//

class FormaSuspenderComVoto : public ASN1::ENUMERATED
{
    typedef ASN1::ENUMERATED Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    FormaSuspenderComVoto(const void* info) : Inherited(info) {}
public:
    FormaSuspenderComVoto() : Inherited(&theInfo) {}
    enum NamedNumber {
        unknownEnumeration_ = -1,
        tornaOutrosBranco = 1,
        tornaOutrosNulo,
        tornaNaoVotouParcial
    };

    bool is_tornaOutrosBranco() const { return value == tornaOutrosBranco; }
    void set_tornaOutrosBranco() { value = tornaOutrosBranco; }

    bool is_tornaOutrosNulo() const { return value == tornaOutrosNulo; }
    void set_tornaOutrosNulo() { value = tornaOutrosNulo; }

    bool is_tornaNaoVotouParcial() const { return value == tornaNaoVotouParcial; }
    void set_tornaNaoVotouParcial() { value = tornaNaoVotouParcial; }

    FormaSuspenderComVoto(const NamedNumber v) : Inherited(&theInfo) { setFromInt(v); }
    FormaSuspenderComVoto & operator=(const NamedNumber v) { setFromInt(v);  return *this; }
    operator NamedNumber() const { return NamedNumber(asInt()); }

    bool operator == (NamedNumber rhs) const { return value == rhs; }
    bool operator != (NamedNumber rhs) const { return value != rhs; }
    bool operator <  (NamedNumber rhs) const { return value <  rhs; }
    bool operator >  (NamedNumber rhs) const { return value >  rhs; }
    bool operator <= (NamedNumber rhs) const { return value <= rhs; }
    bool operator >= (NamedNumber rhs) const { return value >= rhs; }
    bool operator == (const FormaSuspenderComVoto &rhs) const { return value == rhs.value; }
    bool operator != (const FormaSuspenderComVoto &rhs) const { return value != rhs.value; }

    void swap (FormaSuspenderComVoto & that) { Inherited::swap(that); }
    FormaSuspenderComVoto * clone() const
    { return static_cast<FormaSuspenderComVoto*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const char * nameList[];
}; // end class FormaSuspenderComVoto

//
// ModuloParametrizacaoUrna::LabelParametrizado
//

class LabelParametrizado : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    LabelParametrizado(const void* info) : Inherited(info) {}
public:
    LabelParametrizado() : Inherited(&theInfo) {}
    LabelParametrizado(const LabelParametrizado & that) : Inherited(that) {}

    LabelParametrizado& operator=(const LabelParametrizado& that)
    { Inherited::operator=(that); return *this; }


    class genero
    {
    public:
        //
        // ModuloParametrizacaoUrna::LabelParametrizado::genero::value_type
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
                neutro = 1,
                masculino,
                feminino
            };

            bool is_neutro() const { return value == neutro; }
            void set_neutro() { value = neutro; }

            bool is_masculino() const { return value == masculino; }
            void set_masculino() { value = masculino; }

            bool is_feminino() const { return value == feminino; }
            void set_feminino() { value = feminino; }

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
    }; // end class genero

    genero::const_reference get_genero () const { return *static_cast<genero::const_pointer>(fields[0]); }
    genero::reference ref_genero () { return *static_cast<genero::pointer>(fields[0]); }
    genero::reference set_genero () { return *static_cast<genero::pointer>(fields[0]); }
    genero::reference set_genero (genero::value_type::NamedNumber v)
    { return *static_cast<genero::pointer>(fields[0]) = v; }

    class longo
    {
    public:
        //
        // ModuloParametrizacaoUrna::LabelParametrizado::longo::value_type
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
    }; // end class longo

    longo::const_reference get_longo () const { return *static_cast<longo::const_pointer>(fields[1]); }
    longo::reference ref_longo () { return *static_cast<longo::pointer>(fields[1]); }
    longo::reference set_longo () { return *static_cast<longo::pointer>(fields[1]); }
    longo::reference set_longo (const ASN1_STD string& v)
    { return *static_cast<longo::pointer>(fields[1]) = v; }

    class curto
    {
    public:
        //
        // ModuloParametrizacaoUrna::LabelParametrizado::curto::value_type
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
    }; // end class curto

    curto::const_reference get_curto () const { return *static_cast<curto::const_pointer>(fields[2]); }
    curto::reference ref_curto () { return *static_cast<curto::pointer>(fields[2]); }
    curto::reference set_curto () { return *static_cast<curto::pointer>(fields[2]); }
    curto::reference set_curto (const ASN1_STD string& v)
    { return *static_cast<curto::pointer>(fields[2]) = v; }

    class longoPlural
    {
    public:
        //
        // ModuloParametrizacaoUrna::LabelParametrizado::longoPlural::value_type
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
    }; // end class longoPlural

    longoPlural::const_reference get_longoPlural () const { return *static_cast<longoPlural::const_pointer>(fields[3]); }
    longoPlural::reference ref_longoPlural () { return *static_cast<longoPlural::pointer>(fields[3]); }
    longoPlural::reference set_longoPlural () { return *static_cast<longoPlural::pointer>(fields[3]); }
    longoPlural::reference set_longoPlural (const ASN1_STD string& v)
    { return *static_cast<longoPlural::pointer>(fields[3]) = v; }

    class curtoPlural
    {
    public:
        //
        // ModuloParametrizacaoUrna::LabelParametrizado::curtoPlural::value_type
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
    }; // end class curtoPlural

    curtoPlural::const_reference get_curtoPlural () const { return *static_cast<curtoPlural::const_pointer>(fields[4]); }
    curtoPlural::reference ref_curtoPlural () { return *static_cast<curtoPlural::pointer>(fields[4]); }
    curtoPlural::reference set_curtoPlural () { return *static_cast<curtoPlural::pointer>(fields[4]); }
    curtoPlural::reference set_curtoPlural (const ASN1_STD string& v)
    { return *static_cast<curtoPlural::pointer>(fields[4]) = v; }
    void swap(LabelParametrizado& that) { Inherited::swap(that); }
    LabelParametrizado * clone() const
    { return static_cast<LabelParametrizado*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[5];
    static int fieldIds[5];
    static const char* fieldNames[5];
}; // end class LabelParametrizado
// the module's own name for the inline ENUMERATED (app source spells it: ledger/source-locations.tsv)
typedef LabelParametrizado::genero::value_type GeneroLabel;

//
// ModuloParametrizacaoUrna::ParametrosUrna
//

class ParametrosUrna : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    ParametrosUrna(const void* info) : Inherited(info) {}
public:
    ParametrosUrna() : Inherited(&theInfo) {}
    ParametrosUrna(const ParametrosUrna & that) : Inherited(that) {}

    ParametrosUrna& operator=(const ParametrosUrna& that)
    { Inherited::operator=(that); return *this; }


    class formaSuspensaoSemVoto
    {
    public:
        typedef ModuloParametrizacaoUrna::FormaSuspenderSemVoto value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class formaSuspensaoSemVoto

    formaSuspensaoSemVoto::const_reference get_formaSuspensaoSemVoto () const { return *static_cast<formaSuspensaoSemVoto::const_pointer>(fields[0]); }
    formaSuspensaoSemVoto::reference ref_formaSuspensaoSemVoto () { return *static_cast<formaSuspensaoSemVoto::pointer>(fields[0]); }
    formaSuspensaoSemVoto::reference set_formaSuspensaoSemVoto () { return *static_cast<formaSuspensaoSemVoto::pointer>(fields[0]); }
    formaSuspensaoSemVoto::reference set_formaSuspensaoSemVoto (ModuloParametrizacaoUrna::FormaSuspenderSemVoto::NamedNumber v)
    { return *static_cast<formaSuspensaoSemVoto::pointer>(fields[0]) = v; }

    class formaSuspensaoComVoto
    {
    public:
        typedef ModuloParametrizacaoUrna::FormaSuspenderComVoto value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class formaSuspensaoComVoto

    formaSuspensaoComVoto::const_reference get_formaSuspensaoComVoto () const { return *static_cast<formaSuspensaoComVoto::const_pointer>(fields[1]); }
    formaSuspensaoComVoto::reference ref_formaSuspensaoComVoto () { return *static_cast<formaSuspensaoComVoto::pointer>(fields[1]); }
    formaSuspensaoComVoto::reference set_formaSuspensaoComVoto () { return *static_cast<formaSuspensaoComVoto::pointer>(fields[1]); }
    formaSuspensaoComVoto::reference set_formaSuspensaoComVoto (ModuloParametrizacaoUrna::FormaSuspenderComVoto::NamedNumber v)
    { return *static_cast<formaSuspensaoComVoto::pointer>(fields[1]) = v; }

    class numZeresimas
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 50> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class numZeresimas

    numZeresimas::const_reference get_numZeresimas () const { return *static_cast<numZeresimas::const_pointer>(fields[2]); }
    numZeresimas::reference ref_numZeresimas () { return *static_cast<numZeresimas::pointer>(fields[2]); }
    numZeresimas::reference set_numZeresimas () { return *static_cast<numZeresimas::pointer>(fields[2]); }
    numZeresimas::reference set_numZeresimas (ASN1::INTEGER::int_type v)
    { return *static_cast<numZeresimas::pointer>(fields[2]) = v; }

    class numBUVotaObrigatorios
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 50> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class numBUVotaObrigatorios

    numBUVotaObrigatorios::const_reference get_numBUVotaObrigatorios () const { return *static_cast<numBUVotaObrigatorios::const_pointer>(fields[3]); }
    numBUVotaObrigatorios::reference ref_numBUVotaObrigatorios () { return *static_cast<numBUVotaObrigatorios::pointer>(fields[3]); }
    numBUVotaObrigatorios::reference set_numBUVotaObrigatorios () { return *static_cast<numBUVotaObrigatorios::pointer>(fields[3]); }
    numBUVotaObrigatorios::reference set_numBUVotaObrigatorios (ASN1::INTEGER::int_type v)
    { return *static_cast<numBUVotaObrigatorios::pointer>(fields[3]) = v; }

    class numBUVotaAdicionais
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 50> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class numBUVotaAdicionais

    numBUVotaAdicionais::const_reference get_numBUVotaAdicionais () const { return *static_cast<numBUVotaAdicionais::const_pointer>(fields[4]); }
    numBUVotaAdicionais::reference ref_numBUVotaAdicionais () { return *static_cast<numBUVotaAdicionais::pointer>(fields[4]); }
    numBUVotaAdicionais::reference set_numBUVotaAdicionais () { return *static_cast<numBUVotaAdicionais::pointer>(fields[4]); }
    numBUVotaAdicionais::reference set_numBUVotaAdicionais (ASN1::INTEGER::int_type v)
    { return *static_cast<numBUVotaAdicionais::pointer>(fields[4]) = v; }

    class numBUFinalREDObrigat
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 50> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class numBUFinalREDObrigat

    numBUFinalREDObrigat::const_reference get_numBUFinalREDObrigat () const { return *static_cast<numBUFinalREDObrigat::const_pointer>(fields[5]); }
    numBUFinalREDObrigat::reference ref_numBUFinalREDObrigat () { return *static_cast<numBUFinalREDObrigat::pointer>(fields[5]); }
    numBUFinalREDObrigat::reference set_numBUFinalREDObrigat () { return *static_cast<numBUFinalREDObrigat::pointer>(fields[5]); }
    numBUFinalREDObrigat::reference set_numBUFinalREDObrigat (ASN1::INTEGER::int_type v)
    { return *static_cast<numBUFinalREDObrigat::pointer>(fields[5]) = v; }

    class numBUFinalREDAdic
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 50> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class numBUFinalREDAdic

    numBUFinalREDAdic::const_reference get_numBUFinalREDAdic () const { return *static_cast<numBUFinalREDAdic::const_pointer>(fields[6]); }
    numBUFinalREDAdic::reference ref_numBUFinalREDAdic () { return *static_cast<numBUFinalREDAdic::pointer>(fields[6]); }
    numBUFinalREDAdic::reference set_numBUFinalREDAdic () { return *static_cast<numBUFinalREDAdic::pointer>(fields[6]); }
    numBUFinalREDAdic::reference set_numBUFinalREDAdic (ASN1::INTEGER::int_type v)
    { return *static_cast<numBUFinalREDAdic::pointer>(fields[6]) = v; }

    class numBUParcialREDObrigat
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 50> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class numBUParcialREDObrigat

    numBUParcialREDObrigat::const_reference get_numBUParcialREDObrigat () const { return *static_cast<numBUParcialREDObrigat::const_pointer>(fields[7]); }
    numBUParcialREDObrigat::reference ref_numBUParcialREDObrigat () { return *static_cast<numBUParcialREDObrigat::pointer>(fields[7]); }
    numBUParcialREDObrigat::reference set_numBUParcialREDObrigat () { return *static_cast<numBUParcialREDObrigat::pointer>(fields[7]); }
    numBUParcialREDObrigat::reference set_numBUParcialREDObrigat (ASN1::INTEGER::int_type v)
    { return *static_cast<numBUParcialREDObrigat::pointer>(fields[7]) = v; }

    class numBUParcialREDAdic
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 50> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class numBUParcialREDAdic

    numBUParcialREDAdic::const_reference get_numBUParcialREDAdic () const { return *static_cast<numBUParcialREDAdic::const_pointer>(fields[8]); }
    numBUParcialREDAdic::reference ref_numBUParcialREDAdic () { return *static_cast<numBUParcialREDAdic::pointer>(fields[8]); }
    numBUParcialREDAdic::reference set_numBUParcialREDAdic () { return *static_cast<numBUParcialREDAdic::pointer>(fields[8]); }
    numBUParcialREDAdic::reference set_numBUParcialREDAdic (ASN1::INTEGER::int_type v)
    { return *static_cast<numBUParcialREDAdic::pointer>(fields[8]) = v; }

    class numBUFinalSAObrigat
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 50> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class numBUFinalSAObrigat

    numBUFinalSAObrigat::const_reference get_numBUFinalSAObrigat () const { return *static_cast<numBUFinalSAObrigat::const_pointer>(fields[9]); }
    numBUFinalSAObrigat::reference ref_numBUFinalSAObrigat () { return *static_cast<numBUFinalSAObrigat::pointer>(fields[9]); }
    numBUFinalSAObrigat::reference set_numBUFinalSAObrigat () { return *static_cast<numBUFinalSAObrigat::pointer>(fields[9]); }
    numBUFinalSAObrigat::reference set_numBUFinalSAObrigat (ASN1::INTEGER::int_type v)
    { return *static_cast<numBUFinalSAObrigat::pointer>(fields[9]) = v; }

    class numBUFinalSAAdic
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 50> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class numBUFinalSAAdic

    numBUFinalSAAdic::const_reference get_numBUFinalSAAdic () const { return *static_cast<numBUFinalSAAdic::const_pointer>(fields[10]); }
    numBUFinalSAAdic::reference ref_numBUFinalSAAdic () { return *static_cast<numBUFinalSAAdic::pointer>(fields[10]); }
    numBUFinalSAAdic::reference set_numBUFinalSAAdic () { return *static_cast<numBUFinalSAAdic::pointer>(fields[10]); }
    numBUFinalSAAdic::reference set_numBUFinalSAAdic (ASN1::INTEGER::int_type v)
    { return *static_cast<numBUFinalSAAdic::pointer>(fields[10]) = v; }

    class numBUParcialSAObrigat
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 50> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class numBUParcialSAObrigat

    numBUParcialSAObrigat::const_reference get_numBUParcialSAObrigat () const { return *static_cast<numBUParcialSAObrigat::const_pointer>(fields[11]); }
    numBUParcialSAObrigat::reference ref_numBUParcialSAObrigat () { return *static_cast<numBUParcialSAObrigat::pointer>(fields[11]); }
    numBUParcialSAObrigat::reference set_numBUParcialSAObrigat () { return *static_cast<numBUParcialSAObrigat::pointer>(fields[11]); }
    numBUParcialSAObrigat::reference set_numBUParcialSAObrigat (ASN1::INTEGER::int_type v)
    { return *static_cast<numBUParcialSAObrigat::pointer>(fields[11]) = v; }

    class numTentativasHabilitacao
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 50> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class numTentativasHabilitacao

    numTentativasHabilitacao::const_reference get_numTentativasHabilitacao () const { return *static_cast<numTentativasHabilitacao::const_pointer>(fields[12]); }
    numTentativasHabilitacao::reference ref_numTentativasHabilitacao () { return *static_cast<numTentativasHabilitacao::pointer>(fields[12]); }
    numTentativasHabilitacao::reference set_numTentativasHabilitacao () { return *static_cast<numTentativasHabilitacao::pointer>(fields[12]); }
    numTentativasHabilitacao::reference set_numTentativasHabilitacao (ASN1::INTEGER::int_type v)
    { return *static_cast<numTentativasHabilitacao::pointer>(fields[12]) = v; }

    class numTentativasVerificacao
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 50> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class numTentativasVerificacao

    numTentativasVerificacao::const_reference get_numTentativasVerificacao () const { return *static_cast<numTentativasVerificacao::const_pointer>(fields[13]); }
    numTentativasVerificacao::reference ref_numTentativasVerificacao () { return *static_cast<numTentativasVerificacao::pointer>(fields[13]); }
    numTentativasVerificacao::reference set_numTentativasVerificacao () { return *static_cast<numTentativasVerificacao::pointer>(fields[13]); }
    numTentativasVerificacao::reference set_numTentativasVerificacao (ASN1::INTEGER::int_type v)
    { return *static_cast<numTentativasVerificacao::pointer>(fields[13]) = v; }

    class numRelatorioEstado
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 50> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class numRelatorioEstado

    numRelatorioEstado::const_reference get_numRelatorioEstado () const { return *static_cast<numRelatorioEstado::const_pointer>(fields[14]); }
    numRelatorioEstado::reference ref_numRelatorioEstado () { return *static_cast<numRelatorioEstado::pointer>(fields[14]); }
    numRelatorioEstado::reference set_numRelatorioEstado () { return *static_cast<numRelatorioEstado::pointer>(fields[14]); }
    numRelatorioEstado::reference set_numRelatorioEstado (ASN1::INTEGER::int_type v)
    { return *static_cast<numRelatorioEstado::pointer>(fields[14]) = v; }

    class numRelatorioEleitores
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 50> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class numRelatorioEleitores

    numRelatorioEleitores::const_reference get_numRelatorioEleitores () const { return *static_cast<numRelatorioEleitores::const_pointer>(fields[15]); }
    numRelatorioEleitores::reference ref_numRelatorioEleitores () { return *static_cast<numRelatorioEleitores::pointer>(fields[15]); }
    numRelatorioEleitores::reference set_numRelatorioEleitores () { return *static_cast<numRelatorioEleitores::pointer>(fields[15]); }
    numRelatorioEleitores::reference set_numRelatorioEleitores (ASN1::INTEGER::int_type v)
    { return *static_cast<numRelatorioEleitores::pointer>(fields[15]) = v; }

    class numRelatorioVersoesDados
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 50> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class numRelatorioVersoesDados

    numRelatorioVersoesDados::const_reference get_numRelatorioVersoesDados () const { return *static_cast<numRelatorioVersoesDados::const_pointer>(fields[16]); }
    numRelatorioVersoesDados::reference ref_numRelatorioVersoesDados () { return *static_cast<numRelatorioVersoesDados::pointer>(fields[16]); }
    numRelatorioVersoesDados::reference set_numRelatorioVersoesDados () { return *static_cast<numRelatorioVersoesDados::pointer>(fields[16]); }
    numRelatorioVersoesDados::reference set_numRelatorioVersoesDados (ASN1::INTEGER::int_type v)
    { return *static_cast<numRelatorioVersoesDados::pointer>(fields[16]) = v; }

    class numRelatorioPU
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 50> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class numRelatorioPU

    numRelatorioPU::const_reference get_numRelatorioPU () const { return *static_cast<numRelatorioPU::const_pointer>(fields[17]); }
    numRelatorioPU::reference ref_numRelatorioPU () { return *static_cast<numRelatorioPU::pointer>(fields[17]); }
    numRelatorioPU::reference set_numRelatorioPU () { return *static_cast<numRelatorioPU::pointer>(fields[17]); }
    numRelatorioPU::reference set_numRelatorioPU (ASN1::INTEGER::int_type v)
    { return *static_cast<numRelatorioPU::pointer>(fields[17]) = v; }

    class criptografarBU
    {
    public:
        typedef ASN1::BOOLEAN value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class criptografarBU

    criptografarBU::const_reference get_criptografarBU () const { return *static_cast<criptografarBU::const_pointer>(fields[18]); }
    criptografarBU::reference ref_criptografarBU () { return *static_cast<criptografarBU::pointer>(fields[18]); }
    criptografarBU::reference set_criptografarBU () { return *static_cast<criptografarBU::pointer>(fields[18]); }
    criptografarBU::reference set_criptografarBU (bool v)
    { return *static_cast<criptografarBU::pointer>(fields[18]) = v; }

    class criptografarJUFA
    {
    public:
        typedef ASN1::BOOLEAN value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class criptografarJUFA

    criptografarJUFA::const_reference get_criptografarJUFA () const { return *static_cast<criptografarJUFA::const_pointer>(fields[19]); }
    criptografarJUFA::reference ref_criptografarJUFA () { return *static_cast<criptografarJUFA::pointer>(fields[19]); }
    criptografarJUFA::reference set_criptografarJUFA () { return *static_cast<criptografarJUFA::pointer>(fields[19]); }
    criptografarJUFA::reference set_criptografarJUFA (bool v)
    { return *static_cast<criptografarJUFA::pointer>(fields[19]) = v; }

    class numDigitosPartido
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 5> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class numDigitosPartido

    numDigitosPartido::const_reference get_numDigitosPartido () const { return *static_cast<numDigitosPartido::const_pointer>(fields[20]); }
    numDigitosPartido::reference ref_numDigitosPartido () { return *static_cast<numDigitosPartido::pointer>(fields[20]); }
    numDigitosPartido::reference set_numDigitosPartido () { return *static_cast<numDigitosPartido::pointer>(fields[20]); }
    numDigitosPartido::reference set_numDigitosPartido (ASN1::INTEGER::int_type v)
    { return *static_cast<numDigitosPartido::pointer>(fields[20]) = v; }

    class tempoDispararSuspensao
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 15, 360> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class tempoDispararSuspensao

    tempoDispararSuspensao::const_reference get_tempoDispararSuspensao () const { return *static_cast<tempoDispararSuspensao::const_pointer>(fields[21]); }
    tempoDispararSuspensao::reference ref_tempoDispararSuspensao () { return *static_cast<tempoDispararSuspensao::pointer>(fields[21]); }
    tempoDispararSuspensao::reference set_tempoDispararSuspensao () { return *static_cast<tempoDispararSuspensao::pointer>(fields[21]); }
    tempoDispararSuspensao::reference set_tempoDispararSuspensao (ASN1::INTEGER::int_type v)
    { return *static_cast<tempoDispararSuspensao::pointer>(fields[21]) = v; }

    class tempoDispararSuspensaoTE
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 15, 360> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class tempoDispararSuspensaoTE

    tempoDispararSuspensaoTE::const_reference get_tempoDispararSuspensaoTE () const { return *static_cast<tempoDispararSuspensaoTE::const_pointer>(fields[22]); }
    tempoDispararSuspensaoTE::reference ref_tempoDispararSuspensaoTE () { return *static_cast<tempoDispararSuspensaoTE::pointer>(fields[22]); }
    tempoDispararSuspensaoTE::reference set_tempoDispararSuspensaoTE () { return *static_cast<tempoDispararSuspensaoTE::pointer>(fields[22]); }
    tempoDispararSuspensaoTE::reference set_tempoDispararSuspensaoTE (ASN1::INTEGER::int_type v)
    { return *static_cast<tempoDispararSuspensaoTE::pointer>(fields[22]) = v; }

    class tempoConfirmacaoVoto
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 99999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class tempoConfirmacaoVoto

    tempoConfirmacaoVoto::const_reference get_tempoConfirmacaoVoto () const { return *static_cast<tempoConfirmacaoVoto::const_pointer>(fields[23]); }
    tempoConfirmacaoVoto::reference ref_tempoConfirmacaoVoto () { return *static_cast<tempoConfirmacaoVoto::pointer>(fields[23]); }
    tempoConfirmacaoVoto::reference set_tempoConfirmacaoVoto () { return *static_cast<tempoConfirmacaoVoto::pointer>(fields[23]); }
    tempoConfirmacaoVoto::reference set_tempoConfirmacaoVoto (ASN1::INTEGER::int_type v)
    { return *static_cast<tempoConfirmacaoVoto::pointer>(fields[23]) = v; }

    class tempoDesligamentoAutomatico
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 9999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class tempoDesligamentoAutomatico

    tempoDesligamentoAutomatico::const_reference get_tempoDesligamentoAutomatico () const { return *static_cast<tempoDesligamentoAutomatico::const_pointer>(fields[24]); }
    tempoDesligamentoAutomatico::reference ref_tempoDesligamentoAutomatico () { return *static_cast<tempoDesligamentoAutomatico::pointer>(fields[24]); }
    tempoDesligamentoAutomatico::reference set_tempoDesligamentoAutomatico () { return *static_cast<tempoDesligamentoAutomatico::pointer>(fields[24]); }
    tempoDesligamentoAutomatico::reference set_tempoDesligamentoAutomatico (ASN1::INTEGER::int_type v)
    { return *static_cast<tempoDesligamentoAutomatico::pointer>(fields[24]) = v; }

    class tempoAvisoDesligamentoAutomatico
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 9999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class tempoAvisoDesligamentoAutomatico

    tempoAvisoDesligamentoAutomatico::const_reference get_tempoAvisoDesligamentoAutomatico () const { return *static_cast<tempoAvisoDesligamentoAutomatico::const_pointer>(fields[25]); }
    tempoAvisoDesligamentoAutomatico::reference ref_tempoAvisoDesligamentoAutomatico () { return *static_cast<tempoAvisoDesligamentoAutomatico::pointer>(fields[25]); }
    tempoAvisoDesligamentoAutomatico::reference set_tempoAvisoDesligamentoAutomatico () { return *static_cast<tempoAvisoDesligamentoAutomatico::pointer>(fields[25]); }
    tempoAvisoDesligamentoAutomatico::reference set_tempoAvisoDesligamentoAutomatico (ASN1::INTEGER::int_type v)
    { return *static_cast<tempoAvisoDesligamentoAutomatico::pointer>(fields[25]) = v; }

    class cabecalho
    {
    public:
        typedef ModuloParametrizacaoUrna::Cabecalho value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class cabecalho

    cabecalho::const_reference get_cabecalho () const { return *static_cast<cabecalho::const_pointer>(fields[26]); }
    cabecalho::reference ref_cabecalho () { return *static_cast<cabecalho::pointer>(fields[26]); }
    cabecalho::reference set_cabecalho () { return *static_cast<cabecalho::pointer>(fields[26]); }

    class rodapeZEVOTA
    {
    public:
        typedef ModuloParametrizacaoUrna::Cabecalho value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class rodapeZEVOTA

    rodapeZEVOTA::const_reference get_rodapeZEVOTA () const { return *static_cast<rodapeZEVOTA::const_pointer>(fields[27]); }
    rodapeZEVOTA::reference ref_rodapeZEVOTA () { return *static_cast<rodapeZEVOTA::pointer>(fields[27]); }
    rodapeZEVOTA::reference set_rodapeZEVOTA () { return *static_cast<rodapeZEVOTA::pointer>(fields[27]); }

    class rodapeBUVOTA
    {
    public:
        typedef ModuloParametrizacaoUrna::Cabecalho value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class rodapeBUVOTA

    rodapeBUVOTA::const_reference get_rodapeBUVOTA () const { return *static_cast<rodapeBUVOTA::const_pointer>(fields[28]); }
    rodapeBUVOTA::reference ref_rodapeBUVOTA () { return *static_cast<rodapeBUVOTA::pointer>(fields[28]); }
    rodapeBUVOTA::reference set_rodapeBUVOTA () { return *static_cast<rodapeBUVOTA::pointer>(fields[28]); }

    class rodapeZESA
    {
    public:
        typedef ModuloParametrizacaoUrna::Cabecalho value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class rodapeZESA

    rodapeZESA::const_reference get_rodapeZESA () const { return *static_cast<rodapeZESA::const_pointer>(fields[29]); }
    rodapeZESA::reference ref_rodapeZESA () { return *static_cast<rodapeZESA::pointer>(fields[29]); }
    rodapeZESA::reference set_rodapeZESA () { return *static_cast<rodapeZESA::pointer>(fields[29]); }

    class rodapeBUSA
    {
    public:
        typedef ModuloParametrizacaoUrna::Cabecalho value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class rodapeBUSA

    rodapeBUSA::const_reference get_rodapeBUSA () const { return *static_cast<rodapeBUSA::const_pointer>(fields[30]); }
    rodapeBUSA::reference ref_rodapeBUSA () { return *static_cast<rodapeBUSA::pointer>(fields[30]); }
    rodapeBUSA::reference set_rodapeBUSA () { return *static_cast<rodapeBUSA::pointer>(fields[30]); }

    class rodapeBUParcial
    {
    public:
        typedef ModuloParametrizacaoUrna::Cabecalho value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class rodapeBUParcial

    rodapeBUParcial::const_reference get_rodapeBUParcial () const { return *static_cast<rodapeBUParcial::const_pointer>(fields[31]); }
    rodapeBUParcial::reference ref_rodapeBUParcial () { return *static_cast<rodapeBUParcial::pointer>(fields[31]); }
    rodapeBUParcial::reference set_rodapeBUParcial () { return *static_cast<rodapeBUParcial::pointer>(fields[31]); }

    class telaEmissaoBU
    {
    public:
        //
        // ModuloParametrizacaoUrna::ParametrosUrna::telaEmissaoBU::value_type
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
    }; // end class telaEmissaoBU

    telaEmissaoBU::const_reference get_telaEmissaoBU () const { return *static_cast<telaEmissaoBU::const_pointer>(fields[32]); }
    telaEmissaoBU::reference ref_telaEmissaoBU () { return *static_cast<telaEmissaoBU::pointer>(fields[32]); }
    telaEmissaoBU::reference set_telaEmissaoBU () { return *static_cast<telaEmissaoBU::pointer>(fields[32]); }
    telaEmissaoBU::reference set_telaEmissaoBU (const ASN1_STD string& v)
    { return *static_cast<telaEmissaoBU::pointer>(fields[32]) = v; }

    class labelMunicipio
    {
    public:
        typedef ModuloParametrizacaoUrna::LabelParametrizado value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class labelMunicipio

    labelMunicipio::const_reference get_labelMunicipio () const { return *static_cast<labelMunicipio::const_pointer>(fields[33]); }
    labelMunicipio::reference ref_labelMunicipio () { return *static_cast<labelMunicipio::pointer>(fields[33]); }
    labelMunicipio::reference set_labelMunicipio () { return *static_cast<labelMunicipio::pointer>(fields[33]); }

    class labelZona
    {
    public:
        typedef ModuloParametrizacaoUrna::LabelParametrizado value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class labelZona

    labelZona::const_reference get_labelZona () const { return *static_cast<labelZona::const_pointer>(fields[34]); }
    labelZona::reference ref_labelZona () { return *static_cast<labelZona::pointer>(fields[34]); }
    labelZona::reference set_labelZona () { return *static_cast<labelZona::pointer>(fields[34]); }

    class labelSecao
    {
    public:
        typedef ModuloParametrizacaoUrna::LabelParametrizado value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class labelSecao

    labelSecao::const_reference get_labelSecao () const { return *static_cast<labelSecao::const_pointer>(fields[35]); }
    labelSecao::reference ref_labelSecao () { return *static_cast<labelSecao::pointer>(fields[35]); }
    labelSecao::reference set_labelSecao () { return *static_cast<labelSecao::pointer>(fields[35]); }

    class labelPartido
    {
    public:
        typedef ModuloParametrizacaoUrna::LabelParametrizado value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class labelPartido

    labelPartido::const_reference get_labelPartido () const { return *static_cast<labelPartido::const_pointer>(fields[36]); }
    labelPartido::reference ref_labelPartido () { return *static_cast<labelPartido::pointer>(fields[36]); }
    labelPartido::reference set_labelPartido () { return *static_cast<labelPartido::pointer>(fields[36]); }

    class imprimirZeradosBU
    {
    public:
        typedef ASN1::BOOLEAN value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class imprimirZeradosBU

    imprimirZeradosBU::const_reference get_imprimirZeradosBU () const { return *static_cast<imprimirZeradosBU::const_pointer>(fields[37]); }
    imprimirZeradosBU::reference ref_imprimirZeradosBU () { return *static_cast<imprimirZeradosBU::pointer>(fields[37]); }
    imprimirZeradosBU::reference set_imprimirZeradosBU () { return *static_cast<imprimirZeradosBU::pointer>(fields[37]); }
    imprimirZeradosBU::reference set_imprimirZeradosBU (bool v)
    { return *static_cast<imprimirZeradosBU::pointer>(fields[37]) = v; }

    class gravarZeradosBU
    {
    public:
        typedef ASN1::BOOLEAN value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class gravarZeradosBU

    gravarZeradosBU::const_reference get_gravarZeradosBU () const { return *static_cast<gravarZeradosBU::const_pointer>(fields[38]); }
    gravarZeradosBU::reference ref_gravarZeradosBU () { return *static_cast<gravarZeradosBU::pointer>(fields[38]); }
    gravarZeradosBU::reference set_gravarZeradosBU () { return *static_cast<gravarZeradosBU::pointer>(fields[38]); }
    gravarZeradosBU::reference set_gravarZeradosBU (bool v)
    { return *static_cast<gravarZeradosBU::pointer>(fields[38]) = v; }

    class aceitarJustificativa
    {
    public:
        typedef ASN1::BOOLEAN value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class aceitarJustificativa

    aceitarJustificativa::const_reference get_aceitarJustificativa () const { return *static_cast<aceitarJustificativa::const_pointer>(fields[39]); }
    aceitarJustificativa::reference ref_aceitarJustificativa () { return *static_cast<aceitarJustificativa::pointer>(fields[39]); }
    aceitarJustificativa::reference set_aceitarJustificativa () { return *static_cast<aceitarJustificativa::pointer>(fields[39]); }
    aceitarJustificativa::reference set_aceitarJustificativa (bool v)
    { return *static_cast<aceitarJustificativa::pointer>(fields[39]) = v; }

    class aceitarBrancoNulo
    {
    public:
        typedef ASN1::BOOLEAN value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class aceitarBrancoNulo

    aceitarBrancoNulo::const_reference get_aceitarBrancoNulo () const { return *static_cast<aceitarBrancoNulo::const_pointer>(fields[40]); }
    aceitarBrancoNulo::reference ref_aceitarBrancoNulo () { return *static_cast<aceitarBrancoNulo::pointer>(fields[40]); }
    aceitarBrancoNulo::reference set_aceitarBrancoNulo () { return *static_cast<aceitarBrancoNulo::pointer>(fields[40]); }
    aceitarBrancoNulo::reference set_aceitarBrancoNulo (bool v)
    { return *static_cast<aceitarBrancoNulo::pointer>(fields[40]) = v; }

    class apresentarPartido
    {
    public:
        typedef ASN1::BOOLEAN value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class apresentarPartido

    apresentarPartido::const_reference get_apresentarPartido () const { return *static_cast<apresentarPartido::const_pointer>(fields[41]); }
    apresentarPartido::reference ref_apresentarPartido () { return *static_cast<apresentarPartido::pointer>(fields[41]); }
    apresentarPartido::reference set_apresentarPartido () { return *static_cast<apresentarPartido::pointer>(fields[41]); }
    apresentarPartido::reference set_apresentarPartido (bool v)
    { return *static_cast<apresentarPartido::pointer>(fields[41]) = v; }

    class permitirHabManualAudio
    {
    public:
        typedef ASN1::BOOLEAN value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class permitirHabManualAudio

    permitirHabManualAudio::const_reference get_permitirHabManualAudio () const { return *static_cast<permitirHabManualAudio::const_pointer>(fields[42]); }
    permitirHabManualAudio::reference ref_permitirHabManualAudio () { return *static_cast<permitirHabManualAudio::pointer>(fields[42]); }
    permitirHabManualAudio::reference set_permitirHabManualAudio () { return *static_cast<permitirHabManualAudio::pointer>(fields[42]); }
    permitirHabManualAudio::reference set_permitirHabManualAudio (bool v)
    { return *static_cast<permitirHabManualAudio::pointer>(fields[42]) = v; }

    class imprimirQrCodeNoBU
    {
    public:
        typedef ASN1::BOOLEAN value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class imprimirQrCodeNoBU

    imprimirQrCodeNoBU::const_reference get_imprimirQrCodeNoBU () const { return *static_cast<imprimirQrCodeNoBU::const_pointer>(fields[43]); }
    imprimirQrCodeNoBU::reference ref_imprimirQrCodeNoBU () { return *static_cast<imprimirQrCodeNoBU::pointer>(fields[43]); }
    imprimirQrCodeNoBU::reference set_imprimirQrCodeNoBU () { return *static_cast<imprimirQrCodeNoBU::pointer>(fields[43]); }
    imprimirQrCodeNoBU::reference set_imprimirQrCodeNoBU (bool v)
    { return *static_cast<imprimirQrCodeNoBU::pointer>(fields[43]) = v; }

    class exibirScoreBiometria
    {
    public:
        typedef ASN1::BOOLEAN value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class exibirScoreBiometria

    exibirScoreBiometria::const_reference get_exibirScoreBiometria () const { return *static_cast<exibirScoreBiometria::const_pointer>(fields[44]); }
    exibirScoreBiometria::reference ref_exibirScoreBiometria () { return *static_cast<exibirScoreBiometria::pointer>(fields[44]); }
    exibirScoreBiometria::reference set_exibirScoreBiometria () { return *static_cast<exibirScoreBiometria::pointer>(fields[44]); }
    exibirScoreBiometria::reference set_exibirScoreBiometria (bool v)
    { return *static_cast<exibirScoreBiometria::pointer>(fields[44]) = v; }

    class registrarMesarios
    {
    public:
        typedef ASN1::BOOLEAN value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class registrarMesarios

    registrarMesarios::const_reference get_registrarMesarios () const { return *static_cast<registrarMesarios::const_pointer>(fields[45]); }
    registrarMesarios::reference ref_registrarMesarios () { return *static_cast<registrarMesarios::pointer>(fields[45]); }
    registrarMesarios::reference set_registrarMesarios () { return *static_cast<registrarMesarios::pointer>(fields[45]); }
    registrarMesarios::reference set_registrarMesarios (bool v)
    { return *static_cast<registrarMesarios::pointer>(fields[45]) = v; }

    class pedeAnoNascimentoEleitor
    {
    public:
        typedef ASN1::BOOLEAN value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class pedeAnoNascimentoEleitor

    pedeAnoNascimentoEleitor::const_reference get_pedeAnoNascimentoEleitor () const { return *static_cast<pedeAnoNascimentoEleitor::const_pointer>(fields[46]); }
    pedeAnoNascimentoEleitor::reference ref_pedeAnoNascimentoEleitor () { return *static_cast<pedeAnoNascimentoEleitor::pointer>(fields[46]); }
    pedeAnoNascimentoEleitor::reference set_pedeAnoNascimentoEleitor () { return *static_cast<pedeAnoNascimentoEleitor::pointer>(fields[46]); }
    pedeAnoNascimentoEleitor::reference set_pedeAnoNascimentoEleitor (bool v)
    { return *static_cast<pedeAnoNascimentoEleitor::pointer>(fields[46]) = v; }
    void swap(ParametrosUrna& that) { Inherited::swap(that); }
    ParametrosUrna * clone() const
    { return static_cast<ParametrosUrna*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[47];
    static int fieldIds[47];
    static const char* fieldNames[47];
}; // end class ParametrosUrna

//
// ModuloParametrizacaoUrna::EntidadeParametrizacaoUrna
//

class EntidadeParametrizacaoUrna : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EntidadeParametrizacaoUrna(const void* info) : Inherited(info) {}
public:
    EntidadeParametrizacaoUrna() : Inherited(&theInfo) {}
    EntidadeParametrizacaoUrna(const EntidadeParametrizacaoUrna & that) : Inherited(that) {}

    EntidadeParametrizacaoUrna& operator=(const EntidadeParametrizacaoUrna& that)
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

    class parametros
    {
    public:
        typedef ModuloParametrizacaoUrna::ParametrosUrna value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class parametros

    parametros::const_reference get_parametros () const { return *static_cast<parametros::const_pointer>(fields[1]); }
    parametros::reference ref_parametros () { return *static_cast<parametros::pointer>(fields[1]); }
    parametros::reference set_parametros () { return *static_cast<parametros::pointer>(fields[1]); }
    void swap(EntidadeParametrizacaoUrna& that) { Inherited::swap(that); }
    EntidadeParametrizacaoUrna * clone() const
    { return static_cast<EntidadeParametrizacaoUrna*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[2];
    static int fieldIds[2];
    static const char* fieldNames[2];
}; // end class EntidadeParametrizacaoUrna

} // namespace ModuloParametrizacaoUrna

#endif // __MODULOPARAMETRIZACAOURNA_H
