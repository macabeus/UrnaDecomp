#ifndef __MODULORESULTADOURNACADASTRO_H
#define __MODULORESULTADOURNACADASTRO_H

#include "asn1.h"

#include "ModuloTiposEcoUrna.h"
#include "ModuloTiposEleitorais.h"

namespace ModuloResultadoUrnaCadastro {

//
// ModuloResultadoUrnaCadastro::DadosCifracao
//

class DadosCifracao : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    DadosCifracao(const void* info) : Inherited(info) {}
public:
    DadosCifracao() : Inherited(&theInfo) {}
    DadosCifracao(const DadosCifracao & that) : Inherited(that) {}

    DadosCifracao& operator=(const DadosCifracao& that)
    { Inherited::operator=(that); return *this; }


    class chave
    {
    public:
        typedef ASN1::OCTET_STRING value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class chave

    chave::const_reference get_chave () const { return *static_cast<chave::const_pointer>(fields[0]); }
    chave::reference ref_chave () { return *static_cast<chave::pointer>(fields[0]); }
    chave::reference set_chave () { return *static_cast<chave::pointer>(fields[0]); }
    chave::reference set_chave (const ASN1::OCTET_STRING& v)
    { return *static_cast<chave::pointer>(fields[0]) = v; }

    class salt
    {
    public:
        typedef ASN1::OCTET_STRING value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class salt

    salt::const_reference get_salt () const { return *static_cast<salt::const_pointer>(fields[1]); }
    salt::reference ref_salt () { return *static_cast<salt::pointer>(fields[1]); }
    salt::reference set_salt () { return *static_cast<salt::pointer>(fields[1]); }
    salt::reference set_salt (const ASN1::OCTET_STRING& v)
    { return *static_cast<salt::pointer>(fields[1]) = v; }

    class informacaoAdicional
    {
    public:
        typedef ASN1::OCTET_STRING value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class informacaoAdicional

    informacaoAdicional::const_reference get_informacaoAdicional () const { return *static_cast<informacaoAdicional::const_pointer>(fields[2]); }
    informacaoAdicional::reference ref_informacaoAdicional () { return *static_cast<informacaoAdicional::pointer>(fields[2]); }
    informacaoAdicional::reference set_informacaoAdicional () { return *static_cast<informacaoAdicional::pointer>(fields[2]); }
    informacaoAdicional::reference set_informacaoAdicional (const ASN1::OCTET_STRING& v)
    { return *static_cast<informacaoAdicional::pointer>(fields[2]) = v; }
    void swap(DadosCifracao& that) { Inherited::swap(that); }
    DadosCifracao * clone() const
    { return static_cast<DadosCifracao*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[3];
    static int fieldIds[3];
    static const char* fieldNames[3];
}; // end class DadosCifracao

//
// ModuloResultadoUrnaCadastro::DadosComparecimentoCifrado
//

class DadosComparecimentoCifrado : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    DadosComparecimentoCifrado(const void* info) : Inherited(info) {}
public:
    DadosComparecimentoCifrado() : Inherited(&theInfo) {}
    DadosComparecimentoCifrado(const DadosComparecimentoCifrado & that) : Inherited(that) {}

    DadosComparecimentoCifrado& operator=(const DadosComparecimentoCifrado& that)
    { Inherited::operator=(that); return *this; }


    class dadosCifracao
    {
    public:
        typedef ModuloResultadoUrnaCadastro::DadosCifracao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class dadosCifracao

    dadosCifracao::const_reference get_dadosCifracao () const { return *static_cast<dadosCifracao::const_pointer>(fields[0]); }
    dadosCifracao::reference ref_dadosCifracao () { return *static_cast<dadosCifracao::pointer>(fields[0]); }
    dadosCifracao::reference set_dadosCifracao () { return *static_cast<dadosCifracao::pointer>(fields[0]); }

    class conteudo
    {
    public:
        typedef ASN1::OCTET_STRING value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class conteudo

    conteudo::const_reference get_conteudo () const { return *static_cast<conteudo::const_pointer>(fields[1]); }
    conteudo::reference ref_conteudo () { return *static_cast<conteudo::pointer>(fields[1]); }
    conteudo::reference set_conteudo () { return *static_cast<conteudo::pointer>(fields[1]); }
    conteudo::reference set_conteudo (const ASN1::OCTET_STRING& v)
    { return *static_cast<conteudo::pointer>(fields[1]) = v; }
    void swap(DadosComparecimentoCifrado& that) { Inherited::swap(that); }
    DadosComparecimentoCifrado * clone() const
    { return static_cast<DadosComparecimentoCifrado*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[2];
    static int fieldIds[2];
    static const char* fieldNames[2];
}; // end class DadosComparecimentoCifrado

//
// ModuloResultadoUrnaCadastro::IdentificacaoJustificativa
//

class IdentificacaoJustificativa : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    IdentificacaoJustificativa(const void* info) : Inherited(info) {}
public:
    IdentificacaoJustificativa() : Inherited(&theInfo) {}
    IdentificacaoJustificativa(const IdentificacaoJustificativa & that) : Inherited(that) {}

    IdentificacaoJustificativa& operator=(const IdentificacaoJustificativa& that)
    { Inherited::operator=(that); return *this; }


    class identificacaoEleitor
    {
    public:
        typedef ModuloTiposEcoUrna::RegistroIdentificacaoEleitor value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class identificacaoEleitor

    identificacaoEleitor::const_reference get_identificacaoEleitor () const { return *static_cast<identificacaoEleitor::const_pointer>(fields[0]); }
    identificacaoEleitor::reference ref_identificacaoEleitor () { return *static_cast<identificacaoEleitor::pointer>(fields[0]); }
    identificacaoEleitor::reference set_identificacaoEleitor () { return *static_cast<identificacaoEleitor::pointer>(fields[0]); }

    class anoNascimentoEleitor
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 9999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class anoNascimentoEleitor

    anoNascimentoEleitor::const_reference get_anoNascimentoEleitor () const { return *static_cast<anoNascimentoEleitor::const_pointer>(fields[1]); }
    anoNascimentoEleitor::reference ref_anoNascimentoEleitor () { return *static_cast<anoNascimentoEleitor::pointer>(fields[1]); }
    anoNascimentoEleitor::reference set_anoNascimentoEleitor () { return *static_cast<anoNascimentoEleitor::pointer>(fields[1]); }
    anoNascimentoEleitor::reference set_anoNascimentoEleitor (ASN1::INTEGER::int_type v)
    { return *static_cast<anoNascimentoEleitor::pointer>(fields[1]) = v; }
    void swap(IdentificacaoJustificativa& that) { Inherited::swap(that); }
    IdentificacaoJustificativa * clone() const
    { return static_cast<IdentificacaoJustificativa*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[2];
    static int fieldIds[2];
    static const char* fieldNames[2];
}; // end class IdentificacaoJustificativa

//
// ModuloResultadoUrnaCadastro::SituacaoComparecimentoEleitor
//

class SituacaoComparecimentoEleitor : public ASN1::ENUMERATED
{
    typedef ASN1::ENUMERATED Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    SituacaoComparecimentoEleitor(const void* info) : Inherited(info) {}
public:
    SituacaoComparecimentoEleitor() : Inherited(&theInfo) {}
    enum NamedNumber {
        unknownEnumeration_ = -1,
        faltou = 1,
        naoVotou,
        votou,
        semCargoParaVotar
    };

    bool is_faltou() const { return value == faltou; }
    void set_faltou() { value = faltou; }

    bool is_naoVotou() const { return value == naoVotou; }
    void set_naoVotou() { value = naoVotou; }

