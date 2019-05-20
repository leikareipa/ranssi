/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 */

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <unordered_map>
#include "wav/playback.h"

class wav_playback_c;
class project_c;
class QShortcut;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const project_c& ranssiProject);
    ~MainWindow(void);

private:
    void update_window_title(void);

    Ui::MainWindow *ui;

    // The keyboard shortcuts assigned for this window.
    std::unordered_map<std::string/*QKeySequence string*/, QShortcut*> keyboardShortcuts;

    // The transcription project the user is now working on.
    const project_c &project;
};

#endif
