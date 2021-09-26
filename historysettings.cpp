#include "historysettings.h"


HistorySettings::HistorySettings(QWidget *parent) : QGroupBox(parent)
{
    setTitle("История сигнала");
    setObjectName("historysettings");
    layout = new QVBoxLayout(this);


    //Настройки логирования
    autoSaveShotCheckBox = new QCheckBox("Авто-сохранение снимка");

    connect(autoSaveShotCheckBox,&QCheckBox::stateChanged,this,&HistorySettings::autoSaveShotCheked);
    autoSaveShotCheckBox->setEnabled(false);
    layout->addWidget(autoSaveShotCheckBox);

    //История
    shotsComboBox = new QComboBox;
    clearButton = new QPushButton("Очистить список");
    layout->addWidget(shotsComboBox);
    layout->addWidget(clearButton);

    connect(shotsComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
          [=](int index){emit shotSelected(index);});

    connect(clearButton,&QPushButton::clicked,this, &HistorySettings::clearButtonClicked);

}
