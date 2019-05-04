#ifndef TEXT_FIELD_OVERLAY_H
#define TEXT_FIELD_OVERLAY_H

#include <QWidget>

class QPaintEvent;
class QPixmap;
class QRect;

class TextFieldOverlay : public QWidget
{
    Q_OBJECT

public:
    TextFieldOverlay(QWidget *parent = 0);
    ~TextFieldOverlay();

    void set_cursor_rect(const QRect &cursorRect);

    void set_background_pixmap(const QPixmap &backgroundPixmap);

protected:
    void paintEvent(QPaintEvent *);

private:
    QRect cursorRect;
    QPixmap backgroundPixmap;
};

#endif
