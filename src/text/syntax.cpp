/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 * Parses the syntax of the given text, and returns a list of metadata spans
 * describing any issues and other such properties of interest of the text's
 * syntax. The text, in this case, is a string of transcription produced by
 * the user; e.g. "Hello there, snowman."
 *
 * For instance, let's assume that we're given the string "Hello there,snowman."
 * It contains a syntax error: a missing space following the comma after "there".
 * To draw the user's attention to the error, ranssi wants the comma to be drawn
 * in a visually distinct manner. That's where the syntax parser class comes into
 * play - it inspects the string to produce metadata about its syntax; and the
 * metadata can then be fed into a text formatter that embeds the relevant
 * rendering tags into the text.
 *
 */

#include <QRegularExpression>
#include <QString>
#include "text/syntax.h"
#include "common.h"

text_syntax_c::text_syntax_c(const QString &text) :
    text(text)
{
    k_assert(!this->text.isEmpty(), "Expected non-empty text for syntax validation.");

    return;
}

// Inspect the text and return any metadata of interest about the text's syntax.
std::vector<text_metadata_span_s> text_syntax_c::spans(void)
{
    std::vector<text_metadata_span_s> syntaxSpans;

    // Takes in a list of regular expressions, and tests them against the text.
    // Matches will be added to the list of metadata spans as containing errors.
    auto mark_as_error = [this, &syntaxSpans](const std::initializer_list<const char *const> &regexpStrings)
    {
        for (const auto &regexpString: regexpStrings)
        {
            QRegularExpression r(regexpString);

            k_assert(r.isValid(), "Invalid regular expression.");

            auto matches = r.globalMatch(this->text);
            while (matches.hasNext())
            {
                auto match = matches.next();
                syntaxSpans.push_back(text_metadata_span_s{unsigned(match.capturedStart()),
                                                           unsigned(match.capturedLength()),
                                                           text_metadata_span_s::metadata_e::error});
            }
        }
    };

    // Test the text against particualr regular expressions to find incorrect syntax.
    // Any spans of text that match any of these expressions will be marked accordingly
    // with metadata.
    /// FIXME. Certain kinds of text, like "...text." get incorrectly marked as errors.
    /// TODO. Add more tests, like for unterminated quotes.
    mark_as_error({
                  // No space following punctuation.
                  "[.,?!;:][^ ]",

                  // Punctuation as the string's first character. But allow "...text"
                  // as a continuation of a previous utterance.
                  "^[,?!;:]+",

                  // Text ending with whitespace.
                  "(\\s)+$",

                  // Text starting with whitespace.
                  "^(\\s)+",

                  // Space preceding punctuation ("Text .", "Text !", "Text ,", etc).
                  " +[.,?!;:]",

                  // A sentence not ending in sentence-final punctuation.
                  "[^.!?]$",

                  // No capitalization following sentence-final punctuation
                  "(^|[.?!]) *\\p{Ll}",

                  // Capitalization following non-sentence-final punctuation.
                  "[:;,] *\\p{Lu}",

                  // Two subsequent periods (..) but not three (...).
                  ///...

                  // Unterminated quotes.
                  /// ...
                });

    return syntaxSpans;
}
