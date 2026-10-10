#pragma once
#include <string>
#include <vector>

#include "asn1/asn1.h"
#include "ecourna/api/io/cfile.hpp"
#include "ecourna/api/io/cioerror.hpp"

namespace ecourna::api::io {

// (original lines 11-36: content not recovered)
#line 37
// BER-decodes buffer into entidade and insists on a valid result.
template <typename ClasseASN1>
void DeserializeFromBuffer(ClasseASN1& entidade, const std::vector<char>& buffer)
{
    ASN1::CoderEnv env;
    env.set_encodingRule(ASN1::CoderEnv::ber);
    if (!env.decode(buffer.begin(), buffer.end(), entidade, false)) {
        throw CIoError(EIO_DECODE_BER, EFileOperation::None, "Falha ao fazer decode BER.");
    }
    if (!entidade.isValid() || !entidade.isStrictlyValid()) {
        throw CIoError(EIO_CONTEUDO_INVALIDO, EFileOperation::None, "Foi lido conteúdo inválido.");
    }
}

// Reads the whole file through CFile and decodes it.
template <typename ClasseASN1>
void DeserializeFromFile(ClasseASN1& entidade, const std::string& nome)
{
    CFile arquivo(nome, "rb");
    arquivo.Seek(0, SEEK_END);
    long tamanho = arquivo.Position();
    std::vector<char> buffer(tamanho);
    arquivo.Seek(0, SEEK_SET);
    uedword lidos = arquivo.RawRead(buffer.data(), buffer.size());
    arquivo.Close();
    buffer.resize(lidos);
    DeserializeFromBuffer(entidade, buffer);
}

} // namespace ecourna::api::io
