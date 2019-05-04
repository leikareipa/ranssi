#include <QTextEdit>
#include <QPainter>
#include <QDebug>
#include "src/gui/qt/subclasses/QTextEdit_text_editor.h"

TextEditor::TextEditor(QWidget *parent) : QTextEdit(parent)
{
    /// Temporary styling.
    this->setCursorWidth(0);
    this->setFont(QFont("Ubuntu Mono", 13));
    this->setStyleSheet("selection-background-color: gray; selection-color: lightgray;");

    return;
}

TextEditor::~TextEditor()
{
    return;
}
