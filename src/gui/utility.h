/*
 * 2018 Tarpeeksi Hyvae Soft
 *
 * Miscellaneous Qt-related utility functions.
 *
 */

#ifndef GUI_UTIL_H
#define GUI_UTIL_H

#include <QWidget>

// Has the given widget's signals blocked while in the scope of this object,
// after which the widget's previous blocking status is restored.
class inhibit_widget_signals_c
{
public:
    inhibit_widget_signals_c(QWidget *const widget) :
        widget(widget),
        previousBlockStatus(widget->blockSignals(true))
    {
        return;
    }

    ~inhibit_widget_signals_c(void)
    {
        widget->blockSignals(previousBlockStatus);
    }

private:
    QWidget *const widget;
    const bool previousBlockStatus;
};
#define inhibit_widget_signals(widget) inhibit_widget_signals_c block______(widget);

#endif
