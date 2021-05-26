#include "shotviewer.h"
#include "qcustomplot/qcustomplot.h"

ShotViewer::ShotViewer(QWidget *parent) : QWidget(parent)
{
    //Графики
    customPlot1 = new QCustomPlot();
    customPlot2 = new QCustomPlot();

    layoutV = new QVBoxLayout;

    setLayout(layoutV);
    layoutV->addWidget(customPlot1);
    layoutV->addWidget(customPlot2);


    customPlot1->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    customPlot2->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    //Настройка CustomPlot
    customPlot1->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables);
    customPlot1->axisRect()->setupFullAxesBox();
    customPlot2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables);
    customPlot2->axisRect()->setupFullAxesBox();
    customPlot1->plotLayout()->insertRow(0);
    QCPTextElement *title1 = new QCPTextElement(customPlot1, "Канал 1", QFont("sans", 14, QFont::Bold));
    customPlot1->plotLayout()->addElement(0, 0, title1);
    customPlot2->plotLayout()->insertRow(0);
    QCPTextElement *title2 = new QCPTextElement(customPlot2, "Канал 2", QFont("sans", 14, QFont::Bold));
    customPlot2->plotLayout()->addElement(0, 0, title2);
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

    //Изменение заголовка
    connect(title1, SIGNAL(clicked(QMouseEvent*)), this, SLOT(titleDoubleClick1(QMouseEvent*)));
    connect(title2, SIGNAL(clicked(QMouseEvent*)), this, SLOT(titleDoubleClick2(QMouseEvent*)));

    //Отображение текущей точки в Тултипе
    connect(customPlot1, SIGNAL(plottableClick(QCPAbstractPlottable*,int,QMouseEvent*)), this, SLOT(graphClicked1(QCPAbstractPlottable*,int)));
    connect(customPlot2, SIGNAL(plottableClick(QCPAbstractPlottable*,int,QMouseEvent*)), this, SLOT(graphClicked2(QCPAbstractPlottable*,int)));

    //Переключение между 1 и 2 графиками
    connect(customPlot1,SIGNAL(mouseDoubleClick(QMouseEvent*)),this,SLOT(graphDoubleClicked1()));
    connect(customPlot2,SIGNAL(mouseDoubleClick(QMouseEvent*)),this,SLOT(graphDoubleClicked2()));

    //запоминание текущей позиции мыши для выведения радом с ней тултипа
    connect(customPlot1, SIGNAL(mouseMove(QMouseEvent*)), this,SLOT(showPointToolTip(QMouseEvent*)));
    connect(customPlot2, SIGNAL(mouseMove(QMouseEvent*)), this,SLOT(showPointToolTip(QMouseEvent*)));
}

void ShotViewer::showGraphs(ShotViewer::viewerState state){
    switch (state){
    case CH1_CH2:
        customPlot1->show();
        customPlot2->hide();
        break;
    case CH3_CH4:
        customPlot2->show();
        customPlot1->hide();
        break;
    case Both:
        customPlot1->show();
        customPlot2->show();
        break;
    default:
        qDebug() << "in ShotViewer::showGraphs incorrect state!" ;
    }
}

void ShotViewer::clearGraphs(ShotViewer::viewerState state){
    switch (state){
    case CH1_CH2:
        mGraph1.clear();
        mGraph2.clear();
        customPlot1->clearGraphs();
        customPlot1->replot();
        break;
    case CH1_Only:
        if(customPlot1->hasPlottable(mGraph1))
            customPlot1->removeGraph(mGraph1);
        customPlot1->replot();
        mGraph1.clear();
        break;
    case CH2_Only:
        if(customPlot1->hasPlottable(mGraph2))
            customPlot1->removeGraph(mGraph2);
        customPlot1->replot();
        mGraph2.clear();
        break;
    case CH3_CH4:
        mGraph3.clear();
        mGraph4.clear();
        customPlot2->clearGraphs();
        customPlot2->replot();
        break;
    case CH3_Only:
        if(customPlot2->hasPlottable(mGraph3))
            customPlot2->removeGraph(mGraph3);
        customPlot2->replot();
        mGraph3.clear();
        break;
    case CH4_Only:
        if(customPlot2->hasPlottable(mGraph4))
            customPlot2->removeGraph(mGraph4);
        customPlot2->replot();
        mGraph4.clear();
        break;
    case Both:
        mGraph1.clear();
        mGraph2.clear();
        mGraph3.clear();
        mGraph4.clear();
        customPlot1->clearGraphs();
        customPlot1->replot();
        customPlot2->clearGraphs();
        customPlot2->replot();
        break;
    default:
        qDebug() << "in ShotViewer::clearGraphs incorrect state!" ;
    }
}

