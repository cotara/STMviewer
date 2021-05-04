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
    m_console->setMaximumHeight(200);

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
    std::srand(QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0);

    //Настройка CustomPlot
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables);
    customPlot->axisRect()->setupFullAxesBox();

    customPlot->plotLayout()->insertRow(0);
    QCPTextElement *title = new QCPTextElement(customPlot, "Видеосигнал", QFont("sans", 17, QFont::Bold));
    customPlot->plotLayout()->addElement(0, 0, title);

    customPlot->xAxis->setLabel("X");
    customPlot->yAxis->setLabel("Y");
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
    connect(title, SIGNAL(doubleClicked(QMouseEvent*)), this, SLOT(titleDoubleClick(QMouseEvent*)));

    // connect slot that shows a message in the status bar when a graph is clicked:
    //connect(customPlot, SIGNAL(plottableClick(QCPAbstractPlottable*,int,QMouseEvent*)), this, SLOT(graphClicked(QCPAbstractPlottable*,int)));



    //Интерфейс
    layoutH = new QHBoxLayout;
    layoutV = new QVBoxLayout;
    controlLayout = new QVBoxLayout;
    controlGroup = new QGroupBox;
    packetSizeSpinbox = new QSpinBox;
    getButton = new QPushButton;
    autoGetCheckBox = new QCheckBox;
    autoSaveShotCheckBox = new QCheckBox;
    consoleEnable = new QCheckBox;
    shotsComboBox = new QComboBox;
    clearButton = new QPushButton;

    centralWidget()->setLayout(layoutV);
    layoutV->addLayout(layoutH);

    layoutV->addWidget(m_console);
    layoutH->addWidget(customPlot);
    layoutH->addWidget(controlGroup);
    controlGroup->setLayout(controlLayout);
    controlGroup->setMinimumWidth(100);
    customPlot->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    controlLayout->addWidget(autoGetCheckBox);
    autoGetCheckBox->setText("Авто-получение по готовности");

    controlLayout->addWidget(autoSaveShotCheckBox);
    autoSaveShotCheckBox->setText("Авто-сохранение снимка");

    controlLayout->addWidget(packetSizeSpinbox);
    packetSizeSpinbox->setRange(50,1000);
    packetSizeSpinbox->setValue(100);
    connect(packetSizeSpinbox, QOverload<int>::of(&QSpinBox::valueChanged),
         [=](short i){
            setPacketSize(i);});

    controlLayout->addWidget(getButton);
    getButton->setText("Получить снимок");
    getButton->setEnabled(false);
    connect(getButton,&QPushButton::clicked,this, &MainWindow::manualGetShotButton);

    m_spacer= new QSpacerItem(1,1, QSizePolicy::Fixed, QSizePolicy::Expanding);
    controlLayout->addSpacerItem(m_spacer);

    controlLayout->addWidget(consoleEnable);
    consoleEnable->setText("Включить вывод в консоль");
    consoleEnable->setChecked(true);
    connect(consoleEnable,&QCheckBox::stateChanged,this,&MainWindow::consoleEnabledCheked);

    controlLayout->addWidget(shotsComboBox);
    connect(shotsComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
          [=](int index){
            selectShot(index);
    });

    controlLayout->addWidget(clearButton);
    clearButton->setText("Очистить список");
    connect(clearButton,&QPushButton::clicked,this, &MainWindow::on_clearButton);

    //Создание папки с логами, если ее нет.
    QDir dir(dirname);
    if (!dir.exists()) {
        QDir().mkdir(dirname);
    }
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
        m_timer->start(1000);
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
    nowShot.clear();
}

//Настройка разбиения данных на пакеты
void MainWindow::setPacketSize(short n){
    packetSize=n;
}
//Запрость у MCU пакет длины n
void MainWindow::getPacketFromMCU(short n)
{
    QByteArray data;
    char msb,lsb;
    msb=(n&0xFF00)>>8;
    lsb=n&0x00FF;
    data.append(REQUEST_POINTS);

    data.append(msb);
    data.append(lsb);
    m_transp->sendPacket(data);
}


