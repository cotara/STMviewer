#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "serialsettings.h"

#include "slip.h"
#include "transp.h"
#include "statusbar.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->disconnect->setEnabled(false);

    settings_ptr = new SerialSettings();
    serial = new QSerialPort();

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::handlerTimer);

    viewer = new ShotViewer(this);
    connect(viewer,&ShotViewer::graph_selected,this,&MainWindow::fillTable);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(2);
    m_table->setRowCount(1);
    m_table->setShowGrid(true); // Включаем сетку
    m_table->setHorizontalHeaderLabels(QStringList{"X","Y"});
    m_table->horizontalHeader()->resizeSection(0, 50);//ширина
    m_table->horizontalHeader()->resizeSection(1, 50);

    //Консоль
    m_console = new Console(this);
    m_console->setMaximumHeight(150);
    m_console->hide();
    //Транспортный уровень SLIP протокола
    m_slip = new Slip(serial,m_console);
    connect(m_slip,&Slip::serialPortError,this,&MainWindow::on_disconnect_triggered);

    m_transp = new Transp(m_slip);
    connect(m_transp, &Transp::answerReceive, this, &MainWindow::handlerTranspAnswerReceive);
    connect(m_transp, &Transp::transpError, this, &MainWindow::handlerTranspError);
    connect(m_transp, &Transp::reSentInc,this, &MainWindow::reSentInc);

    //Статус бар
    statusBar = new StatusBar(ui->statusBar);
    connect(this, &MainWindow::statusUpdate, [this](bool online) { statusBar->setStatus(online); });
    connect(this, &MainWindow::dataReadyUpdate, [this](int ready) { statusBar->setDataReady(ready); });
    connect(this, &MainWindow::infoUpdate, [this](int info) { statusBar->setInfo(info); });

    //Интерфейс
    //Здесь нужно создавать виджет(главный), устанавливать его, как центральный и передавать в конструктор лайонута уже его..
    //widget = QWidget(self)
    //self.setCentralWidget(widget)
    //hbox = QHBoxLayout(widget)
    // hbox.setSpacing(10)


    layoutV = new QVBoxLayout();
    centralWidget()->setLayout(layoutV);

    layoutH = new QHBoxLayout();
    layoutV->addLayout(layoutH);

    layoutV->addWidget(m_console);

    controlLayout = new QVBoxLayout();

    layoutH->addWidget(viewer);
    layoutH->addWidget(m_table);
    m_table->setMaximumWidth(200);

    //layoutH->addLayout(controlLayout);

    scroll = new QScrollArea(this);
    settingsWidget = new QWidget(scroll);
    scrolLayout = new QVBoxLayout(settingsWidget);
    scroll->setMaximumWidth(290);



