/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 */

#ifndef TEXT_EDITOR_H
#define TEXT_EDITOR_H

#include <QTextEdit>

class TextEditor : public QTextEdit
{
    Q_OBJECT

public:
    TextEditor(QWidget *parent = 0);
    ~TextEditor(void);

    void load_transcription(const QString &transcriptionFilename);

    bool save_transcription(const QString &transcriptionFilename);

protected:
    bool eventFilter(QObject *, QEvent *event);
    void resizeEvent(QResizeEvent *event);
    void insertFromMimeData(const QMimeData *source);

signals:
    // Emitted when this widget is resized.
    void resized(void);

private:
    void erase_text_in_block(QTextCursor cursor);

    QString text_in_block(QTextCursor cursor);

    void insert_text_into_block(const QString &text, QTextCursor cursor);

    void begin_new_block(void);
};

#endif
