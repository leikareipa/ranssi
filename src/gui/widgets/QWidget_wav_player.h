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

    void load_wav_data(const std::string &wavFilename);

    wav_playback_c& playback(void) const;

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *event);

private:
    void regenerate_waveform_image(const int delay = -1);

    std::unique_ptr<wav_playback_c> player;

    // The WAV audio that the player will play.
    std::unique_ptr<wav_c> wav;

    // The waveform image we'll generate from the WAV's data.
    QImage waveformImage;

    // For regenerating the waveform image in a separate, non-GUI thread.
    QFuture<void> waveformRegenThread;

    // If we're asked to regenerate the waveform image while fulfilling a previous
    // request to do so, we'll use this timer to queue a retry.
    QTimer waveformRegenCountdown;
};

#endif
