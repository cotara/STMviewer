#include "transmitionsettings.h"
#include "qmessagebox.h"

TransmitionSettings::TransmitionSettings(QWidget *parent) : CollapsibleGroupBox(parent)
{
    setTitle("Параметры передачи");
    setObjectName("transmitionsettings");
    layout = new QVBoxLayout(this);

    //Настройки передачи
    chCheckBox.append(new QCheckBox("К1.Origin"));
    chCheckBox.append(new QCheckBox("К1.Filter"));
    chCheckBox.append(new QCheckBox("К2.Origin"));
    chCheckBox.append(new QCheckBox("К2.Filter"));


    //Сдвиг фильтрованного сигнала
    shift1Layout = new QHBoxLayout();
    shift1Label = new QLabel("Сдвиг фильтрованного сигнала");
    shift1Spinbox = new QSpinBox (this);
    shift1Spinbox->setRange(-100,100);
    shift1Spinbox->setValue(0);
    shift1Layout->addWidget(shift1Label);
    shift1Layout->addWidget(shift1Spinbox);
    //Сдвиг нефильтрованного сигнала
    shift2Layout = new QHBoxLayout();
    shift2Label = new QLabel("Сдвиг нефильтрованного сигнала");
    shift2Spinbox = new QSpinBox (this);
    shift2Spinbox->setRange(-100,100);
    shift2Spinbox->setValue(0);
    shift2Layout->addWidget(shift2Label);
    shift2Layout->addWidget(shift2Spinbox);

    connect(shift1Spinbox,&QSpinBox::valueChanged, this, [=](int val) {emit shiftChanged(0, val);});
    connect(shift2Spinbox,&QSpinBox::valueChanged, this, [=](int val) {emit shiftChanged(1, val);});

    layout->addLayout(shift1Layout);
    layout->addLayout(shift2Layout);

    getButton = new QPushButton("Получать сигнал");
    getButton->setCheckable(true);
    layout->addWidget(getButton);
    connect(getButton,&QPushButton::clicked,this, [=](bool en){
        if(order==0){
            QMessageBox::warning(this, "Внимание!", "Не выбрано ни одного канала!",QMessageBox::Ok);
            return;
        }
        emit getButtonClicked(en);
    });

    chChkLayout = new QHBoxLayout();
    layout->addLayout(chChkLayout);
    for(int i=0;i<chCheckBox.count();i++){
        chChkLayout->addWidget(chCheckBox.at(i));
        connect(chCheckBox.at(i),&QCheckBox::checkStateChanged,this, [this, i](){
            if(qobject_cast<QCheckBox*>(sender()) -> isChecked())
                order|= 1<<i;
            else
                order&=~(1<<i);

            emit chChooseChanged(order);
        });
    }

    chCheckBox.at(1)->setChecked(true);
    chCheckBox.at(3)->setChecked(true);
}

//Включение / отключение выбора каналов
void TransmitionSettings::setChEn(bool en){
    for(int i=0;i<chCheckBox.count();i++)
        (chCheckBox.at(i)->setEnabled(en));
}

void TransmitionSettings::setGetButton(bool en){
    getButton->setChecked(en);
}

bool TransmitionSettings::getStatusGetButton(){
    return getButton->isChecked();
}
