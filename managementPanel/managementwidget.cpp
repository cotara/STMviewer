#include "managementwidget.h"

ManagementWidget::ManagementWidget(QWidget *parent) : QWidget(parent)
{
    setObjectName("managementwidget");
    layout = new QVBoxLayout(this);
    m_plisSettings = new PlisSettings(this);
    m_TransmitionSettings = new TransmitionSettings(this);
    m_HistorySettings = new HistorySettings(this);
    m_DiameterTransmition = new DiameterTransmition(this);

    layout->addWidget(m_plisSettings);
    layout->addWidget(m_TransmitionSettings);
    layout->addWidget(m_HistorySettings);
    layout->addWidget(m_DiameterTransmition);

    layout->addStretch(2);

    m_HistorySettings->setVisible(false);
    m_DiameterTransmition->setVisible(false);
    //setStyleSheet("background-color: #120A52;");
}
