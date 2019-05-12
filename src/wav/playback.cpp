/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 * Plays back raw sound data using Qt's audio output functionality.
 *
 */

#include <cstring>
#include "wav/playback.h"
#include "wav/wav.h"
#include "common.h"

wav_playback_c::wav_playback_c(uint sampleRate, uint numChannels, uint bitsPerSample)
{
    // Define the audio format.
    {
        this->audio.format.setSampleRate(sampleRate);
        this->audio.format.setChannelCount(numChannels);
        this->audio.format.setSampleSize(bitsPerSample);
        this->audio.format.setCodec("audio/pcm");
        this->audio.format.setByteOrder(QAudioFormat::LittleEndian);
        this->audio.format.setSampleType(QAudioFormat::UnSignedInt);

        k_assert(QAudioDeviceInfo::defaultOutputDevice().isFormatSupported(this->audio.format),
                 "The audio format is not supported by the audio backend.");
    }

    // Create an audio output.
    {
        this->audio.output = new QAudioOutput(this->audio.format, this);
        this->audio.output->setBufferSize(7000);

        // Leave the audio engine running in the background, waiting for sound data.
        this->audio.output->start(this);
        this->audio.output->suspend();

        this->open(QIODevice::Unbuffered | QIODevice::ReadOnly);
        this->seek(0);
    }

    QObject::connect(this->audio.output, &QAudioOutput::stateChanged, [=](const QAudio::State state)
    {
        switch (state)
        {
            case QAudio::State::IdleState: this->audio.isPlaying = false; break;
            case QAudio::State::ActiveState: this->audio.isPlaying = true; break;
            default: break;
        }
    });

    k_assert((this->audio.output->error() == QAudio::NoError), "Failed to initialize audio playback.");

    return;
}

wav_playback_c::~wav_playback_c()
{
    return;
}

void wav_playback_c::copy_wav_data(const wav_c &wav)
{
    k_assert(!this->audio.isPlaying, "Not allowed to assign audio data while the audio is playing.");
    k_assert((wav.sample_rate() == this->audio.format.sampleRate()), "Mismatched sample rate for new audio data.");
    k_assert((wav.num_channels() == this->audio.format.channelCount()), "Mismatched channel count for new audio data.");
    k_assert((wav.bits_per_sample() == this->audio.format.sampleSize()), "Mismatched bit count for new audio data.");

    this->audio.data.clear();

    // Convert the 16-bit samples to 8-bit pairs.
    for (auto sample: wav.samples())
    {
        this->audio.data.push_back(sample & 0xff);
        this->audio.data.push_back((sample >> 8) & 0xff);
    }

    return;
}

bool wav_playback_c::resume(void)
{
    this->audio.playLimit = std::numeric_limits<int>::max();

    this->audio.output->start(this);

    return (this->audio.output->error() == QAudio::NoError);
}

// Qt reference: "Reads up to maxSize bytes from the device into data, and returns
// the number of bytes read or -1 if an error occurred."
qint64 wav_playback_c::readData(char *dst, qint64 maxSize)
{
    this->audio.streamPos = this->pos();
    this->audio.playbackPos = ((this->audio.streamPos / 2) / (this->audio.format.sampleRate() / 1000.0));

    // Make sure we don't try to read into an empty sound buffer.
    if (this->audio.streamPos >= int(this->audio.data.size())) return 0;

    // Make sure we don't try to read more from the sound buffer than is available in there.
    if ((this->audio.streamPos + maxSize) >= int(this->audio.data.size())) maxSize = (this->audio.data.size() - this->audio.streamPos);

    // Also don't read over the play limit, if any.
    if ((this->audio.playLimit <= 0) || (this->audio.playLimit - maxSize) == 0) return 0;

    if ((this->audio.playLimit - maxSize) < 0) maxSize = (maxSize - this->audio.playLimit);

    this->audio.playLimit -= maxSize;

    std::memcpy(dst, &this->audio.data[this->audio.streamPos], maxSize);

    return maxSize;
}

// Qt reference: "Writes up to maxSize bytes from data to the device. Returns the
// number of bytes written, or -1 if an error occurred."
qint64 wav_playback_c::writeData(const char *dst, qint64 maxSize)
{
    (void)dst;
    (void)maxSize;

    return 0;
}

// Qt reference: "Returns the number of bytes that are available for reading. This
// function is commonly used with sequential devices to determine the number of
// bytes to allocate in a buffer before reading."
qint64 wav_playback_c::bytesAvailable() const
{
    return (this->audio.data.size() + QIODevice::bytesAvailable());
}
