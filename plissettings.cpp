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

    borderLeftSpinbox = new QSpinBox();
    borderRightSpinbox = new QSpinBox();
    compCH1Spinbox = new QSpinBox();
    compCH2Spinbox = new QSpinBox();

    borderLeftLabel = new QLabel("Лев. гр.:");
    borderRightLabel = new QLabel("Прав. гр.:");
    compCH1Label = new QLabel("Комп. кан. 1:");
    compCH2Label = new QLabel("Комп. кан. 2:");

    borderLeftSpinbox->setRange(0,255);
    borderRightSpinbox->setRange(0,255);
    compCH1Spinbox->setRange(0,255);
    compCH2Spinbox->setRange(0,255);

    borderLeftLayout->addWidget(borderLeftLabel);
    borderLeftLayout->addWidget(borderLeftSpinbox);
    borderRightLayout->addWidget(borderRightLabel);
    borderRightLayout->addWidget(borderRightSpinbox);
    compCH1Layout->addWidget(compCH1Label);
    compCH1Layout->addWidget(compCH1Spinbox);
    compCH2Layout->addWidget(compCH2Label);
    compCH2Layout->addWidget(compCH2Spinbox);

    borderLeftSpinbox->setEnabled(false);
    borderRightSpinbox->setEnabled(false);
    compCH1Spinbox->setEnabled(false);
    compCH2Spinbox->setEnabled(false);

    connect(borderLeftSpinbox, QOverload<int>::of(&QSpinBox::valueChanged),
          [=](int i){emit sendBorderLeft(i);});
    connect(borderRightSpinbox, QOverload<int>::of(&QSpinBox::valueChanged),
          [=](int i){emit sendBorderRight(i);});
    connect(compCH1Spinbox, QOverload<int>::of(&QSpinBox::valueChanged),
          [=](int i){emit sendCompCH1(i);});
    connect(compCH2Spinbox, QOverload<int>::of(&QSpinBox::valueChanged),
          [=](int i){emit sendCompCH2(i);});

    lazer1Spinbox->setEnabled(false);
    lazer2Spinbox->setEnabled(false);
    saveButton->setEnabled(false);
    borderLeftSpinbox->setEnabled(false);
    borderRightSpinbox->setEnabled(false);
    compCH1Spinbox->setEnabled(false);
    compCH2Spinbox->setEnabled(false);

}
