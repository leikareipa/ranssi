/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 */

#ifndef WAV_H
#define WAV_H

#include <vector>
#include <string>

// Loads a WAV (mono, 16-bit samples) file into memory as discrete samples, and
// provides access to said sample data.
class wav_c
{
public:
    wav_c(const std::string wavFilename);
    ~wav_c(void);

    const std::vector<int16_t>& samples(void) const;

    int sample_rate(void) const;

    int num_channels(void) const;

    int bits_per_sample(void) const;

    int duration_ms(void) const;

    bool is_valid(void) const;

private:
    std::pair<std::vector<int16_t>, int> load_audio_data(void) const;

    // The name of the WAV file from which the audio data were loaded.
    const std::string filename;

    // Discrete samples and the audio's original sample rate.
    const std::pair<std::vector<int16_t>/*samples*/, int/*sample rate*/> audioData;
};

#endif
