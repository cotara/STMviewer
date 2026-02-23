#include "managementwidget.h"

ManagementWidget::ManagementWidget(QWidget *parent) : QWidget(parent)
{
    setObjectName("managementwidget");

    m_plisSettings = new PlisSettings(this);
    m_plisSettings->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    m_TransmitionSettings = new TransmitionSettings(this);
    m_TransmitionSettings->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    m_HistorySettings = new HistorySettings(this);
    //m_HistorySettings->setMaximumHeight(200);
    m_HistorySettings->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    //m_HistorySettings->toggleCollapse();

    m_DiameterTransmition = new DiameterTransmition(this);
    m_DiameterTransmition->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    m_resultWidget = new ResultsWidget(this);
    m_resultWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    // 3. Создаем контейнер для groupbox'ов
    сontent = new QWidget(this);
    сontentLayout = new QVBoxLayout(сontent);
    сontentLayout->setSpacing(5); // Межгрупповой отступ
    сontentLayout->setContentsMargins(2, 2, 2, 2); // Минимальные отступы
    сontentLayout->addWidget(m_plisSettings);
    сontentLayout->addWidget(m_TransmitionSettings);
    сontentLayout->addWidget(m_DiameterTransmition);
    сontentLayout->addWidget(m_resultWidget);
    сontentLayout->addWidget(m_HistorySettings);
    сontentLayout->addStretch(); // Растягивающийся элемент в конце

    //1.Область Скрола
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setFrameShape(QFrame::NoFrame); // Убираем рамку
    scrollArea->setWidget(сontent);             // Устанавливаем контент в scroll area

    //2. Главый вертикальный лайоут
    layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0); // Убираем отступы
    layout->addWidget(scrollArea);

    m_DiameterTransmition->setVisible(false);
    //setStyleSheet("background-color: #120A52;");

}
