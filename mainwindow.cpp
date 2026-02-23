#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "transport\serialsettings.h"

#include "transport\slip.h"
#include "transport\transp.h"
#include "statusbar.h"
#include <QSplitter>

#include <QRandomGenerator>
#include <QtGlobal>
//#define TEST_MODE

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("LDMExplorer");
    ui->disconnect->setEnabled(false);


    settings_ptr = new SerialSettings(this);
    serial = new QSerialPort();

    m_timer = new QTimer(this);
    m_timer->setInterval(100);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::handlerTimer);

    //Логгирование
    log = new SaveLog(this);
    connect(log,&SaveLog::SaveToFolder,this,[=](QString &dirname){saveHistory(dirname);});

    //Консоль
    m_console = new Console(this);
    m_console->setMinimumWidth(150);
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

    //Виджеты интерфейса
    layoutV = new QVBoxLayout();
    centralWidget()->setLayout(layoutV);

    //Правая управления(настройки ПЛИС / настройка запросов / диаметры)
    m_ManagementWidget = new ManagementWidget(this);

    //Включаем интерфейс
    m_ManagementWidget->setEnabled(false);
    m_ManagementWidget->setMinimumWidth(300);

    m_windowSize = m_ManagementWidget->m_DiameterTransmition->windowSizeSpinbox->value();   //Окно медианного фильтра
    m_average  = m_ManagementWidget->m_DiameterTransmition->averageSpinbox->value();        //Усреднение медианного фильтра
    m_limit=m_ManagementWidget->m_DiameterTransmition->limitSpinbox->value();               //Лимит срабатывания медианного фильтра


    //Таблица
    m_table = new QTableWidget(this);
    m_table->setColumnCount(2);
    m_table->setRowCount(1);
    m_table->setShowGrid(true); // Включаем сетку
    m_table->setHorizontalHeaderLabels(QStringList{"X","Y"});
    m_table->horizontalHeader()->resizeSection(0, 50);//ширина
    m_table->horizontalHeader()->resizeSection(1, 50);
    m_table->hide();
    m_table->setMinimumWidth(150);

    //ТАБЫ
    m_tab = new QTabWidget(this);

    //Центральный виджет с графиками
    viewer = new ShotViewer(m_tab);
    connect(viewer,&ShotViewer::graph_selected,this,&MainWindow::fillTable);

    //Центральный виджет с диаметром и Фурьем
    d_viewer = new DiameterViewer(m_tab);
    d_viewer->setWindow(m_ManagementWidget->m_DiameterTransmition->xWindow->value()); //Сколько отображать точек

    //ТАБЫ
    m_tab->addTab(viewer, "Сигнал");
    m_tab->addTab(d_viewer, "Диаметр");
    m_tab->setTabBarAutoHide(true);
    connect(m_tab,&QTabWidget::currentChanged,this,[=](int index){
        if(index==0){
            m_ManagementWidget->m_plisSettings->setVisible(true);
            m_ManagementWidget->m_TransmitionSettings->setVisible(true);
            m_ManagementWidget->m_HistorySettings->setVisible(true);
            m_ManagementWidget->m_DiameterTransmition->setVisible(false);
        }
        else if(index == 1){
          m_ManagementWidget->m_plisSettings->setVisible(false);
          m_ManagementWidget->m_TransmitionSettings->setVisible(false);
          m_ManagementWidget->m_HistorySettings->setVisible(false);
          m_ManagementWidget->m_DiameterTransmition->setVisible(true);
        }
    });
    //ПРИНУДИТЕЛЬНО НАЖАТЬ ТАБ №1 emit m_tab->currentChanged(m_tab->currentIndex());

    //Разделители
    QWidget *container = new QWidget;
    QSplitter *splitterV = new QSplitter(Qt::Vertical, this);
    QSplitter *splitterH = new QSplitter(Qt::Horizontal, this);
    QWidget *rightPanel = new QWidget;
    QVBoxLayout *qVBoxLayout  = new QVBoxLayout();
    QVBoxLayout *rightPanelLayout  = new QVBoxLayout();

    splitterH->addWidget(m_tab);
    splitterH->addWidget(rightPanel);
    rightPanel->setLayout(rightPanelLayout);
    rightPanelLayout->addWidget(m_ManagementWidget);

    splitterH->addWidget(m_console);
    splitterH->addWidget(m_table);
    qVBoxLayout->addWidget(splitterH);
    container->setLayout(qVBoxLayout);

    splitterV->addWidget(container);
    splitterV->addWidget(m_console);
    layoutV->addWidget(splitterV);

    splitterH->setStretchFactor(0,15);
    splitterH->setStretchFactor(1,2);


    //Коннекты от Настроек ПЛИС
    connect(m_ManagementWidget->m_plisSettings,&PlisSettings::lazer1Send,this,[=](int i){sendByteToMK(LAZER1_SET,i,"Set Lazer1 Setting: ");});
    connect(m_ManagementWidget->m_plisSettings,&PlisSettings::lazer2Send,this,[=](int i){sendByteToMK(LAZER2_SET, i,"Set Lazer2 Setting: ");});
    connect(m_ManagementWidget->m_plisSettings,&PlisSettings::saveSend,this,[=]{sendByteToMK(LAZERS_SAVE, 0,"Save lazer's parameters to EEPROM: ");});//?? проверить работу

    connect(m_ManagementWidget->m_plisSettings,&PlisSettings::sendBorderLeft,this,[=](int i){sendByteToMK(LEFT_BORDER_SET, i,"Set left border: ");});
    connect(m_ManagementWidget->m_plisSettings,&PlisSettings::sendBorderRight,this,[=](int i){sendByteToMK(RIGHT_BORDER_SET, i,"Set right border: ");});
    connect(m_ManagementWidget->m_plisSettings,&PlisSettings::sendCompCH1,this,[=](int i){sendByteToMK(COMP_CH1_SET, i,"Set comp level CH1: ");});
    connect(m_ManagementWidget->m_plisSettings,&PlisSettings::sendCompCH2,this,[=](int i){sendByteToMK(COMP_CH2_SET, i,"Set comp level CH2: ");});
    connect(m_ManagementWidget->m_plisSettings,&PlisSettings::sendGreenOffset,this,[=](int i){sendByteToMK(OFFSET_GREEN_SET, i,"Set green offset: ");});
    connect(m_ManagementWidget->m_plisSettings,&PlisSettings::sendBlueOffset,this,[=](int i){sendByteToMK(OFFSET_BLUE_SET, i,"Set blue offset: ");});
    connect(m_ManagementWidget->m_plisSettings,&PlisSettings::sendMultyLaserMode,this,[=](int i){sendByteToMK(MULTY_LASER_MODE, i,"Set Mylty Laser Mode: ");});

    //Коннекты от параметров передачи
    connect(m_ManagementWidget->m_TransmitionSettings,&TransmitionSettings::chChooseChanged,this,&MainWindow::chOrderSend);
    connect(m_ManagementWidget->m_TransmitionSettings,&TransmitionSettings::getButtonClicked,this,&MainWindow::getButtonClicked);
    connect(m_ManagementWidget->m_TransmitionSettings, &TransmitionSettings::shiftChanged,this, [=](int ch, int val){ shiftFactor[ch] = val;});

    //Коннекты от истории
    connect(m_ManagementWidget->m_HistorySettings,&HistorySettings::shotSelected,this,&MainWindow::selectShot);
    connect(m_ManagementWidget->m_HistorySettings,&HistorySettings::saveHistory,log,&SaveLog::show);
    connect(m_ManagementWidget->m_HistorySettings,&HistorySettings::loadHistory,this,&MainWindow::loadHistory);
    connect(m_ManagementWidget->m_HistorySettings,&HistorySettings::clearHistory,this,&MainWindow::on_clearButton);

    //Коннекты от графика диаметра
    connect(m_ManagementWidget->m_DiameterTransmition,&DiameterTransmition::getDiameterChanged,this,[=](int state){
        if(state){
            m_timer->setInterval(1000/m_ManagementWidget->m_DiameterTransmition->reqFreqSpinbox->value());
            //m_timer->setInterval(1000);
            filled =0;
            d_viewer->clearGraphs();
        }
        else{
            m_timer->setInterval(100);
        }
    });
    connect(m_ManagementWidget->m_DiameterTransmition,&DiameterTransmition::xWindowChanged, this,[=](int value){ d_viewer->setWindow(value); });
    connect(m_ManagementWidget->m_DiameterTransmition,&DiameterTransmition::diameterModeChanged,this, [=](bool mode){ d_viewer->setDiamPlotMode(mode);});   //Изменен режим запроса диаметров
    connect(m_ManagementWidget->m_DiameterTransmition,&DiameterTransmition::windowSizeChanged, this,[=](int value){ m_windowSize = value;});   //Окно фильтра изменилось
    connect(m_ManagementWidget->m_DiameterTransmition,&DiameterTransmition::averageChanged, this,[=](int value){ m_average = value; });   //Усреднение изменилось
    connect(m_ManagementWidget->m_DiameterTransmition,&DiameterTransmition::furieLimitChanged, this,[=](int value){ m_furieLimit = value; });
    connect(m_ManagementWidget->m_DiameterTransmition,&DiameterTransmition::diameterCheckChanged, this,[=](int value){ d_viewer->setDiamEnPlot(value); });
    connect(m_ManagementWidget->m_DiameterTransmition,&DiameterTransmition::offsetsCheckChanged, this,[=](int value){ d_viewer->setOffsetsEnPlot(value); });
    connect(m_ManagementWidget->m_DiameterTransmition,&DiameterTransmition::filteredCheckChanged, this,[=](int value){ d_viewer->setFilteredEnPlot(value); });
    connect(m_ManagementWidget->m_DiameterTransmition,&DiameterTransmition::furieCheckChanged, this,[=](int value){ d_viewer->setFurieEnPlot(value); });

    //Тулбар
    tableSizeSpinbox = new QSpinBox(this);
    tableSizeSpinbox->setRange(1,1000);
    tableSizeSpinbox->setValue(tableSize);
    connect(tableSizeSpinbox, QOverload<int>::of(&QSpinBox::valueChanged),this,[=](int val){
        tableSize = val;

        if(m_table->rowCount()>=tableSize){
            for(int i=0;i<tableSize;i++)
                m_table->showRow(i);
            for(int i=tableSize;i<m_table->rowCount();i++)
                m_table->hideRow(i);
        }
    });
    tableSizeSpinbox->setVisible(false);

    tableSizeLabel = new QLabel("Размер таблицы",this);
    tableSizeLabel->setVisible(false);


    ui->ShowRightPanel->setChecked(true);

    //connect(ui->ShowMainControl,&QAction::toggled,this,[=](bool i){if(i) m_MainControlWidget->show(); else m_MainControlWidget->hide();});
    connect(ui->ShowRightPanel,&QAction::toggled,this,[=](bool i){if(i) rightPanel->show(); else rightPanel->hide();});


    connect(ui->showConsole,&QAction::toggled,this,[=](bool i){if(i) {m_console->show(); m_console->clearAll();} else m_console->hide();});
    connect(ui->TableShow,&QAction::toggled,this,[=](bool i){
       if(i) {
           m_table->show();
           //tableSizeSpinbox->setEnabled(true);
       } else {
           m_table->hide();
           //tableSizeSpinbox->setEnabled(false);
       }
    });

    connect(ui->AutoRange,&QAction::triggered,d_viewer,&DiameterViewer::autoScale);
    connect(ui->AutoRange,&QAction::triggered,viewer, &ShotViewer::autoScale);
    connect(ui->ShowRightPanel,&QAction::toggled,[=](bool i){if(i) m_ManagementWidget->show(); else m_ManagementWidget->hide();});

    //Пустой виджет, разделяющий кнопки на mainToolBar
   QWidget* empty = new QWidget();
   empty->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
   ui->mainToolBar->insertWidget(ui->showConsole,empty);




    //Создание папки с логами, если ее нет.
    dir = new QDir(dirnameDefault);
    if (!dir->exists()) {
        dir->mkdir(dirnameDefault);
    }
    dir->setFilter( QDir::NoDotAndDotDot);

   ShadowSettings = new SettingsShadowsFindDialog(this);
   connect(ShadowSettings, &SettingsShadowsFindDialog::settingsChanged,this,&MainWindow::settingsChanged);//Обновляем настройки в фильтре
   connect(ShadowSettings, &SettingsShadowsFindDialog::sendSettingsToMK,[=]{
       sendVectorToMK(NEWSHADFINDPAR,ShadowSettings->getShadowFindSettings(),"Новые параметры поиска диаметра отправлены в МК: ");//Засылаем настройки в МК
       sendByteToMK(REQUEST_MODEL,0,"\nSEND REQUEST_MODEL: ");                //Запрашиваем геометрические параметры
   });



   //Fir filter
   filter = new firFilter(ShadowSettings->getShadowFindSettings());//Инициализируем настройками из файла

   //Сбор данных для подбора коэффициентов
   connect(ShadowSettings->wizard->catchData,&catchDataDialog::buttonClicked,[=](int i){
       QVector<double> temp;
       temp = tempPLISextremums1 + tempPLISextremums2;
       if(diameterPlis.size()>0)
        temp.append(diameterPlis.at(0) + diameterPlis.at(1));
       ShadowSettings->wizard->catchData->setButtonPushed(temp,i);
   });

    m_ManagementWidget->m_plisSettings->setEnabled(false);
    QShortcut *shortcut = new QShortcut(QKeySequence("Ctrl+F5"), this);
    connect(shortcut, &QShortcut::activated, this, &MainWindow::onCtrlF5Pressed);

    for(int i=0;i<4;i++)
        shots.append(QMap<int,QByteArray>());

    //Отключаем
    ui->TableShow->setVisible(false);
    ui->showConsole->setVisible(false);
    ui->ShdowSet->setVisible(false);
}

