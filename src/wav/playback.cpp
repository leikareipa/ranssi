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

// Creates a playback object suitable for the given WAV object. Note that at the
// moment, only 16-bit mono WAV data is supported.
wav_playback_c::wav_playback_c(const wav_c &wav) :
    sampleBuffer(wav_16bit_as_8bit_samples(wav))
{
    // Define the audio format.
    {
        // The playback functionality only supports 16-bit mono audio.
        const unsigned numChannels = 1;
        const unsigned bitsPerSample = 16;

        k_assert((wav.num_channels() == numChannels), "Mismatched channel count for WAV playback.");
        k_assert((wav.bits_per_sample() == bitsPerSample), "Mismatched sample size for WAV playback.");

        this->audio.format.setSampleRate(wav.sample_rate());
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

        // Leave the audio engine running in the background, waiting for sound data.
        this->audio.output->start(this);
        this->audio.output->suspend();

        this->open(QIODevice::Unbuffered | QIODevice::ReadOnly);
        this->seek(0);

        k_assert((this->audio.output->error() == QAudio::NoError), "Failed to initialize audio playback.");
    }

    QObject::connect(this->audio.output, &QAudioOutput::stateChanged, [=](const QAudio::State state)
    {
        switch (state)
        {
            case QAudio::State::SuspendedState:
            case QAudio::State::StoppedState:
            case QAudio::State::IdleState:
            {
                this->audio.isPlaying = false;
                emit stopped();
                break;
            }

            case QAudio::State::ActiveState:
            {
                this->audio.isPlaying = true;
                emit started();
                break;
            }

            default: break;
        }
    });

    return;
}

wav_playback_c::~wav_playback_c(void)
{
    this->stop();

    return;
}

// Returns the playback's current audio position, in milliseconds, counting from
// the beginning of the audio. Assumes that the audio samples are 16 bits each,
// each presented as a pair of 8-bit values.
int wav_playback_c::pos_ms(void) const
{
    return int(((this->pos() - this->audio.output->bufferSize()) / 2.0) / (this->audio.format.sampleRate() / 1000.0));
}

// Returns the playback's current audio position, in percentage of the audio's
// total length, counting from the beginning of the audio.
double wav_playback_c::pos_percent(void) const
{
    return ((this->pos() - this->audio.output->bufferSize()) / double(this->sampleBuffer.size()));
}

// Seek the playback forward or backward by the given delta, relative to the
// playback's current position. Assumes that the audio samples are 16 bits each,
// each presented as a pair of 8-bit values.
void wav_playback_c::seek_ms(const int deltaMs)
{
    const int newSampleIdx = std::min(int(this->sampleBuffer.size()),
                                      std::max(0, int((pos_ms() + deltaMs) * (this->audio.format.sampleRate() / 1000.0) * 2)));

    this->seek(newSampleIdx);

    emit pos_changed();

    return;
}

// Takes in a WAV of 16-bit samples, and returns its samples as consecutive
// 8-bit pairs.
std::vector<uchar> wav_playback_c::wav_16bit_as_8bit_samples(const wav_c &wav)
{
    k_assert((wav.bits_per_sample() == 16), "Expected 16-bit WAV data.");

    std::vector<uchar> samples8bit;

    // Convert the 16-bit samples to 8-bit pairs.
    for (auto sample: wav.samples())
    {
        samples8bit.push_back(sample & 0xff);
        samples8bit.push_back((sample >> 8) & 0xff);
    }

    return samples8bit;
}

bool wav_playback_c::resume(void)
{
    this->audio.playLimit = std::numeric_limits<decltype(this->audio.playLimit)>::max();

    this->audio.output->start(this);

    return (this->audio.output->error() == QAudio::NoError);
}

void wav_playback_c::stop(void)
{
    if (this->audio.isPlaying)
    {
        this->audio.output->stop();

        // Make sure to position the stream back to the last position from which
        // audio was actually played (and not just buffered from).
        this->seek(std::max(qint64(0),
                            (this->pos() - this->audio.output->bufferSize() - this->bytesToWrite())));

        // Align to 16-bit samples.
        if (this->pos() % 2 != 0) this->seek(this->pos() - 1);
    }

    return;
}

void wav_playback_c::rewind_ms(const unsigned ms)
{
    this->seek_ms(-ms);

    return;
}

void wav_playback_c::forward_ms(const unsigned ms)
{
    this->seek_ms(ms);

    return;
}

void wav_playback_c::toggle_pause(void)
{
    if (this->audio.isPlaying)
    {
        this->stop();
    }
    else this->resume();

    return;
}

bool wav_playback_c::is_playing(void) const
{
    return this->audio.isPlaying;
}

// Qt reference: "Reads up to maxSize bytes from the device into dst, and returns
// the number of bytes read or -1 if an error occurred."
qint64 wav_playback_c::readData(char *dst, qint64 maxSize)
{
    emit pos_changed();

    this->audio.streamPos = this->pos();
    this->audio.playbackPos = ((this->audio.streamPos / 2) / (this->audio.format.sampleRate() / 1000.0));

    // Make sure we don't try to read into an empty sound buffer.
    if (this->audio.streamPos >= int(this->sampleBuffer.size())) return 0;

    // Make sure we don't try to read more from the sound buffer than is available in there.
    if ((this->audio.streamPos + maxSize) >= int(this->sampleBuffer.size())) maxSize = (this->sampleBuffer.size() - this->audio.streamPos);

    if (this->audio.playLimit <= 0) return 0;

    if ((this->audio.playLimit - maxSize) < 0) maxSize = (maxSize - this->audio.playLimit);

    this->audio.playLimit -= maxSize;

    std::memcpy(dst, &this->sampleBuffer[this->audio.streamPos], maxSize);

    return maxSize;
}

// Qt reference: "Writes up to maxSize bytes from src to the device. Returns the
// number of bytes written, or -1 if an error occurred."
qint64 wav_playback_c::writeData(const char *src, qint64 maxSize)
{
    (void)src;
    (void)maxSize;

    return 0;
}

// Qt reference: "Returns the number of bytes that are available for reading. This
// function is commonly used with sequential devices to determine the number of
// bytes to allocate in a buffer before reading."
qint64 wav_playback_c::bytesAvailable(void) const
{
    return (this->sampleBuffer.size() + QIODevice::bytesAvailable());
}
