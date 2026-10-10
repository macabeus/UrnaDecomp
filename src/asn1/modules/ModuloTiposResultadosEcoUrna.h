#ifndef __MODULOTIPOSRESULTADOSECOURNA_H
#define __MODULOTIPOSRESULTADOSECOURNA_H

#include "asn1.h"

#include "ModuloFotosCandidatos.h"
#include "ModuloTiposEcoUrna.h"
#include "ModuloTiposEleitorais.h"

namespace ModuloTiposResultadosEcoUrna {

//
// ModuloTiposResultadosEcoUrna::Identificacao   // SYNTHESIZED name (field ['identificacao', 'identificacao', 'identificacaoUrna'])
//

class Identificacao : public ASN1::CHOICE
{
    typedef ASN1::CHOICE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    Identificacao(const void* info) : Inherited(info) {}
public:
    Identificacao() : Inherited(&theInfo) {}
    enum Choice_Ids {
      unknownSelection_ = -2,
      unselected_ = -1,
      identificacaoSecaoEleitoral0 = 0,
      identificacaoContingencia1 = 1
    };

    class identificacaoSecaoEleitoral
    {
    public:
        enum Id { eid = 0 };
        typedef ModuloTiposEleitorais::IdentificacaoSecaoEleitoral value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class identificacaoSecaoEleitoral

    identificacaoSecaoEleitoral::const_reference get_identificacaoSecaoEleitoral () const
    {
        assert(currentSelection() == identificacaoSecaoEleitoral::eid);
        return *static_cast<identificacaoSecaoEleitoral::const_pointer>(choice.get());
    }
    identificacaoSecaoEleitoral::reference ref_identificacaoSecaoEleitoral ()
    {
        assert(currentSelection() == identificacaoSecaoEleitoral::eid);
        return *static_cast<identificacaoSecaoEleitoral::pointer>(choice.get());
    }
    identificacaoSecaoEleitoral::reference select_identificacaoSecaoEleitoral ()
    {
        return *static_cast<identificacaoSecaoEleitoral::pointer>(setSelection(identificacaoSecaoEleitoral::eid, ASN1::AbstractData::create(&identificacaoSecaoEleitoral::value_type::theInfo)));
    }
    bool identificacaoSecaoEleitoral_isSelected () const { return currentSelection() == identificacaoSecaoEleitoral::eid; }

    class identificacaoContingencia
    {
    public:
        enum Id { eid = 1 };
        typedef ModuloTiposEleitorais::IdentificacaoContingencia value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class identificacaoContingencia

    identificacaoContingencia::const_reference get_identificacaoContingencia () const
    {
        assert(currentSelection() == identificacaoContingencia::eid);
        return *static_cast<identificacaoContingencia::const_pointer>(choice.get());
    }
    identificacaoContingencia::reference ref_identificacaoContingencia ()
    {
        assert(currentSelection() == identificacaoContingencia::eid);
        return *static_cast<identificacaoContingencia::pointer>(choice.get());
    }
    identificacaoContingencia::reference select_identificacaoContingencia ()
    {
        return *static_cast<identificacaoContingencia::pointer>(setSelection(identificacaoContingencia::eid, ASN1::AbstractData::create(&identificacaoContingencia::value_type::theInfo)));
    }
    bool identificacaoContingencia_isSelected () const { return currentSelection() == identificacaoContingencia::eid; }

    Identificacao(const Identificacao & that) : Inherited(that) {}
    Identificacao & operator=(const Identificacao & that)
    { Inherited::operator=(that); return *this; }
    void swap(Identificacao & that) { Inherited::swap(that); }
    Identificacao * clone() const
    { return static_cast<Identificacao*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const void* selectionInfos[2];
    static const char* selectionNames[2];
}; // end class Identificacao

//
// ModuloTiposResultadosEcoUrna::CorrespondenciaResultado
//

class CorrespondenciaResultado : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    CorrespondenciaResultado(const void* info) : Inherited(info) {}
public:
    CorrespondenciaResultado() : Inherited(&theInfo) {}
    CorrespondenciaResultado(const CorrespondenciaResultado & that) : Inherited(that) {}

    CorrespondenciaResultado& operator=(const CorrespondenciaResultado& that)
    { Inherited::operator=(that); return *this; }


    class identificacao
    {
    public:
        typedef ModuloTiposResultadosEcoUrna::Identificacao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class identificacao

    identificacao::const_reference get_identificacao () const { return *static_cast<identificacao::const_pointer>(fields[0]); }
    identificacao::reference ref_identificacao () { return *static_cast<identificacao::pointer>(fields[0]); }
    identificacao::reference set_identificacao () { return *static_cast<identificacao::pointer>(fields[0]); }

    class carga
    {
    public:
        typedef ModuloTiposEcoUrna::Carga value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class carga

    carga::const_reference get_carga () const { return *static_cast<carga::const_pointer>(fields[1]); }
    carga::reference ref_carga () { return *static_cast<carga::pointer>(fields[1]); }
    carga::reference set_carga () { return *static_cast<carga::pointer>(fields[1]); }
    void swap(CorrespondenciaResultado& that) { Inherited::swap(that); }
    CorrespondenciaResultado * clone() const
    { return static_cast<CorrespondenciaResultado*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[2];
    static int fieldIds[2];
    static const char* fieldNames[2];
}; // end class CorrespondenciaResultado

//
// ModuloTiposResultadosEcoUrna::TipoArquivo   // named by exact field/RTTI match
//

class TipoArquivo : public ASN1::ENUMERATED
{
    typedef ASN1::ENUMERATED Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    TipoArquivo(const void* info) : Inherited(info) {}
public:
    TipoArquivo() : Inherited(&theInfo) {}
    enum NamedNumber {
        unknownEnumeration_ = -1,
        votacaoUE = 1,
        votacaoRED,
        saMistaMRParcialCedula,
        saMistaBUImpressoCedula,
        saManual,
        saEletronica
    };

    bool is_votacaoUE() const { return value == votacaoUE; }
    void set_votacaoUE() { value = votacaoUE; }

    bool is_votacaoRED() const { return value == votacaoRED; }
    void set_votacaoRED() { value = votacaoRED; }

    bool is_saMistaMRParcialCedula() const { return value == saMistaMRParcialCedula; }
    void set_saMistaMRParcialCedula() { value = saMistaMRParcialCedula; }

    bool is_saMistaBUImpressoCedula() const { return value == saMistaBUImpressoCedula; }
    void set_saMistaBUImpressoCedula() { value = saMistaBUImpressoCedula; }

    bool is_saManual() const { return value == saManual; }
    void set_saManual() { value = saManual; }

    bool is_saEletronica() const { return value == saEletronica; }
    void set_saEletronica() { value = saEletronica; }

    TipoArquivo(const NamedNumber v) : Inherited(&theInfo) { setFromInt(v); }
    TipoArquivo & operator=(const NamedNumber v) { setFromInt(v);  return *this; }
    operator NamedNumber() const { return NamedNumber(asInt()); }

    bool operator == (NamedNumber rhs) const { return value == rhs; }
    bool operator != (NamedNumber rhs) const { return value != rhs; }
    bool operator <  (NamedNumber rhs) const { return value <  rhs; }
    bool operator >  (NamedNumber rhs) const { return value >  rhs; }
    bool operator <= (NamedNumber rhs) const { return value <= rhs; }
    bool operator >= (NamedNumber rhs) const { return value >= rhs; }
    bool operator == (const TipoArquivo &rhs) const { return value == rhs.value; }
    bool operator != (const TipoArquivo &rhs) const { return value != rhs.value; }

