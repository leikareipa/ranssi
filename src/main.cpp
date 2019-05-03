#include <QApplication>
#include "src/gui/qt/windows/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Disable cursor blink.
    a.setCursorFlashTime(0);

    MainWindow w;
    w.show();

    return a.exec();
}
