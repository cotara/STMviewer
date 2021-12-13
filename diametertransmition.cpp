#include "diametertransmition.h"

DiameterTransmition::DiameterTransmition(QWidget *parent): QGroupBox(parent){

    setTitle("График диаметра");
    layout = new QVBoxLayout(this);
    averageLayout = new QHBoxLayout();
    windowSizeLayout = new QHBoxLayout();
    reqFreqLayout = new QHBoxLayout();
    sliderLayout = new QHBoxLayout();

    gettingDiameterButton = new QPushButton("Получать радиус");
    gettingDiameterButton->setCheckable(true);

    diemetersCheckBox = new QCheckBox("Выводить диаметры",this);
    centersCheckBox= new QCheckBox("Выводить центры",this);
    medianFilterCheckbox = new QCheckBox("Медианный фильтр",this);

    windowSizeSpinbox = new QSpinBox(this);
    windowSizeSpinbox->setEnabled(false);
    averageSpinbox = new QSpinBox(this);
    averageSpinbox->setEnabled(false);
    reqFreqSpinbox = new QSpinBox(this);
    windowSizeLabel = new QLabel ("Окно фильтра", this);
    averageLabel = new QLabel ("Усреднение фильтра", this);
    reqFreqLabel = new QLabel ("Периодичность запросов, раз/с", this);

    xWindow = new QSlider(Qt::Horizontal,this);
    xWindow->setMinimum(100);
    xWindow->setMaximum(10000);
    xWindow->setValue(5000);
    xWindowsLabel = new QLabel("Окно отображения");
    sliderValue = new QLabel("5000");
    sliderValue->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    windowSizeSpinbox->setRange(1,100);
    windowSizeSpinbox->setValue(10);
    averageSpinbox->setRange(1,100);
    averageSpinbox->setValue(10);
    reqFreqSpinbox->setRange(1,20);
    reqFreqSpinbox->setValue(10);

    sliderLayout->addWidget(xWindowsLabel);
    sliderLayout->addWidget(xWindow);
    sliderLayout->addWidget(sliderValue);
    layout->addLayout(sliderLayout);

    layout->addWidget(diemetersCheckBox);
    layout->addWidget(centersCheckBox);
    layout->addWidget(medianFilterCheckbox);

    windowSizeLayout->addWidget(windowSizeLabel);
    windowSizeLayout->addWidget(windowSizeSpinbox);
    layout->addLayout(windowSizeLayout);

    averageLayout->addWidget(averageLabel);
    averageLayout->addWidget(averageSpinbox);
    layout->addLayout(averageLayout);

    reqFreqLayout->addWidget(reqFreqLabel);
    reqFreqLayout->addWidget(reqFreqSpinbox);
    layout->addLayout(reqFreqLayout);

    layout->addWidget(gettingDiameterButton);

    connect(gettingDiameterButton,&QPushButton::clicked,[=](bool checked){
        if(!checked){
            medianFilterCheckbox->setEnabled(true);
            windowSizeSpinbox->setEnabled(true);
            averageSpinbox->setEnabled(true);
        }
        else{
            medianFilterCheckbox->setEnabled(false);
            windowSizeSpinbox->setEnabled(false);
            averageSpinbox->setEnabled(false);
        }
        emit getDiameterChanged(checked);
    });
    connect(reqFreqSpinbox, QOverload<int>::of(&QSpinBox::valueChanged),[=](int i){ emit reqFreqValueChanged(i); });

    connect(xWindow, &QSlider::valueChanged,this,&DiameterTransmition::xWindowChanged);
    connect(xWindow, &QSlider::valueChanged,[=](int value) {sliderValue->setNum(value);});
    connect(medianFilterCheckbox, &QCheckBox::stateChanged,averageSpinbox,&QSpinBox::setEnabled);
    connect(medianFilterCheckbox, &QCheckBox::stateChanged,windowSizeSpinbox,&QSpinBox::setEnabled);


}
