#include "diametertransmition.h"

DiameterTransmition::DiameterTransmition(QWidget *parent): QGroupBox(parent){

    setTitle("График диаметра");
    layout = new QVBoxLayout(this);
    averageLayout = new QHBoxLayout();
    windowSizeLayout = new QHBoxLayout();
    reqFreqLayout = new QHBoxLayout();

    gettingDiameterCheckbox = new QCheckBox(this);
    medianFilterCheckbox = new QCheckBox(this);
    windowSizeSpinbox = new QSpinBox(this);
    averageSpinbox = new QSpinBox(this);
    reqFreqSpinbox = new QSpinBox(this);
    windowSizeLabel = new QLabel ("Окно фильтра", this);
    averageLabel = new QLabel ("Усреднение фильтра", this);
    reqFreqLabel = new QLabel ("Периодичность запросов, раз/с", this);

    gettingDiameterCheckbox->setText("Получать диаметр");
    medianFilterCheckbox->setText("Медианный фильтр");
    medianFilterCheckbox->setEnabled(false);
    windowSizeSpinbox->setEnabled(false);
    averageSpinbox->setEnabled(false);
    windowSizeSpinbox->setRange(1,100);
    windowSizeSpinbox->setValue(10);
    averageSpinbox->setRange(1,100);
    averageSpinbox->setValue(10);
    reqFreqSpinbox->setRange(1,20);
    reqFreqSpinbox->setValue(10);

    reqFreqLayout->addWidget(reqFreqLabel);
    reqFreqLayout->addWidget(reqFreqSpinbox);
    layout->addWidget(gettingDiameterCheckbox);
    layout->addWidget(medianFilterCheckbox);
    windowSizeLayout->addWidget(windowSizeLabel);
    windowSizeLayout->addWidget(windowSizeSpinbox);
    averageLayout->addWidget(averageLabel);
    averageLayout->addWidget(averageSpinbox);

    layout->addLayout(reqFreqLayout);
    layout->addLayout(windowSizeLayout);
    layout->addLayout(averageLayout);

    connect(gettingDiameterCheckbox,&QCheckBox::stateChanged,[=](int state){
        if(state){
            medianFilterCheckbox->setEnabled(true);
            windowSizeSpinbox->setEnabled(true);
            averageSpinbox->setEnabled(true);
        }
        else{
            medianFilterCheckbox->setEnabled(false);
            windowSizeSpinbox->setEnabled(false);
            averageSpinbox->setEnabled(false);
        }
        emit getDiameterChanged(state);
    });
    connect(reqFreqSpinbox, QOverload<int>::of(&QSpinBox::valueChanged),
                [=](int i){ emit reqFreqValueChanged(i); });

}