//    for(int i=0; i<100;i++)
//        scrolLayout->addWidget(new QLabel("Привет, мир!"));


    errorsGroup = new QGroupBox("Ошибки");
    lazerGroup = new QGroupBox("Настройка лазеров");
    transmitGroup = new QGroupBox("Обмен данными");
    borderGroup = new QGroupBox("Границы сигналов");
    resultGroup = new QGroupBox("Результаты расчетов");
    appSettingsGroup = new QGroupBox("Настройки интерфейса");
    logGroup = new QGroupBox("Логирование");
    historyGrouop = new QGroupBox("История");

    errorsGroup->setMaximumWidth(250);
    lazerGroup->setMaximumWidth(250);
    transmitGroup->setMaximumWidth(250);
    borderGroup->setMaximumWidth(250);
    resultGroup->setMaximumWidth(250);
    appSettingsGroup->setMaximumWidth(250);
    logGroup->setMaximumWidth(250);
    historyGrouop->setMaximumWidth(250);

    scrolLayout->addWidget(errorsGroup);
    scrolLayout->addWidget(lazerGroup);
    scrolLayout->addWidget(transmitGroup);
    scrolLayout->addWidget(borderGroup);
    scrolLayout->addWidget(resultGroup);
    scrolLayout->addWidget(appSettingsGroup);
    scrolLayout->addWidget(logGroup);
    scrolLayout->addWidget(historyGrouop);


    errorsLayout = new QVBoxLayout();
    lazerLayout = new QHBoxLayout();
    transmitLayout = new QVBoxLayout();
    borderLayout = new QHBoxLayout();
    resultLayout = new QVBoxLayout();
    appSettingsLayout = new QVBoxLayout();
    logLayout = new QVBoxLayout();
    historyLayout = new QVBoxLayout();

    errorsGroup->setLayout(errorsLayout);
    lazerGroup->setLayout(lazerLayout);
    transmitGroup->setLayout(transmitLayout);
    borderGroup->setLayout(borderLayout);
    resultGroup->setLayout(resultLayout);
    appSettingsGroup->setLayout(appSettingsLayout);
    logGroup->setLayout(logLayout);
    historyGrouop->setLayout(historyLayout);

    //Ошибки
    err1 = new QLCDNumber(2,this);
    err2 = new QLCDNumber(2,this);
    errorsLayout->addWidget(err1);
    errorsLayout->addWidget(err2);



    //Настройки лазера
    lazer1SettingLayout = new QVBoxLayout();
    lazer2SettingLayout = new QVBoxLayout();
    lazerLayout->addLayout(lazer1SettingLayout);
    lazerLayout->addLayout(lazer2SettingLayout);
    lazer1Spinbox = new QSpinBox();
    lazer2Spinbox = new QSpinBox();
    lazer1Label = new QLabel("Лазер 1:");
    lazer2Label = new QLabel("Лазер 2:");
    lazersSaveButton = new QPushButton("Сохранить в EEPROM");

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
    lazerLayout->addWidget(lazersSaveButton);

    lazer1Spinbox->setEnabled(false);
    lazer2Spinbox->setEnabled(false);
    lazersSaveButton->setEnabled(false);

    connect(lazer1Spinbox, QOverload<int>::of(&QSpinBox::valueChanged),
          [=](int i){MainWindow::sendLazer1(i); });
    connect(lazer2Spinbox, QOverload<int>::of(&QSpinBox::valueChanged),
          [=](int i){MainWindow::sendLazer2(i);});
    connect(lazersSaveButton,&QPushButton::clicked,this,&MainWindow::sendSaveEeprom);
    //Настройки передачи
    packetSizeLabel = new QLabel("Размер пакета:",this);
    packetSizeSpinbox = new QSpinBox(this);
    ch1CheckBox = new QCheckBox("Канал 1. Нефильтрованный");
    ch2CheckBox = new QCheckBox("Канал 1. Фильтрованный");
    ch2InCheckBox = new QCheckBox("Канал 1. Фильтрованный*");
    ch3CheckBox = new QCheckBox("Канал 2. Нефильтрованный");
    ch4CheckBox = new QCheckBox("Канал 2. Фильтрованный");
    ch4InCheckBox = new QCheckBox("Канал 2. Фильтрованный*");

    getButton = new QPushButton("Получить снимок");
    autoGetCheckBox = new QCheckBox("Авто-получение по готовности");

    transmitLayout->addWidget(packetSizeLabel);
    transmitLayout->addWidget(packetSizeSpinbox);
    transmitLayout->addWidget(ch1CheckBox);
    transmitLayout->addWidget(ch2CheckBox);
    transmitLayout->addWidget(ch2InCheckBox);
    transmitLayout->addWidget(ch3CheckBox);
    transmitLayout->addWidget(ch4CheckBox);
    transmitLayout->addWidget(ch4InCheckBox);
    ch1CheckBox->setEnabled(false);
    ch2CheckBox->setEnabled(false);
    ch2InCheckBox->setEnabled(false);
    ch3CheckBox->setEnabled(false);
    ch4CheckBox->setEnabled(false);
    ch4InCheckBox->setEnabled(false);
    transmitLayout->addWidget(getButton);
    transmitLayout->addWidget(autoGetCheckBox);

    packetSizeSpinbox->setRange(50,15000);
    packetSizeSpinbox->setValue(11000);
    connect(packetSizeSpinbox, QOverload<int>::of(&QSpinBox::valueChanged),
         [=](unsigned short i){
            setPacketSize(i);});
    emit packetSizeSpinbox->valueChanged(packetSizeSpinbox->value());

    signalMapper = new QSignalMapper(this);
    connect(signalMapper, QOverload<int>::of(&QSignalMapper::mapped), [=](int i){ incCountCh(i); });
         signalMapper->setMapping(ch1CheckBox, 1);
         signalMapper->setMapping(ch2CheckBox, 2);
         signalMapper->setMapping(ch3CheckBox, 3);
         signalMapper->setMapping(ch4CheckBox, 4);


         connect(ch1CheckBox,SIGNAL(stateChanged(int)),signalMapper,SLOT(map()));
         connect(ch2CheckBox,SIGNAL(stateChanged(int)),signalMapper,SLOT(map()));
         connect(ch3CheckBox,SIGNAL(stateChanged(int)),signalMapper,SLOT(map()));
         connect(ch4CheckBox,SIGNAL(stateChanged(int)),signalMapper,SLOT(map()));


    getButton->setEnabled(false);
    connect(getButton,&QPushButton::clicked,this, &MainWindow::manualGetShotButton);

    autoGetCheckBox->setEnabled(false);
    connect(autoGetCheckBox,&QCheckBox::stateChanged,this, &MainWindow::autoGetCheckBoxChanged);
    //Настройка границ
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

