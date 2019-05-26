/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 * Ranssi's main text editor widget. A subclass of QTextEdit providing the means
 * to enter and edit text; it's where the user types in their transcription.
 *
 * Text entered into the widget is grouped into blocks (QTextBlock), where each
 * block presents an utterance of a particular speaker. For example, the following
 * exchange between Q and A consists of two blocks:
 *
 *      Q: Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do
 *         eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut
 *         enim ad minim veniam, quis nostrud exercitation ullamco laboris
 *         nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor
 *         in reprehenderit in voluptate velit esse cillum dolore eu fugiat
 *         nulla pariatur?
 *
 *      A: Excepteur sint occaecat cupidatat non proident, sunt in culpa qui
 *         officia deserunt mollit anim id est laborum.
 *
 * Blocks are thus in effect paragraphs, potentially spanning multiple lines of
 * text when displayed in the widget but nonetheless being conceptually discrete
 * units that are also visually separated from each other (by e.g. greater
 * vertical whitespace than between lines within blocks).
 *
 */

#include <QTextDocumentFragment>
#include <QGuiApplication>
#include <QTextBlock>
#include <QTextEdit>
#include <QMimeData>
#include <QFileInfo>
#include <QPainter>
#include <QDebug>
#include "gui/widgets/QWidget_text_editor_overlay.h"
#include "gui/widgets/QTextEdit_text_editor.h"
#include "text/formatting.h"
#include "text/elements.h"
#include "text/syntax.h"
#include "gui/utility.h"
#include "common.h"
#include "csv.h"

// Vertical spacing between individual blocks of text.
static const unsigned BLOCK_VERTICAL_MARGIN = 20;

TextEditor::TextEditor(QWidget *parent) : QTextEdit(parent)
{
    // Set up the text editor's overlay.
    {
        TextEditorOverlay *overlay = new TextEditorOverlay(this);

        overlay->raise();

        auto update_overlay = [=]
        {
            // Properly align the overlay over the parent text editor.
            overlay->move(0, 0);
            overlay->resize(this->size());
            overlay->update();
        };

        connect(this, &TextEditor::cursorPositionChanged, this, [=]{ process_cursor_movement(); update_overlay(); });
        connect(this, &TextEditor::textChanged, this, [=]{ update_overlay(); });
        connect(this, &TextEditor::resized, this, [=]{ update_overlay(); });
    }

    /// Temporary styling.
    this->setCursorWidth(0);
    this->setFont(QFont("Ubuntu Mono", 13));
    this->setStyleSheet("QTextEdit {"
                        "  selection-background-color: #a6a6a6;"
                        "  selection-color: black;"
                        "  background-color: #1e1e1e;"
                        "  color: #a6a6a6;"
                        "  border: none;"
                        "}");

    // Disable font anti-aliasing; for stylistic reasons.
    /// In the future, will likely have a user-accessible toggle for this.
    {
        QFont f = this->font();
        f.setStyleStrategy(QFont::NoAntialias);
        this->setFont(f);
    }

    this->previousTextCursor = this->textCursor();

    // Tab will be used for word completion etc.
    this->setTabChangesFocus(false);

    this->installEventFilter(this);

    return;
}

TextEditor::~TextEditor(void)
{
    return;
}

void TextEditor::resizeEvent(QResizeEvent *event)
{
    emit resized();
    QTextEdit::resizeEvent(event);

    return;
}

// Strip formatting from text pasted into the editor.
void TextEditor::insertFromMimeData(const QMimeData *source)
{
    QTextEdit::insertPlainText(source->text());

    return;
}

void TextEditor::strip_block_formatting(QTextCursor cursor)
{
    const QString strippedText = QTextDocumentFragment::fromHtml(cursor.block().text()).toPlainText();

    erase_text_in_block(cursor);
    insert_text_into_block(strippedText, cursor);

    return;
}

// Performs syntax-checking and certain actions of clean-up (like removing trailing
// spaces) on the given cursor's text block, replacing the block's text with the
// modified version. Returns false if there was no text to operate on (i.e. an empty
// block) or if the modified text turned out empty; otherwise returns true.
bool TextEditor::update_block_formatting(QTextCursor cursor)
{
    const QString blockText = text_in_block(cursor);
    if (blockText.isEmpty()) return false;

    const QString formattedText = text_formatting_c(blockText).formatted();
    if (formattedText.isEmpty()) return false;

    erase_text_in_block(cursor);
    insert_text_into_block(formattedText, cursor);

    return true;
}

