#ifndef __MODULOESTADOGERALGAP_H
#define __MODULOESTADOGERALGAP_H

#include "asn1.h"

#include "ModuloComplementosMunicipios.h"
#include "ModuloEstadoGeralDefs.h"
#include "ModuloEstadoGeralUrna.h"

namespace ModuloEstadoGeralGap {

//
// ModuloEstadoGeralGap::UrnaAplicativo
//

class UrnaAplicativo : public ASN1::ENUMERATED
{
    typedef ASN1::ENUMERATED Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    UrnaAplicativo(const void* info) : Inherited(info) {}
public:
    UrnaAplicativo() : Inherited(&theInfo) {}
    enum NamedNumber {
        unknownEnumeration_ = -1,
        apvotatreinaeleitor1,
        apvotatreinaeleitor2,
        apvotatreinamesario1,
        apvotatreinamesario2,
        apvotasimula1,
        apvotasimula2,
        apvotaoficial1,
        apvotaoficial2,
        apapuracaotreina1,
        apapuracaotreina2,
        apapuracaosimula1,
        apapuracaosimula2,
        apapuracaooficial1,
        apapuracaooficial2,
        apsemaplicativo,
        apultimoid
    };

    bool is_apvotatreinaeleitor1() const { return value == apvotatreinaeleitor1; }
    void set_apvotatreinaeleitor1() { value = apvotatreinaeleitor1; }

    bool is_apvotatreinaeleitor2() const { return value == apvotatreinaeleitor2; }
    void set_apvotatreinaeleitor2() { value = apvotatreinaeleitor2; }

    bool is_apvotatreinamesario1() const { return value == apvotatreinamesario1; }
    void set_apvotatreinamesario1() { value = apvotatreinamesario1; }

    bool is_apvotatreinamesario2() const { return value == apvotatreinamesario2; }
    void set_apvotatreinamesario2() { value = apvotatreinamesario2; }

    bool is_apvotasimula1() const { return value == apvotasimula1; }
    void set_apvotasimula1() { value = apvotasimula1; }

    bool is_apvotasimula2() const { return value == apvotasimula2; }
    void set_apvotasimula2() { value = apvotasimula2; }

    bool is_apvotaoficial1() const { return value == apvotaoficial1; }
    void set_apvotaoficial1() { value = apvotaoficial1; }

    bool is_apvotaoficial2() const { return value == apvotaoficial2; }
    void set_apvotaoficial2() { value = apvotaoficial2; }

    bool is_apapuracaotreina1() const { return value == apapuracaotreina1; }
    void set_apapuracaotreina1() { value = apapuracaotreina1; }

    bool is_apapuracaotreina2() const { return value == apapuracaotreina2; }
    void set_apapuracaotreina2() { value = apapuracaotreina2; }

    bool is_apapuracaosimula1() const { return value == apapuracaosimula1; }
    void set_apapuracaosimula1() { value = apapuracaosimula1; }

    bool is_apapuracaosimula2() const { return value == apapuracaosimula2; }
    void set_apapuracaosimula2() { value = apapuracaosimula2; }

    bool is_apapuracaooficial1() const { return value == apapuracaooficial1; }
    void set_apapuracaooficial1() { value = apapuracaooficial1; }

    bool is_apapuracaooficial2() const { return value == apapuracaooficial2; }
    void set_apapuracaooficial2() { value = apapuracaooficial2; }

    bool is_apsemaplicativo() const { return value == apsemaplicativo; }
    void set_apsemaplicativo() { value = apsemaplicativo; }

    bool is_apultimoid() const { return value == apultimoid; }
    void set_apultimoid() { value = apultimoid; }

    UrnaAplicativo(const NamedNumber v) : Inherited(&theInfo) { setFromInt(v); }
    UrnaAplicativo & operator=(const NamedNumber v) { setFromInt(v);  return *this; }
    operator NamedNumber() const { return NamedNumber(asInt()); }

    bool operator == (NamedNumber rhs) const { return value == rhs; }
    bool operator != (NamedNumber rhs) const { return value != rhs; }
    bool operator <  (NamedNumber rhs) const { return value <  rhs; }
    bool operator >  (NamedNumber rhs) const { return value >  rhs; }
    bool operator <= (NamedNumber rhs) const { return value <= rhs; }
    bool operator >= (NamedNumber rhs) const { return value >= rhs; }
    bool operator == (const UrnaAplicativo &rhs) const { return value == rhs.value; }
    bool operator != (const UrnaAplicativo &rhs) const { return value != rhs.value; }

