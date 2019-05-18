/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 * Appends formatting metadata to the given text. The text, in this case, is a
 * string of transcription produced by the user; e.g. "Hello there, snowman."
 *
 * For instance, let's assume that we're given the string "Hello there,snowman."
 * It contains a syntax error: a missing space following the comma after "there".
 * To draw the user's attention to the error, ranssi wants the comma to be drawn
 * in a visually distinct manner. That's where the formatter class comes into
 * play - it inserts formatting tags into the string, so that when rendered by
 * QTextEdit, the text will display with the desired visual effect (while the
 * formatting tags themselves remain invisible to the user).
 *
 * When given the string "Hello there,snowman.", the formatter will return a new
 * string - something like "Hello there<...>,</...>snowman." - in other words, a
 * string in which the erroneous comma segment is encapsulated inside HTML-like
 * tags, such that QTextEdit will render this text in the desired way. The actual
 * content of the tags will depend on the formatting desired; in this example,
 * the full output might be "Hello there<span style='background-color: red;'>,
 * </span>snowman."
 *
 */

#include <QStringList>
#include <QString>
#include <map>
#include "text/formatting.h"
#include "text/syntax.h"
#include "common.h"

text_formatting_c::text_formatting_c(const QString &text) :
    text(text)
{
    k_assert(!this->text.isEmpty(), "Expected non-empty text for formatting.");

    return;
}

// Return a copy of the text into which relevant formatting tags have been inserted.
QString text_formatting_c::formatted(void)
{
    // Parse the text to find which formatting tags should be inserted and where.
    std::map<uint        /*character idx in the text where the formatting begins*/,
             QStringList /*formatting tags at this idx"*/> formatting;
    {
        auto add_tags = [&formatting](const text_metadata_span_s &span,
                                      const QString &startTag,
                                      const QString &endTag)
        {
            formatting[span.start] << startTag;
            formatting[span.start+span.length] << endTag;
        };

        // Parse the text's syntax to find which kinds of tags should be inserted
        // and where.
        const auto syntaxSpans = text_syntax_c(this->text).spans();

        // Create a list of the relevant formatting tags based on the syntax parse.
        for (auto span: syntaxSpans)
        {
            switch (span.metadata)
            {
                case text_metadata_span_s::metadata_e::error:
                {
                    add_tags(span, "<span style='color: lightgray; background-color: brown;'>",
                                   "</span>");

                    break;
                }

                case text_metadata_span_s::metadata_e::bold:
                {
                    add_tags(span, "<span style='font-weight: bold;'>",
                                   "</span>");

                    break;
                }

                /// TODO. Other types of formatting.

                default: k_assert(0, "Unknown syntax span."); break;
            }
        }
    }

    // Append the formatting tags to the text.
    QString formattedText;
    {
        for (int i = 0; i <= this->text.length(); i++)
        {
            if (formatting.find(i) != formatting.end()) formattedText += formatting[i].join("");

            if (i < this->text.length()) formattedText += this->text[i];
        }
    }

    return formattedText;
}
