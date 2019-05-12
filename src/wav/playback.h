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
public:
    wav_playback_c(uint sampleRate, uint numChannels, uint bitsPerSample);
    ~wav_playback_c();

    // Assign the given raw audio samples as the data we'll play back.
    void copy_wav_data(const wav_c &wav);

    // Resume audio playback.
    bool resume(void);

protected:
    qint64 readData(char *dst, qint64 maxSize);
    qint64 writeData(const char *dst, qint64 maxSize);
    qint64 bytesAvailable() const;

private:
    struct
    {
        QAudioFormat format;

        QAudioOutput *output = nullptr;

        bool isPlaying = false;

        // The audio data we're to play.
      //  const uchar *data = nullptr;
        std::vector<uchar> data;
        //int dataLen = 0;

        // The byte position we're at in the audio stream.
        int streamPos;

        // The index of the last raw audio sample we played.
        int playbackPos;

        // Set to >= 0 to have the audio playback be limited to the next n samples.
        int playLimit = 0;
    } audio;
};

#endif
