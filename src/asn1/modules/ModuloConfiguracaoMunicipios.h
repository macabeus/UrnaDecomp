#ifndef __MODULOCONFIGURACAOMUNICIPIOS_H
#define __MODULOCONFIGURACAOMUNICIPIOS_H

#include "asn1.h"

#include "ModuloTiposEleitorais.h"

namespace ModuloConfiguracaoMunicipios {

//
// ModuloConfiguracaoMunicipios::HorariosUrna
//

class HorariosUrna : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    HorariosUrna(const void* info) : Inherited(info) {}
public:
    HorariosUrna() : Inherited(&theInfo) {}
    HorariosUrna(const HorariosUrna & that) : Inherited(that) {}

    HorariosUrna& operator=(const HorariosUrna& that)
    { Inherited::operator=(that); return *this; }


    class emissaoZeresima
    {
    public:
        typedef ModuloTiposEleitorais::DataGeracao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class emissaoZeresima

    emissaoZeresima::const_reference get_emissaoZeresima () const { return *static_cast<emissaoZeresima::const_pointer>(fields[0]); }
    emissaoZeresima::reference ref_emissaoZeresima () { return *static_cast<emissaoZeresima::pointer>(fields[0]); }
    emissaoZeresima::reference set_emissaoZeresima () { return *static_cast<emissaoZeresima::pointer>(fields[0]); }
    emissaoZeresima::reference set_emissaoZeresima (const ASN1_STD string& v)
    { return *static_cast<emissaoZeresima::pointer>(fields[0]) = v; }

    class inicioVotacao
    {
    public:
        typedef ModuloTiposEleitorais::DataGeracao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class inicioVotacao

    inicioVotacao::const_reference get_inicioVotacao () const { return *static_cast<inicioVotacao::const_pointer>(fields[1]); }
    inicioVotacao::reference ref_inicioVotacao () { return *static_cast<inicioVotacao::pointer>(fields[1]); }
    inicioVotacao::reference set_inicioVotacao () { return *static_cast<inicioVotacao::pointer>(fields[1]); }
    inicioVotacao::reference set_inicioVotacao (const ASN1_STD string& v)
    { return *static_cast<inicioVotacao::pointer>(fields[1]) = v; }

    class encerramentoVotacao
    {
    public:
        typedef ModuloTiposEleitorais::DataGeracao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class encerramentoVotacao

    encerramentoVotacao::const_reference get_encerramentoVotacao () const { return *static_cast<encerramentoVotacao::const_pointer>(fields[2]); }
    encerramentoVotacao::reference ref_encerramentoVotacao () { return *static_cast<encerramentoVotacao::pointer>(fields[2]); }
    encerramentoVotacao::reference set_encerramentoVotacao () { return *static_cast<encerramentoVotacao::pointer>(fields[2]); }
    encerramentoVotacao::reference set_encerramentoVotacao (const ASN1_STD string& v)
    { return *static_cast<encerramentoVotacao::pointer>(fields[2]) = v; }

    class terminoVotacao
    {
    public:
        typedef ModuloTiposEleitorais::DataGeracao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class terminoVotacao

    terminoVotacao::const_reference get_terminoVotacao () const { return *static_cast<terminoVotacao::const_pointer>(fields[3]); }
    terminoVotacao::reference ref_terminoVotacao () { return *static_cast<terminoVotacao::pointer>(fields[3]); }
    terminoVotacao::reference set_terminoVotacao () { return *static_cast<terminoVotacao::pointer>(fields[3]); }
    terminoVotacao::reference set_terminoVotacao (const ASN1_STD string& v)
    { return *static_cast<terminoVotacao::pointer>(fields[3]) = v; }
    void swap(HorariosUrna& that) { Inherited::swap(that); }
    HorariosUrna * clone() const
    { return static_cast<HorariosUrna*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[4];
    static int fieldIds[4];
    static const char* fieldNames[4];
}; // end class HorariosUrna

//
// ModuloConfiguracaoMunicipios::ConfiguracaoMunicipio
//

class ConfiguracaoMunicipio : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    ConfiguracaoMunicipio(const void* info) : Inherited(info) {}
public:
    ConfiguracaoMunicipio() : Inherited(&theInfo) {}
    ConfiguracaoMunicipio(const ConfiguracaoMunicipio & that) : Inherited(that) {}