    void swap (TipoArquivo & that) { Inherited::swap(that); }
    TipoArquivo * clone() const
    { return static_cast<TipoArquivo*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const char * nameList[];
}; // end class TipoArquivo

//
// ModuloTiposResultadosEcoUrna::TipoUrna   // named by exact field/RTTI match
//

class TipoUrna : public ASN1::ENUMERATED
{
    typedef ASN1::ENUMERATED Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    TipoUrna(const void* info) : Inherited(info) {}
public:
    TipoUrna() : Inherited(&theInfo) {}
    enum NamedNumber {
        unknownEnumeration_ = -1,
        secao = 1,
        contingencia = 3,
        contingenciaSecao,
        contingenciaEncerrandoSecao = 6
    };

    bool is_secao() const { return value == secao; }
    void set_secao() { value = secao; }

    bool is_contingencia() const { return value == contingencia; }
    void set_contingencia() { value = contingencia; }

    bool is_contingenciaSecao() const { return value == contingenciaSecao; }
    void set_contingenciaSecao() { value = contingenciaSecao; }

    bool is_contingenciaEncerrandoSecao() const { return value == contingenciaEncerrandoSecao; }
    void set_contingenciaEncerrandoSecao() { value = contingenciaEncerrandoSecao; }

    TipoUrna(const NamedNumber v) : Inherited(&theInfo) { setFromInt(v); }
    TipoUrna & operator=(const NamedNumber v) { setFromInt(v);  return *this; }
    operator NamedNumber() const { return NamedNumber(asInt()); }

    bool operator == (NamedNumber rhs) const { return value == rhs; }
    bool operator != (NamedNumber rhs) const { return value != rhs; }
    bool operator <  (NamedNumber rhs) const { return value <  rhs; }
    bool operator >  (NamedNumber rhs) const { return value >  rhs; }
    bool operator <= (NamedNumber rhs) const { return value <= rhs; }
    bool operator >= (NamedNumber rhs) const { return value >= rhs; }
    bool operator == (const TipoUrna &rhs) const { return value == rhs.value; }
    bool operator != (const TipoUrna &rhs) const { return value != rhs.value; }

    void swap (TipoUrna & that) { Inherited::swap(that); }
    TipoUrna * clone() const
    { return static_cast<TipoUrna*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const char * nameList[];
}; // end class TipoUrna

//
// ModuloTiposResultadosEcoUrna::TipoApuracao   // SYNTHESIZED name (field ['tipoApuracao', 'tipoapuracao', 'tipoapuracao', 'tipoapuracao'])
//

class TipoApuracao : public ASN1::ENUMERATED
{
    typedef ASN1::ENUMERATED Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    TipoApuracao(const void* info) : Inherited(info) {}
public:
    TipoApuracao() : Inherited(&theInfo) {}
    enum NamedNumber {
        unknownEnumeration_ = -1,
        totalmenteManual = 1,
        totalmenteEletronica,
        mistaBU,
        mistaMR
    };

    bool is_totalmenteManual() const { return value == totalmenteManual; }
    void set_totalmenteManual() { value = totalmenteManual; }

    bool is_totalmenteEletronica() const { return value == totalmenteEletronica; }
    void set_totalmenteEletronica() { value = totalmenteEletronica; }

    bool is_mistaBU() const { return value == mistaBU; }
    void set_mistaBU() { value = mistaBU; }

    bool is_mistaMR() const { return value == mistaMR; }
    void set_mistaMR() { value = mistaMR; }

    TipoApuracao(const NamedNumber v) : Inherited(&theInfo) { setFromInt(v); }
    TipoApuracao & operator=(const NamedNumber v) { setFromInt(v);  return *this; }
    operator NamedNumber() const { return NamedNumber(asInt()); }

    bool operator == (NamedNumber rhs) const { return value == rhs; }
    bool operator != (NamedNumber rhs) const { return value != rhs; }
    bool operator <  (NamedNumber rhs) const { return value <  rhs; }
    bool operator >  (NamedNumber rhs) const { return value >  rhs; }
    bool operator <= (NamedNumber rhs) const { return value <= rhs; }
    bool operator >= (NamedNumber rhs) const { return value >= rhs; }
    bool operator == (const TipoApuracao &rhs) const { return value == rhs.value; }
    bool operator != (const TipoApuracao &rhs) const { return value != rhs.value; }

    void swap (TipoApuracao & that) { Inherited::swap(that); }
    TipoApuracao * clone() const
    { return static_cast<TipoApuracao*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const char * nameList[];
}; // end class TipoApuracao

//
// ModuloTiposResultadosEcoUrna::ApuracaoMistaMR
//

class ApuracaoMistaMR : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    ApuracaoMistaMR(const void* info) : Inherited(info) {}
public:
    ApuracaoMistaMR() : Inherited(&theInfo) {}
    ApuracaoMistaMR(const ApuracaoMistaMR & that) : Inherited(that) {}

    ApuracaoMistaMR& operator=(const ApuracaoMistaMR& that)
    { Inherited::operator=(that); return *this; }


    class tipoApuracao
    {
    public:
        typedef ModuloTiposResultadosEcoUrna::TipoApuracao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class tipoApuracao

    tipoApuracao::const_reference get_tipoApuracao () const { return *static_cast<tipoApuracao::const_pointer>(fields[0]); }
    tipoApuracao::reference ref_tipoApuracao () { return *static_cast<tipoApuracao::pointer>(fields[0]); }
    tipoApuracao::reference set_tipoApuracao () { return *static_cast<tipoApuracao::pointer>(fields[0]); }
    tipoApuracao::reference set_tipoApuracao (ModuloTiposResultadosEcoUrna::TipoApuracao::NamedNumber v)
    { return *static_cast<tipoApuracao::pointer>(fields[0]) = v; }

    class motivoApuracao
    {
    public:
        //
        // ModuloTiposResultadosEcoUrna::ApuracaoMistaMR::motivoApuracao::value_type
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
                naoObteveExitoContingencia = 1,
                indisponibilidadeUrnaContingencia,
                indisponibilidadeFlashContingencia,
                problemaEnergiaEletrica,
                naoFoiPossivelTrocarUrna,
                naoFoiSolicitadaTrocaUrna,
                outros = 99
            };

            bool is_naoObteveExitoContingencia() const { return value == naoObteveExitoContingencia; }
            void set_naoObteveExitoContingencia() { value = naoObteveExitoContingencia; }

            bool is_indisponibilidadeUrnaContingencia() const { return value == indisponibilidadeUrnaContingencia; }
            void set_indisponibilidadeUrnaContingencia() { value = indisponibilidadeUrnaContingencia; }

            bool is_indisponibilidadeFlashContingencia() const { return value == indisponibilidadeFlashContingencia; }
            void set_indisponibilidadeFlashContingencia() { value = indisponibilidadeFlashContingencia; }

            bool is_problemaEnergiaEletrica() const { return value == problemaEnergiaEletrica; }
            void set_problemaEnergiaEletrica() { value = problemaEnergiaEletrica; }

