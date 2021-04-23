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
    settings_ptr = new SerialSettings();
    serial = new QSerialPort();
    customPlot = new QCustomPlot();

    m_transp = new Transp(new Slip(serial));
    connect(m_transp, &Transp::answerReceive, this, &MainWindow::handlerTranspAnswerReceive);
    connect(m_transp, &Transp::transpError, this, &MainWindow::handlerTranspError);

    m_timer = new QTimer();
    connect(m_timer, &QTimer::timeout, this, &MainWindow::handlerTimer);



    connect(this, &MainWindow::statusUpdate, [this](bool online) {
        statusBar->setStatus(online);
    });
    connect(this, &MainWindow::dataReadyUpdate, [this](int ready) {
        statusBar->setDataReady(ready);
    });
    connect(this, &MainWindow::manualUpdate, [this](bool manual) {
        statusBar->setManual(manual);
    });
    statusBar = new StatusBar(ui->statusBar);


    ui->disconnect->setEnabled(false);


    std::srand(QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0);
    //Интерфейс
    layout = new QHBoxLayout;
    controlLayout = new QVBoxLayout;
    controlGroup = new QGroupBox;
    getButton = new QPushButton;
    autoGetCheckBox = new QCheckBox;
    autoSaveShotCheckBox = new QCheckBox;
    shotsComboBox = new QComboBox;

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
    getButton->setText("Получить снимок, если готов");
    connect(getButton,&QPushButton::clicked,this, &MainWindow::manualGetShotButton);
    controlLayout->addWidget(shotsComboBox);
    //connect(shotsComboBox,&QComboBox::currentTextChanged,this, &MainWindow::selectShot);
    connect(shotsComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
          [=](int index){
            selectShot(index);
    });
    //Настройка CustomPlot
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables);
    //customPlot->xAxis->setRange(-8, 8);
    //customPlot->yAxis->setRange(-5, 5);
    customPlot->axisRect()->setupFullAxesBox();

    customPlot->plotLayout()->insertRow(0);
    QCPTextElement *title = new QCPTextElement(customPlot, "Видеосигнал", QFont("sans", 17, QFont::Bold));
    customPlot->plotLayout()->addElement(0, 0, title);

    customPlot->xAxis->setLabel("x Axis");
    customPlot->yAxis->setLabel("y Axis");
    customPlot->legend->setVisible(true);
    QFont legendFont = font();
    legendFont.setPointSize(10);
    customPlot->legend->setFont(legendFont);
    customPlot->legend->setSelectedFont(legendFont);
    customPlot->legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items


    // connect slot that ties some axis selections together (especially opposite axes):
    connect(customPlot, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
    // connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed:
    connect(customPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
    connect(customPlot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));

    // make bottom and left axes transfer their ranges to top and right axes:
    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

    // connect some interaction slots:
    connect(customPlot, SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)), this, SLOT(axisLabelDoubleClick(QCPAxis*,QCPAxis::SelectablePart)));
    connect(customPlot, SIGNAL(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)), this, SLOT(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*)));
    connect(title, SIGNAL(doubleClicked(QMouseEvent*)), this, SLOT(titleDoubleClick(QMouseEvent*)));

    // connect slot that shows a message in the status bar when a graph is clicked:
    connect(customPlot, SIGNAL(plottableClick(QCPAbstractPlottable*,int,QMouseEvent*)), this, SLOT(graphClicked(QCPAbstractPlottable*,int)));

    QDir dir(dirname);
    if (!dir.exists()) {
        QDir().mkdir(dirname);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_settings_triggered()
{
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

void MainWindow::on_disconnect_triggered()
{
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
    }
    else {
        ui->statusBar->showMessage("Невозможно отключиться от COM-порта");
    }
}

void MainWindow::manualGetShotButton()
{
    QByteArray data;
    data.append(REQUEST_POINTS);
    m_transp->sendPacket(data);
}


void MainWindow::selectShot(int index)
{
    QByteArray shot = shots.at(index);                   //Взяли из листа нужный шот.

    addUserGraph(shot,shot.size());
}

//Обработка входящих пакетов
void MainWindow::handlerTranspAnswerReceive(QByteArray &bytes) {
    char cmd = bytes[0];
    int dataReady=-1;
    switch(cmd) {
    case ASK_MCU:
        if (bytes[1] == OK) {
            m_online = true;
            emit statusUpdate(m_online);
        }
        break;
    case REQUEST_STATUS:
        if (bytes[1] == DATA_READY) {
            dataReady = 1;
            if(autoGetCheckBox->isChecked()){   //Если включен автозапрос данных
                QByteArray data;
                data.append(REQUEST_POINTS);
                m_transp->sendPacket(data);
            }
        }
        else if (bytes[1] == NO_DATA_READY) {
            dataReady = 0;
        }
        emit dataReadyUpdate(dataReady);
        break;
     case REQUEST_POINTS:
        if (bytes[1] == OK) {
            bytes.remove(0, 2);                     //Удалили 2 байта (команду и значение)
            shots.append(bytes);                    //Добавили шот в лист
            //addUserGraph(bytes,bytes.size());
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
        else if(bytes[1] == NO_DATA_READY){
            QMessageBox::critical(nullptr,"Ошибка!","Данные не готовы для получения!");
        }
        break;
    }
}

void MainWindow::handlerTranspError() {
    m_online = false;
    //serial->close();
    emit statusUpdate(m_online);
}

void MainWindow::handlerTimer() {
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
}

//customPlot
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

void MainWindow::legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item)
{
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

void MainWindow::selectionChanged()
{
  /*
   normally, axis base line, axis tick labels and axis labels are selectable separately, but we want
   the user only to be able to select the axis as a whole, so we tie the selected states of the tick labels
   and the axis base line together. However, the axis label shall be selectable individually.

   The selection state of the left and right axes shall be synchronized as well as the state of the
   bottom and top axes.

   Further, we want to synchronize the selection of the graphs with the selection state of the respective
   legend item belonging to that graph. So the user can select a graph by either clicking on the graph itself
   or on its legend item.
  */

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
  for (int i=0; i<customPlot->graphCount(); ++i)
  {
    QCPGraph *graph = customPlot->graph(i);
    QCPPlottableLegendItem *item = customPlot->legend->itemWithPlottable(graph);
    if (item->selected() || graph->selected())
    {
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

void MainWindow::mouseWheel()
{
  // if an axis is selected, only allow the direction of that axis to be zoomed
  // if no axis is selected, both directions may be zoomed

  if (customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    customPlot->axisRect()->setRangeZoom(customPlot->xAxis->orientation());
  else if (customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    customPlot->axisRect()->setRangeZoom(customPlot->yAxis->orientation());
  else
    customPlot->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

void MainWindow::addUserGraph(QByteArray &buf, int len){

    int n = len; // number of points in graph

    QVector<double> x(n), y(n);

    for (int i=0; i<len; i++)
    {
      x[i] = i;
      y[i] = buf.at(i);
    }
    if(customPlot->graphCount()!=0)
        customPlot->clearGraphs();
    customPlot->addGraph();
    customPlot->graph()->setName(QString("New graph %1").arg(shots.count()));
    customPlot->graph()->setData(x, y);
    /*
    customPlot->graph()->setLineStyle((QCPGraph::LineStyle)(std::rand()%5+1));
    if (std::rand()%100 > 50)
      customPlot->graph()->setScatterStyle(QCPScatterStyle((QCPScatterStyle::ScatterShape)(std::rand()%14+1)));
    QPen graphPen;
    graphPen.setColor(QColor(std::rand()%245+10, std::rand()%245+10, std::rand()%245+10));
    graphPen.setWidthF(std::rand()/(double)RAND_MAX*2+1);
    customPlot->graph()->setPen(graphPen);
    */
    customPlot->rescaleAxes();
    customPlot->replot();

}

void MainWindow::moveLegend()
{
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

void MainWindow::graphClicked(QCPAbstractPlottable *plottable, int dataIndex)
{
  // since we know we only have QCPGraphs in the plot, we can immediately access interface1D()
  // usually it's better to first check whether interface1D() returns non-zero, and only then use it.
  double dataValue = plottable->interface1D()->dataMainValue(dataIndex);
  QString message = QString("Clicked on graph '%1' at data point #%2 with value %3.").arg(plottable->name()).arg(dataIndex).arg(dataValue);
  ui->statusBar->showMessage(message, 2500);
}
