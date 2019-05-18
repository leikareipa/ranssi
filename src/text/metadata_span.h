/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 */

#ifndef TEXT_METADATA_SPAN_H
#define TEXT_METADATA_SPAN_H

// Metadata for a span of text.
struct text_metadata_span_s
{
    // Character index in the text where this metadata begins.
    unsigned start;

    // The length, in characters of the text, that this metadata spans,
    // beginning from the start index.
    unsigned length;

    enum class metadata_e
    {
        error,    // Erroneous span; e.g. a spelling or syntax mistake.
        bold,     // Bold span; text to be shown in boldface.
    } metadata;
};

#endif
