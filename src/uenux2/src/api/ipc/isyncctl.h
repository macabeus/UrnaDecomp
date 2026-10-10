#pragma once

namespace api {

/**
 * Controle de sincronização (exclusão mútua).
 */
class ISyncCtl {
public:
    virtual ~ISyncCtl() = default;

    virtual void Lock() = 0;
    virtual void Unlock() = 0;
};

} // namespace api
