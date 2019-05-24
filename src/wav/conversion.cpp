/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 * Converts files (sound files, video files, etc.) into ranssi-compatible WAV
 * files.
 *
 */

#include <QFileInfo>
#include <QString>
#include "wav/conversion.h"

// Attempts to convert the given file into a ranssi-compatible WAV file using
// FFmpeg. Returns true if the conversion succeeded; false otherwise (including
// if FFmpeg was not found on the system).
bool wav_conversion_c::convert_to_wav_file(const QString &srcFilePath, const QString &dstFilePath)
{
    QString sysCommand = QString("ffmpeg "
                                 "-n " // Stop and return false if the output file already exists.
                                 "-i '%1' "
                                 "-flags bitexact "
                                 "-map_metadata -1 "
                                 "-acodec pcm_s16le "
                                 "-ac 1 "
                                 "'%2'").arg(srcFilePath).arg(dstFilePath);

    return bool(std::system(qPrintable(sysCommand)) == 0);
}