    void swap (UrnaAplicativo & that) { Inherited::swap(that); }
    UrnaAplicativo * clone() const
    { return static_cast<UrnaAplicativo*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const char * nameList[];
}; // end class UrnaAplicativo

//
// ModuloEstadoGeralGap::EstadoGeralGap
//

class EstadoGeralGap : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EstadoGeralGap(const void* info) : Inherited(info) {}
public:
    EstadoGeralGap() : Inherited(&theInfo) {}
    EstadoGeralGap(const EstadoGeralGap & that) : Inherited(that) {}

    EstadoGeralGap& operator=(const EstadoGeralGap& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_dataSegundoTurno
    };

    class correspondencias
    {
    public:
        //
        // ModuloEstadoGeralGap::EstadoGeralGap::correspondencias::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ModuloEstadoGeralUrna::DadoCorrespondencia, ASN1::SizeConstraint<ASN1::FixedConstraint, 0, 10>>
        {
            typedef ASN1::SEQUENCE_OF<ModuloEstadoGeralUrna::DadoCorrespondencia, ASN1::SizeConstraint<ASN1::FixedConstraint, 0, 10>> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ModuloEstadoGeralUrna::DadoCorrespondencia& val) : Inherited(n, val) {}
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
    }; // end class correspondencias

    correspondencias::const_reference get_correspondencias () const { return *static_cast<correspondencias::const_pointer>(fields[0]); }
    correspondencias::reference ref_correspondencias () { return *static_cast<correspondencias::pointer>(fields[0]); }
    correspondencias::reference set_correspondencias () { return *static_cast<correspondencias::pointer>(fields[0]); }

    class appId
    {
    public:
        typedef ModuloEstadoGeralGap::UrnaAplicativo value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class appId

    appId::const_reference get_appId () const { return *static_cast<appId::const_pointer>(fields[1]); }
    appId::reference ref_appId () { return *static_cast<appId::pointer>(fields[1]); }
    appId::reference set_appId () { return *static_cast<appId::pointer>(fields[1]); }
    appId::reference set_appId (ModuloEstadoGeralGap::UrnaAplicativo::NamedNumber v)
    { return *static_cast<appId::pointer>(fields[1]) = v; }

    class appAnteriorId
    {
    public:
        typedef ModuloEstadoGeralGap::UrnaAplicativo value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class appAnteriorId

    appAnteriorId::const_reference get_appAnteriorId () const { return *static_cast<appAnteriorId::const_pointer>(fields[2]); }
    appAnteriorId::reference ref_appAnteriorId () { return *static_cast<appAnteriorId::pointer>(fields[2]); }
    appAnteriorId::reference set_appAnteriorId () { return *static_cast<appAnteriorId::pointer>(fields[2]); }
    appAnteriorId::reference set_appAnteriorId (ModuloEstadoGeralGap::UrnaAplicativo::NamedNumber v)
    { return *static_cast<appAnteriorId::pointer>(fields[2]) = v; }

    class executadoRED
    {
    public:
        typedef ASN1::BOOLEAN value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class executadoRED

    executadoRED::const_reference get_executadoRED () const { return *static_cast<executadoRED::const_pointer>(fields[3]); }
    executadoRED::reference ref_executadoRED () { return *static_cast<executadoRED::pointer>(fields[3]); }
    executadoRED::reference set_executadoRED () { return *static_cast<executadoRED::pointer>(fields[3]); }
    executadoRED::reference set_executadoRED (bool v)
    { return *static_cast<executadoRED::pointer>(fields[3]) = v; }

    class identificadoATUE
    {
    public:
        typedef ASN1::BOOLEAN value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class identificadoATUE

    identificadoATUE::const_reference get_identificadoATUE () const { return *static_cast<identificadoATUE::const_pointer>(fields[4]); }
    identificadoATUE::reference ref_identificadoATUE () { return *static_cast<identificadoATUE::pointer>(fields[4]); }
    identificadoATUE::reference set_identificadoATUE () { return *static_cast<identificadoATUE::pointer>(fields[4]); }
    identificadoATUE::reference set_identificadoATUE (bool v)
    { return *static_cast<identificadoATUE::pointer>(fields[4]) = v; }