bool TextEditor::eventFilter(QObject *, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        const QKeyEvent *const keyEvent = static_cast<QKeyEvent*>(event);

        // Accept the current text block, and start a new one.
        if (keyEvent->key() == Qt::Key_Enter ||
            keyEvent->key() == Qt::Key_Return)
        {
            // Ignore this event if the block is not valid.
            if (!update_block_formatting(this->textCursor())) return true;

            // If the current block is the last one in the document, append a
            // new block after it.
            if (this->textCursor().blockNumber() == (this->document()->blockCount()-1) ||
                (QGuiApplication::keyboardModifiers() & Qt::ControlModifier))
            {
                begin_new_block();
            }
            // Otherwise, just move the cursor to the beginning of the next block.
            else
            {
                auto cursor = this->textCursor();
                cursor.movePosition(QTextCursor::NextBlock);
                this->setTextCursor(cursor);
            }

            return true;
        }

        if (keyEvent->key() == Qt::Key_Backspace)
        {
            // If backspace is pressed while at the beginning of a block, all of
            // the current block's text would be moved to the end of the preceding
            // block. We don't want that, so we'll prevent it by eating away (and
            // thus ignoring) the event. But note that if the current block contains
            // no text, we'll allow backspace to remove that block completely.
            if ((this->textCursor().positionInBlock() == 0) &&
                !text_in_block(this->textCursor()).isEmpty())
            {
                return true;
            }
        }

        // String autocomplete.
        if (keyEvent->key() == Qt::Key_Tab)
        {
            const QString currentText = text_in_block(this->textCursor());

            // Speaker name autocomplete.
            if (is_cursor_inside_speaker_name_element())
            {
                if (!this->speakerNames.empty())
                {
                    QString completedSpeakerName;

                    // If the user has typed in some text, find its closest matching
                    // speaker name.
                    const QStringList matchingNames =
                            (currentText.simplified().isEmpty()? QStringList()
                                                               : this->speakerNames.filter(currentText.simplified(), Qt::CaseInsensitive));

                    if (matchingNames.count())
                    {
                        completedSpeakerName = matchingNames.at(0);
                    }
                    else
                    {
                        completedSpeakerName = this->speakerNames[(this->speakerNameIdx++ % this->speakerNames.count())];
                    }

                    /// TODO. Don't replace all the text, just the speaker name part.
                    erase_text_in_block(this->textCursor());
                    insert_text_into_block((completedSpeakerName + ": "), this->textCursor());
                }
            }
            else if (is_cursor_inside_utterance_element())
            {
                /// TODO.

                qDebug() << "utterance completion";
            }

            return true;
        }
    }

    return false;
}

// Gets called when the position of the text cursor changes, i.e. on the
// cursorPositionChanged signal.
void TextEditor::process_cursor_movement(void)
{
    // If the cursor moved from one block to another, force an update on the
    // previous block's formatting, and remove all formatting from the current
    // block. The latter is done as a bit of a kludge, since if you have e.g.
    // a span of bold text (like the speaker name) and begin typing new text
    // within that span, all the new text you produce will be in bold also, and
    // there'll be no way for you to make it not bold, except by locating the
    // cursor outside of the block to force a formatting update on it. Stripping
    // away the formatting gets around that, and the formatting is restored
    // when the cursor leaves the block.
    if (this->previousTextCursor.blockNumber() != this->textCursor().block().blockNumber())
    {
        // The block's text is about to get modified in an automated fashion,
        // which may move the cursor and thus fire another cursorPositionChanged
        // signal. Processing that signal is unnecessary, so we'll inhibit it.
        inhibit_widget_signals_in_block_scope(this);

        update_block_formatting(this->previousTextCursor);

        strip_block_formatting(this->textCursor());
    }

    this->previousTextCursor = this->textCursor();

    return;
}

// Returns true if the text cursor is currently inside the text block's speaker
// name element.
bool TextEditor::is_cursor_inside_speaker_name_element(void)
{
    const QString currentText = text_in_block(this->textCursor());
    const int speakerNameLen = currentText.indexOf(":");

    // If we didn't find a speaker name. We'll assume that whatever text is there
    // is of the speaker name element.
    if (speakerNameLen < 0) return true;

    // If there's nothing but whitespace after the speaker name, interpret for
    // convenience that it's all part of the speaker name element.
    if (currentText.right(currentText.length() - currentText.indexOf(":") - 1).trimmed().isEmpty())
    {
        return true;
    }

    return (this->textCursor().positionInBlock() <= (speakerNameLen + 1)); // +2 to include the ": " that follows a speaker name.
}

// Returns true if the text cursor is currently inside the text block's utterance
// element.
bool TextEditor::is_cursor_inside_utterance_element(void)
{
    const QString currentText = text_in_block(this->textCursor());
    const int speakerNameLen = currentText.indexOf(":");

    // If didn't find a speaker name.
    if (speakerNameLen < 0) return false;

    return (this->textCursor().positionInBlock() > speakerNameLen);
}

