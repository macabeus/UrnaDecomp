#pragma once

#include <dirent.h>
#include <string>

namespace api {

/**
 * Percorre as entradas de um diretório.
 */
class CDirReader {
public:
    explicit CDirReader(const std::string& diretorio);

    bool NextEntry();

private:
    std::string m_diretorio;
    DIR* m_dir = nullptr;
    dirent* m_entrada = nullptr;
};

} // namespace api