MainWindow::~MainWindow(){
    delete ui;
    delete settings_ptr;
    delete serial;
    delete m_transp;
    delete m_timer;
    delete viewer;
}


//Открыть настройки связи
void MainWindow::on_settings_triggered(){
    settings_ptr->show();
}

//Переключение в девелопер мод
void MainWindow::toDeveloperMode()
{
    //Создаем компорт и настраиваем из полей в форме
    serial->setPortName(settings_ptr->getName());
    serial->setBaudRate(settings_ptr->getBoudeRate());
    serial->setDataBits(settings_ptr->getDataBits());
    serial->setParity(settings_ptr->getParity());
    serial->setStopBits(settings_ptr->getStopBits());

    //Открываем и собираем пакет
    if (serial->open(QIODevice::WriteOnly)){
        QByteArray command;
        command.append(char(settings_ptr->getSlaveAdd()));//Адрес
        command.append(char(0x06));//Функция
        command.append(char(0x00));//Адрес
        command.append(char(0x02));
        command.append(char(0x00));//Значение
        command.append(char(0x01));
        //Считаем CRC по 6 байтам
        int crc = 0xFFFF;
        for (int pos = 0; pos < 6; pos++) {
            crc ^= command[pos] & 0xFF;

            for (int i = 8; i != 0; i--) {
                if ((crc & 0x0001) != 0) {
                    crc >>= 1;
                    crc ^= 0xA001;
                } else
                    crc >>= 1;
            }
        }
        command.append(char(crc&0xFF));
        command.append(char((crc>>8)&0xFF));//CRC

        serial->write(command);           //Засылаем пакет
        //waitingD->waitingStart(3000);       //Запускаем анимацию

        if (serial->waitForBytesWritten(1000)) {}//блокирующая отправка с таймаутом 1 сек
        else
            QMessageBox::critical(this, "Ошибка!","Отправка команды завершилась неудачей!",QMessageBox::Ok);
        serial->close();
    }
    else
        QMessageBox::critical(this, "Ошибка!","Невозможно открыть указанный COM-порт!",QMessageBox::Ok);
}