            bool is_naoFoiPossivelTrocarUrna() const { return value == naoFoiPossivelTrocarUrna; }
            void set_naoFoiPossivelTrocarUrna() { value = naoFoiPossivelTrocarUrna; }

            bool is_naoFoiSolicitadaTrocaUrna() const { return value == naoFoiSolicitadaTrocaUrna; }
            void set_naoFoiSolicitadaTrocaUrna() { value = naoFoiSolicitadaTrocaUrna; }

            bool is_outros() const { return value == outros; }
            void set_outros() { value = outros; }

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
    }; // end class motivoApuracao

    motivoApuracao::const_reference get_motivoApuracao () const { return *static_cast<motivoApuracao::const_pointer>(fields[1]); }
    motivoApuracao::reference ref_motivoApuracao () { return *static_cast<motivoApuracao::pointer>(fields[1]); }
    motivoApuracao::reference set_motivoApuracao () { return *static_cast<motivoApuracao::pointer>(fields[1]); }
    motivoApuracao::reference set_motivoApuracao (motivoApuracao::value_type::NamedNumber v)
    { return *static_cast<motivoApuracao::pointer>(fields[1]) = v; }
    void swap(ApuracaoMistaMR& that) { Inherited::swap(that); }
    ApuracaoMistaMR * clone() const
    { return static_cast<ApuracaoMistaMR*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[2];
    static int fieldIds[2];
    static const char* fieldNames[2];
}; // end class ApuracaoMistaMR

//
// ModuloTiposResultadosEcoUrna::ApuracaoMistaBUAE
//

class ApuracaoMistaBUAE : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    ApuracaoMistaBUAE(const void* info) : Inherited(info) {}
public:
    ApuracaoMistaBUAE() : Inherited(&theInfo) {}
    ApuracaoMistaBUAE(const ApuracaoMistaBUAE & that) : Inherited(that) {}

    ApuracaoMistaBUAE& operator=(const ApuracaoMistaBUAE& that)
    { Inherited::operator=(that); return *this; }


    class tipoapuracao
    {
    public:
        typedef ModuloTiposResultadosEcoUrna::TipoApuracao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class tipoapuracao

    tipoapuracao::const_reference get_tipoapuracao () const { return *static_cast<tipoapuracao::const_pointer>(fields[0]); }
    tipoapuracao::reference ref_tipoapuracao () { return *static_cast<tipoapuracao::pointer>(fields[0]); }
    tipoapuracao::reference set_tipoapuracao () { return *static_cast<tipoapuracao::pointer>(fields[0]); }
    tipoapuracao::reference set_tipoapuracao (ModuloTiposResultadosEcoUrna::TipoApuracao::NamedNumber v)
    { return *static_cast<tipoapuracao::pointer>(fields[0]) = v; }

    class motivoApuracao
    {
    public:
        //
        // ModuloTiposResultadosEcoUrna::ApuracaoMistaBUAE::motivoApuracao::value_type
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
                urnaDataHoraIncorreta = 1,
                urnaComDefeito,
                urnaOutrasecao,
                urnaPreparadaIncorretamente,
                urnaEncerradaComEleitoresNaFila,
                outros = 99
            };

            bool is_urnaDataHoraIncorreta() const { return value == urnaDataHoraIncorreta; }
            void set_urnaDataHoraIncorreta() { value = urnaDataHoraIncorreta; }

            bool is_urnaComDefeito() const { return value == urnaComDefeito; }
            void set_urnaComDefeito() { value = urnaComDefeito; }

            bool is_urnaOutrasecao() const { return value == urnaOutrasecao; }
            void set_urnaOutrasecao() { value = urnaOutrasecao; }

            bool is_urnaPreparadaIncorretamente() const { return value == urnaPreparadaIncorretamente; }
            void set_urnaPreparadaIncorretamente() { value = urnaPreparadaIncorretamente; }

            bool is_urnaEncerradaComEleitoresNaFila() const { return value == urnaEncerradaComEleitoresNaFila; }
            void set_urnaEncerradaComEleitoresNaFila() { value = urnaEncerradaComEleitoresNaFila; }

            bool is_outros() const { return value == outros; }
            void set_outros() { value = outros; }

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
    }; // end class motivoApuracao

    motivoApuracao::const_reference get_motivoApuracao () const { return *static_cast<motivoApuracao::const_pointer>(fields[1]); }
    motivoApuracao::reference ref_motivoApuracao () { return *static_cast<motivoApuracao::pointer>(fields[1]); }
    motivoApuracao::reference set_motivoApuracao () { return *static_cast<motivoApuracao::pointer>(fields[1]); }
    motivoApuracao::reference set_motivoApuracao (motivoApuracao::value_type::NamedNumber v)
    { return *static_cast<motivoApuracao::pointer>(fields[1]) = v; }
    void swap(ApuracaoMistaBUAE& that) { Inherited::swap(that); }
    ApuracaoMistaBUAE * clone() const
    { return static_cast<ApuracaoMistaBUAE*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[2];
    static int fieldIds[2];
    static const char* fieldNames[2];
}; // end class ApuracaoMistaBUAE

//
// ModuloTiposResultadosEcoUrna::ApuracaoTotalmenteManualDigitacaoAE
//

class ApuracaoTotalmenteManualDigitacaoAE : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    ApuracaoTotalmenteManualDigitacaoAE(const void* info) : Inherited(info) {}
public:
    ApuracaoTotalmenteManualDigitacaoAE() : Inherited(&theInfo) {}
    ApuracaoTotalmenteManualDigitacaoAE(const ApuracaoTotalmenteManualDigitacaoAE & that) : Inherited(that) {}

    ApuracaoTotalmenteManualDigitacaoAE& operator=(const ApuracaoTotalmenteManualDigitacaoAE& that)
    { Inherited::operator=(that); return *this; }


    class tipoapuracao
    {
    public:
        typedef ModuloTiposResultadosEcoUrna::TipoApuracao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class tipoapuracao

    tipoapuracao::const_reference get_tipoapuracao () const { return *static_cast<tipoapuracao::const_pointer>(fields[0]); }
    tipoapuracao::reference ref_tipoapuracao () { return *static_cast<tipoapuracao::pointer>(fields[0]); }
    tipoapuracao::reference set_tipoapuracao () { return *static_cast<tipoapuracao::pointer>(fields[0]); }
    tipoapuracao::reference set_tipoapuracao (ModuloTiposResultadosEcoUrna::TipoApuracao::NamedNumber v)
    { return *static_cast<tipoapuracao::pointer>(fields[0]) = v; }

    class motivoApuracao
    {
    public:
        //
        // ModuloTiposResultadosEcoUrna::ApuracaoTotalmenteManualDigitacaoAE::motivoApuracao::value_type
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
                urnaComDefeito = 1,
                urnaIndisponivelInicio,
                urnaOutraSecao,
                outros = 99
            };

            bool is_urnaComDefeito() const { return value == urnaComDefeito; }
            void set_urnaComDefeito() { value = urnaComDefeito; }

            bool is_urnaIndisponivelInicio() const { return value == urnaIndisponivelInicio; }
            void set_urnaIndisponivelInicio() { value = urnaIndisponivelInicio; }

            bool is_urnaOutraSecao() const { return value == urnaOutraSecao; }
            void set_urnaOutraSecao() { value = urnaOutraSecao; }

