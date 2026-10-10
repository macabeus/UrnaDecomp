#ifndef __MODULOESTADOGERALDEFS_H
#define __MODULOESTADOGERALDEFS_H

#include "asn1.h"

namespace ModuloEstadoGeralDefs {

//
// ModuloEstadoGeralDefs::NumViasImpressasRelatorios
//

class NumViasImpressasRelatorios : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    NumViasImpressasRelatorios(const void* info) : Inherited(info) {}
public:
    NumViasImpressasRelatorios() : Inherited(&theInfo) {}
    NumViasImpressasRelatorios(const NumViasImpressasRelatorios & that) : Inherited(that) {}

    NumViasImpressasRelatorios& operator=(const NumViasImpressasRelatorios& that)
    { Inherited::operator=(that); return *this; }


    class numViasEstadoUrna
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class numViasEstadoUrna

    numViasEstadoUrna::const_reference get_numViasEstadoUrna () const { return *static_cast<numViasEstadoUrna::const_pointer>(fields[0]); }
    numViasEstadoUrna::reference ref_numViasEstadoUrna () { return *static_cast<numViasEstadoUrna::pointer>(fields[0]); }
    numViasEstadoUrna::reference set_numViasEstadoUrna () { return *static_cast<numViasEstadoUrna::pointer>(fields[0]); }
    numViasEstadoUrna::reference set_numViasEstadoUrna (ASN1::INTEGER::int_type v)
    { return *static_cast<numViasEstadoUrna::pointer>(fields[0]) = v; }

    class numViasEleitores
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class numViasEleitores

    numViasEleitores::const_reference get_numViasEleitores () const { return *static_cast<numViasEleitores::const_pointer>(fields[1]); }
    numViasEleitores::reference ref_numViasEleitores () { return *static_cast<numViasEleitores::pointer>(fields[1]); }
    numViasEleitores::reference set_numViasEleitores () { return *static_cast<numViasEleitores::pointer>(fields[1]); }
    numViasEleitores::reference set_numViasEleitores (ASN1::INTEGER::int_type v)
    { return *static_cast<numViasEleitores::pointer>(fields[1]) = v; }

    class numViasVersoesDados
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class numViasVersoesDados

    numViasVersoesDados::const_reference get_numViasVersoesDados () const { return *static_cast<numViasVersoesDados::const_pointer>(fields[2]); }
    numViasVersoesDados::reference ref_numViasVersoesDados () { return *static_cast<numViasVersoesDados::pointer>(fields[2]); }
    numViasVersoesDados::reference set_numViasVersoesDados () { return *static_cast<numViasVersoesDados::pointer>(fields[2]); }
    numViasVersoesDados::reference set_numViasVersoesDados (ASN1::INTEGER::int_type v)
    { return *static_cast<numViasVersoesDados::pointer>(fields[2]) = v; }

    class numViasPU
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class numViasPU

    numViasPU::const_reference get_numViasPU () const { return *static_cast<numViasPU::const_pointer>(fields[3]); }
    numViasPU::reference ref_numViasPU () { return *static_cast<numViasPU::pointer>(fields[3]); }
    numViasPU::reference set_numViasPU () { return *static_cast<numViasPU::pointer>(fields[3]); }
    numViasPU::reference set_numViasPU (ASN1::INTEGER::int_type v)
    { return *static_cast<numViasPU::pointer>(fields[3]) = v; }
    void swap(NumViasImpressasRelatorios& that) { Inherited::swap(that); }
    NumViasImpressasRelatorios * clone() const
    { return static_cast<NumViasImpressasRelatorios*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[4];
    static int fieldIds[4];
    static const char* fieldNames[4];
}; // end class NumViasImpressasRelatorios

} // namespace ModuloEstadoGeralDefs

#endif // __MODULOESTADOGERALDEFS_H