//Нажата кнопка "ПОДКЛЮЧИТЬСЯ"
void MainWindow::on_connect_triggered()
{ 
    toDeveloperMode();

    //Настраиваем компорт
    serial->setPortName(settings_ptr->getName());
    serial->setBaudRate(460800);                    //Жестко вбиваем основную скорость обмена девелопер мода.
    serial->setDataBits(settings_ptr->getDataBits());
    serial->setParity(settings_ptr->getParity());
    serial->setStopBits(settings_ptr->getStopBits());

    if (serial->open(QIODevice::ReadWrite)){
        statusBar->setMessageBar("Подключено к " + settings_ptr->getName());

        ui->connect->setEnabled(false);
        ui->settings->setEnabled(false);
        ui->disconnect->setEnabled(true);

        statusBar->clearReSent();
        m_transp->clearQueue();
        serial->readAll();
        if(channelsOrder!=0)
            sendByteToMK(CH_ORDER,channelsOrder,"SEND CH_ORDER: ");

        //Включаем интерфейс
        m_ManagementWidget->setEnabled(true);

        //Запрашиваем модель
        sendByteToMK(REQUEST_MODEL,0,"\nSEND REQUEST_MODEL: ");
    }
    else{
         statusBar->setMessageBar("Невозможно подключиться COM-порту");
    }
}

