#include <QPainter>
#include <QRect>
#include "src/gui/qt/subclasses/QWidget_text_field_overlay.h"

static const uint CURSOR_WIDTH = 9;

TextFieldOverlay::TextFieldOverlay(QWidget *parent) : QWidget(parent)
{
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

void TextFieldOverlay::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.setPen(QColor("lightgray"));
    painter.setBrush(QBrush(QColor("lightgray")));

    painter.drawRect(QRect(cursorRect.left()+1, cursorRect.top(), CURSOR_WIDTH, cursorRect.height()));

    return;
}
