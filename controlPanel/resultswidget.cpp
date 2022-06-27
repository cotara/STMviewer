#include "resultswidget.h"

ResultsWidget::ResultsWidget(QWidget *parent) : QGroupBox(parent)
{
    setObjectName("resultswidget");
    layout = new QVBoxLayout(this);
    setTitle("Результаты расчетов");
    //Результат
    diametrLabel = new QLabel("Диаметр(внутр): -");
    diametrLabel->setObjectName("BigLabel");
    diametrPlisLabel = new QLabel("Диаметр: -");
    diametrPlisLabel->setObjectName("BigLabel");
    diametrFinalLabel = new QLabel("Диаметр (финал): -");
    diametrFinalLabel->setObjectName("BigLabel");
    leftShadow1Label = new QLabel("   Фронт(внутр): -");
    rightShadow1Label = new QLabel("   Спад(внутр): -");
    leftShadow2Label = new QLabel("   Фронт(внутр): -");
    rightShadow2Label = new QLabel("   Спад(внутр): -");
    m_centerViewer = new centerViewer(this,20);
    centerPositionLabel = new QLabel("Смещение: -");
    centerPositionLabel->setObjectName("BigLabel");

    extr1Ch1 = new QLabel("   Экстр1: -");
    extr2Ch1 = new QLabel("   Экстр2: -");
    extr3Ch1 = new QLabel("   Экстр3: -");
    extr4Ch1 = new QLabel("   Экстр4: -");
    extr1Ch2 = new QLabel("   Экстр1: -");
    extr2Ch2 = new QLabel("   Экстр2: -");
    extr3Ch2 = new QLabel("   Экстр3: -");
    extr4Ch2 = new QLabel("   Экстр4: -");

    shad1Ch1 = new QLabel("   Фронт: -");
    shad2Ch1 = new QLabel("   Спад: -");
    shad1Ch2 = new QLabel("   Фронт: -");
    shad2Ch2 = new QLabel("   Спад: -");
    radiusX = new QLabel("Радиус Х (внутр): -");
    radiusY = new QLabel("Радиус Y (внутр): -");
    radiusPLISX = new QLabel("Радиус Х: -");
    radiusPLISY = new QLabel("Радиус Y: -");
    radiusFinalX = new QLabel("Радиус Х (финал): -");
    radiusFinalX->setObjectName("BigLabel");
    radiusFinalY = new QLabel("Радиус Y (финал): -");
    radiusFinalY->setObjectName("BigLabel");
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
    layout->addWidget(radiusX);
    layout->addWidget(radiusY);
    layout->addWidget(radiusPLISX);
    layout->addWidget(radiusPLISY);
    layout->addWidget(radiusFinalX);
    layout->addWidget(radiusFinalY);
    layout->addWidget(diametrLabel);
    layout->addWidget(diametrPlisLabel);
    layout->addWidget(diametrFinalLabel);
    layout->addWidget(m_centerViewer);
    m_centerViewer->setMinimumHeight(100);
    layout->addWidget(centerPositionLabel);


    extr1Ch1->hide();
    extr2Ch1->hide();
    extr3Ch1->hide();
    extr4Ch1->hide();
    extr1Ch2->hide();
    extr2Ch2->hide();
    extr3Ch2->hide();
    extr4Ch2->hide();
    shad1Ch1->hide();
    shad2Ch1->hide();
    shad1Ch2->hide();
    shad2Ch2->hide();
    radiusX->hide();
    radiusY->hide();
    radiusPLISX->hide();
    radiusPLISY->hide();
    ch1ShadowsLabel->hide();
    ch2ShadowsLabel->hide();
    diametrLabel->hide();
    diametrPlisLabel->hide();
    leftShadow1Label->hide();
    rightShadow1Label->hide();
    leftShadow2Label->hide();
    rightShadow2Label->hide();

}