    bool is_votou() const { return value == votou; }
    void set_votou() { value = votou; }

    bool is_semCargoParaVotar() const { return value == semCargoParaVotar; }
    void set_semCargoParaVotar() { value = semCargoParaVotar; }

    SituacaoComparecimentoEleitor(const NamedNumber v) : Inherited(&theInfo) { setFromInt(v); }
    SituacaoComparecimentoEleitor & operator=(const NamedNumber v) { setFromInt(v);  return *this; }
    operator NamedNumber() const { return NamedNumber(asInt()); }

    bool operator == (NamedNumber rhs) const { return value == rhs; }
    bool operator != (NamedNumber rhs) const { return value != rhs; }
    bool operator <  (NamedNumber rhs) const { return value <  rhs; }
    bool operator >  (NamedNumber rhs) const { return value >  rhs; }
    bool operator <= (NamedNumber rhs) const { return value <= rhs; }
    bool operator >= (NamedNumber rhs) const { return value >= rhs; }
    bool operator == (const SituacaoComparecimentoEleitor &rhs) const { return value == rhs.value; }
    bool operator != (const SituacaoComparecimentoEleitor &rhs) const { return value != rhs.value; }

    void swap (SituacaoComparecimentoEleitor & that) { Inherited::swap(that); }
    SituacaoComparecimentoEleitor * clone() const
    { return static_cast<SituacaoComparecimentoEleitor*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const char * nameList[];
}; // end class SituacaoComparecimentoEleitor

//
// ModuloResultadoUrnaCadastro::EstadoApresentacaoFotoEleitor
//

class EstadoApresentacaoFotoEleitor : public ASN1::ENUMERATED
{
    typedef ASN1::ENUMERATED Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EstadoApresentacaoFotoEleitor(const void* info) : Inherited(info) {}
public:
    EstadoApresentacaoFotoEleitor() : Inherited(&theInfo) {}
    enum NamedNumber {
        unknownEnumeration_ = -1,
        eleitorSemFoto = 1,
        apresentada,
        naoApresentadaPorErro
    };

    bool is_eleitorSemFoto() const { return value == eleitorSemFoto; }
    void set_eleitorSemFoto() { value = eleitorSemFoto; }

    bool is_apresentada() const { return value == apresentada; }
    void set_apresentada() { value = apresentada; }

    bool is_naoApresentadaPorErro() const { return value == naoApresentadaPorErro; }
    void set_naoApresentadaPorErro() { value = naoApresentadaPorErro; }

    EstadoApresentacaoFotoEleitor(const NamedNumber v) : Inherited(&theInfo) { setFromInt(v); }
    EstadoApresentacaoFotoEleitor & operator=(const NamedNumber v) { setFromInt(v);  return *this; }
    operator NamedNumber() const { return NamedNumber(asInt()); }

    bool operator == (NamedNumber rhs) const { return value == rhs; }
    bool operator != (NamedNumber rhs) const { return value != rhs; }
    bool operator <  (NamedNumber rhs) const { return value <  rhs; }
    bool operator >  (NamedNumber rhs) const { return value >  rhs; }
    bool operator <= (NamedNumber rhs) const { return value <= rhs; }
    bool operator >= (NamedNumber rhs) const { return value >= rhs; }
    bool operator == (const EstadoApresentacaoFotoEleitor &rhs) const { return value == rhs.value; }
    bool operator != (const EstadoApresentacaoFotoEleitor &rhs) const { return value != rhs.value; }

    void swap (EstadoApresentacaoFotoEleitor & that) { Inherited::swap(that); }
    EstadoApresentacaoFotoEleitor * clone() const
    { return static_cast<EstadoApresentacaoFotoEleitor*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const char * nameList[];
}; // end class EstadoApresentacaoFotoEleitor

//
// ModuloResultadoUrnaCadastro::ResultadoApresentacaoFotoEleitor
//

class ResultadoApresentacaoFotoEleitor : public ASN1::ENUMERATED
{
    typedef ASN1::ENUMERATED Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    ResultadoApresentacaoFotoEleitor(const void* info) : Inherited(info) {}
public:
    ResultadoApresentacaoFotoEleitor() : Inherited(&theInfo) {}
    enum NamedNumber {
        unknownEnumeration_ = -1,
        semErro,
        erroFormato,
        erroFormacaoImagem,
        erroResolucao,
        erroCompressao,
        erroProfundidadeCores,
        erroApresentacao,
        erroDecifrandoBuffer,
        erroDesconhecidoDeCriptografia
    };

    bool is_semErro() const { return value == semErro; }
    void set_semErro() { value = semErro; }

    bool is_erroFormato() const { return value == erroFormato; }
    void set_erroFormato() { value = erroFormato; }

    bool is_erroFormacaoImagem() const { return value == erroFormacaoImagem; }
    void set_erroFormacaoImagem() { value = erroFormacaoImagem; }

    bool is_erroResolucao() const { return value == erroResolucao; }
    void set_erroResolucao() { value = erroResolucao; }

    bool is_erroCompressao() const { return value == erroCompressao; }
    void set_erroCompressao() { value = erroCompressao; }

    bool is_erroProfundidadeCores() const { return value == erroProfundidadeCores; }
    void set_erroProfundidadeCores() { value = erroProfundidadeCores; }

    bool is_erroApresentacao() const { return value == erroApresentacao; }
    void set_erroApresentacao() { value = erroApresentacao; }

    bool is_erroDecifrandoBuffer() const { return value == erroDecifrandoBuffer; }
    void set_erroDecifrandoBuffer() { value = erroDecifrandoBuffer; }

    bool is_erroDesconhecidoDeCriptografia() const { return value == erroDesconhecidoDeCriptografia; }
    void set_erroDesconhecidoDeCriptografia() { value = erroDesconhecidoDeCriptografia; }

    ResultadoApresentacaoFotoEleitor(const NamedNumber v) : Inherited(&theInfo) { setFromInt(v); }
    ResultadoApresentacaoFotoEleitor & operator=(const NamedNumber v) { setFromInt(v);  return *this; }
    operator NamedNumber() const { return NamedNumber(asInt()); }

    bool operator == (NamedNumber rhs) const { return value == rhs; }
    bool operator != (NamedNumber rhs) const { return value != rhs; }
    bool operator <  (NamedNumber rhs) const { return value <  rhs; }
    bool operator >  (NamedNumber rhs) const { return value >  rhs; }
    bool operator <= (NamedNumber rhs) const { return value <= rhs; }
    bool operator >= (NamedNumber rhs) const { return value >= rhs; }
    bool operator == (const ResultadoApresentacaoFotoEleitor &rhs) const { return value == rhs.value; }
    bool operator != (const ResultadoApresentacaoFotoEleitor &rhs) const { return value != rhs.value; }

    void swap (ResultadoApresentacaoFotoEleitor & that) { Inherited::swap(that); }
    ResultadoApresentacaoFotoEleitor * clone() const
    { return static_cast<ResultadoApresentacaoFotoEleitor*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const char * nameList[];
}; // end class ResultadoApresentacaoFotoEleitor

//
// ModuloResultadoUrnaCadastro::SituacaoHabilitacaoAudio
//

class SituacaoHabilitacaoAudio : public ASN1::ENUMERATED
{
    typedef ASN1::ENUMERATED Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    SituacaoHabilitacaoAudio(const void* info) : Inherited(info) {}
public:
    SituacaoHabilitacaoAudio() : Inherited(&theInfo) {}
    enum NamedNumber {
        unknownEnumeration_ = -1,
        automatica = 1,
        peloMesario,
        naoHabilitado
    };

