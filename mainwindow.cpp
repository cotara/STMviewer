#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "serialsettings.h"
#include "qcustomplot/qcustomplot.h"
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

    //График
    customPlot1 = new QCustomPlot();
    customPlot2 = new QCustomPlot();

    //Настройка CustomPlot
    customPlot1->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables);
    customPlot1->axisRect()->setupFullAxesBox();
    customPlot2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables);
    customPlot2->axisRect()->setupFullAxesBox();
    //customPlot->plotLayout()->insertRow(0);
    //QCPTextElement *title = new QCPTextElement(customPlot, "Видеосигнал", QFont("sans", 17, QFont::Bold));
    //customPlot->plotLayout()->addElement(0, 0, title);

    //customPlot->xAxis->setLabel("X");
    //customPlot->yAxis->setLabel("Y");
    QFont legendFont = font();
    legendFont.setPointSize(10);

    customPlot1->xAxis->setRangeLower(0);
    customPlot1->xAxis->setRangeUpper(11000);
    customPlot1->yAxis->setRangeLower(-1);
    customPlot1->yAxis->setRangeUpper(260);
    customPlot1->legend->setVisible(true);
    customPlot1->legend->setFont(legendFont);
    customPlot1->legend->setSelectedFont(legendFont);
    customPlot1->legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items

    customPlot2->xAxis->setRangeLower(0);
    customPlot2->xAxis->setRangeUpper(11000);
    customPlot2->yAxis->setRangeLower(-1);
    customPlot2->yAxis->setRangeUpper(260);
    customPlot2->legend->setVisible(true);
    customPlot2->legend->setFont(legendFont);
    customPlot2->legend->setSelectedFont(legendFont);
    customPlot2->legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items


    // Выделение одной оси, ведет к выделению противоположной
    connect(customPlot1, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged1()));
    connect(customPlot2, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged2()));
    // зумируется только выделенная ось
    connect(customPlot1, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress1()));
    connect(customPlot1, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel1()));
    connect(customPlot2, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress2()));
    connect(customPlot2, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel2()));

    // При зумировании одной оси зизменяется диапазон противоположной
    connect(customPlot1->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot1->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot1->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot1->yAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot2->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot2->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot2->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot2->yAxis2, SLOT(setRange(QCPRange)));

    // connect some interaction slots:
    connect(customPlot1, SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)), this, SLOT(axisLabelDoubleClick(QCPAxis*,QCPAxis::SelectablePart)));
    connect(customPlot2, SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)), this, SLOT(axisLabelDoubleClick(QCPAxis*,QCPAxis::SelectablePart)));
    //connect(customPlot, SIGNAL(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)), this, SLOT(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*)));
    //connect(title, SIGNAL(doubleClicked(QMouseEvent*)), this, SLOT(titleDoubleClick(QMouseEvent*)));

    //connect slot that shows a message in the status bar when a graph is clicked:
    connect(customPlot1, SIGNAL(plottableClick(QCPAbstractPlottable*,int,QMouseEvent*)), this, SLOT(graphClicked(QCPAbstractPlottable*,int)));
    connect(customPlot2, SIGNAL(plottableClick(QCPAbstractPlottable*,int,QMouseEvent*)), this, SLOT(graphClicked(QCPAbstractPlottable*,int)));
    //Интерфейс
    layoutV = new QVBoxLayout;
    centralWidget()->setLayout(layoutV);

    layoutH = new QHBoxLayout;
    layoutV->addLayout(layoutH);
    layoutV->addWidget(m_console);

    controlLayout = new QVBoxLayout;
    graphsLayout = new QVBoxLayout;
    emptyArea = new QLabel;
    layoutH->addLayout(graphsLayout);
    layoutH->addLayout(controlLayout);

    graphsLayout->addWidget(customPlot1);
    graphsLayout->addWidget(customPlot2);
    graphsLayout->addWidget(emptyArea);
    emptyArea->show();
    customPlot1->hide();
    customPlot2->hide();
    customPlot1->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    customPlot2->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    emptyArea->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

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
    connect(autoRangeGraph,&QPushButton::clicked,this, &MainWindow::autoRangeGraphClicked);

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
    delete customPlot1;
    delete customPlot2;
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
        emptyArea->hide();
    }
    else{
        autoGetCheckBox->setEnabled(false);
        emptyArea->show();
    }

    channelsOrder=0;
    if(ch1CheckBox->isChecked() || ch2CheckBox->isChecked()){
        customPlot1->show();
        if(ch1CheckBox->isChecked())
            channelsOrder|=0x01;
        if(ch2CheckBox->isChecked())
            channelsOrder|=0x02;
    }
    else {
        customPlot1->hide();
        customPlot1->clearGraphs();
        customPlot1->replot();
    }
    if(ch3CheckBox->isChecked() || ch4CheckBox->isChecked()){
        customPlot2->show();
        if(ch3CheckBox->isChecked())
            channelsOrder|=0x04;
        if(ch4CheckBox->isChecked())
            channelsOrder|=0x08;
    }
    else {
        customPlot2->hide();
        customPlot2->clearGraphs();
        customPlot2->replot();
    }

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
            QMessageBox::critical(nullptr,"Ошибка!","Не выбрано ни одного канала для получения данных");
            return;
        }
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
         QMessageBox::critical(nullptr,"Ошибка!","Данные не готовы для получения!");
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

