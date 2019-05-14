/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 */

#ifndef TEXT_EDITOR_OVERLAY_H
#define TEXT_EDITOR_OVERLAY_H

#include <QPointer>
#include <QWidget>

class TextEditor;

class TextEditorOverlay : public QWidget
{
    Q_OBJECT

public:
    TextEditorOverlay(TextEditor *textEditorWidget);
    ~TextEditorOverlay(void);

protected:
    void paintEvent(QPaintEvent *);

private:
    // A pointer to the text editor for which this is an overlay.
    QPointer<TextEditor> textEditor;
};

#endif
