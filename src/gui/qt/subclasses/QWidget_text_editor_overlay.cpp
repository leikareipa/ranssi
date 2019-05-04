#include <QPainter>
#include <QDebug>
#include <QRect>
#include "gui/qt/subclasses/QWidget_text_editor_overlay.h"

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

void TextEditorOverlay::set_cursor_rect(const QRect &cursorRect)
{
    this->cursorRect = cursorRect;

    return;
}

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