void TextEditor::begin_new_block(void)
{
    QTextBlockFormat blockFormat = QTextBlockFormat();
    blockFormat.setTopMargin(BLOCK_VERTICAL_MARGIN);

    this->textCursor().insertBlock(blockFormat);

    return;
}

// Removes all text from the given cursor's block. This will also move the
// cursor to the beginning of the block.
void TextEditor::erase_text_in_block(QTextCursor cursor)
{
    cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    cursor.removeSelectedText();

    return;
}

// Returns as a QString the text in the block of the given cursor, i.e. any text
// the user has entered prior to pressing enter/return.
QString TextEditor::text_in_block(QTextCursor cursor)
{
    return this->document()->findBlockByLineNumber(cursor.blockNumber()).text();
}

// Inserts the given string into the given cursor's block.
void TextEditor::insert_text_into_block(const QString &text, QTextCursor cursor)
{
    // Don't allow empty entries.
    if (text.isEmpty()) return;

    cursor.clearSelection();
    cursor.insertHtml(text);

    return;
}

bool TextEditor::save_speaker_names(const QString &speakersFilename)
{
    // Save the speaker names into the file.
    {
        QFile speakersFile(speakersFilename);
        speakersFile.open(QIODevice::WriteOnly | QIODevice::Text);

        if (!speakersFile.isOpen())
        {
            qCritical() << "Failed to open the project's speaker names file" << speakersFilename;

            return false;
        }

        QTextStream output(&speakersFile);
        output.setCodec("utf-8");

        for (const auto &speaker: this->speakerNames)
        {
            output << "{" << speaker << "},";
        }
    }

    // Verify that the data was saved correctly.
    {
        const auto savedSpeakerNameRows = csv_parse_c(speakersFilename).rows();

        if ((savedSpeakerNameRows.count() != 1) ||
            (savedSpeakerNameRows.at(0) != this->speakerNames))
        {
            qCritical() << "Found incorrect data saved to disk for the project's speaker names file" << speakersFilename;

            return false;
        }
    }

    return true;
}

// Saves the contents of the text editor into the given text file. The visual
// formatting of the text is not saved; and blocks of text will be separated by
// newlines. Returns true if saving succeeded; false otherwise.
bool TextEditor::save_transcription(const QString &transcriptionFilename)
{
    const QString transcriptionText = this->document()->toPlainText();

    // Save the transcription into the file.
    {
        QFile transcriptionFile(transcriptionFilename);
        transcriptionFile.open(QIODevice::WriteOnly | QIODevice::Text);

        if (!transcriptionFile.isOpen())
        {
            qCritical() << "Failed to open the project's transcription file" << transcriptionFilename;

            return false;
        }

        QTextStream output(&transcriptionFile);
        output.setCodec("utf-8");
        output << transcriptionText;
    }

    // Verify that the data was saved correctly.
    {
        QFile transcriptionFile(transcriptionFilename);
        transcriptionFile.open(QIODevice::ReadOnly | QIODevice::Text);

        if (QString::fromUtf8(transcriptionFile.readAll()) != transcriptionText)
        {
            qCritical() << "Found incorrect data saved to disk for the project's transcription file" << transcriptionFilename;

            return false;
        }
    }

    return true;
}

// Given the path to a text file containing a transcription where each utterance
// is separated by a newline, e.g. "Man: Hello there!\nOther man: Yo.", will insert
// each of the utterances into the editor as a new block of text. Additionally,
// each block will be run through the syntax checker and appended with relevant
// visual formatting.
void TextEditor::load_transcription(const QString &transcriptionFilename)
{
    if (!QFileInfo(transcriptionFilename).exists()) return;

    /// TODO. For now, bail if we try to overwrite existing text. In the future,
    /// we'll likely have a system that queries the user on whether they want to
    /// overwrite or not.
    k_assert(this->document()->isEmpty(), "Was asked to initialize the contents of a non-empty text editor.");

    QFile transcriptionFile(transcriptionFilename);
    transcriptionFile.open(QIODevice::ReadOnly | QIODevice::Text);

    k_assert(transcriptionFile.isOpen(), "Failed to open the given transcription file for loading.");

    const QStringList paragraphs = QString::fromUtf8(transcriptionFile.readAll()).split("\n");
    for (const auto &paragraph: paragraphs)
    {
        if (paragraph.isEmpty()) continue;

        insert_text_into_block(text_formatting_c(paragraph).formatted(), this->textCursor());

        begin_new_block();
    }

    return;
}

void TextEditor::set_speaker_names(const QStringList &speakerNames)
{
    this->speakerNames = speakerNames;

    return;
}
