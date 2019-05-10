#include <QApplication>
#include "src/gui/qt/windows/mainwindow.h"

void test_wav(void);

int main(int argc, char *argv[])
{
    test_wav();

    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}

#include "wav/wav.h"
#include <cassert>
void test_wav(void)
{
    // Two seconds of a 1 kHz sine wave at a rate of 44100.
    wav_c wav("test.wav");

    assert(wav.is_valid() &&
           wav.sample_rate() == 44100 &&
           wav.samples().size() == 88200
           && "Failed to load the WAV file.");

    return;
}
