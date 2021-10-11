#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "serialsettings.h"

#include "slip.h"
#include "transp.h"
#include "statusbar.h"
#include <QSplitter>

#define TEST_MODE

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
    m_ManagementWidget->setMinimumWidth(250);
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
    diameterPlot->addGraph();

    //Настройка CustomPlot
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

    // зумируется только выделенная ось
    //connect(customPlot1, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress1(QMouseEvent*)));
    //connect(customPlot1, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel1()));


    // При зумировании одной оси зизменяется диапазон противоположной
    //connect(customPlot1->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot1->xAxis2, SLOT(setRange(QCPRange)));
    //connect(customPlot1->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot1->yAxis2, SLOT(setRange(QCPRange)));


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
          m_ManagementWidget->m_TransmitionSettings->autoGetCheckBox->setChecked(false);
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
    connect(m_ManagementWidget->m_TransmitionSettings,&TransmitionSettings::setPacketSize,this,&MainWindow::setPacketSize);
    connect(m_ManagementWidget->m_TransmitionSettings,&TransmitionSettings::chChooseChanged,this,&MainWindow::incCountCh);
    connect(m_ManagementWidget->m_TransmitionSettings,&TransmitionSettings::getButtonClicked,this,&MainWindow::manualGetShotButton);
    connect(m_ManagementWidget->m_TransmitionSettings,&TransmitionSettings::autoGetCheckBoxChanged,this,&MainWindow::autoGetCheckBoxChanged);

    //Коннекты от истории
    connect(m_ManagementWidget->m_HistorySettings,&HistorySettings::autoSaveShotCheked,this,&MainWindow::autoSaveShotCheked);
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
    connect(m_ManagementWidget->m_DiameterTransmition,&DiameterTransmition::reqFreqValueChanged,[=](int value){
            m_GettingDiameterTimer->setInterval(1000/value);
            //m_GettingDiameterTimer->start();
    });


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

#ifdef TEST_MODE
   constructorTest();
#endif

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

    filename = "2021_09_16__19_35_37_CH1";
    tempFile->setFileName(dirname + "/" + filename);
    if(!tempFile->open(QIODevice::ReadOnly)){
        qDebug() << "tempFile can`t be open";
        return;
    }
    QByteArray tempBuf = tempFile->readAll();                                           //Читаем большой буфер с несколькими кадрами
    QList<QByteArray> list_tempCH1=tempBuf.split(0xFF);                                    //разделяем кадры
    tempFile->close();

    filename = "2021_09_16__19_35_37_CH2";
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
        m_ManagementWidget->m_HistorySettings->shotsComboBox->addItem(QString::number(i));
    }

}