void MainWindow::autoRangeGraphClicked(){
    customPlot1->rescaleAxes();
    customPlot1->yAxis->setRange(customPlot1->yAxis->range().lower-1,customPlot1->yAxis->range().upper+5);
    customPlot1->xAxis->setRange(customPlot1->xAxis->range().lower,customPlot1->xAxis->range().upper );
    customPlot1->replot();
    customPlot2->rescaleAxes();
    customPlot2->yAxis->setRange(customPlot2->yAxis->range().lower-1,customPlot2->yAxis->range().upper+5);
    customPlot2->xAxis->setRange(customPlot2->xAxis->range().lower,customPlot2->xAxis->range().upper );
    customPlot2->replot();
}

//Выбрать шот из списка
void MainWindow::selectShot(int index){
    if(!shotsCH1.isEmpty() || !shotsCH2.isEmpty() ||!shotsCH3.isEmpty() ||!shotsCH4.isEmpty()){
        QByteArray ch;
        if(customPlot1->graphCount()!=0)
            customPlot1->clearGraphs();
        if(customPlot2->graphCount()!=0)
            customPlot2->clearGraphs();

        if(shotsCH1.contains(index)){
            ch = shotsCH1[index];
            addUserGraph(ch,ch.size(),1);
            customPlot1->show();
        }
        else customPlot1->hide();
        if(shotsCH2.contains(index)){
            ch = shotsCH2[index];
            addUserGraph(ch,ch.size(),2);
            customPlot1->show();
        }
        else customPlot1->hide();
        if(shotsCH3.contains(index)){
            ch = shotsCH3[index];
            addUserGraph(ch,ch.size(),3);
            customPlot2->show();
        }
        else customPlot2->hide();
        if(shotsCH4.contains(index)){
            ch = shotsCH4[index];
            addUserGraph(ch,ch.size(),4);
            customPlot2->show();
        }
        else customPlot2->hide();

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
    customPlot1->clearGraphs();
    customPlot1->replot();
    customPlot2->clearGraphs();
    customPlot2->replot();
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
                m_timer->start();                                                       //Стартуем таймер опроса статуса
                statusBar->setInfo(m_transp->getQueueCount());
                if (notYetFlag == 0){                                                       //Все точки всех отмеченных каналов приняты
                    m_console->putData("\n\n");
                    shotCountRecieved++;                                                    //Увеличиваем счетчик пачек
                    shotsComboBox->addItem(QString::number(shotCountRecieved));
                    shotsComboBox->setCurrentIndex(shotCountRecieved-1);
                }
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

/**************************************************/
/*                  customPlot                    */
/**************************************************/
void MainWindow::addUserGraph(QByteArray &buf, int len, int ch){
    QVector<double> x(len), y(len);
    QPen graphPen;
    QColor color;

    for (int i=0; i<len; i++){
      x[i] = i;
      y[i] = (unsigned char)buf.at(i);
    }
    if (ch==1 || ch==2){
        customPlot1->addGraph();
        customPlot1->graph()->setData(x, y);
        if (ch==1){
            customPlot1->graph()->setName(QString("Канал 1. Фильтрованный"));
            color =  QColorConstants::Black;
        }
        else{
            customPlot1->graph()->setName(QString("Канал 1. Нефильтрованный"));
            color =  QColorConstants::DarkGray;
        }
        graphPen.setColor(color);
        customPlot1->graph()->setPen(graphPen);
        customPlot1->replot();
    }
    else if(ch==3 || ch==4){
        customPlot2->addGraph();
        customPlot2->graph()->setData(x, y);
        if (ch==3){
            customPlot2->graph()->setName(QString("Канал 2. Фильтрованный"));
            color =  QColorConstants::DarkMagenta;
        }
        else{
            customPlot2->graph()->setName(QString("Канал 2. Нефильтрованный"));
            color =  QColorConstants::Red;
        }
        graphPen.setColor(color);
        customPlot2->graph()->setPen(graphPen);
        customPlot2->replot();
    }
}
void MainWindow::titleDoubleClick(QMouseEvent* event)
{
  Q_UNUSED(event)
  if (QCPTextElement *title = qobject_cast<QCPTextElement*>(sender()))
  {
    // Set the plot title by double clicking on it
    bool ok;
    QString newTitle = QInputDialog::getText(this, "QCustomPlot example", "New plot title:", QLineEdit::Normal, title->text(), &ok);
    if (ok)
    {
      title->setText(newTitle);
      //customPlot->replot();
    }
  }
}
void MainWindow::axisLabelDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part)
{
  // Set an axis label by double clicking on it
  if (part == QCPAxis::spAxisLabel) // only react when the actual axis label is clicked, not tick label or axis backbone
  {
    bool ok;
    QString newLabel = QInputDialog::getText(this, "QCustomPlot example", "New axis label:", QLineEdit::Normal, axis->label(), &ok);
    if (ok)
    {
      axis->setLabel(newLabel);
      //customPlot->replot();
    }
  }
}

void MainWindow::legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item){
  // Rename a graph by double clicking on its legend item
  Q_UNUSED(legend)
  if (item) // only react if item was clicked (user could have clicked on border padding of legend where there is no item, then item is 0)
  {
    QCPPlottableLegendItem *plItem = qobject_cast<QCPPlottableLegendItem*>(item);
    bool ok;
    QString newName = QInputDialog::getText(this, "QCustomPlot example", "New graph name:", QLineEdit::Normal, plItem->plottable()->name(), &ok);
    if (ok)
    {
      plItem->plottable()->setName(newName);
      //customPlot->replot();
    }
  }
}
void MainWindow::selectionChanged1(){
  // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (customPlot1->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || customPlot1->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      customPlot1->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || customPlot1->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    customPlot1->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    customPlot1->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
  // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (customPlot1->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || customPlot1->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      customPlot1->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || customPlot1->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    customPlot1->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    customPlot1->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
  // synchronize selection of graphs with selection of corresponding legend items:
  for (int i=0; i<customPlot1->graphCount(); ++i){
    QCPGraph *graph = customPlot1->graph(i);
    QCPPlottableLegendItem *item = customPlot1->legend->itemWithPlottable(graph);
    if (item->selected() || graph->selected())    {
      item->setSelected(true);
      graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
    }
  }
}

