#pragma once

#include <string>

#include "api/gui/primitives.h"

namespace api {

template <typename MEDIA>
class IForm;

/**
 * Campo de um formulário, desenhado numa mídia (tela, papel).
 */
template <typename MEDIA>
class IFormFieldBase {
public:
    virtual ~IFormFieldBase() = default;

    virtual void Draw(MEDIA& media) const = 0;

    virtual void Show() {}

    virtual void Hide() {}

    /**
     * Redesenha o campo se ele foi alterado desde o último desenho.
     *
     * @return true se o campo foi redesenhado
     */
    virtual bool Refresh(MEDIA& media)
    {
        const bool alterado = m_alterado;
        m_alterado = false;
        if (alterado) {
            Draw(media);
        }
        return alterado;
    }

    virtual void SetForm(IForm<MEDIA>* form) { m_form = form; }

    virtual std::string GetTypeName() const = 0;

protected:
    bool m_alterado = false;
    IForm<MEDIA>* m_form = nullptr;
    std::string m_nome;
};

template <typename MEDIA>
class IFormField : public IFormFieldBase<MEDIA> {
public:
    virtual SRect Rect() const = 0;

    virtual void Move(const SPoint& posicao) = 0;
};

} // namespace api