//Нажата кнопка "ОТКЛЮЧИТЬСЯ"
void MainWindow::on_disconnect_triggered(){
    if(serial->isOpen()){
        serial->close();
        statusBar->setMessageBar("Отключено от " + settings_ptr->getName());
        emit m_slip->serialPortClosed();
    }
    else {
        statusBar->setMessageBar("Невозможно отключиться от COM-порта");
    }

    ui->connect->setEnabled(true);
    ui->settings->setEnabled(true);
    ui->disconnect->setEnabled(false);

    m_timer->stop();
    m_online=false;

    statusBar->setStatus(m_online);
    countRecievedDots=0;
    countAvaibleDots=0;
    notYetFlag=0;
    statusBar->setDownloadBarValue(0);
    statusBar->setDataReady(countAvaibleDots);

    //Вырубаем автополучение на всякий  
    m_ManagementWidget->m_TransmitionSettings->setGetButton(false);
    //Вырубаем интерфейс
    m_ManagementWidget->setEnabled(false);
}

//Отправка 1 байт в прибор
void MainWindow::sendByteToMK(char dst, int dataByte, const QString &msg)
{
    QByteArray data;
    char msb,lsb;
    if(serial->isOpen()){
        data.append(dst);
        msb= static_cast< char>((dataByte&0xFF00)>>8);
        lsb= static_cast< char>(dataByte&0x00FF);
        data.append(msb);
        data.append(lsb);
        m_console->putData(msg.toUtf8());
        m_transp->sendPacket(data);
    }
    else{
        QMessageBox::critical(nullptr,"Порт закрыт!","Невозможно отправить сообщение (" + msg + "). ");
        m_console->putData("Ошибка передачи сообщения : " + msg.toUtf8() + ". Порт закрыт.");
    }
}

//Отправить вектор Double в прибор
void MainWindow::sendVectorToMK(char dst, QVector<double> dataV, const QString &msg){
    conversation_t conv;
    QByteArray data;
    char msb,lsb;
     if(serial->isOpen()){
        data.append(dst);
        msb=(0&0xFF00)>>8;
        lsb=static_cast<char> (dataV.size()&0x00FF);
        data.append(msb);
        data.append(lsb);

        for(double d:dataV){
            conv.d=d;
            for(int i=0;i<8;i++)
            data.append(conv.ch[i]);
        }

        m_console->putData(msg.toUtf8());
        m_transp->sendPacket(data);
    }
    else{
        QMessageBox::critical(nullptr,"Порт закрыт!","Невозможно отправить сообщение (" + msg + "). ");
        m_console->putData("Ошибка передачи сообщения : " + msg.toUtf8() + ". Порт закрыт.");
    }
}

//Отправка комбинации отмеченных каналов
void MainWindow::chOrderSend(int ch){
    channelsOrder = ch;
    sendByteToMK(CH_ORDER,channelsOrder,"SEND CH_ORDER: ");
}

//Подсчет количества отмеченных каналов
int MainWindow::countCheckedCH(){
    int chCountChecked=0;
    for (int i=0;i<4;i++){
        if(channelsOrder&(1<<i))
            chCountChecked++;
    }
    return chCountChecked;
}

//Нажата кнопка "ПОЛУЧИТЬ СИГНАЛ"
void MainWindow::getButtonClicked(bool checked){
    if(checked){
        notYetFlag = countCheckedCH();                              //На старте передачи запоминаем сколько надо ждать каналов
        m_ManagementWidget->m_TransmitionSettings->setChEn(false);
    }
    else {
        if(notYetFlag==0)//Если передача всех каналов пачки завершена
            m_ManagementWidget->m_TransmitionSettings->setChEn(true);
    }
}

