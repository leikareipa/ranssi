/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 * Ranssi is a minimalist and to-the-point transcription app.
 *
 */

#include <QPixmap>
#include <QDebug>
#include "gui/widgets/QWidget_text_editor_overlay.h"
#include "gui/widgets/QTextEdit_text_editor.h"
#include "gui/windows/mainwindow.h"
#include "project/project.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(const project_c &project) :
    QMainWindow(nullptr),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->wavDisplay->copy_wav_data(project.wav);

    this->setStyleSheet("#MainWindow { background-color: #1e1e1e; }");

    // Set up the text editor's overlay.
    {
        TextEditorOverlay *overlay = new TextEditorOverlay(this, ui->textEdit);

        ui->textEdit->stackUnder(overlay);

        connect(ui->textEdit, &TextEditor::cursorPositionChanged, this, [=]{ overlay->update(); });
        connect(ui->textEdit, &TextEditor::textChanged, this, [=]{ overlay->update(); });
        connect(ui->textEdit, &TextEditor::resized, this, [=]{ overlay->update(); });
    }

    return;
}

MainWindow::~MainWindow(void)
{
    delete ui;

    return;
}
