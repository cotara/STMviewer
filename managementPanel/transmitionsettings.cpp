#include "transmitionsettings.h"

TransmitionSettings::TransmitionSettings(QWidget *parent) : QGroupBox(parent)
{
    setTitle("Параметры передачи");
    setObjectName("transmitionsettings");
    layout = new QVBoxLayout(this);

    //Настройки передачи
    packetSizeLabel = new QLabel("Размер пакета:",this);
    packetSizeSpinbox = new QSpinBox(this);
    ch1CheckBox = new QCheckBox("Канал 1. Нефильтрованный");
    ch2CheckBox = new QCheckBox("Канал 1. Фильтрованный");
    ch2InCheckBox = new QCheckBox("Канал 1. Фильтрованный*");
    ch3CheckBox = new QCheckBox("Канал 2. Нефильтрованный");
    ch4CheckBox = new QCheckBox("Канал 2. Фильтрованный");
    ch4InCheckBox = new QCheckBox("Канал 2. Фильтрованный*");


    getButton = new QPushButton("Получать сигнал");
    getButton->setCheckable(true);

    layout->addWidget(packetSizeLabel);
    layout->addWidget(packetSizeSpinbox);
    layout->addWidget(ch1CheckBox);
    layout->addWidget(ch2CheckBox);
    layout->addWidget(ch2InCheckBox);
    layout->addWidget(ch3CheckBox);
    layout->addWidget(ch4CheckBox);
    layout->addWidget(ch4InCheckBox);
    ch1CheckBox->setEnabled(false);
    ch2CheckBox->setEnabled(false);
    ch2InCheckBox->setEnabled(false);
    ch3CheckBox->setEnabled(false);
    ch4CheckBox->setEnabled(false);
    ch4InCheckBox->setEnabled(false);
    layout->addWidget(getButton);

    packetSizeSpinbox->setRange(50,15000);
    packetSizeSpinbox->setValue(11000);


    connect(packetSizeSpinbox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int i){ emit setPacketSize(i);});


    signalMapper = new QSignalMapper(this);
    connect(signalMapper, QOverload<int>::of(&QSignalMapper::mapped), [=](int i){ chChooseChanged(i); });
         signalMapper->setMapping(ch1CheckBox, 1);
         signalMapper->setMapping(ch2CheckBox, 2);
         signalMapper->setMapping(ch3CheckBox, 3);
         signalMapper->setMapping(ch4CheckBox, 4);

         connect(ch1CheckBox,SIGNAL(stateChanged(int)),signalMapper,SLOT(map()));
         connect(ch2CheckBox,SIGNAL(stateChanged(int)),signalMapper,SLOT(map()));
         connect(ch3CheckBox,SIGNAL(stateChanged(int)),signalMapper,SLOT(map()));
         connect(ch4CheckBox,SIGNAL(stateChanged(int)),signalMapper,SLOT(map()));

    getButton->setEnabled(false);
    connect(getButton,&QPushButton::clicked,this, &TransmitionSettings::getButtonClicked);

    ch4InCheckBox->hide();
    ch2InCheckBox->hide();
    packetSizeLabel->hide();
}