    ConfiguracaoMunicipio& operator=(const ConfiguracaoMunicipio& that)
    { Inherited::operator=(that); return *this; }


    class codigoMunicipio
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 1, 99999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class codigoMunicipio

    codigoMunicipio::const_reference get_codigoMunicipio () const { return *static_cast<codigoMunicipio::const_pointer>(fields[0]); }
    codigoMunicipio::reference ref_codigoMunicipio () { return *static_cast<codigoMunicipio::pointer>(fields[0]); }
    codigoMunicipio::reference set_codigoMunicipio () { return *static_cast<codigoMunicipio::pointer>(fields[0]); }
    codigoMunicipio::reference set_codigoMunicipio (ASN1::INTEGER::int_type v)
    { return *static_cast<codigoMunicipio::pointer>(fields[0]) = v; }

    class horariosUrna
    {
    public:
        typedef ModuloConfiguracaoMunicipios::HorariosUrna value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class horariosUrna

    horariosUrna::const_reference get_horariosUrna () const { return *static_cast<horariosUrna::const_pointer>(fields[1]); }
    horariosUrna::reference ref_horariosUrna () { return *static_cast<horariosUrna::pointer>(fields[1]); }
    horariosUrna::reference set_horariosUrna () { return *static_cast<horariosUrna::pointer>(fields[1]); }
    void swap(ConfiguracaoMunicipio& that) { Inherited::swap(that); }
    ConfiguracaoMunicipio * clone() const
    { return static_cast<ConfiguracaoMunicipio*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[2];
    static int fieldIds[2];
    static const char* fieldNames[2];
}; // end class ConfiguracaoMunicipio

//
// ModuloConfiguracaoMunicipios::EntidadeConfiguracaoMunicipios
//

class EntidadeConfiguracaoMunicipios : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EntidadeConfiguracaoMunicipios(const void* info) : Inherited(info) {}
public:
    EntidadeConfiguracaoMunicipios() : Inherited(&theInfo) {}
    EntidadeConfiguracaoMunicipios(const EntidadeConfiguracaoMunicipios & that) : Inherited(that) {}

    EntidadeConfiguracaoMunicipios& operator=(const EntidadeConfiguracaoMunicipios& that)
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

    class configuracoes
    {
    public:
        //
        // ModuloConfiguracaoMunicipios::EntidadeConfiguracaoMunicipios::configuracoes::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ModuloConfiguracaoMunicipios::ConfiguracaoMunicipio>
        {
            typedef ASN1::SEQUENCE_OF<ModuloConfiguracaoMunicipios::ConfiguracaoMunicipio> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ModuloConfiguracaoMunicipios::ConfiguracaoMunicipio& val) : Inherited(n, val) {}
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
    }; // end class configuracoes

    configuracoes::const_reference get_configuracoes () const { return *static_cast<configuracoes::const_pointer>(fields[1]); }
    configuracoes::reference ref_configuracoes () { return *static_cast<configuracoes::pointer>(fields[1]); }
    configuracoes::reference set_configuracoes () { return *static_cast<configuracoes::pointer>(fields[1]); }
    void swap(EntidadeConfiguracaoMunicipios& that) { Inherited::swap(that); }
    EntidadeConfiguracaoMunicipios * clone() const
    { return static_cast<EntidadeConfiguracaoMunicipios*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[2];
    static int fieldIds[2];
    static const char* fieldNames[2];
}; // end class EntidadeConfiguracaoMunicipios

} // namespace ModuloConfiguracaoMunicipios

#endif // __MODULOCONFIGURACAOMUNICIPIOS_H