            bool is_outros() const { return value == outros; }
            void set_outros() { value = outros; }

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
    }; // end class motivoApuracao

    motivoApuracao::const_reference get_motivoApuracao () const { return *static_cast<motivoApuracao::const_pointer>(fields[1]); }
    motivoApuracao::reference ref_motivoApuracao () { return *static_cast<motivoApuracao::pointer>(fields[1]); }
    motivoApuracao::reference set_motivoApuracao () { return *static_cast<motivoApuracao::pointer>(fields[1]); }
    motivoApuracao::reference set_motivoApuracao (motivoApuracao::value_type::NamedNumber v)
    { return *static_cast<motivoApuracao::pointer>(fields[1]) = v; }
    void swap(ApuracaoTotalmenteManualDigitacaoAE& that) { Inherited::swap(that); }
    ApuracaoTotalmenteManualDigitacaoAE * clone() const
    { return static_cast<ApuracaoTotalmenteManualDigitacaoAE*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[2];
    static int fieldIds[2];
    static const char* fieldNames[2];
}; // end class ApuracaoTotalmenteManualDigitacaoAE

//
// ModuloTiposResultadosEcoUrna::ApuracaoEletronica
//

class ApuracaoEletronica : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    ApuracaoEletronica(const void* info) : Inherited(info) {}
public:
    ApuracaoEletronica() : Inherited(&theInfo) {}
    ApuracaoEletronica(const ApuracaoEletronica & that) : Inherited(that) {}

    ApuracaoEletronica& operator=(const ApuracaoEletronica& that)
    { Inherited::operator=(that); return *this; }


    class tipoapuracao
    {
    public:
        typedef ModuloTiposResultadosEcoUrna::TipoApuracao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class tipoapuracao

    tipoapuracao::const_reference get_tipoapuracao () const { return *static_cast<tipoapuracao::const_pointer>(fields[0]); }
    tipoapuracao::reference ref_tipoapuracao () { return *static_cast<tipoapuracao::pointer>(fields[0]); }
    tipoapuracao::reference set_tipoapuracao () { return *static_cast<tipoapuracao::pointer>(fields[0]); }
    tipoapuracao::reference set_tipoapuracao (ModuloTiposResultadosEcoUrna::TipoApuracao::NamedNumber v)
    { return *static_cast<tipoapuracao::pointer>(fields[0]) = v; }

    class motivoApuracao
    {
    public:
        //
        // ModuloTiposResultadosEcoUrna::ApuracaoEletronica::motivoApuracao::value_type
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
                naoFoiPossivelReuperarResultado = 1,
                urnaNaoChegouMidiaDefeituosa,
                urnaNaoChegouMidiaExtraviada,
                outros = 99
            };

            bool is_naoFoiPossivelReuperarResultado() const { return value == naoFoiPossivelReuperarResultado; }
            void set_naoFoiPossivelReuperarResultado() { value = naoFoiPossivelReuperarResultado; }

            bool is_urnaNaoChegouMidiaDefeituosa() const { return value == urnaNaoChegouMidiaDefeituosa; }
            void set_urnaNaoChegouMidiaDefeituosa() { value = urnaNaoChegouMidiaDefeituosa; }

            bool is_urnaNaoChegouMidiaExtraviada() const { return value == urnaNaoChegouMidiaExtraviada; }
            void set_urnaNaoChegouMidiaExtraviada() { value = urnaNaoChegouMidiaExtraviada; }

            bool is_outros() const { return value == outros; }
            void set_outros() { value = outros; }

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
    }; // end class motivoApuracao

    motivoApuracao::const_reference get_motivoApuracao () const { return *static_cast<motivoApuracao::const_pointer>(fields[1]); }
    motivoApuracao::reference ref_motivoApuracao () { return *static_cast<motivoApuracao::pointer>(fields[1]); }
    motivoApuracao::reference set_motivoApuracao () { return *static_cast<motivoApuracao::pointer>(fields[1]); }
    motivoApuracao::reference set_motivoApuracao (motivoApuracao::value_type::NamedNumber v)
    { return *static_cast<motivoApuracao::pointer>(fields[1]) = v; }
    void swap(ApuracaoEletronica& that) { Inherited::swap(that); }
    ApuracaoEletronica * clone() const
    { return static_cast<ApuracaoEletronica*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[2];
    static int fieldIds[2];
    static const char* fieldNames[2];
}; // end class ApuracaoEletronica

//
// ModuloTiposResultadosEcoUrna::Urna
//

class Urna : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    Urna(const void* info) : Inherited(info) {}
public:
    Urna() : Inherited(&theInfo) {}
    Urna(const Urna & that) : Inherited(that) {}

    Urna& operator=(const Urna& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_motivoUtilizacaoSA
    };

    class tipoUrna
    {
    public:
        typedef ModuloTiposResultadosEcoUrna::TipoUrna value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class tipoUrna

    tipoUrna::const_reference get_tipoUrna () const { return *static_cast<tipoUrna::const_pointer>(fields[0]); }
    tipoUrna::reference ref_tipoUrna () { return *static_cast<tipoUrna::pointer>(fields[0]); }
    tipoUrna::reference set_tipoUrna () { return *static_cast<tipoUrna::pointer>(fields[0]); }
    tipoUrna::reference set_tipoUrna (ModuloTiposResultadosEcoUrna::TipoUrna::NamedNumber v)
    { return *static_cast<tipoUrna::pointer>(fields[0]) = v; }

    class versaoVotacao
    {
    public:
        typedef ASN1::GeneralString value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class versaoVotacao

    versaoVotacao::const_reference get_versaoVotacao () const { return *static_cast<versaoVotacao::const_pointer>(fields[1]); }
    versaoVotacao::reference ref_versaoVotacao () { return *static_cast<versaoVotacao::pointer>(fields[1]); }
    versaoVotacao::reference set_versaoVotacao () { return *static_cast<versaoVotacao::pointer>(fields[1]); }
    versaoVotacao::reference set_versaoVotacao (const ASN1_STD string& v)
    { return *static_cast<versaoVotacao::pointer>(fields[1]) = v; }

    class correspondenciaResultado
    {
    public:
        typedef ModuloTiposResultadosEcoUrna::CorrespondenciaResultado value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class correspondenciaResultado

    correspondenciaResultado::const_reference get_correspondenciaResultado () const { return *static_cast<correspondenciaResultado::const_pointer>(fields[2]); }
    correspondenciaResultado::reference ref_correspondenciaResultado () { return *static_cast<correspondenciaResultado::pointer>(fields[2]); }
    correspondenciaResultado::reference set_correspondenciaResultado () { return *static_cast<correspondenciaResultado::pointer>(fields[2]); }

    class tipoArquivo
    {
    public:
        typedef ModuloTiposResultadosEcoUrna::TipoArquivo value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class tipoArquivo

    tipoArquivo::const_reference get_tipoArquivo () const { return *static_cast<tipoArquivo::const_pointer>(fields[3]); }
    tipoArquivo::reference ref_tipoArquivo () { return *static_cast<tipoArquivo::pointer>(fields[3]); }
    tipoArquivo::reference set_tipoArquivo () { return *static_cast<tipoArquivo::pointer>(fields[3]); }
    tipoArquivo::reference set_tipoArquivo (ModuloTiposResultadosEcoUrna::TipoArquivo::NamedNumber v)
    { return *static_cast<tipoArquivo::pointer>(fields[3]) = v; }

