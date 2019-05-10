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
#include <QPainter>
#include <QDebug>
#include "gui/qt/widgets/QTextEdit_text_editor.h"

// Vertical spacing between individual blocks of text.
static const uint BLOCK_VERTICAL_MARGIN = 20;

TextEditor::TextEditor(QWidget *parent) : QTextEdit(parent)
{
    /// Temporary styling.
    this->setCursorWidth(0);
    this->setFont(QFont("Ubuntu Mono", 13));
    this->setStyleSheet("QTextEdit {"
                        "  selection-background-color: gray;"
                        "  selection-color: lightgray;"
                        "  background-color: #1e1e1e;"
                        "  color: #a6a6a6;"
                        "  border: none;"
                        "}");

    // Tab will be used for word completion etc.
    this->setTabChangesFocus(false);

    this->installEventFilter(this);

    return;
}

TextEditor::~TextEditor()
{
    return;
}

bool TextEditor::eventFilter(QObject *, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        const QKeyEvent *const keyEvent = static_cast<QKeyEvent*>(event);

        // Validate the current text block, and initialize a new one.
        if (keyEvent->key() == Qt::Key_Enter ||
            keyEvent->key() == Qt::Key_Return)
        {
            QString newBlockText = text_in_block(this->textCursor());

            // Disallow adding empty blocks.
            if (newBlockText.isEmpty()) return true;

            // Validate the text.
            {
                /// TODO. newBlockText = validated(newBlockText);
            }

            // Insert the validated text.
            {
                erase_text_in_block(this->textCursor());
                insert_text_into_block(newBlockText, this->textCursor());
            }

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
