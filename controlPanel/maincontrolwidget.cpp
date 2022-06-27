#include "maincontrolwidget.h"

MainControlWidget::MainControlWidget(QWidget *parent) : QWidget(parent)
{
    setObjectName("maincontrolwidget");
    layout = new QVBoxLayout(this);
    m_signalErrWidget = new SignalErrWidget(this);
    m_signalErrWidget->setMaximumHeight(100);
    m_resultWidget = new ResultsWidget(this);


    layout->addWidget(m_signalErrWidget);
    layout->addWidget(m_resultWidget);
    layout->addStretch(2);
}
