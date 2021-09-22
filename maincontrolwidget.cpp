#include "maincontrolwidget.h"

MainControlWidget::MainControlWidget(QWidget *parent) : QWidget(parent)
{
    layout = new QVBoxLayout(this);
    m_signalErrWidget = new SignalErrWidget(this);
    m_signalErrWidget->setMaximumHeight(100);
    m_resultWidget = new ResultsWidget(this);


    layout->addWidget(m_signalErrWidget);
    layout->addWidget(m_resultWidget);
}
