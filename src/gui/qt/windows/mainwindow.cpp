#include "src/gui/qt/windows/mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    return;
}

MainWindow::~MainWindow()
{
    delete ui;

    return;
}
