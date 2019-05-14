/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 */

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <memory>
#include "wav/playback.h"

class wav_playback_c;
class project_c;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const project_c& project);
    ~MainWindow(void);

private:
    Ui::MainWindow *ui;
};

#endif
