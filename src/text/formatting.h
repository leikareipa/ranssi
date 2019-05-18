/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 */

#ifndef TEXT_FORMATTING_H
#define TEXT_FORMATTING_H

#include <QString>
#include "text/metadata_span.h"

class text_formatting_c
{
public:
    text_formatting_c(const QString &text);

    QString formatted(void);

private:
    const QString text;
};

#endif
