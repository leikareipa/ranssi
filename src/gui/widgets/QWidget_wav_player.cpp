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
void WavPlayer::set_wav_data(const wav_c &wav)
{
    if (this->player) this->player->stop();

    this->wavData.samples = wav.samples();
    this->wavData.sampleRate = wav.sample_rate();

    this->player.reset(new wav_playback_c(wav));
    connect(this->player.get(), &wav_playback_c::stopped, this, []{ qDebug() << "Audio playback stopped."; });
    connect(this->player.get(), &wav_playback_c::started, this, []{ qDebug() << "Audio playback started."; });

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
    if (this->wavData.samples.empty())
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
        const uint peakSampleValue = *std::max_element(this->wavData.samples.begin(),
                                                       this->wavData.samples.end(),
                                                       [](int16_t a, int16_t b){ return abs(a) < abs(b); });
        const uint binWidth = (this->wavData.samples.size() / double(waveformWidth));
        const double yStep = (waveformHeight / double(peakSampleValue + 1) / 2);

        QPainter painter(&this->waveformImage);

        painter.setPen(QColor("#6e6e6e"));

        for (uint x = 0; x < waveformWidth; x++)
        {
            const int binMin = *std::min_element(this->wavData.samples.begin() + (x * binWidth),
                                                 this->wavData.samples.begin() + ((x+1) * binWidth));
            const int binMax = *std::max_element(this->wavData.samples.begin() + (x * binWidth),
                                                 this->wavData.samples.begin() + ((x+1) * binWidth));

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
    }

    return;
}

void WavPlayer::resizeEvent(QResizeEvent *event)
{
    update_waveform_image();
    QWidget::resizeEvent(event);

    return;
}
