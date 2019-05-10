/*
 * Tarpeeksi Hyvae Soft 2019 /
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
    // Loads in the audio data from the given WAV file.
    wav_c(const std::string wavFilename);
    ~wav_c(void);

    const std::vector<int16_t>& samples(void) const;

    int sample_rate(void) const;

    // Can be called to find out whether the audio data has been successfully loaded.
    bool is_valid(void) const;

private:
    // Returns the discrete sample data from a mono WAV file of 16-bit samples,
    // together with the audio's sample rate.
    //
    // The WAV's filename (this->filename) is expected to the set prior to calling
    // this function.
    std::pair<std::vector<int16_t>, int> load_audio_data(void) const;

    // The name of the WAV file from which the audio data were loaded.
    const std::string filename;

    // Discrete samples and the audio's original sample rate.
    const std::pair<std::vector<int16_t>/*samples*/, int/*sample rate*/> audioData;
};

#endif
