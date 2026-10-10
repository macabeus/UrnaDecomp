#pragma once

namespace api {

/**
 * Implementação da thread na plataforma (simulador::CWasmThread no wasm), criada pela IGenericFactory<IThreadImpl>.
 */
// Recovered: the destructor and Create (slot 2, simulador::CWasmThread::Create's record); the three other virtuals
// CWasmThread overrides are not. The file's path, the parameter names and Create being pure are not in the target.
class IThreadImpl {
public:
    virtual ~IThreadImpl() = default;

    virtual void Create(void* (*rotina)(void*), void* parametro) = 0;
};

} // namespace api
