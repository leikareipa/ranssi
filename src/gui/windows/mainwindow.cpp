/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 * Ranssi is a minimalist and to-the-point transcription app.
 *
 */

#include <QShortcut>
#include <QDebug>
#include <QTime>
#include "gui/windows/mainwindow.h"
#include "project/project.h"
#include "ui_mainwindow.h"
#include "wav/playback.h"
#include "common.h"

MainWindow::MainWindow(const project_c &ranssiProject) :
    QMainWindow(nullptr),
    ui(new Ui::MainWindow),
    project(ranssiProject)
{
    ui->setupUi(this);

    this->setStyleSheet("#MainWindow { background-color: #1e1e1e; }");

    // Set up the WAV player.
      {
        ui->wavPlayer->set_wav_data(project.wav);

        connect(&ui->wavPlayer->playback(), &wav_playback_c::pos_changed, this, [this]
        {
            update_window_title();
        });
    }

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

    update_window_title();

    return;
}

MainWindow::~MainWindow(void)
{
    delete ui;

    return;
}

void MainWindow::update_window_title(void)
{
    const QString title = QString::fromStdString(project.title);

    const QString playbackTimestamp = QTime(0, 0).addMSecs(std::max(0, ui->wavPlayer->playback().pos_ms()))
                                                 .toString("hh:mm:ss");

    /// TODO. There's no way to query the playback's tempo as of yet.
    const QString playbackTempo = "x1.00";

    this->setWindowTitle(title + " - "
                         + playbackTimestamp + " "
                         + playbackTempo + " - "
                         + "ranssi");

    return;
}
