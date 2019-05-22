/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 * Ranssi is a minimalist and to-the-point transcription app.
 *
 */

#include <QShortcut>
#include <QSplitter>
#include <QDebug>
#include <QTime>
#include <QFile>
#include "gui/widgets/QTextEdit_text_editor.h"
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

    // Set up the WAV player with the project's data.
    {
        ui->wavPlayer->load_wav_data(project.filenames.wavFile);

        connect(&ui->wavPlayer->playback(), &wav_playback_c::pos_changed, this, [this]
        {
            update_window_title();
        });
    }

    // Set up the text editor.
    {
        ui->textEditor->load_transcription(project.filenames.transcriptionFile);
    }

    // Set up a splitter between the WAV player and text editor; mainly so the
    // WAV player can be made smaller or larger vertically.
    {
        QSplitter *const splitter = new QSplitter(this);
        splitter->addWidget(ui->wavPlayer);
        splitter->addWidget(ui->textEditor);
        splitter->setOrientation(Qt::Vertical);
        splitter->setStretchFactor(0, 1);
        splitter->setStretchFactor(1, 1);
        splitter->setHandleWidth(11);
        ui->centralWidget->layout()->addWidget(splitter);
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

        connect(keyboardShortcut("ctrl+s"), &QShortcut::activated, [this]
        {
            ui->textEditor->save_transcription(project.filenames.transcriptionFile);
        });
    }

    // Make sure the window title reflects the properties of the project we just
    // loaded in.
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
    const QString title = QString::fromStdString(project.name);

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