    bool is_automatica() const { return value == automatica; }
    void set_automatica() { value = automatica; }

    bool is_peloMesario() const { return value == peloMesario; }
    void set_peloMesario() { value = peloMesario; }

    bool is_naoHabilitado() const { return value == naoHabilitado; }
    void set_naoHabilitado() { value = naoHabilitado; }

    SituacaoHabilitacaoAudio(const NamedNumber v) : Inherited(&theInfo) { setFromInt(v); }
    SituacaoHabilitacaoAudio & operator=(const NamedNumber v) { setFromInt(v);  return *this; }
    operator NamedNumber() const { return NamedNumber(asInt()); }

    bool operator == (NamedNumber rhs) const { return value == rhs; }
    bool operator != (NamedNumber rhs) const { return value != rhs; }
    bool operator <  (NamedNumber rhs) const { return value <  rhs; }
    bool operator >  (NamedNumber rhs) const { return value >  rhs; }
    bool operator <= (NamedNumber rhs) const { return value <= rhs; }
    bool operator >= (NamedNumber rhs) const { return value >= rhs; }
    bool operator == (const SituacaoHabilitacaoAudio &rhs) const { return value == rhs.value; }
    bool operator != (const SituacaoHabilitacaoAudio &rhs) const { return value != rhs.value; }

    void swap (SituacaoHabilitacaoAudio & that) { Inherited::swap(that); }
    SituacaoHabilitacaoAudio * clone() const
    { return static_cast<SituacaoHabilitacaoAudio*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const char * nameList[];
}; // end class SituacaoHabilitacaoAudio

//
// ModuloResultadoUrnaCadastro::ErroLeituraBiometria
//

class ErroLeituraBiometria : public ASN1::ENUMERATED
{
    typedef ASN1::ENUMERATED Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    ErroLeituraBiometria(const void* info) : Inherited(info) {}
public:
    ErroLeituraBiometria() : Inherited(&theInfo) {}
    enum NamedNumber {
        unknownEnumeration_ = -1,
        semErro,
        tipoArquivoInvalido,
        idCriptografiaInvalido,
        chaveSecretaVazia,
        chaveSecaoVazia,
        conteudoVazio,
        chaveSecaoComTamanhoInvalido,
        chaveSecretaComTamanhoInvalido,
        naoFoiPossivelIniciarADecifragem,
        erroDecifrandoBuffer,
        erroDesconhecidoDeCriptografia,
        erroFormacaoTemplate,
        erroCodificacaoMinuncias
    };

    bool is_semErro() const { return value == semErro; }
    void set_semErro() { value = semErro; }

    bool is_tipoArquivoInvalido() const { return value == tipoArquivoInvalido; }
    void set_tipoArquivoInvalido() { value = tipoArquivoInvalido; }

    bool is_idCriptografiaInvalido() const { return value == idCriptografiaInvalido; }
    void set_idCriptografiaInvalido() { value = idCriptografiaInvalido; }

    bool is_chaveSecretaVazia() const { return value == chaveSecretaVazia; }
    void set_chaveSecretaVazia() { value = chaveSecretaVazia; }

    bool is_chaveSecaoVazia() const { return value == chaveSecaoVazia; }
    void set_chaveSecaoVazia() { value = chaveSecaoVazia; }

    bool is_conteudoVazio() const { return value == conteudoVazio; }
    void set_conteudoVazio() { value = conteudoVazio; }

    bool is_chaveSecaoComTamanhoInvalido() const { return value == chaveSecaoComTamanhoInvalido; }
    void set_chaveSecaoComTamanhoInvalido() { value = chaveSecaoComTamanhoInvalido; }

    bool is_chaveSecretaComTamanhoInvalido() const { return value == chaveSecretaComTamanhoInvalido; }
    void set_chaveSecretaComTamanhoInvalido() { value = chaveSecretaComTamanhoInvalido; }

    bool is_naoFoiPossivelIniciarADecifragem() const { return value == naoFoiPossivelIniciarADecifragem; }
    void set_naoFoiPossivelIniciarADecifragem() { value = naoFoiPossivelIniciarADecifragem; }

    bool is_erroDecifrandoBuffer() const { return value == erroDecifrandoBuffer; }
    void set_erroDecifrandoBuffer() { value = erroDecifrandoBuffer; }

    bool is_erroDesconhecidoDeCriptografia() const { return value == erroDesconhecidoDeCriptografia; }
    void set_erroDesconhecidoDeCriptografia() { value = erroDesconhecidoDeCriptografia; }

    bool is_erroFormacaoTemplate() const { return value == erroFormacaoTemplate; }
    void set_erroFormacaoTemplate() { value = erroFormacaoTemplate; }

    bool is_erroCodificacaoMinuncias() const { return value == erroCodificacaoMinuncias; }
    void set_erroCodificacaoMinuncias() { value = erroCodificacaoMinuncias; }

    ErroLeituraBiometria(const NamedNumber v) : Inherited(&theInfo) { setFromInt(v); }
    ErroLeituraBiometria & operator=(const NamedNumber v) { setFromInt(v);  return *this; }
    operator NamedNumber() const { return NamedNumber(asInt()); }

    bool operator == (NamedNumber rhs) const { return value == rhs; }
    bool operator != (NamedNumber rhs) const { return value != rhs; }
    bool operator <  (NamedNumber rhs) const { return value <  rhs; }
    bool operator >  (NamedNumber rhs) const { return value >  rhs; }
    bool operator <= (NamedNumber rhs) const { return value <= rhs; }
    bool operator >= (NamedNumber rhs) const { return value >= rhs; }
    bool operator == (const ErroLeituraBiometria &rhs) const { return value == rhs.value; }
    bool operator != (const ErroLeituraBiometria &rhs) const { return value != rhs.value; }

    void swap (ErroLeituraBiometria & that) { Inherited::swap(that); }
    ErroLeituraBiometria * clone() const
    { return static_cast<ErroLeituraBiometria*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const char * nameList[];
}; // end class ErroLeituraBiometria

//
// ModuloResultadoUrnaCadastro::EstadoColetaDigital
//

class EstadoColetaDigital : public ASN1::ENUMERATED
{
    typedef ASN1::ENUMERATED Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EstadoColetaDigital(const void* info) : Inherited(info) {}
public:
    EstadoColetaDigital() : Inherited(&theInfo) {}
    enum NamedNumber {
        unknownEnumeration_ = -1,
        naoColetadaReconhecidaNoCadastro = 1,
        coletadaMesarioNaoPossuiDigitalNoCadastro,
        coletadaDigitalNaoReconhecidaNoCadastro,
        coletadaMesarioNaoPertenceASecao
    };

    bool is_naoColetadaReconhecidaNoCadastro() const { return value == naoColetadaReconhecidaNoCadastro; }
    void set_naoColetadaReconhecidaNoCadastro() { value = naoColetadaReconhecidaNoCadastro; }

    bool is_coletadaMesarioNaoPossuiDigitalNoCadastro() const { return value == coletadaMesarioNaoPossuiDigitalNoCadastro; }
    void set_coletadaMesarioNaoPossuiDigitalNoCadastro() { value = coletadaMesarioNaoPossuiDigitalNoCadastro; }

    bool is_coletadaDigitalNaoReconhecidaNoCadastro() const { return value == coletadaDigitalNaoReconhecidaNoCadastro; }
    void set_coletadaDigitalNaoReconhecidaNoCadastro() { value = coletadaDigitalNaoReconhecidaNoCadastro; }

    bool is_coletadaMesarioNaoPertenceASecao() const { return value == coletadaMesarioNaoPertenceASecao; }
    void set_coletadaMesarioNaoPertenceASecao() { value = coletadaMesarioNaoPertenceASecao; }

