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
#include "gui/widgets/QWidget_text_editor_overlay.h"
#include "gui/widgets/QTextEdit_text_editor.h"

static const uint TEXT_CURSOR_WIDTH = 10;

TextEditorOverlay::TextEditorOverlay(QWidget *parentWidget, TextEditor *textEditorWidget) :
    QWidget(parentWidget),
    textEditor(textEditorWidget)
{
    // The overlay is expected to sit on top of the text edit widget, but we
    // don't want it to steal the edit widget's mouse interaction.
    this->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    this->setFont(textEditorWidget->font());

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

    // Draw a block of color as the text cursor.
    {
        QRect cursorBlock = QRect(textEditor->cursorRect().left(),
                                  textEditor->cursorRect().top(),
                                  TEXT_CURSOR_WIDTH,
                                  textEditor->cursorRect().height());

        painter.setPen(QColor("transparent"));
        painter.setBrush(QColor("#a6a6a6"));

        // If the user has selected text, highlight the cursor block a little bit
        // from the rest of the selection.
        if (!textEditor->textCursor().selectedText().isEmpty()) painter.setBrush(QColor("#d6d6d6"));

        painter.drawRect(cursorBlock);

        // Redraw the character that was under the opaque cursor block.
        const QChar charUnderCursor = textEditor->document()->characterAt(textEditor->textCursor().position());
        if (!charUnderCursor.isNonCharacter())
        {
            painter.setPen(QColor("#1e1e1e"));
            painter.drawText(cursorBlock, charUnderCursor);
        }
    }

    return;
}
