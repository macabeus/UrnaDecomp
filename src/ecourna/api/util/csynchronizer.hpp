#pragma once
#include <unistd.h>

namespace ecourna::api::util {

// Process-wide switch deciding whether CFile::Sync() really fsyncs its descriptor.
class CSynchronizer {
public:
    static void CreateInst(bool habilitado);

    static CSynchronizer& GetInst()
    {
        if (s_instancia == nullptr) {
            CreateInst(true);
        }
        return *s_instancia;
    }

    int Sync(int descritor) const { return m_habilitado ? fsync(descritor) : 0; }

private:
    explicit CSynchronizer(bool habilitado) : m_habilitado(habilitado) {}

    bool m_habilitado;

    static CSynchronizer* s_instancia;
};

} // namespace ecourna::api::util