    EstadoColetaDigital(const NamedNumber v) : Inherited(&theInfo) { setFromInt(v); }
    EstadoColetaDigital & operator=(const NamedNumber v) { setFromInt(v);  return *this; }
    operator NamedNumber() const { return NamedNumber(asInt()); }

    bool operator == (NamedNumber rhs) const { return value == rhs; }
    bool operator != (NamedNumber rhs) const { return value != rhs; }
    bool operator <  (NamedNumber rhs) const { return value <  rhs; }
    bool operator >  (NamedNumber rhs) const { return value >  rhs; }
    bool operator <= (NamedNumber rhs) const { return value <= rhs; }
    bool operator >= (NamedNumber rhs) const { return value >= rhs; }
    bool operator == (const EstadoColetaDigital &rhs) const { return value == rhs.value; }
    bool operator != (const EstadoColetaDigital &rhs) const { return value != rhs.value; }

    void swap (EstadoColetaDigital & that) { Inherited::swap(that); }
    EstadoColetaDigital * clone() const
    { return static_cast<EstadoColetaDigital*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const char * nameList[];
}; // end class EstadoColetaDigital

//
// ModuloResultadoUrnaCadastro::SituacaoReconhecimentoMesario
//

class SituacaoReconhecimentoMesario : public ASN1::ENUMERATED
{
    typedef ASN1::ENUMERATED Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    SituacaoReconhecimentoMesario(const void* info) : Inherited(info) {}
public:
    SituacaoReconhecimentoMesario() : Inherited(&theInfo) {}
    enum NamedNumber {
        unknownEnumeration_ = -1,
        naoReconhecido = 1,
        reconhecidoDigitalCadastroUrna,
        reconhecidoDigitalRegistroMesarios
    };

    bool is_naoReconhecido() const { return value == naoReconhecido; }
    void set_naoReconhecido() { value = naoReconhecido; }

    bool is_reconhecidoDigitalCadastroUrna() const { return value == reconhecidoDigitalCadastroUrna; }
    void set_reconhecidoDigitalCadastroUrna() { value = reconhecidoDigitalCadastroUrna; }

    bool is_reconhecidoDigitalRegistroMesarios() const { return value == reconhecidoDigitalRegistroMesarios; }
    void set_reconhecidoDigitalRegistroMesarios() { value = reconhecidoDigitalRegistroMesarios; }

    SituacaoReconhecimentoMesario(const NamedNumber v) : Inherited(&theInfo) { setFromInt(v); }
    SituacaoReconhecimentoMesario & operator=(const NamedNumber v) { setFromInt(v);  return *this; }
    operator NamedNumber() const { return NamedNumber(asInt()); }

    bool operator == (NamedNumber rhs) const { return value == rhs; }
    bool operator != (NamedNumber rhs) const { return value != rhs; }
    bool operator <  (NamedNumber rhs) const { return value <  rhs; }
    bool operator >  (NamedNumber rhs) const { return value >  rhs; }
    bool operator <= (NamedNumber rhs) const { return value <= rhs; }
    bool operator >= (NamedNumber rhs) const { return value >= rhs; }
    bool operator == (const SituacaoReconhecimentoMesario &rhs) const { return value == rhs.value; }
    bool operator != (const SituacaoReconhecimentoMesario &rhs) const { return value != rhs.value; }

    void swap (SituacaoReconhecimentoMesario & that) { Inherited::swap(that); }
    SituacaoReconhecimentoMesario * clone() const
    { return static_cast<SituacaoReconhecimentoMesario*> (Inherited::clone()); }
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
    static const InfoType theInfo;
private:
    static const char * nameList[];
}; // end class SituacaoReconhecimentoMesario

//
// ModuloResultadoUrnaCadastro::ApresentacaoFotoEleitor
//

class ApresentacaoFotoEleitor : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    ApresentacaoFotoEleitor(const void* info) : Inherited(info) {}
public:
    ApresentacaoFotoEleitor() : Inherited(&theInfo) {}
    ApresentacaoFotoEleitor(const ApresentacaoFotoEleitor & that) : Inherited(that) {}

    ApresentacaoFotoEleitor& operator=(const ApresentacaoFotoEleitor& that)
    { Inherited::operator=(that); return *this; }


    class estadoApresentacao
    {
    public:
        typedef ModuloResultadoUrnaCadastro::EstadoApresentacaoFotoEleitor value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class estadoApresentacao

    estadoApresentacao::const_reference get_estadoApresentacao () const { return *static_cast<estadoApresentacao::const_pointer>(fields[0]); }
    estadoApresentacao::reference ref_estadoApresentacao () { return *static_cast<estadoApresentacao::pointer>(fields[0]); }
    estadoApresentacao::reference set_estadoApresentacao () { return *static_cast<estadoApresentacao::pointer>(fields[0]); }
    estadoApresentacao::reference set_estadoApresentacao (ModuloResultadoUrnaCadastro::EstadoApresentacaoFotoEleitor::NamedNumber v)
    { return *static_cast<estadoApresentacao::pointer>(fields[0]) = v; }

    class resultadoApresentacao
    {
    public:
        typedef ModuloResultadoUrnaCadastro::ResultadoApresentacaoFotoEleitor value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class resultadoApresentacao

    resultadoApresentacao::const_reference get_resultadoApresentacao () const { return *static_cast<resultadoApresentacao::const_pointer>(fields[1]); }
    resultadoApresentacao::reference ref_resultadoApresentacao () { return *static_cast<resultadoApresentacao::pointer>(fields[1]); }
    resultadoApresentacao::reference set_resultadoApresentacao () { return *static_cast<resultadoApresentacao::pointer>(fields[1]); }
    resultadoApresentacao::reference set_resultadoApresentacao (ModuloResultadoUrnaCadastro::ResultadoApresentacaoFotoEleitor::NamedNumber v)
    { return *static_cast<resultadoApresentacao::pointer>(fields[1]) = v; }
    void swap(ApresentacaoFotoEleitor& that) { Inherited::swap(that); }
    ApresentacaoFotoEleitor * clone() const
    { return static_cast<ApresentacaoFotoEleitor*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[2];
    static int fieldIds[2];
    static const char* fieldNames[2];
}; // end class ApresentacaoFotoEleitor

//
// ModuloResultadoUrnaCadastro::ComparecimentoMesario
//

class ComparecimentoMesario : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    ComparecimentoMesario(const void* info) : Inherited(info) {}
public:
    ComparecimentoMesario() : Inherited(&theInfo) {}
    ComparecimentoMesario(const ComparecimentoMesario & that) : Inherited(that) {}

    ComparecimentoMesario& operator=(const ComparecimentoMesario& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_estadoColetaDigital
    };

    class identificacaoMesario
    {
    public:
        typedef ModuloTiposEcoUrna::RegistroIdentificacaoEleitor value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class identificacaoMesario

    identificacaoMesario::const_reference get_identificacaoMesario () const { return *static_cast<identificacaoMesario::const_pointer>(fields[0]); }
    identificacaoMesario::reference ref_identificacaoMesario () { return *static_cast<identificacaoMesario::pointer>(fields[0]); }
    identificacaoMesario::reference set_identificacaoMesario () { return *static_cast<identificacaoMesario::pointer>(fields[0]); }

    class dataHoraColeta
    {
    public:
        typedef ModuloTiposEleitorais::DataHoraJE value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class dataHoraColeta

