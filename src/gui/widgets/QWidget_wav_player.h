/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 */

#ifndef WAV_PLAYER_H
#define WAV_PLAYER_H

#include <QPointer>
#include <QWidget>

class wav_c;

class WavPlayer : public QWidget
{
    Q_OBJECT

public:
    WavPlayer(QWidget *parent = 0);
    ~WavPlayer();

    void copy_wav_data(const wav_c &wav);

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *event);

private:
    void update_waveform_image(void);

    // The raw data of the WAV of which we'll display a waveform.
    struct
    {
        std::vector<int16_t> samples;
        uint sampleRate;
    } wav;

    // The waveform image we generate from the WAV sample data, and which will
    // be displayed on this widget.
    QImage waveformImage;
};

#endif