//    borderLeftSpinbox->setEnabled(false);
//    borderRightSpinbox->setEnabled(false);
//    compCH1Spinbox->setEnabled(false);
//    compCH2Spinbox->setEnabled(false);

    connect(borderLeftSpinbox, QOverload<int>::of(&QSpinBox::valueChanged),
          [=](int i){MainWindow::sendBorderLeft(i);});
    connect(borderRightSpinbox, QOverload<int>::of(&QSpinBox::valueChanged),
          [=](int i){MainWindow::sendBorderRight(i);});
    connect(compCH1Spinbox, QOverload<int>::of(&QSpinBox::valueChanged),
          [=](int i){MainWindow::sendCompCH1(i);});
    connect(compCH2Spinbox, QOverload<int>::of(&QSpinBox::valueChanged),
          [=](int i){MainWindow::sendCompCH2(i);});


    //Результат
    diametrLabel = new QLabel("Диаметр: ");
    diametrPlisLabel = new QLabel("Диаметр ПЛИС: ");
    leftShadow1Label = new QLabel("   Лев.тень: ");
    rightShadow1Label = new QLabel("   Прав.тень: ");
    leftShadow2Label = new QLabel("   Лев.тень: ");
    rightShadow2Label = new QLabel("   Прав.тень: ");
    m_centerViewer = new centerViewer();
    centerPositionLabel = new QLabel("Смещение: ");
    ch1ShadowsLabel.setText("Канал 1: ");
    ch2ShadowsLabel.setText("Канал 2: ");
    resultLayout->addWidget(&ch1ShadowsLabel);
    resultLayout->addWidget(leftShadow1Label);
    resultLayout->addWidget(rightShadow1Label);
    resultLayout->addWidget(&ch2ShadowsLabel);
    resultLayout->addWidget(leftShadow2Label);
    resultLayout->addWidget(rightShadow2Label);
    resultLayout->addWidget(diametrLabel);
    resultLayout->addWidget(diametrPlisLabel);
    resultLayout->addWidget(m_centerViewer);
    m_centerViewer->setMinimumHeight(105);
    resultLayout->addWidget(centerPositionLabel);

    //Настройки интерфейса
    consoleEnable = new QCheckBox("Вывод в консоль");
    appSettingsLayout->addWidget(consoleEnable);
    consoleEnable->setChecked(false);
    connect(consoleEnable,&QCheckBox::stateChanged,this,&MainWindow::consoleEnabledCheked);

    autoRangeGraph = new QPushButton("Автомасштаб");
    appSettingsLayout->addWidget(autoRangeGraph);
    connect(autoRangeGraph,&QPushButton::clicked,viewer, &ShotViewer::autoScale);

    tableSizeLayout = new QHBoxLayout;
    tableSizeSpinbox = new QSpinBox;
    tableSizeLabel = new QLabel("Размер таблицы");

    appSettingsLayout->addLayout(tableSizeLayout);
    tableSizeLayout->addWidget(tableSizeLabel);
    tableSizeLayout->addWidget(tableSizeSpinbox);
    tableSizeSpinbox->setMaximum(20000);
    tableSizeSpinbox->setValue(100);

    connect(tableSizeSpinbox, QOverload<int>::of(&QSpinBox::valueChanged),[=](int val){
            tableSize = val;

            if(m_table->rowCount()>=tableSize){
                for(int i=0;i<tableSize;i++)
                     m_table->showRow(i);
                for(int i=tableSize;i<m_table->rowCount();i++)
                    m_table->hideRow(i);
            }

    });

    //Настройки логирования
    autoSaveShotCheckBox = new QCheckBox("Авто-сохранение снимка");
    logLayout->addWidget(autoSaveShotCheckBox);
    connect(autoSaveShotCheckBox,&QCheckBox::stateChanged,this,&MainWindow::autoSaveShotCheked);
    autoSaveShotCheckBox->setEnabled(false);

    //Создание папки с логами, если ее нет.
    dir = new QDir(dirname);
    if (!dir->exists()) {
        dir->mkdir(dirname);
    }
    dir->setFilter( QDir::NoDotAndDotDot);

    file1 = new QFile();
    file2 = new QFile();
    file3 = new QFile();
    file4 = new QFile();

    //История
    shotsComboBox = new QComboBox;
    clearButton = new QPushButton("Очистить список");
    historyLayout->addWidget(shotsComboBox);
    historyLayout->addWidget(clearButton);

    connect(shotsComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
          [=](int index){
            selectShot(index);
    });

    connect(clearButton,&QPushButton::clicked,this, &MainWindow::on_clearButton);

   ShadowSettings = new SettingsShadowsFindDialog(this);
   //Fir filter
   filter = new firFilter(ShadowSettings->getShadowFindSettings());//Инициализируем настройками из файла
   //constructorTest();

   settingsWidget->setLayout(scrolLayout);
   scroll->setWidget(settingsWidget);
   layoutH->addWidget(scroll);
}

MainWindow::~MainWindow(){
    delete ui;
    delete settings_ptr;
    delete serial;
    delete m_transp;
    delete m_timer;
    delete viewer;
}
void MainWindow::constructorTest(){
    QFile *tempFile;                                                            //Файл лога
    tempFile = new QFile();

    filename = "2021_08_16__20_05_22_CH1";
    tempFile->setFileName(dirname + "/" + filename);
    if(!tempFile->open(QIODevice::ReadOnly)){
        qDebug() << "tempFile can`t be open";
        return;
    }
    QByteArray tempBuf = tempFile->readAll();                                           //Читаем большой буфер с несколькими кадрами
    QList<QByteArray> list_tempCH1=tempBuf.split(0xFF);                                    //разделяем кадры
    tempFile->close();

    filename = "2021_08_16__20_05_22_CH2";
    tempFile->setFileName(dirname + "/" + filename);
    if(!tempFile->open(QIODevice::ReadOnly)){
        qDebug() << "tempFile can`t be open";
        return;
    }
    tempBuf = tempFile->readAll();                                           //Читаем большой буфер с несколькими кадрами
    QList<QByteArray> list_tempCH2=tempBuf.split(0xFF);                                    //разделяем кадры
    tempFile->close();

    QByteArray tempBuf2;
    QByteArray filtered;
    int k=0;
    for(int i=0;i<list_tempCH2.size();i+=2){
        tempBuf2 = list_tempCH1.at(i);
        if(tempBuf2.size()>10){
            shotsCH1.insert(k,tempBuf2);
            //shotsCH2.insert(k,filter->toFilter(tempBuf2,tempBuf2.size()));
            shotsCH2In.insert(k,filter->toButterFilter(tempBuf2,tempBuf2.size()));
        }
        tempBuf2 = list_tempCH2.at(i);
        if(tempBuf2.size()>10){
            shotsCH3.insert(k,tempBuf2);
            //shotsCH4.insert(k,filter->toFilter(tempBuf2,tempBuf2.size()));
            shotsCH4In.insert(k,filter->toButterFilter(tempBuf2,tempBuf2.size()));
        }
        k++;
        shotsComboBox->addItem(QString::number(i));
    }
}

