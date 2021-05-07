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
    customPlot = new QCustomPlot();

    //Настройка CustomPlot
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables);
    customPlot->axisRect()->setupFullAxesBox();

    //customPlot->plotLayout()->insertRow(0);
    //QCPTextElement *title = new QCPTextElement(customPlot, "Видеосигнал", QFont("sans", 17, QFont::Bold));
    //customPlot->plotLayout()->addElement(0, 0, title);

    customPlot->xAxis->setLabel("X");
    customPlot->xAxis->setRangeLower(0);
    customPlot->xAxis->setRangeUpper(11000);
    customPlot->yAxis->setLabel("Y");
    customPlot->yAxis->setRangeLower(-1);
    customPlot->yAxis->setRangeUpper(260);
    customPlot->legend->setVisible(true);
    QFont legendFont = font();
    legendFont.setPointSize(10);
    customPlot->legend->setFont(legendFont);
    customPlot->legend->setSelectedFont(legendFont);
    customPlot->legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items


    // Выделение одной оси, ведет к выделению противоположной
    connect(customPlot, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
    // зумируется только выделенная ось
    connect(customPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
    connect(customPlot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));

    // При зумировании одной оси зизменяется диапазон противоположной
    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

    // connect some interaction slots:
    connect(customPlot, SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)), this, SLOT(axisLabelDoubleClick(QCPAxis*,QCPAxis::SelectablePart)));
    //connect(customPlot, SIGNAL(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)), this, SLOT(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*)));
    //connect(title, SIGNAL(doubleClicked(QMouseEvent*)), this, SLOT(titleDoubleClick(QMouseEvent*)));

    //connect slot that shows a message in the status bar when a graph is clicked:
    connect(customPlot, SIGNAL(plottableClick(QCPAbstractPlottable*,int,QMouseEvent*)), this, SLOT(graphClicked(QCPAbstractPlottable*,int)));

    //Интерфейс
    layoutV = new QVBoxLayout;
    centralWidget()->setLayout(layoutV);

    layoutH = new QHBoxLayout;
    layoutV->addLayout(layoutH);
    layoutV->addWidget(m_console);

    controlLayout = new QVBoxLayout;
    layoutH->addWidget(customPlot);
    layoutH->addLayout(controlLayout);
    customPlot->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

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
    ch1CheckBox = new QCheckBox("Канал 1. Карловы Пивовары");
    ch2CheckBox = new QCheckBox("Канал 1. Нефильтрофф");
    ch3CheckBox = new QCheckBox("Канал 2. Карловы Пивовары");
    ch4CheckBox = new QCheckBox("Канал 2. Нефильтрофф");
    getButton = new QPushButton("Получить снимок");
    autoGetCheckBox = new QCheckBox("Авто-получение по готовности");
    transmitLayout->addWidget(packetSizeLabel);
    transmitLayout->addWidget(packetSizeSpinbox);
    transmitLayout->addWidget(ch1CheckBox);
    transmitLayout->addWidget(ch2CheckBox);
    transmitLayout->addWidget(ch3CheckBox);
    transmitLayout->addWidget(ch4CheckBox);
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
    delete customPlot;
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
        m_timer->start(500);
        statusBar->clearReSent();
        getButton->setEnabled(true);
        m_transp->clearQueue();
        serial->readAll();
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
    statusBar->setDownloadBarValue(0);
    emit dataReadyUpdate(-1);
    nowShotCH1.clear();
    nowShotCH2.clear();
    nowShotCH3.clear();
    nowShotCH4.clear();
}

//Подсчет количества отмеченных каналов
void MainWindow::incCountCh(bool st){
    if(st) chCountChecked++;
    else   chCountChecked--;

    if(chCountChecked)  autoGetCheckBox->setEnabled(true);
    else                autoGetCheckBox->setEnabled(false);
}
//Настройка разбиения данных на пакеты
void MainWindow::setPacketSize(short n){
    packetSize=n;
}
//Запрость у MCU пакет длины n с канала ch
void MainWindow::getPacketFromMCU(short n, const unsigned short ch)
{
    QByteArray data;
    char msb,lsb;

    data.append(ch);
    msb=(n&0xFF00)>>8;
    lsb=n&0x00FF;
    data.append(msb);
    data.append(lsb);
    m_transp->sendPacket(data);
}