/**************************************************/
/*                  customPlot                    */
/**************************************************/
void ShotViewer::addUserGraph(QByteArray &buf, int len, int ch){
    QVector<double> x(len), y(len);
    QPen graphPen;
    QColor color;

    for (int i=0; i<len; i++){
      x[i] = i;
      y[i] = (unsigned char)buf.at(i);
    }
    if (ch==1 || ch==2){      
        if (ch==1){
            mGraph1=customPlot1->addGraph();
            mGraph1->setData(x, y);
            mGraph1->setName(QString("Канал 1. Нефильтрованный"));
            color =  Qt::black;
        }
        else{
            mGraph2=customPlot1->addGraph();
            mGraph2->setData(x, y);
            mGraph2->setName(QString("Канал 1. Фильтрованный"));
            color =  Qt::darkGray;
        }
        graphPen.setColor(color);
        customPlot1->graph()->setPen(graphPen);
        customPlot1->replot();
    }
    else if(ch==3 || ch==4){
        if (ch==3){
            mGraph3=customPlot2->addGraph();
            mGraph3->setData(x, y);
            mGraph3->setName(QString("Канал 2. Нефильтрованный"));
            color =  Qt::darkMagenta;
        }
        else{
            mGraph4=customPlot2->addGraph();
            mGraph4->setData(x, y);
            mGraph4->setName(QString("Канал 2. Фильтрованный"));
            color =  Qt::red;
        }
        graphPen.setColor(color);
        customPlot2->graph()->setPen(graphPen);
        customPlot2->replot();
    }
}

void ShotViewer::titleDoubleClick1(QMouseEvent* event)
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
      //customPlot1->replot();
    }
  }
}
void ShotViewer::titleDoubleClick2(QMouseEvent* event)
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
      //customPlot1->replot();
    }
  }
}
void ShotViewer::legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item){
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
void ShotViewer::selectionChanged1(){
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
void ShotViewer::selectionChanged2(){
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

void ShotViewer::mousePress1(){
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
void ShotViewer::mousePress2(){
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
void ShotViewer::mouseWheel1(){
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
void ShotViewer::mouseWheel2(){
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

void ShotViewer::graphDoubleClicked1(){
    if(!customPlot2->isHidden())
        showGraphs(CH1_CH2);
    else
        showGraphs(Both);
}
void ShotViewer::graphDoubleClicked2(){
    if(!customPlot1->isHidden())
        showGraphs(CH3_CH4);
    else
        showGraphs(Both);

}

void ShotViewer::graphClicked1(QCPAbstractPlottable *plottable, int dataIndex){
  // since we know we only have QCPGraphs in the plot, we can immediately access interface1D()
  // usually it's better to first check whether interface1D() returns non-zero, and only then use it.
  /*double dataValue = plottable->interface1D()->dataMainValue(dataIndex);
  QString message = QString("Clicked on graph '%1' at data point #%2 with value %3.").arg(plottable->name()).arg(dataIndex).arg(dataValue);
  //ui->statusBar->showMessage(message, 2500);
  mTag1->updatePosition(dataValue);
  mTag1->setText(QString::number(dataValue));*/

        int y = plottable->interface1D()->dataMainValue(dataIndex);
        QToolTip::showText(currentMousePosition,QString("%1 , %2").arg(dataIndex).arg(y));
}
void ShotViewer::graphClicked2(QCPAbstractPlottable *plottable, int dataIndex){
        int y = plottable->interface1D()->dataMainValue(dataIndex);
        QToolTip::showText(currentMousePosition,QString("%1 , %2").arg(dataIndex).arg(y));
}
void ShotViewer::autoScale(){
        customPlot1->rescaleAxes();
        customPlot1->yAxis->setRange(customPlot1->yAxis->range().lower-1,customPlot1->yAxis->range().upper+5);
        customPlot1->xAxis->setRange(customPlot1->xAxis->range().lower,customPlot1->xAxis->range().upper );
        customPlot1->replot();
        customPlot2->rescaleAxes();
        customPlot2->yAxis->setRange(customPlot2->yAxis->range().lower-1,customPlot2->yAxis->range().upper+5);
        customPlot2->xAxis->setRange(customPlot2->xAxis->range().lower,customPlot2->xAxis->range().upper );
        customPlot2->replot();
}

void ShotViewer::showPointToolTip(QMouseEvent *event){
     currentMousePosition = event->globalPos();
}