//Настройки, коннекты
void MainWindow::on_settings_triggered(){
    settings_ptr->show();
}
void MainWindow::on_connect_triggered()
{
    //Настраиваем компорт
    serial->setPortName(settings_ptr->getName());
    serial->setBaudRate(settings_ptr->getBoudeRate());
    serial->setDataBits(settings_ptr->getDataBits());
    serial->setParity(settings_ptr->getParity());
    serial->setStopBits(settings_ptr->getStopBits());

    if (serial->open(QIODevice::ReadWrite)){
        statusBar->setMessageBar("Подключено к " + settings_ptr->getName());

        ui->connect->setEnabled(false);
        ui->settings->setEnabled(false);
        ui->disconnect->setEnabled(true);
        m_timer->start(150);
        statusBar->clearReSent();
        getButton->setEnabled(true);
        m_transp->clearQueue();
        serial->readAll();
        ch1CheckBox->setEnabled(true);
        ch2CheckBox->setEnabled(true);
        ch2InCheckBox->setEnabled(true);
        ch3CheckBox->setEnabled(true);
        ch4CheckBox->setEnabled(true);
        ch4InCheckBox->setEnabled(true);
        autoSaveShotCheckBox->setEnabled(true);
        if(channelsOrder!=0){
            sendChannelOrder();
        }
        lazer1Spinbox->setEnabled(true);
        lazer2Spinbox->setEnabled(true);
        lazersSaveButton->setEnabled(true);
        borderLeftSpinbox->setEnabled(true);
        borderRightSpinbox->setEnabled(true);
        compCH1Spinbox->setEnabled(true);
        compCH2Spinbox->setEnabled(true);

        ch1CheckBox->setChecked(false);
        ch2CheckBox->setChecked(false);
        ch2InCheckBox->setChecked(false);
        ch3CheckBox->setChecked(false);
        ch4CheckBox->setChecked(false);
        ch4InCheckBox->setChecked(false);
    }
    else{
         statusBar->setMessageBar("Невозможно подключиться COM-порту");
    }
}
void MainWindow::on_disconnect_triggered(){

    if(serial->isOpen()){
        serial->close();
        statusBar->setMessageBar("Отключено от " + settings_ptr->getName());
        emit m_slip->serialPortClosed();
    }
    else {
        statusBar->setMessageBar("Невозможно отключиться от COM-порта");
    }

    getButton->setEnabled(false);
    ui->connect->setEnabled(true);
    ui->settings->setEnabled(true);
    ui->disconnect->setEnabled(false);

    m_timer->stop();
    m_online=false;

    emit statusUpdate(m_online);
    countRecievedDots=0;
    countAvaibleDots=0;
    notYetFlag=0;
    statusBar->setDownloadBarValue(0);
    emit dataReadyUpdate(-1);
    currentShot.clear();

    autoGetCheckBox->setCheckState(Qt::Unchecked);          //Вырубаем автополучение на всякий

    ch1CheckBox->setEnabled(false);
    ch2CheckBox->setEnabled(false);
    ch2InCheckBox->setEnabled(false);
    ch3CheckBox->setEnabled(false);
    ch4CheckBox->setEnabled(false);
    ch4InCheckBox->setEnabled(false);

    autoSaveShotCheckBox->setEnabled(false);
    autoSaveShotCheckBox->setCheckState(Qt::Unchecked);
    lazer1Spinbox->setEnabled(false);
    lazer2Spinbox->setEnabled(false);
    lazersSaveButton->setEnabled(false);
    borderLeftSpinbox->setEnabled(false);
    borderRightSpinbox->setEnabled(false);
    compCH1Spinbox->setEnabled(false);
    compCH2Spinbox->setEnabled(false);

}

//Управление лазером
void MainWindow::sendLazer1(int lazer1Par){
    QByteArray data;
    char msb,lsb;
    data.append(LAZER1_SET);
    msb=(0&0xFF00)>>8;
    lsb=static_cast<char> (lazer1Par&0x00FF);
    data.append(msb);
    data.append(lsb);
    m_console->putData("Set Lazer1 Setting: ");
    m_transp->sendPacket(data);
}
void MainWindow::sendLazer2(int lazer2Par){
    QByteArray data;
    char msb,lsb;
    data.append(LAZER2_SET);
    msb=(0&0xFF00)>>8;
    lsb=static_cast<char> (lazer2Par&0x00FF);
    data.append(msb);
    data.append(lsb);

    m_console->putData("Set Lazer2 Setting: ");
    m_transp->sendPacket(data);
}
void MainWindow::sendSaveEeprom()
{
    QByteArray data;
    data.append(LAZERS_SAVE);
    m_console->putData("Save lazer's parameters to EEPROM: ");
    m_transp->sendPacket(data);
}

void MainWindow::sendBorderLeft(int leftBorderVal)
{
    QByteArray data;
    char msb,lsb;
    data.append(LEFT_BORDER_SET);
    msb=(0&0xFF00)>>8;
    lsb=static_cast<char> (leftBorderVal&0x00FF);
    data.append(msb);
    data.append(lsb);
    m_console->putData("Set left border: ");
    m_transp->sendPacket(data);
}

void MainWindow::sendBorderRight(int rightBorderVal)
{
    QByteArray data;
    char msb,lsb;
    data.append(RIGHT_BORDER_SET);
    msb=(0&0xFF00)>>8;
    lsb=static_cast<char> (rightBorderVal&0x00FF);
    data.append(msb);
    data.append(lsb);
    m_console->putData("Set left border: ");
    m_transp->sendPacket(data);
}

void MainWindow::sendCompCH1(int compCH1Val)
{
    QByteArray data;
    char msb,lsb;
    data.append(COMP_CH1_SET);
    msb=(0&0xFF00)>>8;
    lsb=static_cast<char> (compCH1Val&0x00FF);
    data.append(msb);
    data.append(lsb);
    m_console->putData("Set left border: ");
    m_transp->sendPacket(data);
}

void MainWindow::sendCompCH2(int compCH2Val)
{
    QByteArray data;
    char msb,lsb;
    data.append(COMP_CH2_SET);
    msb=(0&0xFF00)>>8;
    lsb=static_cast<char> (compCH2Val&0x00FF);
    data.append(msb);
    data.append(lsb);
    m_console->putData("Set left border: ");
    m_transp->sendPacket(data);
}

