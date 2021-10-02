#include "managementwidget.h"

ManagementWidget::ManagementWidget(QWidget *parent) : QWidget(parent)
{
    setObjectName("managementwidget");
    layout = new QVBoxLayout(this);
    m_plisSettings = new PlisSettings(this);
    m_TransmitionSettings = new TransmitionSettings(this);
    m_HistorySettings = new HistorySettings(this);

    layout->addWidget(m_plisSettings);
    layout->addWidget(m_TransmitionSettings);
    layout->addWidget(m_HistorySettings);

    layout->addStretch(0);

    //setStyleSheet("background-color: #120A52;");
}
