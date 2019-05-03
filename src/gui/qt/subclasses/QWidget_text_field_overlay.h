#ifndef TEXT_FIELD_OVERLAY_H
#define TEXT_FIELD_OVERLAY_H

#include <QWidget>

class QPaintEvent;
class QRect;

class TextFieldOverlay : public QWidget
{
    Q_OBJECT

public:
    TextFieldOverlay(QWidget *parent = 0);
    ~TextFieldOverlay();

    void set_cursor_rect(const QRect &cursorRect);

protected:
    void paintEvent(QPaintEvent *);

private:
    QRect cursorRect;
};

#endif