//Подсчет количества отмеченных каналов
void MainWindow::incCountCh(int ch)
{

    switch (ch){
    case 1:
       if(ch1CheckBox->isChecked())
           channelsOrder|=0x01;
       else
          channelsOrder&=~0x01;
        break;
    case 2:
       if(ch2CheckBox->isChecked())
           channelsOrder|=0x02;
       else
          channelsOrder&=~0x02;
        break;
    case 3:
       if(ch3CheckBox->isChecked())
           channelsOrder|=0x04;
       else
          channelsOrder&=~0x04;
        break;
    case 4:
       if(ch4CheckBox->isChecked())
           channelsOrder|=0x08;
       else
          channelsOrder&=~0x08;
        break;
    }
    chCountChecked=0;
    for (int i=0;i<4;i++){
        if(channelsOrder&(1<<i))
            chCountChecked++;
    }
    if(chCountChecked)
         autoGetCheckBox->setEnabled(true);
    else
        autoGetCheckBox->setEnabled(false);
    sendChannelOrder();
}

//Настройка разбиения данных на пакеты
void MainWindow::setPacketSize(unsigned short n){
    packetSize=n;
}

//Запрость у MCU пакет длины n с канала ch
void MainWindow::getPacketFromMCU(unsigned short n)
{
    QByteArray data;
    char msb,lsb;
    data.append(REQUEST_POINTS);
    msb=static_cast<char> ((n&0xFF00)>>8);
    lsb=static_cast<char> (n&0x00FF);
    data.append(msb);
    data.append(lsb);
    m_transp->sendPacket(data);
}
void MainWindow::autoGetCheckBoxChanged(int st)
{
    if(st){
        ch1CheckBox->setEnabled(false);
        ch2CheckBox->setEnabled(false);
        ch3CheckBox->setEnabled(false);
        ch4CheckBox->setEnabled(false);
        getButton->setEnabled(false);
    }
    else {
        ch1CheckBox->setEnabled(true);
        ch2CheckBox->setEnabled(true);
        ch3CheckBox->setEnabled(true);
        ch4CheckBox->setEnabled(true);
        getButton->setEnabled(true);
    }
}

/////////////////////////////////////////////////////ОСНОВНЫЕ МЕТОДЫ///////////////////////////////////////////////////////////
//Запрос канала, отмеченного в чекбоксах chXCheckBox
//Метод вызывается в трех случаях:
//1. Нажата кнопка "Получить точки".
//2. Отмечено "Автополучение по готовноси"
//3. Запрос следующего выбранного канала
//Последовательность отправки знает плата. Клиент только отправляет однотипные зарпосы
//В этом случае счетчик notYetFlag устанавливается по количеству отмеченных каналов (1 или 2)
//отправляется первый запрос. Плата отвечает тем, что считает нужным.
//После успешного приема первого канала, notYetFlag уменьшается на единицу и если отмечено 2 канала, то происходит повторный запрос
void MainWindow::manualGetShotButton(){
    statusBar->setMessageBar("");
    if(countAvaibleDots){
        if (!ch1CheckBox->isChecked() && !ch2CheckBox->isChecked()
            && !ch3CheckBox->isChecked() && !ch4CheckBox->isChecked()){
            statusBar->setMessageBar("ОШИБКА!, Не выбрано ни одного канала!");
            return;
        }
        m_timer->stop();
        getButton->setEnabled(false);                                                   //Защита от двойного нажатия кнопки
        m_console->putData("REQUEST_POINTS: ");
        countWaitingDots = countAvaibleDots;                                             //Запоминаем, сколько точек всего придет в одном канале                                                                           //заправшиваем новую пачку
        statusBar->setDownloadBarRange(countAvaibleDots);
        statusBar->setDownloadBarValue(0);
        if(notYetFlag == 0)
            notYetFlag = chCountChecked;
        while (countAvaibleDots>0){                                                                          //Отправляем запрос несоклько раз по packetSize точек.
            getPacketFromMCU(countAvaibleDots>packetSize?packetSize:countAvaibleDots);
            countAvaibleDots-=packetSize;
        }
        notYetFlag--;
    }
    else
         statusBar->setMessageBar("ОШИБКА!, Данные не готовы для получения!");
}

