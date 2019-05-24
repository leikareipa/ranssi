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
#include <QDir>
#include "gui/widgets/QTextEdit_text_editor.h"
#include "gui/widgets/QWidget_tarpaulin.h"
#include "gui/windows/mainwindow.h"
#include "project/project.h"
#include "ui_mainwindow.h"
#include "wav/playback.h"
#include "common.h"

MainWindow::MainWindow(void) :
    QMainWindow(nullptr),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setStyleSheet("#MainWindow { background-color: #1e1e1e; }");

    // Set up the tarp.
    {
        this->tarp = new Tarpaulin(this);

        connect(tarp, &Tarpaulin::on, this, [this]
        {
            ui->wavPlayer->setEnabled(false);
            ui->wavPlayer->setVisible(false);

            ui->textEditor->setEnabled(false);
            ui->textEditor->setVisible(false);
        });

        connect(tarp, &Tarpaulin::off, this, [this]
        {
            ui->wavPlayer->setEnabled(true);
            ui->wavPlayer->setVisible(true);

            ui->textEditor->setEnabled(true);
            ui->textEditor->setVisible(true);
        });

        connect(tarp, &Tarpaulin::create_project, this, [this](const QString &audioFilePath)
        {
            create_project(audioFilePath);
        });

        connect(tarp, &Tarpaulin::open_project, this, [this](const QString &projectDirectory)
        {
            open_project(projectDirectory);
        });
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
            save_current_project();
        });
    }

    this->tarp->put_on();

    return;
}

MainWindow::~MainWindow(void)
{
    delete ui;

    return;
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    this->tarp->move(0, 0);
    this->tarp->resize(this->size());

    return;
}

// Opens the project contained in the given directory.
void MainWindow::open_project(const QString &projectDirectory)
{
    this->project.reset(new project_c(projectDirectory));

    if (!this->project->isValid)
    {
        this->project.release();
        this->tarp->put_on();

        qCritical() << "Invalid project directory:" << projectDirectory;

        return;
    }

    ui->wavPlayer->load_wav_data(this->project->filenames.wavFile);
    ui->textEditor->load_transcription(this->project->filenames.transcriptionFile);

    connect(&ui->wavPlayer->playback(), &wav_playback_c::pos_changed, this, [this]
    {
        update_window_title();
    });

    update_window_title();

    this->tarp->pull_back();

    return;
}

// Creates a new project with the given audio file. Returns true if the project
// was successfully created; false otherwise.
bool MainWindow::create_project(const QString &audioFilePath)
{
    if (!QFileInfo(audioFilePath).exists())
    {
        qCritical() << "Failed to open the audio file" << audioFilePath;

        return false;
    }

    const QString projectName = QFileInfo(audioFilePath).baseName();

    if (QDir(projectName).exists())
    {
        qCritical() << "A project by the name" << projectName << "already exists in the current working directory.";

        return false;
    }

    if (!QDir().mkdir(projectName))
    {
        qCritical() << "Failed to create a directory for project:" << projectName;

        return false;
    }

    /// Convert the audio file to WAV.

    return true;
}

void MainWindow::save_current_project(void)
{
    if (!this->project) return;

    ui->textEditor->save_transcription(this->project->filenames.transcriptionFile);

    return;
}

void MainWindow::update_window_title(void)
{
    if (!this->project)
    {
        this->setWindowTitle("ranssi");

        return;
    }

    const QString title = this->project->name;

    const QString playbackTimestamp = QTime(0, 0).addMSecs(std::max(0, ui->wavPlayer->playback().pos_ms()))
                                                 .toString("hh:mm:ss");

    /// TODO. There's no way to query the playback's true tempo as of yet.
    const QString playbackTempo = "x1.00";

    this->setWindowTitle(title + " - "
                         + playbackTimestamp + " "
                         + playbackTempo + " - "
                         + "ranssi");

    return;
}
