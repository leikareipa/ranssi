/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 * Plays back raw sound data using Qt's audio output functionality.
 *
 */

#ifndef PLAYBACK_H
#define PLAYBACK_H

#include <QAudioOutput>
#include <QIODevice>

class wav_c;

class wav_playback_c : public QIODevice
{
    Q_OBJECT

public:
    wav_playback_c(const wav_c &wav);
    ~wav_playback_c(void);

    // For user control of the playback.
    bool resume(void);
    void stop(void);
    void rewind_ms(const unsigned ms);
    void forward_ms(const unsigned ms);
    void toggle_pause(void);

protected:
    qint64 readData(char *dst, qint64 maxSize);
    qint64 writeData(const char *dst, qint64 maxSize);
    qint64 bytesAvailable(void) const;

signals:
    // Emitted when playback is started.
    void started(void);

    // Emitted when playback is stopped.
    void stopped(void);

private:
    std::vector<uchar> wav_16bit_as_8bit_samples(const wav_c &wav);

    int pos_ms(void) const;

    void seek_ms(const int deltaMs);

    // The audio data we're to play.
    const std::vector<uchar> sampleBuffer;

    struct
    {
        QAudioFormat format;
        QAudioOutput *output = nullptr;

        bool isPlaying = false;

        // The byte position we're at in the audio stream.
        int streamPos;

        // The index of the last raw audio sample we played.
        int playbackPos;

        // Set to >= 0 to have the audio playback be limited to the next n samples.
        int playLimit = 0;
    } audio;
};

#endif