/////////////////////////////////////////////////////ОСНОВНЫЕ МЕТОДЫ///////////////////////////////////////////////////////////
// Обработчик таймаута отправки запросов
void MainWindow::handlerTimer() {
    statusBar->setInfo(m_transp->getQueueCount());
    QByteArray data;
    if (m_online) {
        if(notYetFlag){    //Хотим получать сигнал
            statusBar->setMessageBar("");
            if(countAvaibleDots){
                countWaitingDots = countAvaibleDots;                          //Запоминаем, сколько точек всего придет в одном канале                                                                           //заправшиваем новую пачку
                sendByteToMK(REQUEST_POINTS,countAvaibleDots, "REQUEST_POINTS: ");                                  //Отправляем запрос на точки
                notYetFlag--;
            }
            else
                statusBar->setMessageBar("ОШИБКА!, Данные не готовы для получения!");
        }
        else if(m_ManagementWidget->m_DiameterTransmition->isButtonChecked()){       //Если хотим запрашивать диаметр
            sendByteToMK(REQUEST_DIAMETER,0,"\nSEND REQUEST_DIAMETER: ");
        }
        else
            sendByteToMK(REQUEST_STATUS,0,"\nSEND REQUEST_STATUS: ");


    }
    else {
        if (serial->isOpen())
            sendByteToMK(ASK_MCU,0,"SEND ASK_MCU: ");
    }
    m_timer->stop();
}

