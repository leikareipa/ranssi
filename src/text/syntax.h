/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 */

#ifndef TEXT_VALIDATION_H
#define TEXT_VALIDATION_H

#include <QString>
#include <vector>
#include <set>
#include "text/metadata_span.h"

class text_syntax_c
{
public:
    text_syntax_c(const QString &text);

    std::vector<text_metadata_span_s> spans(void);

private:
    const QString text;
};

#endif