//Обработка входящих пакетов
void MainWindow::handlerTranspAnswerReceive(QByteArray &bytes) {
    unsigned char cmd = static_cast<unsigned char>(bytes[0]);
    unsigned short value = static_cast<unsigned short>(bytes[1])*256+static_cast<unsigned short>(bytes[2]);
    int dataReady=-1;
    QString chName;
    switch(cmd){
    case ASK_MCU:                                                           //Пришел ответ, mcu жив
        if (value == OK) {
            m_online = true;
            m_console->putData(" :RECIEVED ANSWER_MCU\n\n");
            emit statusUpdate(m_online);
        }
        else{
            statusBar->setMessageBar("Error: Wrong ASK_MCU ansver message!");
            m_console->putData("Error: Wrong ASK_MCU ansver message!\n\n");
        }
        break;


    case REQUEST_STATUS:                                                                //Пришло количество точек
        m_console->putData(" :RECIEVED ANSWER_STATUS\n\n");
        countAvaibleDots=value;
        if (value != NO_DATA_READY) {
            dataReady = value;
            if(autoGetCheckBox->isChecked() || notYetFlag){                       //Если включен автозапрос данных или не вычитали все пачку каналов
                manualGetShotButton();                                                  //Запрашиваем шот
            }
        }
        else {
            dataReady = 0;
            m_console->putData("Warning: MCU has no data\n\n");
        }
        emit dataReadyUpdate(dataReady);
        m_timer->start();                                                              //Если получили статус, то можно запрашивать еще
        break;


     case REQUEST_POINTS:
        if ((value==CH1)|| (value==CH2) || (value==CH3) || (value==CH4)){                                                //Если пришли точки по одному из каналов, то обрабатываем
            bytes.remove(0, 3);                                                         //Удалили 3 байта (команду и значение)
            //Забираем 16 байт метаданных
            tempPLISextremums2.clear();
            tempPLISextremums1.clear();
            for(int i=0;i<8;i+=2){
                tempPLISextremums2.prepend(static_cast <unsigned char> (bytes.at(i+1))*256+static_cast <unsigned char>(bytes.at(i))+20);
                tempPLISextremums1.prepend(static_cast <unsigned char> (bytes.at(i+9))*256+static_cast <unsigned char>(bytes.at(i+8))+20);
            }
            bytes.remove(0, 16);
            err1->display(static_cast <unsigned char> (bytes.at(1))*256+static_cast <unsigned char>(bytes.at(0)));
            err2->display(static_cast <unsigned char> (bytes.at(3))*256+static_cast <unsigned char>(bytes.at(2)));

            lazer1Spinbox->setValue(static_cast <unsigned char> (bytes.at(5))*256+static_cast <unsigned char>(bytes.at(4)));
            lazer2Spinbox->setValue(static_cast <unsigned char> (bytes.at(7))*256+static_cast <unsigned char>(bytes.at(6)));
            borderLeftSpinbox->setValue(static_cast <unsigned char> (bytes.at(9))*256+static_cast <unsigned char>(bytes.at(8)));
            borderRightSpinbox->setValue(static_cast <unsigned char> (bytes.at(11))*256+static_cast <unsigned char>(bytes.at(10)));
            compCH1Spinbox->setValue(static_cast <unsigned char> (bytes.at(13))*256+static_cast <unsigned char>(bytes.at(12)));
            compCH2Spinbox->setValue(static_cast <unsigned char> (bytes.at(15))*256+static_cast <unsigned char>(bytes.at(14)));
            bytes.remove(0, 12);

            countRecievedDots+=bytes.count();                                           //Считаем, сколько уже пришло
            statusBar->setDownloadBarValue(countRecievedDots);                              //Прогресс бар апгрейд
            currentShot.append(bytes);                                                  //Добавляем в шот данные, которые пришли
            if(countRecievedDots>=countWaitingDots){                                  //Приняли канал целиком
                //Кладем принятый шот в соответствующий мап
                if (value == CH1){
                     if(shotsCH1.contains(shotCountRecieved)) {                         //Если в мапе уже есть запись с текущим индексом пачки
                         shotCountRecieved++;                                           //Начинаем следующую пачку
                         qDebug() << "Attantion! Dublicate CH1";
                     }
                    chName="CH1_NF";
                    shotsCH1.insert(shotCountRecieved,currentShot);                                     //Добавили пришедший канал в мап с текущим индексом
                    m_console->putData(" :RECIEVED ANSWER_POINTS CH1_NF  ");
                    chCountRecieved++;                                                  //Получили канал

                    if(ch2InCheckBox->isChecked()) {                                                      //Если нужна фильтрация
                        QByteArray filtered = filter->toFilter(currentShot,currentShot.size());          //Получаем фильтрованный массив
                        shotsCH2In.insert(shotCountRecieved,filtered);
                    }
                }
                else if(value == CH2){
                    if(shotsCH2.contains(shotCountRecieved)) {                         //Если в мапе уже есть запись с текущим индексом пачки
                        shotCountRecieved++;                                           //Начинаем следующую пачку
                        qDebug() << "Attantion! Dublicate CH2";
                    }
                   chName="CH2_NF";
                   currentShot=currentShot.mid(20);                                 //Смещение отфильтрованного сигнала из плисы
                   currentShot.append(20,0);
                   shotsCH2.insert(shotCountRecieved,currentShot);                                     //Добавили пришедший канал в мап с текущим индексом
                   m_console->putData(" :RECIEVED ANSWER_POINTS CH1_F  ");
                   chCountRecieved++;
                }
                else if(value == CH3){
                     if(shotsCH3.contains(shotCountRecieved)) {
                         shotCountRecieved++;
                         qDebug() << "Attantion! Dublicate CH3";
                     }
                     chName="CH2_NF";
                     shotsCH3.insert(shotCountRecieved,currentShot);
                     m_console->putData(" :RECIEVED ANSWER_POINTS CH2_NF  ");
                     chCountRecieved++;                                                  //Получили канал
                     if(ch4InCheckBox->isChecked()){                                                      //Если нужна фильтрация
                        QByteArray filtered =filter->toFilter(currentShot,currentShot.size());          //Получаем фильтрованный массив
                        shotsCH4In.insert(shotCountRecieved,filtered);                                    //Добавляем его на график
                     }
                }
                else if(value == CH4){
                     if(shotsCH4.contains(shotCountRecieved)) {
                         shotCountRecieved++;
                         qDebug() << "Attantion! Dublicate CH4";
                     }
                     chName="CH2_F";
                     currentShot=currentShot.mid(20);                                 //Смещение отфильтрованного сигнала из плисы
                     currentShot.append(20,0);
                     shotsCH4.insert(shotCountRecieved,currentShot);
                     m_console->putData(" :RECIEVED ANSWER_POINTS CH2_F  ");
                     chCountRecieved++;                                                  //Получили канал
                }

                //Если включено автосохранение в файл
                if(autoSaveShotCheckBox->isChecked())
                    writeToLogfile(chName);
                //Обнуляем всякое
                countRecievedDots=0;                                                    //Обнуляем количество пришедших точек
                currentShot.clear();                                                    //Чистим временное хранилище текущего принимаемого канала
                statusBar->setInfo(m_transp->getQueueCount());                          //Обновляем статус бар

                if (chCountRecieved == chCountChecked){                                 //Если приняли все заправшиваемые каналы                                                  //Все точки всех отмеченных каналов приняты
                    m_console->putData("\n\n");
                    chCountRecieved=0;
                    shotCountRecieved++;                                                //Увеличиваем счетчик пачек
                    shotsComboBox->addItem(QString::number(shotCountRecieved-1));
                    //shotsComboBox->setCurrentIndex(shotCountRecieved-1);
                    shotsComboBox->setCurrentIndex(shotsComboBox->count()-1);
                    if (!autoGetCheckBox->isChecked())                                  //Если не стоит автополучение, то можно разблокировать кнопку
                        getButton->setEnabled(true);
                }
                m_timer->start();                                                       //Стартуем таймер опроса статуса
            }
        }

        else if(value == NO_DATA_READY){                              //Точки по какой-то причин не готовы. Это может случиться только если точки были запрошены вручную, игнорируя статус данных
            QMessageBox::critical(nullptr,"Ошибка!","Данные не готовы для получения!");
            m_console->putData("Warning: MCU has no data\n\n");
            getButton->setEnabled(true);
        }
        else{
            statusBar->setMessageBar("Error: Wrong REQUEST_POINTS ansver message!");
            m_console->putData("Warning: MCU has no data\n\n");
            getButton->setEnabled(true);
        }
        break;
   }
}

