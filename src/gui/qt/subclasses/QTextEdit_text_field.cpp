#include <QTextEdit>
#include "src/gui/qt/subclasses/QTextEdit_text_field.h"

TextField::TextField(QWidget *parent) : QTextEdit(parent)
{
    /// Temporary styling.
    this->setCursorWidth(10);
    this->setFont(QFont("Ubuntu Mono", 13));

    return;
}

TextField::~TextField()
{
    return;
}
