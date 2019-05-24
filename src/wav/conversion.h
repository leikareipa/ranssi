/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 * Converts audio files (mp3, flac, etc.) into ranssi-compatible WAV files.
 *
 */

#ifndef WAV_CONVERSION_H
#define WAV_CONVERSION_H

class QString;

class wav_conversion_c
{
public:
    static bool convert_to_wav_file(const QString &srcFilePath, const QString &dstFilePath);

private:
};

#endif
