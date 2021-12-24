#include "resultswidget.h"

ResultsWidget::ResultsWidget(QWidget *parent) : QGroupBox(parent)
{
    setObjectName("resultswidget");
    layout = new QVBoxLayout(this);
    setTitle("Результаты расчетов");
    //Результат
    diametrLabel = new QLabel("Диаметр: ");
    diametrLabel->setObjectName("BigLabel");
    diametrPlisLabel = new QLabel("Диаметр ПЛИС: -");
    diametrPlisLabel->setObjectName("BigLabel");
    leftShadow1Label = new QLabel("   Лев.тень: -");
    rightShadow1Label = new QLabel("   Прав.тень: -");
    leftShadow2Label = new QLabel("   Лев.тень: -");
    rightShadow2Label = new QLabel("   Прав.тень: -");
    m_centerViewer = new centerViewer();
    centerPositionLabel = new QLabel("Смещение: -");

    extr1Ch1 = new QLabel("   Экстр1: -");
    extr2Ch1 = new QLabel("   Экстр2: -");
    extr3Ch1 = new QLabel("   Экстр3: -");
    extr4Ch1 = new QLabel("   Экстр4: -");
    extr1Ch2 = new QLabel("   Экстр1: -");
    extr2Ch2 = new QLabel("   Экстр2: -");
    extr3Ch2 = new QLabel("   Экстр3: -");
    extr4Ch2 = new QLabel("   Экстр4: -");

    shad1Ch1 = new QLabel("   Тень1: -");
    shad2Ch1 = new QLabel("   Тень2: -");
    shad1Ch2 = new QLabel("   Тень1: -");
    shad2Ch2 = new QLabel("   Тень2: -");
    radius1 = new QLabel("   Радиус1: -");
    radius2 = new QLabel("   Радиус2: -");
    ch1ShadowsLabel = new QLabel("Канал 1:");
    ch2ShadowsLabel = new QLabel("Канал 2:");

    ch1ShadowsLabel->setAlignment(Qt::AlignCenter);
    ch2ShadowsLabel->setAlignment(Qt::AlignCenter);
    ch1ShadowsLabel->setObjectName("BigLabel");
    ch2ShadowsLabel->setObjectName("BigLabel");

    layout->addWidget(ch1ShadowsLabel);
    layout->addWidget(leftShadow1Label);
    layout->addWidget(rightShadow1Label);
    layout->addWidget(extr1Ch1);
    layout->addWidget(extr2Ch1);
    layout->addWidget(extr3Ch1);
    layout->addWidget(extr4Ch1);
    layout->addWidget(shad1Ch1);
    layout->addWidget(shad2Ch1);

    layout->addWidget(ch2ShadowsLabel);
    layout->addWidget(leftShadow2Label);
    layout->addWidget(rightShadow2Label);
    layout->addWidget(extr1Ch2);
    layout->addWidget(extr2Ch2);
    layout->addWidget(extr3Ch2);
    layout->addWidget(extr4Ch2);
    layout->addWidget(shad1Ch2);
    layout->addWidget(shad2Ch2);

    QFrame *line = new QFrame(this);
    line->setObjectName("line");
    line->setFrameShape(QFrame::HLine);
    line->setLineWidth(1);
    layout->addWidget(line);
    layout->addWidget(radius1);
    layout->addWidget(radius2);
    layout->addWidget(diametrLabel);
    layout->addWidget(diametrPlisLabel);
    layout->addWidget(m_centerViewer);
    m_centerViewer->setMinimumHeight(100);
    layout->addWidget(centerPositionLabel);
}
