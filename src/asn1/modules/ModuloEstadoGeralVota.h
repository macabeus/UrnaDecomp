#ifndef __MODULOESTADOGERALVOTA_H
#define __MODULOESTADOGERALVOTA_H

#include "asn1.h"

#include "ModuloEstadoGeralDefs.h"
#include "ModuloTiposEleitorais.h"

namespace ModuloEstadoGeralVota {

//
// ModuloEstadoGeralVota::EstadoVota   // named by exact field/RTTI match
//

class EstadoVota : public ASN1::ENUMERATED
{
    typedef ASN1::ENUMERATED Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EstadoVota(const void* info) : Inherited(info) {}
public:
    EstadoVota() : Inherited(&theInfo) {}
    enum NamedNumber {
        unknownEnumeration_ = -1,
        inicial,
        gerabasedinamica,
        aguardahorazeresima,
        gerarze,
        zeresimagerada,
        zeresimaimpressa,
        registromesarioinicial,
        votar,
        fimaquisicaovotos,
        registromesariofinal,
        gerarbu,
        gerarrelatorios,
        imprimirbu,
        gravarresultados,
        copiaresultadosmr,
        encerrada,
        exibealertadesligamento,
        ultimoid
    };

    bool is_inicial() const { return value == inicial; }
    void set_inicial() { value = inicial; }

    bool is_gerabasedinamica() const { return value == gerabasedinamica; }
    void set_gerabasedinamica() { value = gerabasedinamica; }

    bool is_aguardahorazeresima() const { return value == aguardahorazeresima; }
    void set_aguardahorazeresima() { value = aguardahorazeresima; }

    bool is_gerarze() const { return value == gerarze; }
    void set_gerarze() { value = gerarze; }

    bool is_zeresimagerada() const { return value == zeresimagerada; }
    void set_zeresimagerada() { value = zeresimagerada; }

    bool is_zeresimaimpressa() const { return value == zeresimaimpressa; }
    void set_zeresimaimpressa() { value = zeresimaimpressa; }

    bool is_registromesarioinicial() const { return value == registromesarioinicial; }
    void set_registromesarioinicial() { value = registromesarioinicial; }

    bool is_votar() const { return value == votar; }
    void set_votar() { value = votar; }

    bool is_fimaquisicaovotos() const { return value == fimaquisicaovotos; }
    void set_fimaquisicaovotos() { value = fimaquisicaovotos; }

    bool is_registromesariofinal() const { return value == registromesariofinal; }
    void set_registromesariofinal() { value = registromesariofinal; }

    bool is_gerarbu() const { return value == gerarbu; }
    void set_gerarbu() { value = gerarbu; }

    bool is_gerarrelatorios() const { return value == gerarrelatorios; }
    void set_gerarrelatorios() { value = gerarrelatorios; }

    bool is_imprimirbu() const { return value == imprimirbu; }
    void set_imprimirbu() { value = imprimirbu; }

    bool is_gravarresultados() const { return value == gravarresultados; }
    void set_gravarresultados() { value = gravarresultados; }

    bool is_copiaresultadosmr() const { return value == copiaresultadosmr; }
    void set_copiaresultadosmr() { value = copiaresultadosmr; }

    bool is_encerrada() const { return value == encerrada; }
    void set_encerrada() { value = encerrada; }

    bool is_exibealertadesligamento() const { return value == exibealertadesligamento; }
    void set_exibealertadesligamento() { value = exibealertadesligamento; }

    bool is_ultimoid() const { return value == ultimoid; }
    void set_ultimoid() { value = ultimoid; }

    EstadoVota(const NamedNumber v) : Inherited(&theInfo) { setFromInt(v); }
    EstadoVota & operator=(const NamedNumber v) { setFromInt(v);  return *this; }
    operator NamedNumber() const { return NamedNumber(asInt()); }

