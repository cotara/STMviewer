#include "plissettings.h"

PlisSettings::PlisSettings(QWidget *parent) : QGroupBox(parent)
{
    setTitle("Настройки ПЛИС");
    setObjectName("plissettings");
    layout = new QVBoxLayout(this);
    lazerLayout = new QHBoxLayout();
    //Настройки лазера
    layout->addLayout(lazerLayout);
    lazer1SettingLayout = new QVBoxLayout();
    lazer2SettingLayout = new QVBoxLayout();

    lazerLayout->addLayout(lazer1SettingLayout);
    lazerLayout->addLayout(lazer2SettingLayout);
    lazer1Button = new AsynchronButton(this,10,50);
    lazer2Button = new AsynchronButton(this,10,50);
    lazer1Label = new QLabel("Лазер 1:");
    lazer2Label = new QLabel("Лазер 2:");
    saveButton = new QPushButton("Сохранить");

    emit lazer1Send(40);
    emit lazer2Send(40);

    lazer1SettingLayout->addWidget(lazer1Label);
    lazer1SettingLayout->addWidget(lazer1Button);
    lazer2SettingLayout->addWidget(lazer2Label);
    lazer2SettingLayout->addWidget(lazer2Button);
    lazerLayout->addWidget(saveButton);

    //lazer1Button->setEnabled(false);
    //lazer2Button->setEnabled(false);
    //saveButton->setEnabled(false);

    connect(lazer1Button, &AsynchronButton::sendValue,
          [=](int i){emit lazer1Send(i); });
    connect(lazer2Button, &AsynchronButton::sendValue,
          [=](int i){emit lazer2Send(i); });



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


    borderLeftLabel = new QLabel("Л.Гр.:");
    borderRightLabel = new QLabel("П.Гр.:");
    compCH1Label = new QLabel("Ур.К1:");
    compCH2Label = new QLabel("Ур.К2:");
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
        }});
    saveButton->setObjectName("saveButton");

    lazer1Button->setEnabled(false);
    lazer2Button->setEnabled(false);
    borderLeftButton->setEnabled(false);
    borderRightButton->setEnabled(false);
    compCH1Button->setEnabled(false);
    compCH2Button->setEnabled(false);
}
