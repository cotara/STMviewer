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

    m_timer = new QTimer();
    connect(m_timer, &QTimer::timeout, this, &MainWindow::handlerTimer);

    viewer = new ShotViewer();

    //Консоль
    m_console = new Console;
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
    layoutV = new QVBoxLayout;
    centralWidget()->setLayout(layoutV);

    layoutH = new QHBoxLayout;
    layoutV->addLayout(layoutH);
    layoutV->addWidget(m_console);

    controlLayout = new QVBoxLayout;
    graphsLayout = new QVBoxLayout;

    layoutH->addWidget(viewer);
    layoutH->addLayout(controlLayout);

    lazerGroup = new QGroupBox("Настройка лазеров");
    transmitGroup = new QGroupBox("Обмен данными");
    signalProcessingGroup = new QGroupBox("Обработка сигнала");
    appSettingsGroup = new QGroupBox("Настройки интерфейса");
    logGroup = new QGroupBox("Логирование");
    historyGrouop = new QGroupBox("История");

    lazerGroup->setMaximumWidth(250);
    transmitGroup->setMaximumWidth(250);
    signalProcessingGroup->setMaximumWidth(250);
    appSettingsGroup->setMaximumWidth(250);
    logGroup->setMaximumWidth(250);
    historyGrouop->setMaximumWidth(250);

    controlLayout->addWidget(lazerGroup);
    controlLayout->addWidget(transmitGroup);
    controlLayout->addWidget(signalProcessingGroup);
    controlLayout->addWidget(appSettingsGroup);
    controlLayout->addWidget(logGroup);
    controlLayout->addWidget(historyGrouop);

    lazerLayout = new QVBoxLayout;
    transmitLayout = new QVBoxLayout;
    signalProcessingLayout = new QVBoxLayout;
    appSettingsLayout = new QVBoxLayout;
    logLayout = new QVBoxLayout;
    historyLayout = new QVBoxLayout;

    lazerGroup->setLayout(lazerLayout);
    transmitGroup->setLayout(transmitLayout);
    signalProcessingGroup->setLayout(signalProcessingLayout);
    appSettingsGroup->setLayout(appSettingsLayout);
    logGroup->setLayout(logLayout);
    historyGrouop->setLayout(historyLayout);

    //Настройки лазера
    lazersHorizontalLayout = new QHBoxLayout;
    lazerLayout->addLayout(lazersHorizontalLayout);
    lazer1SettingLayout = new QVBoxLayout;
    lazer2SettingLayout = new QVBoxLayout;
    lazersHorizontalLayout->addLayout(lazer1SettingLayout);
    lazersHorizontalLayout->addLayout(lazer2SettingLayout);
    lazer1Spinbox = new QSpinBox();
    lazer2Spinbox = new QSpinBox();
    lazer1Label = new QLabel("Лазер 1:");
    lazer2Label = new QLabel("Лазер 2:");
    lazersSaveButton = new QPushButton("Сохранить в EEPROM");

    lazer1Spinbox->setRange(0,255);
    lazer2Spinbox->setRange(0,255);
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
    packetSizeLabel = new QLabel("Размер пакета:");
    packetSizeSpinbox = new QSpinBox;
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
    ch1CheckBox->setEnabled(false);
    ch2CheckBox->setEnabled(false);
    ch3CheckBox->setEnabled(false);
    ch4CheckBox->setEnabled(false);
    transmitLayout->addWidget(getButton);
    transmitLayout->addWidget(autoGetCheckBox);

    packetSizeSpinbox->setRange(50,15000);
    packetSizeSpinbox->setValue(11000);
    connect(packetSizeSpinbox, QOverload<int>::of(&QSpinBox::valueChanged),
         [=](short i){
            setPacketSize(i);});
    emit packetSizeSpinbox->valueChanged(packetSizeSpinbox->value());

    connect(ch1CheckBox,&QCheckBox::stateChanged,this,&MainWindow::incCountCh);
    connect(ch2CheckBox,&QCheckBox::stateChanged,this,&MainWindow::incCountCh);
    connect(ch3CheckBox,&QCheckBox::stateChanged,this,&MainWindow::incCountCh);
    connect(ch4CheckBox,&QCheckBox::stateChanged,this,&MainWindow::incCountCh);

    getButton->setEnabled(false);
    connect(getButton,&QPushButton::clicked,this, &MainWindow::manualGetShotButton);

    autoGetCheckBox->setEnabled(false);
    connect(autoGetCheckBox,&QCheckBox::stateChanged,this, &MainWindow::autoGetCheckBoxChanged);

    //Обработка сигнала
    shiftCH1NF_Slider = new QSlider(Qt::Horizontal);
    shiftCH2NF_Slider = new QSlider(Qt::Horizontal);
    shiftedCH2Label = new QLabel("Смещение фильтрованного канала 1: 0");
    shiftedCH4Label = new QLabel("Смещение фильтрованного канала 2: 0");
    shiftCH1NF_Slider->setTickPosition(QSlider::TicksBothSides);
    shiftCH2NF_Slider->setTickPosition(QSlider::TicksBothSides);
    shiftCH1NF_Slider->setTickInterval(100);
    shiftCH2NF_Slider->setTickInterval(100);
    shiftCH1NF_Slider->setRange(-250,250);
    shiftCH2NF_Slider->setRange(-250,250);
    signalProcessingLayout->addWidget(shiftedCH2Label);
    signalProcessingLayout->addWidget(shiftCH1NF_Slider);
    signalProcessingLayout->addWidget(shiftedCH4Label);
    signalProcessingLayout->addWidget(shiftCH2NF_Slider);

    connect(shiftCH1NF_Slider, &QSlider::valueChanged,this,&MainWindow::shiftCH2);
    connect(shiftCH2NF_Slider, &QSlider::valueChanged,this,&MainWindow::shiftCH4);

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

}

