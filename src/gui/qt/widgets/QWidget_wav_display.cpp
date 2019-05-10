/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 * A widget that displays across its surface an image of a waveform.
 *
 */

#include <QPainter>
#include "gui/qt/widgets/QWidget_wav_display.h"

WavDisplay::WavDisplay(QWidget *parent) : QWidget(parent)
{
    return;
}

WavDisplay::~WavDisplay()
{
    return;
}

void WavDisplay::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.fillRect(this->rect(), QColor("green"));

    return;
}