    dataHoraColeta::const_reference get_dataHoraColeta () const { return *static_cast<dataHoraColeta::const_pointer>(fields[1]); }
    dataHoraColeta::reference ref_dataHoraColeta () { return *static_cast<dataHoraColeta::pointer>(fields[1]); }
    dataHoraColeta::reference set_dataHoraColeta () { return *static_cast<dataHoraColeta::pointer>(fields[1]); }
    dataHoraColeta::reference set_dataHoraColeta (const ASN1_STD string& v)
    { return *static_cast<dataHoraColeta::pointer>(fields[1]) = v; }

    class estadoColetaDigital
    {
    public:
        typedef ModuloResultadoUrnaCadastro::EstadoColetaDigital value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class estadoColetaDigital

    estadoColetaDigital::const_reference get_estadoColetaDigital () const
    {
      assert(hasOptionalField(e_estadoColetaDigital));
      return *static_cast<estadoColetaDigital::const_pointer>(fields[2]);
    }
    estadoColetaDigital::reference ref_estadoColetaDigital ()
    {
      assert(hasOptionalField(e_estadoColetaDigital));
      return *static_cast<estadoColetaDigital::pointer>(fields[2]);
    }
    estadoColetaDigital::reference set_estadoColetaDigital ()
    {
      includeOptionalField( e_estadoColetaDigital, 2);
      return *static_cast<estadoColetaDigital::pointer>(fields[2]);
    }
    estadoColetaDigital::reference set_estadoColetaDigital (ModuloResultadoUrnaCadastro::EstadoColetaDigital::NamedNumber v)
    {
      includeOptionalField( e_estadoColetaDigital, 2);
      return *static_cast<estadoColetaDigital::pointer>(fields[2]) = v;
    }
    void omit_estadoColetaDigital () { removeOptionalField( e_estadoColetaDigital); }
    bool estadoColetaDigital_isPresent () const { return hasOptionalField( e_estadoColetaDigital); }
    void swap(ComparecimentoMesario& that) { Inherited::swap(that); }
    ComparecimentoMesario * clone() const
    { return static_cast<ComparecimentoMesario*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[3];
    static int fieldIds[3];
    static const char* fieldNames[3];
}; // end class ComparecimentoMesario

//
// ModuloResultadoUrnaCadastro::EstadoHabilitacaoPorCodigo
//

class EstadoHabilitacaoPorCodigo : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EstadoHabilitacaoPorCodigo(const void* info) : Inherited(info) {}
public:
    EstadoHabilitacaoPorCodigo() : Inherited(&theInfo) {}
    EstadoHabilitacaoPorCodigo(const EstadoHabilitacaoPorCodigo & that) : Inherited(that) {}

    EstadoHabilitacaoPorCodigo& operator=(const EstadoHabilitacaoPorCodigo& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_identificacaoMesario
    };

    class situacaoReconhecimentoMesario
    {
    public:
        typedef ModuloResultadoUrnaCadastro::SituacaoReconhecimentoMesario value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class situacaoReconhecimentoMesario

    situacaoReconhecimentoMesario::const_reference get_situacaoReconhecimentoMesario () const { return *static_cast<situacaoReconhecimentoMesario::const_pointer>(fields[0]); }
    situacaoReconhecimentoMesario::reference ref_situacaoReconhecimentoMesario () { return *static_cast<situacaoReconhecimentoMesario::pointer>(fields[0]); }
    situacaoReconhecimentoMesario::reference set_situacaoReconhecimentoMesario () { return *static_cast<situacaoReconhecimentoMesario::pointer>(fields[0]); }
    situacaoReconhecimentoMesario::reference set_situacaoReconhecimentoMesario (ModuloResultadoUrnaCadastro::SituacaoReconhecimentoMesario::NamedNumber v)
    { return *static_cast<situacaoReconhecimentoMesario::pointer>(fields[0]) = v; }

    class identificacaoMesario
    {
    public:
        typedef ModuloTiposEcoUrna::RegistroIdentificacaoEleitor value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class identificacaoMesario

    identificacaoMesario::const_reference get_identificacaoMesario () const
    {
      assert(hasOptionalField(e_identificacaoMesario));
      return *static_cast<identificacaoMesario::const_pointer>(fields[1]);
    }
    identificacaoMesario::reference ref_identificacaoMesario ()
    {
      assert(hasOptionalField(e_identificacaoMesario));
      return *static_cast<identificacaoMesario::pointer>(fields[1]);
    }
    identificacaoMesario::reference set_identificacaoMesario ()
    {
      includeOptionalField( e_identificacaoMesario, 1);
      return *static_cast<identificacaoMesario::pointer>(fields[1]);
    }
    void omit_identificacaoMesario () { removeOptionalField( e_identificacaoMesario); }
    bool identificacaoMesario_isPresent () const { return hasOptionalField( e_identificacaoMesario); }
    void swap(EstadoHabilitacaoPorCodigo& that) { Inherited::swap(that); }
    EstadoHabilitacaoPorCodigo * clone() const
    { return static_cast<EstadoHabilitacaoPorCodigo*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[2];
    static int fieldIds[2];
    static const char* fieldNames[2];
}; // end class EstadoHabilitacaoPorCodigo

//
// ModuloResultadoUrnaCadastro::HabilitacaoBiometrica
//

class HabilitacaoBiometrica : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    HabilitacaoBiometrica(const void* info) : Inherited(info) {}
public:
    HabilitacaoBiometrica() : Inherited(&theInfo) {}
    HabilitacaoBiometrica(const HabilitacaoBiometrica & that) : Inherited(that) {}

    HabilitacaoBiometrica& operator=(const HabilitacaoBiometrica& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_habilitacaoPorCodigo,
      e_ultimoScore
    };

    class tentativasHabilitacaoBiometrica
    {
    public:
        typedef ASN1::INTEGER value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class tentativasHabilitacaoBiometrica

    tentativasHabilitacaoBiometrica::const_reference get_tentativasHabilitacaoBiometrica () const { return *static_cast<tentativasHabilitacaoBiometrica::const_pointer>(fields[0]); }
    tentativasHabilitacaoBiometrica::reference ref_tentativasHabilitacaoBiometrica () { return *static_cast<tentativasHabilitacaoBiometrica::pointer>(fields[0]); }
    tentativasHabilitacaoBiometrica::reference set_tentativasHabilitacaoBiometrica () { return *static_cast<tentativasHabilitacaoBiometrica::pointer>(fields[0]); }
    tentativasHabilitacaoBiometrica::reference set_tentativasHabilitacaoBiometrica (ASN1::INTEGER::int_type v)
    { return *static_cast<tentativasHabilitacaoBiometrica::pointer>(fields[0]) = v; }

    class dedoHabilitado
    {
    public:
        typedef ModuloTiposEleitorais::TipoDedo value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class dedoHabilitado

    dedoHabilitado::const_reference get_dedoHabilitado () const { return *static_cast<dedoHabilitado::const_pointer>(fields[1]); }
    dedoHabilitado::reference ref_dedoHabilitado () { return *static_cast<dedoHabilitado::pointer>(fields[1]); }
    dedoHabilitado::reference set_dedoHabilitado () { return *static_cast<dedoHabilitado::pointer>(fields[1]); }
    dedoHabilitado::reference set_dedoHabilitado (ModuloTiposEleitorais::TipoDedo::NamedNumber v)
    { return *static_cast<dedoHabilitado::pointer>(fields[1]) = v; }

    class erroLeituraBiometria
    {
    public:
        typedef ModuloResultadoUrnaCadastro::ErroLeituraBiometria value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class erroLeituraBiometria

