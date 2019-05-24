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

    MainWindow w;
    w.show();

    return a.exec();
}