    class numeroSerieFV
    {
    public:
        typedef ASN1::Constrained_OCTET_STRING<ASN1::SizeConstraint<ASN1::FixedConstraint, 4, 4> > value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class numeroSerieFV

    numeroSerieFV::const_reference get_numeroSerieFV () const { return *static_cast<numeroSerieFV::const_pointer>(fields[4]); }
    numeroSerieFV::reference ref_numeroSerieFV () { return *static_cast<numeroSerieFV::pointer>(fields[4]); }
    numeroSerieFV::reference set_numeroSerieFV () { return *static_cast<numeroSerieFV::pointer>(fields[4]); }
    numeroSerieFV::reference set_numeroSerieFV (const ASN1::OCTET_STRING& v)
    { return *static_cast<numeroSerieFV::pointer>(fields[4]) = v; }

    class motivoUtilizacaoSA
    {
    public:
        //
        // ModuloTiposResultadosEcoUrna::Urna::motivoUtilizacaoSA::value_type
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
              apuracaoMistaMR0 = 0,
              apuracaoMistaBUAE1 = 1,
              apuracaoTotalmenteManual2 = 2,
              apuracaoEletronica3 = 3
            };

            class apuracaoMistaMR
            {
            public:
                enum Id { eid = 0 };
                typedef ModuloTiposResultadosEcoUrna::ApuracaoMistaMR value_type;
                typedef value_type&        reference;
                typedef const value_type&  const_reference;
                typedef value_type*        pointer;
                typedef const value_type*  const_pointer;
            }; // end class apuracaoMistaMR

            apuracaoMistaMR::const_reference get_apuracaoMistaMR () const
            {
                assert(currentSelection() == apuracaoMistaMR::eid);
                return *static_cast<apuracaoMistaMR::const_pointer>(choice.get());
            }
            apuracaoMistaMR::reference ref_apuracaoMistaMR ()
            {
                assert(currentSelection() == apuracaoMistaMR::eid);
                return *static_cast<apuracaoMistaMR::pointer>(choice.get());
            }
            apuracaoMistaMR::reference select_apuracaoMistaMR ()
            {
                return *static_cast<apuracaoMistaMR::pointer>(setSelection(apuracaoMistaMR::eid, ASN1::AbstractData::create(&apuracaoMistaMR::value_type::theInfo)));
            }
            bool apuracaoMistaMR_isSelected () const { return currentSelection() == apuracaoMistaMR::eid; }

            class apuracaoMistaBUAE
            {
            public:
                enum Id { eid = 1 };
                typedef ModuloTiposResultadosEcoUrna::ApuracaoMistaBUAE value_type;
                typedef value_type&        reference;
                typedef const value_type&  const_reference;
                typedef value_type*        pointer;
                typedef const value_type*  const_pointer;
            }; // end class apuracaoMistaBUAE

            apuracaoMistaBUAE::const_reference get_apuracaoMistaBUAE () const
            {
                assert(currentSelection() == apuracaoMistaBUAE::eid);
                return *static_cast<apuracaoMistaBUAE::const_pointer>(choice.get());
            }
            apuracaoMistaBUAE::reference ref_apuracaoMistaBUAE ()
            {
                assert(currentSelection() == apuracaoMistaBUAE::eid);
                return *static_cast<apuracaoMistaBUAE::pointer>(choice.get());
            }
            apuracaoMistaBUAE::reference select_apuracaoMistaBUAE ()
            {
                return *static_cast<apuracaoMistaBUAE::pointer>(setSelection(apuracaoMistaBUAE::eid, ASN1::AbstractData::create(&apuracaoMistaBUAE::value_type::theInfo)));
            }
            bool apuracaoMistaBUAE_isSelected () const { return currentSelection() == apuracaoMistaBUAE::eid; }

            class apuracaoTotalmenteManual
            {
            public:
                enum Id { eid = 2 };
                typedef ModuloTiposResultadosEcoUrna::ApuracaoTotalmenteManualDigitacaoAE value_type;
                typedef value_type&        reference;
                typedef const value_type&  const_reference;
                typedef value_type*        pointer;
                typedef const value_type*  const_pointer;
            }; // end class apuracaoTotalmenteManual

            apuracaoTotalmenteManual::const_reference get_apuracaoTotalmenteManual () const
            {
                assert(currentSelection() == apuracaoTotalmenteManual::eid);
                return *static_cast<apuracaoTotalmenteManual::const_pointer>(choice.get());
            }
            apuracaoTotalmenteManual::reference ref_apuracaoTotalmenteManual ()
            {
                assert(currentSelection() == apuracaoTotalmenteManual::eid);
                return *static_cast<apuracaoTotalmenteManual::pointer>(choice.get());
            }
            apuracaoTotalmenteManual::reference select_apuracaoTotalmenteManual ()
            {
                return *static_cast<apuracaoTotalmenteManual::pointer>(setSelection(apuracaoTotalmenteManual::eid, ASN1::AbstractData::create(&apuracaoTotalmenteManual::value_type::theInfo)));
            }
            bool apuracaoTotalmenteManual_isSelected () const { return currentSelection() == apuracaoTotalmenteManual::eid; }

            class apuracaoEletronica
            {
            public:
                enum Id { eid = 3 };
                typedef ModuloTiposResultadosEcoUrna::ApuracaoEletronica value_type;
                typedef value_type&        reference;
                typedef const value_type&  const_reference;
                typedef value_type*        pointer;
                typedef const value_type*  const_pointer;
            }; // end class apuracaoEletronica

            apuracaoEletronica::const_reference get_apuracaoEletronica () const
            {
                assert(currentSelection() == apuracaoEletronica::eid);
                return *static_cast<apuracaoEletronica::const_pointer>(choice.get());
            }
            apuracaoEletronica::reference ref_apuracaoEletronica ()
            {
                assert(currentSelection() == apuracaoEletronica::eid);
                return *static_cast<apuracaoEletronica::pointer>(choice.get());
            }
            apuracaoEletronica::reference select_apuracaoEletronica ()
            {
                return *static_cast<apuracaoEletronica::pointer>(setSelection(apuracaoEletronica::eid, ASN1::AbstractData::create(&apuracaoEletronica::value_type::theInfo)));
            }
            bool apuracaoEletronica_isSelected () const { return currentSelection() == apuracaoEletronica::eid; }

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
            static const void* selectionInfos[4];
            static const char* selectionNames[4];
        }; // end class value_type

        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class motivoUtilizacaoSA

