#include "ecourna/api/security/base64.hpp"

#include <cstdlib>
#include <cstring>

#include "ecourna/api/security/esecurityerror.hpp"

namespace ecourna::api::security {

namespace {

const char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                               'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                               'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                               'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};
const unsigned int mod_table[] = {0, 2, 1};

char* base64_encode(const unsigned char* data, size_t input_length, size_t* output_length)
{
    *output_length = 4 * ((input_length + 2) / 3);

    char* encoded_data = static_cast<char*>(malloc(*output_length));
    if (encoded_data == NULL) {
        return NULL;
    }

    for (size_t i = 0, j = 0; i < input_length;) {
        uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
    }

    for (size_t i = 0; i < mod_table[input_length % 3]; i++) {
        encoded_data[*output_length - 1 - i] = '=';
    }

    return encoded_data;
}

size_t Base64Encode(const std::vector<uebyte>& dados, char* saida, size_t tamanhoSaida)
{
    if (saida == NULL || dados.data() == NULL || dados.empty() || tamanhoSaida < (dados.size() / 3 + dados.size() % 3) * 4) {
        return 0;
    }
    size_t tamanhoCodificado = 0;
    char* codificado = base64_encode(dados.data(), dados.size(), &tamanhoCodificado);
    if (codificado == NULL) {
        return 0;
    }
    if (tamanhoCodificado > tamanhoSaida) {
        free(codificado);
        return 0;
    }
    memcpy(saida, codificado, tamanhoCodificado);
    free(codificado);
    return tamanhoCodificado;
}

} // namespace

// Blank lines below keep both throws on the lines the target's std::source_location records
// (ledger/source-locations.tsv).

// (original lines 71-243: content not recovered)
#line 244
std::string EncodeBase64(const std::vector<uebyte>& dados)
{
    if (dados.empty()) {
        throw CSecurityError(ESE_VETOR_DADOS_VAZIO, "Vetor de dados vazio.");
    }

    std::string resultado;
    const size_t tamanho = (dados.size() / 3 + dados.size() % 3) * 4;
    char* buffer = static_cast<char*>(malloc(tamanho));
    const size_t escrito = Base64Encode(dados, buffer, tamanho);

    // (original lines 255-264: content not recovered)
#line 265
    if (escrito == 0) {
        free(buffer);
        throw CSecurityError(ESE_FALHA_BASE64, "Nao foi converter para base64.");
    }
    resultado.assign(buffer, escrito);
    free(buffer);
    return resultado;
}

} // namespace ecourna::api::security
