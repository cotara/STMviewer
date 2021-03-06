#include "historysettings.h"


HistorySettings::HistorySettings(QWidget *parent) : QGroupBox(parent)
{
    setTitle("История сигнала");
    setObjectName("historysettings");
    layout = new QVBoxLayout(this);


    //Настройки логирования
    saveHistoryButton = new QPushButton("Авто-сохранение снимка");

    connect(saveHistoryButton,&QPushButton::clicked,this,&HistorySettings::saveHistoryPushed);
    saveHistoryButton->setEnabled(false);
    layout->addWidget(saveHistoryButton);

    //История
    shotsComboBox = new QComboBox;
    clearButton = new QPushButton("Очистить список");
    layout->addWidget(shotsComboBox);
    layout->addWidget(clearButton);

    connect(shotsComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
          [=](int index){emit shotSelected(index);});

    connect(clearButton,&QPushButton::clicked,this, &HistorySettings::clearButtonClicked);

}
