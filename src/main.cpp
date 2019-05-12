/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 */

#include <QApplication>
#include "gui/windows/mainwindow.h"
#include "wav/wav.h"
#include "common.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    /// Load in a test WAV file. This is temporary. In the future, we'll
    /// probably load a project file instead, which includes the WAV file,
    /// and we pass that to the main window.
    wav_c wav("test2.wav");
    wav.player().resume();

    MainWindow w(wav);
    w.show();

    return a.exec();
}