    bool operator == (NamedNumber rhs) const { return value == rhs; }
    bool operator != (NamedNumber rhs) const { return value != rhs; }
    bool operator <  (NamedNumber rhs) const { return value <  rhs; }
    bool operator >  (NamedNumber rhs) const { return value >  rhs; }
    bool operator <= (NamedNumber rhs) const { return value <= rhs; }
    bool operator >= (NamedNumber rhs) const { return value >= rhs; }
    bool operator == (const EstadoVota &rhs) const { return value == rhs.value; }
    bool operator != (const EstadoVota &rhs) const { return value != rhs.value; }

    void swap (EstadoVota & that) { Inherited::swap(that); }
    EstadoVota * clone() const
    { return static_cast<EstadoVota*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const char * nameList[];
}; // end class EstadoVota

//
// ModuloEstadoGeralVota::EstadoEncerramento   // named by exact field/RTTI match
//

class EstadoEncerramento : public ASN1::ENUMERATED
{
    typedef ASN1::ENUMERATED Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EstadoEncerramento(const void* info) : Inherited(info) {}
public:
    EstadoEncerramento() : Inherited(&theInfo) {}
    enum NamedNumber {
        unknownEnumeration_ = -1,
        inicial,
        imprimirobrigatoriabu,
        retirarmr,
        fimdostrabalhos,
        ultimoid
    };

    bool is_inicial() const { return value == inicial; }
    void set_inicial() { value = inicial; }

    bool is_imprimirobrigatoriabu() const { return value == imprimirobrigatoriabu; }
    void set_imprimirobrigatoriabu() { value = imprimirobrigatoriabu; }

    bool is_retirarmr() const { return value == retirarmr; }
    void set_retirarmr() { value = retirarmr; }

    bool is_fimdostrabalhos() const { return value == fimdostrabalhos; }
    void set_fimdostrabalhos() { value = fimdostrabalhos; }

    bool is_ultimoid() const { return value == ultimoid; }
    void set_ultimoid() { value = ultimoid; }

    EstadoEncerramento(const NamedNumber v) : Inherited(&theInfo) { setFromInt(v); }
    EstadoEncerramento & operator=(const NamedNumber v) { setFromInt(v);  return *this; }
    operator NamedNumber() const { return NamedNumber(asInt()); }

    bool operator == (NamedNumber rhs) const { return value == rhs; }
    bool operator != (NamedNumber rhs) const { return value != rhs; }
    bool operator <  (NamedNumber rhs) const { return value <  rhs; }
    bool operator >  (NamedNumber rhs) const { return value >  rhs; }
    bool operator <= (NamedNumber rhs) const { return value <= rhs; }
    bool operator >= (NamedNumber rhs) const { return value >= rhs; }
    bool operator == (const EstadoEncerramento &rhs) const { return value == rhs.value; }
    bool operator != (const EstadoEncerramento &rhs) const { return value != rhs.value; }

    void swap (EstadoEncerramento & that) { Inherited::swap(that); }
    EstadoEncerramento * clone() const
    { return static_cast<EstadoEncerramento*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const char * nameList[];
}; // end class EstadoEncerramento

//
// ModuloEstadoGeralVota::EstadoGeralVota
//

class EstadoGeralVota : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EstadoGeralVota(const void* info) : Inherited(info) {}
public:
    EstadoGeralVota() : Inherited(&theInfo) {}
    EstadoGeralVota(const EstadoGeralVota & that) : Inherited(that) {}

    EstadoGeralVota& operator=(const EstadoGeralVota& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_urnaIdGerouZeresima,
      e_dhIniAquisicao,
      e_dhFimAquisicao,
      e_dhUltimoVoto,
      e_dhEmissao
    };

    class estadoVota
    {
    public:
        typedef ModuloEstadoGeralVota::EstadoVota value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class estadoVota

