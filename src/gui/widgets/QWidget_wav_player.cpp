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

    this->waveformUpdateCountdown.setSingleShot(true);
    connect(&this->waveformUpdateCountdown, &QTimer::timeout, this, [this]
    {
        if (!this->waveformUpdateThread.isFinished())
        {
            this->waveformUpdateCountdown.start(WAVEFORM_REGEN_DELAY);
            return;
        }

        this->waveformUpdateThread = QtConcurrent::run([this]{ update_waveform_image(); });
    });

    return;
}

WavPlayer::~WavPlayer()
{
    if (this->player) this->player->stop();

    this->waveformUpdateThread.waitForFinished();

    return;
}

// Call this to request that the waveform image be updated. The update will
// commence in some number of milliseconds. Calling this function again before
// then will reset the timer.
void WavPlayer::queue_waveform_image_update(void)
{
    if (!this->wav ||
        !this->wav->is_valid() ||
        this->wav->samples().empty())
    {
        this->waveformImage = QImage();

        return;
    }

    this->waveformUpdateCountdown.start(WAVEFORM_REGEN_DELAY);

    return;
}

// (Re-)generates the image of the waveform that we'll display on this widget.
// You might call this, for instance, when the WAV data changes, or when the
// widget is resized.
void WavPlayer::update_waveform_image(const QSize imageSize)
{
    QImage updatedImage((imageSize.width() | imageSize.height())? imageSize : this->size(), QImage::Format_ARGB32);

    updatedImage.fill(QColor("transparent"));

    // Draw the waveform as vertical lines on each horizontal bin. Note that this
    // assumes that each bin has more than one sample in it. If there isn't, the
    // resulting image is likely not visually optimal.
    {
        const unsigned peakSampleValue = *std::max_element(this->wav->samples().begin(),
                                                           this->wav->samples().end(),
                                                           [](int16_t a, int16_t b){ return abs(a) < abs(b); });
        const unsigned binWidth = (this->wav->samples().size() / double(updatedImage.width()));
        const double yStep = (updatedImage.height() / double(peakSampleValue + 1) / 2);

        QPainter painter(&updatedImage);

        painter.setPen(QColor("#6e6e6e"));

        for (int x = 0; x < updatedImage.width(); x++)
        {
            const int binMin = *std::min_element(this->wav->samples().begin() + (x * binWidth),
                                                 this->wav->samples().begin() + ((x+1) * binWidth));

            const int binMax = *std::max_element(this->wav->samples().begin() + (x * binWidth),
                                                 this->wav->samples().begin() + ((x+1) * binWidth));

            // Draw a vertical line between the highest and lowest point on this bin.
            painter.drawLine(x, (binMin * yStep + (updatedImage.height() / 2)),
                             x, (binMax * yStep + (updatedImage.height() / 2)));
        }
    }

    this->waveformImage = updatedImage;

    this->update();

    return;
}

// Assigns the raw audio data of the given WAV to the player.
void WavPlayer::load_wav_data(const QString &wavFilename)
{
    if (this->player) this->player->stop();

    this->wav.reset(new wav_c(wavFilename));

    this->player.reset(new wav_playback_c(*this->wav));

    connect(this->player.get(), &wav_playback_c::stopped, this, [this]{ this->update(); });
    connect(this->player.get(), &wav_playback_c::started, this, [this]{ this->update(); });
    connect(this->player.get(), &wav_playback_c::pos_changed, this, [this]{ this->update(); });

    // Since generating an image of the WAV's waveform may take a few seconds,
    // give the user a low-res version of it, initially.
    update_waveform_image(this->size() / 6);

    // Then queue up a re-generation of the waveform image in full resolution in a
    // non-GUI thread.
    queue_waveform_image_update();

    return;
}

wav_playback_c& WavPlayer::playback() const
{
    k_assert(this->player, "Tried to access the WAV player before it had been initialized.");

    return *this->player;
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
    queue_waveform_image_update();

    QWidget::resizeEvent(event);

    return;
}