//Выбрать шот из списка
void MainWindow::selectShot(int index){
    if(!shotsCH1.isEmpty() || !shotsCH2.isEmpty() ||!shotsCH3.isEmpty() ||!shotsCH4.isEmpty()){
        QByteArray ch;
        int shotNum = shotsComboBox->currentText().toInt();
        viewer->clearGraphs(ShotViewer::AllCH);
        //Первый канал
        if(shotsCH1.contains(shotNum)){
            ch = shotsCH1[shotNum];
            viewer->addUserGraph(ch,ch.size(),1);


        }
        if(shotsCH2.contains(shotNum)){
            ch = shotsCH2[shotNum];
            viewer->addUserGraph(ch,ch.size(),2);
            viewer->addLines(tempPLISextremums1,1,1);
             viewer->addLines(QVector<double>{static_cast<double>(borderLeftSpinbox->value()),static_cast<double>(10800-borderRightSpinbox->value())},1,3);
            viewer->addLines2(QVector<double>{static_cast<double>(compCH1Spinbox->value())},1,3);
        }
        if(shotsCH2In.contains(shotNum)){
            ch = shotsCH2In[shotNum];
            viewer->addUserGraph(ch,ch.size(),2);
            QVector<QVector<unsigned int>> dots = filter->extrFind2(ch,ch.size());
            viewer->addDots(dots,1);
            QVector <unsigned int> xDots;
            for (int i = 0;i<4;i++){
                xDots.append(dots.at(i).at(0));
            }

            shadowsCh1 = filter->shadowFind(xDots);
            if(tempPLISextremums1.size()==4)
                shadowsCh1Plis = filter->shadowFind(tempPLISextremums1);//Расчет теней на основании экстремумов из плисины
            viewer->addLines(shadowsCh1,1,2);
            leftShadow1Label->setText("   Лев. тень: " +QString::number(shadowsCh1.at(0)));
            rightShadow1Label->setText("   Прав. тень: " +QString::number(shadowsCh1.at(1)));
        }

        //Второй канал
        if(shotsCH3.contains(shotNum)){
            ch = shotsCH3[shotNum];
            viewer->addUserGraph(ch,ch.size(),3);


        }
        if(shotsCH4.contains(shotNum)){
            ch = shotsCH4[shotNum];
            viewer->addUserGraph(ch,ch.size(),4);
            viewer->addLines(tempPLISextremums2,2,1);   //найденные в плисине экстремумы.
            viewer->addLines(QVector<double>{static_cast<double>(borderLeftSpinbox->value()),static_cast<double>(10800-borderRightSpinbox->value())},2,3);
            viewer->addLines2(QVector<double>{static_cast<double>(compCH2Spinbox->value())},2,3);
        }
        if(shotsCH4In.contains(shotNum)){
            ch = shotsCH4In[shotNum];
            viewer->addUserGraph(ch,ch.size(),4);
            QVector<QVector<unsigned int>> dots = filter->extrFind2(ch,ch.size());
            viewer->addDots(dots,2);
            QVector <unsigned int> xDots;
            for (int i = 0;i<4;i++){
                xDots.append(dots.at(i).at(0));
            }
            shadowsCh2 = filter->shadowFind(xDots);
            if(tempPLISextremums2.size()==4)
                shadowsCh2Plis = filter->shadowFind(tempPLISextremums2);//Расчет теней на основании экстремумов из плисины
            viewer->addLines(shadowsCh2,2,2);
            leftShadow2Label->setText("   Лев. тень: " + QString::number(shadowsCh2.at(0)));
            rightShadow2Label->setText("   Прав. тень: " + QString::number(shadowsCh2.at(1)));

        }
        if(shadowsCh1.size()>1 && shadowsCh2.size()>1){
            diameter = filter->diameterFind(shadowsCh1,shadowsCh2);
            diametrLabel->setText("Диаметр: " +QString::number(diameter.at(0)));
            m_centerViewer->setCoord(static_cast<int>(diameter.at(1)),static_cast<int>(diameter.at(2)));
            centerPositionLabel->setText("Смещение: " + QString::number(diameter.at(1)) + ", " + QString::number(diameter.at(2)));

        }
        if(shadowsCh1Plis.size()>1 && shadowsCh1Plis.size()>1){
            diameter = filter->diameterFind(shadowsCh1,shadowsCh2);
            diameterPlis = filter->diameterFind(shadowsCh1Plis,shadowsCh2Plis);
            diametrLabel->setText("Диаметр: " +QString::number(diameter.at(0)));
            diametrPlisLabel->setText("Диаметр ПЛИС: " +QString::number(diameterPlis.at(0)));
            m_centerViewer->setCoord(static_cast<int>(diameter.at(1)),static_cast<int>(diameter.at(2)));
            centerPositionLabel->setText("Смещение: " + QString::number(diameter.at(1)) + ", " + QString::number(diameter.at(2)));

        }

    }
}


