#pragma once
#include <string>

#include <boost/signals2/signal.hpp>

namespace ecourna::api::pattern {

// Progress source observed through a boost::signals2 signal (atual, total, descricao). Layout from the target:
// vptr + signal (vptr, shared_ptr<signal_impl>) = 16 bytes; the only virtual is the destructor.
class IObservableProgressWithDescription {
public:
    typedef boost::signals2::signal<void(unsigned long, unsigned long, const std::string&)> TSinalProgresso;

    virtual ~IObservableProgressWithDescription() = default;

    boost::signals2::connection ConectarProgresso(const TSinalProgresso::slot_type& observador)
    {
        return m_sinalProgresso.connect(observador);
    }

protected:
    void NotificarProgresso(unsigned long atual, unsigned long total, const std::string& descricao)
    {
        m_sinalProgresso(atual, total, descricao);
    }

private:
    TSinalProgresso m_sinalProgresso;
};

} // namespace ecourna::api::pattern
