#include "historysettings.h"


HistorySettings::HistorySettings(QWidget *parent) : CollapsibleGroupBox(parent)
{
    setTitle("История сигнала");
    setObjectName("historysettings");
    layout = new QVBoxLayout(this);
    butLayout = new QHBoxLayout();

    //Настройки логирования
    saveButton = new QPushButton("Сохранить");
    connect(saveButton,&QPushButton::clicked,this,&HistorySettings::saveHistory);
    loadButton = new QPushButton("Загрузить");
    connect(loadButton,&QPushButton::clicked,this,&HistorySettings::loadHistory);
    clearButton = new QPushButton("Очистить");
    connect(clearButton,&QPushButton::clicked,this, &HistorySettings::clear);

    butLayout->addWidget(saveButton);
    butLayout->addWidget(loadButton);
    butLayout->addWidget(clearButton);


    //История
    shotsComboBox = new QComboBox;
    connect(shotsComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [=](int index){emit shotSelected(index);});

    layout->addWidget(shotsComboBox);
    layout->addLayout(butLayout);
}

void HistorySettings::addShot(int val){
    shotsComboBox->addItem(QString::number(val));
    shotsComboBox->setCurrentIndex(shotsComboBox->count()-1);
}

int HistorySettings::curShot(){
    return shotsComboBox->currentText().toInt();
}

void HistorySettings::clear(){
    shotsComboBox->clear();
    emit clearHistory();
}
