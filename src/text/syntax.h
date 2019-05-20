/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 * Parses the given text - which is expected to be a string of transcription
 * produced by the user - and returns a list of metadata describing relevent
 * syntactical properties of the text.
 *
 */

#ifndef TEXT_VALIDATION_H
#define TEXT_VALIDATION_H

#include <QString>
#include <vector>
#include <set>
#include "text/metadata.h"

class text_syntax_c
{
public:
    text_syntax_c(const QString &utteranceText);

    std::vector<text_metadata_span_s> spans(void);

private:
    const QString utterance;
};

#endif
