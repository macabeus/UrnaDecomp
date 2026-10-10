#pragma once

#include <string>
#include <vector>

#include "api/gui/iformfield.h"
#include "api/gui/itext.h"
#include "api/gui/primitives.h"

namespace api {

class IScreen;

/**
 * Campo de texto que quebra o texto em várias linhas dentro de um retângulo.
 */
class CTextFieldMultiLine : public IFormField<IScreen> {
public:
    CTextFieldMultiLine(const SRect& rect, const SharedIText& text, const SFont& font, TColor corTexto, TColor corFundo);

    std::string GetTypeName() const override;

    void Draw(IScreen& screen) const override;

    SRect Rect() const override;

    void Move(const SPoint& posicao) override;

private:
    std::vector<std::string> GetTextLines() const;

    SRect m_rect;
    mutable SRect m_rectDesenhado;
    mutable bool m_desenhado = false;
    SharedIText m_text;
    SFont m_font;
    TColor m_corTexto;
    TColor m_corFundo;
    TPosition m_alturaLinha;
};

} // namespace api
