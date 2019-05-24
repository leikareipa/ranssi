/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 * The tarpaulin is a widget that covers ranssi's entire main window while no
 * project is loaded in for editing. It functions to hide the normal GUI editing
 * controls while they're not needed, and to accept file drag/drop.
 *
 */

#ifndef TARPAULIN_H
#define TARPAULIN_H

#include <QWidget>

class Tarpaulin : public QWidget
{
    Q_OBJECT

public:
    Tarpaulin(QWidget *parent = 0);
    ~Tarpaulin();

    void pull_back(void);

    void put_on(void);

signals:
    // Emitted when the tarp has been put on.
    void on(void);

    // Emitted when the tarp has been pulled back.
    void off(void);

    // Emitted when the user has dragged a ranssi project directory onto the tarp.
    void open_project(const QString &directoryName);

    // Emitted when the user has dragged an audio file onto the tarp.
    void create_project(const QString &audioFilePath);

protected:
    void paintEvent(QPaintEvent *);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

private:
};

#endif
