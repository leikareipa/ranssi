/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 */

#ifndef WAV_DISPLAY_H
#define WAV_DISPLAY_H

#include <QWidget>

class WavDisplay : public QWidget
{
    Q_OBJECT

public:
    WavDisplay(QWidget *parent = 0);
    ~WavDisplay();

protected:
    void paintEvent(QPaintEvent *);
};

#endif
