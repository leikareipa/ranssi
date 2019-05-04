#include <QPainter>
#include <QDebug>
#include <QRect>
#include "src/gui/qt/subclasses/QWidget_text_field_overlay.h"

static const uint CURSOR_WIDTH = 9;

TextFieldOverlay::TextFieldOverlay(QWidget *parent) : QWidget(parent)
{
    // The overlay is expected to sit on top of the text edit widget, but we
    // don't want it to steal the edit widget's mouse interaction.
    this->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    return;
}

TextFieldOverlay::~TextFieldOverlay()
{
    return;
}

void TextFieldOverlay::set_cursor_rect(const QRect &cursorRect)
{
    this->cursorRect = cursorRect;

    return;
}

void TextFieldOverlay::set_background_pixmap(const QPixmap &backgroundPixmap)
{
    this->backgroundPixmap = backgroundPixmap;

    return;
}

void TextFieldOverlay::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    // Draw an inverted rectangle where the text editor's cursor is.
    QRect fullSizeCursorRect = QRect(this->cursorRect.left()+1,
                               this->cursorRect.top()+1,
                               CURSOR_WIDTH,
                               this->cursorRect.height()-1);
    QImage cursorImage = this->backgroundPixmap.copy(fullSizeCursorRect).toImage();
    cursorImage.invertPixels();
    painter.drawImage(fullSizeCursorRect, cursorImage);

    return;
}