    erroLeituraBiometria::const_reference get_erroLeituraBiometria () const { return *static_cast<erroLeituraBiometria::const_pointer>(fields[2]); }
    erroLeituraBiometria::reference ref_erroLeituraBiometria () { return *static_cast<erroLeituraBiometria::pointer>(fields[2]); }
    erroLeituraBiometria::reference set_erroLeituraBiometria () { return *static_cast<erroLeituraBiometria::pointer>(fields[2]); }
    erroLeituraBiometria::reference set_erroLeituraBiometria (ModuloResultadoUrnaCadastro::ErroLeituraBiometria::NamedNumber v)
    { return *static_cast<erroLeituraBiometria::pointer>(fields[2]) = v; }

    class habilitacaoPorCodigo
    {
    public:
        typedef ModuloResultadoUrnaCadastro::EstadoHabilitacaoPorCodigo value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class habilitacaoPorCodigo

    habilitacaoPorCodigo::const_reference get_habilitacaoPorCodigo () const
    {
      assert(hasOptionalField(e_habilitacaoPorCodigo));
      return *static_cast<habilitacaoPorCodigo::const_pointer>(fields[3]);
    }
    habilitacaoPorCodigo::reference ref_habilitacaoPorCodigo ()
    {
      assert(hasOptionalField(e_habilitacaoPorCodigo));
      return *static_cast<habilitacaoPorCodigo::pointer>(fields[3]);
    }
    habilitacaoPorCodigo::reference set_habilitacaoPorCodigo ()
    {
      includeOptionalField( e_habilitacaoPorCodigo, 3);
      return *static_cast<habilitacaoPorCodigo::pointer>(fields[3]);
    }
    void omit_habilitacaoPorCodigo () { removeOptionalField( e_habilitacaoPorCodigo); }
    bool habilitacaoPorCodigo_isPresent () const { return hasOptionalField( e_habilitacaoPorCodigo); }

    class ultimoScore
    {
    public:
        typedef ASN1::Constrained_INTEGER<ASN1::FixedConstraint, 0, 999> value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class ultimoScore

    ultimoScore::const_reference get_ultimoScore () const
    {
      assert(hasOptionalField(e_ultimoScore));
      return *static_cast<ultimoScore::const_pointer>(fields[4]);
    }
    ultimoScore::reference ref_ultimoScore ()
    {
      assert(hasOptionalField(e_ultimoScore));
      return *static_cast<ultimoScore::pointer>(fields[4]);
    }
    ultimoScore::reference set_ultimoScore ()
    {
      includeOptionalField( e_ultimoScore, 4);
      return *static_cast<ultimoScore::pointer>(fields[4]);
    }
    ultimoScore::reference set_ultimoScore (ASN1::INTEGER::int_type v)
    {
      includeOptionalField( e_ultimoScore, 4);
      return *static_cast<ultimoScore::pointer>(fields[4]) = v;
    }
    void omit_ultimoScore () { removeOptionalField( e_ultimoScore); }
    bool ultimoScore_isPresent () const { return hasOptionalField( e_ultimoScore); }
    void swap(HabilitacaoBiometrica& that) { Inherited::swap(that); }
    HabilitacaoBiometrica * clone() const
    { return static_cast<HabilitacaoBiometrica*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[5];
    static int fieldIds[5];
    static unsigned fieldTags[5];
    static const char* fieldNames[5];
}; // end class HabilitacaoBiometrica

//
// ModuloResultadoUrnaCadastro::EstadoComparecimento
//

class EstadoComparecimento : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EstadoComparecimento(const void* info) : Inherited(info) {}
public:
    EstadoComparecimento() : Inherited(&theInfo) {}
    EstadoComparecimento(const EstadoComparecimento & that) : Inherited(that) {}

    EstadoComparecimento& operator=(const EstadoComparecimento& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_apresentacaoFoto,
      e_situacaoHabilitacaoAudio,
      e_habilitacaoBiometrica
    };

    class identificacaoEleitor
    {
    public:
        typedef ModuloTiposEcoUrna::RegistroIdentificacaoEleitor value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class identificacaoEleitor

    identificacaoEleitor::const_reference get_identificacaoEleitor () const { return *static_cast<identificacaoEleitor::const_pointer>(fields[0]); }
    identificacaoEleitor::reference ref_identificacaoEleitor () { return *static_cast<identificacaoEleitor::pointer>(fields[0]); }
    identificacaoEleitor::reference set_identificacaoEleitor () { return *static_cast<identificacaoEleitor::pointer>(fields[0]); }

    class situacaoComparecimentoEleitor
    {
    public:
        typedef ModuloResultadoUrnaCadastro::SituacaoComparecimentoEleitor value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class situacaoComparecimentoEleitor

    situacaoComparecimentoEleitor::const_reference get_situacaoComparecimentoEleitor () const { return *static_cast<situacaoComparecimentoEleitor::const_pointer>(fields[1]); }
    situacaoComparecimentoEleitor::reference ref_situacaoComparecimentoEleitor () { return *static_cast<situacaoComparecimentoEleitor::pointer>(fields[1]); }
    situacaoComparecimentoEleitor::reference set_situacaoComparecimentoEleitor () { return *static_cast<situacaoComparecimentoEleitor::pointer>(fields[1]); }
    situacaoComparecimentoEleitor::reference set_situacaoComparecimentoEleitor (ModuloResultadoUrnaCadastro::SituacaoComparecimentoEleitor::NamedNumber v)
    { return *static_cast<situacaoComparecimentoEleitor::pointer>(fields[1]) = v; }

    class apresentacaoFoto
    {
    public:
        typedef ModuloResultadoUrnaCadastro::ApresentacaoFotoEleitor value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class apresentacaoFoto

    apresentacaoFoto::const_reference get_apresentacaoFoto () const
    {
      assert(hasOptionalField(e_apresentacaoFoto));
      return *static_cast<apresentacaoFoto::const_pointer>(fields[2]);
    }
    apresentacaoFoto::reference ref_apresentacaoFoto ()
    {
      assert(hasOptionalField(e_apresentacaoFoto));
      return *static_cast<apresentacaoFoto::pointer>(fields[2]);
    }
    apresentacaoFoto::reference set_apresentacaoFoto ()
    {
      includeOptionalField( e_apresentacaoFoto, 2);
      return *static_cast<apresentacaoFoto::pointer>(fields[2]);
    }
    void omit_apresentacaoFoto () { removeOptionalField( e_apresentacaoFoto); }
    bool apresentacaoFoto_isPresent () const { return hasOptionalField( e_apresentacaoFoto); }

    class situacaoHabilitacaoAudio
    {
    public:
        typedef ModuloResultadoUrnaCadastro::SituacaoHabilitacaoAudio value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class situacaoHabilitacaoAudio

    situacaoHabilitacaoAudio::const_reference get_situacaoHabilitacaoAudio () const
    {
      assert(hasOptionalField(e_situacaoHabilitacaoAudio));
      return *static_cast<situacaoHabilitacaoAudio::const_pointer>(fields[3]);
    }
    situacaoHabilitacaoAudio::reference ref_situacaoHabilitacaoAudio ()
    {
      assert(hasOptionalField(e_situacaoHabilitacaoAudio));
      return *static_cast<situacaoHabilitacaoAudio::pointer>(fields[3]);
    }
    situacaoHabilitacaoAudio::reference set_situacaoHabilitacaoAudio ()
    {
      includeOptionalField( e_situacaoHabilitacaoAudio, 3);
      return *static_cast<situacaoHabilitacaoAudio::pointer>(fields[3]);
    }
    situacaoHabilitacaoAudio::reference set_situacaoHabilitacaoAudio (ModuloResultadoUrnaCadastro::SituacaoHabilitacaoAudio::NamedNumber v)
    {
      includeOptionalField( e_situacaoHabilitacaoAudio, 3);
      return *static_cast<situacaoHabilitacaoAudio::pointer>(fields[3]) = v;
    }
    void omit_situacaoHabilitacaoAudio () { removeOptionalField( e_situacaoHabilitacaoAudio); }
    bool situacaoHabilitacaoAudio_isPresent () const { return hasOptionalField( e_situacaoHabilitacaoAudio); }