void MainWindow::selectionChanged2(){
  // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (customPlot2->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || customPlot2->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      customPlot2->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || customPlot2->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    customPlot2->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    customPlot2->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
  // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (customPlot2->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || customPlot2->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      customPlot2->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || customPlot2->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    customPlot2->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    customPlot2->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
  // synchronize selection of graphs with selection of corresponding legend items:
  for (int i=0; i<customPlot2->graphCount(); ++i){
    QCPGraph *graph = customPlot2->graph(i);
    QCPPlottableLegendItem *item = customPlot2->legend->itemWithPlottable(graph);
    if (item->selected() || graph->selected())    {
      item->setSelected(true);
      graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
    }
  }
}

void MainWindow::mousePress1(){
  // if an axis is selected, only allow the direction of that axis to be dragged
  // if no axis is selected, both directions may be dragged

  if (customPlot1->xAxis->selectedParts().testFlag(QCPAxis::spAxis)){
    customPlot1->axisRect()->setRangeDrag(customPlot1->xAxis->orientation());
    if(customPlot1->xAxis->range().lower <0)
         customPlot1->xAxis->setRangeLower(0);
    if(customPlot1->xAxis->range().upper > 11000)
         customPlot1->xAxis->setRangeUpper(11000);
  }
  else if (customPlot1->yAxis->selectedParts().testFlag(QCPAxis::spAxis)){
   customPlot1->axisRect()->setRangeDrag(customPlot1->yAxis->orientation());
   if(customPlot1->yAxis->range().lower <-1)
        customPlot1->yAxis->setRangeLower(-1);
   if(customPlot1->yAxis->range().upper >260)
        customPlot1->yAxis->setRangeUpper(260);
  }
  else{
    customPlot1->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
    if(customPlot1->xAxis->range().lower <0)
         customPlot1->xAxis->setRangeLower(0);
    if(customPlot1->xAxis->range().upper > 11000)
         customPlot1->xAxis->setRangeUpper(11000);
    if(customPlot1->yAxis->range().lower <-1)
         customPlot1->yAxis->setRangeLower(-1);
    if(customPlot1->yAxis->range().upper >260)
         customPlot1->yAxis->setRangeUpper(260);
  }
}
void MainWindow::mousePress2(){
  // if an axis is selected, only allow the direction of that axis to be dragged
  // if no axis is selected, both directions may be dragged

  if (customPlot2->xAxis->selectedParts().testFlag(QCPAxis::spAxis)){
    customPlot2->axisRect()->setRangeDrag(customPlot2->xAxis->orientation());
    if(customPlot2->xAxis->range().lower <0)
         customPlot2->xAxis->setRangeLower(0);
    if(customPlot2->xAxis->range().upper > 11000)
         customPlot2->xAxis->setRangeUpper(11000);
  }
  else if (customPlot2->yAxis->selectedParts().testFlag(QCPAxis::spAxis)){
   customPlot2->axisRect()->setRangeDrag(customPlot2->yAxis->orientation());
   if(customPlot2->yAxis->range().lower <-1)
        customPlot2->yAxis->setRangeLower(-1);
   if(customPlot2->yAxis->range().upper >260)
        customPlot2->yAxis->setRangeUpper(260);
  }
  else{
    customPlot2->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
    if(customPlot2->xAxis->range().lower <0)
         customPlot2->xAxis->setRangeLower(0);
    if(customPlot2->xAxis->range().upper > 11000)
         customPlot2->xAxis->setRangeUpper(11000);
    if(customPlot2->yAxis->range().lower <-1)
         customPlot2->yAxis->setRangeLower(-1);
    if(customPlot2->yAxis->range().upper >260)
         customPlot2->yAxis->setRangeUpper(260);
  }
}
void MainWindow::mouseWheel1(){
  // if an axis is selected, only allow the direction of that axis to be zoomed
  // if no axis is selected, both directions may be zoomed

  if (customPlot1->xAxis->selectedParts().testFlag(QCPAxis::spAxis)){
      customPlot1->axisRect()->setRangeZoom(customPlot1->xAxis->orientation());
      if(customPlot1->xAxis->range().lower <0)
           customPlot1->xAxis->setRangeLower(0);
      if(customPlot1->xAxis->range().upper > 11000)
           customPlot1->xAxis->setRangeUpper(11000);
   }

  else if (customPlot1->yAxis->selectedParts().testFlag(QCPAxis::spAxis)){
    customPlot1->axisRect()->setRangeZoom(customPlot1->yAxis->orientation());
    if(customPlot1->yAxis->range().lower <-1)
         customPlot1->yAxis->setRangeLower(-1);
    if(customPlot1->yAxis->range().upper >260)
         customPlot1->yAxis->setRangeUpper(260);
  }
  else{
    customPlot1->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
    if(customPlot1->xAxis->range().lower <0)
         customPlot1->xAxis->setRangeLower(0);
    if(customPlot1->xAxis->range().upper > 11000)
         customPlot1->xAxis->setRangeUpper(11000);
    if(customPlot1->yAxis->range().lower <-1)
         customPlot1->yAxis->setRangeLower(-1);
    if(customPlot1->yAxis->range().upper >260)
         customPlot1->yAxis->setRangeUpper(260);
   }
}