//Запихиваем в очередь Х запросов в соответствии с разбивкой по пакетам, установленной в спинбоксе и отмеченным каналам
void MainWindow::manualGetShotButton(){
    if(countAvaibleDots){
        if (!ch1CheckBox->isChecked() && !ch2CheckBox->isChecked() && !ch3CheckBox->isChecked() && !ch4CheckBox->isChecked()){
            QMessageBox::critical(nullptr,"Ошибка!","Не выбрано ни одного канала для получения данных");
            return;
        }

        m_console->putData("REQUEST_POINTS: ");
        m_timer->stop();
        statusBar->setDownloadBarRange(countAvaibleDots*chCountChecked);


        if(ch1CheckBox->isChecked())     countAvaibleDotsCH1 = countAvaibleDots;
        if(ch2CheckBox->isChecked())     countAvaibleDotsCH2 = countAvaibleDots;
        if(ch3CheckBox->isChecked())     countAvaibleDotsCH3 = countAvaibleDots;
        if(ch4CheckBox->isChecked())     countAvaibleDotsCH4 = countAvaibleDots;

        while (countAvaibleDotsCH1>0){                                 //Отправляем запрос несоклько раз по packetSize точек.
            getPacketFromMCU(countAvaibleDotsCH1>packetSize?packetSize:countAvaibleDotsCH1,CH1);
            countAvaibleDotsCH1-=packetSize;
        }
        while (countAvaibleDotsCH2>0){                                 //Отправляем запрос несоклько раз по packetSize точек.
            getPacketFromMCU(countAvaibleDotsCH2>packetSize?packetSize:countAvaibleDotsCH2,CH2);
            countAvaibleDotsCH2-=packetSize;
        }
        while (countAvaibleDotsCH3>0){                                 //Отправляем запрос несоклько раз по packetSize точек.
            getPacketFromMCU(countAvaibleDotsCH3>packetSize?packetSize:countAvaibleDotsCH3,CH3);
            countAvaibleDotsCH3-=packetSize;
        }
        while (countAvaibleDotsCH4>0){                                 //Отправляем запрос несоклько раз по packetSize точек.
            getPacketFromMCU(countAvaibleDotsCH4>packetSize?packetSize:countAvaibleDotsCH4,CH4);
            countAvaibleDotsCH4-=packetSize;
        }
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
    customPlot->rescaleAxes();
    customPlot->yAxis->setRange(customPlot->yAxis->range().lower-1,customPlot->yAxis->range().upper+5);
    customPlot->xAxis->setRange(customPlot->xAxis->range().lower,customPlot->xAxis->range().upper );
    customPlot->replot();
}

//Выбрать шот из списка
void MainWindow::selectShot(int index){
    if(!shotsCH1.isEmpty() || !shotsCH2.isEmpty() ||!shotsCH3.isEmpty() ||!shotsCH4.isEmpty()){
        QByteArray ch;
        if(customPlot->graphCount()!=0)
            customPlot->clearGraphs();

        if(shotsCH1.contains(index)){
            ch = shotsCH1[index];
            addUserGraph(ch,ch.size(),1);
        }
        if(shotsCH2.contains(index)){
            ch = shotsCH2[index];
            addUserGraph(ch,ch.size(),2);
        }
        if(shotsCH3.contains(index)){
            ch = shotsCH3[index];
            addUserGraph(ch,ch.size(),3);
        }
        if(shotsCH4.contains(index)){
            ch = shotsCH4[index];
            addUserGraph(ch,ch.size(),4);
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
    customPlot->clearGraphs();
    customPlot->replot();
}

//Обработка входящих пакетов
void MainWindow::handlerTranspAnswerReceive(QByteArray &bytes) {
    unsigned char cmd = bytes[0];
    unsigned short value = (unsigned char)bytes[1]*256+(unsigned char)bytes[2];
    int dataReady=-1;
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


    case REQUEST_STATUS:                                                    //Пришло количество точек
        m_console->putData(" :RECIEVED ANSWER_STATUS\n\n");
        countAvaibleDots=value;
        if (value != NO_DATA_READY) {
            dataReady = value;
            if(autoGetCheckBox->isChecked()){                                //Если включен автозапрос данных
                manualGetShotButton();                                       //Запрашиваем шот
            }
        }
        else {
            dataReady = 0;
            m_console->putData("Warning: MCU has no data\n\n");
        }
        emit dataReadyUpdate(dataReady);
        break;


     case REQUEST_POINTS:
        if ((value == CH1*256+CH1) || (value == CH2*256+CH2) || (value == CH3*256+CH3) || (value == CH4*256+CH4)){
            bytes.remove(0, 3);                                                             //Удалили 3 байта (команду и значение)
            countRecievedDots+=bytes.count();
            statusBar->setDownloadBarValue(countRecievedDots);
            if (value == CH1*256+CH1) {
                nowShotCH1.append(bytes);                                                     //Добавили пришедшие байты в шот
                 m_console->putData(" :RECIEVED ANSWER_POINTS CH1  ");
            }
            else if(value == CH2*256+CH2){
                 nowShotCH2.append(bytes);
                  m_console->putData(" :RECIEVED ANSWER_POINTS CH2  ");
            }
            else if(value == CH3*256+CH3){
                 nowShotCH3.append(bytes);
                 m_console->putData(" :RECIEVED ANSWER_POINTS CH3  ");
            }
            else if(value == CH4*256+CH4){
                 nowShotCH4.append(bytes);
                 m_console->putData(" :RECIEVED ANSWER_POINTS CH4  ");
            }

            if (countRecievedDots == statusBar->getDownloadBarRange()){     //Все точки всех отмеченных каналов приняты
                m_console->putData("\n\n");
                //Раскладываем принятое по контенерам. Отдельно хранятся 4 канала в мапе.
                if(!nowShotCH1.isEmpty())
                    shotsCH1.insert(shotCountRecieved,nowShotCH1);
                if(!nowShotCH2.isEmpty())
                    shotsCH2.insert(shotCountRecieved,nowShotCH2);
                if(!nowShotCH3.isEmpty())
                    shotsCH3.insert(shotCountRecieved,nowShotCH3);
                if(!nowShotCH4.isEmpty())
                    shotsCH4.insert(shotCountRecieved,nowShotCH4);

                shotCountRecieved++;
                countRecievedDots=0;
                countAvaibleDots=0;

                shotsComboBox->addItem(QString::number(shotCountRecieved));
                shotsComboBox->setCurrentIndex(shotCountRecieved-1);

                //Если включено автосохранение
                if(autoSaveShotCheckBox->isChecked()){
                    if (dirname.isEmpty()){
                        QMessageBox::critical(nullptr,"Ошибка!","Директория для сохранения данных отсутствует!");
                    }
                    else{
                        filename=QString::number(shotCountRecieved);
                        file.setFileName(dirname + "/" + filename + ".txt");
                        if(file.open(QIODevice::WriteOnly) == true){
                            file.write(nowShotCH1);
                            file.write(nowShotCH2);
                            file.write(nowShotCH3);
                            file.write(nowShotCH4);
                            file.close();
                         }
                    }
                }
                nowShotCH1.clear();
                nowShotCH2.clear();
                nowShotCH3.clear();
                nowShotCH4.clear();
                statusBar->setInfo(m_transp->getQueueCount());
                m_timer->start();
               QByteArray data;
               if (m_online) {                                                                     //Сразу после запроса данных запрашиваем статус
                   data.append(REQUEST_STATUS);
                   m_console->putData("SEND REQUEST_STATUS: ");
                   m_transp->sendPacket(data);
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
    for (int i=0; i<len; i++){
      x[i] = i;
      y[i] = (unsigned char)buf.at(i);
    }
    customPlot->addGraph();
    customPlot->graph()->setName(QString("Канал %1").arg(ch));
    customPlot->graph()->setData(x, y);

    QPen graphPen;
    QColor color;
    if (ch==1)
        color =  QColorConstants::Black;
    if (ch==2)
        color =  QColorConstants::DarkGray;
    if (ch==3)
        color =  QColorConstants::DarkRed;
    if (ch==4)
        color =  QColorConstants::Red;

    graphPen.setColor(color);
    customPlot->graph()->setPen(graphPen);


    customPlot->replot();
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
      customPlot->replot();
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
      customPlot->replot();
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
      customPlot->replot();
    }
  }
}
void MainWindow::selectionChanged(){
  // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || customPlot->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    customPlot->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    customPlot->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
  // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || customPlot->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    customPlot->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    customPlot->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
  // synchronize selection of graphs with selection of corresponding legend items:
  for (int i=0; i<customPlot->graphCount(); ++i){
    QCPGraph *graph = customPlot->graph(i);
    QCPPlottableLegendItem *item = customPlot->legend->itemWithPlottable(graph);
    if (item->selected() || graph->selected())    {
      item->setSelected(true);
      graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
    }
  }
}
void MainWindow::mousePress(){
  // if an axis is selected, only allow the direction of that axis to be dragged
  // if no axis is selected, both directions may be dragged

  if (customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis)){
    customPlot->axisRect()->setRangeDrag(customPlot->xAxis->orientation());
    if(customPlot->xAxis->range().lower <0)
         customPlot->xAxis->setRangeLower(0);
    if(customPlot->xAxis->range().upper > 11000)
         customPlot->xAxis->setRangeUpper(11000);
  }
  else if (customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis)){
   customPlot->axisRect()->setRangeDrag(customPlot->yAxis->orientation());
   if(customPlot->yAxis->range().lower <-1)
        customPlot->yAxis->setRangeLower(-1);
   if(customPlot->yAxis->range().upper >260)
        customPlot->yAxis->setRangeUpper(260);
  }
  else{
    customPlot->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
    if(customPlot->xAxis->range().lower <0)
         customPlot->xAxis->setRangeLower(0);
    if(customPlot->xAxis->range().upper > 11000)
         customPlot->xAxis->setRangeUpper(11000);
    if(customPlot->yAxis->range().lower <-1)
         customPlot->yAxis->setRangeLower(-1);
    if(customPlot->yAxis->range().upper >260)
         customPlot->yAxis->setRangeUpper(260);
  }
}
void MainWindow::mouseWheel(){
  // if an axis is selected, only allow the direction of that axis to be zoomed
  // if no axis is selected, both directions may be zoomed

  if (customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis)){
      customPlot->axisRect()->setRangeZoom(customPlot->xAxis->orientation());
      if(customPlot->xAxis->range().lower <0)
           customPlot->xAxis->setRangeLower(0);
      if(customPlot->xAxis->range().upper > 11000)
           customPlot->xAxis->setRangeUpper(11000);
   }

  else if (customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis)){
    customPlot->axisRect()->setRangeZoom(customPlot->yAxis->orientation());
    if(customPlot->yAxis->range().lower <-1)
         customPlot->yAxis->setRangeLower(-1);
    if(customPlot->yAxis->range().upper >260)
         customPlot->yAxis->setRangeUpper(260);
  }
  else{
    customPlot->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
    if(customPlot->xAxis->range().lower <0)
         customPlot->xAxis->setRangeLower(0);
    if(customPlot->xAxis->range().upper > 11000)
         customPlot->xAxis->setRangeUpper(11000);
    if(customPlot->yAxis->range().lower <-1)
         customPlot->yAxis->setRangeLower(-1);
    if(customPlot->yAxis->range().upper >260)
         customPlot->yAxis->setRangeUpper(260);
   }
}
void MainWindow::moveLegend(){
  if (QAction* contextAction = qobject_cast<QAction*>(sender())) // make sure this slot is really called by a context menu action, so it carries the data we need
  {
    bool ok;
    int dataInt = contextAction->data().toInt(&ok);
    if (ok)
    {
      customPlot->axisRect()->insetLayout()->setInsetAlignment(0, (Qt::Alignment)dataInt);
      customPlot->replot();
    }
  }
}
void MainWindow::graphClicked(QCPAbstractPlottable *plottable, int dataIndex){
  // since we know we only have QCPGraphs in the plot, we can immediately access interface1D()
  // usually it's better to first check whether interface1D() returns non-zero, and only then use it.
  double dataValue = plottable->interface1D()->dataMainValue(dataIndex);
  QString message = QString("Clicked on graph '%1' at data point #%2 with value %3.").arg(plottable->name()).arg(dataIndex).arg(dataValue);
  ui->statusBar->showMessage(message, 2500);
}
