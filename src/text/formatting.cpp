/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 * Appends formatting metadata to the given text, which is expected to be a string
 * of transcription produced by the user; e.g. "Man: Hello there, snowman."
 *
 * For instance, let's assume that the user has just transcribed the following:
 *
 *      "Man: Hello there,snowman."
 *
 * This string contains a syntax error: a missing space following the comma after
 * "there". To highlight this error to the user so that it doesn't go unnoticed,
 * ranssi wants the comma to be rendered in a visually distinct manner. That's
 * where this formatting class comes into play.
 *
 * The class's functionality inserts formatting tags - HTML-like tags, e.g. <span
 * style='color: red;'> - into the text, so that the QTextEdit-based text editor
 * widget knows to render these parts of the text in the desired, non-standard
 * manner.
 *
 * Taking the above example,
 *
 *      "Man: Hello there,snowman.",
 *
 * the formatter will return a new string; something like (but not necessarily
 * exactly like)
 *
 *      "Man: Hello there<...>,</...>snowman."
 *
 * Which is to say, a string in which the erroneous comma segment is encapsulated
 * inside formatting tags.
 *
 * The type of tag(s) used depends on the style of formatting desired; in this
 * case, perhaps something along the lines of
 *
 *      "Man: Hello there<span style='background-color: red;'>,</span>snowman."
 *
 */

#include <QStringList>
#include <QString>
#include <unordered_map>
#include <map>
#include "text/formatting.h"
#include "text/elements.h"
#include "text/syntax.h"
#include "common.h"

// Map formatting tags to their metadata. The text is first passed through a
// syntax checker, which creates spans of metadata, which in turn get converted
// into the corresponding tag strings - defined here - that are then inserted
// into the text.
const std::unordered_map<unsigned/*metadata enum*/, QStringList/*opening and closing tags*/> formattingTags =
{
    {unsigned(text_metadata_e::formatting_bold),
    {"<span style='font-weight: bold;'>", "</span>"}},

    {unsigned(text_metadata_e::formatting_error),
    {"<span style='color: lightgray; background-color: brown;'>", "</span>"}},
};

text_formatting_c::text_formatting_c(const QString &transcribedText) :
    transcription(transcribedText)
{
    k_assert(!this->transcription.isEmpty(), "Expected non-empty text for formatting.");

    return;
}

// Returns a copy of the text with relevant formatting tags inserted.
// Will return a null QString if either the speaker name or utterance, or both,
// are empty in the given text. If a null QString is returned, the formatting
// is to be considered failed.
QString text_formatting_c::formatted(void)
{
    // Handle the speaker name and utterance separately, so we can e.g. display
    // the name in bold and the utterance in a normal font.
    const QString speaker = formatted_speaker_name(text_elements_c(this->transcription).speaker());
    const QString utterance = formatted_utterance(text_elements_c(this->transcription).utterance());

    if (speaker.isEmpty() || utterance.isEmpty()) return QString();

    return QString(speaker + ": " + utterance);
}

QString text_formatting_c::formatted_speaker_name(const QString &speakerName)
{
    QString formattedName = speakerName;

    const std::set<QString> validSpeakerNames = {"this", "that", "other"};

    // Valid speaker names are to be shown in bold; invalid ones as erroneous.
    const QStringList tag = (validSpeakerNames.find(speakerName) != validSpeakerNames.end()
                             ? formattingTags.at(unsigned(text_metadata_e::formatting_bold))
                             : formattingTags.at(unsigned(text_metadata_e::formatting_error)));

    formattedName.prepend(tag.first());
    formattedName.append(tag.last());

    return formattedName;
}

QString text_formatting_c::formatted_utterance(const QString &utterance)
{
    // Derive the list of formatting tags to be inserted into the text.
    std::map<unsigned/*character idx in text*/, QStringList/*formatting tags at this idx"*/> tags;
    {
        auto add_tags = [&tags](const text_metadata_span_s &span,
                                const QStringList &openCloseTags)
        {
            k_assert((openCloseTags.count() == 2), "Expected two tags: <...> and </...>.");

            tags[span.start] << openCloseTags.first();
            tags[span.start+span.length] << openCloseTags.last();
        };

        const auto syntaxSpans = text_syntax_c(utterance).spans();

        for (auto span: syntaxSpans)
        {
            if (span.metadata.type == text_metadata_type_e::formatting)
            {
                add_tags(span, formattingTags.at(unsigned(span.metadata.data)));
            }
        }
    }

    // Insert the formatting tags to the text.
    QString formattedUtterance;
    {
        for (int i = 0; i <= utterance.length(); i++)
        {
            // There might be multiple tags for a given character index; we just
            // join them together without spaces.
            if (tags.find(i) != tags.end()) formattedUtterance += tags[i].join("");

            if (i < utterance.length()) formattedUtterance += utterance[i];
        }
    }

    return formattedUtterance;
}
