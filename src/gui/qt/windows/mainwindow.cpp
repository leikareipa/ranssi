/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 * Ranssi is a minimalist and to-the-point transcription app.
 *
 */

#include <QPixmap>
#include <QDebug>
#include <QTimer>
#include "src/gui/qt/widgets/QWidget_text_editor_overlay.h"
#include "src/gui/qt/widgets/QTextEdit_text_editor.h"
#include "src/gui/qt/windows/mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setStyleSheet("#MainWindow { background-color: #1e1e1e; }");

    // Set up the text editor's overlay.
    /// TODO. Temprary. In the future, better integrate the overlay into the code
    /// and the parent widget's layout.
    {
        TextEditorOverlay *overlay = new TextEditorOverlay(this, ui->textEdit);

        ui->textEdit->stackUnder(overlay);

        connect(ui->textEdit, &TextEditor::cursorPositionChanged, this, [=]{ overlay->update(); });
        connect(ui->textEdit, &TextEditor::textChanged, this, [=]{ overlay->update(); });
        connect(ui->textEdit, &TextEditor::resized, this, [=]{ overlay->update(); });

        /// FIXME. Temporary hack. Without this, the text cursor position in the
        /// overlay is incorrect on startup.
        QTimer::singleShot(0, [=]{ overlay->update(); });
    }

    return;
}

MainWindow::~MainWindow()
{
    delete ui;

    return;
}
