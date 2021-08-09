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
    graphsLayout = new QVBoxLayout();

    layoutH->addWidget(viewer);
    layoutH->addLayout(controlLayout);

    lazerGroup = new QGroupBox("Настройка лазеров");
    transmitGroup = new QGroupBox("Обмен данными");
    resultGroup = new QGroupBox("Результаты расчетов");
    appSettingsGroup = new QGroupBox("Настройки интерфейса");
    logGroup = new QGroupBox("Логирование");
    historyGrouop = new QGroupBox("История");

    lazerGroup->setMaximumWidth(250);
    transmitGroup->setMaximumWidth(250);
    resultGroup->setMaximumWidth(250);
    appSettingsGroup->setMaximumWidth(250);
    logGroup->setMaximumWidth(250);
    historyGrouop->setMaximumWidth(250);

    controlLayout->addWidget(lazerGroup);
    controlLayout->addWidget(transmitGroup);
    controlLayout->addWidget(resultGroup);
    controlLayout->addWidget(appSettingsGroup);
    controlLayout->addWidget(logGroup);
    controlLayout->addWidget(historyGrouop);

    lazerLayout = new QVBoxLayout();
    transmitLayout = new QVBoxLayout();
    resultLayout = new QVBoxLayout();
    appSettingsLayout = new QVBoxLayout();
    logLayout = new QVBoxLayout();
    historyLayout = new QVBoxLayout();

    lazerGroup->setLayout(lazerLayout);
    transmitGroup->setLayout(transmitLayout);
    resultGroup->setLayout(resultLayout);
    appSettingsGroup->setLayout(appSettingsLayout);
    logGroup->setLayout(logLayout);
    historyGrouop->setLayout(historyLayout);

    //Настройки лазера
    lazersHorizontalLayout = new QHBoxLayout();
    lazerLayout->addLayout(lazersHorizontalLayout);
    lazer1SettingLayout = new QVBoxLayout();
    lazer2SettingLayout = new QVBoxLayout();
    lazersHorizontalLayout->addLayout(lazer1SettingLayout);
    lazersHorizontalLayout->addLayout(lazer2SettingLayout);
    lazer1Spinbox = new QSpinBox();
    lazer2Spinbox = new QSpinBox();
    lazer1Label = new QLabel("Лазер 1:");
    lazer2Label = new QLabel("Лазер 2:");
    lazersSaveButton = new QPushButton("Сохранить в EEPROM");

    lazer1Spinbox->setRange(20,50);
    lazer2Spinbox->setRange(20,50);
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
          [=](int i){MainWindow::sendLazer1(i);});
    connect(lazer2Spinbox, QOverload<int>::of(&QSpinBox::valueChanged),
          [=](int i){MainWindow::sendLazer2(i);});
    connect(lazersSaveButton,&QPushButton::clicked,this,&MainWindow::sendSaveEeprom);
    //Настройки передачи
    packetSizeLabel = new QLabel("Размер пакета:",this);
    packetSizeSpinbox = new QSpinBox(this);
    ch1CheckBox = new QCheckBox("Канал 1. Нефильтрованный");
    ch2CheckBox = new QCheckBox("Канал 1. Фильтрованный");
    ch3CheckBox = new QCheckBox("Канал 2. Нефильтрованный");
    ch4CheckBox = new QCheckBox("Канал 2. Фильтрованный");
    getButton = new QPushButton("Получить снимок");
    autoGetCheckBox = new QCheckBox("Авто-получение по готовности");

    transmitLayout->addWidget(packetSizeLabel);
    transmitLayout->addWidget(packetSizeSpinbox);
    transmitLayout->addWidget(ch1CheckBox);
    transmitLayout->addWidget(ch2CheckBox);
    transmitLayout->addWidget(ch3CheckBox);
    transmitLayout->addWidget(ch4CheckBox);
    //ch1CheckBox->setEnabled(false);
    //ch2CheckBox->setEnabled(false);
    //ch3CheckBox->setEnabled(false);
    //ch4CheckBox->setEnabled(false);
    transmitLayout->addWidget(getButton);
    transmitLayout->addWidget(autoGetCheckBox);

    packetSizeSpinbox->setRange(50,15000);
    packetSizeSpinbox->setValue(11000);
    connect(packetSizeSpinbox, QOverload<int>::of(&QSpinBox::valueChanged),
         [=](short i){
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

    //Результат
    diametrLabel = new QLabel("Диаметр: ");
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

    //Fir filter
    filter = new firFilter;


    //constructorTest();
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


    //int shotNum=47;                                                             //Номер кадра в логе
    QFile *tempFile;                                                            //Файл лога
    tempFile = new QFile();

    filename = "2021_06_23__15_07_56_CH1";
    tempFile->setFileName(dirname + "/" + filename);
    if(!tempFile->open(QIODevice::ReadOnly)){
        qDebug() << "tempFile can`t be open";
        return;
    }
    QByteArray tempBuf = tempFile->readAll();                                           //Читаем большой буфер с несколькими кадрами
    QList<QByteArray> list_tempCH1=tempBuf.split(0xFF);                                    //разделяем кадры
    tempFile->close();

    filename = "2021_06_23__15_07_56_CH2";
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
            shotsCH2.insert(k,filter->toButterFilter(tempBuf2,tempBuf2.size()));
        }
        tempBuf2 = list_tempCH2.at(i);
        if(tempBuf2.size()>10){
            shotsCH3.insert(k,tempBuf2);
            //shotsCH4.insert(k,filter->toFilter(tempBuf2,tempBuf2.size()));
            shotsCH4.insert(k,filter->toButterFilter(tempBuf2,tempBuf2.size()));
        }
        k++;
        shotsComboBox->addItem(QString::number(i));
    }

