#ifndef __MODULOVERSAOARQUIVOS_H
#define __MODULOVERSAOARQUIVOS_H

#include "asn1.h"

#include "ModuloHashes.h"

namespace ModuloVersaoArquivos {

//
// ModuloVersaoArquivos::EntidadeVersaoArquivos
//

class EntidadeVersaoArquivos : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EntidadeVersaoArquivos(const void* info) : Inherited(info) {}
public:
    EntidadeVersaoArquivos() : Inherited(&theInfo) {}
    EntidadeVersaoArquivos(const EntidadeVersaoArquivos & that) : Inherited(that) {}

    EntidadeVersaoArquivos& operator=(const EntidadeVersaoArquivos& that)
    { Inherited::operator=(that); return *this; }


    class versaoTag
    {
    public:
        typedef ASN1::GeneralString value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class versaoTag

    versaoTag::const_reference get_versaoTag () const { return *static_cast<versaoTag::const_pointer>(fields[0]); }
    versaoTag::reference ref_versaoTag () { return *static_cast<versaoTag::pointer>(fields[0]); }
    versaoTag::reference set_versaoTag () { return *static_cast<versaoTag::pointer>(fields[0]); }
    versaoTag::reference set_versaoTag (const ASN1_STD string& v)
    { return *static_cast<versaoTag::pointer>(fields[0]) = v; }

    class arquivos
    {
    public:
        typedef ModuloHashes::HashesArquivos value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class arquivos

    arquivos::const_reference get_arquivos () const { return *static_cast<arquivos::const_pointer>(fields[1]); }
    arquivos::reference ref_arquivos () { return *static_cast<arquivos::pointer>(fields[1]); }
    arquivos::reference set_arquivos () { return *static_cast<arquivos::pointer>(fields[1]); }
    void swap(EntidadeVersaoArquivos& that) { Inherited::swap(that); }
    EntidadeVersaoArquivos * clone() const
    { return static_cast<EntidadeVersaoArquivos*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[2];
    static int fieldIds[2];
    static const char* fieldNames[2];
}; // end class EntidadeVersaoArquivos

} // namespace ModuloVersaoArquivos

#endif // __MODULOVERSAOARQUIVOS_H