    motivoUtilizacaoSA::const_reference get_motivoUtilizacaoSA () const
    {
      assert(hasOptionalField(e_motivoUtilizacaoSA));
      return *static_cast<motivoUtilizacaoSA::const_pointer>(fields[5]);
    }
    motivoUtilizacaoSA::reference ref_motivoUtilizacaoSA ()
    {
      assert(hasOptionalField(e_motivoUtilizacaoSA));
      return *static_cast<motivoUtilizacaoSA::pointer>(fields[5]);
    }
    motivoUtilizacaoSA::reference set_motivoUtilizacaoSA ()
    {
      includeOptionalField( e_motivoUtilizacaoSA, 5);
      return *static_cast<motivoUtilizacaoSA::pointer>(fields[5]);
    }
    void omit_motivoUtilizacaoSA () { removeOptionalField( e_motivoUtilizacaoSA); }
    bool motivoUtilizacaoSA_isPresent () const { return hasOptionalField( e_motivoUtilizacaoSA); }
    void swap(Urna& that) { Inherited::swap(that); }
    Urna * clone() const
    { return static_cast<Urna*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[6];
    static int fieldIds[6];
    static const char* fieldNames[6];
}; // end class Urna

//
// ModuloTiposResultadosEcoUrna::DadosCandidato
//

class DadosCandidato : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    DadosCandidato(const void* info) : Inherited(info) {}
public:
    DadosCandidato() : Inherited(&theInfo) {}
    DadosCandidato(const DadosCandidato & that) : Inherited(that) {}

    DadosCandidato& operator=(const DadosCandidato& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_candidatoSubstituido,
      e_nomeSocial,
      e_nomeFonetico,
      e_situacaoCassacao,
      e_ordemSuplencia
    };

    class codigo
    {
    public:
        typedef ModuloFotosCandidatos::CodigoCandidato value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class codigo

    codigo::const_reference get_codigo () const { return *static_cast<codigo::const_pointer>(fields[0]); }
    codigo::reference ref_codigo () { return *static_cast<codigo::pointer>(fields[0]); }
    codigo::reference set_codigo () { return *static_cast<codigo::pointer>(fields[0]); }
    codigo::reference set_codigo (const ASN1_STD string& v)
    { return *static_cast<codigo::pointer>(fields[0]) = v; }

    class candidatoSubstituido
    {
    public:
        typedef ModuloFotosCandidatos::CodigoCandidato value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class candidatoSubstituido

    candidatoSubstituido::const_reference get_candidatoSubstituido () const
    {
      assert(hasOptionalField(e_candidatoSubstituido));
      return *static_cast<candidatoSubstituido::const_pointer>(fields[1]);
    }
    candidatoSubstituido::reference ref_candidatoSubstituido ()
    {
      assert(hasOptionalField(e_candidatoSubstituido));
      return *static_cast<candidatoSubstituido::pointer>(fields[1]);
    }
    candidatoSubstituido::reference set_candidatoSubstituido ()
    {
      includeOptionalField( e_candidatoSubstituido, 1);
      return *static_cast<candidatoSubstituido::pointer>(fields[1]);
    }
    candidatoSubstituido::reference set_candidatoSubstituido (const ASN1_STD string& v)
    {
      includeOptionalField( e_candidatoSubstituido, 1);
      return *static_cast<candidatoSubstituido::pointer>(fields[1]) = v;
    }
    void omit_candidatoSubstituido () { removeOptionalField( e_candidatoSubstituido); }
    bool candidatoSubstituido_isPresent () const { return hasOptionalField( e_candidatoSubstituido); }

    class nome
    {
    public:
        //
        // ModuloTiposResultadosEcoUrna::DadosCandidato::nome::value_type
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

    nome::const_reference get_nome () const { return *static_cast<nome::const_pointer>(fields[2]); }
    nome::reference ref_nome () { return *static_cast<nome::pointer>(fields[2]); }
    nome::reference set_nome () { return *static_cast<nome::pointer>(fields[2]); }
    nome::reference set_nome (const ASN1_STD string& v)
    { return *static_cast<nome::pointer>(fields[2]) = v; }

    class nomeUrna
    {
    public:
        //
        // ModuloTiposResultadosEcoUrna::DadosCandidato::nomeUrna::value_type
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
    }; // end class nomeUrna

    nomeUrna::const_reference get_nomeUrna () const { return *static_cast<nomeUrna::const_pointer>(fields[3]); }
    nomeUrna::reference ref_nomeUrna () { return *static_cast<nomeUrna::pointer>(fields[3]); }
    nomeUrna::reference set_nomeUrna () { return *static_cast<nomeUrna::pointer>(fields[3]); }
    nomeUrna::reference set_nomeUrna (const ASN1_STD string& v)
    { return *static_cast<nomeUrna::pointer>(fields[3]) = v; }

    class nomeSocial
    {
    public:
        //
        // ModuloTiposResultadosEcoUrna::DadosCandidato::nomeSocial::value_type
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
    }; // end class nomeSocial

    nomeSocial::const_reference get_nomeSocial () const
    {
      assert(hasOptionalField(e_nomeSocial));
      return *static_cast<nomeSocial::const_pointer>(fields[4]);
    }
    nomeSocial::reference ref_nomeSocial ()
    {
      assert(hasOptionalField(e_nomeSocial));
      return *static_cast<nomeSocial::pointer>(fields[4]);
    }
    nomeSocial::reference set_nomeSocial ()
    {
      includeOptionalField( e_nomeSocial, 4);
      return *static_cast<nomeSocial::pointer>(fields[4]);
    }
    nomeSocial::reference set_nomeSocial (const ASN1_STD string& v)
    {
      includeOptionalField( e_nomeSocial, 4);
      return *static_cast<nomeSocial::pointer>(fields[4]) = v;
    }
    void omit_nomeSocial () { removeOptionalField( e_nomeSocial); }
    bool nomeSocial_isPresent () const { return hasOptionalField( e_nomeSocial); }

    class nomeFonetico
    {
    public:
        typedef ASN1::GeneralString value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class nomeFonetico

    nomeFonetico::const_reference get_nomeFonetico () const
    {
      assert(hasOptionalField(e_nomeFonetico));
      return *static_cast<nomeFonetico::const_pointer>(fields[5]);
    }
    nomeFonetico::reference ref_nomeFonetico ()
    {
      assert(hasOptionalField(e_nomeFonetico));
      return *static_cast<nomeFonetico::pointer>(fields[5]);
    }
    nomeFonetico::reference set_nomeFonetico ()
    {
      includeOptionalField( e_nomeFonetico, 5);
      return *static_cast<nomeFonetico::pointer>(fields[5]);
    }
    nomeFonetico::reference set_nomeFonetico (const ASN1_STD string& v)
    {
      includeOptionalField( e_nomeFonetico, 5);
      return *static_cast<nomeFonetico::pointer>(fields[5]) = v;
    }
    void omit_nomeFonetico () { removeOptionalField( e_nomeFonetico); }
    bool nomeFonetico_isPresent () const { return hasOptionalField( e_nomeFonetico); }

    class dataNascimento
    {
    public:
        //
        // ModuloTiposResultadosEcoUrna::DadosCandidato::dataNascimento::value_type
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
    }; // end class dataNascimento

    dataNascimento::const_reference get_dataNascimento () const { return *static_cast<dataNascimento::const_pointer>(fields[6]); }
    dataNascimento::reference ref_dataNascimento () { return *static_cast<dataNascimento::pointer>(fields[6]); }
    dataNascimento::reference set_dataNascimento () { return *static_cast<dataNascimento::pointer>(fields[6]); }
    dataNascimento::reference set_dataNascimento (const ASN1_STD string& v)
    { return *static_cast<dataNascimento::pointer>(fields[6]) = v; }

    class sexo
    {
    public:
        //
        // ModuloTiposResultadosEcoUrna::DadosCandidato::sexo::value_type
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
                sexoNaoInformado,
                sexoMasculino = 2,
                sexoFeminino = 4
            };