//Обработка входящих пакетов
void MainWindow::handlerTranspAnswerReceive(QByteArray &bytes) {
    if(bytes.size()<3){
        m_timer->start();
        m_console->putData(" !!small Message!!!\n");
        return;
    }
    unsigned char cmd = static_cast<unsigned char>(bytes[0]);
    charToShort.ch[0]=bytes[2];
    charToShort.ch[1]=bytes[1];
    unsigned short value = charToShort.sh;
    bytes.remove(0, 3);                                                     //Удалили 3 байта (команду и значение)
    statusBar->setInfo(m_transp->getQueueCount());                          //Обновляем статус бар

    switch(cmd){
    case ASK_MCU:                                                           //Пришел ответ, mcu жив
        if (value == OK) {
            m_online = true;
            m_console->putData(" :RECIEVED ANSWER_MCU\n");
        }
        else{
            m_online = false;
            statusBar->setMessageBar("Error: Wrong ASK_MCU ansver message!");
            m_console->putData("Error: Wrong ASK_MCU ansver message!\n");
        }
        statusBar->setStatus(m_online);
        m_timer->start();
        break;

    case REQUEST_MODEL:
        m_console->putData(" :RECIEVED ANSWER_MODEL\n");
        ldmModel = value & 0xFF;
        ShadowSettings->ldmModel = ldmModel;
        m_ManagementWidget->m_resultWidget->setModel(ldmModel);
        for (int i = 0; i<6; i++){
            for (int j =0;j<8;j++)
                charToDouble.ch[j] =  bytes.at(j+i*8);
            ldmGeomParams[i] = charToDouble.d;
        }
        setWindowTitle("LDMExplorer (LDM" + QString::number(ldmModel) + ")");
        filter->updateSettings(ldmGeomParams);

        m_ManagementWidget->m_plisSettings->setEn200Mode(false);
        m_ManagementWidget->m_plisSettings->setEnOffsetsTool(true);
        switch (ldmModel){
         case 20:
            filter->setResolution(ldm20Res);
            shiftFactor[0] = 10;
            signalSize = 10800;
            break;
        case 35:
            wordLen = true;
            filter->setResolution(ldm20Res);
            shiftFactor[0] = 0;
            signalSize = 130;
            break;
        case 40:
           filter->setResolution(ldm20Res);
           shiftFactor[0] = 10;
           signalSize = 10800;
           break;
         case 50:
            filter->setResolution(ldm50Res);
            shiftFactor[0] = 40;
            signalSize = 7700;
            break;
        case 120:
            m_ManagementWidget->m_plisSettings->setEnOffsetsTool(false);
            filter->setResolution(ldm120Res);
            shiftFactor[0] = 0;
            signalSize = 10501;
            break;
         case 200:
            m_ManagementWidget->m_plisSettings->setEn200Mode(true);
            shiftFactor[0] = 0;
            signalSize = 10501;
        }
        viewer->rescaleX(0,signalSize);
        ShadowSettings->updateSettingsStructSlot(ldmGeomParams);
        ShadowSettings->filLabels(ldmGeomParams);
        m_timer->start();
        break;
    case REQUEST_STATUS:                                                                //Пришло количество точек
        m_console->putData(" :RECIEVED ANSWER_STATUS\n");

        if (value != NO_DATA_READY) {
            countAvaibleDots=value;

            //Забираем 16 байт метаданных
            tempPLISextremums1.clear();
            tempPLISextremums2.clear();
            for(int i=0;i<8;i+=2){
                charToShort.ch[0] = bytes.at(i);
                charToShort.ch[1] = bytes.at(i+1);
                tempPLISextremums2.prepend(charToShort.sh);
                charToShort.ch[0] = bytes.at(i+8);
                charToShort.ch[1] = bytes.at(i+9);
                tempPLISextremums1.prepend(charToShort.sh);
            }
            if( bytes.size()>=16) bytes.remove(0, 16);

            //16 байт - ошибки и значения параметров ПЛИС
            errorCh1 = bytes.at(0);
            errorCh2 = bytes.at(2);
            m_ManagementWidget->m_resultWidget->setError(bytes);   //Младшие значащие байты

            if( bytes.size()>=4) bytes.remove(0, 4);

            QVector<short> tempData;
            int dataSize=6;
            for(int i=0;i<dataSize;i++){
                charToShort.ch[0] = bytes.at(2*i);
                charToShort.ch[1] = bytes.at(2*i+1);
                tempData.append(charToShort.sh);
            }
            m_ManagementWidget->m_plisSettings->setButtonsData(tempData);
            if( bytes.size()>=dataSize*2) bytes.remove(0, dataSize*2);
            tempData.clear();

            //Средние и длительности
            if(ldmModel==200)   dataSize=12;
            else                dataSize=4;
            for(int i=0;i<dataSize;i++){
                charToShort.ch[0] = bytes.at(2*i);
                charToShort.ch[1] = bytes.at(2*i+1);
                tempData.append(charToShort.sh);
            }
            m_ManagementWidget->m_plisSettings->setLazersData(tempData);
            if( bytes.size()>=dataSize*2) bytes.remove(0, dataSize*2);
            tempData.clear();

            //Параметры зеленого и голубого оффсета
            dataSize=2;
            if(ldmModel==20 || ldmModel==40 || ldmModel==50){
                for(int i=0;i<dataSize;i++){
                    charToShort.ch[0] = bytes.at(2*i);
                    charToShort.ch[1] = bytes.at(2*i+1);
                    tempData.append(charToShort.sh);
                }
                m_ManagementWidget->m_plisSettings->setOffsetsData(tempData);
                if( bytes.size()>=dataSize*2) bytes.remove(0, dataSize*2);
                tempData.clear();
            }

            QVector<double> tempData2;
            tempData2.append(tempPLISextremums1);
            tempData2.append(tempPLISextremums2);


            if(tempPLISextremums1.size()==4){
                shadowsCh1Plis = filter->shadowFind(tempPLISextremums1);//Расчет теней на основании экстремумов из плисины
                tempData2.append(shadowsCh1Plis);
            }
            else{
                tempData2.append(0);
                tempData2.append(0);
            }
            if(tempPLISextremums2.size()==4){
                shadowsCh2Plis = filter->shadowFind(tempPLISextremums2);//Расчет теней на основании экстремумов из плисины
               tempData2.append(shadowsCh2Plis);
            }
            else{
                tempData2.append(0);
                tempData2.append(0);
            }

            if(shadowsCh1Plis.size()==2 && shadowsCh2Plis.size()==2){
                diameterPlis = filter->diameterFind(shadowsCh1Plis,shadowsCh2Plis);
                tempData2.append(diameterPlis.at(0));
                tempData2.append(diameterPlis.at(1));
            }

            //32 байт финальные радиусы и центры
            for (int i = 0; i<4; i++){
                for (int j =0;j<8;j++)
                    charToDouble.ch[j] =  bytes.at(j+i*8);
                finalDiamCenters[i] = charToDouble.d;
            }
            if(errorCh1==0)
                tempData2.append(finalDiamCenters.at(0));
            else
                tempData2.append(0);

            if(errorCh2==0)
                tempData2.append(finalDiamCenters.at(1));
            else
                tempData2.append(0);

            tempData2.append(finalDiamCenters.at(2));
            tempData2.append(finalDiamCenters.at(3));

            if( bytes.size()>=32) bytes.remove(0, 32);

            m_ManagementWidget->m_resultWidget->setData(tempData2);
            shadowsCh1Plis.clear();
            shadowsCh2Plis.clear();

            if(m_ManagementWidget->m_TransmitionSettings->getStatusGetButton())                 //Если нажата кнопка
                notYetFlag = countCheckedCH();                                                  //Запрашиваем шот
        }
        else {
            countAvaibleDots = 0;
            m_console->putData("Warning: MCU has no data\n");
        }
        statusBar->setDataReady(countAvaibleDots);
        m_timer->start();                                                              //Если получили статус, то можно запрашивать еще
        break;


      //Получили много экстремумов, по которым рассчитываем диаметры
     case REQUEST_DIAMETER:
        if(value!=0){
            m_console->putData(" :RECIEVED ANSWER_DIAMETER\n");

            r1FromMCU.clear();
            r2FromMCU.clear();
            c1FromMCU.clear();
            c2FromMCU.clear();
            shadowsFromMCU.clear();
            for(int i=0; i<bytes.size();i+=2){
                int num = static_cast<unsigned char>(bytes.at(i)) + static_cast<unsigned char>(bytes.at(i+1))*256;
                shadowsFromMCU.append(num);//Front1,Spad1,Front2,Spad2
            }

            for(int i = 0;i<shadowsFromMCU.size();i+=4){
                QVector<double> tempDiameters = filter->diameterFind(QVector<double>{shadowsFromMCU.at(i),shadowsFromMCU.at(i+1)},QVector<double>{shadowsFromMCU.at(i+2),shadowsFromMCU.at(i+3)});
                r1FromMCU.append(tempDiameters.at(0));
                r2FromMCU.append(tempDiameters.at(1));
                c1FromMCU.append(tempDiameters.at(2));
                c2FromMCU.append(tempDiameters.at(3));

            }
            m1FromMCU = filter->medianFilterX(r1FromMCU,m_windowSize,m_average,m_limit);
            m2FromMCU = filter->medianFilterY(r2FromMCU,m_windowSize,m_average,m_limit);
            d_viewer->addDataToGraph(r1FromMCU,r2FromMCU,c1FromMCU,c2FromMCU,m1FromMCU,m2FromMCU);
            m_ManagementWidget->m_DiameterTransmition->r1ValueLabel->setNum(m1FromMCU.last());
            m_ManagementWidget->m_DiameterTransmition->r2ValueLabel->setNum(m2FromMCU.last());
        }
        m_timer->start();//Продолжаем запрос статуса
        break;

    case REQUEST_POINTS:
        if ((value==CH1)|| (value==CH2) || (value==CH3) || (value==CH4)){                                                //Если пришли точки по одному из каналов, то обрабатываем
            countRecievedDots=bytes.size();                                           //Считаем, сколько уже пришло
            if(wordLen) countRecievedDots/=2;

            if(countRecievedDots>=countWaitingDots){                                    //Приняли канал целиком
                //Кладем принятый шот в соответствующий мап
                if(shots.at(qCountTrailingZeroBits(value)).contains(shotCountRecieved)){                       //Если текущий принятый канал уже содержит такой номер шота, переходим к следующей пачке
                    shotCountRecieved++;
                    qDebug() << "Attantion! Dublicate CH" + QString::number(value);
                }
                shots[qCountTrailingZeroBits(value)].insert(shotCountRecieved,bytes);                          //Добавили пришедший канал в мап с текущим индексом
                m_console->putData(" :RECIEVED ANSWER_POINTS CH" + QString::number(value).toUtf8() );

                if (notYetFlag==0){                                                     //Если приняли все заправшиваемые каналы                                                  //Все точки всех отмеченных каналов приняты
                    m_console->putData("\n");
                    m_ManagementWidget->m_HistorySettings->addShot(shotCountRecieved);
                    shotCountRecieved++;                                                //Увеличиваем счетчик пачек
                }
                m_timer->start();                                                       //Стартуем таймер опроса статуса
            }
        }
        else if(value == NO_DATA_READY){                              //Точки по какой-то причин не готовы. Это может случиться только если точки были запрошены вручную, игнорируя статус данных
            QMessageBox::critical(nullptr,"Ошибка!","Данные не готовы для получения!");
            m_console->putData("Warning: MCU has no data\n");
            m_ManagementWidget->m_TransmitionSettings->setGetButton(true);
        }
        else{
            statusBar->setMessageBar("Error: Wrong REQUEST_POINTS ansver message!");
            m_console->putData("Warning: MCU has no data\n");
            m_ManagementWidget->m_TransmitionSettings->setGetButton(true);
        }
        break;
   }
}

