#include "diametertransmition.h"
#include <QMessageBox>

DiameterTransmition::DiameterTransmition(QWidget *parent): QGroupBox(parent){

    setTitle("График диаметра");
    layout = new QVBoxLayout(this);
    averageLayout = new QHBoxLayout();
    windowSizeLayout = new QHBoxLayout();
    reqFreqLayout = new QHBoxLayout();
    sliderLayout = new QHBoxLayout();
    radioLayout = new QHBoxLayout();
    colectLayout = new QHBoxLayout();

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

    //Выбор режима
    continiousMode = new QRadioButton("Реальное время",this);
    collectMode = new QRadioButton("Собрать пул",this);
    radioLayout->addWidget(continiousMode);
    radioLayout->addWidget(collectMode);
    layout->addLayout(radioLayout);


    //Окно просмотра по оси Х
    xWindow = new QSlider(Qt::Horizontal,this);
    xWindow->setMinimum(1000);
    xWindow->setMaximum(10000);
    xWindow->setValue(5000);
    xWindowsLabel = new QLabel("Окно отображения");
    sliderValue = new QLabel("5000");
    sliderValue->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    sliderLayout->addWidget(xWindowsLabel);
    sliderLayout->addWidget(xWindow);
    sliderLayout->addWidget(sliderValue);
    layout->addLayout(sliderLayout);

    //Выбор что выводить на график
    layout->addWidget(diemetersCheckBox);
    layout->addWidget(centersCheckBox);
    layout->addWidget(medianFilterCheckbox);

    //Настройки медианного фильтра
    windowSizeLayout->addWidget(windowSizeLabel);
    windowSizeLayout->addWidget(windowSizeSpinbox);
    layout->addLayout(windowSizeLayout);
    averageLayout->addWidget(averageLabel);
    averageLayout->addWidget(averageSpinbox);
    layout->addLayout(averageLayout);

    windowSizeSpinbox->setRange(1,100);
    windowSizeSpinbox->setValue(10);
    averageSpinbox->setRange(1,100);
    averageSpinbox->setValue(10);
    reqFreqSpinbox->setRange(1,20);
    reqFreqSpinbox->setValue(10);

    //Частота опроса
    reqFreqLayout->addWidget(reqFreqLabel);
    reqFreqLayout->addWidget(reqFreqSpinbox);
    layout->addLayout(reqFreqLayout);

    //Собрать пул
    countPointsLabel = new QLabel("Собрать точек, тыс",this);
    countPointsBox = new QSpinBox(this);
    countPointsBox->setRange(1,100);
    countPointsBox->setValue(50);
    progressBar = new QProgressBar(this);
    progressBar->setMaximum(10000);
    colectLayout->addWidget(countPointsLabel);
    colectLayout->addWidget(countPointsBox);
    layout->addLayout(colectLayout);
    layout->addWidget(progressBar);

    //Получать диаметры
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
        if(medianFilterCheckbox->isChecked() || diemetersCheckBox->isChecked() || centersCheckBox->isChecked())
            emit getDiameterChanged(checked);
        else
            QMessageBox::warning(this, "Внимание!", "Не выбрано данных для вывода!",QMessageBox::Ok);
    });
    connect(reqFreqSpinbox, QOverload<int>::of(&QSpinBox::valueChanged),[=](int i){ emit reqFreqValueChanged(i); });

    connect(xWindow, &QSlider::valueChanged,this,&DiameterTransmition::xWindowChanged);
    connect(xWindow, &QSlider::valueChanged,[=](int value) {sliderValue->setNum(value);});
    connect(medianFilterCheckbox, &QCheckBox::stateChanged,averageSpinbox,&QSpinBox::setEnabled);
    connect(medianFilterCheckbox, &QCheckBox::stateChanged,windowSizeSpinbox,&QSpinBox::setEnabled);
    connect(countPointsBox, QOverload<int>::of(&QSpinBox::valueChanged),[=](int i){ progressBar->setMaximum(i*1000);  emit countPointsChanged(i*1000);});
    connect(continiousMode, &QRadioButton::clicked,[=](bool checked){
        emit diameterModeChanged(false);
        collectMode->setChecked(!checked);
        countPointsLabel->setVisible(!checked);
        countPointsBox->setVisible(!checked);
        progressBar->setVisible(!checked);
    });
    connect(collectMode, &QRadioButton::clicked,[=](bool checked){
        emit diameterModeChanged(true);
        continiousMode->setChecked(!checked);
        countPointsLabel->setVisible(checked);
        countPointsBox->setVisible(checked);
        progressBar->setVisible(checked);
    });

    continiousMode->click();
}
