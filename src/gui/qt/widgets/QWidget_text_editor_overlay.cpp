/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 * A widget drawn on top of ranssi's main text editor widget (where the user
 * types in their transcription) for displaying additional visual highlights,
 * like a custom text cursor.
 *
 * The overlay is to be fully transparent besides the visual elements it adds,
 * and likewise transparent for user input, e.g. mouse clicks.
 *
 */

#include <QPainter>
#include <QDebug>
#include <QRect>
#include "gui/qt/widgets/QWidget_text_editor_overlay.h"
#include "gui/qt/widgets/QTextEdit_text_editor.h"

static const uint TEXT_CURSOR_WIDTH = 9;

TextEditorOverlay::TextEditorOverlay(QWidget *parentWidget, TextEditor *textEditorWidget) :
    QWidget(parentWidget),
    textEditor(textEditorWidget)
{
    // The overlay is expected to sit on top of the text edit widget, but we
    // don't want it to steal the edit widget's mouse interaction.
    this->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    return;
}

TextEditorOverlay::~TextEditorOverlay()
{
    return;
}

void TextEditorOverlay::paintEvent(QPaintEvent *)
{
    if (textEditor.isNull()) return;

    // Properly align the overlay over the parent text editor.
    this->move(textEditor->pos());
    this->resize(textEditor->size());

    QPainter painter(this);
    const QPixmap backgroundPixmap = textEditor->grab();

    // Draw an inverted rectangle where the text editor's cursor is.
    {
        QRect fullSizeCursorRect = QRect(textEditor->cursorRect().left()+1,
                                         textEditor->cursorRect().top()+1,
                                         TEXT_CURSOR_WIDTH,
                                         textEditor->cursorRect().height()-1);

        QImage cursorImage = backgroundPixmap.copy(fullSizeCursorRect).toImage();
        cursorImage.invertPixels();
        painter.drawImage(fullSizeCursorRect, cursorImage);
    }

    return;
}
