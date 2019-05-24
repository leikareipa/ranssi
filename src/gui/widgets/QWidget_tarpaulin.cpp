/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 * The tarpaulin is a widget that covers ranssi's entire main window while no
 * project is loaded in for editing. It functions to hide the normal GUI editing
 * controls while they're not needed, and to accept file drag/drop.
 *
 * The tarp includes event watchers for drag/drop events, so that the user can
 * drag projects onto the window to open them. When a project is opened, the tarp
 * is pulled back to reveal the GUI editing controls.
 *
 */

#include <QDragEnterEvent>
#include <QMimeDatabase>
#include <QFileInfo>
#include <QMimeData>
#include <QPainter>
#include <QDebug>
#include "gui/widgets/QWidget_tarpaulin.h"
#include "common.h"

Tarpaulin::Tarpaulin(QWidget *parent) :
    QWidget(parent)
{
    this->setAcceptDrops(true);

    // Set up the font.
    {
        QFont f = parent->font();
        f.setPointSize(11);

        this->setFont(f);
    }

    return;
}

Tarpaulin::~Tarpaulin()
{
    return;
}

void Tarpaulin::pull_back(void)
{
    this->setEnabled(false);
    this->setVisible(false);

    emit off();

    return;
}

void Tarpaulin::put_on(void)
{
    this->setEnabled(true);
    this->setVisible(true);

    emit on();

    return;
}

void Tarpaulin::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    /// Temporary styling.
    painter.fillRect(this->rect(), QColor("#404040"));
    painter.setPen(QColor("#afafaf"));
    painter.drawText(this->rect(), Qt::AlignCenter, "Drag...\n\n\n"
                                                    "A folder\nto open a project\n\n"
                                                    "An audio file\nto create a new project\n\n");

    return;
}

void Tarpaulin::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) event->accept();

    return;
}

// If the user dropped an audio file on the tarp, assume it's a sound file with
// which the user wants to initialize a new project. Otherwise, if the user
// dropped a directory, assume it's a ranssi project that the user wants to open.
// If multiple files are dropped in at once, all but the first are ignored.
void Tarpaulin::dropEvent(QDropEvent *event)
{
    k_assert(event->mimeData()->hasUrls(), "Expected all accepted drops to contain URLs.");

    const QString firstFile = event->mimeData()->urls().at(0).toLocalFile();

    if (QFileInfo(firstFile).isDir())
    {
        emit open_project(firstFile);
    }
    else
    {
        const auto mimeType = QMimeDatabase().mimeTypeForFile(firstFile);

        if (mimeType.isValid() && mimeType.name().startsWith("audio/"))
        {
            emit create_project(firstFile);
        }
    }

    return;
}
