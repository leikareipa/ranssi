/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 * Ranssi is a minimalist and to-the-point transcription app.
 *
 */

#include <QShortcut>
#include <QDebug>
#include "gui/windows/mainwindow.h"
#include "project/project.h"
#include "ui_mainwindow.h"
#include "common.h"

MainWindow::MainWindow(const project_c &project) :
    QMainWindow(nullptr),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->wavPlayer->set_wav_data(project.wav);

    this->setWindowTitle(QString::fromStdString(project.title) + " - ranssi");

    this->setStyleSheet("#MainWindow { background-color: #1e1e1e; }");

    // Set up keyboard shortcuts.
    {
        // Creates a new QShortcut instance assigned to the given QKeySequence-
        // compatible sequence string (e.g. "F1" for the F1 key).
        auto keyboardShortcut = [this](const std::string keySequence)->QShortcut*
        {
            k_assert((this->keyboardShortcuts.find(keySequence) == this->keyboardShortcuts.end()),
                      "Attempting to overwrite an existing keyboard shortcut.");

            QShortcut *shortcut = new QShortcut(QKeySequence(keySequence.c_str()), this);

            return (this->keyboardShortcuts[keySequence] = shortcut);
        };

        connect(keyboardShortcut("f1"), &QShortcut::activated, [this]
        {
            ui->wavPlayer->playback().rewind_ms(2500);
        });

        connect(keyboardShortcut("f2"), &QShortcut::activated, [this]
        {
            ui->wavPlayer->playback().forward_ms(2500);
        });

        connect(keyboardShortcut("esc"), &QShortcut::activated, [this]
        {
            ui->wavPlayer->playback().toggle_pause();
        });
    }

    return;
}

MainWindow::~MainWindow(void)
{
    delete ui;

    return;
}
