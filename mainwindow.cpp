#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "serialsettings.h"

#include "slip.h"
#include "transp.h"
#include "statusbar.h"
#include <QSplitter>


//#define TEST_MODE

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->disconnect->setEnabled(false);

    settings_ptr = new SerialSettings(this);
    serial = new QSerialPort();
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::handlerTimer);
    m_GettingDiameterTimer=new QTimer(this);
    connect(m_GettingDiameterTimer, &QTimer::timeout, this, &MainWindow::handlerGettingDiameterTimer);

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
    //Интерфейс
    //Здесь нужно создавать виджет(главный), устанавливать его, как центральный и передавать в конструктор лайонута уже его..
    //widget = QWidget(self)
    //self.setCentralWidget(widget)
    //hbox = QHBoxLayout(widget)
    // hbox.setSpacing(10)

    //Статус бар
    statusBar = new StatusBar(ui->statusBar);
    connect(this, &MainWindow::statusUpdate, [this](bool online) { statusBar->setStatus(online); });
    connect(this, &MainWindow::dataReadyUpdate, [this](int ready) { statusBar->setDataReady(ready); });
    connect(this, &MainWindow::infoUpdate, [this](int info) { statusBar->setInfo(info); });

    //Виджеты интерфейса
    layoutV = new QVBoxLayout();
    centralWidget()->setLayout(layoutV);

    //Левая панель
    m_MainControlWidget = new MainControlWidget(this);
    m_MainControlWidget->setMinimumWidth(250);

    //Правая панель
    m_ManagementWidget = new ManagementWidget(this);
    m_ManagementWidget->setMinimumWidth(270);
    packetSize = m_ManagementWidget->m_TransmitionSettings->packetSizeSpinbox->value();

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

    //Центр
    viewer = new ShotViewer(m_tab);
    connect(viewer,&ShotViewer::graph_selected,this,&MainWindow::fillTable);

    diameterPlot = new QCustomPlot(m_tab);

    //ПОСТРОЕНИЕ ДИАМЕТРА
    diameterPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables);
    diameterPlot->axisRect()->setupFullAxesBox();

    QFont legendFont = font();
    legendFont.setPointSize(10);
    diameterPlot->xAxis->setRangeLower(0);
    diameterPlot->xAxis->setRangeUpper(5000);
    diameterPlot->yAxis->setRangeLower(-2);
    diameterPlot->yAxis->setRangeUpper(2);
    diameterPlot->legend->setVisible(true);
    diameterPlot->legend->setFont(legendFont);
    diameterPlot->legend->setSelectedFont(legendFont);
    diameterPlot->legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items

    m_tab->addTab(viewer, "Сигнал");
    m_tab->addTab(diameterPlot, "Диаметр");
    m_tab->setTabBarAutoHide(true);
    connect(m_tab,&QTabWidget::currentChanged,[=](int index){
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
    emit m_tab->currentChanged(m_tab->currentIndex());
    //Разделители
    QWidget *container = new QWidget;
    QSplitter *splitterV = new QSplitter(Qt::Vertical, this);
    QSplitter *splitterH = new QSplitter(Qt::Horizontal, this);
    QVBoxLayout *qVBoxLayout  = new QVBoxLayout();

    splitterH->addWidget(m_MainControlWidget);
    splitterH->addWidget(m_tab);
    splitterH->addWidget(m_ManagementWidget);
    splitterH->addWidget(m_table);
    qVBoxLayout->addWidget(splitterH);
    container->setLayout(qVBoxLayout);

    splitterV->addWidget(container);
    splitterV->addWidget(m_console);
    layoutV->addWidget(splitterV);

    splitterH->setStretchFactor(0,1);
    splitterH->setStretchFactor(1,40);
    splitterH->setStretchFactor(2,2);

    //Коннекты от Настроек ПЛИС
    connect(m_ManagementWidget->m_plisSettings,&PlisSettings::lazer1Send,[=](int i){sendByteToMK(LAZER1_SET, static_cast<char>(i),"Set Lazer1 Setting: ");});
    connect(m_ManagementWidget->m_plisSettings,&PlisSettings::lazer2Send,[=](int i){sendByteToMK(LAZER2_SET, static_cast<char>(i),"Set Lazer2 Setting: ");});
    connect(m_ManagementWidget->m_plisSettings,&PlisSettings::saveSend,[=]{sendByteToMK(LAZERS_SAVE, 0,"Save lazer's parameters to EEPROM: ");});//?? проверить работу

    connect(m_ManagementWidget->m_plisSettings,&PlisSettings::sendBorderLeft,[=](int i){sendByteToMK(LEFT_BORDER_SET, static_cast<char>(i),"Set left border: ");});
    connect(m_ManagementWidget->m_plisSettings,&PlisSettings::sendBorderRight,[=](int i){sendByteToMK(RIGHT_BORDER_SET, static_cast<char>(i),"Set right border: ");});
    connect(m_ManagementWidget->m_plisSettings,&PlisSettings::sendCompCH1,[=](int i){sendByteToMK(COMP_CH1_SET, static_cast<char>(i),"Set comp level CH1: ");});
    connect(m_ManagementWidget->m_plisSettings,&PlisSettings::sendCompCH2,[=](int i){sendByteToMK(COMP_CH2_SET, static_cast<char>(i),"Set comp level CH2: ");});

    //Коннекты от параметров передачи
    connect(m_ManagementWidget->m_TransmitionSettings,&TransmitionSettings::setPacketSize,[=](int n) {packetSize=n;});
    connect(m_ManagementWidget->m_TransmitionSettings,&TransmitionSettings::chChooseChanged,this,&MainWindow::chOrderSend);
    connect(m_ManagementWidget->m_TransmitionSettings,&TransmitionSettings::getButtonClicked,this,&MainWindow::getButtonClicked);

    //Коннекты от истории
    connect(m_ManagementWidget->m_HistorySettings,&HistorySettings::saveHistoryPushed,[=]{saveHistory(dirnameDefault);});
    connect(m_ManagementWidget->m_HistorySettings,&HistorySettings::shotSelected,this,&MainWindow::selectShot);
    connect(m_ManagementWidget->m_HistorySettings,&HistorySettings::clearButtonClicked,this,&MainWindow::on_clearButton);

    //Коннекты от графика диаметра
    connect(m_ManagementWidget->m_DiameterTransmition,&DiameterTransmition::getDiameterChanged,[=](int state){
        if(state){
            m_GettingDiameterTimer->setInterval(1000/m_ManagementWidget->m_DiameterTransmition->reqFreqSpinbox->value());
            m_GettingDiameterTimer->start();
        }
        else{
            m_GettingDiameterTimer->stop();
        }
    });
    connect(m_ManagementWidget->m_DiameterTransmition,&DiameterTransmition::reqFreqValueChanged,[=](int value){ m_GettingDiameterTimer->setInterval(1000/value); });
    connect(m_ManagementWidget->m_DiameterTransmition,&DiameterTransmition::xWindowChanged, [=](int value){ xWindowDiameter = value; });
    connect(m_ManagementWidget->m_DiameterTransmition,&DiameterTransmition::countPointsChanged, [=](int value){ countOfCollect = value; });//Количество точек для коллекционирования
    connect(m_ManagementWidget->m_DiameterTransmition,&DiameterTransmition::diameterModeChanged, [=](bool mode){ diameterMode = mode; clearDiameterVectors(); });   //Изменен режим запроса диаметров
    connect(m_ManagementWidget->m_DiameterTransmition,&DiameterTransmition::windowSizeChanged, [=](int value){ m_windowSize = value;});   //Окно фильтра изменилось
    connect(m_ManagementWidget->m_DiameterTransmition,&DiameterTransmition::averageChanged, [=](int value){ m_average = value; });   //Усреднение изменилось
    connect(diameterPlot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));


    //Тулбар
    tableSizeSpinbox = new QSpinBox(this);
    tableSizeLabel = new QLabel("Размер таблицы",this);
    tableSizeSpinbox->setRange(1,1000);
    tableSizeSpinbox->setValue(tableSize);

    ui->mainToolBar->addWidget(tableSizeLabel);
    ui->mainToolBar->addWidget(tableSizeSpinbox);
    tableSizeSpinbox->setEnabled(false);

    ui->ShowMainControl->setChecked(true);
    ui->ShowManagementPanel->setChecked(true);
    connect(ui->showConsole,&QAction::toggled,[=](bool i){if(i) m_console->show(); else m_console->hide();});
    connect(ui->TableShow,&QAction::toggled,[=](bool i){
        if(i) {
            m_table->show();
            tableSizeSpinbox->setEnabled(true);
        } else {
            m_table->hide();
            tableSizeSpinbox->setEnabled(false);
        }
    });
    connect(tableSizeSpinbox, QOverload<int>::of(&QSpinBox::valueChanged),[=](int val){
            tableSize = val;

            if(m_table->rowCount()>=tableSize){
                for(int i=0;i<tableSize;i++)
                     m_table->showRow(i);
                for(int i=tableSize;i<m_table->rowCount();i++)
                    m_table->hideRow(i);
            }
    });

    connect(ui->ShowMainControl,&QAction::toggled,[=](bool i){if(i) m_MainControlWidget->show(); else m_MainControlWidget->hide();});
    connect(ui->ShowManagementPanel,&QAction::toggled,[=](bool i){if(i) m_ManagementWidget->show(); else m_ManagementWidget->hide();});
    connect(ui->AutoRange,&QAction::triggered,viewer, &ShotViewer::autoScale);

    //Пустой виджет, разделяющий кнопки на mainToolBar
    QWidget* empty = new QWidget();
    empty->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    ui->mainToolBar->insertWidget(ui->showConsole,empty);

    //Логгирование
    log = new SaveLog(this);
    connect(log,&SaveLog::SaveToFolder,[=](QString &dirname){saveHistory(dirname);});
    connect(ui->SaveLog,&QAction::triggered,[=]{log->show();});
    //Создание папки с логами, если ее нет.
    dir = new QDir(dirnameDefault);
    if (!dir->exists()) {
        dir->mkdir(dirnameDefault);
    }
    dir->setFilter( QDir::NoDotAndDotDot);

    file= new QFile();

   ShadowSettings = new SettingsShadowsFindDialog(this);
   connect(ShadowSettings, &SettingsShadowsFindDialog::settingsChanged,this,&MainWindow::settingsChanged);//Обновляем настройки в фильтре
   connect(ShadowSettings, &SettingsShadowsFindDialog::sendSettingsToMK,[=]{
       sendVectorToMK(NEWSHADFINDPAR,ShadowSettings->getShadowFindSettings(),"Новые параметры поиска диаметра отправлены в МК: ");//Засылаем настройки в МК
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
        m_ManagementWidget->m_TransmitionSettings->getButton->setEnabled(true);
        m_transp->clearQueue();
        serial->readAll();

        m_ManagementWidget->m_HistorySettings->saveHistoryButton->setEnabled(true);
        if(channelsOrder!=0)
            sendByteToMK(CH_ORDER,channelsOrder,"SEND CH_ORDER: ");
        //Включаем галки приема-передачи
        m_ManagementWidget->m_TransmitionSettings->ch1CheckBox->setEnabled(true);
        m_ManagementWidget->m_TransmitionSettings->ch2CheckBox->setEnabled(true);
        m_ManagementWidget->m_TransmitionSettings->ch3CheckBox->setEnabled(true);
        m_ManagementWidget->m_TransmitionSettings->ch4CheckBox->setEnabled(true);
        //Сбрасываем галки приема-передачи
        m_ManagementWidget->m_TransmitionSettings->ch1CheckBox->setChecked(false);
        m_ManagementWidget->m_TransmitionSettings->ch2CheckBox->setChecked(false);
        m_ManagementWidget->m_TransmitionSettings->ch2InCheckBox->setChecked(false);
        m_ManagementWidget->m_TransmitionSettings->ch3CheckBox->setChecked(false);
        m_ManagementWidget->m_TransmitionSettings->ch4CheckBox->setChecked(false);
        m_ManagementWidget->m_TransmitionSettings->ch4InCheckBox->setChecked(false);
        //Включаем кнопки управления настройками ПЛИС
        m_ManagementWidget->m_plisSettings->lazer1Button->setEnabled(true);
        m_ManagementWidget->m_plisSettings->lazer2Button->setEnabled(true);
        m_ManagementWidget->m_plisSettings->borderLeftButton->setEnabled(true);
        m_ManagementWidget->m_plisSettings->borderRightButton->setEnabled(true);
        m_ManagementWidget->m_plisSettings->compCH1Button->setEnabled(true);
        m_ManagementWidget->m_plisSettings->compCH2Button->setEnabled(true);
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

    m_ManagementWidget->m_HistorySettings->saveHistoryButton->setEnabled(false);

    //Вырубаем автополучение на всякий
    m_ManagementWidget->m_TransmitionSettings->getButton->setEnabled(false);
    m_ManagementWidget->m_TransmitionSettings->getButton->setChecked(false);

    m_ManagementWidget->m_TransmitionSettings->ch1CheckBox->setEnabled(false);
    m_ManagementWidget->m_TransmitionSettings->ch2CheckBox->setEnabled(false);
    m_ManagementWidget->m_TransmitionSettings->ch2InCheckBox->setEnabled(false);
    m_ManagementWidget->m_TransmitionSettings->ch3CheckBox->setEnabled(false);
    m_ManagementWidget->m_TransmitionSettings->ch4CheckBox->setEnabled(false);
    m_ManagementWidget->m_TransmitionSettings->ch4InCheckBox->setEnabled(false);

    m_ManagementWidget->m_plisSettings->lazer1Button->setEnabled(false);
    m_ManagementWidget->m_plisSettings->lazer2Button->setEnabled(false);
    m_ManagementWidget->m_plisSettings->borderLeftButton->setEnabled(false);
    m_ManagementWidget->m_plisSettings->borderRightButton->setEnabled(false);
    m_ManagementWidget->m_plisSettings->compCH1Button->setEnabled(false);
    m_ManagementWidget->m_plisSettings->compCH2Button->setEnabled(false);
}

void MainWindow::sendByteToMK(char dst, int dataByte, const QString &msg)
{
    QByteArray data;
    char msb,lsb;
    data.append(dst);
    msb=(0&0xFF00)>>8;
    lsb=static_cast<char> (dataByte&0x00FF);
    data.append(msb);
    data.append(lsb);
    m_console->putData(msg.toUtf8());
    m_transp->sendPacket(data);
}

void MainWindow::sendVectorToMK(char dst, QVector<double> dataV, const QString &msg){
    conversation_t conv;
    QByteArray data;
    char msb,lsb;
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

//Подсчет количества отмеченных каналов
void MainWindow::chOrderSend(int ch)
{
    switch (ch){
    case 1:
       if(m_ManagementWidget->m_TransmitionSettings->ch1CheckBox->isChecked()){
           channelsOrder|=0x01;
           m_ManagementWidget->m_TransmitionSettings->ch2InCheckBox->setEnabled(true);
       }
       else{
          channelsOrder&=~0x01;
          m_ManagementWidget->m_TransmitionSettings->ch2InCheckBox->setEnabled(false);
       }
        break;
    case 2:
       if(m_ManagementWidget->m_TransmitionSettings->ch2CheckBox->isChecked())
           channelsOrder|=0x02;
       else
          channelsOrder&=~0x02;
        break;
    case 3:
       if(m_ManagementWidget->m_TransmitionSettings->ch3CheckBox->isChecked()){
           channelsOrder|=0x04;
           m_ManagementWidget->m_TransmitionSettings->ch4InCheckBox->setEnabled(true);
       }
       else{
          channelsOrder&=~0x04;
          m_ManagementWidget->m_TransmitionSettings->ch4InCheckBox->setEnabled(false);
       }
        break;
    case 4:
       if(m_ManagementWidget->m_TransmitionSettings->ch4CheckBox->isChecked())
           channelsOrder|=0x08;
       else
          channelsOrder&=~0x08;
        break;
    }
    sendByteToMK(CH_ORDER,channelsOrder,"SEND CH_ORDER: ");
}

int MainWindow::countCheckedCH()
{
    int chCountChecked=0;
    for (int i=0;i<4;i++){
        if(channelsOrder&(1<<i))
            chCountChecked++;
    }
    return chCountChecked;
}

//Запрость у MCU пакет длины n с канала ch
void MainWindow::getPacketFromMCU(int n)
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
void MainWindow::getButtonClicked(bool checked)
{
    if(checked){
        notYetFlag = countCheckedCH();  //На старте передачи запоминаем сколько надо ждать каналов
        if(notYetFlag==0){
            statusBar->setMessageBar("Внимание!, Не выбрано ни одного канала!");
            QMessageBox::warning(this, "Внимание!", "Не выбрано ни одного канала!",QMessageBox::Ok);
            m_ManagementWidget->m_TransmitionSettings->getButton->setChecked(false);
            return;
        }
        m_ManagementWidget->m_TransmitionSettings->ch1CheckBox->setEnabled(false);
        m_ManagementWidget->m_TransmitionSettings->ch2CheckBox->setEnabled(false);
        m_ManagementWidget->m_TransmitionSettings->ch3CheckBox->setEnabled(false);
        m_ManagementWidget->m_TransmitionSettings->ch4CheckBox->setEnabled(false);
    }
    else {
        if(notYetFlag==0){//Если передача всех каналов пачки завершена
            m_ManagementWidget->m_TransmitionSettings->ch1CheckBox->setEnabled(true);
            m_ManagementWidget->m_TransmitionSettings->ch2CheckBox->setEnabled(true);
            m_ManagementWidget->m_TransmitionSettings->ch3CheckBox->setEnabled(true);
            m_ManagementWidget->m_TransmitionSettings->ch4CheckBox->setEnabled(true);
         }
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
        m_timer->stop();                                              //Останавливаем запрос статусов
        m_console->putData("REQUEST_POINTS: ");
        countWaitingDots = countAvaibleDots;                          //Запоминаем, сколько точек всего придет в одном канале                                                                           //заправшиваем новую пачку
        statusBar->setDownloadBarRange(countAvaibleDots);
        statusBar->setDownloadBarValue(0);
        while (countAvaibleDots>0){                                   //Отправляем запрос несоклько раз по packetSize точек.
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
    charToShort.ch[0]=bytes[2];
    charToShort.ch[1]=bytes[1];
    unsigned short value = charToShort.sh;
    int dataReady=-1;
    QString chName;
    bytes.remove(0, 3);                                                         //Удалили 3 байта (команду и значение)

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

        if (value != NO_DATA_READY) {
            dataReady = value;
            countAvaibleDots=value;

            //Забираем 16 байт метаданных
            tempPLISextremums2.clear();
            tempPLISextremums1.clear();
            for(int i=0;i<8;i+=2){
                charToShort.ch[0] = bytes.at(i);
                charToShort.ch[1] = bytes.at(i+1);
                tempPLISextremums2.prepend(charToShort.sh);
                charToShort.ch[0] = bytes.at(i+8);
                charToShort.ch[1] = bytes.at(i+9);
                tempPLISextremums1.prepend(charToShort.sh);
            }
            m_MainControlWidget->m_resultWidget->extr1Ch1->setText("   Экстр1: " + QString::number(tempPLISextremums1.at(0)));
            m_MainControlWidget->m_resultWidget->extr2Ch1->setText("   Экстр2: " + QString::number(tempPLISextremums1.at(1)));
            m_MainControlWidget->m_resultWidget->extr3Ch1->setText("   Экстр3: " + QString::number(tempPLISextremums1.at(2)));
            m_MainControlWidget->m_resultWidget->extr4Ch1->setText("   Экстр4: " + QString::number(tempPLISextremums1.at(3)));
            m_MainControlWidget->m_resultWidget->extr1Ch2->setText("   Экстр1: " + QString::number(tempPLISextremums2.at(0)));
            m_MainControlWidget->m_resultWidget->extr2Ch2->setText("   Экстр2: " + QString::number(tempPLISextremums2.at(1)));
            m_MainControlWidget->m_resultWidget->extr3Ch2->setText("   Экстр3: " + QString::number(tempPLISextremums2.at(2)));
            m_MainControlWidget->m_resultWidget->extr4Ch2->setText("   Экстр4: " + QString::number(tempPLISextremums2.at(3)));
            bytes.remove(0, 16);
            //16 байт - ошибки и значения параметров ПЛИС
            m_MainControlWidget->m_signalErrWidget->setVal(bytes.at(0),1);   //Младшие значащие байты
            m_MainControlWidget->m_signalErrWidget->setVal(bytes.at(2),2);

            charToShort.ch[0] = bytes.at(4);
            charToShort.ch[1] = bytes.at(5);
            m_ManagementWidget->m_plisSettings->lazer1Button->setText(QString::number(charToShort.sh));

            charToShort.ch[0] = bytes.at(6);
            charToShort.ch[1] = bytes.at(7);
            m_ManagementWidget->m_plisSettings->lazer2Button->setText(QString::number(charToShort.sh));

            charToShort.ch[0] = bytes.at(8);
            charToShort.ch[1] = bytes.at(9);
            m_ManagementWidget->m_plisSettings->borderLeftButton->setText(QString::number(charToShort.sh));

            charToShort.ch[0] = bytes.at(10);
            charToShort.ch[1] = bytes.at(11);
            m_ManagementWidget->m_plisSettings->borderRightButton->setText(QString::number(charToShort.sh));

            charToShort.ch[0] = bytes.at(12);
            charToShort.ch[1] = bytes.at(13);
            m_ManagementWidget->m_plisSettings->compCH1Button->setText(QString::number(charToShort.sh));

            charToShort.ch[0] = bytes.at(14);
            charToShort.ch[1] = bytes.at(15);
            m_ManagementWidget->m_plisSettings->compCH2Button->setText(QString::number(charToShort.sh));

            bytes.remove(0, 16);

            if(tempPLISextremums1.size()==4){
                shadowsCh1Plis = filter->shadowFind(tempPLISextremums1);//Расчет теней на основании экстремумов из плисины
                m_MainControlWidget->m_resultWidget->shad1Ch1->setText("   Тень1: " + QString::number(shadowsCh1Plis.at(0)));
                m_MainControlWidget->m_resultWidget->shad2Ch1->setText("   Тень2: " + QString::number(shadowsCh1Plis.at(1)));
            }
            if(tempPLISextremums2.size()==4){
                shadowsCh2Plis = filter->shadowFind(tempPLISextremums2);//Расчет теней на основании экстремумов из плисины
                m_MainControlWidget->m_resultWidget->shad1Ch2->setText("   Тень1: " + QString::number(shadowsCh2Plis.at(0)));
                m_MainControlWidget->m_resultWidget->shad2Ch2->setText("   Тень2: " + QString::number(shadowsCh2Plis.at(1)));
            }
            if(shadowsCh1Plis.size()>1 && shadowsCh2Plis.size()>1){
                diameterPlis = filter->diameterFind(shadowsCh1Plis,shadowsCh2Plis);
                m_MainControlWidget->m_resultWidget->diametrPlisLabel->setText("Диаметр ПЛИС: " +QString::number(diameterPlis.at(0) + diameterPlis.at(1)));
                m_MainControlWidget->m_resultWidget->centerPositionLabel->setText("Смещение: " + QString::number(diameterPlis.at(2),'f',0) + ", " + QString::number(diameterPlis.at(3),'f',0));
                m_MainControlWidget->m_resultWidget->radius1->setText("   Радиус1: " + QString::number(diameterPlis.at(0)));
                m_MainControlWidget->m_resultWidget->radius2->setText("   Радиус2: " + QString::number(diameterPlis.at(1)));
                m_MainControlWidget->m_resultWidget->m_centerViewer->setCoord(diameterPlis.at(2)/1000,diameterPlis.at(3)/1000);
                m_MainControlWidget->m_resultWidget->m_centerViewer->setRad(diameterPlis.at(0)/1000,diameterPlis.at(1)/1000);
            }
            if(notYetFlag)                                                             //Если есть непринятые каналы
                manualGetShotButton();                                                  //Запрашиваем шот
        }
        else {
            dataReady = 0;
            m_console->putData("Warning: MCU has no data\n\n");
        }
        emit dataReadyUpdate(dataReady);
        m_timer->start();                                                              //Если получили статус, то можно запрашивать еще
        break;

      //Получили данные с диаметром
     case REQUEST_DIAMETER:
        if(value!=0){
            r1FromMCU.clear();
            r2FromMCU.clear();
            c1FromMCU.clear();
            c2FromMCU.clear();
            shadowsFromMCU.clear();
            for(int i=0; i<bytes.size();i+=2){
                int num = static_cast<unsigned char>(bytes.at(i)) + static_cast<unsigned char>(bytes.at(i+1))*256;
                shadowsFromMCU.append(num);//Front,Spad1,Front2,Spad2
            }

            for(int i = 0;i<shadowsFromMCU.size();i+=4){
                QVector<double> tempDiameters = filter->diameterFind(QVector<double>{shadowsFromMCU.at(i),shadowsFromMCU.at(i+1)},QVector<double>{shadowsFromMCU.at(i+2),shadowsFromMCU.at(i+3)});
                r1FromMCU.append(tempDiameters.at(0)*2);
                r2FromMCU.append(tempDiameters.at(1)*2);
                c1FromMCU.append(tempDiameters.at(2));
                c2FromMCU.append(tempDiameters.at(3));

            }
            m1FromMCU = filter->medianFilterX(r1FromMCU,m_windowSize,m_average);
            m2FromMCU = filter->medianFilterY(r2FromMCU,m_windowSize,m_average);
            m_ManagementWidget->m_DiameterTransmition->r1ValueLabel->setNum(m1FromMCU.last());
            m_ManagementWidget->m_DiameterTransmition->r2ValueLabel->setNum(m2FromMCU.last());
            if(diameterMode)//CollectMode
                collectDiameter();
            else
                realTimeDiameter();
            break;
        }
         break;


    case REQUEST_POINTS:
        if ((value==CH1)|| (value==CH2) || (value==CH3) || (value==CH4)){                                                //Если пришли точки по одному из каналов, то обрабатываем
            countRecievedDots+=bytes.count();                                           //Считаем, сколько уже пришло
            statusBar->setDownloadBarValue(countRecievedDots);                          //Прогресс бар апгрейд
            currentShot.append(bytes);                                                  //Добавляем в шот данные, которые пришли
            if(countRecievedDots>=countWaitingDots){                                    //Приняли канал целиком
                //Кладем принятый шот в соответствующий мап
                if (value == CH1){
                    if(shotsCH1.contains(shotCountRecieved)) {                         //Если в мапе уже есть запись с текущим индексом пачки
                         shotCountRecieved++;                                           //Начинаем следующую пачку
                         qDebug() << "Attantion! Dublicate CH1";
                    }
                    chName="CH1_NF";
                    currentShot=currentShot.mid(16);                                    //Смещение влево
                    currentShot.append(16,0);
                    shotsCH1.insert(shotCountRecieved,currentShot);                     //Добавили пришедший канал в мап с текущим индексом
                    m_console->putData(" :RECIEVED ANSWER_POINTS CH1_NF  ");

                    if(m_ManagementWidget->m_TransmitionSettings->ch2InCheckBox->isChecked()) {                //Если нужна фильтрация (внутренняя)
                        QByteArray filtered = filter->toButterFilter(currentShot,currentShot.size());          //Получаем фильтрованный массив
                        shotsCH2In.insert(shotCountRecieved,filtered);
                    }
                }
                else if(value == CH2){
                   if(shotsCH2.contains(shotCountRecieved)) {                         //Если в мапе уже есть запись с текущим индексом пачки
                        shotCountRecieved++;                                           //Начинаем следующую пачку
                        qDebug() << "Attantion! Dublicate CH2";
                   }
                   chName="CH1_F";
                   currentShot=currentShot.mid(38);                                 //Смещение отфильтрованного сигнала из плисы
                   currentShot.append(38,0);
                   shotsCH2.insert(shotCountRecieved,currentShot);                                     //Добавили пришедший канал в мап с текущим индексом
                   m_console->putData(" :RECIEVED ANSWER_POINTS CH1_F  ");
                }
                else if(value == CH3){
                     if(shotsCH3.contains(shotCountRecieved)) {
                         shotCountRecieved++;
                         qDebug() << "Attantion! Dublicate CH3";
                     }
                     chName="CH2_NF";
                     currentShot=currentShot.mid(16);                                    //Смещение влево
                     currentShot.append(16,0);
                     shotsCH3.insert(shotCountRecieved,currentShot);
                     m_console->putData(" :RECIEVED ANSWER_POINTS CH2_NF  ");
                     if(m_ManagementWidget->m_TransmitionSettings->ch4InCheckBox->isChecked()){                                                      //Если нужна фильтрация
                        QByteArray filtered =filter->toButterFilter(currentShot,currentShot.size());          //Получаем фильтрованный массив
                        shotsCH4In.insert(shotCountRecieved,filtered);                                    //Добавляем его на график
                     }
                }
                else if(value == CH4){
                     if(shotsCH4.contains(shotCountRecieved)) {
                         shotCountRecieved++;
                         qDebug() << "Attantion! Dublicate CH4";
                     }
                     chName="CH2_F";
                     currentShot=currentShot.mid(38);                                 //Смещение отфильтрованного сигнала из плисы
                     currentShot.append(38,0);
                     shotsCH4.insert(shotCountRecieved,currentShot);
                     m_console->putData(" :RECIEVED ANSWER_POINTS CH2_F  ");
                }

                //Обнуляем всякое
                countRecievedDots=0;                                                    //Обнуляем количество пришедших точек
                currentShot.clear();                                                    //Чистим временное хранилище текущего принимаемого канала
                statusBar->setInfo(m_transp->getQueueCount());                          //Обновляем статус бар

                if (notYetFlag==0){                                                     //Если приняли все заправшиваемые каналы                                                  //Все точки всех отмеченных каналов приняты
                    m_console->putData("\n\n");
                    shotCountRecieved++;                                                //Увеличиваем счетчик пачек
                    m_ManagementWidget->m_HistorySettings->shotsComboBox->addItem(QString::number(shotCountRecieved-1));
                    m_ManagementWidget->m_HistorySettings->shotsComboBox->setCurrentIndex(m_ManagementWidget->m_HistorySettings->shotsComboBox->count()-1);
                    if(m_ManagementWidget->m_TransmitionSettings->getButton->isChecked()) //Если кнопка все еще нажата
                        getButtonClicked(true);                                           //Вызываем слот нажатия кнопки и инициации получения новой пачки
                    else
                        getButtonClicked(false);                                          //Иначе обнуляем все счетчики, разблокируем чекбоксы и т.д.
                }
                m_timer->start();                                                       //Стартуем таймер опроса статуса
            }
        }

        else if(value == NO_DATA_READY){                              //Точки по какой-то причин не готовы. Это может случиться только если точки были запрошены вручную, игнорируя статус данных
            QMessageBox::critical(nullptr,"Ошибка!","Данные не готовы для получения!");
            m_console->putData("Warning: MCU has no data\n\n");
            m_ManagementWidget->m_TransmitionSettings->getButton->setEnabled(true);
        }
        else{
            statusBar->setMessageBar("Error: Wrong REQUEST_POINTS ansver message!");
            m_console->putData("Warning: MCU has no data\n\n");
            m_ManagementWidget->m_TransmitionSettings->getButton->setEnabled(true);
        }
        break;
   }
}

//Выбрать шот из списка
void MainWindow::selectShot(){
    if(!shotsCH1.isEmpty() || !shotsCH2.isEmpty() ||!shotsCH3.isEmpty() ||!shotsCH4.isEmpty()){
        QByteArray ch;
        int shotNum = m_ManagementWidget->m_HistorySettings->shotsComboBox->currentText().toInt();
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
            viewer->addLines(QVector<double>{static_cast<double>(m_ManagementWidget->m_plisSettings->borderLeftButton->text().toInt()),static_cast<double>(10800-m_ManagementWidget->m_plisSettings->borderRightButton->text().toInt())},1,3);
            viewer->addLines2(QVector<double>{static_cast<double>(m_ManagementWidget->m_plisSettings->compCH1Button->text().toInt())},1,3);


        }
        if(shotsCH2In.contains(shotNum)){                                                           //Добавление на график внутреннего отфильтрованного сигнала с экстремумами и тенями
            ch = shotsCH2In[shotNum];
            viewer->addUserGraph(ch,ch.size(),2);
            QVector<QVector<unsigned int>> dots = filter->extrFind2(ch,ch.size());                  //Поиск экстремумов
            viewer->addDots(dots,1);
            QVector <unsigned int> xDots;
            for (int i = 0;i<4;i++){
                xDots.append(dots.at(i).at(0));
            }
            shadowsCh1 = filter->shadowFind(xDots);                                                 //Поиск теней
            m_MainControlWidget->m_resultWidget->leftShadow1Label->setText("   Лев. тень: " +QString::number(shadowsCh1.at(0)));
            m_MainControlWidget->m_resultWidget->rightShadow1Label->setText("   Прав. тень: " +QString::number(shadowsCh1.at(1)));
            viewer->addLines(shadowsCh1,1,2);
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
            viewer->addLines(QVector<double>{static_cast<double>(m_ManagementWidget->m_plisSettings->borderLeftButton->text().toInt()),static_cast<double>(10800-m_ManagementWidget->m_plisSettings->borderRightButton->text().toInt())},2,3);
            viewer->addLines2(QVector<double>{static_cast<double>(m_ManagementWidget->m_plisSettings->compCH2Button->text().toInt())},2,3);
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
            viewer->addLines(shadowsCh2,2,2);
            m_MainControlWidget->m_resultWidget->leftShadow2Label->setText("   Лев. тень: " + QString::number(shadowsCh2.at(0)));
            m_MainControlWidget->m_resultWidget->rightShadow2Label->setText("   Прав. тень: " + QString::number(shadowsCh2.at(1)));

        }
        viewer->replotGraphs(ShotViewer::AllCH);
        //Расчет диаметра
        if(shadowsCh1.size()>1 && shadowsCh2.size()>1){
            diameter = filter->diameterFind(shadowsCh1,shadowsCh2);
            m_MainControlWidget->m_resultWidget->diametrLabel->setText("Диаметр: " +QString::number(diameter.at(0) + diameter.at(1)));
            m_MainControlWidget->m_resultWidget->m_centerViewer->setCoord(diameter.at(2)/1000,diameter.at(3)/1000);
            m_MainControlWidget->m_resultWidget->m_centerViewer->setRad(diameter.at(0)/1000,diameter.at(1)/1000);
            m_MainControlWidget->m_resultWidget->centerPositionLabel->setText("Смещение: " + QString::number(diameter.at(2)) + ", " + QString::number(diameter.at(3)));
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
    m_ManagementWidget->m_HistorySettings->shotsComboBox->clear();
    viewer->clearGraphs(ShotViewer::AllCH);
    viewer->replotGraphs(ShotViewer::AllCH);
}

//Обработка ошибок SLIP
void MainWindow::handlerTranspError() {
    emit on_disconnect_triggered();                         //Отключаемся   
}
//Слот на сигнал от m_transp, что произошла повторная отправка
void MainWindow::reSentInc(){
    statusBar->incReSent();
}
// Обработчик таймаута опроса состояния MCU
void MainWindow::handlerTimer() {
    statusBar->setInfo(m_transp->getQueueCount());
    QByteArray data;
    if (m_online) {
        sendByteToMK(REQUEST_STATUS,0,"SEND REQUEST_STATUS: ");
        m_timer->stop();
    }
    else {
        if (serial->isOpen())
            sendByteToMK(ASK_MCU,0,"SEND ASK_MCU: ");
    }
}



void MainWindow::writeToLogfileMeta(QString name){
    QFile tempFile;
    tempFile.setFileName(dirnameDefault + "/" + QDate::currentDate().toString("yyyy_MM_dd") + QTime::currentTime().toString("__hh_mm_ss")+ name);
    tempFile.open(QIODevice::WriteOnly);
    tempFile.write(currentShot,32);
    tempFile.write(endShotLine,endShotLine.size());
    tempFile.flush();
    tempFile.close();
}

void MainWindow::fillTable(QCPGraphDataContainer &dataMap){
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

void MainWindow::on_ShdowSet_triggered(){
    ShadowSettings->updateSettingsStruct();
    ShadowSettings->show();
}

void MainWindow::settingsChanged(){
    filter->updateSettings(ShadowSettings->getShadowFindSettings());//Забрали обновленные настройки
}

//Сохранить лог
void MainWindow::saveHistory(QString &dirname){
    QByteArray zeroBytes(100,0),dataBytes;
    QMap <int,QByteArray> tempMap;
    QList<QMap<int,QByteArray>> dataList;
    qint64 error=0;
    filename = QDate::currentDate().toString("yyyy_MM_dd") + QTime::currentTime().toString("__hh_mm_ss");
    file->setFileName(dirname + "/" + filename);

    if(m_ManagementWidget->m_HistorySettings->shotsComboBox->count()==0){
        QMessageBox::warning(this, "Внимание!", "История пуста. Примите новые сигналы для их сохранения",QMessageBox::Ok);
        return;
    }
    if(!file->open(QIODevice::ReadWrite)){
        QMessageBox::warning(this, "Внимание!", "Файл для сохранения истории не может быть открыт",QMessageBox::Ok);
        return;
    }

    dataList.append(shotsCH1);
    dataList.append(shotsCH2);
    dataList.append(shotsCH3);
    dataList.append(shotsCH4);
    QVector<int> lastKeys;
    lastKeys.append(shotsCH1.isEmpty()?0:shotsCH1.lastKey());
    lastKeys.append(shotsCH2.isEmpty()?0:shotsCH2.lastKey());
    lastKeys.append(shotsCH3.isEmpty()?0:shotsCH3.lastKey());
    lastKeys.append(shotsCH4.isEmpty()?0:shotsCH4.lastKey());
    int maxLastKey=0;
    for(int i=0;i<4;i++)
        if(lastKeys.at(i)>maxLastKey)
            maxLastKey = lastKeys.at(i);

    for(int j = 0; j<4;j++){
        tempMap = dataList.at(j);
        for (int i = 0;i<maxLastKey;i++) {
            if(tempMap.contains(i)){
               dataBytes = tempMap[i];
               if(file->write(dataBytes)<0)
               error++;
            }
            else
                if(file->write(zeroBytes)<0)
                    error++;
            if(file->write(endShotLine)<0)       //Разделение между шотами одного канала
                error++;
        }
        if(file->write(endChannelLine)<0)        //Разделение между каналами
            error++;
    }
    file->close();
    if(error>0)
        QMessageBox::warning(this, "Внимание!", " При записи лога произошло "+ QString::number(error) + "ошибок!",QMessageBox::Ok);
    else
        QMessageBox::information(this, "Успешно!", "Данные успешно записаны в файл " + dirname + "/" + filename,QMessageBox::Ok);
}

//Открытие истории
void MainWindow::on_action_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, "Открыть историю... ");
    //QString filename = "C:\qt_pr\STMviewer\build-STMviewer-Desktop_Qt_5_15_2_MinGW_64_bit-Debug\log\2021_10_29__19_17_34";
    QFile *tempFile = new QFile();                                                            //Файл лога

    tempFile->setFileName(filename);
    if(!tempFile->open(QIODevice::ReadOnly)){
        qDebug() << "tempFile can`t be open";
        return;
    }
    QByteArray tempBuf = tempFile->readAll();                                           //Читаем большой буфер с несколькими кадрами
    tempFile->close();
    QByteArray chAll;
    int line;
    QMap<int,QMap<int,QByteArray>> channels;

    for(int i=0;i<4;i++){
        line = tempBuf.indexOf(endChannelLine);                 //индекс вхождения строки окончания канала
        chAll = tempBuf.left(line);                             //Взяли весь первый канал
        tempBuf.remove(0,chAll.size()+endChannelLine.size());   //Удалили первый канал из общего буфера

        int n=0;
        QMap<int,QByteArray> shots;
        while(chAll.size()!=0){
            int endShot = chAll.indexOf(endShotLine);                   //бьем канал на шоты
            shots.insert(n,chAll.left(endShot));                        //Вставляем первый шот во временный мап
            chAll.remove(0,shots[n].size()+endChannelLine.size());      //Удаляем шот и линию окончания из канала
            n++;
        }
        channels.insert(i,shots);
    }

    shotsCH1 = channels[0];
    shotsCH2 = channels[1];
    shotsCH3 = channels[2];
    shotsCH4 = channels[3];

    for(int i = 0; i<shotsCH1.size();i++)
        m_ManagementWidget->m_HistorySettings->shotsComboBox->addItem(QString::number(i));

}


/*******************************РАБОТА С ДИАМЕТРАМИ****************************/
// Обработчик таймаута запроса диаметра
void MainWindow::handlerGettingDiameterTimer(){
    sendByteToMK(REQUEST_DIAMETER,0,"SEND REQUEST_DIAMETER: ");
}
void MainWindow::plotDiameter()
{
    if(m_ManagementWidget->m_DiameterTransmition->diemetersCheckBox->isChecked()){
        if(r1 == nullptr){
            r1 = diameterPlot->addGraph();
            r1->setName("Диаметр по оси Х");
        }
        if(r2 == nullptr){
            r2 = diameterPlot->addGraph();
            r2->setName("Диаметр по оси Y");
        }
       r1->setData(xDiameter,yr1);
       r2->setData(xDiameter,yr2);
     }
     else{
        if(r1 != nullptr){
            diameterPlot->removeGraph(r1);
            r1 = nullptr;
        }
        if(r2 != nullptr){
            diameterPlot->removeGraph(r2);
            r2 = nullptr;
        }
    }

    if(m_ManagementWidget->m_DiameterTransmition->centersCheckBox->isChecked()){
        if(c1 == nullptr){
            c1 = diameterPlot->addGraph();
            c1->setName("Отклонение от центра по оси Х");
        }
        if(c2 == nullptr){
            c2 = diameterPlot->addGraph();
            c2->setName("Отклонение от центра по оси Y");
        }
       c1->setData(xDiameter,yc1);
       c2->setData(xDiameter,yc2);
     }
     else{
        if(c1 != nullptr){
            diameterPlot->removeGraph(c1);
            c1 = nullptr;
        }
        if(c2 != nullptr){
            diameterPlot->removeGraph(c2);
            c2 = nullptr;
        }
    }
    if(m_ManagementWidget->m_DiameterTransmition->medianFilterCheckbox->isChecked()){
        if(m1 == nullptr){
            m1 = diameterPlot->addGraph();
            m1->setName("Фильтрованный диаметр по оси X");
            m1->setPen(QPen(QColor(Qt::red)));
        }
        if(m2 == nullptr){
            m2 = diameterPlot->addGraph();
            m2->setName("Фильтрованный диаметр по оси Y");
            m2->setPen(QPen(QColor(Qt::red)));
        }
       m1->setData(xDiameter,ym1);
       m2->setData(xDiameter,ym2);
     }
     else{
        if(m1 != nullptr){
            diameterPlot->removeGraph(m1);
            m1 = nullptr;

        }
        if(m2 != nullptr){
            diameterPlot->removeGraph(m2);
            m2 = nullptr;
        }
    }

    diameterPlot->xAxis->rescale();
    diameterPlot->yAxis->rescale();
    diameterPlot->xAxis->setRange(diameterPlot->xAxis->range().upper, xWindowDiameter, Qt::AlignRight);
    diameterPlot->yAxis->setRangeLower(diameterPlot->yAxis->range().lower-10);
    diameterPlot->yAxis->setRangeUpper(diameterPlot->yAxis->range().upper+10);
    //diameterPlot->yAxis->setRangeLower(0);
    diameterPlot->replot();

}

void MainWindow::realTimeDiameter(){
    int size = r1FromMCU.size();//Размер данных, которые надо добавить на график
    for(int i = 0;i<size; i++){
        xDiameter.append(lastIndex++);
    }
    yr1.append(r1FromMCU);
    yr2.append(r2FromMCU);
    yc1.append(c1FromMCU);
    yc2.append(c2FromMCU);
    ym1.append(m1FromMCU);
    ym2.append(m2FromMCU);
    filled+=size;

    int overload = filled-xWindowDiameter;//Если данных больше, чем окно, отрезаем хвост
    if(overload>=0){
        if(yr1.size()>overload){
            xDiameter.remove(0,overload);
            yr1.remove(0,overload);
            yr2.remove(0,overload);
            yc1.remove(0,overload);
            yc2.remove(0,overload);
            ym1.remove(0,overload);
            ym2.remove(0,overload);
            filled-=overload;
        }
        else{
            xDiameter.clear();
            yr1.clear();
            yr2.clear();
            yc1.clear();
            yc2.clear();
            ym1.clear();
            ym2.clear();
            filled=0;
        }
    }
    plotDiameter();
}

void MainWindow::collectDiameter(){
    int size = r1FromMCU.size();//Размер принятых данных

    yr1.append(r1FromMCU);
    yr2.append(r2FromMCU);
    yc1.append(c1FromMCU);
    yc2.append(c2FromMCU);
    ym1.append(m1FromMCU);
    ym2.append(m2FromMCU);
    currentCollected+=size;
    m_ManagementWidget->m_DiameterTransmition->progressBar->setValue(currentCollected);

    if(currentCollected>=countOfCollect){
        m_ManagementWidget->m_DiameterTransmition->progressBar->setValue(countOfCollect);
        m_ManagementWidget->m_DiameterTransmition->gettingDiameterButton->click();
        lastIndex=0;
        for(int i = 0;i<currentCollected; i++){
            xDiameter.append(lastIndex++);
        }
        plotDiameter();
        currentCollected = 0;
        clearDiameterVectors();
    }
}

void MainWindow::clearDiameterVectors(){
    yr1.clear();
    yr2.clear();
    yc1.clear();
    yc2.clear();
    ym1.clear();
    ym2.clear();
    xDiameter.clear();
}

void MainWindow::mouseWheel(){
  // if an axis is selected, only allow the direction of that axis to be zoomed
  // if no axis is selected, both directions may be zoomed

  if (diameterPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
      diameterPlot->axisRect()->setRangeZoom(diameterPlot->xAxis->orientation());

  else if (diameterPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    diameterPlot->axisRect()->setRangeZoom(diameterPlot->yAxis->orientation());

  else
    diameterPlot->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}
