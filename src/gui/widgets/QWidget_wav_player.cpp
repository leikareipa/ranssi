/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 * A widget that acts as a player of raw WAV audio data. Provides controls for
 * managing the playback, and displays across its surface an image of the audio's
 * waveform.
 *
 */

#include <QPainter>
#include <QDebug>
#include "gui/widgets/QWidget_wav_player.h"
#include "gui/windows/mainwindow.h"
#include "wav/wav.h"
#include "common.h"

WavPlayer::WavPlayer(QWidget *parent) :
    QWidget(parent)
{
    this->setStyleSheet("border: none;");

    return;
}

WavPlayer::~WavPlayer()
{
    this->player->stop();

    return;
}

// Assigns the raw audio data of the given WAV to the player.
void WavPlayer::load_wav_data(const std::string &wavFilename)
{
    if (this->player) this->player->stop();

    this->wav.reset(new wav_c(wavFilename));

    this->player.reset(new wav_playback_c(*this->wav));

    connect(this->player.get(), &wav_playback_c::stopped, this, [this]{ this->update(); });
    connect(this->player.get(), &wav_playback_c::started, this, [this]{ this->update(); });
    connect(this->player.get(), &wav_playback_c::pos_changed, this, [this]{ this->update(); });

    update_waveform_image();

    return;
}

wav_playback_c& WavPlayer::playback() const
{
    k_assert(this->player, "Tried to access the WAV player before it had been initialized.");

    return *this->player;
}

// (Re-)generates the image of the waveform that we'll display on this widget.
// You might call this, for instance, when the WAV data changes, or when the
// widget is resized.
void WavPlayer::update_waveform_image(void)
{
    if (!this->wav->is_valid() ||
        this->wav->samples().empty())
    {
        // Assign a null image.
        this->waveformImage = QImage();

        return;
    }

    const unsigned waveformWidth = (this->width() * 0.97);
    const unsigned waveformHeight = (this->height() * 0.70);

    this->waveformImage = QImage(waveformWidth, waveformHeight, QImage::Format_ARGB32);
    this->waveformImage.fill(QColor("transparent"));

    // Draw the waveform as vertical lines on each horizontal bin. Note that this
    // assumes that each bin has more than one sample in it. If there isn't, the
    // resulting image is likely not visually optimal.
    {
        const unsigned peakSampleValue = *std::max_element(this->wav->samples().begin(),
                                                           this->wav->samples().end(),
                                                           [](int16_t a, int16_t b){ return abs(a) < abs(b); });
        const unsigned binWidth = (this->wav->samples().size() / double(waveformWidth));
        const double yStep = (waveformHeight / double(peakSampleValue + 1) / 2);

        QPainter painter(&this->waveformImage);

        painter.setPen(QColor("#6e6e6e"));

        for (unsigned x = 0; x < waveformWidth; x++)
        {
            const int binMin = *std::min_element(this->wav->samples().begin() + (x * binWidth),
                                                 this->wav->samples().begin() + ((x+1) * binWidth));
            const int binMax = *std::max_element(this->wav->samples().begin() + (x * binWidth),
                                                 this->wav->samples().begin() + ((x+1) * binWidth));

            // Draw a vertical line between the highest and lowest point on this bin.
            painter.drawLine(x, (binMin * yStep + (waveformHeight / 2)),
                             x, (binMax * yStep + (waveformHeight / 2)));
        }
    }

    this->update();

    return;
}

void WavPlayer::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.fillRect(this->rect(), QColor("#464646"));

    // Draw the waveform image in the middle of the widget.
    if (!this->waveformImage.isNull())
    {
        painter.drawImage(((this->width() - this->waveformImage.width()) / 2),
                          ((this->height() - this->waveformImage.height()) / 2 + 3), // +3 to make some vertical room for the audio position indicator.
                          this->waveformImage);

        // Draw a playback position indicator.
        {
            const int indicatorOffs = (this->waveformImage.width() * this->playback().pos_percent());

            painter.setPen(QColor("dimgray"));
            painter.setBrush(this->playback().is_playing()? QColor("dimgray") : QColor("transparent"));

            painter.translate(((this->width() - this->waveformImage.width()) / 2.0 + indicatorOffs), 3);

            const QPolygon triangle({QPoint(-5, 0), QPoint(5, 0), QPoint(0, 5)});
            painter.drawPolygon(triangle);
        }
    }

    return;
}

void WavPlayer::resizeEvent(QResizeEvent *event)
{
    update_waveform_image();
    QWidget::resizeEvent(event);

    return;
}