/*

    viewer->addUserGraph(tempBuf2,tempBuf2.size(),1);
    tempBuf2 = filter->toFilter(tempBuf2,tempBuf2.size());
    viewer->addUserGraph(tempBuf2, tempBuf2.size(), 2);

    QVector<QVector<double>> dots = filter->extrFind(tempBuf2,tempBuf2.size());
    viewer->addDots(dots,1);
    if(dots.size()==6){
        QVector <double> xDots;
        for (int i = 0;i<6;i++){
            xDots.append(dots.at(i).at(0));
        }
        QVector<double> shadows = filter->shadowFind(xDots);
        viewer->addLines(shadows,1);
        leftShadow1Label->setText("   Лев. тень: " +QString::number(shadows.at(0)));
        rightShadow1Label->setText("   Прав. тень: " +QString::number(shadows.at(1)));
        QVector <double> tempVect;
        for(int i = 0;i<2;i++){
            if(shadows.at(i)>0){
                tempVect.append(shadows.at(i));
                tempVect.append((unsigned char)tempBuf2.at((int)shadows.at(i)));
                shadowsCh1.push_back(tempVect);
                tempVect.clear();
            }
        }
    }
    tempFile->close();


    filename = "2021_06_23__15_07_56_CH2";
    tempFile->setFileName(dirname + "/" + filename);
    if(!tempFile->open(QIODevice::ReadOnly)){
        qDebug() << "tempFile can`t be open";
        return;
    }

    tempBuf = tempFile->readAll();                                           //Читаем большой буфер с несколькими кадрами
    list_temp=tempBuf.split(0xFF);                                    //разделяем кадры
    tempBuf2 = list_temp.at(shotNum*2);                                              //Валидные - четные 0,2,4...

    viewer->addUserGraph(tempBuf2,tempBuf2.size(),3);
    tempBuf2 = filter->toFilter(tempBuf2,tempBuf2.size());
    viewer->addUserGraph(tempBuf2, tempBuf2.size(), 4);

    dots = filter->extrFind(tempBuf2,tempBuf2.size());
    viewer->addDots(dots,2);
    if(dots.size()==6){
        QVector <double> xDots;
        for (int i = 0;i<6;i++){
            xDots.append(dots.at(i).at(0));
        }
        QVector<double> shadows = filter->shadowFind(xDots);
        viewer->addLines(shadows,2);
        leftShadow2Label->setText("   Лев. тень: " +QString::number(shadows.at(0)));
        rightShadow2Label->setText("   Прав. тень: " +QString::number(shadows.at(1)));
        QVector <double> tempVect;
        for(int i = 0;i<2;i++){
            if(shadows.at(i)>0){
                tempVect.append(shadows.at(i));
                tempVect.append((unsigned char)tempBuf2.at((int)shadows.at(i)));
                shadowsCh2.push_back(tempVect);
                tempVect.clear();
            }
        }
    }

    tempFile->close();

    if(shadowsCh1.size()>1 && shadowsCh2.size()>1){
        diameter = filter->diameterFind(shadowsCh1,shadowsCh2);
        diametrLabel->setText("Диаметр: " +QString::number(diameter.at(0)));
    }
    */
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
        ch3CheckBox->setEnabled(true);
        ch4CheckBox->setEnabled(true);
        autoSaveShotCheckBox->setEnabled(true);
        if(channelsOrder!=0){
            sendChannelOrder();
        }
        lazer1Spinbox->setEnabled(true);
        lazer2Spinbox->setEnabled(true);
        lazersSaveButton->setEnabled(true);
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
    ch3CheckBox->setEnabled(false);
    ch4CheckBox->setEnabled(false);
    autoSaveShotCheckBox->setEnabled(false);
    autoSaveShotCheckBox->setCheckState(Qt::Unchecked);
    lazer1Spinbox->setEnabled(false);
    lazer2Spinbox->setEnabled(false);
    lazersSaveButton->setEnabled(false);
}
//Управление лазером
void MainWindow::sendLazer1(int lazer1Par){
    QByteArray data;
    char msb,lsb;
    data.append(LAZER1_SET);
    msb=(0&0xFF00)>>8;
    lsb=lazer1Par&0x00FF;
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
    lsb=lazer2Par&0x00FF;
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
//Подсчет количества отмеченных каналов
void MainWindow::incCountCh(int ch)
{
    chCountChecked++;
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
    if(chCountChecked)
         autoGetCheckBox->setEnabled(true);
    else
        autoGetCheckBox->setEnabled(false);

}

void MainWindow::incCountCh1(bool st){
    if(st){
        chCountChecked++;
        channelsOrder|=0x01;
        autoGetCheckBox->setEnabled(true);
    }
    else {
        chCountChecked--;
        channelsOrder&=~0x01;
        if(!chCountChecked)
            autoGetCheckBox->setEnabled(false);
    }
    sendChannelOrder();
}
void MainWindow::incCountCh2(bool st){
    if(st){
        chCountChecked++;
        channelsOrder|=0x04;
        autoGetCheckBox->setEnabled(true);
    }
    else {
        chCountChecked--;
        channelsOrder&=~0x04;
        if(!chCountChecked)
           autoGetCheckBox->setEnabled(false);
    }
    sendChannelOrder();
}
//Настройка разбиения данных на пакеты
void MainWindow::setPacketSize(short n){
    packetSize=n;
}


//Запрость у MCU пакет длины n с канала ch
void MainWindow::getPacketFromMCU(short n)
{
    QByteArray data;
    char msb,lsb;
    data.append(REQUEST_POINTS);
    msb=(n&0xFF00)>>8;
    lsb=n&0x00FF;
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

//Выбрать шот из списка
void MainWindow::selectShot(int index){
    if(!shotsCH1.isEmpty() || !shotsCH2.isEmpty() ||!shotsCH3.isEmpty() ||!shotsCH4.isEmpty()){
        QByteArray ch;

        viewer->clearGraphs(ShotViewer::AllCH);
        if(shotsCH1.contains(index)){
            ch = shotsCH1[index];
            viewer->addUserGraph(ch,ch.size(),1);
        }
        if(shotsCH2.contains(index)){
            ch = shotsCH2[index];
            viewer->addUserGraph(ch,ch.size(),2);
            QVector<QVector<double>> dots = filter->extrFind(ch,ch.size());                                 //Размерность результата либо 6 либо 0
            if(dots.size()==6){
                viewer->addDots(dots,1);
                QVector <double> xDots;
                for (int i = 0;i<6;i++){
                    xDots.append(dots.at(i).at(0));
                }
                QVector<double> shadows = filter->shadowFind(xDots);
                viewer->addLines(shadows,1);
                leftShadow1Label->setText("   Лев. тень: " +QString::number(shadows.at(0)));
                rightShadow1Label->setText("   Прав. тень: " +QString::number(shadows.at(1)));
                QVector <double> tempVect;
                shadowsCh1.clear();
                for(int i = 0;i<2;i++){
                  if(shadows.at(i)>0){
                    tempVect.append(shadows.at(i));
                    tempVect.append(ch.at((int)shadows.at(i)));
                    shadowsCh1.push_back(tempVect);
                    tempVect.clear();
                  }
                }

            }
        }
        if(shotsCH3.contains(index)){
            ch = shotsCH3[index];
            viewer->addUserGraph(ch,ch.size(),3);
        }
        if(shotsCH4.contains(index)){
            ch = shotsCH4[index];
            viewer->addUserGraph(ch,ch.size(),4);
            QVector<QVector<double>> dots = filter->extrFind(ch,ch.size());
            viewer->addDots(dots,2);
            if(dots.size()==6){
                QVector <double> xDots;
                for (int i = 0;i<6;i++){
                    xDots.append(dots.at(i).at(0));
                }
                QVector<double> shadows = filter->shadowFind(xDots);
                viewer->addLines(shadows,2);
                leftShadow2Label->setText("   Лев. тень: " +QString::number(shadows.at(0)));
                rightShadow2Label->setText("   Прав. тень: " +QString::number(shadows.at(1)));
                QVector <double> tempVect;
                shadowsCh2.clear();
                for(int i = 0;i<2;i++){
                    if(shadows.at(i)>0){
                        tempVect.append(shadows.at(i));
                        tempVect.append(ch.at((int)shadows.at(i)));
                        shadowsCh2.push_back(tempVect);
                        tempVect.clear();
                    }
                }
            }
        }
        if(shadowsCh1.size()>1 && shadowsCh2.size()>1){
            diameter = filter->diameterFind(shadowsCh1,shadowsCh2);
            diametrLabel->setText("Диаметр: " +QString::number(diameter.at(0)));
            m_centerViewer->setCoord(diameter.at(1),diameter.at(2));
            centerPositionLabel->setText("Смещение: " + QString::number(diameter.at(1)) + ", " + QString::number(diameter.at(2)));
        }

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


//Обработка входящих пакетов
void MainWindow::handlerTranspAnswerReceive(QByteArray &bytes) {
    unsigned char cmd = bytes[0];
    unsigned short value = (unsigned char)bytes[1]*256+(unsigned char)bytes[2];
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
        if ((value==CH1)||(value==CH3)){                                                //Если пришли точки по одному из каналов, то обрабатываем
            bytes.remove(0, 3);                                                         //Удалили 3 байта (команду и значение)
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
                    if(ch2CheckBox->isChecked()) {                                                      //Если нужна фильтрация
                        QByteArray filtered = filter->toFilter(currentShot,currentShot.size());          //Получаем фильтрованный массив
                        shotsCH2.insert(shotCountRecieved,filtered);                                    //Добавляем его на график
                    }
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
                     if(ch4CheckBox->isChecked()){                                                      //Если нужна фильтрация
                        QByteArray filtered =filter->toFilter(currentShot,currentShot.size());          //Получаем фильтрованный массив
                        shotsCH4.insert(shotCountRecieved,filtered);                                    //Добавляем его на график
                     }
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
                    shotsComboBox->addItem(QString::number(shotCountRecieved));
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
        }
        else{
            statusBar->setMessageBar("Error: Wrong REQUEST_POINTS ansver message!");
            m_console->putData("Warning: MCU has no data\n\n");
        }
        break;
   }
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
    data.append(CH_ORDER);
    m_console->putData("SEND CH_ORDER: ");

    msb=(channelsOrder&0xFF00)>>8;
    lsb=channelsOrder&0x00FF;
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


