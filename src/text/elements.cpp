/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 * Extracts constituents from a transcribed string of text.
 *
 * For instance, given the text
 *
 *     "Mr. ABC: Hello there. Let's begin the interview...",
 *
 * the constituents are:
 *
 *     speaker: "Mr. ABC",
 *     utterance: "Hello there. Let's begin the interview...".
 *
 * It holds, then, that, reading from the left, anything up to the first instance
 * of ':' constitutes the speaker name; and anything past it the utterance.
 *
 * Note that the constituent strings are returned with whitespace trimmed from
 * across the string. So given the text
 *
 *     " Ape Escape     :               Hat   hut  ",
 *
 * the constituents are:
 *
 *     speaker: "Ape Escape",
 *     utterance: "Hat hut".
 *
 */

#include "text/elements.h"

text_elements_c::text_elements_c(const QString &text) :
    text(text)
{
    return;
}

// Returns a null QString if a speaker name was not found.
QString text_elements_c::speaker(void) const
{
    // If no speaker name found.
    if (this->text.indexOf(":") == -1) return QString();

    return this->text.left(this->text.indexOf(":")).simplified();
}

QString text_elements_c::utterance(void) const
{
    return this->text.right(this->text.length() - this->text.indexOf(":") - 1).simplified();
}
