#include "resultswidget.h"

ResultsWidget::ResultsWidget(QWidget *parent) : CollapsibleGroupBox(parent)
{
    setObjectName("resultswidget");
    hLayout = new QHBoxLayout(this);
    layout = new QVBoxLayout();
    errLayout= new QVBoxLayout();

    setTitle("Результаты расчетов");
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    //Результат
    diametrLabel = new QLabel("Диаметр(внутр): -");
    diametrLabel->setObjectName("BigLabel");
    diametrPlisLabel = new QLabel("Диаметр: -");
    diametrPlisLabel->setObjectName("BigLabel");
    diametrFinalLabel = new QLabel("Диаметр: -");
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
    radiusFinalX = new QLabel("Диаметр Х: -");
    radiusFinalX->setObjectName("BigLabel");
    radiusFinalY = new QLabel("Диаметр Y: -");
    radiusFinalY->setObjectName("BigLabel");
    ch1ShadowsLabel = new QLabel("Канал 1:");
    ch2ShadowsLabel = new QLabel("Канал 2:");

    errorMarker1 = new ErrorMarker(this, "Канал 1");
    errorMarker2 = new ErrorMarker(this, "Канал 2");
    errLayout->addWidget(errorMarker1);
    errLayout->addWidget(errorMarker2);

    ch1ShadowsLabel->setAlignment(Qt::AlignCenter);
    ch2ShadowsLabel->setAlignment(Qt::AlignCenter);
    ch1ShadowsLabel->setObjectName("BigLabel");
    ch2ShadowsLabel->setObjectName("BigLabel");

    hLayout->addWidget(m_centerViewer);
    hLayout->addLayout(layout);
    hLayout->addLayout(errLayout);

    hLayout->setStretchFactor(layout,1);
    hLayout->setStretchFactor(m_centerViewer,1);
    hLayout->setStretchFactor(errLayout,1);
    m_centerViewer->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    m_centerViewer->setMaximumHeight(1000);


    //layout->addSpacing(200);
    //layout->addSpacerItem(new QSpacerItem(0,10, QSizePolicy::Expanding, QSizePolicy::Expanding));
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
    line->hide();
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

void ResultsWidget::setData(const QVector<double> &data)
{
    extr1Ch1->setText("   Экстр1: " + QString::number(data.at(0)));
    extr2Ch1->setText("   Экстр2: " + QString::number(data.at(1)));
    extr3Ch1->setText("   Экстр3: " + QString::number(data.at(2)));
    extr4Ch1->setText("   Экстр4: " + QString::number(data.at(3)));
    extr1Ch2->setText("   Экстр1: " + QString::number(data.at(4)));
    extr2Ch2->setText("   Экстр2: " + QString::number(data.at(5)));
    extr3Ch2->setText("   Экстр3: " + QString::number(data.at(6)));
    extr4Ch2->setText("   Экстр4: " + QString::number(data.at(7)));

    shad1Ch1->setText("   Фронт: " + QString::number(data.at(8)));
    shad2Ch1->setText("   Спад: " + QString::number(data.at(9)));
    shad1Ch2->setText("   Фронт: " + QString::number(data.at(10)));
    shad2Ch2->setText("   Спад: " + QString::number(data.at(11)));

    if(data.at(12) > 0 && data.at(13) > 0){
        diametrPlisLabel->setText("Диаметр: " +QString::number(data.at(12) + data.at(13)));
        radiusPLISX->setText("   Радиус X: " + QString::number(data.at(12)));
        radiusPLISY->setText("   Радиус Y: " + QString::number(data.at(13)));
    }
    else{
        diametrPlisLabel->setText("Диаметр:-");
        radiusPLISX->setText("Радиус X:-");
        radiusPLISY->setText("Радиус Y:-");
    }

    if(data.at(12) != 0)
        radiusFinalX->setText("Диаметр Х:  " +QString::number(data.at(14)/1000,'f',3) + "мм");
    else
        radiusFinalX->setText("Диаметр Х: - мм");

    if(data.at(13) != 0)
        radiusFinalY->setText("Диаметр Y:  " +QString::number(data.at(15)/1000,'f',3) + "мм");
    else
        radiusFinalY->setText("Диаметр Y: - мм");

    if(data.at(14) != 0 && data.at(15) != 0){
        diametrFinalLabel->setText("Диаметр:  " +QString::number((data.at(14)/1000 + data.at(15)/1000),'f',3) + "мм");
        centerPositionLabel->setText("Смещение: " + QString::number(data.at(16)/1000,'f',2) + ", " + QString::number(data.at(17)/1000,'f',2) + "мм");
        m_centerViewer->setCoord(data.at(16)/1000/1000,data.at(17)/1000/1000);
        m_centerViewer->setRad(data.at(14)/1000/1000,data.at(15)/1000/1000);
    }
    else{
        diametrFinalLabel->setText("Диаметр: - мм");
        centerPositionLabel->setText("Смещение:-");
        m_centerViewer->setCoord(0,0);
        m_centerViewer->setRad(0,0);
    }
}

void ResultsWidget::setModel(int model){
    m_centerViewer->setScale(model);
}

void ResultsWidget::setError(const QByteArray &bytes){
    errorMarker1->setError(bytes.at(0));
    errorMarker2->setError(bytes.at(2));
}
