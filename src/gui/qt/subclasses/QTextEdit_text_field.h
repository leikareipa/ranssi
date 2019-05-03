#ifndef TEXT_FIELD_H
#define TEXT_FIELD_H

#include <QTextEdit>

class TextField : public QTextEdit
{
    Q_OBJECT

public:
    TextField(QWidget *parent = 0);
    ~TextField();
};

#endif