    class audio
    {
    public:
        typedef ASN1::BOOLEAN value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class audio

    audio::const_reference get_audio () const { return *static_cast<audio::const_pointer>(fields[5]); }
    audio::reference ref_audio () { return *static_cast<audio::pointer>(fields[5]); }
    audio::reference set_audio () { return *static_cast<audio::pointer>(fields[5]); }
    audio::reference set_audio (bool v)
    { return *static_cast<audio::pointer>(fields[5]) = v; }

    class data2T
    {
    public:
        typedef ASN1::BOOLEAN value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class data2T

    data2T::const_reference get_data2T () const { return *static_cast<data2T::const_pointer>(fields[6]); }
    data2T::reference ref_data2T () { return *static_cast<data2T::pointer>(fields[6]); }
    data2T::reference set_data2T () { return *static_cast<data2T::pointer>(fields[6]); }
    data2T::reference set_data2T (bool v)
    { return *static_cast<data2T::pointer>(fields[6]) = v; }

    class numViasImpressasRelatorios1T
    {
    public:
        typedef ModuloEstadoGeralDefs::NumViasImpressasRelatorios value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class numViasImpressasRelatorios1T

    numViasImpressasRelatorios1T::const_reference get_numViasImpressasRelatorios1T () const { return *static_cast<numViasImpressasRelatorios1T::const_pointer>(fields[7]); }
    numViasImpressasRelatorios1T::reference ref_numViasImpressasRelatorios1T () { return *static_cast<numViasImpressasRelatorios1T::pointer>(fields[7]); }
    numViasImpressasRelatorios1T::reference set_numViasImpressasRelatorios1T () { return *static_cast<numViasImpressasRelatorios1T::pointer>(fields[7]); }

    class numViasImpressasRelatorios2T
    {
    public:
        typedef ModuloEstadoGeralDefs::NumViasImpressasRelatorios value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class numViasImpressasRelatorios2T

    numViasImpressasRelatorios2T::const_reference get_numViasImpressasRelatorios2T () const { return *static_cast<numViasImpressasRelatorios2T::const_pointer>(fields[8]); }
    numViasImpressasRelatorios2T::reference ref_numViasImpressasRelatorios2T () { return *static_cast<numViasImpressasRelatorios2T::pointer>(fields[8]); }
    numViasImpressasRelatorios2T::reference set_numViasImpressasRelatorios2T () { return *static_cast<numViasImpressasRelatorios2T::pointer>(fields[8]); }

    class dataSegundoTurno
    {
    public:
        typedef ModuloComplementosMunicipios::InicioHV value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class dataSegundoTurno

    dataSegundoTurno::const_reference get_dataSegundoTurno () const
    {
      assert(hasOptionalField(e_dataSegundoTurno));
      return *static_cast<dataSegundoTurno::const_pointer>(fields[9]);
    }
    dataSegundoTurno::reference ref_dataSegundoTurno ()
    {
      assert(hasOptionalField(e_dataSegundoTurno));
      return *static_cast<dataSegundoTurno::pointer>(fields[9]);
    }
    dataSegundoTurno::reference set_dataSegundoTurno ()
    {
      includeOptionalField( e_dataSegundoTurno, 9);
      return *static_cast<dataSegundoTurno::pointer>(fields[9]);
    }
    dataSegundoTurno::reference set_dataSegundoTurno (const ASN1_STD string& v)
    {
      includeOptionalField( e_dataSegundoTurno, 9);
      return *static_cast<dataSegundoTurno::pointer>(fields[9]) = v;
    }
    void omit_dataSegundoTurno () { removeOptionalField( e_dataSegundoTurno); }
    bool dataSegundoTurno_isPresent () const { return hasOptionalField( e_dataSegundoTurno); }
    void swap(EstadoGeralGap& that) { Inherited::swap(that); }
    EstadoGeralGap * clone() const
    { return static_cast<EstadoGeralGap*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[10];
    static int fieldIds[10];
    static const char* fieldNames[10];
}; // end class EstadoGeralGap

} // namespace ModuloEstadoGeralGap

#endif // __MODULOESTADOGERALGAP_H
