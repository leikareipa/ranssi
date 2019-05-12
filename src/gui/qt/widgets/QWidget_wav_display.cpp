/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 * A widget that displays across its surface an image of a waveform.
 *
 */

#include <QPainter>
#include <QDebug>
#include "gui/qt/widgets/QWidget_wav_display.h"
#include "gui/qt/windows/mainwindow.h"
#include "wav/wav.h"

WavDisplay::WavDisplay(QWidget *parent) :
    QWidget(parent)
{
    this->setStyleSheet("border: none;");

    return;
}

WavDisplay::~WavDisplay()
{
    return;
}

void WavDisplay::copy_wav_data(const wav_c &wav)
{
    this->wav.samples = wav.samples();
    this->wav.sampleRate = wav.sample_rate();

    update_waveform_image();

    return;
}

// (Re-)generates the image of the waveform that we'll display on this widget.
// You might call this, for instance, when the WAV data changes, or when the
// widget is resized.
void WavDisplay::update_waveform_image(void)
{
    if (this->wav.samples.empty())
    {
        // Assign a null image.
        this->waveformImage = QImage();

        return;
    }

    const uint waveformWidth = (this->width() * 0.97);
    const uint waveformHeight = (this->height() * 0.70);

    this->waveformImage = QImage(waveformWidth, waveformHeight, QImage::Format_ARGB32);
    this->waveformImage.fill(QColor("transparent"));

    // Draw the waveform as vertical lines on each horizontal bin. Note that this
    // assumes that each bin has more than one sample in it. If there isn't, the
    // resulting image is likely not visually optimal.
    {
        const uint peakSampleValue = *std::max_element(this->wav.samples.begin(),
                                                       this->wav.samples.end(),
                                                       [](int16_t a, int16_t b){ return abs(a) < abs(b); });
        const uint binWidth = (this->wav.samples.size() / double(waveformWidth));
        const double yStep = (waveformHeight / double(peakSampleValue + 1) / 2);

        QPainter painter(&this->waveformImage);

        painter.setPen(QColor("#6e6e6e"));

        for (uint x = 0; x < waveformWidth; x++)
        {
            const int binMin = *std::min_element(this->wav.samples.begin() + (x * binWidth),
                                                 this->wav.samples.begin() + ((x+1) * binWidth));
            const int binMax = *std::max_element(this->wav.samples.begin() + (x * binWidth),
                                                 this->wav.samples.begin() + ((x+1) * binWidth));

            // Draw a vertical line between the highest and lowest point on this bin.
            painter.drawLine(x, (binMin * yStep + (waveformHeight / 2)),
                             x, (binMax * yStep + (waveformHeight / 2)));
        }
    }

    this->update();

    return;
}

void WavDisplay::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.fillRect(this->rect(), QColor("#464646"));

    // Draw the waveform image in the middle of the widget.
    if (!this->waveformImage.isNull())
    {
        painter.drawImage(((this->width() - this->waveformImage.width()) / 2),
                          ((this->height() - this->waveformImage.height()) / 2 + 3), // +3 to make some vertical room for the audio position indicator.
                          this->waveformImage);
    }

    return;
}

void WavDisplay::resizeEvent(QResizeEvent *event)
{
    update_waveform_image();
    QWidget::resizeEvent(event);

    return;
}