    class habilitacaoBiometrica
    {
    public:
        typedef ModuloResultadoUrnaCadastro::HabilitacaoBiometrica value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class habilitacaoBiometrica

    habilitacaoBiometrica::const_reference get_habilitacaoBiometrica () const
    {
      assert(hasOptionalField(e_habilitacaoBiometrica));
      return *static_cast<habilitacaoBiometrica::const_pointer>(fields[4]);
    }
    habilitacaoBiometrica::reference ref_habilitacaoBiometrica ()
    {
      assert(hasOptionalField(e_habilitacaoBiometrica));
      return *static_cast<habilitacaoBiometrica::pointer>(fields[4]);
    }
    habilitacaoBiometrica::reference set_habilitacaoBiometrica ()
    {
      includeOptionalField( e_habilitacaoBiometrica, 4);
      return *static_cast<habilitacaoBiometrica::pointer>(fields[4]);
    }
    void omit_habilitacaoBiometrica () { removeOptionalField( e_habilitacaoBiometrica); }
    bool habilitacaoBiometrica_isPresent () const { return hasOptionalField( e_habilitacaoBiometrica); }
    void swap(EstadoComparecimento& that) { Inherited::swap(that); }
    EstadoComparecimento * clone() const
    { return static_cast<EstadoComparecimento*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[5];
    static int fieldIds[5];
    static unsigned fieldTags[5];
    static const char* fieldNames[5];
}; // end class EstadoComparecimento

//
// ModuloResultadoUrnaCadastro::ComparecimentoSecao
//

class ComparecimentoSecao : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    ComparecimentoSecao(const void* info) : Inherited(info) {}
public:
    ComparecimentoSecao() : Inherited(&theInfo) {}
    ComparecimentoSecao(const ComparecimentoSecao & that) : Inherited(that) {}

    ComparecimentoSecao& operator=(const ComparecimentoSecao& that)
    { Inherited::operator=(that); return *this; }


    class identificacao
    {
    public:
        typedef ModuloTiposEleitorais::IdentificacaoSecaoEleitoral value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class identificacao

    identificacao::const_reference get_identificacao () const { return *static_cast<identificacao::const_pointer>(fields[0]); }
    identificacao::reference ref_identificacao () { return *static_cast<identificacao::pointer>(fields[0]); }
    identificacao::reference set_identificacao () { return *static_cast<identificacao::pointer>(fields[0]); }

    class eleitores
    {
    public:
        //
        // ModuloResultadoUrnaCadastro::ComparecimentoSecao::eleitores::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ModuloResultadoUrnaCadastro::EstadoComparecimento>
        {
            typedef ASN1::SEQUENCE_OF<ModuloResultadoUrnaCadastro::EstadoComparecimento> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ModuloResultadoUrnaCadastro::EstadoComparecimento& val) : Inherited(n, val) {}
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
    }; // end class eleitores

    eleitores::const_reference get_eleitores () const { return *static_cast<eleitores::const_pointer>(fields[1]); }
    eleitores::reference ref_eleitores () { return *static_cast<eleitores::pointer>(fields[1]); }
    eleitores::reference set_eleitores () { return *static_cast<eleitores::pointer>(fields[1]); }
    void swap(ComparecimentoSecao& that) { Inherited::swap(that); }
    ComparecimentoSecao * clone() const
    { return static_cast<ComparecimentoSecao*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[2];
    static int fieldIds[2];
    static const char* fieldNames[2];
}; // end class ComparecimentoSecao

//
// ModuloResultadoUrnaCadastro::DadosComparecimento
//

class DadosComparecimento : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    DadosComparecimento(const void* info) : Inherited(info) {}
public:
    DadosComparecimento() : Inherited(&theInfo) {}
    DadosComparecimento(const DadosComparecimento & that) : Inherited(that) {}

    DadosComparecimento& operator=(const DadosComparecimento& that)
    { Inherited::operator=(that); return *this; }

    enum OptionalFields {
      e_mesariosAbertura,
      e_mesariosEncerramento
    };

    class justificativas
    {
    public:
        //
        // ModuloResultadoUrnaCadastro::DadosComparecimento::justificativas::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ModuloResultadoUrnaCadastro::IdentificacaoJustificativa>
        {
            typedef ASN1::SEQUENCE_OF<ModuloResultadoUrnaCadastro::IdentificacaoJustificativa> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ModuloResultadoUrnaCadastro::IdentificacaoJustificativa& val) : Inherited(n, val) {}
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
    }; // end class justificativas

    justificativas::const_reference get_justificativas () const { return *static_cast<justificativas::const_pointer>(fields[0]); }
    justificativas::reference ref_justificativas () { return *static_cast<justificativas::pointer>(fields[0]); }
    justificativas::reference set_justificativas () { return *static_cast<justificativas::pointer>(fields[0]); }

    class identificacaoComparecimento
    {
    public:
        typedef ModuloResultadoUrnaCadastro::ComparecimentoSecao value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class identificacaoComparecimento

    identificacaoComparecimento::const_reference get_identificacaoComparecimento () const { return *static_cast<identificacaoComparecimento::const_pointer>(fields[1]); }
    identificacaoComparecimento::reference ref_identificacaoComparecimento () { return *static_cast<identificacaoComparecimento::pointer>(fields[1]); }
    identificacaoComparecimento::reference set_identificacaoComparecimento () { return *static_cast<identificacaoComparecimento::pointer>(fields[1]); }

    class mesariosAbertura
    {
    public:
        //
        // ModuloResultadoUrnaCadastro::DadosComparecimento::mesariosAbertura::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ModuloResultadoUrnaCadastro::ComparecimentoMesario>
        {
            typedef ASN1::SEQUENCE_OF<ModuloResultadoUrnaCadastro::ComparecimentoMesario> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ModuloResultadoUrnaCadastro::ComparecimentoMesario& val) : Inherited(n, val) {}
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
    }; // end class mesariosAbertura

    mesariosAbertura::const_reference get_mesariosAbertura () const
    {
      assert(hasOptionalField(e_mesariosAbertura));
      return *static_cast<mesariosAbertura::const_pointer>(fields[2]);
    }
    mesariosAbertura::reference ref_mesariosAbertura ()
    {
      assert(hasOptionalField(e_mesariosAbertura));
      return *static_cast<mesariosAbertura::pointer>(fields[2]);
    }
    mesariosAbertura::reference set_mesariosAbertura ()
    {
      includeOptionalField( e_mesariosAbertura, 2);
      return *static_cast<mesariosAbertura::pointer>(fields[2]);
    }
    void omit_mesariosAbertura () { removeOptionalField( e_mesariosAbertura); }
    bool mesariosAbertura_isPresent () const { return hasOptionalField( e_mesariosAbertura); }

    class mesariosEncerramento
    {
    public:
        //
        // ModuloResultadoUrnaCadastro::DadosComparecimento::mesariosEncerramento::value_type
        //

