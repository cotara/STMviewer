#include "plissettings.h"

PlisSettings::PlisSettings(QWidget *parent) : QGroupBox(parent)
{
    setTitle("Настройки ПЛИС");
    setObjectName("plissettings");

    lazer1 = new QLabel("ЛАЗЕР 1");
    lazer2 = new QLabel("ЛАЗЕР 2");
    lazer1->setObjectName("BigLabel");
    lazer2->setObjectName("BigLabel");

    lazer1averageNum1 = new QLabel("0");
    lazer1averageNum2 = new QLabel("0");
    lazer1averageNum3 = new QLabel("0");
    lazer1durationNum1 = new QLabel("0");
    lazer1durationNum2 = new QLabel("0");
    lazer1durationNum3 = new QLabel("0");
    lazer2averageNum1 = new QLabel("0");
    lazer2averageNum2 = new QLabel("0");
    lazer2averageNum3 = new QLabel("0");
    lazer2durationNum1 = new QLabel("0");
    lazer2durationNum2 = new QLabel("0");
    lazer2durationNum3 = new QLabel("0");

    lazer1averageNum1->setObjectName("BigBigLabel");
    lazer1averageNum2->setObjectName("BigBigLabel");
    lazer1averageNum3->setObjectName("BigBigLabel");
    lazer1durationNum1->setObjectName("BigBigLabel");
    lazer1durationNum2->setObjectName("BigBigLabel");
    lazer1durationNum3->setObjectName("BigBigLabel");
    lazer2averageNum1->setObjectName("BigBigLabel");
    lazer2averageNum2->setObjectName("BigBigLabel");
    lazer2averageNum3->setObjectName("BigBigLabel");
    lazer2durationNum1->setObjectName("BigBigLabel");
    lazer2durationNum2->setObjectName("BigBigLabel");
    lazer2durationNum3->setObjectName("BigBigLabel");

    layout = new QVBoxLayout(this);
    lazer1Layout = new QHBoxLayout();
    lazer2Layout = new QHBoxLayout();
    //Настройки лазера
    layout->addWidget(lazer1);

    layout->addLayout(lazer1Layout);
    layout->addWidget(lazer2);
    layout->addLayout(lazer2Layout);
    lazer1levelLayout = new QVBoxLayout();
    lazer1averageLayout = new QVBoxLayout();
    lazer1durationLayout = new QVBoxLayout();
    lazer2levelLayout = new QVBoxLayout();
    lazer2averageLayout = new QVBoxLayout();
    lazer2durationLayout = new QVBoxLayout();

    lazer1AverageValueLayout = new QHBoxLayout();
    lazer2AverageValueLayout = new QHBoxLayout();
    lazer1DurationValueLayout = new QHBoxLayout();
    lazer2DurationValueLayout = new QHBoxLayout();

    lazer1Layout->addLayout(lazer1levelLayout);
    lazer1Layout->addLayout(lazer1averageLayout);
    lazer1Layout->addLayout(lazer1durationLayout);
    lazer2Layout->addLayout(lazer2levelLayout);
    lazer2Layout->addLayout(lazer2averageLayout);
    lazer2Layout->addLayout(lazer2durationLayout);

    lazer1Button = new AsynchronButton(this,0,255);
    lazer2Button = new AsynchronButton(this,0,255);
    lazer1levelLabel = new QLabel("Уровень");
    lazer1averageLabel = new QLabel("Среднее");
    lazer1durationLabel = new QLabel("Длительность");
    lazer2levelLabel = new QLabel("Уровень");
    lazer2averageLabel = new QLabel("Среднее");
    lazer2durationLabel = new QLabel("Длительность");



    saveButton = new QPushButton("Сохранить");

    lazer1levelLayout->addWidget(lazer1levelLabel);
    lazer1levelLayout->addWidget(lazer1Button);
    lazer1averageLayout->addWidget(lazer1averageLabel);
    lazer1averageLayout->addLayout(lazer1AverageValueLayout);
    lazer1durationLayout->addWidget(lazer1durationLabel);
    lazer1durationLayout->addLayout(lazer1DurationValueLayout);


    lazer2levelLayout->addWidget(lazer2levelLabel);
    lazer2levelLayout->addWidget(lazer2Button);
    lazer2averageLayout->addWidget(lazer2averageLabel);
    lazer2averageLayout->addLayout(lazer2AverageValueLayout);
    lazer2durationLayout->addWidget(lazer2durationLabel);
    lazer2durationLayout->addLayout(lazer2DurationValueLayout);

    lazer1AverageValueLayout->addWidget(lazer1averageNum1);
    lazer1AverageValueLayout->addWidget(lazer1averageNum2);
    lazer1AverageValueLayout->addWidget(lazer1averageNum3);
    lazer1DurationValueLayout->addWidget(lazer1durationNum1);
    lazer1DurationValueLayout->addWidget(lazer1durationNum2);
    lazer1DurationValueLayout->addWidget(lazer1durationNum3);
    lazer2AverageValueLayout->addWidget(lazer2averageNum1);
    lazer2AverageValueLayout->addWidget(lazer2averageNum2);
    lazer2AverageValueLayout->addWidget(lazer2averageNum3);
    lazer2DurationValueLayout->addWidget(lazer2durationNum1);
    lazer2DurationValueLayout->addWidget(lazer2durationNum2);
    lazer2DurationValueLayout->addWidget(lazer2durationNum3);

    lazer1averageLabel->setAlignment(Qt::AlignCenter);
    lazer1averageNum1->setAlignment(Qt::AlignCenter);
    lazer1averageNum2->setAlignment(Qt::AlignCenter);
    lazer1averageNum3->setAlignment(Qt::AlignCenter);
    lazer1durationLabel->setAlignment(Qt::AlignCenter);
    lazer1durationNum1->setAlignment(Qt::AlignCenter);
    lazer1durationNum2->setAlignment(Qt::AlignCenter);
    lazer1durationNum3->setAlignment(Qt::AlignCenter);
    lazer2averageLabel->setAlignment(Qt::AlignCenter);
    lazer2averageNum1->setAlignment(Qt::AlignCenter);
    lazer2averageNum2->setAlignment(Qt::AlignCenter);
    lazer2averageNum3->setAlignment(Qt::AlignCenter);
    lazer2durationLabel->setAlignment(Qt::AlignCenter);
    lazer2durationNum1->setAlignment(Qt::AlignCenter);
    lazer2durationNum2->setAlignment(Qt::AlignCenter);
    lazer2durationNum3->setAlignment(Qt::AlignCenter);

    //lazerLayout->setAlignment(saveButton,Qt::AlignBottom);

    //lazer1Button->setEnabled(false);
    //lazer2Button->setEnabled(false);
    //saveButton->setEnabled(false);

    connect(lazer1Button, &AsynchronButton::sendValue,
          [=](int i){emit lazer1Send(i);});
    connect(lazer2Button, &AsynchronButton::sendValue,
          [=](int i){emit lazer2Send(i);});


    lazer1Button->setText(QString::number(0));
    lazer2Button->setText(QString::number(0));

    //Настройка границ
    borderLayout = new QHBoxLayout();
    layout->addLayout(borderLayout);

    borderLeftLayout = new QVBoxLayout();
    borderRightLayout = new QVBoxLayout();
    compCH1Layout = new QVBoxLayout();
    compCH2Layout = new QVBoxLayout();
    borderLayout->addLayout(borderLeftLayout);
    borderLayout->addLayout(borderRightLayout);
    borderLayout->addLayout(compCH1Layout);
    borderLayout->addLayout(compCH2Layout);

    borderLeftButton = new AsynchronButton(this,0,255);
    borderRightButton = new AsynchronButton(this,0,255);
    compCH1Button = new AsynchronButton(this,0,255);
    compCH2Button = new AsynchronButton(this,0,255);

    borderLeftButton->setText(QString::number(0));
    borderRightButton->setText(QString::number(0));
    compCH1Button->setText(QString::number(0));
    compCH2Button->setText(QString::number(0));


    borderLeftLabel = new QLabel("Л.Гр.");
    borderRightLabel = new QLabel("П.Гр.");
    compCH1Label = new QLabel("Ур.К1");
    compCH2Label = new QLabel("Ур.К2");
    borderLeftLabel->setAlignment(Qt::AlignCenter);
    borderRightLabel->setAlignment(Qt::AlignCenter);
    compCH1Label->setAlignment(Qt::AlignCenter);
    compCH2Label->setAlignment(Qt::AlignCenter);

    borderLeftLayout->addWidget(borderLeftLabel);
    borderLeftLayout->addWidget(borderLeftButton);
    borderRightLayout->addWidget(borderRightLabel);
    borderRightLayout->addWidget(borderRightButton);
    compCH1Layout->addWidget(compCH1Label);
    compCH1Layout->addWidget(compCH1Button);
    compCH2Layout->addWidget(compCH2Label);
    compCH2Layout->addWidget(compCH2Button);

    connect(borderLeftButton, &AsynchronButton::sendValue,
          [=](int i){emit sendBorderLeft(i); });
    connect(borderRightButton, &AsynchronButton::sendValue,
          [=](int i){emit sendBorderRight(i); });
    connect(compCH1Button, &AsynchronButton::sendValue,
          [=](int i){emit sendCompCH1(i); });
    connect(compCH2Button, &AsynchronButton::sendValue,
          [=](int i){emit sendCompCH2(i); });

    //Смещения green и blue каналов
    offsetLayout =  new QHBoxLayout();
    layout->addLayout(offsetLayout);
    offsetGreenLayout= new QVBoxLayout();
    offsetBlueLayout= new QVBoxLayout();
    offsetLayout->addLayout(offsetGreenLayout);
    offsetLayout->addLayout(offsetBlueLayout);

    offsetGreenLabel = new QLabel("Offset Green");
    offsetGreenLabel->setAlignment(Qt::AlignCenter);
    offsetBlueLabel = new QLabel("Offset Blue");
    offsetBlueLabel->setAlignment(Qt::AlignCenter);

    offsetGreenButton = new AsynchronButton(this,0,255);
    offsetGreenButton->setText(QString::number(0));
    offsetBlueButton = new AsynchronButton(this,0,255);
    offsetBlueButton->setText(QString::number(0));

    offsetGreenLayout->addWidget(offsetGreenLabel);
    offsetGreenLayout->addWidget(offsetGreenButton);
    offsetBlueLayout->addWidget(offsetBlueLabel);
    offsetBlueLayout->addWidget(offsetBlueButton);

    connect(offsetGreenButton, &AsynchronButton::sendValue,
          [=](int i){emit sendGreenOffset(i);});
    connect(offsetBlueButton, &AsynchronButton::sendValue,
          [=](int i){emit sendBlueOffset(i);});

    offsetGreenButton->setEnabled(false);
    offsetBlueButton->setEnabled(false);

    multyLaserMode = new QHBoxLayout();
    autoModeRadio = new QRadioButton("Авто", this);
    centerModeRadio = new QRadioButton("Центральный", this);
    rearModeRadio = new QRadioButton("Боковые", this);
    autoModeRadio->setObjectName("BigRadio");
    centerModeRadio->setObjectName("BigRadio");
    rearModeRadio->setObjectName("BigRadio");

    layout->addLayout(multyLaserMode);

    multyLaserMode->addWidget(centerModeRadio);
    multyLaserMode->addWidget(rearModeRadio);
    multyLaserMode->addWidget(autoModeRadio);

    connect(autoModeRadio,&QRadioButton::clicked,[=](){emit sendMultyLaserMode(2);});
    connect(centerModeRadio,&QRadioButton::clicked,[=](){emit sendMultyLaserMode(0);});
    connect(rearModeRadio,&QRadioButton::clicked,[=](){emit sendMultyLaserMode(1);});

    layout->addWidget(saveButton);

    connect(saveButton,&QPushButton::clicked,[=]{
        emit saveSend();
        QFile file(":/qss/styleWhiteButtons.css");
          if(file.open(QFile::ReadOnly)){
              QByteArray style = file.readAll();
              borderLeftButton->setStyleSheet(style);
              borderRightButton->setStyleSheet(style);
              compCH1Button->setStyleSheet(style);
              compCH2Button->setStyleSheet(style);
              lazer1Button->setStyleSheet(style);
              lazer2Button->setStyleSheet(style);
              offsetGreenButton->setStyleSheet(style);
              offsetBlueButton->setStyleSheet(style);
        }});
    saveButton->setObjectName("saveButton");
}
