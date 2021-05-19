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

    transmitGroup = new QGroupBox("Обмен данными");
    appSettingsGroup = new QGroupBox("Настройки интерфейса");
    logGroup = new QGroupBox("Логирование");
    historyGrouop = new QGroupBox("История");

    transmitGroup->setMinimumWidth(100);
    appSettingsGroup->setMinimumWidth(100);
    logGroup->setMinimumWidth(100);
    historyGrouop->setMinimumWidth(100);

    controlLayout->addWidget(transmitGroup);
    controlLayout->addWidget(appSettingsGroup);
    controlLayout->addWidget(logGroup);
    controlLayout->addWidget(historyGrouop);

    transmitLayout = new QVBoxLayout;
    appSettingsLayout = new QVBoxLayout;
    logLayout = new QVBoxLayout;
    historyLayout = new QVBoxLayout;
    transmitGroup->setLayout(transmitLayout);
    appSettingsGroup->setLayout(appSettingsLayout);
    logGroup->setLayout(logLayout);
    historyGrouop->setLayout(historyLayout);

    //Настройки передачи
    packetSizeLabel = new QLabel("Размер пакета:");
    packetSizeSpinbox = new QSpinBox;
    ch1CheckBox = new QCheckBox("Канал 1. Фильтрованный");
    ch2CheckBox = new QCheckBox("Канал 1. Нефильтрованный");
    ch3CheckBox = new QCheckBox("Канал 2. Фильтрованный");
    ch4CheckBox = new QCheckBox("Канал 2. Нефильтрованный");
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
    //m_spacer= new QSpacerItem(1,1, QSizePolicy::Fixed, QSizePolicy::Expanding);
    //controlLayout->addSpacerItem(m_spacer);

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

    //Создание папки с логами, если ее нет.
    QDir dir(dirname);
    if (!dir.exists()) {
        QDir().mkdir(dirname);
    }

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
        if(channelsOrder!=0){
            sendChannelOrder();
        }
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
    }
    else {
        ch1CheckBox->setEnabled(true);
        ch2CheckBox->setEnabled(true);
        ch3CheckBox->setEnabled(true);
        ch4CheckBox->setEnabled(true);
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
            viewer->addUserGraph(ch,ch.size(),2);
        }
        if(shotsCH3.contains(index)){
            ch = shotsCH3[index];
            viewer->addUserGraph(ch,ch.size(),3);
        }
        if(shotsCH4.contains(index)){
            ch = shotsCH4[index];
            viewer->addUserGraph(ch,ch.size(),4);
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
                        chName="CH1_F";
                        shotsCH1.insert(shotCountRecieved,currentShot);                    //Добавили пришедший канал в мап с текущим индексом
                        m_console->putData(" :RECIEVED ANSWER_POINTS CH1  ");
                     }
                }
                else if(value == CH2){
                     if(shotsCH2.contains(shotCountRecieved)) {
                        qDebug() << "Attantion! Dublicate CH2";
                     }
                     else{
                         chName="CH1_NF";
                         shotsCH2.insert(shotCountRecieved,currentShot);
                         m_console->putData(" :RECIEVED ANSWER_POINTS CH2  ");
                     }
                }
                else if(value == CH3){
                     if(shotsCH3.contains(shotCountRecieved)) {
                        qDebug() << "Attantion! Dublicate CH3";
                     }
                     else{
                         chName="CH2_F";
                         shotsCH3.insert(shotCountRecieved,currentShot);
                         m_console->putData(" :RECIEVED ANSWER_POINTS CH3  ");
                     }
                }
                else if(value == CH4){
                     if(shotsCH4.contains(shotCountRecieved)) {
                        qDebug() << "Attantion! Dublicate CH4";
                     }
                     else{
                         chName="CH2_NF";
                         shotsCH4.insert(shotCountRecieved,currentShot);
                         m_console->putData(" :RECIEVED ANSWER_POINTS CH4  ");
                     }
                }
                //Если включено автосохранение
                if(autoSaveShotCheckBox->isChecked()){
                    if (dirname.isEmpty()){
                        QMessageBox::critical(nullptr,"Ошибка!","Директория для сохранения данных отсутствует!");
                    }
                    else{
                        filename=QString::number(shotCountRecieved) + "_" + chName;
                        file.setFileName(dirname + "/" + filename + ".txt");
                        if(file.open(QIODevice::WriteOnly) == true){
                            file.write(currentShot);
                            file.close();
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


