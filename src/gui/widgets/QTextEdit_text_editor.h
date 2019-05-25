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

    bool save_speaker_names(const QString &speakersFilename);

    void set_speaker_names(const QStringList &speakerNames);

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

    bool is_cursor_inside_speaker_name_element(void);

    bool is_cursor_inside_utterance_element(void);

    // The names of the speakers appearing in this transcription. Will be used
    // for e.g. name autocomplete.
    QStringList speakerNames;
    unsigned speakerNameIdx = 0;
};

#endif