            bool is_sexoNaoInformado() const { return value == sexoNaoInformado; }
            void set_sexoNaoInformado() { value = sexoNaoInformado; }

            bool is_sexoMasculino() const { return value == sexoMasculino; }
            void set_sexoMasculino() { value = sexoMasculino; }

            bool is_sexoFeminino() const { return value == sexoFeminino; }
            void set_sexoFeminino() { value = sexoFeminino; }

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
    }; // end class sexo

    sexo::const_reference get_sexo () const { return *static_cast<sexo::const_pointer>(fields[7]); }
    sexo::reference ref_sexo () { return *static_cast<sexo::pointer>(fields[7]); }
    sexo::reference set_sexo () { return *static_cast<sexo::pointer>(fields[7]); }
    sexo::reference set_sexo (sexo::value_type::NamedNumber v)
    { return *static_cast<sexo::pointer>(fields[7]) = v; }

    class situacao
    {
    public:
        //
        // ModuloTiposResultadosEcoUrna::DadosCandidato::situacao::value_type
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
                cadastrado = 1,
                deferido,
                inapto,
                indeferidoComRecurso,
                cancelado,
                renuncia,
                falecido,
                aguardandoJulgamento,
                impugnado = 11,
                apto,
                pedidoNaoConhecido,
                indeferido,
                comNoticiaInelegibilidade,
                deferidoComRecurso,
                candidatoPendenteJulgamento,
                canceladoComRecurso = 19,
                pedidoNaoConhecidoComRecurso
            };

            bool is_cadastrado() const { return value == cadastrado; }
            void set_cadastrado() { value = cadastrado; }

            bool is_deferido() const { return value == deferido; }
            void set_deferido() { value = deferido; }

            bool is_inapto() const { return value == inapto; }
            void set_inapto() { value = inapto; }

            bool is_indeferidoComRecurso() const { return value == indeferidoComRecurso; }
            void set_indeferidoComRecurso() { value = indeferidoComRecurso; }

            bool is_cancelado() const { return value == cancelado; }
            void set_cancelado() { value = cancelado; }

            bool is_renuncia() const { return value == renuncia; }
            void set_renuncia() { value = renuncia; }

            bool is_falecido() const { return value == falecido; }
            void set_falecido() { value = falecido; }

            bool is_aguardandoJulgamento() const { return value == aguardandoJulgamento; }
            void set_aguardandoJulgamento() { value = aguardandoJulgamento; }

            bool is_impugnado() const { return value == impugnado; }
            void set_impugnado() { value = impugnado; }

            bool is_apto() const { return value == apto; }
            void set_apto() { value = apto; }

            bool is_pedidoNaoConhecido() const { return value == pedidoNaoConhecido; }
            void set_pedidoNaoConhecido() { value = pedidoNaoConhecido; }

            bool is_indeferido() const { return value == indeferido; }
            void set_indeferido() { value = indeferido; }

            bool is_comNoticiaInelegibilidade() const { return value == comNoticiaInelegibilidade; }
            void set_comNoticiaInelegibilidade() { value = comNoticiaInelegibilidade; }

            bool is_deferidoComRecurso() const { return value == deferidoComRecurso; }
            void set_deferidoComRecurso() { value = deferidoComRecurso; }

            bool is_candidatoPendenteJulgamento() const { return value == candidatoPendenteJulgamento; }
            void set_candidatoPendenteJulgamento() { value = candidatoPendenteJulgamento; }

            bool is_canceladoComRecurso() const { return value == canceladoComRecurso; }
            void set_canceladoComRecurso() { value = canceladoComRecurso; }

            bool is_pedidoNaoConhecidoComRecurso() const { return value == pedidoNaoConhecidoComRecurso; }
            void set_pedidoNaoConhecidoComRecurso() { value = pedidoNaoConhecidoComRecurso; }

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
    }; // end class situacao

    situacao::const_reference get_situacao () const { return *static_cast<situacao::const_pointer>(fields[8]); }
    situacao::reference ref_situacao () { return *static_cast<situacao::pointer>(fields[8]); }
    situacao::reference set_situacao () { return *static_cast<situacao::pointer>(fields[8]); }
    situacao::reference set_situacao (situacao::value_type::NamedNumber v)
    { return *static_cast<situacao::pointer>(fields[8]) = v; }

    class situacaoCassacao
    {
    public:
        //
        // ModuloTiposResultadosEcoUrna::DadosCandidato::situacaoCassacao::value_type
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
                cassado = 1,
                cassadoComRecurso,
                regular,
                levantado
            };

            bool is_cassado() const { return value == cassado; }
            void set_cassado() { value = cassado; }

            bool is_cassadoComRecurso() const { return value == cassadoComRecurso; }
            void set_cassadoComRecurso() { value = cassadoComRecurso; }

            bool is_regular() const { return value == regular; }
            void set_regular() { value = regular; }

            bool is_levantado() const { return value == levantado; }
            void set_levantado() { value = levantado; }

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
    }; // end class situacaoCassacao

    situacaoCassacao::const_reference get_situacaoCassacao () const
    {
      assert(hasOptionalField(e_situacaoCassacao));
      return *static_cast<situacaoCassacao::const_pointer>(fields[9]);
    }
    situacaoCassacao::reference ref_situacaoCassacao ()
    {
      assert(hasOptionalField(e_situacaoCassacao));
      return *static_cast<situacaoCassacao::pointer>(fields[9]);
    }
    situacaoCassacao::reference set_situacaoCassacao ()
    {
      includeOptionalField( e_situacaoCassacao, 9);
      return *static_cast<situacaoCassacao::pointer>(fields[9]);
    }
    situacaoCassacao::reference set_situacaoCassacao (situacaoCassacao::value_type::NamedNumber v)
    {
      includeOptionalField( e_situacaoCassacao, 9);
      return *static_cast<situacaoCassacao::pointer>(fields[9]) = v;
    }
    void omit_situacaoCassacao () { removeOptionalField( e_situacaoCassacao); }
    bool situacaoCassacao_isPresent () const { return hasOptionalField( e_situacaoCassacao); }

    class partido
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 99> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class partido

    partido::const_reference get_partido () const { return *static_cast<partido::const_pointer>(fields[10]); }
    partido::reference ref_partido () { return *static_cast<partido::pointer>(fields[10]); }
    partido::reference set_partido () { return *static_cast<partido::pointer>(fields[10]); }
    partido::reference set_partido (ASN1::INTEGER::int_type v)
    { return *static_cast<partido::pointer>(fields[10]) = v; }

    class ordemSuplencia
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 9> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class ordemSuplencia

    ordemSuplencia::const_reference get_ordemSuplencia () const
    {
      assert(hasOptionalField(e_ordemSuplencia));
      return *static_cast<ordemSuplencia::const_pointer>(fields[11]);
    }
    ordemSuplencia::reference ref_ordemSuplencia ()
    {
      assert(hasOptionalField(e_ordemSuplencia));
      return *static_cast<ordemSuplencia::pointer>(fields[11]);
    }
    ordemSuplencia::reference set_ordemSuplencia ()
    {
      includeOptionalField( e_ordemSuplencia, 11);
      return *static_cast<ordemSuplencia::pointer>(fields[11]);
    }
    ordemSuplencia::reference set_ordemSuplencia (ASN1::INTEGER::int_type v)
    {
      includeOptionalField( e_ordemSuplencia, 11);
      return *static_cast<ordemSuplencia::pointer>(fields[11]) = v;
    }
    void omit_ordemSuplencia () { removeOptionalField( e_ordemSuplencia); }
    bool ordemSuplencia_isPresent () const { return hasOptionalField( e_ordemSuplencia); }

