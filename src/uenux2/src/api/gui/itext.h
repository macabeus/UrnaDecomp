#pragma once

#include <memory>
#include <string>

namespace api {

enum ETextAlignment {
    ALINHAMENTO_ESQUERDA = 0,
    ALINHAMENTO_DIREITA = 1,
    ALINHAMENTO_CENTRO = 2,
};

/**
 * Fonte do texto exibido por um campo.
 */
class IText {
public:
    virtual ~IText() = default;
    virtual std::string GetText() const = 0;
    virtual ETextAlignment GetAlignment() const = 0;
};

using SharedIText = std::shared_ptr<IText>;

} // namespace api
