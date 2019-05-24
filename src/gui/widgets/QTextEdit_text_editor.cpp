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
#include "common.h"

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

        connect(this, &TextEditor::cursorPositionChanged, this, [=]{ update_overlay(); });
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

bool TextEditor::eventFilter(QObject *, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        const QKeyEvent *const keyEvent = static_cast<QKeyEvent*>(event);

        // Accept the current text block, and start a new one.
        if (keyEvent->key() == Qt::Key_Enter ||
            keyEvent->key() == Qt::Key_Return)
        {
            const QString transcribedText = text_in_block(this->textCursor());
            const QString textFormatted = text_formatting_c(transcribedText).formatted();

            // Don't acceps blocks whose text failed to format.
            if (textFormatted.isEmpty()) return true;

            // Insert the formatted block into the editor, replacing the raw text
            // the user had entered.
            erase_text_in_block(this->textCursor());
            insert_text_into_block(textFormatted, this->textCursor());

            begin_new_block();

            return true;
        }
    }

    return false;
}

// Finalizes the current text block (e.g. by assigning to it the vertical block
// margin), and moves the cursor to a new block.
void TextEditor::begin_new_block(void)
{
    QTextBlock currentBlock = this->document()->findBlockByLineNumber(this->textCursor().blockNumber());

    // Separate each block with vertical whitespace.
    QTextBlockFormat currentBlockFormat = currentBlock.blockFormat();
    currentBlockFormat.setTopMargin(BLOCK_VERTICAL_MARGIN);

    // Prevent any styling of the previous block from carrying over to the next one.
    QTextCharFormat currentBlockCharFormat = currentBlock.charFormat();
    currentBlockCharFormat = TextEditor::document()->findBlockByNumber(0).charFormat();

    this->textCursor().insertBlock(currentBlockFormat, currentBlockCharFormat);
    this->setTextCursor(textCursor());

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

        if (!transcriptionFile.isOpen()) return false;

        QTextStream output(&transcriptionFile);
        output.setCodec("utf-8");
        output << transcriptionText;
    }

    // Verify that the data was saved correctly.
    {
        QFile transcriptionFile(transcriptionFilename);
        transcriptionFile.open(QIODevice::ReadOnly | QIODevice::Text);

        return bool(QString::fromUtf8(transcriptionFile.readAll()) == transcriptionText);
    }
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
