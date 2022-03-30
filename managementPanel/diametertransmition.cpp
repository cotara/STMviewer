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
    limitLayout = new QHBoxLayout();

    gettingDiameterButton = new QPushButton("Получать радиус");
    gettingDiameterButton->setCheckable(true);
    gettingDiameterButton->setEnabled(false);



    diemetersCheckBox = new QCheckBox("Выводить диаметры",this);
    centersCheckBox= new QCheckBox("Выводить центры",this);
    medianFilterCheckbox = new QCheckBox("Медианный фильтр",this);

    windowSizeSpinbox = new QSpinBox(this);
    windowSizeSpinbox->setEnabled(false);
    averageSpinbox = new QSpinBox(this);
    averageSpinbox->setEnabled(false);
    limitSpinbox = new QSpinBox(this);
    limitSpinbox->setEnabled(false);
    reqFreqSpinbox = new QSpinBox(this);

    windowSizeLabel = new QLabel ("Окно фильтра", this);
    averageLabel = new QLabel ("Усреднение фильтра", this);
    limitLabel = new QLabel ("Порог срабатывания",this);
    reqFreqLabel = new QLabel ("Периодичность запросов, раз/с", this);

    //Выбор режима
    continiousMode = new QRadioButton("Реальное время",this);
    collectMode = new QRadioButton("Накопительный режим",this);
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
    limitLayout->addWidget(limitLabel);
    limitLayout->addWidget(limitSpinbox);
    layout->addLayout(limitLayout);

    windowSizeSpinbox->setRange(3,10000);
    windowSizeSpinbox->setValue(101);
    averageSpinbox->setRange(0,10000);
    averageSpinbox->setValue(100);
    limitSpinbox->setRange(10,1000);
    limitSpinbox->setValue(100);
    reqFreqSpinbox->setRange(1,7);
    reqFreqSpinbox->setValue(5);

    //Частота опроса
    reqFreqLayout->addWidget(reqFreqLabel);
    reqFreqLayout->addWidget(reqFreqSpinbox);
    layout->addLayout(reqFreqLayout);

    //Накопительный режим
    collectLabel = new QLabel("Собрано точек",this);
    collectCountLabel = new QLabel("-",this);
    colectLayout->addWidget(collectLabel);
    colectLayout->addWidget(collectCountLabel);
    layout->addLayout(colectLayout);

    //ФУРЬЕ
    furieLimitLayout = new QHBoxLayout();
    furieCheckbox = new QCheckBox("Преобразование фурье",this);
    furieLimitSpinbox = new  QSpinBox(this);
    furieLimitSpinbox->setValue(20);
    furieLimitLabel = new QLabel("Амплитуда среза, Дб", this);
    furieLimitLayout->addWidget(furieLimitLabel);
    furieLimitLayout->addWidget(furieLimitSpinbox);
    layout->addLayout(furieLimitLayout);
    layout->addWidget(furieCheckbox);


    //КНОПКА
    layout->addWidget(gettingDiameterButton);

    //Вывод диаметров
    r1Label = new QLabel("R1: ",this);
    r2Label = new QLabel("R2: ",this);
    r1ValueLabel = new QLabel(this);
    r2ValueLabel = new QLabel(this);
    r1HLayout = new QHBoxLayout();
    r2HLayout = new QHBoxLayout();
    r1HLayout->addWidget(r1Label);
    r1HLayout->addWidget(r1ValueLabel);
    r2HLayout->addWidget(r2Label);
    r2HLayout->addWidget(r2ValueLabel);
    layout->addLayout(r1HLayout);
    layout->addLayout(r2HLayout);
    r1ValueLabel->setObjectName("BigLabel");
    r2ValueLabel->setObjectName("BigLabel");



    /********************************КОННЕКТЫ*********************************************************/
    connect(gettingDiameterButton,&QPushButton::clicked,[=](bool checked){
        if(medianFilterCheckbox->isChecked() || diemetersCheckBox->isChecked() || centersCheckBox->isChecked())
            emit getDiameterChanged(checked);
        else
            QMessageBox::warning(this, "Внимание!", "Не выбрано данных для вывода!",QMessageBox::Ok);
    });
    connect(reqFreqSpinbox, QOverload<int>::of(&QSpinBox::valueChanged),[=](int i){ emit reqFreqValueChanged(i); });

    connect(xWindow, &QSlider::valueChanged,[=](int value) {sliderValue->setNum(value); emit xWindowChanged(value);});
    connect(medianFilterCheckbox, &QCheckBox::stateChanged,averageSpinbox,&QSpinBox::setEnabled);
    connect(medianFilterCheckbox, &QCheckBox::stateChanged,windowSizeSpinbox,&QSpinBox::setEnabled);
    connect(medianFilterCheckbox, &QCheckBox::stateChanged,limitSpinbox,&QSpinBox::setEnabled);
    connect(averageSpinbox, QOverload<int>::of(&QSpinBox::valueChanged),[=](int i){   emit averageChanged(i);});
    connect(limitSpinbox, QOverload<int>::of(&QSpinBox::valueChanged),[=](int i){   emit limitChanged(i);});
    connect(windowSizeSpinbox, QOverload<int>::of(&QSpinBox::valueChanged),[=](int i){  emit windowSizeChanged(i);});
    connect(furieLimitSpinbox, QOverload<int>::of(&QSpinBox::valueChanged),[=](int i){  emit furieLimitChanged(i);});

    connect(continiousMode, &QRadioButton::clicked,[=](bool checked){
        emit diameterModeChanged(false);
        collectMode->setChecked(!checked);
        collectLabel->setVisible(!checked);
        collectCountLabel->setVisible(!checked);
    });
    connect(collectMode, &QRadioButton::clicked,[=](bool checked){
        emit diameterModeChanged(true);
        continiousMode->setChecked(!checked);
        collectLabel->setVisible(checked);
        collectCountLabel->setVisible(checked);
    });

    continiousMode->click();

}