    estadoVota::const_reference get_estadoVota () const { return *static_cast<estadoVota::const_pointer>(fields[0]); }
    estadoVota::reference ref_estadoVota () { return *static_cast<estadoVota::pointer>(fields[0]); }
    estadoVota::reference set_estadoVota () { return *static_cast<estadoVota::pointer>(fields[0]); }
    estadoVota::reference set_estadoVota (ModuloEstadoGeralVota::EstadoVota::NamedNumber v)
    { return *static_cast<estadoVota::pointer>(fields[0]) = v; }

    class estadoEncerramento
    {
    public:
        typedef ModuloEstadoGeralVota::EstadoEncerramento value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class estadoEncerramento

    estadoEncerramento::const_reference get_estadoEncerramento () const { return *static_cast<estadoEncerramento::const_pointer>(fields[1]); }
    estadoEncerramento::reference ref_estadoEncerramento () { return *static_cast<estadoEncerramento::pointer>(fields[1]); }
    estadoEncerramento::reference set_estadoEncerramento () { return *static_cast<estadoEncerramento::pointer>(fields[1]); }
    estadoEncerramento::reference set_estadoEncerramento (ModuloEstadoGeralVota::EstadoEncerramento::NamedNumber v)
    { return *static_cast<estadoEncerramento::pointer>(fields[1]) = v; }

    class qtdBU
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class qtdBU

    qtdBU::const_reference get_qtdBU () const { return *static_cast<qtdBU::const_pointer>(fields[2]); }
    qtdBU::reference ref_qtdBU () { return *static_cast<qtdBU::pointer>(fields[2]); }
    qtdBU::reference set_qtdBU () { return *static_cast<qtdBU::pointer>(fields[2]); }
    qtdBU::reference set_qtdBU (ASN1::INTEGER::int_type v)
    { return *static_cast<qtdBU::pointer>(fields[2]) = v; }

    class comparecimento
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 99999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class comparecimento

    comparecimento::const_reference get_comparecimento () const { return *static_cast<comparecimento::const_pointer>(fields[3]); }
    comparecimento::reference ref_comparecimento () { return *static_cast<comparecimento::pointer>(fields[3]); }
    comparecimento::reference set_comparecimento () { return *static_cast<comparecimento::pointer>(fields[3]); }
    comparecimento::reference set_comparecimento (ASN1::INTEGER::int_type v)
    { return *static_cast<comparecimento::pointer>(fields[3]) = v; }

    class qtdJustificativa
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 99999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class qtdJustificativa

    qtdJustificativa::const_reference get_qtdJustificativa () const { return *static_cast<qtdJustificativa::const_pointer>(fields[4]); }
    qtdJustificativa::reference ref_qtdJustificativa () { return *static_cast<qtdJustificativa::pointer>(fields[4]); }
    qtdJustificativa::reference set_qtdJustificativa () { return *static_cast<qtdJustificativa::pointer>(fields[4]); }
    qtdJustificativa::reference set_qtdJustificativa (ASN1::INTEGER::int_type v)
    { return *static_cast<qtdJustificativa::pointer>(fields[4]) = v; }

    class treinamentoEleitor
    {
    public:
        typedef ASN1::BOOLEAN value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class treinamentoEleitor

    treinamentoEleitor::const_reference get_treinamentoEleitor () const { return *static_cast<treinamentoEleitor::const_pointer>(fields[5]); }
    treinamentoEleitor::reference ref_treinamentoEleitor () { return *static_cast<treinamentoEleitor::pointer>(fields[5]); }
    treinamentoEleitor::reference set_treinamentoEleitor () { return *static_cast<treinamentoEleitor::pointer>(fields[5]); }
    treinamentoEleitor::reference set_treinamentoEleitor (bool v)
    { return *static_cast<treinamentoEleitor::pointer>(fields[5]) = v; }

    class numViasImpressasRelatorios
    {
    public:
        typedef ModuloEstadoGeralDefs::NumViasImpressasRelatorios value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class numViasImpressasRelatorios

