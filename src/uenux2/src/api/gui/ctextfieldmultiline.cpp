// api::CTextFieldMultiLine (/home/rubio/tse/uenux2/src/api/gui/ctextfieldmultiline.cpp).
// Target functions of this file:
//   5498  CTextFieldMultiLine(const SRect&, const SharedIText&, const SFont&, TColor, TColor)   (here; its throw is
//         the record at line 71, column 15, so the ctor's position in this file is fixed)
//   10921 GetTypeName() const                                                               (here)
//   10923 / 10922  ~CTextFieldMultiLine() (complete / deleting; implicit, emitted with the vtable)
//   5497  GetTextLines() const   record at line 116, col 21: CPolySingleton<IScreen>::instance()  (pending)
//   10925 Draw(IScreen&) const                                                              (pending)
//   10924 Rect() const                                                                      (pending)
//   2783  Move(const SPoint&)    one body binaryen shares with CFillField/CRectField/CTextRectField (pending)
// The pending members are defined in src/units/14-uenux2-ctextfieldmultiline-pending.cpp until GetTextLines' dependencies
// (api/pattern/cpolysingleton.h, fn 512) and primitives.cpp (SRect, fns 1915/1916) are reconstructed.

#include "api/gui/ctextfieldmultiline.h"

#include <cmath>

#include "api/gui/eueguierror.h"

namespace api {

namespace {

/**
 * Divisor da altura da fonte que dá o espaço entre duas linhas: cada linha
 * ocupa a altura da fonte mais um sexto dela, arredondado para cima.
 */
constexpr double DIVISOR_ESPACO_ENTRE_LINHAS = 6.0;

} // namespace

/**
 * Cria um campo de texto com várias linhas.
 *
 * O texto é obtido de @p text a cada desenho e quebrado em linhas que cabem na
 * largura de @p rect, sempre num espaço entre palavras quando há um; uma
 * palavra maior que a largura do retângulo é quebrada no último caractere que
 * cabe. As linhas são desenhadas de cima para baixo a partir do topo do
 * retângulo, alinhadas horizontalmente como o texto pedir (à esquerda, ao
 * centro ou à direita), até a última que cabe inteira na altura do retângulo;
 * as demais são descartadas.
 *
 * A área efetivamente ocupada pelo texto (a união dos retângulos das linhas
 * desenhadas) é guardada no desenho, para que o próximo desenho apague apenas
 * ela com a cor de fundo antes de escrever o texto novo, e é o que Rect()
 * devolve depois do primeiro desenho. Antes dele, Rect() calcula a área que o
 * texto ocuparia.
 *
 * O campo não guarda o texto quebrado: um texto dinâmico (CDataText,
 * CDataTextFmt) é lido de novo a cada desenho.
 *
 * As cores são as da paleta da tela (TColor).
 *
 * @param rect retângulo onde o texto é desenhado
 * @param text texto do campo; não pode ser nulo
 * @param font fonte do texto
 * @param corTexto cor do texto
 * @param corFundo cor do fundo, usada também para apagar o texto anterior
 *
 * @throw CUeGuiError se @p text for nulo
 */
CTextFieldMultiLine::CTextFieldMultiLine(const SRect& rect, const SharedIText& text, const SFont& font, TColor corTexto, TColor corFundo)
    : m_rect(rect)
    , m_text(text)
    , m_font(font)
    , m_corTexto(corTexto)
    , m_corFundo(corFundo)
    , m_alturaLinha(static_cast<TPosition>(font.size + std::ceil(font.size / DIVISOR_ESPACO_ENTRE_LINHAS)))
{
    if (!text) {
        throw CUeGuiError(EUE_GUI_TEXTFIELDMULTILINE_TEXTO_NULO, "Campo estava com o texto nulo");
    }
}

std::string CTextFieldMultiLine::GetTypeName() const
{
    return "CTextFieldMultiLine";
}

} // namespace api
