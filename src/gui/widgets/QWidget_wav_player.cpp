/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 * A widget that acts as a player of raw WAV audio data. Provides controls for
 * managing the playback, and displays across its surface an image of the audio's
 * waveform.
 *
 */

#include <QtConcurrent/QtConcurrent>
#include <QPainter>
#include <QDebug>
#include "gui/widgets/QWidget_wav_player.h"
#include "gui/windows/mainwindow.h"
#include "wav/wav.h"
#include "common.h"

// Margins, in percentages of the widget's size, of the waveform image displayed
// on the widget.
const double WAVEFORM_HORIZONTAL_MARGIN = 0.97;
const double WAVEFORM_VERTICAL_MARGIN = 0.7;

// How long to wait, in milliseconds, after receiving a request to repaint the
// waveform image until we actually initiate the repaint. For instance, if the
// user is resizing the window, we'll receive many such requests in a short
// period of time, but there's no point regenerating the image at a frequency
// that high.
const unsigned WAVEFORM_REGEN_DELAY = 100;

WavPlayer::WavPlayer(QWidget *parent) :
    QWidget(parent)
{
    this->setStyleSheet("border: none;");

    this->waveformRegenCountdown.setSingleShot(true);
    connect(&this->waveformRegenCountdown, &QTimer::timeout, this, [this]
    {
        if (!this->waveformRegenThread.isFinished())
        {
            this->waveformRegenCountdown.start(WAVEFORM_REGEN_DELAY);
            return;
        }

        // Regenerate the waveform image.
        this->waveformRegenThread = QtConcurrent::run([=]
        {
            QImage newWaveformImage(this->size(), QImage::Format_ARGB32);

            newWaveformImage.fill(QColor("transparent"));

            // Draw the waveform as vertical lines on each horizontal bin. Note that this
            // assumes that each bin has more than one sample in it. If there isn't, the
            // resulting image is likely not visually optimal.
            {
                const unsigned peakSampleValue = *std::max_element(this->wav->samples().begin(),
                                                                   this->wav->samples().end(),
                                                                   [](int16_t a, int16_t b){ return abs(a) < abs(b); });
                const unsigned binWidth = (this->wav->samples().size() / double(newWaveformImage.width()));
                const double yStep = (newWaveformImage.height() / double(peakSampleValue + 1) / 2);

                QPainter painter(&newWaveformImage);

                painter.setPen(QColor("#6e6e6e"));

                for (int x = 0; x < newWaveformImage.width(); x++)
                {
                    const int binMin = *std::min_element(this->wav->samples().begin() + (x * binWidth),
                                                         this->wav->samples().begin() + ((x+1) * binWidth));

                    const int binMax = *std::max_element(this->wav->samples().begin() + (x * binWidth),
                                                         this->wav->samples().begin() + ((x+1) * binWidth));

                    // Draw a vertical line between the highest and lowest point on this bin.
                    painter.drawLine(x, (binMin * yStep + (newWaveformImage.height() / 2)),
                                     x, (binMax * yStep + (newWaveformImage.height() / 2)));
                }
            }

            this->waveformImage = newWaveformImage;

            this->update();
        });
    });

    return;
}

WavPlayer::~WavPlayer()
{
    this->player->stop();

    this->waveformRegenThread.waitForFinished();

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

    regenerate_waveform_image(0);

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
void WavPlayer::regenerate_waveform_image(const int delay)
{
    if (!this->wav->is_valid() ||
        this->wav->samples().empty())
    {
        this->waveformImage = QImage();

        return;
    }

    // Queue a regeneration of the waveform.
    this->waveformRegenCountdown.start(delay < 0? WAVEFORM_REGEN_DELAY : delay);

    return;
}

void WavPlayer::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.fillRect(this->rect(), QColor("#464646"));

    // Draw the waveform image in the middle of the widget.
    if (!this->waveformImage.isNull())
    {
        const unsigned xOffs = (this->width() * ((1 - WAVEFORM_HORIZONTAL_MARGIN) / 2));
        const unsigned yOffs = (this->height() * ((1 - WAVEFORM_VERTICAL_MARGIN) / 2));

        painter.drawImage(xOffs, (yOffs + 3), // +3 to make room for the position indicator.
                          this->waveformImage.scaled((this->width() * WAVEFORM_HORIZONTAL_MARGIN),
                                                     (this->height() * WAVEFORM_VERTICAL_MARGIN),
                                                     Qt::IgnoreAspectRatio, Qt::FastTransformation));

        // Draw a playback position indicator.
        {
            const int indicatorOffs = (this->width() * WAVEFORM_HORIZONTAL_MARGIN * this->playback().pos_percent());

            painter.setPen(QColor("dimgray"));
            painter.setBrush(this->playback().is_playing()? QColor("dimgray") : QColor("transparent"));

            painter.translate((xOffs + indicatorOffs), 3);

            const QPolygon triangle({QPoint(-5, 0), QPoint(5, 0), QPoint(0, 5)});
            painter.drawPolygon(triangle);
        }
    }

    return;
}

void WavPlayer::resizeEvent(QResizeEvent *event)
{
    regenerate_waveform_image();

    QWidget::resizeEvent(event);

    return;
}
