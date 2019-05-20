/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 * Appends formatting metadata to the given text, which is expected to be a string
 * of transcription produced by the user; e.g. "Man: Hello there, snowman."
 *
 */

#ifndef TEXT_FORMATTING_H
#define TEXT_FORMATTING_H

#include <QString>
#include "text/metadata.h"

class text_formatting_c
{
public:
    text_formatting_c(const QString &transcribedText);

    QString formatted(void);

private:
    const QString transcription;

    QString formatted_speaker_name(const QString &speakerName);

    QString formatted_utterance(const QString &utterance);
};

#endif