//Запихиваем в очередь Х запросов в соответствии с разбивкой по пакетам, установленной в спинбоксе
void MainWindow::manualGetShotButton(){
    if(countAvaibleDots){
        m_console->putData("REQUEST_POINTS: ");
        m_timer->stop();
        statusBar->setDownloadBarRange(countAvaibleDots);
        while (countAvaibleDots>0){                                 //Отправляем запрос несоклько раз по packetSize точек.
            getPacketFromMCU(countAvaibleDots>packetSize?packetSize:countAvaibleDots);
            countAvaibleDots-=packetSize;
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
//Выбрать шот из списка
void MainWindow::selectShot(int index){
    if(!shots.isEmpty()){
        QByteArray shot = shots.at(index);                   //Взяли из листа нужный шот.
        addUserGraph(shot,shot.size());
    }
}
//Очистить список
void MainWindow::on_clearButton(){
    shots.clear();
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
            m_console->putData(" :RECIEVED ANSWER_MCU\n");
            emit statusUpdate(m_online);
        }
        else{
            statusBar->setMessageBar("Error: Wrong ASK_MCU ansver message!");
            m_console->putData("Error: Wrong ASK_MCU ansver message!\n");
        }
        m_console->putData("\n");
        break;


    case REQUEST_STATUS:                                                    //Пришло количество точек
        m_console->putData(" :RECIEVED ANSWER_STATUS\n");
        if (value != NO_DATA_READY) {
            dataReady = value;
            countAvaibleDots=value;
            if(autoGetCheckBox->isChecked()){                                //Если включен автозапрос данных
                manualGetShotButton();                                       //Запрашиваем шот
            }
        }
        else {
            dataReady = 0;
            m_console->putData("Warning: MCU has no data\n");
        }
        emit dataReadyUpdate(dataReady);
        m_console->putData("\n");
        break;


     case REQUEST_POINTS:                                                   //Пришли точки
        m_console->putData(" :RECIEVED ANSWER_POINTS\n");
        if (value == OK) {
            bytes.remove(0, 3);                                             //Удалили 3 байта (команду и значение)
            countRecievedDots+=bytes.count();
            statusBar->setDownloadBarValue(countRecievedDots);
            nowShot.append(bytes);                                          //Добавили пришедшие байты в шот

            if (countRecievedDots == statusBar->getDownloadBarRange()){     //Все точки пакета приняты
                countRecievedDots=0;
                m_timer->start();
                shots.append(nowShot);                                      //Добавили шот в лист
                shotsComboBox->addItem(QString::number(shots.count()));
                shotsComboBox->setCurrentIndex(shots.count()-1);

                //Если включено автосохранение
                if(autoSaveShotCheckBox->isChecked()){
                    if (dirname.isEmpty()){
                        QMessageBox::critical(nullptr,"Ошибка!","Директория для сохранения данных отсутствует!");
                    }
                    else{
                        filename=QString::number(shots.count());
                        file.setFileName(dirname + "/" + filename + ".txt");
                        if(file.open(QIODevice::WriteOnly) == true){
                            file.write(nowShot);
                            file.close();
                         }
                    }
                }
                nowShot.clear();
            }
        }
        else if(value == NO_DATA_READY){                              //Точки по какой-то причин не готовы. Это может случиться только если точки были запрошены вручную, игнорируя статус данных
            QMessageBox::critical(nullptr,"Ошибка!","Данные не готовы для получения!");
            m_console->putData("Warning: MCU has no data\n");
        }
        else{
            statusBar->setMessageBar("Error: Wrong REQUEST_POINTS ansver message!");
            m_console->putData("Warning: MCU has no data\n");
        }
        m_console->putData("\n");
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
void MainWindow::addUserGraph(QByteArray &buf, int len){
    QVector<double> x(len), y(len);
    for (int i=0; i<len; i++){
      x[i] = i;
      y[i] = (unsigned char)buf.at(i);
    }
    if(customPlot->graphCount()!=0)
        customPlot->clearGraphs();
    customPlot->addGraph();
    customPlot->graph()->setName(QString("New graph %1").arg(shots.count()));
    customPlot->graph()->setData(x, y);

    QPen graphPen;
    graphPen.setColor(QColor(std::rand()%245+10, std::rand()%245+10, std::rand()%245+10));
    customPlot->graph()->setPen(graphPen);

    customPlot->rescaleAxes();
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
void MainWindow::mousePress()
{
  // if an axis is selected, only allow the direction of that axis to be dragged
  // if no axis is selected, both directions may be dragged

  if (customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    customPlot->axisRect()->setRangeDrag(customPlot->xAxis->orientation());
  else if (customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
   customPlot->axisRect()->setRangeDrag(customPlot->yAxis->orientation());
  else
    customPlot->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}
void MainWindow::mouseWheel(){
  // if an axis is selected, only allow the direction of that axis to be zoomed
  // if no axis is selected, both directions may be zoomed

  if (customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    customPlot->axisRect()->setRangeZoom(customPlot->xAxis->orientation());
  else if (customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    customPlot->axisRect()->setRangeZoom(customPlot->yAxis->orientation());
  else
    customPlot->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
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
