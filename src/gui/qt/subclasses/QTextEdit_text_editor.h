#ifndef TEXT_EDITOR_H
#define TEXT_EDITOR_H

#include <QTextEdit>

class QPaintEvent;

class TextEditor : public QTextEdit
{
    Q_OBJECT

public:
    TextEditor(QWidget *parent = 0);
    ~TextEditor();
};

#endif
