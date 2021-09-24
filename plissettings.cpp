#include "plissettings.h"

PlisSettings::PlisSettings(QWidget *parent) : QGroupBox(parent)
{
    setTitle("Настройки ПЛИС");
    layout = new QVBoxLayout(this);
    lazerLayout = new QHBoxLayout();
    //Настройки лазера
    layout->addLayout(lazerLayout);
    lazer1SettingLayout = new QVBoxLayout();
    lazer2SettingLayout = new QVBoxLayout();

    lazerLayout->addLayout(lazer1SettingLayout);
    lazerLayout->addLayout(lazer2SettingLayout);
    lazer1Spinbox = new QSpinBox();
    lazer2Spinbox = new QSpinBox();
    lazer1Label = new QLabel("Лазер 1:");
    lazer2Label = new QLabel("Лазер 2:");
    saveButton = new QPushButton("Сохранить в EEPROM");

    lazer1Spinbox->setRange(10,50);
    lazer2Spinbox->setRange(10,50);
    lazer1Spinbox->setMaximumWidth(50);
    lazer2Spinbox->setMaximumWidth(50);
    lazer1Spinbox->setValue(40);
    lazer2Spinbox->setValue(40);
    lazer1SettingLayout->addWidget(lazer1Label);
    lazer1SettingLayout->addWidget(lazer1Spinbox);
    lazer2SettingLayout->addWidget(lazer2Label);
    lazer2SettingLayout->addWidget(lazer2Spinbox);
    lazerLayout->addWidget(saveButton);

    lazer1Spinbox->setEnabled(false);
    lazer2Spinbox->setEnabled(false);
    saveButton->setEnabled(false);

    connect(lazer1Spinbox, QOverload<int>::of(&QSpinBox::valueChanged),
          [=](int i){emit lazer1Send(i); });
    connect(lazer2Spinbox, QOverload<int>::of(&QSpinBox::valueChanged),
          [=](int i){emit lazer2Send(i);});

    connect(saveButton,&QPushButton::clicked,this,&PlisSettings::saveSend);

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

    borderLeftButton = new QPushButton();
    borderRightButton = new QPushButton();
    compCH1Button = new QPushButton();
    compCH2Button = new QPushButton();
    borderLeftButton->setText(QString::number(0));
    borderRightButton->setText(QString::number(0));
    compCH1Button->setText(QString::number(0));
    compCH2Button->setText(QString::number(0));

    borderLeftWidget = new EnterValueWidget(this);
    borderRightWidget = new EnterValueWidget(this);
    compCH1Widget = new EnterValueWidget(this);
    compCH2Widget = new EnterValueWidget(this);

    borderLeftLabel = new QLabel("Лев. гр.:");
    borderRightLabel = new QLabel("Прав. гр.:");
    compCH1Label = new QLabel("Комп. кан. 1:");
    compCH2Label = new QLabel("Комп. кан. 2:");

    borderLeftLayout->addWidget(borderLeftLabel);
    borderLeftLayout->addWidget(borderLeftButton);
    borderRightLayout->addWidget(borderRightLabel);
    borderRightLayout->addWidget(borderRightButton);
    compCH1Layout->addWidget(compCH1Label);
    compCH1Layout->addWidget(compCH1Button);
    compCH2Layout->addWidget(compCH2Label);
    compCH2Layout->addWidget(compCH2Button);

    connect(borderLeftWidget,&EnterValueWidget::sendValue, [=](int i)
        {borderLeftButton->setText(QString::number(i)); emit sendBorderLeft(i);});
    connect(borderRightWidget,&EnterValueWidget::sendValue, [=](int i)
        {borderRightButton->setText(QString::number(i)); emit sendBorderRight(i);});
    connect(compCH1Widget,&EnterValueWidget::sendValue, [=](int i)
        {compCH1Button->setText(QString::number(i)); emit sendCompCH1(i);});
    connect(compCH2Widget,&EnterValueWidget::sendValue, [=](int i)
        {compCH2Button->setText(QString::number(i)); emit sendCompCH2(i);});

    connect(borderLeftButton,&QPushButton::clicked, [=]{borderLeftWidget->show();});
    connect(borderRightButton,&QPushButton::clicked, [=]{borderRightWidget->show();});
    connect(compCH1Button,&QPushButton::clicked, [=]{compCH1Widget->show();});
    connect(compCH2Button,&QPushButton::clicked, [=]{compCH2Widget->show();});

    lazer1Spinbox->setEnabled(false);
    lazer2Spinbox->setEnabled(false);
    saveButton->setEnabled(false);
}