QByteArray MainWindow::generateBytes(int count)
{
    QVector<double> vector;
    QByteArray data;
    for (int i=0;i<count;i++)
        vector.append(QRandomGenerator::global()->generateDouble());

    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << vector;
    //data.remove(0,4);
//    data = QByteArray::fromRawData(
//            reinterpret_cast<const char*>(vector.constData()),
//            sizeof(double) * vector.size()
//        );
    //QByteArray img(reinterpret_cast<const char*>(vector.data()), vector.size());
    return data;
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

        m_ManagementWidget->m_HistorySettings->autoSaveShotCheckBox->setEnabled(true);
        if(channelsOrder!=0)
            sendByteToMK(CH_ORDER,channelsOrder,"SEND CH_ORDER: ");

        m_ManagementWidget->m_plisSettings->lazer1Button->setEnabled(true);
        m_ManagementWidget->m_plisSettings->lazer2Button->setEnabled(true);
        m_ManagementWidget->m_plisSettings->saveButton->setEnabled(true);


        m_ManagementWidget->m_TransmitionSettings->ch1CheckBox->setEnabled(true);
        m_ManagementWidget->m_TransmitionSettings->ch2CheckBox->setEnabled(true);
        m_ManagementWidget->m_TransmitionSettings->ch2InCheckBox->setEnabled(true);
        m_ManagementWidget->m_TransmitionSettings->ch3CheckBox->setEnabled(true);
        m_ManagementWidget->m_TransmitionSettings->ch4CheckBox->setEnabled(true);
        m_ManagementWidget->m_TransmitionSettings->ch4InCheckBox->setEnabled(true);

        m_ManagementWidget->m_TransmitionSettings->ch1CheckBox->setChecked(false);
        m_ManagementWidget->m_TransmitionSettings->ch2CheckBox->setChecked(false);
        m_ManagementWidget->m_TransmitionSettings->ch2InCheckBox->setChecked(false);
        m_ManagementWidget->m_TransmitionSettings->ch3CheckBox->setChecked(false);
        m_ManagementWidget->m_TransmitionSettings->ch4CheckBox->setChecked(false);
        m_ManagementWidget->m_TransmitionSettings->ch4InCheckBox->setChecked(false);
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

    m_ManagementWidget->m_HistorySettings->autoSaveShotCheckBox->setEnabled(false);
    m_ManagementWidget->m_HistorySettings->autoSaveShotCheckBox->setChecked(false);

    //Вырубаем автополучение на всякий
    m_ManagementWidget->m_TransmitionSettings->getButton->setEnabled(false);
    m_ManagementWidget->m_TransmitionSettings->autoGetCheckBox->setChecked(false);

    m_ManagementWidget->m_TransmitionSettings->ch1CheckBox->setEnabled(false);
    m_ManagementWidget->m_TransmitionSettings->ch2CheckBox->setEnabled(false);
    m_ManagementWidget->m_TransmitionSettings->ch2InCheckBox->setEnabled(false);
    m_ManagementWidget->m_TransmitionSettings->ch3CheckBox->setEnabled(false);
    m_ManagementWidget->m_TransmitionSettings->ch4CheckBox->setEnabled(false);
    m_ManagementWidget->m_TransmitionSettings->ch4InCheckBox->setEnabled(false);

    m_ManagementWidget->m_plisSettings->lazer1Button->setEnabled(false);
    m_ManagementWidget->m_plisSettings->lazer2Button->setEnabled(false);
    m_ManagementWidget->m_plisSettings->saveButton->setEnabled(false);


}

void MainWindow::sendByteToMK(char dst, char dataByte, const QString &msg)
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
    conversation conv;
    QByteArray data;
    data.append(dst);
    for(double d:dataV){
        conv.d=d;
        for(int i=0;i<8;i++)
        data.append(conv.ch[i]);
    }

    m_console->putData(msg.toUtf8());
    m_transp->sendPacket(data);
}

