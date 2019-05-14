/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 * Ranssi is a minimalist and to-the-point transcription app.
 *
 */

#include <QDebug>
#include "gui/windows/mainwindow.h"
#include "project/project.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(const project_c &project) :
    QMainWindow(nullptr),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->wavPlayer->set_wav_data(project.wav);

    this->setStyleSheet("#MainWindow { background-color: #1e1e1e; }");

    return;
}

MainWindow::~MainWindow(void)
{
    delete ui;

    return;
}