    numViasImpressasRelatorios::const_reference get_numViasImpressasRelatorios () const { return *static_cast<numViasImpressasRelatorios::const_pointer>(fields[6]); }
    numViasImpressasRelatorios::reference ref_numViasImpressasRelatorios () { return *static_cast<numViasImpressasRelatorios::pointer>(fields[6]); }
    numViasImpressasRelatorios::reference set_numViasImpressasRelatorios () { return *static_cast<numViasImpressasRelatorios::pointer>(fields[6]); }

    class tecladoTestadoPosConversao
    {
    public:
        typedef ASN1::BOOLEAN value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class tecladoTestadoPosConversao

    tecladoTestadoPosConversao::const_reference get_tecladoTestadoPosConversao () const { return *static_cast<tecladoTestadoPosConversao::const_pointer>(fields[7]); }
    tecladoTestadoPosConversao::reference ref_tecladoTestadoPosConversao () { return *static_cast<tecladoTestadoPosConversao::pointer>(fields[7]); }
    tecladoTestadoPosConversao::reference set_tecladoTestadoPosConversao () { return *static_cast<tecladoTestadoPosConversao::pointer>(fields[7]); }
    tecladoTestadoPosConversao::reference set_tecladoTestadoPosConversao (bool v)
    { return *static_cast<tecladoTestadoPosConversao::pointer>(fields[7]) = v; }

    class urnaIdGerouZeresima
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 99999999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class urnaIdGerouZeresima

    urnaIdGerouZeresima::const_reference get_urnaIdGerouZeresima () const
    {
      assert(hasOptionalField(e_urnaIdGerouZeresima));
      return *static_cast<urnaIdGerouZeresima::const_pointer>(fields[8]);
    }
    urnaIdGerouZeresima::reference ref_urnaIdGerouZeresima ()
    {
      assert(hasOptionalField(e_urnaIdGerouZeresima));
      return *static_cast<urnaIdGerouZeresima::pointer>(fields[8]);
    }
    urnaIdGerouZeresima::reference set_urnaIdGerouZeresima ()
    {
      includeOptionalField( e_urnaIdGerouZeresima, 8);
      return *static_cast<urnaIdGerouZeresima::pointer>(fields[8]);
    }
    urnaIdGerouZeresima::reference set_urnaIdGerouZeresima (ASN1::INTEGER::int_type v)
    {
      includeOptionalField( e_urnaIdGerouZeresima, 8);
      return *static_cast<urnaIdGerouZeresima::pointer>(fields[8]) = v;
    }
    void omit_urnaIdGerouZeresima () { removeOptionalField( e_urnaIdGerouZeresima); }
    bool urnaIdGerouZeresima_isPresent () const { return hasOptionalField( e_urnaIdGerouZeresima); }

    class dhIniAquisicao
    {
    public:
        typedef ModuloTiposEleitorais::DataGeracao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class dhIniAquisicao

    dhIniAquisicao::const_reference get_dhIniAquisicao () const
    {
      assert(hasOptionalField(e_dhIniAquisicao));
      return *static_cast<dhIniAquisicao::const_pointer>(fields[9]);
    }
    dhIniAquisicao::reference ref_dhIniAquisicao ()
    {
      assert(hasOptionalField(e_dhIniAquisicao));
      return *static_cast<dhIniAquisicao::pointer>(fields[9]);
    }
    dhIniAquisicao::reference set_dhIniAquisicao ()
    {
      includeOptionalField( e_dhIniAquisicao, 9);
      return *static_cast<dhIniAquisicao::pointer>(fields[9]);
    }
    dhIniAquisicao::reference set_dhIniAquisicao (const ASN1_STD string& v)
    {
      includeOptionalField( e_dhIniAquisicao, 9);
      return *static_cast<dhIniAquisicao::pointer>(fields[9]) = v;
    }
    void omit_dhIniAquisicao () { removeOptionalField( e_dhIniAquisicao); }
    bool dhIniAquisicao_isPresent () const { return hasOptionalField( e_dhIniAquisicao); }

    class dhFimAquisicao
    {
    public:
        typedef ModuloTiposEleitorais::DataGeracao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class dhFimAquisicao

