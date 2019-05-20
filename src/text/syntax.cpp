/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 * Parses the given text - which is expected to be a string of transcription
 * produced by the user - and returns a list of metadata describing relevent
 * syntactical properties of the text.
 *
 * For instance, given the transcription
 *
 *      "Hello there,snowman.",
 *
 * the syntax parser would find that there is a missing space following the
 * comma after "there", and mark it with metadata identifying it as a syntax
 * error.
 *
 * The metadata produced can then be used to e.g. format the string for rendering
 * such that syntax errors are highlighted.
 *
 * Note that only the utterance portion of a transcribed string can be submitted
 * to syntax-parsing. Given the transcription
 *
 *      "Man: Hello there, snowman.",
 *
 * the utterance is everything after "Man: ".
 *
 */

#include <QRegularExpression>
#include <QString>
#include "text/syntax.h"
#include "common.h"

text_syntax_c::text_syntax_c(const QString &utteranceText) :
    utterance(utteranceText)
{
    k_assert(!this->utterance.isEmpty(), "Expected non-empty text for syntax validation.");

    k_assert((this->utterance.length() == this->utterance.simplified().length()),
             "Expected text for syntax validation to have been stripped to extraneous whitespace.");

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

            auto matches = r.globalMatch(this->utterance);
            while (matches.hasNext())
            {
                auto match = matches.next();
                syntaxSpans.push_back(text_metadata_span_s{unsigned(match.capturedStart()),
                                                           unsigned(match.capturedLength()),
                                                           text_metadata_type_e::formatting,
                                                           text_metadata_e::formatting_error});
            }
        }
    };

    // Test the text against particular regular expressions to find incorrect syntax.
    // Any spans of text that match any of these expressions will be marked accordingly
    // with metadata.
    /// FIXME. Certain kinds of text, like "...text." get incorrectly marked as errors.
    /// TODO. Add more tests, like for unterminated quotes.
    mark_as_error({
                  // No space following punctuation.
                  {"[.,?!;:][^ ]"},

                  // Punctuation as the string's first character. But allow "...text"
                  // as a continuation of a previous utterance.
                  {"^[,?!;:]+"},

                  // Text ending with whitespace.
                  {"(\\s)+$"},

                  // Text starting with whitespace.
                  {"^(\\s)+"},

                  // Space preceding punctuation ("Text .", "Text !", "Text ,", etc).
                  {" +[.,?!;:]"},

                  // A sentence not ending in sentence-final punctuation.
                  {"[^.!?]$"},

                  // No capitalization following sentence-final punctuation
                  {"(^|[.?!]) *\\p{Ll}"},

                  // Capitalization following non-sentence-final punctuation.
                  {"[:;,] *\\p{Lu}"},

                  // Two subsequent periods (..) but not three (...).
                  ///...

                  // Unterminated quotes.
                  /// ...
                });

    return syntaxSpans;
}