void MainWindow::mouseWheel2(){
  // if an axis is selected, only allow the direction of that axis to be zoomed
  // if no axis is selected, both directions may be zoomed

  if (customPlot2->xAxis->selectedParts().testFlag(QCPAxis::spAxis)){
      customPlot2->axisRect()->setRangeZoom(customPlot2->xAxis->orientation());
      if(customPlot2->xAxis->range().lower <0)
           customPlot2->xAxis->setRangeLower(0);
      if(customPlot2->xAxis->range().upper > 11000)
           customPlot2->xAxis->setRangeUpper(11000);
   }

  else if (customPlot2->yAxis->selectedParts().testFlag(QCPAxis::spAxis)){
    customPlot2->axisRect()->setRangeZoom(customPlot2->yAxis->orientation());
    if(customPlot2->yAxis->range().lower <-1)
         customPlot2->yAxis->setRangeLower(-1);
    if(customPlot2->yAxis->range().upper >260)
         customPlot2->yAxis->setRangeUpper(260);
  }
  else{
    customPlot2->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
    if(customPlot2->xAxis->range().lower <0)
         customPlot2->xAxis->setRangeLower(0);
    if(customPlot2->xAxis->range().upper > 11000)
         customPlot2->xAxis->setRangeUpper(11000);
    if(customPlot2->yAxis->range().lower <-1)
         customPlot2->yAxis->setRangeLower(-1);
    if(customPlot2->yAxis->range().upper >260)
         customPlot2->yAxis->setRangeUpper(260);
   }
}


void MainWindow::graphClicked(QCPAbstractPlottable *plottable, int dataIndex){
  // since we know we only have QCPGraphs in the plot, we can immediately access interface1D()
  // usually it's better to first check whether interface1D() returns non-zero, and only then use it.
  double dataValue = plottable->interface1D()->dataMainValue(dataIndex);
  QString message = QString("Clicked on graph '%1' at data point #%2 with value %3.").arg(plottable->name()).arg(dataIndex).arg(dataValue);
  ui->statusBar->showMessage(message, 2500);
}