        class value_type : public ASN1::SEQUENCE_OF<ModuloResultadoUrnaCadastro::ComparecimentoMesario>
        {
            typedef ASN1::SEQUENCE_OF<ModuloResultadoUrnaCadastro::ComparecimentoMesario> Inherited;
        protected:
            typedef Inherited::InfoType InfoType;
            value_type(const void* info) : Inherited(info) {}
        public:
            value_type(size_type n=0) : Inherited(n) {}
            value_type(size_type n, const ModuloResultadoUrnaCadastro::ComparecimentoMesario& val) : Inherited(n, val) {}
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
    }; // end class mesariosEncerramento

    mesariosEncerramento::const_reference get_mesariosEncerramento () const
    {
      assert(hasOptionalField(e_mesariosEncerramento));
      return *static_cast<mesariosEncerramento::const_pointer>(fields[3]);
    }
    mesariosEncerramento::reference ref_mesariosEncerramento ()
    {
      assert(hasOptionalField(e_mesariosEncerramento));
      return *static_cast<mesariosEncerramento::pointer>(fields[3]);
    }
    mesariosEncerramento::reference set_mesariosEncerramento ()
    {
      includeOptionalField( e_mesariosEncerramento, 3);
      return *static_cast<mesariosEncerramento::pointer>(fields[3]);
    }
    void omit_mesariosEncerramento () { removeOptionalField( e_mesariosEncerramento); }
    bool mesariosEncerramento_isPresent () const { return hasOptionalField( e_mesariosEncerramento); }
    void swap(DadosComparecimento& that) { Inherited::swap(that); }
    DadosComparecimento * clone() const
    { return static_cast<DadosComparecimento*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[4];
    static int fieldIds[4];
    static unsigned fieldTags[4];
    static const char* fieldNames[4];
}; // end class DadosComparecimento

//
// ModuloResultadoUrnaCadastro::EntidadeResultadoUrnaCadastro
//

class EntidadeResultadoUrnaCadastro : public ASN1::SEQUENCE
{
    typedef ASN1::SEQUENCE Inherited;
protected:
    typedef Inherited::InfoType InfoType;
    EntidadeResultadoUrnaCadastro(const void* info) : Inherited(info) {}
public:
    EntidadeResultadoUrnaCadastro() : Inherited(&theInfo) {}
    EntidadeResultadoUrnaCadastro(const EntidadeResultadoUrnaCadastro & that) : Inherited(that) {}

    EntidadeResultadoUrnaCadastro& operator=(const EntidadeResultadoUrnaCadastro& that)
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

    class versaoVotacao
    {
    public:
        typedef ASN1::GeneralString value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class versaoVotacao

    versaoVotacao::const_reference get_versaoVotacao () const { return *static_cast<versaoVotacao::const_pointer>(fields[2]); }
    versaoVotacao::reference ref_versaoVotacao () { return *static_cast<versaoVotacao::pointer>(fields[2]); }
    versaoVotacao::reference set_versaoVotacao () { return *static_cast<versaoVotacao::pointer>(fields[2]); }
    versaoVotacao::reference set_versaoVotacao (const ASN1_STD string& v)
    { return *static_cast<versaoVotacao::pointer>(fields[2]) = v; }

    class situacao
    {
    public:
        typedef ModuloTiposEcoUrna::SituacaoArquivo value_type;
        typedef value_type&        reference;
        typedef const value_type&  const_reference;
        typedef value_type*        pointer;
        typedef const value_type*  const_pointer;
    }; // end class situacao

    situacao::const_reference get_situacao () const { return *static_cast<situacao::const_pointer>(fields[3]); }
    situacao::reference ref_situacao () { return *static_cast<situacao::pointer>(fields[3]); }
    situacao::reference set_situacao () { return *static_cast<situacao::pointer>(fields[3]); }
    situacao::reference set_situacao (ModuloTiposEcoUrna::SituacaoArquivo::NamedNumber v)
    { return *static_cast<situacao::pointer>(fields[3]) = v; }

    class infoDadosComparecimento
    {
    public:
        //
        // ModuloResultadoUrnaCadastro::EntidadeResultadoUrnaCadastro::infoDadosComparecimento::value_type
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
              dadosComparecimento0 = 0,
              dadosComparecimentoCifrado1 = 1
            };

            class dadosComparecimento
            {
            public:
                enum Id { eid = 0 };
                typedef ModuloResultadoUrnaCadastro::DadosComparecimento value_type;
                typedef value_type&        reference;
                typedef const value_type&  const_reference;
                typedef value_type*        pointer;
                typedef const value_type*  const_pointer;
            }; // end class dadosComparecimento

            dadosComparecimento::const_reference get_dadosComparecimento () const
            {
                assert(currentSelection() == dadosComparecimento::eid);
                return *static_cast<dadosComparecimento::const_pointer>(choice.get());
            }
            dadosComparecimento::reference ref_dadosComparecimento ()
            {
                assert(currentSelection() == dadosComparecimento::eid);
                return *static_cast<dadosComparecimento::pointer>(choice.get());
            }
            dadosComparecimento::reference select_dadosComparecimento ()
            {
                return *static_cast<dadosComparecimento::pointer>(setSelection(dadosComparecimento::eid, ASN1::AbstractData::create(&dadosComparecimento::value_type::theInfo)));
            }
            bool dadosComparecimento_isSelected () const { return currentSelection() == dadosComparecimento::eid; }

            class dadosComparecimentoCifrado
            {
            public:
                enum Id { eid = 1 };
                typedef ModuloResultadoUrnaCadastro::DadosComparecimentoCifrado value_type;
                typedef value_type&        reference;
                typedef const value_type&  const_reference;
                typedef value_type*        pointer;
                typedef const value_type*  const_pointer;
            }; // end class dadosComparecimentoCifrado

            dadosComparecimentoCifrado::const_reference get_dadosComparecimentoCifrado () const
            {
                assert(currentSelection() == dadosComparecimentoCifrado::eid);
                return *static_cast<dadosComparecimentoCifrado::const_pointer>(choice.get());
            }
            dadosComparecimentoCifrado::reference ref_dadosComparecimentoCifrado ()
            {
                assert(currentSelection() == dadosComparecimentoCifrado::eid);
                return *static_cast<dadosComparecimentoCifrado::pointer>(choice.get());
            }
            dadosComparecimentoCifrado::reference select_dadosComparecimentoCifrado ()
            {
                return *static_cast<dadosComparecimentoCifrado::pointer>(setSelection(dadosComparecimentoCifrado::eid, ASN1::AbstractData::create(&dadosComparecimentoCifrado::value_type::theInfo)));
            }
            bool dadosComparecimentoCifrado_isSelected () const { return currentSelection() == dadosComparecimentoCifrado::eid; }

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
    }; // end class infoDadosComparecimento

    infoDadosComparecimento::const_reference get_infoDadosComparecimento () const { return *static_cast<infoDadosComparecimento::const_pointer>(fields[4]); }
    infoDadosComparecimento::reference ref_infoDadosComparecimento () { return *static_cast<infoDadosComparecimento::pointer>(fields[4]); }
    infoDadosComparecimento::reference set_infoDadosComparecimento () { return *static_cast<infoDadosComparecimento::pointer>(fields[4]); }
    void swap(EntidadeResultadoUrnaCadastro& that) { Inherited::swap(that); }
    EntidadeResultadoUrnaCadastro * clone() const
    { return static_cast<EntidadeResultadoUrnaCadastro*> (Inherited::clone()); }
    static const Inherited::InfoType theInfo;

private:
    static const void* fieldInfos[5];
    static int fieldIds[5];
    static const char* fieldNames[5];
}; // end class EntidadeResultadoUrnaCadastro

} // namespace ModuloResultadoUrnaCadastro

#endif // __MODULORESULTADOURNACADASTRO_H
