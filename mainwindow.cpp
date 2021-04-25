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

    //Транспортный уровень SLIP протокола
    m_transp = new Transp(new Slip(serial));
    connect(m_transp, &Transp::answerReceive, this, &MainWindow::handlerTranspAnswerReceive);
    connect(m_transp, &Transp::transpError, this, &MainWindow::handlerTranspError);

    m_timer = new QTimer();
    connect(m_timer, &QTimer::timeout, this, &MainWindow::handlerTimer);

    //Статус бар
    statusBar = new StatusBar(ui->statusBar);
    connect(this, &MainWindow::statusUpdate, [this](bool online) { statusBar->setStatus(online); });
    connect(this, &MainWindow::dataReadyUpdate, [this](int ready) { statusBar->setDataReady(ready); });
    connect(this, &MainWindow::infoUpdate, [this](int info) { statusBar->setInfo(info); });
    connect(this, &MainWindow::downloadUpdate, [this](bool downloading) { statusBar->setDownloadGif(downloading); });

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
    layout = new QHBoxLayout;
    controlLayout = new QVBoxLayout;
    controlGroup = new QGroupBox;
    getButton = new QPushButton;
    autoGetCheckBox = new QCheckBox;
    autoSaveShotCheckBox = new QCheckBox;
    shotsComboBox = new QComboBox;
    clearButton = new QPushButton;

    centralWidget()->setLayout(layout);
    layout->addWidget(customPlot);
    layout->addWidget(controlGroup);
    controlGroup->setLayout(controlLayout);
    controlGroup->setMinimumWidth(100);
    customPlot->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    controlLayout->addWidget(autoGetCheckBox);
    autoGetCheckBox->setText("Авто-получение по готовности");

    controlLayout->addWidget(autoSaveShotCheckBox);
    autoSaveShotCheckBox->setText("Авто-сохранение снимка");

    controlLayout->addWidget(getButton);
    getButton->setText("Получить снимок");
    connect(getButton,&QPushButton::clicked,this, &MainWindow::manualGetShotButton);

    controlLayout->addWidget(shotsComboBox);
    connect(shotsComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
          [=](int index){
            selectShot(index);
    });

    controlLayout->addWidget(clearButton);
    clearButton->setText("Очистить список");
    connect(clearButton,&QPushButton::clicked,this, &MainWindow::on_clearButton);

    m_spacer= new QSpacerItem(1,1, QSizePolicy::Fixed, QSizePolicy::Expanding);
    controlLayout->addSpacerItem(m_spacer);

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
        ui->statusBar->showMessage("Подключено к " + settings_ptr->getName());
        ui->connect->setEnabled(false);
        ui->settings->setEnabled(false);
        ui->disconnect->setEnabled(true);
        m_timer->start(1000);
    }
    else{
         ui->statusBar->showMessage("Невозможно подключиться COM-порту");
    }
}
void MainWindow::on_disconnect_triggered(){
    if(serial->isOpen()){
        serial->close();
        ui->connect->setEnabled(true);
        ui->settings->setEnabled(true);
        ui->disconnect->setEnabled(false);
        ui->statusBar->showMessage("Отключено от " + settings_ptr->getName());
        m_timer->stop();
        m_online=false;
        emit statusUpdate(m_online);
        emit dataReadyUpdate(-1);
        emit downloadUpdate(false);
    }
    else {
        ui->statusBar->showMessage("Невозможно отключиться от COM-порта");
    }
}

//Получить шот
void MainWindow::manualGetShotButton(){
    QByteArray data;
    m_transp->setTimeoutValue(5000);                       //Устанавилваем таймаут повторной отправки на 5 секунд
    emit downloadUpdate(true);
    data.append(REQUEST_POINTS);
    m_transp->sendPacket(data);
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
    char cmd = bytes[0];
    int dataReady=-1;
    switch(cmd){
    case ASK_MCU:                                                   //Пришел ответ, mcu жив
        if (bytes[1] == OK) {
            m_online = true;
            m_timer->start(1000);
            emit downloadUpdate(false);
            emit statusUpdate(m_online);
        }
        else{
            qDebug() << "error: wrong answer message!";
        }
        break;
    case REQUEST_STATUS:                                            //Пришел ответ на запрос статуса
        m_timer->start(1000);
        if (bytes[1] == DATA_READY) {
            dataReady = 1;
            emit downloadUpdate(false);
            if(autoGetCheckBox->isChecked()){                       //Если включен автозапрос данных
                    m_transp->setTimeoutValue(5000);                //Устанавилваем таймаут повторной отправки на 5 секунд
                    QByteArray data;
                    data.append(REQUEST_POINTS);                    //Заправшиваем точки
                    emit downloadUpdate(true);
                    m_transp->sendPacket(data);
            }
        }
        else if (bytes[1] == NO_DATA_READY) {
            emit downloadUpdate(false);
            dataReady = 0;
        }
        else{
            qDebug() << "error: wrong answer message!";
        }
        emit dataReadyUpdate(dataReady);
        break;
     case REQUEST_POINTS:                                               //Пришли точки
        m_timer->start(1000);
        if (bytes[1] == OK) {
            m_transp->setTimeoutValue(500);                             //Переводим таймаут в обычный режим
            emit downloadUpdate(false);
            bytes.remove(0, 2);                                         //Удалили 2 байта (команду и значение)
            shots.append(bytes);                                        //Добавили шот в лист
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
                        file.write(bytes);
                        file.close();
                     }
                }
            }
        }
        else if(bytes[1] == NO_DATA_READY){                              //Точки по какой-то причин не готовы. Это может случиться только если точки были запрошены вручную, игнорируя статус данных
            emit downloadUpdate(false);
            QMessageBox::critical(nullptr,"Ошибка!","Данные не готовы для получения!");
        }
        else{
            qDebug() << "error: wrong answer message!";
            emit downloadUpdate(false);
            m_transp->setTimeoutValue(500);
        }
        break;
    }
}

//Обработка ошибок SLIP
void MainWindow::handlerTranspError() {
    m_online = false;
    emit on_disconnect_triggered();                         //Отключаемся
    emit statusUpdate(m_online);
}
// Обработчик таймаута опроса состояние MCU (1 сек)
void MainWindow::handlerTimer() {
    statusBar->setInfo(m_transp->getQueueCount());
    if (m_online) {      
        QByteArray data;
        data.append(REQUEST_STATUS);
        m_transp->sendPacket(data);
    }
    else {
        if (serial->isOpen()) {
            QByteArray data;
            data.append(ASK_MCU);
            m_transp->sendPacket(data);
        }
     }
    m_timer->stop();
    emit downloadUpdate(true);
}

/**************************************************/
/*                  customPlot                    */
/**************************************************/
void MainWindow::addUserGraph(QByteArray &buf, int len){
    QVector<double> x(len), y(len);
    for (int i=0; i<len; i++){
      x[i] = i;
      y[i] = buf.at(i);
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
