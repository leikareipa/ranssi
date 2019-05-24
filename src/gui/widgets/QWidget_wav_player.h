/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 */

#ifndef WAV_PLAYER_H
#define WAV_PLAYER_H

#include <QFutureWatcher>
#include <QPointer>
#include <QWidget>
#include <QTimer>
#include <memory>
#include "wav/playback.h"

class WavPlayer : public QWidget
{
    Q_OBJECT

public:
    WavPlayer(QWidget *parent = 0);
    ~WavPlayer();

    void load_wav_data(const QString &wavFilename);

    wav_playback_c& playback(void) const;

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *event);

private:
    void queue_waveform_image_update(void);

    void update_waveform_image(const QSize imageSize = QSize(0, 0));

    std::unique_ptr<wav_playback_c> player;

    // The WAV audio that the player will play.
    std::unique_ptr<wav_c> wav;

    // The waveform image we'll generate from the WAV's data.
    QImage waveformImage;

    // For regenerating the waveform image in a separate, non-GUI thread.
    QFuture<void> waveformUpdateThread;

    // If we're asked to regenerate the waveform image while fulfilling a previous
    // request to do so, we'll use this timer to queue a retry.
    QTimer waveformUpdateCountdown;
};

#endif
