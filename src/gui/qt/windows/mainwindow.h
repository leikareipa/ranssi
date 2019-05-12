/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 */

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

class wav_c;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /// The constructor taking in a WAV object is temporary functionality.
    explicit MainWindow(const wav_c &wav);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
};

#endif