MainWindow::~MainWindow(){
    delete ui;
    delete settings_ptr;
    delete serial;
    delete m_transp;
    delete m_timer;
    delete viewer;
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

void MainWindow::sendLazer1(int lazer1Par){
    QByteArray data;
    data.append(LAZER1_SET);
    data.append(lazer1Par);
    m_console->putData("Set Lazer1 Setting: ");
    m_transp->sendPacket(data);
}
void MainWindow::sendLazer2(int lazer2Par){
    QByteArray data;
    data.append(LAZER2_SET);
    data.append(lazer2Par);
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
void MainWindow::incCountCh(bool st){
    if(st) chCountChecked++;
    else   chCountChecked--;

    if(chCountChecked){
        autoGetCheckBox->setEnabled(true);
    }
    else{
        autoGetCheckBox->setEnabled(false);
    }
    channelsOrder=0;

    if(ch1CheckBox->isChecked())
        channelsOrder|=0x01;
    if(ch2CheckBox->isChecked())
        channelsOrder|=0x02;
    if(ch3CheckBox->isChecked())
        channelsOrder|=0x04;
    if(ch4CheckBox->isChecked())
        channelsOrder|=0x08;

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

//Запихиваем в очередь Х запросов в соответствии с разбивкой по пакетам, установленной в спинбоксе
void MainWindow::manualGetShotButton(){
    if(countAvaibleDots){
        if (!ch1CheckBox->isChecked() && !ch2CheckBox->isChecked()
            && !ch3CheckBox->isChecked() && !ch4CheckBox->isChecked()){
            statusBar->setMessageBar("ОШИБКА!, Не выбрано ни одного канала!");
            return;
        }
        getButton->setEnabled(false);                                                   //Защита от двойного нажатия кнопки
        m_console->putData("REQUEST_POINTS: ");
        m_timer->stop();
        countWaitingDots = countAvaibleDots;                                                                //Запоминаем, сколько точек всего придет в одном канале                                                                           //заправшиваем новую пачку
        statusBar->setDownloadBarRange(countAvaibleDots);                                //Сколько точек по всем каналам
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
        filename = QDate::currentDate().toString("yyyy_MM_dd") + QTime::currentTime().toString("__hh_mm_ss")+"_CH1";
        file1->setFileName(dirname + "/" + filename);
        file1->open(QIODevice::WriteOnly);


        filename = QDate::currentDate().toString("yyyy_MM_dd") + QTime::currentTime().toString("__hh_mm_ss")+"_CH1F";
        file2->setFileName(dirname + "/" + filename);
        file2->open(QIODevice::WriteOnly);


        filename = QDate::currentDate().toString("yyyy_MM_dd") + QTime::currentTime().toString("__hh_mm_ss")+"_CH2";
        file3->setFileName(dirname + "/" + filename);
        file3->open(QIODevice::WriteOnly);


        filename = QDate::currentDate().toString("yyyy_MM_dd") + QTime::currentTime().toString("__hh_mm_ss")+"_CH2F";
        file4->setFileName(dirname + "/" + filename);
        file4->open(QIODevice::WriteOnly);

    }
    else{
       file1->close();
       file2->close();
       file3->close();
       file4->close();
    }
}

//Выбрать шот из списка
void MainWindow::selectShot(int index){
    if(!shotsCH1.isEmpty() || !shotsCH2.isEmpty() ||!shotsCH3.isEmpty() ||!shotsCH4.isEmpty()){
        QByteArray ch;

        viewer->clearGraphs(ShotViewer::Both);
        if(shotsCH1.contains(index)){
            ch = shotsCH1[index];
            viewer->addUserGraph(ch,ch.size(),1);
        }
        if(shotsCH2.contains(index)){
            ch = shotsCH2[index];
            shiftCH2((shiftedCH2));
            //viewer->addUserGraph(ch,ch.size(),2);
        }
        if(shotsCH3.contains(index)){
            ch = shotsCH3[index];
            viewer->addUserGraph(ch,ch.size(),3);
        }
        if(shotsCH4.contains(index)){
            ch = shotsCH4[index];
            shiftCH4((shiftedCH4));
            //viewer->addUserGraph(ch,ch.size(),4);
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
    viewer->clearGraphs(ShotViewer::Both);
}
//Сдвиг графика путем перемещения слайдера
void MainWindow::shiftCH2(int n){
    int index;
    QString str = "Смещение фильтрованного канала 1: " + QString::number(n);
    shiftedCH2Label->setText(str);
    shiftedCH2=n;
    QByteArray ch,chShifted;
     index=shotsComboBox->currentIndex();               //Двигать будем текущие отрисованные графики

     if(index!=-1){
        if(shotsCH2.contains(index)){
            ch = shotsCH2[index];
            if(n>=0){
                chShifted.fill(0,n);
                ch.truncate(ch.size()-n);
                chShifted.append(ch);
            }
            else{                                       //Если двигаем влево
                chShifted=ch.mid(-n);                   //Берем с позиции n (минус т.к. n - отрицательное)
                chShifted.append(-n,0);
            }
            viewer->clearGraphs(ShotViewer::CH2_Only);//Очищаем график
            viewer->addUserGraph(chShifted,chShifted.size(),2); //Строим сдвинутый график
        }
     }
}

void MainWindow::shiftCH4(int n){
    int index;
    QString str = "Смещение фильтрованного канала 2: " + QString::number(n);
    shiftedCH4Label->setText(str);
    shiftedCH4=n;
    QByteArray ch,chShifted;
     index=shotsComboBox->currentIndex();
     if(index!=-1){
        if(shotsCH4.contains(index)){
            ch = shotsCH4[index];
            if(n>=0){
                chShifted.fill(0,n);
                ch.truncate(ch.size()-n);
                chShifted.append(ch);
            }
            else{
                chShifted=ch.mid(-n);
                chShifted.append(-n,0);
            }
            viewer->clearGraphs(ShotViewer::CH4_Only);
            viewer->addUserGraph(chShifted,chShifted.size(),4);
        }
     }
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
        if ((value==CH1)||(value==CH2)||(value==CH3)||(value==CH4)){                    //Если пришли точки по одному из каналов, то обрабатываем
            bytes.remove(0, 3);                                                         //Удалили 3 байта (команду и значение)
            countRecievedDots+=bytes.count();                                           //Считаем, сколько уже пришло
            statusBar->setDownloadBarValue(countRecievedDots);                              //Прогресс бар апгрейд
            currentShot.append(bytes);                                                  //Добавляем в шот данные, которые пришли
            if(countWaitingDots == countRecievedDots){                                  //Приняли канал целиком
                //Кладем принятый шот в соответствующий мап

                if (value == CH1){
                     if(shotsCH1.contains(shotCountRecieved)) {                         //Если в мапе уже есть запись с текущим индексом пачки
                        qDebug() << "Attantion! Dublicate CH1";
                     }
                     else{
                        chName="CH1_NF";
                        shotsCH1.insert(shotCountRecieved,currentShot);                    //Добавили пришедший канал в мап с текущим индексом
                        m_console->putData(" :RECIEVED ANSWER_POINTS CH1_NF  ");
                     }
                }
                else if(value == CH2){
                     if(shotsCH2.contains(shotCountRecieved)) {
                        qDebug() << "Attantion! Dublicate CH2";
                     }
                     else{
                         chName="CH1_F";
                         shotsCH2.insert(shotCountRecieved,filter->toFilter(currentShot,currentShot.size()));
                         m_console->putData(" :RECIEVED ANSWER_POINTS CH1_F  ");
                     }
                }
                else if(value == CH3){
                     if(shotsCH3.contains(shotCountRecieved)) {
                        qDebug() << "Attantion! Dublicate CH3";
                     }
                     else{
                         chName="CH2_NF";
                         shotsCH3.insert(shotCountRecieved,currentShot);
                         m_console->putData(" :RECIEVED ANSWER_POINTS CH2_NF  ");
                     }
                }
                else if(value == CH4){
                     if(shotsCH4.contains(shotCountRecieved)) {
                        qDebug() << "Attantion! Dublicate CH4";
                     }
                     else{
                         chName="CH2_F";
                         shotsCH4.insert(shotCountRecieved,filter->toFilter(currentShot,currentShot.size()));
                         m_console->putData(" :RECIEVED ANSWER_POINTS CH2_F  ");
                     }
                }
                //Если включено автосохранение
                if(autoSaveShotCheckBox->isChecked()){
                    if (dirname.isEmpty()){
                        QMessageBox::critical(nullptr,"Ошибка!","Директория для сохранения данных отсутствует!");
                    }
                    else{
                        if(chName=="CH1_NF"){
                            file1->write(currentShot,currentShot.size());
                            file1->write(endShotLine,endShotLine.size());
                            file1->flush();
                        }
                        else if(chName=="CH1_F"){
                            file2->write(currentShot,currentShot.size());
                            file2->write(endShotLine,endShotLine.size());
                            file2->flush();
                        }
                        else if(chName=="CH2_NF"){
                            file3->write(currentShot,currentShot.size());
                            file3->write(endShotLine,endShotLine.size());
                            file3->flush();
                        }
                        else if(chName=="CH2_F"){
                            file4->write(currentShot,currentShot.size());
                            file4->write(endShotLine,endShotLine.size());
                            file4->flush();
                        }
                    }
                }
                countRecievedDots=0;                                                    //Обнуляем коилчество пришедших точек
                currentShot.clear();                                                    //Чистим временное хранилище текущего принимаемого канала
                statusBar->setInfo(m_transp->getQueueCount());
                if (notYetFlag == 0){                                                       //Все точки всех отмеченных каналов приняты
                    m_console->putData("\n\n");
                    shotCountRecieved++;                                                    //Увеличиваем счетчик пачек
                    shotsComboBox->addItem(QString::number(shotCountRecieved));
                    shotsComboBox->setCurrentIndex(shotCountRecieved-1);
                    if (!autoGetCheckBox->isChecked())
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