    dhFimAquisicao::const_reference get_dhFimAquisicao () const
    {
      assert(hasOptionalField(e_dhFimAquisicao));
      return *static_cast<dhFimAquisicao::const_pointer>(fields[10]);
    }
    dhFimAquisicao::reference ref_dhFimAquisicao ()
    {
      assert(hasOptionalField(e_dhFimAquisicao));
      return *static_cast<dhFimAquisicao::pointer>(fields[10]);
    }
    dhFimAquisicao::reference set_dhFimAquisicao ()
    {
      includeOptionalField( e_dhFimAquisicao, 10);
      return *static_cast<dhFimAquisicao::pointer>(fields[10]);
    }
    dhFimAquisicao::reference set_dhFimAquisicao (const ASN1_STD string& v)
    {
      includeOptionalField( e_dhFimAquisicao, 10);
      return *static_cast<dhFimAquisicao::pointer>(fields[10]) = v;
    }
    void omit_dhFimAquisicao () { removeOptionalField( e_dhFimAquisicao); }
    bool dhFimAquisicao_isPresent () const { return hasOptionalField( e_dhFimAquisicao); }

    class dhUltimoVoto
    {
    public:
        typedef ModuloTiposEleitorais::DataGeracao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class dhUltimoVoto

    dhUltimoVoto::const_reference get_dhUltimoVoto () const
    {
      assert(hasOptionalField(e_dhUltimoVoto));
      return *static_cast<dhUltimoVoto::const_pointer>(fields[11]);
    }
    dhUltimoVoto::reference ref_dhUltimoVoto ()
    {
      assert(hasOptionalField(e_dhUltimoVoto));
      return *static_cast<dhUltimoVoto::pointer>(fields[11]);
    }
    dhUltimoVoto::reference set_dhUltimoVoto ()
    {
      includeOptionalField( e_dhUltimoVoto, 11);
      return *static_cast<dhUltimoVoto::pointer>(fields[11]);
    }
    dhUltimoVoto::reference set_dhUltimoVoto (const ASN1_STD string& v)
    {
      includeOptionalField( e_dhUltimoVoto, 11);
      return *static_cast<dhUltimoVoto::pointer>(fields[11]) = v;
    }
    void omit_dhUltimoVoto () { removeOptionalField( e_dhUltimoVoto); }
    bool dhUltimoVoto_isPresent () const { return hasOptionalField( e_dhUltimoVoto); }

    class dhEmissao
    {
    public:
        typedef ModuloTiposEleitorais::DataGeracao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class dhEmissao

    dhEmissao::const_reference get_dhEmissao () const
    {
      assert(hasOptionalField(e_dhEmissao));
      return *static_cast<dhEmissao::const_pointer>(fields[12]);
    }
    dhEmissao::reference ref_dhEmissao ()
    {
      assert(hasOptionalField(e_dhEmissao));
      return *static_cast<dhEmissao::pointer>(fields[12]);
    }
    dhEmissao::reference set_dhEmissao ()
    {
      includeOptionalField( e_dhEmissao, 12);
      return *static_cast<dhEmissao::pointer>(fields[12]);
    }
    dhEmissao::reference set_dhEmissao (const ASN1_STD string& v)
    {
      includeOptionalField( e_dhEmissao, 12);
      return *static_cast<dhEmissao::pointer>(fields[12]) = v;
    }
    void omit_dhEmissao () { removeOptionalField( e_dhEmissao); }
    bool dhEmissao_isPresent () const { return hasOptionalField( e_dhEmissao); }
    void swap(EstadoGeralVota& that) { Inherited::swap(that); }
    EstadoGeralVota * clone() const
    { return static_cast<EstadoGeralVota*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[13];
    static int fieldIds[13];
    static unsigned fieldTags[13];
    static const char* fieldNames[13];
}; // end class EstadoGeralVota

} // namespace ModuloEstadoGeralVota

#endif // __MODULOESTADOGERALVOTA_H
