/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 * Extracts constituents from a transcribed string of text.
 *
 */

#ifndef TEXT_ELEMENTS_H
#define TEXT_ELEMENTS_H

#include <QString>

class text_elements_c
{
public:
    text_elements_c(const QString &text);

    QString speaker(void) const;

    QString utterance(void) const;

private:
    const QString text;
};

#endif
