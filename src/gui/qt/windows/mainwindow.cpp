#include "src/gui/qt/subclasses/QWidget_text_field_overlay.h"
#include "src/gui/qt/subclasses/QTextEdit_text_field.h"
#include "src/gui/qt/windows/mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /// TODO. Have proper layouting for the overlay, etc.
    TextFieldOverlay *overlay = new TextFieldOverlay(this);
    overlay->resize(this->size());

    connect(ui->textEdit, &QTextEdit::cursorPositionChanged, this,
            [=]{ overlay->set_cursor_rect(ui->textEdit->cursorRect()); overlay->repaint(); });

    return;
}

MainWindow::~MainWindow()
{
    delete ui;

    return;
}
