#include <QPixmap>
#include "src/gui/qt/subclasses/QWidget_text_field_overlay.h"
#include "src/gui/qt/subclasses/QTextEdit_text_field.h"
#include "src/gui/qt/windows/mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Set up the text editor's overlay.
    /// TODO. Temprary. In the future, better integrate the overlay into the code
    /// and the parent widget's layout.
    {
        TextFieldOverlay *overlay = new TextFieldOverlay(this);
        overlay->resize(this->size());

        {
            auto update_overlay = [=]
            {
                overlay->set_cursor_rect(ui->textEdit->cursorRect());
                overlay->set_background_pixmap(ui->textEdit->grab());
                overlay->update();
            };

            connect(ui->textEdit, &TextField::cursorPositionChanged, this,
                    [=]{ update_overlay(); });

            connect(ui->textEdit, &TextField::textChanged, this,
                    [=]{ update_overlay(); });

            connect(ui->textEdit, &TextField::selectionChanged, this,
                    [=]{ update_overlay(); });
        }
    }

    return;
}

MainWindow::~MainWindow()
{
    delete ui;

    return;
}