    class reeleicao
    {
    public:
        typedef ASN1::BOOLEAN value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class reeleicao

    reeleicao::const_reference get_reeleicao () const { return *static_cast<reeleicao::const_pointer>(fields[12]); }
    reeleicao::reference ref_reeleicao () { return *static_cast<reeleicao::pointer>(fields[12]); }
    reeleicao::reference set_reeleicao () { return *static_cast<reeleicao::pointer>(fields[12]); }
    reeleicao::reference set_reeleicao (bool v)
    { return *static_cast<reeleicao::pointer>(fields[12]) = v; }
    void swap(DadosCandidato& that) { Inherited::swap(that); }
    DadosCandidato * clone() const
    { return static_cast<DadosCandidato*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[13];
    static int fieldIds[13];
    static unsigned fieldTags[13];
    static const char* fieldNames[13];
}; // end class DadosCandidato

//
// ModuloTiposResultadosEcoUrna::Candidatura
//

class Candidatura : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    Candidatura(const void* info) : Inherited(info) {}
public:
    Candidatura() : Inherited(&theInfo) {}
    Candidatura(const Candidatura & that) : Inherited(that) {}

    Candidatura& operator=(const Candidatura& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_suplentesVices
    };

    class numero
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 99999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class numero

    numero::const_reference get_numero () const { return *static_cast<numero::const_pointer>(fields[0]); }
    numero::reference ref_numero () { return *static_cast<numero::pointer>(fields[0]); }
    numero::reference set_numero () { return *static_cast<numero::pointer>(fields[0]); }
    numero::reference set_numero (ASN1::INTEGER::int_type v)
    { return *static_cast<numero::pointer>(fields[0]) = v; }

    class titular
    {
    public:
        typedef ModuloTiposResultadosEcoUrna::DadosCandidato value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class titular

    titular::const_reference get_titular () const { return *static_cast<titular::const_pointer>(fields[1]); }
    titular::reference ref_titular () { return *static_cast<titular::pointer>(fields[1]); }
    titular::reference set_titular () { return *static_cast<titular::pointer>(fields[1]); }

    class drap
    {
    public:
        typedef ModuloFotosCandidatos::CodigoCandidato value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class drap

    drap::const_reference get_drap () const { return *static_cast<drap::const_pointer>(fields[2]); }
    drap::reference ref_drap () { return *static_cast<drap::pointer>(fields[2]); }
    drap::reference set_drap () { return *static_cast<drap::pointer>(fields[2]); }
    drap::reference set_drap (const ASN1_STD string& v)
    { return *static_cast<drap::pointer>(fields[2]) = v; }

    class suplentesVices
    {
    public:
        //
        // ModuloTiposResultadosEcoUrna::Candidatura::suplentesVices::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ModuloTiposResultadosEcoUrna::DadosCandidato>
        {
            typedef ASN1::SEQUENCE_OF<ModuloTiposResultadosEcoUrna::DadosCandidato> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ModuloTiposResultadosEcoUrna::DadosCandidato& val) : Inherited(n, val) {}
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
    }; // end class suplentesVices

    suplentesVices::const_reference get_suplentesVices () const
    {
      assert(hasOptionalField(e_suplentesVices));
      return *static_cast<suplentesVices::const_pointer>(fields[3]);
    }
    suplentesVices::reference ref_suplentesVices ()
    {
      assert(hasOptionalField(e_suplentesVices));
      return *static_cast<suplentesVices::pointer>(fields[3]);
    }
    suplentesVices::reference set_suplentesVices ()
    {
      includeOptionalField( e_suplentesVices, 3);
      return *static_cast<suplentesVices::pointer>(fields[3]);
    }
    void omit_suplentesVices () { removeOptionalField( e_suplentesVices); }
    bool suplentesVices_isPresent () const { return hasOptionalField( e_suplentesVices); }
    void swap(Candidatura& that) { Inherited::swap(that); }
    Candidatura * clone() const
    { return static_cast<Candidatura*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[4];
    static int fieldIds[4];
    static const char* fieldNames[4];
}; // end class Candidatura

//
// ModuloTiposResultadosEcoUrna::CandidatosAptos   // SYNTHESIZED name (field ['candidatosAptos', 'candidatosInaptos'])
//

class CandidatosAptos : public ASN1::SEQUENCE_OF<ModuloTiposResultadosEcoUrna::Candidatura>
{
    typedef ASN1::SEQUENCE_OF<ModuloTiposResultadosEcoUrna::Candidatura> Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    CandidatosAptos(const void* info) : Inherited(info) {}
public:
    CandidatosAptos(size_type n=0) : Inherited(n) {}
    CandidatosAptos(size_type n, const ModuloTiposResultadosEcoUrna::Candidatura& val) : Inherited(n, val) {}
    CandidatosAptos(const_iterator first, const_iterator last) : Inherited(first, last) {}
    CandidatosAptos(const CandidatosAptos& that) : Inherited(that) {}
    CandidatosAptos * clone() const
    { return static_cast<CandidatosAptos*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
}; // end class CandidatosAptos

//
// ModuloTiposResultadosEcoUrna::CandidatoPorPartido
//

class CandidatoPorPartido : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    CandidatoPorPartido(const void* info) : Inherited(info) {}
public:
    CandidatoPorPartido() : Inherited(&theInfo) {}
    CandidatoPorPartido(const CandidatoPorPartido & that) : Inherited(that) {}

    CandidatoPorPartido& operator=(const CandidatoPorPartido& that)
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

    class candidatosAptos
    {
    public:
        typedef ModuloTiposResultadosEcoUrna::CandidatosAptos value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class candidatosAptos

    candidatosAptos::const_reference get_candidatosAptos () const { return *static_cast<candidatosAptos::const_pointer>(fields[1]); }
    candidatosAptos::reference ref_candidatosAptos () { return *static_cast<candidatosAptos::pointer>(fields[1]); }
    candidatosAptos::reference set_candidatosAptos () { return *static_cast<candidatosAptos::pointer>(fields[1]); }

    class candidatosInaptos
    {
    public:
        typedef ModuloTiposResultadosEcoUrna::CandidatosAptos value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class candidatosInaptos

    candidatosInaptos::const_reference get_candidatosInaptos () const { return *static_cast<candidatosInaptos::const_pointer>(fields[2]); }
    candidatosInaptos::reference ref_candidatosInaptos () { return *static_cast<candidatosInaptos::pointer>(fields[2]); }
    candidatosInaptos::reference set_candidatosInaptos () { return *static_cast<candidatosInaptos::pointer>(fields[2]); }
    void swap(CandidatoPorPartido& that) { Inherited::swap(that); }
    CandidatoPorPartido * clone() const
    { return static_cast<CandidatoPorPartido*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[3];
    static int fieldIds[3];
    static const char* fieldNames[3];
}; // end class CandidatoPorPartido

} // namespace ModuloTiposResultadosEcoUrna

#endif // __MODULOTIPOSRESULTADOSECOURNA_H