//Показать консоль
void MainWindow::consoleEnabledCheked(bool en){
    if(en){
        m_console->show();
    }
    else{
        m_console->clear();
        m_console->hide();
    }
}

//Автосохранение
void MainWindow::autoSaveShotCheked(bool en)
{
    if(en){
        if(ch1CheckBox->isChecked()){
            filename = QDate::currentDate().toString("yyyy_MM_dd") + QTime::currentTime().toString("__hh_mm_ss")+"_CH1";
            file1->setFileName(dirname + "/" + filename);
            file1->open(QIODevice::WriteOnly);
        }
        if(ch2CheckBox->isChecked()){
            filename = QDate::currentDate().toString("yyyy_MM_dd") + QTime::currentTime().toString("__hh_mm_ss")+"_CH1F";
            file2->setFileName(dirname + "/" + filename);
            file2->open(QIODevice::WriteOnly);
        }

        if(ch3CheckBox->isChecked()){
            filename = QDate::currentDate().toString("yyyy_MM_dd") + QTime::currentTime().toString("__hh_mm_ss")+"_CH2";
            file3->setFileName(dirname + "/" + filename);
            file3->open(QIODevice::WriteOnly);
        }
        if(ch4CheckBox->isChecked()){
            filename = QDate::currentDate().toString("yyyy_MM_dd") + QTime::currentTime().toString("__hh_mm_ss")+"_CH2F";
            file4->setFileName(dirname + "/" + filename);
            file4->open(QIODevice::WriteOnly);
        }
    }
    else{
       if(file1->isOpen())
        file1->close();
       if(file2->isOpen())
        file2->close();
       if(file3->isOpen())
        file3->close();
       if(file4->isOpen())
        file4->close();
    }
}

//Очистить список
void MainWindow::on_clearButton(){
    shotCountRecieved=0;
    shotsCH1.clear();
    shotsCH2.clear();
    shotsCH3.clear();
    shotsCH4.clear();
    shotsComboBox->clear();
    viewer->clearGraphs(ShotViewer::AllCH);
}

//Обработка ошибок SLIP
void MainWindow::handlerTranspError() {
    emit on_disconnect_triggered();                         //Отключаемся   
}
//Слот на сигнал от m_transp, что произошла повторная отправка
void MainWindow::reSentInc(){
    statusBar->incReSent();
}
// Обработчик таймаута опроса состояние MCU (1 сек)
void MainWindow::handlerTimer() {
    statusBar->setInfo(m_transp->getQueueCount());
    QByteArray data;
    if (m_online) {      
        data.append(REQUEST_STATUS);
        m_console->putData("SEND REQUEST_STATUS: ");
        m_transp->sendPacket(data);
        m_timer->stop();                                                        //Избегать многократного запроса статуса, если не пришел ответ на первый
    }
    else {
        if (serial->isOpen()) {           
            data.append(ASK_MCU);
            m_console->putData("SEND ASK_MCU: ");
            m_transp->sendPacket(data);
        }
    }
}

void MainWindow::sendChannelOrder(){
    //Отправка последовательности байт в плату
    QByteArray data;
    char msb,lsb;
    data.append(static_cast<char> (CH_ORDER));
    m_console->putData("SEND CH_ORDER: ");

    msb=static_cast<char> ((channelsOrder&0xFF00)>>8);
    lsb=static_cast<char> (channelsOrder&0x00FF);
    data.append(msb);
    data.append(lsb);
    m_transp->sendPacket(data);
}

void MainWindow::writeToLogfile(QString name)
{
    if (dirname.isEmpty()){
        QMessageBox::critical(nullptr,"Ошибка!","Директория для сохранения данных отсутствует!");
    }
    else{
        if(name=="CH1_NF"){
            file1->write(currentShot,currentShot.size());
            file1->write(endShotLine,endShotLine.size());   //Чтобы отличать шот друг от друга
            file1->flush();
        }
        else if(name=="CH1_F"){
            file2->write(currentShot,currentShot.size());
            file2->write(endShotLine,endShotLine.size());
            file2->flush();
        }
        else if(name=="CH2_NF"){
            file3->write(currentShot,currentShot.size());
            file3->write(endShotLine,endShotLine.size());
            file3->flush();
        }
        else if(name=="CH2_F"){
            file4->write(currentShot,currentShot.size());
            file4->write(endShotLine,endShotLine.size());
            file4->flush();
        }
    }
}

void MainWindow::fillTable(QCPGraphDataContainer &dataMap)
{
    QCPGraphDataContainer::const_iterator begin = dataMap.begin();
    QCPGraphDataContainer::const_iterator end = dataMap.end();
    m_table->setRowCount(0);

    for (QCPGraphDataContainer::const_iterator it=begin; it!=end; ++it)
    {
        m_table->setRowCount(m_table->rowCount()+1);
        m_table->setItem(m_table->rowCount() - 1, 0, new QTableWidgetItem(QString("%1").arg(it->key)));
        m_table->setItem(m_table->rowCount() - 1, 1, new QTableWidgetItem(QString("%1").arg(it->value)));

    }
    for(int i=tableSize;i<m_table->rowCount();i++)
        m_table->hideRow(i);
}



void MainWindow::on_ShdowSet_triggered()
{
    ShadowSettings->updateSettingsStruct();
    ShadowSettings->fillFileads();
    ShadowSettings->show();
}