//Выбрать шот из списка
void MainWindow::selectShot(){
    if(!shots.at(0).isEmpty() || !shots.at(1).isEmpty() ||!shots.at(2).isEmpty() ||!shots.at(3).isEmpty()){
        QByteArray ch;
        int shift;
        QVector<unsigned short> chWord;
        int shotNum = m_ManagementWidget->m_HistorySettings->curShot();
        viewer->clearGraphs(ShotViewer::AllCH);
        //Первый канал
        for(int i=0;i<shots.size();i++){
            shift =  shiftFactor.at((i+1)%2);
            if(shots.at(i).contains(shotNum)){
                ch = shots.at(i)[shotNum];
                chWord.clear();
                if(shift>0){
                    ch.remove(ch.size()-shift,shift);                   //Сдвигаем нефильтрованный сигнал вправо на количество ячеек в зависимости от модели
                    ch.prepend(shift,0xFF);
                }
                else if(shift<0){
                    ch.remove(0,-shift);                                       //Сдвигаем нефильтрованный сигнал влево на количество ячеек в зависимости от модели
                    ch.append(-shift,0);
                }

                /***********************рисуем сигнал****************************/
                if(wordLen){
                    for(int i=0;i<ch.size()-1;i+=2){
                        chWord.append((ch.at(i) << 8) + (ch.at(i+1) & 0xFF));
                    }
                    viewer->addUserGraph(chWord,chWord.size(),i+1);
                }
                else
                    viewer->addUserGraph(ch,ch.size(),i+1);

                /***********************рисуем границы****************************/
                if(i==1){
                    viewer->addLines(QVector<double>{static_cast<double>(m_ManagementWidget->m_plisSettings->borderLeft()),static_cast<double>(signalSize-m_ManagementWidget->m_plisSettings->borderRight())},1,3);
                    viewer->addLines2(QVector<double>{static_cast<double>(m_ManagementWidget->m_plisSettings->compLevel(1))},1,3);
                }
                else if(i==3){
                    viewer->addLines(QVector<double>{static_cast<double>(m_ManagementWidget->m_plisSettings->borderLeft()),static_cast<double>(signalSize-m_ManagementWidget->m_plisSettings->borderRight())},2,3);
                    viewer->addLines2(QVector<double>{static_cast<double>(m_ManagementWidget->m_plisSettings->compLevel(2))},2,3);
                }
            }
        }

        viewer->replotGraphs(ShotViewer::AllCH);
    }
}

//Очистить список
void MainWindow::on_clearButton(){
    shotCountRecieved=0;
    for(int i=0;i<shots.size();i++){
        shots[i].clear();
    }
    viewer->clearGraphs(ShotViewer::AllCH);
    viewer->replotGraphs(ShotViewer::AllCH);
}

