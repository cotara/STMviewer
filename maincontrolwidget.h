#ifndef MAINCONTROLWIDGET_H
#define MAINCONTROLWIDGET_H

#include <QWidget>
#include "signalerrwidget.h"
#include "resultswidget.h"
class MainControlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainControlWidget(QWidget *parent = nullptr);
    SignalErrWidget *m_signalErrWidget;
    ResultsWidget *m_resultWidget;
signals:
private:

    QVBoxLayout *layout;


};

#endif // MAINCONTROLWIDGET_H