//Подсчет количества отмеченных каналов
void MainWindow::incCountCh(int ch)
{
    switch (ch){
    case 1:
       if(m_ManagementWidget->m_TransmitionSettings->ch1CheckBox->isChecked())
           channelsOrder|=0x01;
       else
          channelsOrder&=~0x01;
        break;
    case 2:
       if(m_ManagementWidget->m_TransmitionSettings->ch2CheckBox->isChecked())
           channelsOrder|=0x02;
       else
          channelsOrder&=~0x02;
        break;
    case 3:
       if(m_ManagementWidget->m_TransmitionSettings->ch3CheckBox->isChecked())
           channelsOrder|=0x04;
       else
          channelsOrder&=~0x04;
        break;
    case 4:
       if(m_ManagementWidget->m_TransmitionSettings->ch4CheckBox->isChecked())
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
         m_ManagementWidget->m_TransmitionSettings->autoGetCheckBox->setEnabled(true);
    else
        m_ManagementWidget->m_TransmitionSettings->autoGetCheckBox->setEnabled(false);
    sendByteToMK(CH_ORDER,channelsOrder,"SEND CH_ORDER: ");
}

//Настройка разбиения данных на пакеты
void MainWindow::setPacketSize(int n){
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
        m_ManagementWidget->m_TransmitionSettings->ch1CheckBox->setEnabled(false);
        m_ManagementWidget->m_TransmitionSettings->ch2CheckBox->setEnabled(false);
        m_ManagementWidget->m_TransmitionSettings->ch3CheckBox->setEnabled(false);
        m_ManagementWidget->m_TransmitionSettings->ch4CheckBox->setEnabled(false);
        m_ManagementWidget->m_TransmitionSettings->getButton->setEnabled(false);
    }
    else {
        m_ManagementWidget->m_TransmitionSettings->ch1CheckBox->setEnabled(true);
        m_ManagementWidget->m_TransmitionSettings->ch2CheckBox->setEnabled(true);
        m_ManagementWidget->m_TransmitionSettings->ch3CheckBox->setEnabled(true);
        m_ManagementWidget->m_TransmitionSettings->ch4CheckBox->setEnabled(true);
        m_ManagementWidget->m_TransmitionSettings->getButton->setEnabled(true);
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
        if (!m_ManagementWidget->m_TransmitionSettings->ch1CheckBox->isChecked() && !m_ManagementWidget->m_TransmitionSettings->ch2CheckBox->isChecked()
            && !m_ManagementWidget->m_TransmitionSettings->ch3CheckBox->isChecked() && !m_ManagementWidget->m_TransmitionSettings->ch4CheckBox->isChecked()){
            statusBar->setMessageBar("ОШИБКА!, Не выбрано ни одного канала!");
            return;
        }
        m_timer->stop();
        m_ManagementWidget->m_TransmitionSettings->getButton->setEnabled(false);                                                   //Защита от двойного нажатия кнопки
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
        countAvaibleDots=value;
        if (value != NO_DATA_READY) {
            dataReady = value;

            //Забираем 16 байт метаданных
            tempPLISextremums2.clear();
            tempPLISextremums1.clear();
            for(int i=0;i<8;i+=2){
                tempPLISextremums2.prepend(static_cast <unsigned char> (bytes.at(i+1))*256+static_cast <unsigned char>(bytes.at(i))+0);
                tempPLISextremums1.prepend(static_cast <unsigned char> (bytes.at(i+9))*256+static_cast <unsigned char>(bytes.at(i+8))+0);
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

            m_MainControlWidget->m_signalErrWidget->setVal(bytes.at(0),1);   //Младшие значащие байты
            m_MainControlWidget->m_signalErrWidget->setVal(bytes.at(2),2);
            m_ManagementWidget->m_plisSettings->lazer1Button->setText(QString::number(static_cast <unsigned char> (bytes.at(5))*256+static_cast <unsigned char>(bytes.at(4))));
            m_ManagementWidget->m_plisSettings->lazer2Button->setText(QString::number(static_cast <unsigned char> (bytes.at(7))*256+static_cast <unsigned char>(bytes.at(6))));
            m_ManagementWidget->m_plisSettings->borderLeftButton->setText(QString::number(static_cast <unsigned char> (bytes.at(9))*256+static_cast <unsigned char>(bytes.at(8))));
            m_ManagementWidget->m_plisSettings->borderRightButton->setText(QString::number(static_cast <unsigned char> (bytes.at(11))*256+static_cast <unsigned char>(bytes.at(10))));
            m_ManagementWidget->m_plisSettings->compCH1Button->setText(QString::number(static_cast <unsigned char> (bytes.at(13))*256+static_cast <unsigned char>(bytes.at(12))));
            m_ManagementWidget->m_plisSettings->compCH2Button->setText(QString::number(static_cast <unsigned char> (bytes.at(15))*256+static_cast <unsigned char>(bytes.at(14))));
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
                m_MainControlWidget->m_resultWidget->radius1->setText("   Радиус1: " + QString::number(diameterPlis.at(0)));
                m_MainControlWidget->m_resultWidget->radius2->setText("   Радиус2: " + QString::number(diameterPlis.at(1)));
            }

            if(m_ManagementWidget->m_TransmitionSettings->autoGetCheckBox->isChecked() || notYetFlag)                       //Если включен автозапрос данных или не вычитали все пачку каналов
                manualGetShotButton();                                                  //Запрашиваем шот
        }
        else {
            dataReady = 0;
            m_console->putData("Warning: MCU has no data\n\n");
        }
        emit dataReadyUpdate(dataReady);
        m_timer->start();                                                              //Если получили статус, то можно запрашивать еще
        break;

     case REQUEST_DIAMETER:
        fromBytes(bytes);
        plotDiameter();

        break;

     case REQUEST_POINTS:
        if ((value==CH1)|| (value==CH2) || (value==CH3) || (value==CH4)){                                                //Если пришли точки по одному из каналов, то обрабатываем
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

                    if(m_ManagementWidget->m_TransmitionSettings->ch2InCheckBox->isChecked()) {                                                      //Если нужна фильтрация
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
                   //currentShot=currentShot.mid(20);                                 //Смещение отфильтрованного сигнала из плисы
                   //currentShot.append(20,0);
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
                     //currentShot=currentShot.mid(20);                                 //Смещение отфильтрованного сигнала из плисы
                     //currentShot.append(20,0);
                     shotsCH4.insert(shotCountRecieved,currentShot);
                     m_console->putData(" :RECIEVED ANSWER_POINTS CH2_F  ");
                     chCountRecieved++;                                                  //Получили канал
                }

                //Если включено автосохранение в файл
                if(m_ManagementWidget->m_HistorySettings->autoSaveShotCheckBox->isChecked())
                    writeToLogfile(chName);
                //Обнуляем всякое
                countRecievedDots=0;                                                    //Обнуляем количество пришедших точек
                currentShot.clear();                                                    //Чистим временное хранилище текущего принимаемого канала
                statusBar->setInfo(m_transp->getQueueCount());                          //Обновляем статус бар

                if (chCountRecieved == chCountChecked){                                 //Если приняли все заправшиваемые каналы                                                  //Все точки всех отмеченных каналов приняты
                    m_console->putData("\n\n");
                    chCountRecieved=0;
                    shotCountRecieved++;                                                //Увеличиваем счетчик пачек
                    m_ManagementWidget->m_HistorySettings->shotsComboBox->addItem(QString::number(shotCountRecieved-1));
                    //shotsComboBox->setCurrentIndex(shotCountRecieved-1);
                    m_ManagementWidget->m_HistorySettings->shotsComboBox->setCurrentIndex(m_ManagementWidget->m_HistorySettings->shotsComboBox->count()-1);
                    if (!m_ManagementWidget->m_TransmitionSettings->autoGetCheckBox->isChecked())                                  //Если не стоит автополучение, то можно разблокировать кнопку
                        m_ManagementWidget->m_TransmitionSettings->getButton->setEnabled(true);
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
void MainWindow::selectShot(int index){
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

            viewer->addLines(shadowsCh1,1,2);
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
            m_MainControlWidget->m_resultWidget->leftShadow1Label->setText("   Лев. тень: " +QString::number(shadowsCh1.at(0)));
            m_MainControlWidget->m_resultWidget->rightShadow1Label->setText("   Прав. тень: " +QString::number(shadowsCh1.at(1)));
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

            viewer->addLines(shadowsCh2,2,2);
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

            m_MainControlWidget->m_resultWidget->leftShadow2Label->setText("   Лев. тень: " + QString::number(shadowsCh2.at(0)));
            m_MainControlWidget->m_resultWidget->rightShadow2Label->setText("   Прав. тень: " + QString::number(shadowsCh2.at(1)));

        }
        if(shadowsCh1.size()>1 && shadowsCh2.size()>1){
            diameter = filter->diameterFind(shadowsCh1,shadowsCh2);
            m_MainControlWidget->m_resultWidget->diametrLabel->setText("Диаметр: " +QString::number(diameter.at(0) + diameter.at(1)));
            m_MainControlWidget->m_resultWidget->m_centerViewer->setCoord(static_cast<int>(diameter.at(2)/1000),static_cast<int>(diameter.at(3)/1000));
            m_MainControlWidget->m_resultWidget->centerPositionLabel->setText("Смещение: " + QString::number(diameter.at(1)) + ", " + QString::number(diameter.at(2)));

        }
    }
}

//Автосохранение
void MainWindow::autoSaveShotCheked(bool en)
{
    if(en){
        if(m_ManagementWidget->m_TransmitionSettings->ch1CheckBox->isChecked()){
            filename = QDate::currentDate().toString("yyyy_MM_dd") + QTime::currentTime().toString("__hh_mm_ss")+"_CH1";
            file1->setFileName(dirname + "/" + filename);
            file1->open(QIODevice::WriteOnly);
        }
        if(m_ManagementWidget->m_TransmitionSettings->ch2CheckBox->isChecked()){
            filename = QDate::currentDate().toString("yyyy_MM_dd") + QTime::currentTime().toString("__hh_mm_ss")+"_CH1F";
            file2->setFileName(dirname + "/" + filename);
            file2->open(QIODevice::WriteOnly);
        }

        if(m_ManagementWidget->m_TransmitionSettings->ch3CheckBox->isChecked()){
            filename = QDate::currentDate().toString("yyyy_MM_dd") + QTime::currentTime().toString("__hh_mm_ss")+"_CH2";
            file3->setFileName(dirname + "/" + filename);
            file3->open(QIODevice::WriteOnly);
        }
        if(m_ManagementWidget->m_TransmitionSettings->ch4CheckBox->isChecked()){
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
    m_ManagementWidget->m_HistorySettings->shotsComboBox->clear();
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

// Обработчик таймаута запроса диаметра
void MainWindow::handlerGettingDiameterTimer(){
#ifdef TEST_MODE
    QByteArray randomBytes;
    randomBytes = generateBytes(1000/m_ManagementWidget->m_DiameterTransmition->reqFreqSpinbox->value());
    randomBytes.prepend(2,0);
    randomBytes.prepend(REQUEST_DIAMETER);
    handlerTranspAnswerReceive(randomBytes);
#endif
    //sendByteToMK(REQUEST_DIAMETER,0,"SEND REQUEST_DIAMETER: ");
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

void MainWindow::writeToLogfileMeta(QString name)
{
    QFile tempFile;
    tempFile.setFileName(dirname + "/" + QDate::currentDate().toString("yyyy_MM_dd") + QTime::currentTime().toString("__hh_mm_ss")+ name);
    tempFile.open(QIODevice::WriteOnly);
    tempFile.write(currentShot,32);
    tempFile.write(endShotLine,endShotLine.size());
    tempFile.flush();
    tempFile.close();
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
    ShadowSettings->show();
}

void MainWindow::settingsChanged()
{
    filter->updateSettings(ShadowSettings->getShadowFindSettings());//Забрали обновленные настройки
}

QVector<double> MainWindow::fromBytes(QByteArray &bytes)
{
    QDataStream stream(bytes);
    //stream.setByteOrder(QDataStream::LittleEndian);
    //diameterKeys.clear();
    //diametersFromMCU.clear();
    //diametersFromMCU.resize(80);
    //for(int i=0;i<80;i++)
    //    stream >> diametersFromMCU[i];
    stream >> diametersFromMCU;
    for(int i=0;i<diametersFromMCU.size();i++)
        diameterKeys.append(i);
    return diametersFromMCU;
}

void MainWindow::plotDiameter()
{
    int size = diametersFromMCU.size();
    diameterKeys.resize(size);
    for(int i = 0;i<size; i++){
        diameterKeys[i]=lastIndex++;
    }
    int overload = filled+size-5000;
    if(overload>=0){
        xDiameter.remove(0,overload);
        yDiameter.remove(0,overload);
        filled-=overload;
    }
    xDiameter.append(diameterKeys);
    yDiameter.append(diametersFromMCU);
    filled+=size;

    diameterPlot->graph()->setData(xDiameter,yDiameter);
    diameterPlot->xAxis->rescale();

    diameterPlot->xAxis->setRange(diameterPlot->xAxis->range().upper, 5000, Qt::AlignRight);
    diameterPlot->replot();
}
