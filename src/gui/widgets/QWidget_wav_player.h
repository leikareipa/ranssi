/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 */

#ifndef WAV_PLAYER_H
#define WAV_PLAYER_H

#include <QPointer>
#include <QWidget>
#include <memory>
#include "wav/playback.h"

class wav_c;

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
    void update_waveform_image(void);

    // The WAV audio the player will play.
    std::unique_ptr<wav_c> wav;

    std::unique_ptr<wav_playback_c> player;

    // The waveform image we generate from the WAV sample data, and which will
    // be displayed on this widget.
    QImage waveformImage;
};

#endif
