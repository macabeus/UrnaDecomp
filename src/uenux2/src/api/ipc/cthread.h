#pragma once

#include <memory>

namespace api {

class IThreadImpl;

/**
 * Thread da aplicação: a classe derivada implementa o laço de execução.
 */
class CThread {
public:
    CThread();
    virtual ~CThread();

    void Start();

protected:
    virtual void Run() = 0;

private:
    int m_estado;
    bool m_iniciada;
    bool m_parar;
    std::unique_ptr<IThreadImpl> m_impl;
    std::unique_ptr<IThreadImpl> m_implEspera;
};

} // namespace api
