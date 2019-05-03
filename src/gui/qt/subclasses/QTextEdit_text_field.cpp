#include <QTextEdit>
#include <QPainter>
#include <QDebug>
#include "src/gui/qt/subclasses/QTextEdit_text_field.h"

TextField::TextField(QWidget *parent) : QTextEdit(parent)
{
    /// Temporary styling.
    this->setCursorWidth(0);
    this->setFont(QFont("Ubuntu Mono", 13));

    return;
}

TextField::~TextField()
{
    return;
}
