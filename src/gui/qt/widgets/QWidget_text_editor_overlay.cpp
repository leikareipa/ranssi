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

static const uint TEXT_CURSOR_WIDTH = 9;

TextEditorOverlay::TextEditorOverlay(QWidget *parent) : QWidget(parent)
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

// Called to communicate to the overlay, prior to repainting it, where the parent
// text editor's cursor is.
void TextEditorOverlay::set_cursor_rect(const QRect &cursorRect)
{
    this->cursorRect = cursorRect;

    return;
}

// Called to communicate to the overlay, prior to repainting it, what's displayed
// on the parent text editor.
void TextEditorOverlay::set_background_pixmap(const QPixmap &backgroundPixmap)
{
    this->backgroundPixmap = backgroundPixmap;

    return;
}

void TextEditorOverlay::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    // Draw an inverted rectangle where the text editor's cursor is.
    QRect fullSizeCursorRect = QRect(this->cursorRect.left()+1,
                               this->cursorRect.top()+1,
                               TEXT_CURSOR_WIDTH,
                               this->cursorRect.height()-1);
    QImage cursorImage = this->backgroundPixmap.copy(fullSizeCursorRect).toImage();
    cursorImage.invertPixels();
    painter.drawImage(fullSizeCursorRect, cursorImage);

    return;
}