//Обработка ошибок SLIP
void MainWindow::handlerTranspError() {
    m_console->putData("\n Transporting Error \n");
    emit on_disconnect_triggered();                         //Отключаемся   
}

//Слот на сигнал от m_transp, что произошла повторная отправка
void MainWindow::reSentInc(){
    statusBar->incReSent();
    m_console->putData("\n Resent \n");
}

void MainWindow::fillTable(QCPGraphDataContainer &dataMap){
    QCPGraphDataContainer::const_iterator begin = dataMap.begin();
    QCPGraphDataContainer::const_iterator end = dataMap.end();
    m_table->setRowCount(0);

    for (QCPGraphDataContainer::const_iterator it=begin; it!=end; ++it){
        m_table->setRowCount(m_table->rowCount()+1);
        m_table->setItem(m_table->rowCount() - 1, 0, new QTableWidgetItem(QString("%1").arg(it->key)));
        m_table->setItem(m_table->rowCount() - 1, 1, new QTableWidgetItem(QString("%1").arg(it->value)));
    }
    for(int i=tableSize;i<m_table->rowCount();i++)
        m_table->hideRow(i);
}

//Изменения настроек фильтра
void MainWindow::settingsChanged(){
    filter->updateSettings(ShadowSettings->getShadowFindSettings());//Забрали обновленные настройки
}

//Сохранить лог
void MainWindow::saveHistory(QString &dirname){
    QByteArray zeroBytes(100,0),dataBytes;
    QMap <int,QByteArray> tempMap;
    QFile *tempFile = new QFile();
    qint64 error=0;
    filename = QDate::currentDate().toString("yyyy_MM_dd") + QTime::currentTime().toString("__hh_mm_ss");

    tempFile->setFileName(dirname + "/" + filename);

    if(m_ManagementWidget->m_HistorySettings->curShot()==0){
        QMessageBox::warning(this, "Внимание!", "История пуста. Примите новые сигналы для их сохранения",QMessageBox::Ok);
        return;
    }
    if(!tempFile->open(QIODevice::ReadWrite)){
        QMessageBox::warning(this, "Внимание!", "Файл для сохранения истории не может быть открыт",QMessageBox::Ok);
        return;
    }

    int maxLastKey=0;
    for(int i=0;i<4;i++)
        if( shots.at(i).isEmpty()?0:shots.at(i).lastKey()>maxLastKey )
            maxLastKey = shots.at(i).lastKey();

    for(int j = 0; j<4;j++){
        tempMap = shots.at(j);
        for (int i = 0;i<maxLastKey;i++) {
            if(tempMap.contains(i)){
               dataBytes = tempMap[i];
               if(tempFile->write(dataBytes)<0)
                error++;
            }
            else
                if(tempFile->write(zeroBytes)<0)
                    error++;

            if(tempFile->write(endShotLine)<0)       //Разделение между шотами одного канала
                error++;
        }
        if(tempFile->write(endChannelLine)<0)        //Разделение между каналами
            error++;
    }
    tempFile->close();
    tempFile->deleteLater();
    if(error>0)
        QMessageBox::warning(this, "Внимание!", " При записи лога произошло "+ QString::number(error) + "ошибок!",QMessageBox::Ok);
    else
        QMessageBox::information(this, "Успешно!", "Данные успешно записаны в файл " + dirname + "/" + filename,QMessageBox::Ok);
}

//Открытие истории
void MainWindow::loadHistory()
{
    QString filename = QFileDialog::getOpenFileName(this, "Открыть историю... ");
    QFile *tempFile = new QFile();                                                            //Файл лога

    tempFile->setFileName(filename);
    if(!tempFile->open(QIODevice::ReadOnly)){
        qDebug() << "tempFile can`t be open";
        return;
    }
    QByteArray tempBuf = tempFile->readAll();                                           //Читаем большой буфер с несколькими кадрами
    tempFile->close();
    tempFile->deleteLater();
    QByteArray chAll;
    int line;

    shots.clear();

    for(int i=0;i<4;i++){
        line = tempBuf.indexOf(endChannelLine);                 //индекс вхождения строки окончания канала
        chAll = tempBuf.left(line);                             //Взяли весь первый канал
        tempBuf.remove(0,chAll.size()+endChannelLine.size());   //Удалили первый канал из общего буфера

        int n=0;
        QMap<int,QByteArray> ch;
        while(chAll.size()!=0){
            int endShot = chAll.indexOf(endShotLine);                   //бьем канал на шоты
            ch.insert(n,chAll.left(endShot));                        //Вставляем первый шот во временный мап
            chAll.remove(0,ch[n].size()+endChannelLine.size());      //Удаляем шот и линию окончания из канала
            n++;
        }
        shots.append(ch);
    }

    for(int i = 0; i<shots.at(0).size();i++)
        m_ManagementWidget->m_HistorySettings->addShot(i);

}


void MainWindow::onCtrlF5Pressed(){
    m_ManagementWidget->m_plisSettings->setEnabled(true);
    ui->TableShow->setVisible(true);
    ui->showConsole->setVisible(true);
    ui->ShdowSet->setVisible(true);
    ui->mainToolBar->addWidget(tableSizeSpinbox);
    ui->mainToolBar->addWidget(tableSizeLabel);
    tableSizeSpinbox->setVisible(true);
    tableSizeLabel->setVisible(true);
}
