/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 */

#ifndef TEXT_EDITOR_OVERLAY_H
#define TEXT_EDITOR_OVERLAY_H

#include <QWidget>

class TextEditorOverlay : public QWidget
{
    Q_OBJECT

public:
    TextEditorOverlay(QWidget *parent = 0);
    ~TextEditorOverlay();

    void set_cursor_rect(const QRect &cursorRect);

    void set_background_pixmap(const QPixmap &backgroundPixmap);

protected:
    void paintEvent(QPaintEvent *);

private:
    QRect cursorRect;
    QPixmap backgroundPixmap;
};

#endif
