#pragma once
#include <string>

namespace ecourna::app::dados {

// Who generated a media: the generator's name, its TPM certificate serial and its installation serial. 36 bytes;
// layout from the constructor (fn 5109), the implicit copy constructor (fn 9872) and the converter
// (app/dados/asn/cconversoridentificadorgeradormidia.cpp, fns 9225/9224). Names from ModuloTiposEcoUrna's fields.
class CIdentificadorGeradorMidia {
public:
    CIdentificadorGeradorMidia(const std::string& nome, const std::string& serialCertificadoTPM,
                               const std::string& serialInstalacao);

    const std::string& GetNome() const { return m_nome; }
    const std::string& GetSerialCertificadoTPM() const { return m_serialCertificadoTPM; }
    const std::string& GetSerialInstalacao() const { return m_serialInstalacao; }

private:
    std::string m_nome;
    std::string m_serialCertificadoTPM;
    std::string m_serialInstalacao;
};

} // namespace ecourna::app::dados
