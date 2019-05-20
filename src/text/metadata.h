/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 * A single piece of metadata about a span of text.
 *
 */

#ifndef TEXT_METADATA_H
#define TEXT_METADATA_H

enum class text_metadata_type_e
{
    // Metadata about the visual (rendering) properties of text.
    formatting,
};

enum class text_metadata_e
{
    // The text should be highlighted as erroneous; it's e.g. a spelling or syntax mistake.
    formatting_error,

    // The text should be drawn in boldface.
    formatting_bold,
};

// A single piece of metadata about a span of text.
struct text_metadata_span_s
{
    // Character index in the text where this span of metadata begins.
    unsigned start;

    // The length, in characters of the text, of this span of metadata,
    // beginning from the 'start' index.
    unsigned length;

    struct
    {
        text_metadata_type_e type;
        text_metadata_e data;
    } metadata;
};

#endif
