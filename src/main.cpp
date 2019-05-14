/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 */

#include <QApplication>
#include "gui/windows/mainwindow.h"
#include "project/project.h"
#include "wav/playback.h"
#include "common.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    /// Temporary.
    project_c p("Hello there", "test2.wav");

    MainWindow w(p);
    w.show();

    return a.exec();
}
