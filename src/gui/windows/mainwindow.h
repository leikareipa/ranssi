/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 */

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <unordered_map>
#include <memory>
#include "wav/playback.h"

class wav_playback_c;
class Tarpaulin;
class project_c;
class QShortcut;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(void);
    ~MainWindow(void);

    void open_project(const QString &projectDirectory);

protected:
    void resizeEvent(QResizeEvent *);

private:
    void update_window_title(void);

    Ui::MainWindow *ui;

    Tarpaulin *tarp = nullptr;

    // The keyboard shortcuts assigned for this window.
    std::unordered_map<std::string/*QKeySequence string*/, QShortcut*> keyboardShortcuts;

    // The transcription project opened for editing in the window.
    std::unique_ptr<const project_c> project;
};

#endif
