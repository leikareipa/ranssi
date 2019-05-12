/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 */

#include <QApplication>
#include "gui/windows/mainwindow.h"
#include "wav/wav.h"
#include "common.h"

void test_wav(void);

int main(int argc, char *argv[])
{
    /// Load in a test WAV file. This is temporary. In the future, we'll
    /// probably load a project file instead, which includes the WAV file,
    /// and we pass that to the main window.
    const wav_c wavFile("test2.wav");

    QApplication a(argc, argv);

    MainWindow w(wavFile);
    w.show();

    return a.exec();
}
