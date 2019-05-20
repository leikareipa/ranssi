/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 */

#include <QDataStream>
#include <QFile>
#include "wav/wav.h"
#include "common.h"

// Loads in the audio data from the given WAV file.
wav_c::wav_c(const std::string wavFilename) :
    filename(wavFilename),
    audioData(load_audio_data())
{
    return;
}

wav_c::~wav_c(void)
{
    return;
}

const std::vector<int16_t>& wav_c::samples(void) const
{
    return this->audioData.first;
}

int wav_c::sample_rate(void) const
{
    return this->audioData.second;
}

int wav_c::num_channels(void) const
{
    return 1;
}

int wav_c::bits_per_sample(void) const
{
    return 16;
}

int wav_c::duration_ms(void) const
{
    return ((this->samples().size() / this->sample_rate()) * 1000);
}

// Can be called to find out whether the audio data has been successfully loaded.
bool wav_c::is_valid(void) const
{
    return !this->samples().empty();
}

// Returns the discrete sample data from a mono WAV file of 16-bit samples,
// together with the audio's sample rate.
//
// The WAV's filename (this->filename) is expected to the set prior to calling
// this function.
std::pair<std::vector<int16_t>, int> wav_c::load_audio_data(void) const
{
    k_assert(!this->filename.empty(), "Was asked to load the contents of an unnamed WAV file. Can't do that.");

    // The discrete samples we load from the WAV file and return to our caller.
    std::vector<int16_t> samples;

    // Audio metadata.
    int32_t numSamples = 0;
    int32_t sampleRate = 0;
    int16_t numChannels = 0;
    int16_t bitsPerSample = 0;
    int16_t blockAlign = 0;

    /// Temp. Reports the given error string to the user.
    static auto oops = [](const std::string errorString)
    {
        std::fprintf(stderr, "%s\n", errorString.c_str());
    };

    // Open the WAV file for reading.
    QFile file(QString::fromStdString(this->filename));
    if (!file.open(QIODevice::ReadOnly))
    {
        oops("Could not open the WAV file for reading.");
        return {};
    }

    // Read the WAV file's contents into memory.
    QByteArray fileData = file.readAll();
    if (fileData.isEmpty())
    {
        oops("Could not read any data from the WAV file.");
        return {};
    }
    file.close();

    // Parse the WAV file's data to extract its discrete audio samples.
    /// TODO: Add some more checks and verification, e.g. that the RIFF chunk is
    /// as we expect it to be.
    {
        QDataStream dataStream(fileData);

        // Riff chunk.
        {
            if ((dataStream.skipRawData(4) == -1) ||    // Should be "RIFF".
                (dataStream.skipRawData(4) == -1) ||    // Should be chunk size.
                (dataStream.skipRawData(4) == -1))      // Should be "WAVE".
            {
                oops("Failed to read the WAV file's RIFF chunk.");
                return {};
            }
        }

        // Fmt chunk.
        {
            if ((dataStream.skipRawData(4) <= 0) ||    // Should be "fmt ".
                (dataStream.skipRawData(4) <= 0) ||    // Should be chunk size.
                (dataStream.skipRawData(2) <= 0) ||    // Should be audio format.
                (dataStream.readRawData((char*)&numChannels, 2) <= 0) ||
                (dataStream.readRawData((char*)&sampleRate, 4) <= 0) ||
                (dataStream.skipRawData(4) <= 0) ||    // Should be byte rate.
                (dataStream.readRawData((char*)&blockAlign, 2) <= 0) ||
                (dataStream.readRawData((char*)&bitsPerSample, 2) <= 0))
            {
                oops(("Failed to read the WAV file's fmt chunk."));
                return {};
            }

            // Assert some hard-coded assumptions we have about how the audio should be.
            if ((numChannels != 1) ||     // We expect a mono audio file.
                (bitsPerSample != 16) ||  // We expect 16-bit audio samples.
                (blockAlign != 2))        // We expect 16-bit audio samples.
            {
                oops("Expected the WAV file to be mono and of 16-bit samples.");
                return {};
            }
        }

        // Data chunk.
        {
            if ((dataStream.skipRawData(4) <= 0) ||    // Should be "data".
                (dataStream.readRawData((char*)&numSamples, 4) <= 0))
            {
                oops("Failed to read the WAV file's data chunk.");
                return {};
            }

            numSamples /= blockAlign;

            // The actual sample data.
            {
                samples.reserve(numSamples);

                for (int i = 0; i < numSamples; i++)
                {
                    // Test for this, just in case it hasn't yet been verified
                    // that we have word-sized samples.
                    k_assert((blockAlign == 2), "Expected 16-bit samples in the WAV file.");

                    int16_t sampleDatum;
                    if (dataStream.readRawData((char*)&sampleDatum, 2) <= 0)
                    {
                        oops("Failed to read the WAV file's sample data.");
                        return {};
                    }

                    samples.push_back(sampleDatum);
                }
            }
        }
    }

    return {samples, sampleRate};
}
